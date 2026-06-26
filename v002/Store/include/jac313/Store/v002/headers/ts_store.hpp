// ts_store/ts_store_headers/ts_store.hpp
// C++26 (uses std::mul_sat / std::add_sat) — GCC 15 / Clang 21
#pragma once

#include "includes.hpp"
#include "sat_compat.hpp"

#include "persistence/DoubleBufferedWriter.hpp"

#ifndef JAC313_STORE_IMPORT_STD
#include <numeric>  // jac313::mul_sat / jac313::add_sat (C++26): overflow-safe capacity math
#endif               // under import std, <numeric> comes from the std module; a textual
                     // include here lands inside the module's export namespace and breaks.

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v002 {
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
        // Saturating multiply (C++26): on overflow returns SIZE_MAX rather than
        // wrapping, so the constructor's memory guard cannot be fooled into
        // under-allocating for an impossibly large (threads x events) request.
        return jac313::mul_sat(max_threads_, events_per_thread_);
    }
    void clear() {
        next_id_.store(0, std::memory_order_relaxed);
        // rows_ is intentionally NOT cleared — next_id_=0 is the logical reset; the fixed buffer is
        // reused (entries are overwritten on the next write), avoiding reallocation.
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
        // Saturating math so an overflowing estimate can't wrap small and slip past
        // the memory check below (which would then attempt a huge real allocation).
        const size_t total_est = jac313::add_sat(jac313::mul_sat(N, per_row), size_t(16ULL << 20)); // headroom

        struct sysinfo info{};
        size_t avail = 0;
        if (sysinfo(&info) == 0) {
            avail = info.freeram + info.bufferram + info.sharedram;
        }
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
}; // namespace jac313::Store::v002
#endif

