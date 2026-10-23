// ts_store/ts_store_headers/impl_details/sorting.hpp
// Optional advanced sorting — safe to include or exclude

#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>

inline std::vector<std::uint64_t> get_all_ids_sorted(int mode = 0) const
{
    // Storage is dense vector now; ids are 0-based indices.
    std::vector<size_t> ids;
    ids.reserve(rows_.size());
    for (size_t i = 0; i < rows_.size(); ++i)
        ids.push_back(i);

    if (mode == 1) {
        // Sort by thread_id
        std::sort(ids.begin(), ids.end(), [this](size_t a, size_t b) {
            return rows_[a].thread_id < rows_[b].thread_id;
        });
    }
    else if (mode == 2) {
        // Sort by payload string (lexicographic) - using bounded_string view
        std::sort(ids.begin(), ids.end(), [this](size_t a, size_t b) {
            return rows_[a].value_storage.view() < rows_[b].value_storage.view();
        });
    }
    // mode == 0 → already sorted by ID (chronological)

    std::vector<std::uint64_t> out(ids.begin(), ids.end());
    return out;
}

// Bonus: helper if you ever want sorted timestamps
inline std::vector<std::uint64_t> get_ids_sorted_by_timestamp() const
{
    if constexpr (!Config::use_timestamps) return {};
    std::vector<size_t> ids;
    ids.reserve(rows_.size());
    for (size_t i = 0; i < rows_.size(); ++i)
        if (rows_[i].ts_us != 0)
            ids.push_back(i);

    std::sort(ids.begin(), ids.end(), [this](size_t a, size_t b) {
        return rows_[a].ts_us < rows_[b].ts_us;
    });

    std::vector<std::uint64_t> out(ids.begin(), ids.end());
    return out;
}