#pragma once

// Harness tree label (v001, v002, v003, ...) from the jac313 source root directory name.

#include <cctype>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace jac313 {

inline std::filesystem::path detect_harness_source_root(std::filesystem::path start) {
    namespace fs = std::filesystem;
    std::error_code ec;
    for (fs::path dir = fs::absolute(start, ec); !ec && !dir.empty(); dir = dir.parent_path()) {
        if (fs::exists(dir / "CMakeLists.txt", ec) && fs::exists(dir / "Store", ec)
            && fs::exists(dir / "Qlite", ec)) {
            return dir;
        }
        if (dir == dir.parent_path()) {
            break;
        }
    }
    return start;
}

inline std::string harness_version_from_source_dir(const std::filesystem::path& source_dir) {
    std::error_code ec;
    const auto root = std::filesystem::weakly_canonical(source_dir, ec);
    const std::string name = (ec ? source_dir : root).filename().string();
    if (name.size() >= 2 && name[0] == 'v') {
        bool digits = true;
        for (std::size_t i = 1; i < name.size(); ++i) {
            if (!std::isdigit(static_cast<unsigned char>(name[i]))) {
                digits = false;
                break;
            }
        }
        if (digits) {
            return name;
        }
    }
    throw std::runtime_error("cannot derive harness version from source dir '" + source_dir.string()
                             + "' (expected .../vNNN)");
}

inline std::string harness_version_from_cwd() {
    return harness_version_from_source_dir(detect_harness_source_root(std::filesystem::current_path()));
}

} // namespace jac313