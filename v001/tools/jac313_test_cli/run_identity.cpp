#include "run_identity.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace jac313::test_cli {

namespace {

std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::string trim(std::string s) {
    auto not_space = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
    return s;
}

std::string strip_quotes(std::string s) {
    if (s.size() >= 2 && (s.front() == '"' || s.front() == '\'') && s.back() == s.front()) {
        s = s.substr(1, s.size() - 2);
    }
    return s;
}

// First value of KEY=... in an os-release body (quotes stripped, trimmed).
std::string os_release_value(const std::string& text, const std::string& key) {
    std::istringstream in(text);
    const std::string prefix = key + "=";
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.rfind(prefix, 0) == 0) {
            return strip_quotes(trim(line.substr(prefix.size())));
        }
    }
    return {};
}

} // namespace

const std::vector<std::string>& run_identity_columns() {
    static const std::vector<std::string> cols = {
        "os", "compiler", "build_type", "disk_type", "size_label", "modules"};
    return cols;
}

std::string identity_select_list(const std::string& table_prefix) {
    std::string out;
    const std::string p = table_prefix.empty() ? "" : table_prefix + ".";
    for (const auto& c : run_identity_columns()) {
        if (!out.empty()) out += ", ";
        out += p + c;
    }
    return out;
}

std::string identity_group_by(const std::string& table_prefix) {
    return identity_select_list(table_prefix);
}

std::string identity_join_on(const std::string& a, const std::string& b) {
    std::string out;
    for (const auto& c : run_identity_columns()) {
        if (!out.empty()) out += " AND ";
        out += a + "." + c + " = " + b + "." + c;
    }
    return out;
}

std::string identity_where_eq() {
    std::string out;
    for (const auto& c : run_identity_columns()) {
        if (!out.empty()) out += " AND ";
        out += c + "=?";
    }
    return out;
}

std::vector<std::string> identity_values(const RunIdentity& id) {
    // Order MUST match run_identity_columns().
    return {id.os, id.compiler, id.build_type, id.disk_type, id.size_label, id.modules};
}

std::filesystem::path identity_path(const std::filesystem::path& project_root,
                                    const RunIdentity& id) {
    return project_root / "test-results" / id.os / id.compiler / id.build_type
           / id.disk_type / id.size_label / id.modules;
}

std::string os_key_from_release(const std::string& os_release_text) {
    const std::string id = lower(os_release_value(os_release_text, "ID"));
    if (id.empty()) {
        return "unknown";
    }
    const std::string version = lower(os_release_value(os_release_text, "VERSION_ID"));
    return version.empty() ? id : id + "-" + version;
}

std::string sensed_os_key() {
    std::ifstream in("/etc/os-release");
    if (!in) {
        return "unknown";
    }
    std::stringstream ss;
    ss << in.rdbuf();
    return os_key_from_release(ss.str());
}

} // namespace jac313::test_cli
