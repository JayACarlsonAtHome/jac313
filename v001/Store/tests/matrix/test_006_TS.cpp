//tests/ts_store_006/Test_006_TS.CPP

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

// The lock-free producer→consumer handoff below coordinates via std::atomic
// release/acquire, which helgrind/DRD do not model as happens-before — so they
// report a spurious race on the payload row even though the access is correctly
// ordered. Annotate the handoff so the thread checkers see the edge. No-op unless
// built with -DJAC313_HELGRIND_ANNOTATE; the real macros live in valgrind's headers,
// which are only required when that flag is set.
#if defined(JAC313_HELGRIND_ANNOTATE)
#  include <valgrind/helgrind.h>
#else
#  define ANNOTATE_HAPPENS_BEFORE(addr) ((void)0)
#  define ANNOTATE_HAPPENS_AFTER(addr)  ((void)0)
#endif




#ifdef JAC313_STORE_HAS_SQL_PERSIST

#endif

// — Aggressive tail-reader stress test (500 threads × 100 ops)

using namespace jac313::Store::v001;
using namespace std::chrono;

// ———————————————————— Test configuration ————————————————————
// Runtime (see 003/005 pattern)
size_t WRITER_THREADS;
size_t OPS_PER_THREAD;
constexpr size_t MAX_ENTRIES        = matrix_test::kTailReaderMaxTracked;

alignas(64) inline std::atomic<size_t> log_stream_write_pos{0};
// Per-slot atomic publication: writer release-stores id+1 (0 == not-yet-published),
// reader acquire-loads and stops at the first empty slot. This pairs each slot's write
// with its read (no torn id read) and transitively makes the store row visible to
// select(), so a concurrent reader never touches an in-flight event.
inline std::array<std::atomic<size_t>, MAX_ENTRIES> log_stream_array{};
inline std::atomic<size_t> total_written{0};

using LogConfig = ts_store_config<true, 6, 20, 43, 9, 6, false>;
using LogxStore = ts_store<LogConfig>;


int main(int argc, char** argv) {
    auto _opts = parse_test_options(argc, argv);
    WRITER_THREADS = _opts.threads;
    OPS_PER_THREAD = _opts.events_per_thread;
    matrix_test::clamp_tail_reader_workload(WRITER_THREADS, OPS_PER_THREAD, MAX_ENTRIES);
    LogxStore store(WRITER_THREADS, OPS_PER_THREAD);

    // Attach double-buffered (asynchronous) persistence for this test run.
    // Chosen via --persist binary|jtext (default jtext). --base-name can override the
    // output file prefix (runner uses this to place files under test_results/*/TS_STORE_TEST_.../).
    {
        std::string ptype = _opts.persist.empty() ? "jtext" : _opts.persist;
        std::string bname = _opts.base_name;
        if (bname.empty()) bname = "persist";

        if (ptype != "none") {
            std::unique_ptr<IEventSink> sink;
            const size_t im = LogConfig::the_IntMetrics;
            const size_t dm = LogConfig::the_DblMetrics;
            if (ptype == "binary") {
                sink = std::make_unique<BinaryEventSink>(bname, im, dm, PersistMode::All);
            } else if (ptype == "sql") {
#ifdef JAC313_STORE_HAS_SQL_PERSIST
                sink = std::make_unique<SqlEventSink>(bname, im, dm, PersistMode::All, false);
#else
                std::cerr << "ERROR: SQL persistence not enabled at compile time (rebuild with -DJAC313_STORE_HAS_SQL_PERSIST=ON)\n";
                return 1;
#endif
            } else {
                sink = std::make_unique<JTextEventSink>(bname, im, dm, PersistMode::All);
            }
            auto writer = std::make_unique<DoubleBufferedWriter>(std::move(sink), 10'000);
            store.attach_persistence(std::move(writer));
        } else {
            std::cout << "No persistence attached — pure in-memory hot path\n";
        }
    }

    std::vector<std::thread> writers;
    writers.reserve(WRITER_THREADS);

    auto writer_start = steady_clock::now();
    std::cout << "Writer start time : "
              << format_locale_int(duration_cast<microseconds>(writer_start.time_since_epoch()).count())
              << " µs\n";

    for (size_t t = 0; t < WRITER_THREADS; ++t) {
        writers.emplace_back([&, t]() {
            for (size_t i = 0; i < OPS_PER_THREAD; ++i) {
                // Now matches verify_test_payloads() expectations

                std::string payload ( LogxStore::test_messages[i % LogxStore::test_messages.size()]);
                std::string cat  = std::string( LogxStore::categories[t % LogxStore::categories.size()]);

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
                if (ok) {
                    size_t pos = log_stream_write_pos.fetch_add(1, std::memory_order_relaxed);
                    if (pos < MAX_ENTRIES) {
                        ANNOTATE_HAPPENS_BEFORE(&log_stream_array[pos]);
                        log_stream_array[pos].store(id + 1, std::memory_order_release);
                    }
                    total_written.fetch_add(1, std::memory_order_release);
                }
            }
        });
    }

    while (total_written.load(std::memory_order_acquire) == 0)
        std::this_thread::yield();

    auto reader_start = steady_clock::now();
    auto lag_us = duration_cast<microseconds>(reader_start - writer_start).count();
    std::cout << "Reader start time : "
              << format_locale_int(duration_cast<microseconds>(reader_start.time_since_epoch()).count())
              << " µs\n";
    std::cout << "Reader start lag  : " << format_locale_int(lag_us) << " µs\n";

    std::atomic<long long> hits{0};
    std::atomic<long long> misses{0};

    const size_t expected_writes = WRITER_THREADS * OPS_PER_THREAD;
    const size_t tracked_limit =
        matrix_test::tail_reader_tracked_limit(expected_writes, MAX_ENTRIES);

    std::thread tail_reader([&]() {
        size_t last_read = 0;

        while (true) {
            const size_t written = total_written.load(std::memory_order_acquire);
            const size_t stream_end = log_stream_write_pos.load(std::memory_order_acquire);
            const size_t readable_end = std::min(stream_end, tracked_limit);

            bool stalled = false;
            while (last_read < readable_end) {
                const size_t slot = log_stream_array[last_read].load(std::memory_order_acquire);
                if (slot == 0) { stalled = true; break; }  // reserved but not yet published
                ANNOTATE_HAPPENS_AFTER(&log_stream_array[last_read]);
                const uint64_t id = slot - 1;
                ++last_read;
                auto [ok, _] = store.select(id);
                (ok ? hits : misses).fetch_add(1, std::memory_order_relaxed);
            }

            if (written >= expected_writes && last_read >= readable_end) {
                break;
            }

            if (stalled || last_read >= readable_end) {
                std::this_thread::sleep_for(microseconds(10));
            }
        }
    });

    for (auto& w : writers) w.join();

    auto writer_stop = steady_clock::now();
    std::cout << "Writer stop time  : "
              << format_locale_int(duration_cast<microseconds>(writer_stop.time_since_epoch()).count())
              << " µs\n";

    tail_reader.join();

    auto reader_stop = steady_clock::now();
    auto finish_lag = duration_cast<microseconds>(reader_stop - writer_stop).count();

    std::cout << "Reader stop time  : "
              << format_locale_int(duration_cast<microseconds>(reader_stop.time_since_epoch()).count())
              << " µs\n";
    std::cout << "Reader finish lag : " << format_locale_int(finish_lag) << " µs\n\n";

    std::cout << "Tail-reader result: " << format_locale_int(hits.load())
              << " hits, " << format_locale_int(misses.load()) << " misses (should be 0)\n";

    // Final verification
    if (!store.verify_level01()) {
        std::cerr << "STRUCTURAL VERIFICATION FAILED\n";
        store.diagnose_failures();
        return 1;
    }

    if (!store.verify_level02()) {
        std::cerr << "TEST PAYLOAD VERIFICATION FAILED\n";
        store.diagnose_failures();
        return 1;
    }

    std::cout << "ALL " << format_locale_int(store.expected_size())
              << " ENTRIES VERIFIED — ZERO CORRUPTION\n";
    store.show_duration("Store");
    store.print(0);
    return 0;
}
