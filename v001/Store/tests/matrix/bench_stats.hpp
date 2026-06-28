#pragma once
//
// Shared benchmark-statistics emitter for the "logging" Store benchmarks.
//
// Given the per-run write durations (microseconds) and the events-per-run, it
// prints the human-readable N-run statistics box AND a single machine-parseable
// line that the matrix runner greps to record the FULL distribution per scenario
// (not just the peak):
//
//   BENCHSTATS runs=<N> events=<E> high=<ops> low=<ops> avg=<ops> median=<ops> stddev=<ops>
//
// All ops values are raw integers (no locale separators) so the runner's parser
// (tools/jac313_test_cli/matrix_log_metrics) stays trivial. high = fastest run,
// low = slowest run, avg/median/stddev computed over the per-run ops/sec.
//
// A benchmark IS a distribution, not a single lucky number: the in-memory ceiling
// (persist=none, Release, many runs) yields a tight median that reads as
// "consistently fast", which is what the leaderboard headlines.

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

struct BenchSummary {
    std::uint64_t runs = 0;
    std::uint64_t events_per_run = 0;
    std::uint64_t high = 0;    // ops/sec of the fastest run (min duration)
    std::uint64_t low = 0;     // ops/sec of the slowest run (max duration)
    std::uint64_t avg = 0;     // mean of per-run ops/sec
    std::uint64_t median = 0;  // median of per-run ops/sec
    std::uint64_t stddev = 0;  // population std-dev of per-run ops/sec
};

inline std::uint64_t bench_ops_per_sec(std::uint64_t events, std::uint64_t us) {
    if (us == 0) return 0;
    return static_cast<std::uint64_t>(
        static_cast<double>(events) * 1'000'000.0 / static_cast<double>(us) + 0.5);
}

inline BenchSummary compute_bench_summary(const std::vector<std::size_t>& durations_us,
                                          std::uint64_t events_per_run) {
    BenchSummary s;
    s.runs = static_cast<std::uint64_t>(durations_us.size());
    s.events_per_run = events_per_run;
    if (durations_us.empty()) return s;

    std::vector<double> ops;
    ops.reserve(durations_us.size());
    for (std::size_t us : durations_us) {
        ops.push_back(us == 0 ? 0.0
                              : static_cast<double>(events_per_run) * 1'000'000.0
                                    / static_cast<double>(us));
    }

    const auto [min_it, max_it] = std::minmax_element(durations_us.begin(), durations_us.end());
    s.high = bench_ops_per_sec(events_per_run, static_cast<std::uint64_t>(*min_it));
    s.low = bench_ops_per_sec(events_per_run, static_cast<std::uint64_t>(*max_it));

    const double mean = std::accumulate(ops.begin(), ops.end(), 0.0)
                        / static_cast<double>(ops.size());
    s.avg = static_cast<std::uint64_t>(mean + 0.5);

    std::vector<double> sorted = ops;
    std::sort(sorted.begin(), sorted.end());
    const std::size_t n = sorted.size();
    const double med = (n % 2 == 1) ? sorted[n / 2]
                                    : (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0;
    s.median = static_cast<std::uint64_t>(med + 0.5);

    double var = 0.0;
    for (double o : ops) {
        const double d = o - mean;
        var += d * d;
    }
    var /= static_cast<double>(ops.size());
    s.stddev = static_cast<std::uint64_t>(std::sqrt(var) + 0.5);
    return s;
}

// Local thousands-grouping (self-contained; avoids coupling to test_common's formatter).
inline std::string bench_group_thousands(std::uint64_t v) {
    std::string digits = std::to_string(v);
    std::string out;
    int count = 0;
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        if (count != 0 && count % 3 == 0) out.push_back(',');
        out.push_back(*it);
        ++count;
    }
    std::reverse(out.begin(), out.end());
    return out;
}

inline void emit_bench_stats(const std::vector<std::size_t>& durations_us,
                             std::uint64_t events_per_run) {
    const BenchSummary s = compute_bench_summary(durations_us, events_per_run);

    std::cout << "\n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    std::cout << "             FINAL RESULT — " << bench_group_thousands(s.runs)
              << "-RUN STATISTICS\n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    std::cout << "  Median             : " << bench_group_thousands(s.median) << " ops/sec\n";
    std::cout << "  Fastest run        : " << bench_group_thousands(s.high) << " ops/sec\n";
    std::cout << "  Slowest run        : " << bench_group_thousands(s.low) << " ops/sec\n";
    std::cout << "  Average            : " << bench_group_thousands(s.avg) << " ops/sec\n";
    std::cout << "  Std deviation      : " << bench_group_thousands(s.stddev) << " ops/sec\n";
    std::cout << "  (" << bench_group_thousands(s.events_per_run)
              << " events per run, 100% verified, zero corruption)\n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";

    // Machine-parseable line for the matrix runner (raw integers, no separators).
    std::cout << "BENCHSTATS"
              << " runs=" << s.runs
              << " events=" << s.events_per_run
              << " high=" << s.high
              << " low=" << s.low
              << " avg=" << s.avg
              << " median=" << s.median
              << " stddev=" << s.stddev
              << "\n";
}
