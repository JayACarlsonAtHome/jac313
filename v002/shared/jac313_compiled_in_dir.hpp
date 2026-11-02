#pragma once

// A program works only where it was compiled: JAC313_COMPILED_IN_DIR is the version folder
// (<root>/vNNN) it was built from, set by tools/CMakeLists.txt. A program whose tree was moved,
// copied or re-mounted stops here and asks for a rebuild instead of working on stale paths.

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <system_error>

namespace jac313 {

inline constexpr std::string_view kCompiledInDir = JAC313_COMPILED_IN_DIR;

inline void require_compiled_in_dir() {
    namespace fs = std::filesystem;
    std::error_code ec;
    const fs::path exe = fs::canonical("/proc/self/exe", ec);
    if (ec) return;   // no way to tell where this program is
    const fs::path compiled = fs::weakly_canonical(fs::path(kCompiledInDir), ec);
    if (ec) return;
    const std::string dir = compiled.string() + "/";
    if (exe.string().starts_with(dir)) return;
    std::cerr << exe.filename().string() << ": compiled in " << kCompiledInDir << " but running from "
              << exe.parent_path().string() << ".\n  Rebuild it: run ./bsir.sh in this checkout's version folder.\n";
    std::exit(3);
}

}  // namespace jac313
