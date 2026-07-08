#pragma once

#ifndef JAC313_STORE_IMPORT_STD
#include <chrono>
#include <format>
#include <ostream>
#include <string_view>
#endif

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v001 {
#endif

enum class PersistMode { All, KeeperOnly, DatabaseOnly };

// Standardized // metadata header for all persist files (kept in sync with jText convention).
inline void write_file_comment_header(std::ostream& os,
                                      std::string_view full_path,
                                      std::string_view purpose,
                                      std::string_view related = {})
{
    auto now = std::chrono::system_clock::now();
    auto today = std::chrono::floor<std::chrono::days>(now);
    const std::string date_str = std::format("{:%Y-%m-%d}", today);

    os << "//File:    " << full_path << "\n";
    os << "//Date:    " << date_str << "\n";
    os << "//Purpose: " << purpose << "\n";
    if (!related.empty()) {
        os << "//Related: " << related << "\n";
    }
    os << "//\n";
}

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v001
#endif