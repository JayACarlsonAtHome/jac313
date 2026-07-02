// ts_store/ts_store_headers/ts_store.hpp
// C++23 — GCC 15 — December 2025
#pragma once

#include "includes.hpp"

#include "persistence/DoubleBufferedWriter.hpp"

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v001 {
#endif
// ——————————————————————— CONCEPTS ———————————————————————
template<typename Config>
class ts_store
{
private:
    struct row_data {
        size_t   event_flags{0};
        size_t   thread_id{0};
        size_t   event_id{0};
        std::array<int64_t, Config::the_IntMetrics> int_metrics{};
        std::array<double,  Config::the_DblMetrics> dbl_metrics{};
        bool     is_debug{false};
        // Now using bounded/fixed storage (no more std::string for hot path cat/payload).
        // This gives exact preallocation, direct writes, no capacity/SSO overhead.
        Config::CategoryT category_storage{};
        Config::ValueT    value_storage{};
        std::conditional_t<Config::use_timestamps, uint64_t, std::monostate> ts_us{};
    };

    const size_t max_threads_;
    const size_t events_per_thread_;

    // Best-available physical memory (bytes) for the preallocation precheck. Prefer the kernel's
    // MemAvailable, which is reclaim-aware (it counts reclaimable page cache). sysinfo() does NOT:
    // freeram+bufferram+sharedram excludes the page cache, so right after a build warms the cache it
    // under-reports by gigabytes and this precheck bails on an allocation the kernel could satisfy —
    // that false bail is what zeroed the recorded bench gate (→ "NOT SAFE"). Fall back to sysinfo
    // when /proc/meminfo can't be read (non-Linux, restricted /proc, etc.).
    static size_t available_ram_bytes() noexcept {
        std::ifstream meminfo("/proc/meminfo");
        std::string key;
        while (meminfo >> key) {
            if (key == "MemAvailable:") {
                size_t kb = 0;
                if (meminfo >> kb) return kb << 10;   // kB -> bytes
                break;
            }
            std::getline(meminfo, key);               // skip the rest of this line
        }
        struct sysinfo info{};
        if (sysinfo(&info) == 0)
            return static_cast<size_t>(info.freeram) + info.bufferram + info.sharedram;
        return 0;
    }
public:
    // ——— GETTERS ———
[[nodiscard]] constexpr size_t id_width() const noexcept {
        const size_t max_id = expected_size() ? expected_size() - 1 : 0;
        if (max_id == 0) return 1;
        return static_cast<size_t>(std::log10(static_cast<double>(max_id))) + 2;
    }

    [[nodiscard]] constexpr size_t get_max_threads() const noexcept { return max_threads_; }

    [[nodiscard]] size_t thread_id_width() const noexcept {
        const size_t n = get_max_threads();
        if (n <= 1) return 2;
        size_t digits = 1;
        size_t temp = n - 1;
        while (temp >= 10) {
            temp /= 10;
            ++digits;
        }
    return digits + 1;
    }

    [[nodiscard]] constexpr size_t get_max_events() const noexcept { return events_per_thread_; }

    [[nodiscard]] size_t events_id_width() const noexcept {
    const size_t n = get_max_events();
    if (n <= 1) return 2;               // enough for "0" or "1"
    const size_t max_value = n - 1;
    // safe log10 — no undefined behavior on 0
    size_t digits = 1;
    size_t temp = max_value;
    while (temp >= 10) {
        temp /= 10;
        ++digits;
    }
    return digits + 1;  // +1 for safety / alignment
    }

    [[nodiscard]] size_t expected_size() const noexcept {
        return size_t(max_threads_) * events_per_thread_;
    }
    void clear() {
        next_id_.store(0, std::memory_order_relaxed);
        // Remove any rows_.clear() if present
    }

    /// Attach a DoubleBufferedWriter (with any IEventSink: JTextEventSink, BinaryEventSink, or future SQL).
    /// Events will be submitted to the background writer after every successful save_event.
    /// This enables true double-buffered asynchronous persistence while keeping the hot path fast.
    void attach_persistence(std::unique_ptr<DoubleBufferedWriter> writer) {
        persistence_writer_ = std::move(writer);
    }

    /// Drain and finalize the background persistence worker (for tests that inspect sink output).
    void finalize_persistence() {
        if (persistence_writer_) {
            persistence_writer_->finalize();
        }
    }

    explicit ts_store(size_t max_threads, size_t events_per_thread)
        : max_threads_(max_threads)
        , events_per_thread_(events_per_thread)
    {
        if (max_threads == 0 || events_per_thread == 0)
            throw std::invalid_argument("ts_store: thread/event count must be > 0");

        // Memory check before allocating the rows (now fixed-size bounded_string storage inside).
        // The vector resize gives us the exact preallocated storage for all cat/payload buffers.
        const size_t N = expected_size();
        const size_t per_row = sizeof(row_data);
        // Note: v001 deliberately uses plain arithmetic here (no C++26 saturating ops).
        const size_t total_est = N * per_row + (16ULL << 20); // headroom

        const size_t avail = available_ram_bytes();
        if (avail > 0 && total_est > (avail * 90 / 100)) {
            std::cerr << "ts_store: insufficient memory for preallocation (need ~"
                      << (total_est >> 20) << " MiB, avail ~" << (avail >> 20) << " MiB) — bailing\n";
            std::exit(1);
        }

        rows_.resize(expected_size());
        // bounded_string members are inline fixed char arrays — no per-row .reserve needed.
        // Storage is fully preallocated by the above resize.

        if constexpr (Config::use_timestamps) {
            const auto min_time = std::chrono::steady_clock::time_point::min();
            if (auto cur = s_epoch_base.load(std::memory_order_relaxed); cur == min_time)
                s_epoch_base.store(std::chrono::steady_clock::now(), std::memory_order_relaxed);
        }
    }

private:
    static inline std::atomic<std::chrono::steady_clock::time_point> s_epoch_base{
        std::chrono::steady_clock::time_point::min()
    };
    std::atomic<size_t> next_id_{0};
    std::vector<row_data> rows_;

    std::unique_ptr<DoubleBufferedWriter> persistence_writer_;

public:
    static constexpr bool debug_mode_v = Config::debug_mode;
    #include "impl_details/core.hpp"

#include "impl_details/test_constants.hpp"
#include "impl_details/testing.hpp"
#include "impl_details/press_to_cont.hpp"
#include "impl_details/printing.hpp"
#include "impl_details/duration.hpp"
#include "impl_details/sorting.hpp"
#include "impl_details/verify_checks.hpp"
#include "impl_details/diagnostic.hpp"

};
#ifndef JAC313_STORE_IMPORT_STD
}; // namespace jac313::Store::v001
#endif

