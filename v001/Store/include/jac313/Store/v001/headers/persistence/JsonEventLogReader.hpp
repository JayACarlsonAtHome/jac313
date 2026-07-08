#pragma once

// JsonEventLogReader.hpp
// Reader for the NDJSON split format produced by JsonSplitEventLog.

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

struct JsonRecord {
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

class JsonEventLogReader {
public:
    explicit JsonEventLogReader(std::string_view filepath);

    bool next(JsonRecord& out_record);
    void rewind();

    [[nodiscard]] size_t records_read() const { return records_read_; }

private:
    bool read_next_record(JsonRecord& out);
    void skip_leading_file_header();
    static bool parse_main_line(std::string_view line, JsonRecord& out);
    static bool parse_ints_line(std::string_view line, JsonRecord& out);
    static bool parse_floats_line(std::string_view line, JsonRecord& out);

    std::ifstream file_;
    std::string filepath_;
    size_t records_read_ = 0;
    bool eof_reached_ = false;
    std::streampos data_start_ = 0;
    enum class FileKind { Main, Ints, Floats } kind_ = FileKind::Main;
};

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v001
#endif