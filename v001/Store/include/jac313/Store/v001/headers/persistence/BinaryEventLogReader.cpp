#include "BinaryEventLogReader.hpp"

#ifndef JAC313_STORE_IMPORT_STD
#include <cstring>
#include <stdexcept>
#endif

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v001 {
#endif

BinaryEventLogReader::BinaryEventLogReader(std::string_view filepath)
    : filepath_(filepath)
{
    file_.open(filepath_, std::ios::binary | std::ios::in);
    if (!file_.is_open()) {
        throw std::runtime_error("BinaryEventLogReader: failed to open " + std::string(filepath));
    }
    skip_leading_file_header();
    data_start_ = file_.tellg();
}

void BinaryEventLogReader::skip_leading_file_header() {
    std::string line;
    while (true) {
        std::streampos before = file_.tellg();
        if (!std::getline(file_, line)) {
            file_.clear();
            file_.seekg(0, std::ios::beg);
            return;
        }
        if (!line.empty() && line[0] == '/' && line.size() > 1 && line[1] == '/') {
            continue;
        }
        file_.clear();
        file_.seekg(before);
        return;
    }
}

bool BinaryEventLogReader::next(BinaryRecord& out_record) {
    return read_next_record(out_record);
}

void BinaryEventLogReader::rewind() {
    file_.clear();
    file_.seekg(data_start_);
    records_read_ = 0;
    eof_reached_ = false;
}

bool BinaryEventLogReader::read_next_record(BinaryRecord& out) {
    if (eof_reached_) return false;

    uint32_t record_len = 0;
    file_.read(reinterpret_cast<char*>(&record_len), sizeof(record_len));

    if (file_.eof() || file_.fail()) {
        eof_reached_ = true;
        return false;
    }

    std::vector<char> buffer(record_len);
    file_.read(buffer.data(), static_cast<std::streamsize>(record_len));

    if (file_.fail()) {
        eof_reached_ = true;
        return false;
    }

    size_t pos = 0;
    auto read_u64 = [&](uint64_t& dst) {
        std::memcpy(&dst, &buffer[pos], sizeof(uint64_t));
        pos += sizeof(uint64_t);
    };

    auto read_u16 = [&](uint16_t& dst) {
        std::memcpy(&dst, &buffer[pos], sizeof(uint16_t));
        pos += sizeof(uint16_t);
    };

    read_u64(out.event_id);
    read_u64(out.thread_id);
    read_u64(out.per_thread_event_id);
    read_u64(out.raw_flags);
    read_u64(out.timestamp_us);

    uint16_t cat_len = 0;
    read_u16(cat_len);
    out.category.assign(&buffer[pos], cat_len);
    pos += cat_len;

    uint16_t pay_len = 0;
    read_u16(pay_len);
    out.payload.assign(&buffer[pos], pay_len);
    pos += pay_len;

    uint16_t i_count = 0;
    read_u16(i_count);
    out.int_metrics.resize(i_count);
    if (i_count > 0) {
        std::memcpy(out.int_metrics.data(), &buffer[pos], i_count * sizeof(int64_t));
        pos += i_count * sizeof(int64_t);
    }

    uint16_t d_count = 0;
    read_u16(d_count);
    out.dbl_metrics.resize(d_count);
    if (d_count > 0) {
        std::memcpy(out.dbl_metrics.data(), &buffer[pos], d_count * sizeof(double));
    }

    records_read_++;
    return true;
}

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v001
#endif