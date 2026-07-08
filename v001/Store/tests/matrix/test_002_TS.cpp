//tests/ts_store_002/Test_002_TS.CPP

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

using LogConfig = ts_store_config<true, 6, 20, 43, 9, 6, false>;
using LogxStore = ts_store<LogConfig>;

int main(int argc, char** argv) {
    auto _opts = parse_test_options(argc, argv);
    if (std::cin.rdbuf()->in_avail() > 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Respect runner / test_params.txt scaling (progressive difficulty).
    // Defaults kept at the historical small values for this test.
    size_t num_threads       = 25;
    size_t events_per_thread = 100;
    if (_opts.threads > 0)          num_threads = _opts.threads;
    if (_opts.events_per_thread > 0) events_per_thread = _opts.events_per_thread;
    size_t total_entries     = uint64_t(num_threads) * events_per_thread;

    std::cout << ansi::yellow() << "=== jac313::Store Test 002 TS with "
              << format_locale_int(total_entries) << " entries ===\n" << ansi::reset();
    std::cout << ansi::white() << "Threads: " << format_locale_int(num_threads)
              << "    Events/thread: " << format_locale_int(events_per_thread)
              << "    Total: " << format_locale_int(total_entries) << "\n\n"
              << ansi::reset();

    LogxStore store(num_threads, events_per_thread);

    // Attach double-buffered (asynchronous) persistence for this test run.
    // Chosen via --persist binary|jtext (default jtext). --base-name can override the
    // output file prefix (runner uses this to place files under test_results/*/TS_STORE_TEST_.../).
    {
        std::string ptype = _opts.persist.empty() ? "jtext" : _opts.persist;
        std::string bname = _opts.base_name;
        if (bname.empty()) bname = "persist";

        if (!persist_skips_sink(ptype)) {
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

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (size_t t = 0; t < num_threads; ++t) {
        threads.emplace_back([t, &store, events_per_thread] {
            for (size_t i = 0; i < events_per_thread; ++i) {

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
                if (!ok) {
                    std::cout << ansi::bold() << ansi::red()
                              << "[FATAL] claim failed — thread " << format_locale_int(t)
                              << " event " << format_locale_int(i) << "\n"
                              << ansi::reset();
                    std::abort();
                }
            }
        });
    }

    std::cout << "All threads launched — crossing the streams at full power...\n";
    for (auto& th : threads) th.join();

    std::cout << "\nAll threads joined — running final verification...\n\n";

    if (!store.verify_level01()) {
        std::cerr << ansi::bold() << ansi::red() << "Structural verification failed!\n" << ansi::reset();
        store.diagnose_failures();
        return 1;
    }

    if (!store.verify_level02()) {
        std::cerr << ansi::bold() << ansi::red() << "Payload verification failed!\n" << ansi::reset();
        store.diagnose_failures();
        return 1;
    }

    std::cout << ansi::bold() << ansi::blue()
              << "╔════════════════════════════════════════════════╗\n"
              << "║ All " << format_locale_int(total_entries)
              << " entries passed verification!                  ║\n"
              << "╚════════════════════════════════════════════════╝\n"
              << ansi::reset();
    return 0;
}
