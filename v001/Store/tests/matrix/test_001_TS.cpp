//tests/ts_store_001/Test_001_TS.CPP

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

using LogConfig = ts_store_config<true, 6, 20, 43, 9, 6, false, false, false, false>;
using LogxStore = ts_store<LogConfig>;

int main(int argc, char** argv) {
    auto _opts = parse_test_options(argc, argv);
    (void)_opts; // silence -Wunused (CLI sets envs; we also read persist/base below)

    size_t threads = 8;
    size_t events  = 8;
    if (_opts.threads > 0) threads = _opts.threads;
    if (_opts.events_per_thread > 0) events = _opts.events_per_thread;

    if (std::cin.rdbuf()->in_avail() > 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cout << ansi::blue() << "=== jac313::Store — Simple Test 001 TS "
                               "-- Equivalent to MultiThread Hello World  ===" << ansi::reset() << "\n\n";
    std::cout << "Threads: " << format_locale_int(threads)
              << "    Events/thread: " << format_locale_int(events) << "\n\n";

    LogxStore prod(threads, events);

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
            prod.attach_persistence(std::move(writer));
        } else {
            std::cout << "No persistence attached — pure in-memory hot path\n";
        }
    }

    prod.test_run(); //Test Run sets Debug == True
    if (!prod.verify_level01()) {
        std::cerr << "PRODUCTION SIMULATION FAILED — structural corruption\n";
        prod.diagnose_failures();
        return 1;
    }
    if (!prod.verify_level02()) {
        std::cerr << "PRODUCTION SIMULATION FAILED — test payload corruption\n";
        prod.diagnose_failures();
        return 1;
    }
    std::cout << "PRODUCTION SIMULATION PASSED — 100% clean\n";
    prod.press_any_key();
    prod.print(0);
    std::cout << "\n=== ALL TESTS COMPLETED SUCCESSFULLY ===\n";
    return 0;
}
