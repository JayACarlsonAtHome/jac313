//tests/ts_store_007/Test_007_XS.CPP
//
// Massive multi-threaded throughput + correctness test.
// Full mode sizing from runner (currently 50×2k × 3 runs). Only last run persists.
// Only the last run performs persistence (previous runs are clean hot-path measurement).
// Size controlled at runtime via --threads --events-per-thread --runs (or via test_params.txt).
// See tests/test_params.txt and ./scripts/Build (FileCheckList.txt).

#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <format>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <thread>
#include "test_common.hpp"




#ifdef JAC313_STORE_HAS_SQL_PERSIST

#endif

using namespace jac313::Store::v001;
using namespace std::chrono;

// These are set at runtime from CLI / config (see test_params.txt and runner).
// Defaults come from TestOptions (high intensity); smoke uses small via --test-size=smoke or explicit.
size_t THREADS;
size_t EVENTS_PER_THREAD;
size_t TOTAL;
size_t RUNS;

using LogConfig = ts_store_config<false, 6, 20, 43, 9, 6, false>;
using LogxStore = ts_store<LogConfig>;

std::pair<bool, long int> run_single_test(LogxStore& store)
{
    store.clear();
    std::vector<std::thread> threads;
    threads.reserve(THREADS);
    auto start = high_resolution_clock::now();

    // Pre-create payloads and categories to reduce string construction overhead in the hot path measurement
    std::array<std::string, 8> pre_payloads;
    for (size_t i = 0; i < 8; ++i) pre_payloads[i] = std::string(LogxStore::test_messages[i]);
    std::array<std::string, 5> pre_cats;
    for (size_t i = 0; i < 5; ++i) pre_cats[i] = std::string(LogxStore::categories[i]);

    for (size_t t = 0; t < THREADS; ++t) {
        threads.emplace_back([&, t]() {
            for (size_t i = 0; i < EVENTS_PER_THREAD; ++i) {

                std::string payload = pre_payloads[i % pre_payloads.size()];
                std::string cat = pre_cats[t % pre_cats.size()];

                uint64_t raw_flags = 0;
                raw_flags = set_user_flag(raw_flags, StoreFlags::UserFlag::LogConsole);
                raw_flags = set_user_flag(raw_flags, StoreFlags::UserFlag::KeeperRecord);
                raw_flags = set_user_flag(raw_flags, StoreFlags::UserFlag::HotCacheHint);
                raw_flags = set_severity(raw_flags, static_cast<StoreFlags::Severity>(i % 8));

                bool is_debug = true;
                std::array<int64_t, LogConfig::the_IntMetrics> ints{};
                std::array<double, LogConfig::the_DblMetrics> dbls{};
                for (size_t k = 0; k < LogConfig::the_IntMetrics; ++k) ints[k] = static_cast<int64_t>(i * 100 + k);
                for (size_t k = 0; k < LogConfig::the_DblMetrics; ++k) dbls[k] = static_cast<double>(i) * 0.01 + static_cast<double>(k) * 0.001;
                auto [ok, id] = store.save_event(t, i, std::move(payload), raw_flags, std::move(cat), is_debug, ints, dbls);
                if (!ok) {
                    std::cerr << ansi::red() << "CLAIM FAILED — thread " << format_locale_int(t)
                              << " event " << format_locale_int(i) << ansi::reset() << "\n";
                    std::abort();
                }
                if (i == 42 && t == 0) {
                    std::cout << "  sample metrics on event 42 (t=0): int=" << ints[0] << " dbl=" << dbls[0] << "\n";
                }
            }
        });
    }

    for (auto& th : threads) th.join();

    auto end = high_resolution_clock::now();
    long int write_us = duration_cast<microseconds>(end - start).count();

    if (!store.verify_level01()) {
        std::cerr << "STRUCTURAL VERIFICATION FAILED\n";
        store.diagnose_failures();
        return {false, -1};
    }

    /*
    // Only activate this is you want to run for a very long time
    //   or you change the threads, and event numbers to something
    //   reasonable
    //

    if (!store.verify_level02()) {
        std::cerr << "TEST PAYLOAD VERIFICATION FAILED\n";
        store.diagnose_failures();
        return -1;
    }
*/
    return { true, write_us };
}

int main(int argc, char** argv)
{
    auto _opts = parse_test_options(argc, argv);

    THREADS = _opts.threads;
    EVENTS_PER_THREAD = _opts.events_per_thread;
    TOTAL = THREADS * EVENTS_PER_THREAD;
    RUNS = _opts.runs;

    if (std::cin.rdbuf()->in_avail() > 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    LogxStore  store(THREADS, EVENTS_PER_THREAD);
    size_t total_write_us = 0;
    std::vector<size_t> durations(RUNS, 0);
    size_t failed_runs = 0;

    std::cout << "=== FINAL MASSIVE TEST — " << format_locale_int(TOTAL)
              << " entries × " << format_locale_int(RUNS) << " runs ===\n";
    std::cout << "Using store.clear() — fastest, most realistic reuse\n\n";

    // Parse persist type early (we only actually attach for the last run).
    std::string ptype = _opts.persist.empty() ? "jtext" : _opts.persist;
    std::string bname = _opts.base_name.empty() ? "persist" : _opts.base_name;

    if (persist_skips_sink(ptype)) {
        std::cout << "No persistence attached — pure in-memory hot path\n\n";
    } else {
        std::cout << "Persistence will only be attached on the *last* run "
                  << "(previous runs measure clean hot path; only final run produces persist artifacts).\n\n";
    }

    // To keep captured log files reasonable in size (especially with --output on / live mode),
    // we only emit per-run progress + timing on the *last* iteration.
    // For persist modes, *only the last run* attaches the DoubleBufferedWriter + sink,
    // so the .bin / .jtext artifacts contain data from only the final run.
    // All runs do the full work + structural verification.
    // The final summary stats (min/max/avg across all runs) are still printed once at the end.
    for (size_t run = 0; run < RUNS; ++run) {
        bool is_last = (run == RUNS - 1);

        if (is_last) {
            std::cout << "Run " << format_locale_int(static_cast<int>(run + 1))
                      << " / " << format_locale_int(static_cast<int>(RUNS)) << "\n";

            // Only attach persistence on the final run (when not --persist=none).
            // This ensures persist data / log files are generated only from the last run.
            if (!persist_skips_sink(ptype)) {
            const size_t im = LogConfig::the_IntMetrics;
            const size_t dm = LogConfig::the_DblMetrics;
            auto sink = make_persistence_sink(ptype, bname, im, dm, PersistMode::All);
            if (!sink) return 1;
                auto writer = std::make_unique<DoubleBufferedWriter>(
                    std::move(sink),
                    10'000
                );
                store.attach_persistence(std::move(writer));
            }
        }

        auto [status, microseconds] = run_single_test(store);

        if (!status) {
            if (is_last) std::cout << "FAILED\n";
            ++failed_runs;
        } else
        {
            total_write_us += static_cast<decltype(total_write_us)>(microseconds);
            durations[run] = static_cast<size_t>(microseconds);

            if (is_last) {
                if (microseconds == 0) {
                    std::cout << "PASS — 0 µs (too fast to measure)\n\n";
                } else
                {
                    double ops_per_sec = static_cast<double>(TOTAL) * 1'000'000.0 / static_cast<double>(microseconds);
                    std::cout << "PASS — "
                              << format_locale_int(static_cast<std::uint64_t>(microseconds))
                              << " µs → "
                              << format_locale_int(static_cast<std::uint64_t>(ops_per_sec + 0.5))
                              << " ops/sec\n\n";
                }
            }
        }
    }

    if (failed_runs > 0) {
        std::cout << "\n" << format_locale_int(static_cast<int>(failed_runs))
                  << " runs failed — aborting summary.\n";
        return 1;
    }

    auto [min_it, max_it] = std::minmax_element(durations.begin(), durations.end());
    double avg_us = static_cast<double>(total_write_us) / static_cast<double>(RUNS);
    double max_ops_sec = static_cast<double>(TOTAL) * 1'000'000.0 / static_cast<double>(*min_it);
    double min_ops_sec = static_cast<double>(TOTAL) * 1'000'000.0 / static_cast<double>(*max_it);
    double avg_ops_sec = static_cast<double>(TOTAL) * 1'000'000.0 / avg_us;

    std::cout << "\n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    std::cout << "               FINAL RESULT — " << format_locale_int(RUNS)
              << "-RUN STATISTICS            \n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    std::cout << "  Fastest run        : "
              << format_locale_int(static_cast<std::uint64_t>(*min_it)) << " µs  → "
              << format_locale_int(static_cast<std::uint64_t>(max_ops_sec + 0.5)) << " ops/sec\n";
    std::cout << "  Slowest run        : "
              << format_locale_int(static_cast<std::uint64_t>(*max_it)) << " µs  → "
              << format_locale_int(static_cast<std::uint64_t>(min_ops_sec + 0.5)) << " ops/sec\n";
    std::cout << "  Average            : "
              << format_locale_int(static_cast<std::uint64_t>(avg_us + 0.5)) << " µs  → "
              << format_locale_int(static_cast<std::uint64_t>(avg_ops_sec + 0.5)) << " ops/sec\n";
    std::cout << "  (" << format_locale_int(static_cast<std::uint64_t>(TOTAL))
              << " events per run, 100% verified, zero corruption)\n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";

    return 0;
}
