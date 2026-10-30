#pragma once

#include <filesystem>
#include <chrono>
#include <format>
#include <fstream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include <fcntl.h>
#include <unistd.h>

namespace jac313::Store::v001 {

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

} // namespace jac313::Store::v001