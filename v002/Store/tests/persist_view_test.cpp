// Persistence reads the store's rows in place (the sliding view): each attached sink has its own
// worker thread, which hands it every saved row, exactly once, in id order; several sinks at once
// each keep the events their PersistMode takes; a failing sink latches only its own worker's
// failed() without blocking anyone; max_persist_rows caps what the sinks see; clear() after
// events went to persistence is refused (a contract), while clear() right after attaching
// restarts at id 0.

#include <jac313/Store/v002.hpp>
#include <jac313/Store/v002/headers/persistence/PersistCommon.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>
#include <span>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace S = jac313::Store::v002;
using Config = S::ts_store_config<true, 6, 20, 43, 2, 2, false>;
using Store  = S::ts_store<Config>;

namespace {

int failures = 0;

void check(bool ok, const std::string& what) {
    if (!ok) {
        std::cerr << "FAIL: " << what << "\n";
        ++failures;
    }
}

// What the sink saw. The sink belongs to the store; the test keeps this.
struct Seen {
    std::mutex          m;          // only the worker writes, the test reads after finalize
    std::vector<size_t> ids;
    std::vector<uint64_t> flags;   // flags[k] belongs to ids[k]
    size_t              wrong_flags = 0;   // events whose flags are not the ones save_flags set
    size_t              calls = 0;
    bool                flushed = false;
    bool                finalized = false;
};

constexpr uint64_t kKeeper   = 1ULL << 1;   // UserFlag::KeeperRecord
constexpr uint64_t kDatabase = 1ULL << 2;   // UserFlag::DatabaseEntry

// The routing flags the multi-sink tests give per-thread event i: a known subset of each.
uint64_t routing_flags(size_t i) {
    return (i % 3 == 0 ? kKeeper : 0) | (i % 5 == 0 ? kDatabase : 0);
}

// Keeps what its PersistMode takes, like the real sinks: KeeperOnly = bit 1, DatabaseOnly = bit 2.
class CountingSink : public S::IEventSink {
public:
    CountingSink(Seen& seen, size_t throw_on_call = SIZE_MAX, S::PersistMode mode = S::PersistMode::All,
                 bool check_flags = false)
        : seen_(seen), throw_on_call_(throw_on_call), mode_(mode), check_flags_(check_flags) {}
    void write_batch(std::span<const S::PersistedEvent> batch) override {
        std::lock_guard lock(seen_.m);
        if (seen_.calls++ == throw_on_call_) throw std::runtime_error("persist_view_test: injected sink failure");
        for (const auto& e : batch) {
            if (mode_ == S::PersistMode::KeeperOnly && (e.flags & kKeeper) == 0) continue;
            if (mode_ == S::PersistMode::DatabaseOnly && (e.flags & kDatabase) == 0) continue;
            if (check_flags_ && (e.flags & (kKeeper | kDatabase)) != routing_flags(e.per_thread_event_id))
                ++seen_.wrong_flags;
            seen_.ids.push_back(e.event_id);
            seen_.flags.push_back(e.flags);
            if (e.payload != "p" + std::to_string(e.per_thread_event_id))
                throw std::runtime_error("persist_view_test: payload does not match its row");
        }
    }
    void flush() override { std::lock_guard lock(seen_.m); seen_.flushed = true; }
    void finalize() override { std::lock_guard lock(seen_.m); seen_.finalized = true; }
    std::string_view name() const override {
        switch (mode_) {
            case S::PersistMode::KeeperOnly:   return "CountingSink(KeeperOnly)";
            case S::PersistMode::DatabaseOnly: return "CountingSink(DatabaseOnly)";
            case S::PersistMode::All:          break;
        }
        return "CountingSink(All)";
    }
private:
    Seen&          seen_;
    size_t         throw_on_call_;
    S::PersistMode mode_;
    bool           check_flags_;
};

void save(Store& store, size_t thread, size_t i) {
    (void)store.save_event(thread, i, Config::ValueT("p" + std::to_string(i)));
}

void save_flags(Store& store, size_t thread, size_t i) {
    (void)store.save_event(thread, i, Config::ValueT("p" + std::to_string(i)), routing_flags(i));
}

bool ids_are_0_to(const std::vector<size_t>& ids, size_t n) {
    if (ids.size() != n) return false;
    for (size_t i = 0; i < n; ++i)
        if (ids[i] != i) return false;
    return true;
}

// N producers x M events: every id in [0, N*M) reaches the sink once, in nondecreasing order.
void many_producers() {
    constexpr size_t N = 8, M = 20'000;
    Seen seen;
    {
        Store store(N, M);
        store.attach_persistence(std::make_unique<CountingSink>(seen), 1'000);
        std::vector<std::thread> producers;
        for (size_t t = 0; t < N; ++t)
            producers.emplace_back([&store, t] { for (size_t i = 0; i < M; ++i) save(store, t, i); });
        store.flush_persistence();   // while the producers run: writes what is ready, never blocks them
        for (auto& p : producers) p.join();
        store.finalize_persistence();
        check(!store.persistence_failed(), "many producers: no sink failure");
        check(store.persisted_rows() == N * M, "many producers: persisted_rows() == N*M");
    }
    check(ids_are_0_to(seen.ids, N * M), "many producers: ids [0, N*M) once each, in order");
    check(seen.flushed && seen.finalized, "many producers: sink flushed and finalized");
}

// A sink that throws once: failed() latches, finalize() still returns, nothing more is written.
void failing_sink() {
    Seen seen;
    Store store(1, 1'000);
    store.attach_persistence(std::make_unique<CountingSink>(seen, 0), 10);
    for (size_t i = 0; i < 1'000; ++i) save(store, 0, i);
    store.finalize_persistence();
    check(store.persistence_failed(), "failing sink: persistence_failed()");
    check(seen.ids.empty() && seen.calls == 1, "failing sink: the sink is not called again");
    check(!seen.finalized, "failing sink: a failed sink is not finalized");
}

// max_persist_rows caps what the sink sees; the store still takes every event.
void capped() {
    Seen seen;
    {
        Store store(2, 100);
        store.set_max_persist_rows(50);
        store.attach_persistence(std::make_unique<CountingSink>(seen), 7);
        for (size_t i = 0; i < 200; ++i) save(store, 0, i % 100);
        store.finalize_persistence();
        check(store.expected_persisted_rows() == 50, "capped: expected_persisted_rows() == 50");
    }
    check(ids_are_0_to(seen.ids, 50), "capped: ids [0, 50) only");
}

// The ids of `all` (an All sink's record) whose flags have `bit`: what a filtering sink must hold.
std::vector<size_t> subset(const Seen& all, uint64_t bit) {
    std::vector<size_t> out;
    for (size_t k = 0; k < all.ids.size(); ++k)
        if (all.flags[k] & bit) out.push_back(all.ids[k]);
    return out;
}

// Three sinks at once (All, KeeperOnly, DatabaseOnly), one worker each, over N producers x M
// events with the routing flags on a known subset: each sink saw exactly its subset, each id once,
// in increasing order; every worker reports its own count and a drain time.
void three_sinks() {
    constexpr size_t N = 8, M = 6'000;
    size_t keeper_per_thread = 0, database_per_thread = 0;
    for (size_t i = 0; i < M; ++i) {
        keeper_per_thread   += (routing_flags(i) & kKeeper)   ? 1 : 0;
        database_per_thread += (routing_flags(i) & kDatabase) ? 1 : 0;
    }
    std::array<Seen, 3> seen;
    {
        Store store(N, M);
        const size_t all = store.attach_persistence(
            std::make_unique<CountingSink>(seen[0], SIZE_MAX, S::PersistMode::All, true), 500);
        const size_t keeper = store.attach_persistence(
            std::make_unique<CountingSink>(seen[1], SIZE_MAX, S::PersistMode::KeeperOnly, true), 1'000);
        const size_t database = store.attach_persistence(
            std::make_unique<CountingSink>(seen[2], SIZE_MAX, S::PersistMode::DatabaseOnly, true), 3'000);
        check(all == 0 && keeper == 1 && database == 2 && store.persistence_count() == 3,
              "three sinks: indices 0, 1, 2");
        std::vector<std::thread> producers;
        for (size_t t = 0; t < N; ++t)
            producers.emplace_back([&store, t] { for (size_t i = 0; i < M; ++i) save_flags(store, t, i); });
        store.flush_persistence();
        for (auto& p : producers) p.join();
        store.finalize_persistence();
        check(!store.persistence_failed(), "three sinks: no sink failure");
        for (size_t w = 0; w < 3; ++w) {
            check(!store.persistence_failed(w), "three sinks: worker " + std::to_string(w) + " not failed");
            check(store.persisted_rows(w) == N * M, "three sinks: worker " + std::to_string(w) + " walked N*M rows");
            const uint64_t us = store.drain_time_us(w);
            check(us > 0 && us < 600'000'000ULL, "three sinks: worker " + std::to_string(w) + " drain time "
                  + std::to_string(us) + " us is non-zero and finite");
            std::cout << "  " << store.persistence_sink_name(w) << ": " << store.persisted_rows(w)
                      << " rows walked, drained in " << us << " us\n";
        }
        check(store.persisted_rows() == N * M, "three sinks: persisted_rows() == N*M");
        bool threw = false;
        try { (void)store.drain_time_us(3); } catch (const std::out_of_range&) { threw = true; }
        check(threw, "three sinks: index 3 is out of range");
    }
    for (const auto& s : seen) {
        check(s.wrong_flags == 0, "three sinks: every event carries the flags it was saved with");
        check(s.flushed && s.finalized, "three sinks: every sink flushed and finalized");
        check(std::ranges::is_sorted(s.ids) && std::ranges::adjacent_find(s.ids) == s.ids.end(),
              "three sinks: ids increasing, each once");
    }
    check(ids_are_0_to(seen[0].ids, N * M), "three sinks: All saw ids [0, N*M)");
    check(seen[1].ids.size() == N * keeper_per_thread && seen[1].ids == subset(seen[0], kKeeper),
          "three sinks: KeeperOnly saw exactly the KeeperRecord events");
    check(seen[2].ids.size() == N * database_per_thread && seen[2].ids == subset(seen[0], kDatabase),
          "three sinks: DatabaseOnly saw exactly the DatabaseEntry events");
}

// Three sinks, the KeeperOnly one throws on its first batch: only its worker is failed, the other
// two complete.
void one_of_three_fails() {
    constexpr size_t N = 4, M = 3'000;
    std::array<Seen, 3> seen;
    {
        Store store(N, M);
        store.attach_persistence(std::make_unique<CountingSink>(seen[0], SIZE_MAX, S::PersistMode::All), 256);
        store.attach_persistence(std::make_unique<CountingSink>(seen[1], 0, S::PersistMode::KeeperOnly), 256);
        store.attach_persistence(std::make_unique<CountingSink>(seen[2], SIZE_MAX, S::PersistMode::DatabaseOnly), 256);
        std::vector<std::thread> producers;
        for (size_t t = 0; t < N; ++t)
            producers.emplace_back([&store, t] { for (size_t i = 0; i < M; ++i) save_flags(store, t, i); });
        for (auto& p : producers) p.join();
        store.finalize_persistence();
        check(store.persistence_failed(), "one of three fails: persistence_failed() is true");
        check(!store.persistence_failed(0) && store.persistence_failed(1) && !store.persistence_failed(2),
              "one of three fails: only worker 1 is failed");
        check(store.persisted_rows(0) == N * M && store.persisted_rows(2) == N * M,
              "one of three fails: the other two walked every row");
        for (size_t w = 0; w < 3; ++w)
            check(store.drain_time_us(w) > 0, "one of three fails: worker " + std::to_string(w) + " has a drain time");
    }
    check(ids_are_0_to(seen[0].ids, N * M) && seen[0].finalized, "one of three fails: All completed");
    check(seen[2].ids == subset(seen[0], kDatabase) && seen[2].finalized, "one of three fails: DatabaseOnly completed");
    check(seen[1].ids.empty() && seen[1].calls == 1 && !seen[1].finalized,
          "one of three fails: the failed sink is not called again");
}

// The matrix tests' pattern: earlier runs without persistence, attach, clear(), the last run.
void clear_right_after_attach() {
    Seen seen;
    {
        Store store(1, 100);
        for (size_t i = 0; i < 100; ++i) save(store, 0, i);   // not persisted: saved before attach
        store.attach_persistence(std::make_unique<CountingSink>(seen), 16);
        store.clear();
        for (size_t i = 0; i < 40; ++i) save(store, 0, i);
        store.finalize_persistence();
    }
    check(ids_are_0_to(seen.ids, 40), "clear after attach: ids [0, 40) once each");
}

// Run `fn` in a forked child; true iff the child did not exit cleanly (a contract fired).
template <class Fn>
bool child_terminates(Fn fn) {
    std::fflush(nullptr);
    const pid_t pid = fork();
    if (pid < 0) { std::perror("fork"); std::_Exit(2); }
    if (pid == 0) {
        fn();
        std::_Exit(0);
    }
    int status = 0;
    while (waitpid(pid, &status, 0) < 0) { /* retry on EINTR */ }
    return !(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

void clear_after_persisting(bool finalize_first) {
    Seen seen;
    Store store(1, 10);
    store.attach_persistence(std::make_unique<CountingSink>(seen));
    for (size_t i = 0; i < 10; ++i) save(store, 0, i);
    if (finalize_first) store.finalize_persistence();
    store.clear();
}

} // namespace

int main() {
    many_producers();
    failing_sink();
    capped();
    clear_right_after_attach();
    three_sinks();
    one_of_three_fails();
#if defined(JAC313_PV_CONTRACTS_TERMINATE)
    check(child_terminates([] { clear_after_persisting(true); }),
          "clear() after persisted events did NOT terminate");
    check(child_terminates([] { clear_after_persisting(false); }),
          "clear() after saved (not yet persisted) events did NOT terminate");
#endif
    if (failures == 0)
        std::cout << "jac313::Store::v002 persistence view — rows read in place, once each, in order\n";
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
