// ts_store/ts_store_headers/impl_details/core.hpp
// Hot path: now uses bounded_string (fixed inline char buffer) instead of std::string
// for category/value_storage. Exact preallocation via vector<row_data>, direct
// assign_truncated (memcpy after utf8 cp count), memory check + bail at ctor.
// Drops std::string overhead in the critical in-memory save_event path.
// NO namespace — this file is included inside ts_store class

inline std::pair<bool, size_t>
save_event(size_t thread_id,
           size_t event_id,
           Config::ValueT value,
           size_t event_flag_param = 0,
           Config::CategoryT category = {},
           bool debug = false,
           std::array<int64_t, Config::the_IntMetrics> int_metrics = {},
           std::array<double,  Config::the_DblMetrics> dbl_metrics = {}
)
{
    const size_t id = next_id_.fetch_add(1, std::memory_order_relaxed);

    // Direct reference into the pre-sized + pre-reserved row slot.
    // Strings already have capacity reserved at construction; we write straight into them (no per-event alloc).
    auto& row = rows_[id];
    row.thread_id = thread_id;
    row.event_id  = event_id;
    row.is_debug  = debug;

    // Direct write into fixed bounded buffer (no std::string, no alloc, memcpy under the hood).
    row.value_storage.assign_truncated(std::string_view(value), Config::max_payload_length);
    row.category_storage.assign_truncated(std::string_view(category), Config::max_category_length);

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

    if (persistence_writer_) {
        const auto& stored = rows_[id];

        PersistedEvent pe;
        pe.event_id             = id;                    // global stable ID (good linking key)
        pe.thread_id            = stored.thread_id;
        pe.per_thread_event_id  = stored.event_id;       // the caller's per-thread id
        pe.flags                = stored.event_flags;
        pe.category             = stored.category_storage.str();  // copy out to PersistedEvent's std::string (persist path only)
        pe.payload              = stored.value_storage.str();

        if constexpr (Config::use_timestamps) {
            pe.timestamp_us = stored.ts_us;
        }

        // Copy metrics (small fixed arrays)
        pe.int_metrics.assign(stored.int_metrics.begin(), stored.int_metrics.end());
        pe.dbl_metrics.assign(stored.dbl_metrics.begin(), stored.dbl_metrics.end());

        persistence_writer_->submit_event(std::move(pe));
    }

    return {true, id};
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