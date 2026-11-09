// ts_store/ts_store_headers/impl_details/core.hpp
// Hot path: now uses bounded_string (fixed inline char buffer) instead of std::string
// for category/value_storage. Exact preallocation via vector<row_data>, direct
// assign_truncated (memcpy after utf8 cp count), memory check + bail at ctor.
// Drops std::string overhead in the critical in-memory save_event path.
// NO namespace — this file is included inside ts_store class

// Take n consecutive ids. The caller writes every one, in order, before the thread
// exits. A hole stays unready and the persistence worker waits on it.
inline size_t claim_ids(size_t n) noexcept {
    return next_id_.fetch_add(n, std::memory_order_relaxed);
}

inline std::pair<bool, size_t>
save_claimed(size_t id,
             size_t thread_id,
             size_t event_id,
             std::string_view value,
             size_t event_flag_param = 0,
             std::string_view category = {},
             bool debug = false,
             std::array<int64_t, Config::the_IntMetrics> int_metrics = {},
             std::array<double,  Config::the_DblMetrics> dbl_metrics = {}
)
{
    // More events than max_threads x events_per_thread would write past the preallocated rows.
    JAC313_ASSERT(id < rows_.size());

    // Direct reference into the pre-sized + pre-reserved row slot.
    // Strings already have capacity reserved at construction; we write straight into them (no per-event alloc).
    auto& row = rows_[id];
    row.thread_id = thread_id;
    row.event_id  = event_id;
    row.is_debug  = debug;

    // Direct write into fixed bounded buffer (no std::string, no alloc, memcpy under the hood).
    row.value_storage.assign_truncated(value, Config::max_payload_length);
    row.category_storage.assign_truncated(category, Config::max_category_length);

    row.int_metrics = std::move(int_metrics);
    row.dbl_metrics = std::move(dbl_metrics);

    if (!row.value_storage.empty()) {
        event_flag_param = flags_set_has_data(event_flag_param);
    } else {
        event_flag_param = flags_clear_has_data(event_flag_param);
    }

    // Set metric presence flags (simple non-zero heuristic; callers can also set the flags explicitly)
    if constexpr (Config::the_IntMetrics > 0) {
        bool has = false;
        for (auto v : row.int_metrics) { if (v != 0) { has = true; break; } }
        if (has) event_flag_param = set_metric_flag(event_flag_param, StoreFlags::MetricFlag::HasIntData);
    }
    if constexpr (Config::the_DblMetrics > 0) {
        bool has = false;
        for (auto v : row.dbl_metrics) { if (v != 0.0) { has = true; break; } }
        if (has) event_flag_param = set_metric_flag(event_flag_param, StoreFlags::MetricFlag::HasDblData);
    }

    row.event_flags = event_flag_param;

    // — TIMESTAMP —
    if constexpr (Config::use_timestamps) {
        const auto now = std::chrono::steady_clock::now();
        auto base = s_epoch_base.load(std::memory_order_relaxed);
        if (base == std::chrono::steady_clock::time_point::min()) {
            auto expected = std::chrono::steady_clock::time_point::min();
            if (s_epoch_base.compare_exchange_strong(expected, now)) {
                base = now;
            } else {
                base = s_epoch_base.load(std::memory_order_relaxed);
            }
        }
        row.ts_us = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(now - base).count());
    }

    // (no rows_[id] = move; we wrote directly into the slot)

    // Last: publish the row. The persistence worker reads it in place once it sees this mark.
    JAC313_HAPPENS_BEFORE(&row.ready);
    row.ready.store(epoch_.load(std::memory_order_relaxed), std::memory_order_release);
    return {true, id};
}

inline std::pair<bool, size_t>
save_event(size_t thread_id,
           size_t event_id,
           std::string_view value,
           size_t event_flag_param = 0,
           std::string_view category = {},
           bool debug = false,
           std::array<int64_t, Config::the_IntMetrics> int_metrics = {},
           std::array<double,  Config::the_DblMetrics> dbl_metrics = {}
)
{
    return save_claimed(claim_ids(1), thread_id, event_id, value, event_flag_param, category, debug,
                        std::move(int_metrics), std::move(dbl_metrics));
}

// fill_persisted_event() — owning copy of row `id` for readers and read-back, written into `pe`.
// Assigns into pe's strings and vectors, so a reused PersistedEvent keeps its capacity.
inline void fill_persisted_event(size_t id, PersistedEvent& pe) const
{
    const auto& stored = rows_[id];
    pe.event_id             = id;                    // global stable ID (good linking key)
    pe.thread_id            = stored.thread_id;
    pe.per_thread_event_id  = stored.event_id;       // the caller's per-thread id
    pe.flags                = stored.event_flags;
    pe.category.assign(stored.category_storage.view());
    pe.payload.assign(stored.value_storage.view());
    if constexpr (Config::use_timestamps) {
        pe.timestamp_us = stored.ts_us;
    } else {
        pe.timestamp_us = 0;
    }
    pe.int_metrics.assign(stored.int_metrics.begin(), stored.int_metrics.end());
    pe.dbl_metrics.assign(stored.dbl_metrics.begin(), stored.dbl_metrics.end());
}

// persisted_event_view() — the view the persistence worker hands a sink. category, payload, and
// the metric spans point at this row. Rows are not overwritten during a run, so the view stays
// valid until write_batch returns. Precondition: id < rows_.size() and the row has been written.
inline PersistedEventView persisted_event_view(size_t id) const
{
    JAC313_PRE(id < rows_.size());
    const auto& stored = rows_[id];
    PersistedEventView v;
    v.event_id            = id;
    v.thread_id           = stored.thread_id;
    v.per_thread_event_id = stored.event_id;
    v.flags               = stored.event_flags;
    v.category            = stored.category_storage.view();
    v.payload             = stored.value_storage.view();
    if constexpr (Config::use_timestamps) {
        v.timestamp_us = stored.ts_us;
    } else {
        v.timestamp_us = 0;
    }
    v.int_metrics = stored.int_metrics;
    v.dbl_metrics = stored.dbl_metrics;
    return v;
}

// persisted_event() — owning copy of row `id`. Read-back compares what a backend stored against this.
// Precondition: id < rows_.size() and the row has been written.
inline PersistedEvent persisted_event(size_t id) const
{
    JAC313_PRE(id < rows_.size());
    PersistedEvent pe;
    fill_persisted_event(id, pe);
    return pe;
}

// select() — returns string_view into stored bounded_string
inline auto select(size_t id) const
{
    if (id >= rows_.size()) {
        return std::pair<bool, std::string_view>{false, {}};
    }
    return std::pair{true, rows_[id].value_storage.view()};
}

// get_all_ids — return 0..N-1, the dense slot indices (storage is vector<row_data>, id == index).
inline std::vector<size_t> get_all_ids() const
{
    std::vector<size_t> ids;
    ids.reserve(rows_.size());
    for (size_t i = 0; i < rows_.size(); ++i) {
        ids.push_back(i);
    }
    return ids;
}

// get_timestamp_us
inline std::pair<bool, size_t> get_timestamp_us(size_t id) const
{
    if constexpr (!Config::use_timestamps) {
        return {false, 0};
    }
    if (id >= rows_.size()) {
        return {false, 0};
    }
    return {true, rows_[id].ts_us};
}