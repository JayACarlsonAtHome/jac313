// ts_store/ts_store_headers/ts_store.hpp
// C++26 (uses std::mul_sat / std::add_sat) — GCC 15 / Clang 21
#pragma once

#include "includes.hpp"
#include "sat_compat.hpp"

#include "persistence/EventSink.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <span>
#include <thread>
#include <unistd.h>   // ::write(2, ...) — the persistence worker's fail-safe error line

// The rows reach the persistence worker through a release/acquire pair on row_data::ready, which
// helgrind/DRD do not model as happens-before. Builds with -DJAC313_HELGRIND_ANNOTATE tell them.
#if defined(JAC313_HELGRIND_ANNOTATE)
#  include <valgrind/helgrind.h>
#  define JAC313_HAPPENS_BEFORE(addr) ANNOTATE_HAPPENS_BEFORE(addr)
#  define JAC313_HAPPENS_AFTER(addr)  ANNOTATE_HAPPENS_AFTER(addr)
#else
#  define JAC313_HAPPENS_BEFORE(addr) ((void)0)
#  define JAC313_HAPPENS_AFTER(addr)  ((void)0)
#endif

#include <numeric>  // jac313::mul_sat / jac313::add_sat (C++26): overflow-safe capacity math
#include <fstream>  // for MemAvailable probe
#include <format>
#include <stdexcept>

namespace jac313::Store::v002 {
// Allocator for the preallocated row table. std::calloc hands back zero pages that the kernel
// maps lazily (large blocks come straight from mmap), so value-initializing the rows needs no
// writes: the no-argument construct() is a no-op because an all-zero row IS the value-initialized
// row. That moves ~N*sizeof(row) bytes of memset and every first-touch page fault off the
// constructor's single thread and into the writer threads, which touch their rows anyway.
template<typename T>
struct zeroed_row_allocator {
    using value_type = T;

    zeroed_row_allocator() noexcept = default;
    template<typename U>
    zeroed_row_allocator(const zeroed_row_allocator<U>&) noexcept {}

    [[nodiscard]] T* allocate(std::size_t n) {
        void* p = std::calloc(n, sizeof(T));
        if (!p) throw std::bad_alloc();
        return static_cast<T*>(p);
    }
    void deallocate(T* p, std::size_t) noexcept { std::free(p); }

    template<typename U>
    void construct(U*) noexcept {}
    template<typename U, typename... Args>
    void construct(U* p, Args&&... args) { ::new (static_cast<void*>(p)) U(std::forward<Args>(args)...); }

    friend bool operator==(const zeroed_row_allocator&, const zeroed_row_allocator&) noexcept { return true; }
};

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
        // The epoch in which save_event finished writing this row (0 = never). Its release store is
        // save_event's last statement; the persistence worker reads the row once it sees it.
        std::atomic<uint32_t> ready{};
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
        // Ids restart at 0. Once events went to attached persistence, the next ones would repeat
        // ids already written: SQLite's INSERT OR IGNORE drops them without a word, and the
        // file logs hold duplicates. A saved event is on its way to the sink even before the
        // worker has taken it, so any event saved since attach refuses, not only a persisted one.
        // Every attached worker is asked.
        JAC313_PRE(nothing_saved_since_attach());
        next_id_.store(0, std::memory_order_relaxed);
        // A new epoch: the rows of the last one no longer count as ready.
        const uint32_t e = epoch_.load(std::memory_order_relaxed) + 1;
        epoch_.store(e == 0 ? 1 : e, std::memory_order_relaxed);
        for (auto& w : persist_) w->restart_at_zero();
        // rows_ is intentionally NOT cleared — next_id_=0 is the logical reset; the fixed buffer is
        // reused (entries are overwritten on the next write), avoiding reallocation.
    }

    /// Persist the saved events through `sink` (JTextEventSink, BinaryEventSink, SqlEventSink, ...).
    /// Each call adds one worker thread for that sink; any number may be attached, before or
    /// during a run. A worker reads the rows in place, `batch_size` rows per sink call, at its own
    /// pace; save_event only marks its row ready (no copy, no lock). Each sink keeps the events its
    /// PersistMode takes, so attaching a KeeperOnly and a DatabaseOnly sink routes by flag. Each
    /// sink must own its own file or connection. Events saved from now on are persisted; a clear()
    /// right after attaching (nothing saved yet) restarts them at id 0. Returns the worker's index
    /// for the per-sink queries below. Owner thread only (like flush / finalize / clear).
    size_t attach_persistence(std::unique_ptr<IEventSink> sink, size_t batch_size = 10'000) {
        if (!sink) throw std::invalid_argument("ts_store::attach_persistence requires a non-null sink");
        persist_.push_back(std::make_unique<persist_worker>(*this, std::move(sink), batch_size,
                                                            next_id_.load(std::memory_order_relaxed)));
        return persist_.size() - 1;
    }

    /// Number of sinks attached (finalized ones included).
    [[nodiscard]] size_t persistence_count() const noexcept { return persist_.size(); }

    /// Every worker: write every row that is ready now, then flush its sink.
    void flush_persistence() {
        for (auto& w : persist_) w->flush();
    }

    /// Every worker: write every saved row, flush and finalize its sink, stop its thread. Call it
    /// after the producer threads joined: a row that is still not ready after 10 s is an error.
    /// Every worker is finalized even when an earlier one reports an error; the errors of all of
    /// them are thrown together afterwards as one std::runtime_error, one line per sink.
    void finalize_persistence() {
        std::string errors;
        for (size_t i = 0; i < persist_.size(); ++i) {
            try {
                persist_[i]->finalize();
            } catch (const std::exception& e) {
                if (!errors.empty()) errors += '\n';
                errors += std::format("sink {} ({}): {}", i, persist_[i]->sink_name(), e.what());
            }
        }
        if (!errors.empty()) throw std::runtime_error(errors);
    }

    /// True when any sink failed: the events since then were dropped for that sink.
    [[nodiscard]] bool persistence_failed() const noexcept {
        return std::ranges::any_of(persist_, [](const auto& w) { return w->failed(); });
    }

    /// True when sink `index` failed.
    [[nodiscard]] bool persistence_failed(size_t index) const { return worker(index).failed(); }

    /// Rows handed to every sink so far: the fewest any one worker has taken (0 with none attached).
    [[nodiscard]] size_t persisted_rows() const noexcept {
        if (persist_.empty()) return 0;
        size_t n = SIZE_MAX;
        for (const auto& w : persist_) n = std::min(n, w->persisted() - w->start_id());
        return n;
    }

    /// Rows handed to sink `index` so far.
    [[nodiscard]] size_t persisted_rows(size_t index) const {
        const auto& w = worker(index);
        return w.persisted() - w.start_id();
    }

    /// Microseconds from attaching sink `index` to its finalize returning; 0 until then.
    [[nodiscard]] uint64_t drain_time_us(size_t index) const { return worker(index).drain_time_us(); }

    /// IEventSink::name() of sink `index`.
    [[nodiscard]] std::string_view persistence_sink_name(size_t index) const { return worker(index).sink_name(); }

    /// Persist at most `n` rows: only events with global id < n are handed to the persistence
    /// writer (0 = no limit, the default). The in-memory store still takes every event, so the
    /// hot path is exercised at full size while a test or proof run writes a bounded dataset.
    /// Ids are assigned 0, 1, 2, ... so the persisted set is exactly ids [0, n) every run.
    void set_max_persist_rows(size_t n) noexcept { max_persist_rows_.store(n, std::memory_order_relaxed); }
    [[nodiscard]] size_t max_persist_rows() const noexcept { return max_persist_rows_.load(std::memory_order_relaxed); }

    /// Number of rows the attached persistence receives for a full run: min(events, limit).
    [[nodiscard]] size_t expected_persisted_rows() const noexcept {
        const size_t n = std::min(next_id_.load(std::memory_order_relaxed), expected_size());
        const size_t cap = max_persist_rows();
        return (cap == 0) ? n : std::min(n, cap);
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
        // Saturating math (C++26 nicety) so an overflowing estimate can't wrap small and slip past
        // the memory check below (which would then attempt a huge real allocation).
        // v001 uses plain arithmetic here because it must stay pre-C++26.
        const size_t total_est = jac313::add_sat(jac313::mul_sat(N, per_row), size_t(16ULL << 20)); // headroom

        // Best-available physical memory (bytes) for the preallocation precheck. Prefer the kernel's
        // MemAvailable, which is reclaim-aware (it counts reclaimable page cache). sysinfo() does NOT:
        // freeram+bufferram+sharedram excludes the page cache, so right after a build warms the cache it
        // under-reports by gigabytes and this precheck bails on an allocation the kernel could satisfy —
        // that false bail is what zeroed the recorded bench gate (→ "NOT SAFE"). Fall back to sysinfo
        // when /proc/meminfo can't be read (non-Linux, restricted /proc, etc.).
        // Uses std::ifstream.
        auto available_bytes = []() -> size_t {
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
        };
        size_t avail = available_bytes();
        if (avail > 0 && total_est > (avail * 90 / 100)) {
            // Thrown, not exit(1): the caller decides, and writers and destructors still run.
            throw std::runtime_error(std::format(
                "ts_store: insufficient memory for preallocation (need ~{} MiB, avail ~{} MiB)",
                total_est >> 20, avail >> 20));
        }

        // Built in place, not resized: row_data holds an atomic, so it cannot be moved.
        rows_ = decltype(rows_)(N);
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
    std::atomic<uint32_t> epoch_{1};   // a row is ready when its mark equals this; clear() bumps it
    // row_data's value-initialized state is all-zero bytes and it needs no destructor, which is
    // what lets zeroed_row_allocator skip construction (see above). An all-zero ready mark is
    // epoch 0, which is never current: a fresh row is not ready.
    static_assert(std::is_trivially_destructible_v<row_data>);
    static_assert(std::is_aggregate_v<row_data>);
    static_assert(std::is_trivially_destructible_v<std::atomic<uint32_t>>);
    static_assert(std::atomic<uint32_t>::is_always_lock_free);
    std::vector<row_data, zeroed_row_allocator<row_data>> rows_;

    std::atomic<size_t> max_persist_rows_{0};   // 0 = no limit (see set_max_persist_rows)

    #include "impl_details/persist_worker.hpp"
    // One worker per attached sink. Declared after rows_ (and everything they read), so every
    // worker is finalized and joined before the rows go.
    std::vector<std::unique_ptr<persist_worker>> persist_;

    [[nodiscard]] const persist_worker& worker(size_t index) const {
        if (index >= persist_.size())
            throw std::out_of_range(std::format("ts_store: no persistence sink {} ({} attached)", index, persist_.size()));
        return *persist_[index];
    }

    // clear()'s guard: no worker has taken a row, and nothing was saved since any of them attached.
    [[nodiscard]] bool nothing_saved_since_attach() const noexcept {
        const size_t next = next_id_.load(std::memory_order_relaxed);
        return std::ranges::all_of(persist_, [next](const auto& w) {
            return w->persisted() == w->start_id() && next == w->start_id();
        });
    }

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
}; // namespace jac313::Store::v002

