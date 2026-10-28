#pragma once

#include <filesystem>
#include <chrono>
#include <format>
#include <ostream>
#include <string_view>

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

} // namespace jac313::Store::v001