#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace jac313::test_cli {

struct TestEntry {
    std::string name;
    std::filesystem::path command;
    std::vector<std::string> args;
    std::filesystem::path ctest_file;
};

// Discover tests by walking CTestTestfile.cmake trees under build_dir.
std::vector<TestEntry> discover_tests(const std::filesystem::path& build_dir);

} // namespace jac313::test_cli