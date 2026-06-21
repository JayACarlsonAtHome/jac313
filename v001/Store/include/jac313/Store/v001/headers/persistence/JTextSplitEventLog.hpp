#pragma once

// JTextSplitEventLog
// Human-readable 3-file split persistence (main + _Ints + _Floats) with early headers.
// 
// This file is only compiled when TS_STORE_ENABLE_JTEXT_PERSIST=ON via CMake.
// There are no preprocessor conditionals inside this translation unit.

#ifndef JAC313_STORE_IMPORT_STD
#include <string>
#include <string_view>
#include <vector>
#include <cstdint>
#include <memory>
#endif

#include "PersistCommon.hpp"

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v001 {
#endif

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
                       PersistMode mode = PersistMode::All);

    ~JTextSplitEventLog();

    JTextSplitEventLog(JTextSplitEventLog&&) noexcept;
    JTextSplitEventLog& operator=(JTextSplitEventLog&&) noexcept;

    void append_event(size_t event_id,
                      size_t thread_id,
                      size_t per_thread_event_id,
                      uint64_t raw_flags,
                      std::string_view category,
                      std::string_view payload,
                      uint64_t timestamp_us,
                      const std::vector<int64_t>& int_metrics,
                      const std::vector<double>& dbl_metrics);

    void flush();
    void finalize();

    [[nodiscard]] const JTextSplitEventLogStats& stats() const;
    [[nodiscard]] const std::string& main_file() const;
    [[nodiscard]] const std::string& ints_file() const;
    [[nodiscard]] const std::string& floats_file() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    void write_sql_companions(std::string_view base_name, size_t int_count, size_t dbl_count);
    void write_all_headers(std::string_view base_name, size_t int_count, size_t dbl_count);
};

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v001
#endif
