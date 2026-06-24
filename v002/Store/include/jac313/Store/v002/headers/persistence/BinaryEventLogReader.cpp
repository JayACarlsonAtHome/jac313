#include "BinaryEventLogReader.hpp"
#include "../sat_compat.hpp"

#ifndef JAC313_STORE_IMPORT_STD
#include <cstring>
#include <stdexcept>
#include <span>
#include <numeric>  // jac313::add_sat / jac313::mul_sat (C++26): overflow-safe offset math
#endif

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v002 {
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
    const std::span<const char> buf{buffer.data(), buffer.size()};
    // Bounds-checked cursor: refuse a truncated/malformed record instead of reading
    // past the buffer (over-read hardening). add_sat/mul_sat keep the offset math
    // overflow-safe even for attacker-chosen lengths. (span::at-style checking,
    // generalized to the bulk memcpy reads.)
    auto have = [&](size_t n) noexcept { return jac313::add_sat(pos, n) <= buf.size(); };

    auto read_u64 = [&](uint64_t& dst) noexcept -> bool {
        if (!have(sizeof(uint64_t))) return false;
        std::memcpy(&dst, buf.data() + pos, sizeof(uint64_t));
        pos += sizeof(uint64_t);
        return true;
    };

    auto read_u16 = [&](uint16_t& dst) noexcept -> bool {
        if (!have(sizeof(uint16_t))) return false;
        std::memcpy(&dst, buf.data() + pos, sizeof(uint16_t));
        pos += sizeof(uint16_t);
        return true;
    };

    if (!(read_u64(out.event_id) && read_u64(out.thread_id) &&
          read_u64(out.per_thread_event_id) && read_u64(out.raw_flags) &&
          read_u64(out.timestamp_us))) { eof_reached_ = true; return false; }

    uint16_t cat_len = 0;
    if (!read_u16(cat_len) || !have(cat_len)) { eof_reached_ = true; return false; }
    out.category.assign(buf.data() + pos, cat_len);
    pos += cat_len;

    uint16_t pay_len = 0;
    if (!read_u16(pay_len) || !have(pay_len)) { eof_reached_ = true; return false; }
    out.payload.assign(buf.data() + pos, pay_len);
    pos += pay_len;

    uint16_t i_count = 0;
    if (!read_u16(i_count) ||
        !have(jac313::mul_sat<size_t>(i_count, sizeof(int64_t)))) { eof_reached_ = true; return false; }
    out.int_metrics.resize(i_count);
    if (i_count > 0) {
        std::memcpy(out.int_metrics.data(), buf.data() + pos, i_count * sizeof(int64_t));
        pos += i_count * sizeof(int64_t);
    }

    uint16_t d_count = 0;
    if (!read_u16(d_count) ||
        !have(jac313::mul_sat<size_t>(d_count, sizeof(double)))) { eof_reached_ = true; return false; }
    out.dbl_metrics.resize(d_count);
    if (d_count > 0) {
        std::memcpy(out.dbl_metrics.data(), buf.data() + pos, d_count * sizeof(double));
    }

    records_read_++;
    return true;
}

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v002
#endif