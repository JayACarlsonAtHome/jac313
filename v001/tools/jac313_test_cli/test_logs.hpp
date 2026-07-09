#pragma once

#include "matrix.hpp"
#include "options.hpp"
#include "run_identity.hpp"
#include "runner.hpp"

#include <filesystem>
#include <vector>

namespace jac313::test_cli {

// test-results/<os>/<compiler>/<build>/<disk>/Ctest/<modules>/logs/
std::filesystem::path ctest_logs_dir(const std::filesystem::path& source_dir,
                                     const std::filesystem::path& build_dir,
                                     const std::string& compiler_label);

void write_ctest_logs(const GlobalOptions& global,
                      const std::string& compiler_label,
                      const std::vector<TestResult>& results);

struct WipeLogsReport {
    std::size_t entries_removed{0};
    std::uint64_t bytes_removed{0};
};

// Remove everything under test-results/ and kept-logs/ except each tree's README.md.
WipeLogsReport wipe_local_logs(const std::filesystem::path& version_root,
                               const std::filesystem::path& keep_logs_override = {});

int run_wipe_logs_command(const GlobalOptions& global);

// gcc16 / clang20 style label from the build tree's CMAKE_CXX_COMPILER.
std::string compiler_label_from_build(const std::filesystem::path& build_dir);

} // namespace jac313::test_cli