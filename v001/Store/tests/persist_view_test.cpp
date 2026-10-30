// Persistence reads the store's rows in place (the sliding view): one worker thread hands the
// sink every saved row, exactly once, in id order; a failing sink latches failed() without
// blocking anyone; max_persist_rows caps what the sink sees; clear() after events went to
// persistence is refused (std::logic_error), while clear() right after attaching restarts at id 0.

#include <jac313/Store/v001.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>
#include <span>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace S = jac313::Store::v001;
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
    size_t              calls = 0;
    bool                flushed = false;
    bool                finalized = false;
};

class CountingSink : public S::IEventSink {
public:
    CountingSink(Seen& seen, size_t throw_on_call = SIZE_MAX) : seen_(seen), throw_on_call_(throw_on_call) {}
    void write_batch(std::span<const S::PersistedEvent> batch) override {
        std::lock_guard lock(seen_.m);
        if (seen_.calls++ == throw_on_call_) throw std::runtime_error("persist_view_test: injected sink failure");
        for (const auto& e : batch) {
            seen_.ids.push_back(e.event_id);
            if (e.payload != "p" + std::to_string(e.per_thread_event_id))
                throw std::runtime_error("persist_view_test: payload does not match its row");
        }
    }
    void flush() override { std::lock_guard lock(seen_.m); seen_.flushed = true; }
    void finalize() override { std::lock_guard lock(seen_.m); seen_.finalized = true; }
private:
    Seen&  seen_;
    size_t throw_on_call_;
};

void save(Store& store, size_t thread, size_t i) {
    (void)store.save_event(thread, i, Config::ValueT("p" + std::to_string(i)));
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

// True iff clear() refused (std::logic_error) after events went to persistence.
bool clear_after_persisting_refused(bool finalize_first) {
    Seen seen;
    Store store(1, 10);
    store.attach_persistence(std::make_unique<CountingSink>(seen));
    for (size_t i = 0; i < 10; ++i) save(store, 0, i);
    if (finalize_first) store.finalize_persistence();
    try {
        store.clear();
    } catch (const std::logic_error&) {
        return true;
    }
    return false;
}

} // namespace

int main() {
    many_producers();
    failing_sink();
    capped();
    clear_right_after_attach();
    check(clear_after_persisting_refused(true), "clear() after persisted events was NOT refused");
    check(clear_after_persisting_refused(false), "clear() after saved (not yet persisted) events was NOT refused");
    if (failures == 0)
        std::cout << "jac313::Store::v001 persistence view — rows read in place, once each, in order\n";
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
