#include "BinaryEventLogReader.hpp"

#ifndef JAC313_STORE_IMPORT_STD
#include <cstring>
#include <stdexcept>
#include <string_view>
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
    // Best-practice safe skip for binary logs (v001):
    // Bounded prefix read + rfind for "//\n". Prevents getline on binary data.
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

    // Hard size limit (v001) to prevent OOM on bad record_len.
    constexpr size_t kMaxBinaryRecordLen = 128ULL * 1024 * 1024;
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