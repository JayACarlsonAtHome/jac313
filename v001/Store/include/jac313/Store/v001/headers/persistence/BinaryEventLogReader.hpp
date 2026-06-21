#pragma once

// BinaryEventLogReader.hpp
// Reader for the fast binary format produced by BinaryEventLog.

#ifndef JAC313_STORE_IMPORT_STD
#include <cstdint>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#endif

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v001 {
#endif

struct BinaryRecord {
    uint64_t event_id = 0;
    uint64_t thread_id = 0;
    uint64_t per_thread_event_id = 0;
    uint64_t raw_flags = 0;
    uint64_t timestamp_us = 0;
    std::string category;
    std::string payload;
    std::vector<int64_t> int_metrics;
    std::vector<double> dbl_metrics;
};

class BinaryEventLogReader {
public:
    explicit BinaryEventLogReader(std::string_view filepath);

    bool next(BinaryRecord& out_record);
    void rewind();

    [[nodiscard]] size_t records_read() const { return records_read_; }

    // Implemented in jac313::store_jtext_persist (link that target to use).
    void convert_to_jtext(std::string_view output_base_name,
                          size_t int_count,
                          size_t dbl_count) const;

private:
    bool read_next_record(BinaryRecord& out);
    void skip_leading_file_header();

    std::ifstream file_;
    std::string filepath_;
    size_t records_read_ = 0;
    bool eof_reached_ = false;
    std::streampos data_start_ = 0;
};

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v001
#endif