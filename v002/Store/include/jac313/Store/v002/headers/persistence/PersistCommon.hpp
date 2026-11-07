#pragma once

#include <algorithm>
#include <charconv>
#include <cmath>
#include <filesystem>
#include <chrono>
#include <format>
#include <fstream>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include <fcntl.h>
#include <locale.h>   // newlocale (parse_double_text)
#include <stdlib.h>   // strtod_l (parse_double_text)
#include <unistd.h>

namespace jac313::Store::v002 {

enum class PersistMode { All, KeeperOnly, DatabaseOnly };

// Standardized // metadata header for all persist files (kept in sync with jText's
// write_file_comment_header; the sinks that do not link jText use this one).
// //File: holds the file's name only: a path goes stale when the file or its project moves.
inline void write_file_comment_header(std::ostream& os,
                                      std::string_view path,
                                      std::string_view purpose,
                                      std::string_view related = {})
{
    auto now = std::chrono::system_clock::now();
    auto today = std::chrono::floor<std::chrono::days>(now);
    const std::string date_str = std::format("{:%Y-%m-%d}", today);

    os << "//File:    " << std::filesystem::path(std::string(path)).filename().string() << "\n";
    os << "//Date:    " << date_str << "\n";
    os << "//Purpose: " << purpose << "\n";
    if (!related.empty()) {
        os << "//Related: " << related << "\n";
    }
    os << "//\n";
}

// A table or type name made from a file name: lower-case letters, digits and '_' only, so every
// SQL sink (SQLite and PostgreSQL alike) can paste it into its statements.
inline std::string sanitize_identifier(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c >= 'A' && c <= 'Z') out += static_cast<char>(c - 'A' + 'a');
        else if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) out += c;
        else if (out.empty() || out.back() != '_') out += '_';
    }
    while (!out.empty() && out.back() == '_') out.pop_back();
    if (out.empty() || (out[0] >= '0' && out[0] <= '9')) out.insert(0, "t_");
    return out;
}

// A double as text, exact: finite values in the shortest form that reads back to the same
// bits (std::to_chars), the non-finite ones spelled NaN, Infinity and -Infinity. Every text
// writer (jText, HTML, JSON, the PostgreSQL COPY stream) and read-back's row hash use this, so
// a double written by any backend reads back as the double that was stored. NaN and the
// infinities are data, not errors. double_to_chars is the one source of that text; append_double
// appends it to a string.
//
// kDoubleTextChars is the longest such text: a sign, 17 significant digits, the point and
// "e-308" (e.g. -2.2250738585072014e-308), longer than "-Infinity".
inline constexpr size_t kDoubleTextChars = 24;

// Writes v's text at `first` like std::to_chars and returns the end of it. [first, last) must
// hold kDoubleTextChars bytes, which every double's text fits.
inline char* double_to_chars(char* first, char* last, double v) {
    const auto copy = [first](std::string_view text) { return std::copy(text.begin(), text.end(), first); };
    if (std::isnan(v)) return copy("NaN");
    if (std::isinf(v)) return copy(v > 0 ? "Infinity" : "-Infinity");
    return std::to_chars(first, last, v).ptr;
}

inline void append_double(std::string& out, double v) {
    char buf[kDoubleTextChars];
    out.append(buf, double_to_chars(buf, buf + sizeof(buf), v));
}

// The reverse of append_double: the whole of `s` must be one double (to_chars' form, any
// exact decimal, or NaN / Infinity / -Infinity). Returns false for anything else.
inline bool parse_double_text(std::string_view s, double& out) {
    if (s == "NaN")       { out = std::numeric_limits<double>::quiet_NaN(); return true; }
    if (s == "Infinity")  { out = std::numeric_limits<double>::infinity(); return true; }
    if (s == "-Infinity") { out = -std::numeric_limits<double>::infinity(); return true; }
    if (s.empty()) return false;
    auto [p, ec] = std::from_chars(s.data(), s.data() + s.size(), out);
    if (ec == std::errc::result_out_of_range && p == s.data() + s.size()) {
        // libstdc++ before GCC 12 (RHEL 9's system libstdc++ 11, which gcc-toolset and clang
        // link against there) calls a subnormal such as 5e-324 out of range and returns 0.
        // from_chars took the whole text as a number, so read it again with strtod in the C
        // locale: glibc rounds it exactly. Overflow and underflow to zero stay refused.
        static const locale_t c_locale = ::newlocale(LC_ALL_MASK, "C", locale_t{});
        if (c_locale == locale_t{}) return false;
        const std::string z(s);
        char* end = nullptr;
        const double d = ::strtod_l(z.c_str(), &end, c_locale);
        if (end != z.c_str() + z.size() || !std::isfinite(d) || d == 0.0) return false;
        out = d;
        return true;
    }
    // from_chars also takes "nan" / "inf": only the three spellings above are ours.
    return ec == std::errc{} && p == s.data() + s.size() && std::isfinite(out);
}

// Flush a text log to disk and fsync it, and throw when either failed: a full disk or an I/O
// error must not leave a truncated file behind stats that say the run succeeded.
inline void sync_text_file(std::ofstream& ofs, const std::string& path) {
    if (path.empty()) return;
    ofs.flush();
    if (ofs.fail()) throw std::runtime_error("persist: write failed: " + path);
    const int fd = ::open(path.c_str(), O_RDONLY);
    if (fd < 0) throw std::runtime_error("persist: cannot open for fsync: " + path);
    const int rc = ::fsync(fd);
    ::close(fd);
    if (rc != 0) throw std::runtime_error("persist: fsync failed: " + path);
}

} // namespace
