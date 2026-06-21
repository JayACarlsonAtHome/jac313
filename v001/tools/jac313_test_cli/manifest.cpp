#include "manifest.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <regex>
#include <set>

namespace jac313::test_cli {
namespace fs = std::filesystem;

namespace {

std::string trim(std::string s) {
    auto not_space = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
    return s;
}

std::vector<std::string> parse_quoted_strings(const std::string& text) {
    std::vector<std::string> out;
    static const std::regex quoted(R"re("([^"]*)")re");
    for (auto it = std::sregex_iterator(text.begin(), text.end(), quoted);
         it != std::sregex_iterator(); ++it) {
        out.push_back((*it)[1].str());
    }
    return out;
}

bool parse_add_test_line(const std::string& line, TestEntry& out) {
    static const std::regex header(R"(add_test\(\[=\[(.+?)\]=\]\s+(.+))");
    std::smatch match;
    if (!std::regex_search(line, match, header)) {
        return false;
    }

    const auto tokens = parse_quoted_strings(match[2].str());
    if (tokens.empty()) {
        return false;
    }

    out.name = match[1].str();
    out.command = tokens.front();
    out.args.clear();
    if (tokens.size() > 1) {
        out.args.assign(tokens.begin() + 1, tokens.end());
    }
    return true;
}

bool parse_subdir_line(const std::string& line, std::string& out_subdir) {
    static const std::regex re(R"(subdirs\(\"([^\"]+)\"\))");
    std::smatch match;
    if (!std::regex_search(line, match, re)) {
        return false;
    }
    out_subdir = match[1].str();
    return true;
}

void load_ctest_file(const fs::path& ctest_file,
                     const fs::path& build_dir,
                     std::vector<TestEntry>& out,
                     std::set<fs::path>& visited)
{
    const fs::path canonical = fs::weakly_canonical(ctest_file);
    if (!visited.insert(canonical).second) {
        return;
    }

    std::ifstream in(ctest_file);
    if (!in) {
        return;
    }

    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        TestEntry entry;
        if (parse_add_test_line(line, entry)) {
            entry.ctest_file = ctest_file;
            out.push_back(std::move(entry));
            continue;
        }

        std::string subdir;
        if (parse_subdir_line(line, subdir)) {
            const fs::path child = ctest_file.parent_path() / subdir / "CTestTestfile.cmake";
            if (fs::exists(child)) {
                load_ctest_file(child, build_dir, out, visited);
            }
        }
    }
}

} // namespace

std::vector<TestEntry> discover_tests(const fs::path& build_dir) {
    std::vector<TestEntry> tests;
    std::set<fs::path> visited;

    const fs::path root_ctest = build_dir / "CTestTestfile.cmake";
    if (!fs::exists(root_ctest)) {
        return tests;
    }

    load_ctest_file(root_ctest, build_dir, tests, visited);
    return tests;
}

} // namespace jac313::test_cli