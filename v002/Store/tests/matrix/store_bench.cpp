// store_bench — one generic, fully-parameterized Store throughput benchmark.
//
// The instrument, not the answer: instead of hardcoding "the fast settings"
// (which are machine-specific), this lets you sweep a parameter on YOUR hardware
// and read the peak off the curve. Mirrors the project's ethos — Setup senses the
// toolchain; store_bench senses the performance envelope.
//
// Single config:
//   store_bench --threads 50 --events-per-thread 20000 --runs 30 --persist none
// Sweep a parameter (find the per-machine peak):
//   store_bench --sweep events=100..2000:100 --threads 16 --runs 20 --persist none
//   store_bench --sweep batch=1000..32000:1000 --persist sql --runs 8
//   store_bench --sweep threads=1..32:1 --events-per-thread 50000 --runs 10
//
// Axes: events (events-per-thread), threads, batch (durable batch size).
// Every config is a measurement SET: --runs N runs -> {median, high, low, stddev}
// via bench_stats.hpp. Median is the headline (a tight median over many runs reads
// as "consistently fast"); persist=none is the in-memory ceiling.
//
// Metric counts are compile-time (ts_store_config) — fixed here to the pure
// payload+flags shape; metric variants would be separate compiled binaries.

#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "test_common.hpp"
#include "bench_stats.hpp"

using namespace jac313::Store::v002;
using namespace std::chrono;

using LogConfig = ts_store_config<true, 6, 20, 43, 0, 0, false>;
using LogxStore = ts_store<LogConfig>;

namespace {

// ---- parameters (runtime) ----
struct Params {
    std::size_t threads = 16;
    std::size_t events_per_thread = 20'000;
    std::size_t runs = 20;
    std::size_t batch = 10'000;          // durable double-buffer batch
    std::string persist = "none";        // none | binary | jtext | sql
    std::string base_name = "bench";
    std::uint64_t flags = 0;             // per-event user flag mask
};

struct Sweep {
    std::string axis;                    // "events" | "threads" | "batch"
    std::size_t lo = 0, hi = 0, step = 0;
};

std::uint64_t default_flags() {
    std::uint64_t f = 0;
    f = set_user_flag(f, StoreFlags::UserFlag::LogConsole);
    f = set_user_flag(f, StoreFlags::UserFlag::KeeperRecord);
    f = set_user_flag(f, StoreFlags::UserFlag::HotCacheHint);
    return f;
}

std::uint64_t parse_flags(const std::string& csv) {
    std::uint64_t f = 0;
    std::string tok;
    auto apply = [&](const std::string& name) {
        if (name == "console")  f = set_user_flag(f, StoreFlags::UserFlag::LogConsole);
        else if (name == "keeper")   f = set_user_flag(f, StoreFlags::UserFlag::KeeperRecord);
        else if (name == "database") f = set_user_flag(f, StoreFlags::UserFlag::DatabaseEntry);
        else if (name == "hotcache") f = set_user_flag(f, StoreFlags::UserFlag::HotCacheHint);
    };
    for (char c : csv) {
        if (c == ',') { apply(tok); tok.clear(); }
        else tok.push_back(c);
    }
    if (!tok.empty()) apply(tok);
    return f;
}

// Set the first N of the 7 user flags (bits 0-6) — for the flag-overhead sweep
// (0 / 2 / 4 / 6). Non-durable has no routing sink, so these are pure bit-set cost:
// exactly the flag overhead we want to isolate.
std::uint64_t flags_by_count(std::size_t n) {
    std::uint64_t f = 0;
    for (std::size_t i = 0; i < n && i < 7; ++i) {
        f = set_user_flag(f, static_cast<StoreFlags::UserFlag>(i));
    }
    return f;
}

std::optional<Sweep> parse_sweep(const std::string& spec) {
    // AXIS=LO..HI:STEP
    const auto eq = spec.find('=');
    const auto dots = spec.find("..");
    const auto colon = spec.find(':', dots == std::string::npos ? 0 : dots);
    if (eq == std::string::npos || dots == std::string::npos || colon == std::string::npos)
        return std::nullopt;
    Sweep s;
    s.axis = spec.substr(0, eq);
    try {
        s.lo = std::stoull(spec.substr(eq + 1, dots - eq - 1));
        s.hi = std::stoull(spec.substr(dots + 2, colon - dots - 2));
        s.step = std::stoull(spec.substr(colon + 1));
    } catch (...) { return std::nullopt; }
    if (s.step == 0 || s.hi < s.lo) return std::nullopt;
    return s;
}

std::string arg_value(const std::string& a, const char* key) {
    const std::string pfx = std::string(key) + "=";
    if (a.rfind(pfx, 0) == 0) return a.substr(pfx.size());
    return {};
}

// minimal, self-contained host tag for the "peak on this machine" line
std::string host_tag() {
    std::string cpu = "cpu?";
    int cores = 0;
    std::ifstream in("/proc/cpuinfo");
    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("model name", 0) == 0) {
            const auto c = line.find(':');
            if (c != std::string::npos) cpu = line.substr(c + 2);
        } else if (line.rfind("processor", 0) == 0) {
            ++cores;
        }
    }
    return cpu + ", " + std::to_string(cores) + "c";
}

std::unique_ptr<IEventSink> make_sink(const Params& p) {
    const std::size_t im = LogConfig::the_IntMetrics;
    const std::size_t dm = LogConfig::the_DblMetrics;
    if (p.persist == "binary") return std::make_unique<BinaryEventSink>(p.base_name, im, dm, PersistMode::All);
    if (p.persist == "jtext")  return std::make_unique<JTextEventSink>(p.base_name, im, dm, PersistMode::All);
#ifdef JAC313_STORE_HAS_SQL_PERSIST
    if (p.persist == "sql")    return std::make_unique<SqlEventSink>(p.base_name, im, dm, PersistMode::All, false);
#endif
    return nullptr;
}

// One timed run of the full workload. Attaches persistence for THIS run when durable.
std::optional<std::size_t> run_one(const Params& p, bool verify) {
    LogxStore store(p.threads, p.events_per_thread);

    if (p.persist != "none") {
        if (auto sink = make_sink(p)) {
            store.attach_persistence(std::make_unique<DoubleBufferedWriter>(std::move(sink), p.batch));
        } else {
            std::cerr << "ERROR: persist='" << p.persist << "' unavailable (SQL needs JAC313_STORE_HAS_SQL_PERSIST)\n";
            return std::nullopt;
        }
    }

    std::array<std::string, 8> pre_payloads;
    for (std::size_t i = 0; i < 8; ++i) pre_payloads[i] = std::string(LogxStore::test_messages[i]);
    std::array<std::string, 5> pre_cats;
    for (std::size_t i = 0; i < 5; ++i) pre_cats[i] = std::string(LogxStore::categories[i]);

    const auto start = high_resolution_clock::now();
    std::vector<std::thread> ths;
    ths.reserve(p.threads);
    for (std::size_t t = 0; t < p.threads; ++t) {
        ths.emplace_back([&, t]() {
            std::array<std::int64_t, LogConfig::the_IntMetrics> ints{};
            std::array<double, LogConfig::the_DblMetrics> dbls{};
            for (std::size_t i = 0; i < p.events_per_thread; ++i) {
                std::string payload = pre_payloads[i % pre_payloads.size()];
                std::string cat = pre_cats[t % pre_cats.size()];
                std::uint64_t f = set_severity(p.flags, static_cast<StoreFlags::Severity>(i % 8));
                auto [ok, id] = store.save_event(t, i, std::move(payload), f, std::move(cat), true, ints, dbls);
                if (!ok) { std::cerr << "CLAIM FAILED t=" << t << " i=" << i << "\n"; std::abort(); }
            }
        });
    }
    for (auto& th : ths) th.join();
    // Durable honesty: drain + flush the persistence writer (and join its worker thread)
    // INSIDE the timed region, so the durable rate counts bytes that actually reached the
    // sink — not just bytes buffered. No-op when non-durable (no writer attached).
    store.finalize_persistence();
    const auto end = high_resolution_clock::now();

    // Verification is correctness, not throughput — kept OUT of the timed region and run
    // only when asked. The benchmark verifies the LAST run as a sanity net (O(N), so we
    // don't pay it every run); full correctness lives in the small correctness tests.
    if (verify && !store.verify_level01()) {
        std::cerr << "STRUCTURAL VERIFICATION FAILED\n";
        store.diagnose_failures();
        return std::nullopt;
    }
    return static_cast<std::size_t>(duration_cast<microseconds>(end - start).count());
}

// One measurement set: RUNS timed runs -> durations. Returns empty on any failure.
std::vector<std::size_t> measure_set(const Params& p) {
    std::vector<std::size_t> durations;
    durations.reserve(p.runs);
    for (std::size_t r = 0; r < p.runs; ++r) {
        auto us = run_one(p, /*verify=*/ (r + 1 == p.runs));   // verify the last run only
        if (!us) return {};
        durations.push_back(*us);
    }
    return durations;
}

void apply_axis(Params& p, const std::string& axis, std::size_t v) {
    if (axis == "events")       p.events_per_thread = v;
    else if (axis == "threads") p.threads = v;
    else if (axis == "batch")   p.batch = v;
}

} // namespace

int main(int argc, char** argv) {
    Params p;
    p.flags = default_flags();
    std::optional<Sweep> sweep;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto next = [&]() -> std::string { return (i + 1 < argc) ? argv[++i] : std::string{}; };
        if (a == "--threads") p.threads = std::stoull(next());
        else if (a == "--events-per-thread") p.events_per_thread = std::stoull(next());
        else if (a == "--runs") p.runs = std::stoull(next());
        else if (a == "--batch") p.batch = std::stoull(next());
        else if (a == "--persist") p.persist = next();
        else if (a == "--base-name") p.base_name = next();
        else if (a == "--flags") p.flags = parse_flags(next());
        else if (a == "--flag-count") p.flags = flags_by_count(std::stoull(next()));
        else if (a == "--sweep") { sweep = parse_sweep(next()); if (!sweep) { std::cerr << "bad --sweep (want AXIS=LO..HI:STEP)\n"; return 2; } }
        else if (!arg_value(a, "--sweep").empty()) { sweep = parse_sweep(arg_value(a, "--sweep")); }
    }

    if (!sweep) {
        // single config — one measurement set
        std::cout << "store_bench: threads=" << p.threads << " events/thread=" << p.events_per_thread
                  << " runs=" << p.runs << " persist=" << p.persist << " batch=" << p.batch << "\n";
        auto durations = measure_set(p);
        if (durations.empty()) { std::cerr << "benchmark failed\n"; return 1; }
        emit_bench_stats(durations, static_cast<std::uint64_t>(p.threads * p.events_per_thread));
        return 0;
    }

    // sweep — find the per-machine peak along the chosen axis
    std::cout << "store_bench sweep: axis=" << sweep->axis
              << " range=" << sweep->lo << ".." << sweep->hi << ":" << sweep->step
              << "  (threads=" << p.threads << " events/thread=" << p.events_per_thread
              << " runs=" << p.runs << " persist=" << p.persist << " batch=" << p.batch << ")\n";
    std::cout << "----------------------------------------------------------------\n";
    std::uint64_t best_median = 0, best_high = 0, best_stddev = 0;
    std::size_t best_val = 0;
    for (std::size_t v = sweep->lo; v <= sweep->hi; v += sweep->step) {
        Params pv = p;
        apply_axis(pv, sweep->axis, v);
        auto durations = measure_set(pv);
        if (durations.empty()) { std::cerr << sweep->axis << "=" << v << "  FAILED\n"; return 1; }
        const BenchSummary s = compute_bench_summary(durations, static_cast<std::uint64_t>(pv.threads * pv.events_per_thread));
        std::cout << "  " << sweep->axis << "=" << bench_group_thousands(v)
                  << "  -> median " << bench_group_thousands(s.median) << " ops/sec"
                  << "  (high " << bench_group_thousands(s.high)
                  << ", σ " << bench_group_thousands(s.stddev) << ", N=" << s.runs << ")\n";
        if (s.median > best_median) { best_median = s.median; best_high = s.high; best_stddev = s.stddev; best_val = v; }
    }
    std::cout << "----------------------------------------------------------------\n";
    std::cout << "PEAK (this machine): " << sweep->axis << "=" << bench_group_thousands(best_val)
              << "  -> median " << bench_group_thousands(best_median) << " ops/sec"
              << "  (high " << bench_group_thousands(best_high)
              << ", σ " << bench_group_thousands(best_stddev) << ")"
              << "   [" << host_tag() << ", persist=" << p.persist << "]\n";
    // machine-parseable peak line for the runner / DB
    std::cout << "BENCHSWEEP axis=" << sweep->axis << " best_val=" << best_val
              << " median=" << best_median << " high=" << best_high << " stddev=" << best_stddev
              << " persist=" << p.persist << "\n";
    return 0;
}
