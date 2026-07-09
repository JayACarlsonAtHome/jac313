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
        if (!attach_persistence_from_opts<LogConfig>(prod, _opts)) return 1;
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
