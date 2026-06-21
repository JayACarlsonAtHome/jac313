#include "platform.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <optional>
#include <stdexcept>

namespace jac313::test_cli {

namespace {

std::string trim(std::string text) {
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front()))) {
        text.erase(text.begin());
    }
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back()))) {
        text.pop_back();
    }
    return text;
}

std::string lower(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return text;
}

std::optional<std::string> read_os_release_value(const std::string& key) {
    std::ifstream in("/etc/os-release");
    if (!in) {
        return std::nullopt;
    }
    const std::string prefix = key + "=";
    std::string line;
    while (std::getline(in, line)) {
        if (!line.starts_with(prefix)) {
            continue;
        }
        std::string value = line.substr(prefix.size());
        if (!value.empty() && value.front() == '"') {
            value.erase(value.begin());
        }
        if (!value.empty() && value.back() == '"') {
            value.pop_back();
        }
        return trim(std::move(value));
    }
    return std::nullopt;
}

bool id_like_contains(const std::string& id_like, const std::string& token) {
    std::string haystack = lower(id_like);
    const std::string needle = lower(token);
    std::size_t pos = 0;
    while ((pos = haystack.find(needle, pos)) != std::string::npos) {
        const bool start_ok = pos == 0 || haystack[pos - 1] == ' ';
        const std::size_t end_pos = pos + needle.size();
        const bool end_ok = end_pos == haystack.size() || haystack[end_pos] == ' ';
        if (start_ok && end_ok) {
            return true;
        }
        pos += needle.size();
    }
    return false;
}

bool is_rhel_family_id(const std::string& id) {
    static constexpr const char* kIds[] = {
        "rhel", "centos", "rocky", "almalinux", "ol", "scientific", "sl",
    };
    const std::string lowered = lower(id);
    for (const char* candidate : kIds) {
        if (lowered == candidate) {
            return true;
        }
    }
    return false;
}

PlatformProfile infer_profile(const std::string& id,
                              const std::string& id_like,
                              const std::string& pretty_name)
{
    const std::string id_lower = lower(id);
    const std::string pretty_lower = lower(pretty_name);

    if (id_lower == "linuxmint" || pretty_lower.find("linux mint") != std::string::npos) {
        return PlatformProfile::LinuxMint;
    }
    if (is_rhel_family_id(id_lower) || id_like_contains(id_like, "rhel")
        || id_like_contains(id_like, "fedora")) {
        return PlatformProfile::Rhel;
    }
    return PlatformProfile::Rhel;
}

ReleaseCompilerSlot make_gcc15_slot() {
    return {"gcc15", "g++-15", true, false};
}

ReleaseCompilerSlot make_clang_slot() {
    return {"clang", "clang++-20", false, true};
}

} // namespace

PlatformInfo detect_platform() {
    PlatformInfo info;
    info.id = read_os_release_value("ID").value_or("unknown");
    info.pretty_name = read_os_release_value("PRETTY_NAME").value_or(info.id);
    const std::string id_like = read_os_release_value("ID_LIKE").value_or("");
    info.profile = infer_profile(info.id, id_like, info.pretty_name);
    return info;
}

PlatformInfo platform_from_name(const std::string& name) {
    const std::string lowered = lower(trim(name));
    PlatformInfo info;
    if (lowered == "mint" || lowered == "linuxmint" || lowered == "linux-mint") {
        info.id = "linuxmint";
        info.pretty_name = "Linux Mint (forced)";
        info.profile = PlatformProfile::LinuxMint;
        return info;
    }
    if (lowered == "rhel" || lowered == "redhat" || lowered == "red-hat") {
        info.id = "rhel";
        info.pretty_name = "RHEL (forced)";
        info.profile = PlatformProfile::Rhel;
        return info;
    }
    throw std::runtime_error("unknown --platform value: " + name + " (use mint or rhel)");
}

std::vector<ReleaseCompilerSlot> release_compilers_for_platform(PlatformProfile profile) {
    // Uniform across platforms: gcc15 + clang. On Mint gcc15 comes from the
    // ubuntu-toolchain-r PPA; on RHEL-family from gcc-toolset-15.
    switch (profile) {
    case PlatformProfile::LinuxMint:
    case PlatformProfile::Rhel:
        return {make_gcc15_slot(), make_clang_slot()};
    }
    return {make_gcc15_slot(), make_clang_slot()};
}

std::string platform_profile_name(PlatformProfile profile) {
    switch (profile) {
    case PlatformProfile::LinuxMint:
        return "linuxmint";
    case PlatformProfile::Rhel:
        return "rhel";
    }
    return "unknown";
}

} // namespace jac313::test_cli