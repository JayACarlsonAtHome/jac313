#include "BinaryEventLogReader.hpp"
#include "../sat_compat.hpp"

#ifndef JAC313_STORE_IMPORT_STD
#include <cstring>
#include <stdexcept>
#include <span>
#include <numeric>  // jac313::add_sat / jac313::mul_sat (C++26): overflow-safe offset math
#include <string_view>
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
    // Best-practice safe skip for binary logs:
    // Read a bounded prefix (never let text parsing touch the binary payload).
    // Locate the end of the comment header by finding the last "//\n".
    // This fixes the original getline slurping arbitrarily large binary data
    // (no \n or late \n in records) which could stall or OOM the machine.
    constexpr size_t MAX_PREFIX = 4096;
    file_.seekg(0, std::ios::beg);

    std::vector<char> prefix(MAX_PREFIX);
    file_.read(prefix.data(), MAX_PREFIX);
    std::streamsize nread = file_.gcount();
    if (nread <= 0) {
        file_.clear();
        file_.seekg(0, std::ios::beg);
        data_start_ = file_.tellg();
        return;
    }

    std::string_view sv(prefix.data(), static_cast<size_t>(nread));
    size_t header_end = 0;

    // Robustly locate end of the standardized text header (written by BinaryEventLog
    // and jText paths). We must consume the *entire* header including the explicit
    // "// -- end text header..." marker line so the first uint32_t record_len we read
    // is real binary data, not ASCII from inside a comment line.
    // The previous rfind("//\n") only advanced to the *penultimate* header line, landing
    // inside the marker and causing bogus large record_len (>kMax) => next() refuse
    // => every write+read smoke/reader test returned failure (rc=1).
    size_t marker = sv.find("// -- end text header, binary records follow --");
    if (marker != std::string_view::npos) {
        size_t nl = sv.find('\n', marker);
        if (nl != std::string_view::npos) {
            header_end = nl + 1;
        } else {
            header_end = marker;
        }
    } else {
        // Fallback for legacy/hand-crafted .bin (e.g. malformed/huge cases in binary_reader_test)
        // or files without the end-marker line: skip past the last "//\n" line.
        size_t last = sv.rfind("//\n");
        if (last != std::string_view::npos) {
            size_t nl = sv.find('\n', last);
            header_end = (nl != std::string_view::npos ? nl + 1 : last + 3);
        }
    }

    file_.clear();
    file_.seekg(static_cast<std::streamoff>(header_end), std::ios::beg);
    data_start_ = file_.tellg();
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

    // Hard size limit prevents OOM / machine stall from corrupt or malicious
    // record_len (common after crashes, partial writes, or temp file collisions).
    constexpr size_t kMaxBinaryRecordLen = 128ULL * 1024 * 1024; // 128 MiB
    if (record_len == 0 || record_len > kMaxBinaryRecordLen) {
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