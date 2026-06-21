// tests/ts_store_008/test_008_TS.cpp
//
// Flag-selective persistence: N in-memory events, K with KeeperRecord (jText file),
// D with DatabaseEntry (SQLite) — disjoint index sets. Proves sinks honor flags
// and measures hot-path throughput when most events skip durable I/O.
// Full mode sizing from runner (currently 50×20k = 1M × 3 runs). Only last run persists.

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <format>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "test_common.hpp"



#ifdef JAC313_STORE_HAS_SQL_PERSIST

#endif

using namespace jac313::Store::v002;
using namespace std::chrono;

using LogConfig = ts_store_config<true, 6, 20, 43, 9, 6, false>;
using LogxStore = ts_store<LogConfig>;

size_t THREADS;
size_t EVENTS_PER_THREAD;
size_t TOTAL;
size_t RUNS;

namespace {

// 1% of total per flag type (min 10): 1k→10, 10k→100, 1M→10k keeper + 10k database.
size_t flagged_per_type(size_t total_events) {
    return std::max<size_t>(10, total_events / 100);
}

void print_test_purpose(size_t total, size_t keeper_n, size_t db_n) {
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    std::cout << " TEST 008 TS — Flag-selective persistence\n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    std::cout << " Purpose:\n";
    std::cout << "   Prove KeeperRecord routes to jText file and DatabaseEntry routes to\n";
    std::cout << "   SQLite while the other "
              << format_locale_int(total - keeper_n - db_n)
              << " events stay in-memory only on the persist path.\n";
    std::cout << "   Expect higher ops/sec than persisting all "
              << format_locale_int(total) << " events.\n\n";
    std::cout << " Plan: " << format_locale_int(total) << " in-memory events × "
              << format_locale_int(RUNS) << " runs\n";
    std::cout << "   • indices 0.." << format_locale_int(keeper_n - 1)
              << "     → KeeperRecord  (jText file)\n";
    std::cout << "   • indices " << format_locale_int(keeper_n) << ".."
              << format_locale_int(keeper_n + db_n - 1)
              << " → DatabaseEntry (SQLite)\n";
    std::cout << "   • remainder           → no persist flags\n\n";
}

uint64_t flags_for_global_index(size_t gid, size_t keeper_n, size_t db_n) {
    uint64_t raw = 0;
    if (gid < keeper_n) {
        raw = set_user_flag(raw, StoreFlags::UserFlag::KeeperRecord);
    } else if (gid < keeper_n + db_n) {
        raw = set_user_flag(raw, StoreFlags::UserFlag::DatabaseEntry);
    }
    return raw;
}

} // namespace

std::pair<bool, long int> run_single_test(LogxStore& store, size_t keeper_n, size_t db_n)
{
    store.clear();

    std::array<std::string, 8> pre_payloads;
    for (size_t i = 0; i < pre_payloads.size(); ++i) {
        pre_payloads[i] = std::string(LogxStore::test_messages[i]);
    }
    std::array<std::string, 5> pre_cats;
    for (size_t i = 0; i < pre_cats.size(); ++i) {
        pre_cats[i] = std::string(LogxStore::categories[i]);
    }

    auto start = high_resolution_clock::now();

    std::vector<std::thread> workers;
    workers.reserve(THREADS);

    for (size_t t = 0; t < THREADS; ++t) {
        workers.emplace_back([&, t]() {
            for (size_t i = 0; i < EVENTS_PER_THREAD; ++i) {
                const size_t gid = t * EVENTS_PER_THREAD + i;
                std::string payload = pre_payloads[i % pre_payloads.size()];
                std::string cat = pre_cats[t % pre_cats.size()];

                uint64_t raw_flags = flags_for_global_index(gid, keeper_n, db_n);

                std::array<int64_t, LogConfig::the_IntMetrics> ints{};
                std::array<double, LogConfig::the_DblMetrics> dbls{};
                for (size_t k = 0; k < LogConfig::the_IntMetrics; ++k) {
                    ints[k] = static_cast<int64_t>(gid * 10 + k);
                }
                for (size_t k = 0; k < LogConfig::the_DblMetrics; ++k) {
                    dbls[k] = static_cast<double>(gid) * 0.01;
                }

                auto [ok, id] = store.save_event(
                    t, i, std::move(payload), raw_flags, std::move(cat), false, ints, dbls);
                if (!ok) {
                    std::cerr << "save_event failed t=" << format_locale_int(t)
                              << " i=" << format_locale_int(i) << "\n";
                    std::abort();
                }
                (void)id;
            }
        });
    }

    for (auto& th : workers) th.join();

    auto end = high_resolution_clock::now();
    long int elapsed_us = duration_cast<microseconds>(end - start).count();

    if (!store.verify_level01()) {
        std::cerr << "STRUCTURAL VERIFICATION FAILED\n";
        return {false, -1};
    }

    return {true, elapsed_us};
}

int main(int argc, char** argv) {
#ifndef JAC313_STORE_HAS_SQL_PERSIST
    std::cerr << "ERROR: test 008 requires SQLite persistence (JAC313_STORE_HAS_SQL_PERSIST=ON)\n";
    return 1;
#endif

    auto opts = parse_test_options(argc, argv);

    THREADS = opts.threads > 0 ? opts.threads : 10;
    EVENTS_PER_THREAD = opts.events_per_thread > 0 ? opts.events_per_thread : 100;
    TOTAL = THREADS * EVENTS_PER_THREAD;
    RUNS = opts.runs > 0 ? opts.runs : 1;

    const size_t keeper_n = flagged_per_type(TOTAL);
    const size_t db_n = keeper_n;

    if (std::cin.rdbuf()->in_avail() > 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    print_test_purpose(TOTAL, keeper_n, db_n);

    std::string bname = opts.base_name.empty() ? "persist" : opts.base_name;
    const std::string file_base = bname + "_keeper";
    const std::string sql_base  = bname + "_database";

    const size_t im = LogConfig::the_IntMetrics;
    const size_t dm = LogConfig::the_DblMetrics;

    LogxStore store(THREADS, EVENTS_PER_THREAD);

    JTextEventSink* jtext_raw = nullptr;
    SqlEventSink*   sql_raw   = nullptr;

    size_t total_write_us = 0;
    std::vector<size_t> durations(RUNS, 0);
    size_t failed_runs = 0;

    std::cout << "Flag routing persistence attaches only on the *last* run "
              << "(previous runs measure clean hot path; final run verifies counts).\n\n";

    for (size_t run = 0; run < RUNS; ++run) {
        const bool is_last = (run == RUNS - 1);

        if (is_last) {
            std::cout << "Run " << format_locale_int(static_cast<int>(run + 1))
                      << " / " << format_locale_int(static_cast<int>(RUNS)) << "\n";

            auto jtext_sink = std::make_unique<JTextEventSink>(file_base, im, dm, PersistMode::KeeperOnly);
            auto sql_sink   = std::make_unique<SqlEventSink>(sql_base, im, dm, PersistMode::DatabaseOnly, false);
            jtext_raw = jtext_sink.get();
            sql_raw   = sql_sink.get();

            auto routing_sink = std::make_unique<FlagRoutingEventSink>(
                std::move(jtext_sink), std::move(sql_sink));
            auto writer = std::make_unique<DoubleBufferedWriter>(std::move(routing_sink), 10'000);
            store.attach_persistence(std::move(writer));
        }

        auto [status, microseconds] = run_single_test(store, keeper_n, db_n);

        if (!status) {
            if (is_last) std::cout << "FAILED\n";
            ++failed_runs;
        } else {
            total_write_us += static_cast<decltype(total_write_us)>(microseconds);
            durations[run] = static_cast<size_t>(microseconds);

            if (is_last) {
                if (microseconds == 0) {
                    std::cout << "PASS — 0 µs (too fast to measure)\n\n";
                } else {
                    const auto ops_per_sec = static_cast<std::uint64_t>(
                        static_cast<double>(TOTAL) * 1'000'000.0 / static_cast<double>(microseconds) + 0.5);
                    std::cout << "PASS — "
                              << format_locale_int(static_cast<std::uint64_t>(microseconds))
                              << " µs → "
                              << format_locale_int(ops_per_sec)
                              << " ops/sec\n\n";
                }

                store.finalize_persistence();

                const size_t file_rows = jtext_raw->main_row_count();
                const size_t sql_rows  = sql_raw->main_row_count();

                std::cout << "Persist verification:\n";
                std::cout << "  jText main rows (KeeperRecord):  "
                          << format_locale_int(file_rows)
                          << " (expected " << format_locale_int(keeper_n) << ")\n";
                std::cout << "  SQLite main rows (DatabaseEntry): "
                          << format_locale_int(sql_rows)
                          << " (expected " << format_locale_int(db_n) << ")\n";

                if (file_rows != keeper_n || sql_rows != db_n) {
                    std::cerr << "FLAG ROUTING FAILED — counts do not match expected selective persist\n";
                    return 1;
                }

                std::cout << "  PASS — flags honored; only "
                          << format_locale_int(keeper_n + db_n) << " / "
                          << format_locale_int(TOTAL)
                          << " events reached durable storage\n\n";
            }
        }
    }

    if (failed_runs > 0) {
        std::cout << "\n" << format_locale_int(static_cast<int>(failed_runs))
                  << " runs failed — aborting summary.\n";
        return 1;
    }

    auto [min_it, max_it] = std::minmax_element(durations.begin(), durations.end());
    const double avg_us = static_cast<double>(total_write_us) / static_cast<double>(RUNS);
    const double max_ops_sec = static_cast<double>(TOTAL) * 1'000'000.0 / static_cast<double>(*min_it);
    const double min_ops_sec = static_cast<double>(TOTAL) * 1'000'000.0 / static_cast<double>(*max_it);
    const double avg_ops_sec = static_cast<double>(TOTAL) * 1'000'000.0 / avg_us;

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
              << " events per run, flag routing verified on final run)\n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";

    return 0;
}
