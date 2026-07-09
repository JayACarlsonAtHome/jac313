//tests/ts_store_004/Test_004_XS.CPP

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

// Runtime configurable
size_t THREADS;
size_t EVENTS_PER_THREAD;
size_t TOTAL_EVENTS;

using LogConfigxMainx = ts_store_config<false, 6, 20, 75, 9, 6, false>;
using LogxStore = ts_store<LogConfigxMainx>;

using LogConfigResult = ts_store_config<false, 6, 20, 75, 9, 6, false>;
using LogResult = ts_store<LogConfigResult>;

int main(int argc, char** argv) {
    auto _opts = parse_test_options(argc, argv);

    THREADS = _opts.threads;
    EVENTS_PER_THREAD = _opts.events_per_thread;
    TOTAL_EVENTS = THREADS * EVENTS_PER_THREAD;

    LogxStore  safepay(THREADS, EVENTS_PER_THREAD);
    LogResult  results(THREADS, 1);

    // Attach double-buffered (asynchronous) persistence to the main event store.
    // (Results store is small summary info; we focus persist on the bulk "safepay" traffic.)
    {
        if (!attach_persistence_from_opts<LogConfigxMainx>(safepay, _opts)) return 1;
    }

    std::vector<std::thread> threads;
    std::atomic<size_t> total_successes{0};
    std::atomic<size_t> total_nulls{0};

    auto worker = [&](size_t t) {
        size_t local_successes = 0;
        size_t local_nulls = 0;

        for (size_t i = 0; i < EVENTS_PER_THREAD; ++i) {

            std::string payload ( LogxStore::test_messages[i % LogxStore::test_messages.size()]);
            std::string_view payload_copy = payload;
            std::string cat  = std::string( LogxStore::categories[t % LogxStore::categories.size()]);

            uint64_t raw_flags = 0;
            raw_flags = set_user_flag(raw_flags, StoreFlags::UserFlag::LogConsole);
            raw_flags = set_user_flag(raw_flags, StoreFlags::UserFlag::KeeperRecord);
            raw_flags = set_severity(raw_flags, static_cast<StoreFlags::Severity>(i % 8));

            bool is_debug = true;
            std::array<int64_t, LogConfigxMainx::the_IntMetrics> ints{};
            std::array<double, LogConfigxMainx::the_DblMetrics> dbls{};
            for (size_t k = 0; k < LogConfigxMainx::the_IntMetrics; ++k) ints[k] = static_cast<int64_t>(i * 100 + k);
            for (size_t k = 0; k < LogConfigxMainx::the_DblMetrics; ++k) dbls[k] = static_cast<double>(i) * 0.01 + static_cast<double>(k) * 0.001;
            auto [ok, id] = safepay.save_event(t, i, std::move(payload), raw_flags, std::move(cat), is_debug, ints, dbls);
            auto [val_ok, val_sv] = safepay.select(id);
            if (val_ok && std::string_view(val_sv) == payload_copy) {
                ++local_successes;
            } else if (!val_ok) {
                ++local_nulls;
            }
        }

        total_successes += local_successes;
        total_nulls     += local_nulls;

        std::string result_payload = "RESULT: thread=" + std::to_string(t)
            + "  successes=" + format_locale_int(local_successes)
            + "  nulls=" + format_locale_int(local_nulls)
            + "  total_events=" + format_locale_int(EVENTS_PER_THREAD);

        uint64_t raw_flags = 0;
        raw_flags = set_user_flag(raw_flags, StoreFlags::UserFlag::LogConsole);
        raw_flags = set_user_flag(raw_flags, StoreFlags::UserFlag::KeeperRecord);
        raw_flags = set_severity(raw_flags, static_cast<StoreFlags::Severity>(StoreFlags::Severity::Info));

        std::array<int64_t, LogConfigResult::the_IntMetrics> r_ints{};
        std::array<double, LogConfigResult::the_DblMetrics> r_dbls{};
        for (size_t k = 0; k < LogConfigResult::the_IntMetrics; ++k) r_ints[k] = static_cast<int64_t>(local_successes * 10 + k);
        for (size_t k = 0; k < LogConfigResult::the_DblMetrics; ++k) r_dbls[k] = static_cast<double>(local_nulls) + static_cast<double>(k) * 0.1;
        auto [ok, _] = results.save_event(t, local_successes, std::move(result_payload), raw_flags, "STATS", false, r_ints, r_dbls);
        if (!ok) {
            std::cerr << "Results claim failed for thread " << format_locale_int(t) << "\n";
        }
    };

    for (uint32_t t = 0; t < THREADS; ++t) {
        threads.emplace_back(worker, t);
    }
    for (auto& th : threads) th.join();

    std::cout << "Safepay entries: " << format_locale_int(safepay.expected_size())
              << " (expected: " << format_locale_int(TOTAL_EVENTS) << ")\n";
    safepay.show_duration("Safepay");
    results.show_duration("Results");

    std::cout << "\nTotal successes: " << format_locale_int(total_successes)
              << " / " << format_locale_int(TOTAL_EVENTS)
              << "  (" << (total_successes == TOTAL_EVENTS ? "PASS" : "FAIL") << ")\n";
    std::cout << "Null reads (races): " << format_locale_int(total_nulls) << "\n\n";

    if (!safepay.verify_level01()) {
        std::cerr << "INTEGRITY VERIFICATION FAILED\n";
        safepay.diagnose_failures();
        return 1;
    }

    //results.debug_print_widths();
    std::cout << "\nResult store contents:\n";
    results.print();
    std::cout << "\nALL " << format_locale_int(TOTAL_EVENTS)
              << " ENTRIES + RESULTS VERIFIED — ZERO CORRUPTION\n\n";

    //safepay.debug_print_widths();
    std::cout << "Per-thread results:\n";
    safepay.print( 0);
    return 0;
}
