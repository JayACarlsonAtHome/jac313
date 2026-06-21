// metric_view_test — the opt-in per-event metric summary view (read-side, zero hot-path cost).
// Shows a FULL event record (ids, flags, category, payload, timestamp + metrics) and the
// summary row that stands in for the raw metric values. Verifies count/min/max/average too.

#include <jac313/Store/v002.hpp>

#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <vector>
#include <span>
#include <iostream>

namespace S = jac313::Store::v002;

static bool close(double a, double b) { return std::fabs(a - b) < 1e-9; }

int main() {
    // --- Build a complete, realistic event -----------------------------------------------
    // Flags: routed to both sinks (KeeperRecord + DatabaseEntry), Info severity, and the two
    // "a metric summary view is available" bits (HasIntStats / HasDblStats).
    S::StoreFlags flags;
    flags.set(S::StoreFlags::UserFlag::KeeperRecord);
    flags.set(S::StoreFlags::UserFlag::DatabaseEntry);
    flags.set(S::StoreFlags::MetricFlag::HasIntStats);
    flags.set(S::StoreFlags::MetricFlag::HasDblStats);
    flags.set_severity(S::StoreFlags::Severity::Info);

    S::PersistedEvent e;
    e.event_id            = 42;
    e.thread_id           = 3;
    e.per_thread_event_id = 7;
    e.flags               = flags.raw();
    e.category            = "sensor.imu";
    e.payload             = "calibration sweep complete";
    e.timestamp_us        = 1718650000000000ULL;
    e.int_metrics         = {10, 20, 30, 40};   // count4 min10 max40 avg25
    e.dbl_metrics         = {1.0, 3.0};         // count2 min1  max3  avg2

    // --- Show the whole event record -----------------------------------------------------
    std::cout << "=== PersistedEvent ===\n";
    std::cout << "  event_id            : " << e.event_id << "\n";
    std::cout << "  thread_id           : " << e.thread_id << "\n";
    std::cout << "  per_thread_event_id : " << e.per_thread_event_id << "\n";
    std::cout << "  category            : " << e.category << "\n";
    std::cout << "  payload             : " << e.payload << "\n";
    std::cout << "  timestamp_us        : " << e.timestamp_us << "\n";
    std::cout << "  flags (raw)         : " << e.flags << "\n";
    std::cout << "  flags (decoded)     : " << flags.to_string() << "\n";
    std::cout << "  int_metrics (raw)   : ";
    for (auto v : e.int_metrics) std::cout << v << ' ';
    std::cout << "\n  dbl_metrics (raw)   : ";
    for (auto v : e.dbl_metrics) std::cout << v << ' ';
    std::cout << "\n";

    // --- The metric_view: one summary row instead of the raw metric values ---------------
    auto view = S::metric_view(e);
    std::cout << "\n=== metric_view (the summary row that replaces the raw metrics) ===\n";
    std::cout << "  ints   : " << view.ints.to_string() << "\n";
    std::cout << "  floats : " << view.doubles.to_string() << "\n";
    std::cout << "  one-row: " << view.to_string() << "\n";

    // --- Checks --------------------------------------------------------------------------
    if (!flags.is_set(S::StoreFlags::MetricFlag::HasIntStats)) return EXIT_FAILURE;
    if (view.ints.count != 4 || view.ints.min != 10 || view.ints.max != 40 || !close(view.ints.average, 25.0))
        return EXIT_FAILURE;
    if (view.doubles.count != 2 || !close(view.doubles.min, 1.0) || !close(view.doubles.max, 3.0) || !close(view.doubles.average, 2.0))
        return EXIT_FAILURE;
    // empty span must be safe (count 0, no UB)
    {
        std::vector<std::int64_t> empty;
        if (S::summarize_metrics(std::span<const std::int64_t>(empty)).count != 0) return EXIT_FAILURE;
    }

    std::cout << "\n=== all metric_view checks passed ===\n";
    return EXIT_SUCCESS;
}
