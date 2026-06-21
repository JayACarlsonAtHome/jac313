#pragma once

#include <string>
#include <vector>

namespace jac313::test_cli {

enum class PlatformProfile {
    LinuxMint,
    Rhel,
};

struct PlatformInfo {
    std::string id;
    std::string pretty_name;
    PlatformProfile profile{PlatformProfile::Rhel};
};

struct ReleaseCompilerSlot {
    std::string label;
    std::string compiler_id;
    bool prefer_gcc15{false};
    bool prefer_clang{false};
};

PlatformInfo detect_platform();
PlatformInfo platform_from_name(const std::string& name);
std::vector<ReleaseCompilerSlot> release_compilers_for_platform(PlatformProfile profile);
std::string platform_profile_name(PlatformProfile profile);

} // namespace jac313::test_cli