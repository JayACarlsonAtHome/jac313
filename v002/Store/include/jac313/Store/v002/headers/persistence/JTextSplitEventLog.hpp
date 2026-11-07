#pragma once

// JTextSplitEventLog
// Human-readable 3-file split persistence (main + _Ints + _Floats) with early headers.
// 
// This file is only compiled when TS_STORE_ENABLE_JTEXT_PERSIST=ON via CMake.
// There are no preprocessor conditionals inside this translation unit.

#include <string>
#include <string_view>
#include <span>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "PersistCommon.hpp"

namespace jac313::Store::v002 {

/// The longest category and payload a log's main rows carry, in bytes. 0 = unknown: the main
/// writer's row buffer is then not reserved up front (it grows to the rows written instead).
struct JTextRowLimits {
    size_t max_category_bytes = 0;
    size_t max_payload_bytes = 0;
};

/// The row limits of a store Config: its bounded category and payload sizes
/// (bounded_string::max_bytes, an upper bound on the bytes either can hold).
template<typename Config>
constexpr JTextRowLimits jtext_row_limits_for() noexcept {
    return {Config::CategoryT::max_bytes, Config::ValueT::max_bytes};
}

/// The bytes of the longest one-line row a jText log writes, so each writer's row buffer can be
/// reserved once and never grow. A row is "<pad>1. <sep><comment> f1<sep>f2...<sep>fN\n":
/// " 1. " (field 1, padded to width 2), the two formatter characters (U+25AE and U+258C, three
/// bytes each in UTF-8; the ASCII fallback formatters are one byte), a space, the fields with a
/// separator between each two, and the newline. The logs write no row comments.
/// A field holding a newline is written as a multiline record ([* ... *]), which can be longer:
/// the writer's buffer then simply grows for that row (it never overflows).
namespace jtext_row_bytes {
    inline constexpr size_t kLineNumber    = 4;    // " 1. "
    inline constexpr size_t kFormatterChar = 3;    // U+25AE / U+258C in UTF-8
    inline constexpr size_t kInteger       = 20;   // any uint64 (20 digits) or int64 (19 digits + sign)
    inline constexpr size_t kFlags         = 18;   // "0x" + 16 hex digits
    inline constexpr size_t kDouble        = kDoubleTextChars;

    /// A row of `field_count` fields holding `field_bytes` bytes of text in all.
    constexpr size_t row(size_t field_count, size_t field_bytes) noexcept {
        const size_t separators = field_count > 0 ? field_count - 1 : 0;
        return kLineNumber + 2 * kFormatterChar + 1 + field_bytes + separators * kFormatterChar + 1;
    }

    /// The main row: event id, thread id, per-thread id, flags, category, payload, timestamp.
    /// 0 when either limit is unknown.
    constexpr size_t main_row(const JTextRowLimits& limits) noexcept {
        if (limits.max_category_bytes == 0 || limits.max_payload_bytes == 0) return 0;
        return row(7, 4 * kInteger + kFlags + limits.max_category_bytes + limits.max_payload_bytes);
    }

    /// An _Ints row: the event id and `int_count` int64 values.
    constexpr size_t ints_row(size_t int_count) noexcept { return row(1 + int_count, (1 + int_count) * kInteger); }

    /// A _Floats row: the event id and `dbl_count` doubles.
    constexpr size_t floats_row(size_t dbl_count) noexcept { return row(1 + dbl_count, kInteger + dbl_count * kDouble); }
} // namespace jtext_row_bytes

struct JTextSplitEventLogStats {
    size_t main_rows = 0;
    size_t ints_rows = 0;
    size_t floats_rows = 0;
    size_t batches_flushed = 0;
};

class JTextSplitEventLog {
public:
    JTextSplitEventLog(std::string_view base_name,
                       size_t int_count,
                       size_t dbl_count,
                       PersistMode mode = PersistMode::All,
                       JTextRowLimits limits = {});

    ~JTextSplitEventLog();

    JTextSplitEventLog(JTextSplitEventLog&&) noexcept;
    JTextSplitEventLog& operator=(JTextSplitEventLog&&) noexcept;

    /// One event: a row in each file. The text and metrics are only read during the call; the
    /// numbers are formatted into buffers the log owns and every field goes to the writers as
    /// a view, so a row makes no string and (after construction) no allocation.
    void append_event(size_t event_id,
                      size_t thread_id,
                      size_t per_thread_event_id,
                      uint64_t raw_flags,
                      std::string_view category,
                      std::string_view payload,
                      uint64_t timestamp_us,
                      std::span<const int64_t> int_metrics,
                      std::span<const double> dbl_metrics);

    void flush();
    void finalize();

    [[nodiscard]] const JTextSplitEventLogStats& stats() const;
    [[nodiscard]] const std::string& main_file() const;
    [[nodiscard]] const std::string& ints_file() const;
    [[nodiscard]] const std::string& floats_file() const;

    /// The capacity of the main writer's row buffer (JTextWriter::batch_capacity), for tests.
    [[nodiscard]] size_t main_batch_capacity() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    void write_sql_companions(std::string_view base_name, size_t int_count, size_t dbl_count);
    void write_all_headers(std::string_view base_name, size_t int_count, size_t dbl_count);
};

} // namespace jac313::Store::v002
