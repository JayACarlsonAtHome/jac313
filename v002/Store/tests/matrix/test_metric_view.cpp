// test_metric_view.cpp — matrix item: the per-event metric SUMMARY view at scale.
//
//   smoke = 100 records, full = 100,000 records (via --test-size).
//
// For each event it builds flags (routing + the HasIntStats/HasDblStats "view available"
// bits + severity), int/float metrics, and computes the metric_view — one summary row
// (count/min/max/avg) standing in for the raw metric values. The output shows the event,
// its metrics (as the summary), and its flags. Read-side only: nothing here is on a store
// hot path.

#include <jac313/Store/v002.hpp>
#include <jac313/Store/v002/headers/impl_details/test_options.hpp>

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <span>

using namespace jac313::Store::v002;

static PersistedEvent make_event(std::size_t i) {
    StoreFlags flags;
    flags.set(StoreFlags::UserFlag::KeeperRecord);
    if (i % 2 == 0) flags.set(StoreFlags::UserFlag::DatabaseEntry);
    flags.set(StoreFlags::MetricFlag::HasIntStats);   // "an int summary view is available"
    flags.set(StoreFlags::MetricFlag::HasDblStats);   // "a float summary view is available"
    flags.set_severity(static_cast<StoreFlags::Severity>(i % 8));

    PersistedEvent e;
    e.event_id            = i;
    e.thread_id           = i % 4;
    e.per_thread_event_id = i / 4;
    e.flags               = flags.raw();
    e.category            = "metric.sample";
    e.payload             = "event " + std::to_string(i);
    e.timestamp_us        = 1718650000000000ULL + i;
    // vary the metrics so min/max/avg differ per event (and counts can differ)
    e.int_metrics = { static_cast<std::int64_t>(i),
                      static_cast<std::int64_t>(i * 2),
                      static_cast<std::int64_t>(i + 5),
                      static_cast<std::int64_t>(100 - (i % 100)) };
    e.dbl_metrics = { static_cast<double>(i) * 0.5,
                      static_cast<double>(i % 10) + 0.25,
                      -static_cast<double>(i % 7) };
    return e;
}

int main(int argc, char** argv) {
    const auto opts = parse_test_options(argc, argv);
    const std::size_t records = (opts.test_size == "full") ? 100000u : 100u;
    const std::size_t show    = records < 100 ? records : 100;   // cap the per-row dump

    std::cout << "=== metric_view matrix item: " << records << " records ("
              << (opts.test_size == "full" ? "full" : "smoke") << ") ===\n";

    // One event in FULL detail so "the event, the metrics, the flags" is all visible once.
    {
        const PersistedEvent e = make_event(0);
        const StoreFlags f(e.flags);
        const auto v = metric_view(e);
        std::cout << "--- event in full ---\n";
        std::cout << "  event_id     : " << e.event_id << "\n";
        std::cout << "  thread_id    : " << e.thread_id << "\n";
        std::cout << "  category     : " << e.category << "\n";
        std::cout << "  payload      : " << e.payload << "\n";
        std::cout << "  timestamp_us : " << e.timestamp_us << "\n";
        std::cout << "  flags        : " << f.to_string() << "\n";
        std::cout << "  int_metrics  : ";
        for (auto x : e.int_metrics) std::cout << x << ' ';
        std::cout << "\n  dbl_metrics  : ";
        for (auto x : e.dbl_metrics) std::cout << x << ' ';
        std::cout << "\n  SUMMARY      : " << v.to_string() << "\n--- per-event summary rows ---\n";
    }

    // Process every record (exercise the view at scale); dump one summary row each up to the cap.
    for (std::size_t i = 0; i < records; ++i) {
        const PersistedEvent e = make_event(i);
        const auto view = metric_view(e);              // read-side summary; this is the work
        if (i < show) {
            const StoreFlags f(e.flags);
            std::cout << "event " << e.event_id << " thr=" << e.thread_id
                      << " flags=[" << f.to_string() << "]"
                      << " | ints " << view.ints.to_string()
                      << " | floats " << view.doubles.to_string() << "\n";
        }
    }
    if (records > show) {
        std::cout << "... (" << (records - show) << " more events processed)\n";
    }

    std::cout << "=== metric_view matrix item: processed " << records << " records ===\n";
    return 0;
}
