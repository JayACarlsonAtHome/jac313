#pragma once

#include "manifest.hpp"
#include "options.hpp"

#include <chrono>
#include <filesystem>
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

} // namespace jac313::test_cli