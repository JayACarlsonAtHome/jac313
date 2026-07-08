#pragma once

// JsonSplitEventLog
// NDJSON 3-file split persistence (main + _Ints + _Floats).

#ifndef JAC313_STORE_IMPORT_STD
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#endif

#include "PersistCommon.hpp"

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v002 {
#endif

struct JsonSplitEventLogStats {
    size_t main_rows = 0;
    size_t ints_rows = 0;
    size_t floats_rows = 0;
    size_t batches_flushed = 0;
};

class JsonSplitEventLog {
public:
    JsonSplitEventLog(std::string_view base_name,
                      size_t int_count,
                      size_t dbl_count,
                      PersistMode mode = PersistMode::All);

    ~JsonSplitEventLog();

    JsonSplitEventLog(JsonSplitEventLog&&) noexcept;
    JsonSplitEventLog& operator=(JsonSplitEventLog&&) noexcept;

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

    [[nodiscard]] const JsonSplitEventLogStats& stats() const;
    [[nodiscard]] const std::string& main_file() const;
    [[nodiscard]] const std::string& ints_file() const;
    [[nodiscard]] const std::string& floats_file() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v002
#endif