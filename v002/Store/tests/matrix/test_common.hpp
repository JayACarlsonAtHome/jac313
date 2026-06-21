#pragma once

#include <jac313/Store/v002.hpp>
#include <jac313/Store/v002/headers/impl_details/format_locale.hpp>
#include <jac313/Store/v002/headers/persistence/BinaryEventSink.hpp>
#include <jac313/Store/v002/headers/persistence/FlagRoutingEventSink.hpp>
#include <jac313/Store/v002/headers/persistence/JTextEventSink.hpp>

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <string>

#ifdef JAC313_STORE_HAS_SQL_PERSIST
#include <jac313/Store/v002/headers/persistence/SqlEventSink.hpp>
#endif

namespace jac313::Store::v002::matrix_test {

// Tail-reader tests (003/006) track stream IDs in a fixed-size ring.
inline constexpr size_t kTailReaderMaxTracked = 100'000;

inline void clamp_tail_reader_workload(size_t& threads,
                                       size_t& events_per_thread,
                                       const size_t max_tracked = kTailReaderMaxTracked)
{
    if (threads == 0 || events_per_thread == 0) {
        return;
    }
    const size_t total = threads * events_per_thread;
    if (total <= max_tracked) {
        return;
    }
    events_per_thread = std::max<size_t>(1, max_tracked / threads);
    if (threads * events_per_thread > max_tracked) {
        threads = std::max<size_t>(1, max_tracked / events_per_thread);
    }
    std::cerr << "NOTE: tail-reader workload clamped to "
              << threads << " x " << events_per_thread << " = "
              << (threads * events_per_thread) << " (cap " << max_tracked << ")\n";
}

inline size_t tail_reader_tracked_limit(const size_t total_events,
                                        const size_t max_tracked = kTailReaderMaxTracked)
{
    return std::min(total_events, max_tracked);
}

inline void consume_pending_stdin() {
    if (std::cin.rdbuf()->in_avail() > 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

template<typename Config, typename Store>
bool attach_persistence_from_opts(Store& prod, const TestOptions& opts) {
    std::string ptype = opts.persist.empty() ? "jtext" : opts.persist;
    std::string bname = opts.base_name.empty() ? "persist" : opts.base_name;

    if (ptype == "none") {
        std::cout << "No persistence attached — pure in-memory hot path\n";
        return true;
    }

    std::unique_ptr<IEventSink> sink;
    const size_t im = Config::the_IntMetrics;
    const size_t dm = Config::the_DblMetrics;

    if (ptype == "binary") {
        sink = std::make_unique<BinaryEventSink>(bname, im, dm, PersistMode::All);
    } else if (ptype == "sql") {
#ifdef JAC313_STORE_HAS_SQL_PERSIST
        sink = std::make_unique<SqlEventSink>(bname, im, dm, PersistMode::All, false);
#else
        std::cerr << "ERROR: SQL persistence not enabled at compile time\n";
        return false;
#endif
    } else {
        sink = std::make_unique<JTextEventSink>(bname, im, dm, PersistMode::All);
    }

    auto writer = std::make_unique<DoubleBufferedWriter>(std::move(sink), 10'000);
    prod.attach_persistence(std::move(writer));
    return true;
}

} // namespace jac313::Store::v002::matrix_test