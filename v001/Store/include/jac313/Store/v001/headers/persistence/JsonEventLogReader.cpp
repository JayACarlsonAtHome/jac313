#include "JsonEventLogReader.hpp"

#ifndef JAC313_STORE_IMPORT_STD
#include <charconv>
#include <cstdlib>
#include <format>
#include <optional>
#include <stdexcept>
#endif

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v001 {
#endif

namespace {

constexpr size_t kMaxJsonLineLen = 16ULL * 1024 * 1024;

bool starts_with(std::string_view s, std::string_view prefix) {
    return s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix;
}

std::optional<size_t> find_key(std::string_view line, std::string_view key) {
    const std::string needle = std::format(R"("{}":)", key);
    const size_t pos = line.find(needle);
    if (pos == std::string_view::npos) return std::nullopt;
    return pos + needle.size();
}

bool parse_u64_at(std::string_view line, size_t pos, uint64_t& out) {
    while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) ++pos;
    const std::string_view tail = line.substr(pos);
    const auto [ptr, ec] = std::from_chars(tail.data(), tail.data() + tail.size(), out);
    return ec == std::errc{} && ptr != tail.data();
}

bool parse_i64_at(std::string_view line, size_t pos, int64_t& out) {
    while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) ++pos;
    const std::string_view tail = line.substr(pos);
    const auto [ptr, ec] = std::from_chars(tail.data(), tail.data() + tail.size(), out);
    return ec == std::errc{} && ptr != tail.data();
}

bool parse_double_at(std::string_view line, size_t pos, double& out) {
    while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) ++pos;
    const std::string_view tail = line.substr(pos);
    char* end = nullptr;
    out = std::strtod(tail.data(), &end);
    return end != tail.data();
}

bool parse_quoted_string_at(std::string_view line, size_t pos, std::string& out) {
    while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) ++pos;
    if (pos >= line.size() || line[pos] != '"') return false;
    ++pos;

    out.clear();
    while (pos < line.size()) {
        const char c = line[pos++];
        if (c == '"') return true;
        if (c != '\\') {
            out.push_back(c);
            continue;
        }
        if (pos >= line.size()) return false;
        const char esc = line[pos++];
        switch (esc) {
            case '"':  out.push_back('"');  break;
            case '\\': out.push_back('\\'); break;
            case '/':  out.push_back('/');  break;
            case 'b':  out.push_back('\b'); break;
            case 'f':  out.push_back('\f'); break;
            case 'n':  out.push_back('\n'); break;
            case 'r':  out.push_back('\r'); break;
            case 't':  out.push_back('\t'); break;
            case 'u': {
                if (pos + 4 > line.size()) return false;
                const std::string_view hex = line.substr(pos, 4);
                unsigned int cp = 0;
                const auto [hptr, hec] = std::from_chars(hex.data(), hex.data() + 4, cp, 16);
                if (hec != std::errc{} || hptr != hex.data() + 4) return false;
                pos += 4;
                if (cp <= 0x7F) {
                    out.push_back(static_cast<char>(cp));
                } else {
                    out += std::format("\\u{:04x}", cp);
                }
                break;
            }
            default: return false;
        }
    }
    return false;
}

bool parse_hex_flags(std::string_view value, uint64_t& out) {
    if (!starts_with(value, "0x") && !starts_with(value, "0X")) return false;
    value.remove_prefix(2);
    out = 0;
    const auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), out, 16);
    return ec == std::errc{} && ptr != value.data();
}

} // namespace

JsonEventLogReader::JsonEventLogReader(std::string_view filepath)
    : filepath_(filepath)
{
    if (filepath_.ends_with("_Ints.json")) {
        kind_ = FileKind::Ints;
    } else if (filepath_.ends_with("_Floats.json")) {
        kind_ = FileKind::Floats;
    } else {
        kind_ = FileKind::Main;
    }

    file_.open(filepath_, std::ios::in);
    if (!file_.is_open()) {
        throw std::runtime_error("JsonEventLogReader: failed to open " + std::string(filepath));
    }
    skip_leading_file_header();
    data_start_ = file_.tellg();
}

void JsonEventLogReader::skip_leading_file_header() {
    std::string line;
    while (std::getline(file_, line)) {
        if (line.empty() || line[0] != '/') {
            if (!line.empty()) {
                file_.seekg(-static_cast<std::streamoff>(line.size() + 1), std::ios::cur);
            }
            break;
        }
    }
}

bool JsonEventLogReader::next(JsonRecord& out_record) {
    return read_next_record(out_record);
}

void JsonEventLogReader::rewind() {
    file_.clear();
    file_.seekg(data_start_);
    records_read_ = 0;
    eof_reached_ = false;
}

bool JsonEventLogReader::read_next_record(JsonRecord& out) {
    if (eof_reached_) return false;

    std::string line;
    while (std::getline(file_, line)) {
        if (line.empty()) continue;
        if (line[0] == '/') continue;
        if (line.size() > kMaxJsonLineLen) {
            eof_reached_ = true;
            return false;
        }
        ++records_read_;
        if (kind_ == FileKind::Main) return parse_main_line(line, out);
        if (kind_ == FileKind::Ints) return parse_ints_line(line, out);
        return parse_floats_line(line, out);
    }

    eof_reached_ = true;
    return false;
}

bool JsonEventLogReader::parse_main_line(std::string_view line, JsonRecord& out) {
    if (auto pos = find_key(line, "id")) {
        if (!parse_u64_at(line, *pos, out.event_id)) return false;
    } else return false;

    if (auto pos = find_key(line, "thread_id")) {
        if (!parse_u64_at(line, *pos, out.thread_id)) return false;
    } else return false;

    if (auto pos = find_key(line, "per_thread_event_id")) {
        if (!parse_u64_at(line, *pos, out.per_thread_event_id)) return false;
    } else return false;

    if (auto pos = find_key(line, "flags_raw")) {
        std::string flags_text;
        if (!parse_quoted_string_at(line, *pos, flags_text)) return false;
        if (!parse_hex_flags(flags_text, out.raw_flags)) return false;
    } else return false;

    if (auto pos = find_key(line, "category")) {
        if (!parse_quoted_string_at(line, *pos, out.category)) return false;
    } else return false;

    if (auto pos = find_key(line, "payload")) {
        if (!parse_quoted_string_at(line, *pos, out.payload)) return false;
    } else return false;

    if (auto pos = find_key(line, "timestamp_us")) {
        if (!parse_u64_at(line, *pos, out.timestamp_us)) return false;
    } else return false;

    return true;
}

bool JsonEventLogReader::parse_ints_line(std::string_view line, JsonRecord& out) {
    out.int_metrics.clear();
    if (auto pos = find_key(line, "id")) {
        if (!parse_u64_at(line, *pos, out.event_id)) return false;
    } else return false;

    for (size_t k = 0; k < 64; ++k) {
        const std::string key = std::format("int{}", k);
        const auto pos = find_key(line, key);
        if (!pos) break;
        int64_t v = 0;
        if (!parse_i64_at(line, *pos, v)) return false;
        out.int_metrics.push_back(v);
    }
    return !out.int_metrics.empty();
}

bool JsonEventLogReader::parse_floats_line(std::string_view line, JsonRecord& out) {
    out.dbl_metrics.clear();
    if (auto pos = find_key(line, "id")) {
        if (!parse_u64_at(line, *pos, out.event_id)) return false;
    } else return false;

    for (size_t k = 0; k < 64; ++k) {
        const std::string key = std::format("dbl{}", k);
        const auto pos = find_key(line, key);
        if (!pos) break;
        double v = 0.0;
        if (!parse_double_at(line, *pos, v)) return false;
        out.dbl_metrics.push_back(v);
    }
    return !out.dbl_metrics.empty();
}

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v001
#endif