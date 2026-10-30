#pragma once

#include "manifest.hpp"
#include "options.hpp"

#include <chrono>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace jac313::test_cli {

enum class TestStatus { Passed, Failed, Skipped, Error };

struct TestResult {
    TestEntry entry;
    TestStatus status{TestStatus::Error};
    int exit_code{-1};
    std::chrono::milliseconds duration{0};
    std::string message;
    std::string stdout_tail;
    std::string stderr_tail;
};

struct RunOptions;

TestResult run_test(const TestEntry& entry, bool verbose, int failsafe_sec = kDefaultFailsafeSec, int memory_mb = 0);
TestResult run_test_with_args(const std::filesystem::path& command,
                              const std::vector<std::string>& args,
                              bool verbose,
                              int failsafe_sec = kDefaultFailsafeSec, int memory_mb = 0);
std::vector<TestResult> run_tests(const std::vector<TestEntry>& tests,
                                  const RunOptions& opts,
                                  bool verbose,
                                  std::int64_t run_id = 0,
                                  const std::filesystem::path& db_path = {});

bool matches_filter(const std::string& name, const std::string& filter);

// Per-test timeouts and memory limits from results.db testControl (seeded from tests/test_limits.jtext).
struct TestControls {
    std::map<std::string, int> timeout_sec;   // only rows with a timeout > 0
    std::map<std::string, int> memory_mb;     // only rows with a limit > 0
};
// Seed testControl from <source_dir>/tests/test_limits.jtext (when both exist), then read it back.
TestControls load_test_controls(const std::filesystem::path& source_dir);

} // namespace jac313::test_cli