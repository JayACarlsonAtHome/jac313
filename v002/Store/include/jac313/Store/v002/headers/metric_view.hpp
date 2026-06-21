#pragma once

// metric_view.hpp — opt-in SUMMARY VIEW over an event's numeric metric fields.
//
// Computes {count, min, max, average} over the integer metrics and (separately) over the
// float metrics of an event, on demand. The point: instead of emitting one row per metric
// value — six rows of the "same" event for six numbers — emit ONE summary row that carries
// min/max/average/count. You only pay for the computation when you ask for the view.
//
// Header-only (templates + inline). Guarded for the import-std module build like every other
// store header (see Claude_Forward / docs/Modules.md).

#ifndef JAC313_STORE_IMPORT_STD
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <format>
#endif

// store cross-include (kept in the module build): PersistedEvent for the convenience overload
#include "persistence/EventSink.hpp"

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v002 {
#endif

/// Summary of a run of numeric values: how many, the smallest, the largest, the mean.
/// `average` is always a double, even for integer metrics.
template <typename T>
struct MetricSummary {
    std::size_t count{0};
    T           min{};
    T           max{};
    double      average{0.0};

    [[nodiscard]] std::string to_string() const {
        return std::format("count={} min={} max={} avg={:.6g}", count, min, max, average);
    }
};

/// Compute {count, min, max, average} over a span of values. Empty span → count 0, the
/// rest default-constructed (so a no-metrics event summarizes cleanly).
template <typename T>
[[nodiscard]] MetricSummary<T> summarize_metrics(std::span<const T> values) {
    MetricSummary<T> s;
    s.count = values.size();
    if (values.empty()) return s;
    s.min = s.max = values.front();
    long double sum = 0.0L;
    for (const T v : values) {
        if (v < s.min) s.min = v;
        if (v > s.max) s.max = v;
        sum += static_cast<long double>(v);
    }
    s.average = static_cast<double>(sum / static_cast<long double>(values.size()));
    return s;
}

/// Per-event view: the integer metrics and the float metrics summarized SEPARATELY, so one
/// row can stand in for the whole metric block of an event.
struct EventMetricSummary {
    MetricSummary<std::int64_t> ints;
    MetricSummary<double>       doubles;

    [[nodiscard]] std::string to_string() const {
        return std::format("ints[{}] floats[{}]", ints.to_string(), doubles.to_string());
    }
};

/// Build the view from raw int + float metric spans.
[[nodiscard]] inline EventMetricSummary metric_view(std::span<const std::int64_t> ints,
                                                    std::span<const double> floats) {
    return { summarize_metrics(ints), summarize_metrics(floats) };
}

/// Build the view from a PersistedEvent — summarize its int and float metrics separately.
/// Use this to write one summary row per event instead of one row per metric value.
[[nodiscard]] inline EventMetricSummary metric_view(const PersistedEvent& e) {
    return metric_view(std::span<const std::int64_t>(e.int_metrics),
                       std::span<const double>(e.dbl_metrics));
}

#ifndef JAC313_STORE_IMPORT_STD
}  // namespace jac313::Store::v002
#endif
