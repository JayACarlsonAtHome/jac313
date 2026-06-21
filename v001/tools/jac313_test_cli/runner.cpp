#include "runner.hpp"
#include "format.hpp"
#include "options.hpp"
#include "process.hpp"

#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <regex>
#include <unistd.h>
#include <vector>

namespace jac313::test_cli {
namespace fs = std::filesystem;

namespace {

constexpr std::size_t kCaptureLimit = 16 * 1024;

} // namespace

bool matches_filter(const std::string& name, const std::string& filter) {
    if (filter.empty()) {
        return true;
    }
    try {
        std::regex re(filter);
        return std::regex_search(name, re);
    } catch (const std::regex_error&) {
        return name.find(filter) != std::string::npos;
    }
}

TestResult execute_command(const fs::path& command,
                           const std::vector<std::string>& args,
                           const bool verbose,
                           const int failsafe_sec)
{
    TestResult result;

    if (!fs::exists(command)) {
        result.status = TestStatus::Skipped;
        result.message = "executable not found (build first?)";
        return result;
    }

    int stdout_pipe[2]{-1, -1};
    int stderr_pipe[2]{-1, -1};
    if (::pipe(stdout_pipe) != 0 || ::pipe(stderr_pipe) != 0) {
        result.status = TestStatus::Error;
        result.message = "pipe() failed";
        return result;
    }

    const auto start = std::chrono::steady_clock::now();
    const pid_t pid = ::fork();
    if (pid < 0) {
        ::close(stdout_pipe[0]);
        ::close(stdout_pipe[1]);
        ::close(stderr_pipe[0]);
        ::close(stderr_pipe[1]);
        result.status = TestStatus::Error;
        result.message = "fork() failed";
        return result;
    }

    if (pid == 0) {
        ::close(stdout_pipe[0]);
        ::close(stderr_pipe[0]);
        ::dup2(stdout_pipe[1], STDOUT_FILENO);
        ::dup2(stderr_pipe[1], STDERR_FILENO);
        ::close(stdout_pipe[1]);
        ::close(stderr_pipe[1]);

        const std::string cmd = command.string();
        std::vector<char*> argv;
        argv.reserve(args.size() + 2);
        argv.push_back(const_cast<char*>(cmd.c_str()));
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        ::execv(cmd.c_str(), argv.data());
        std::_Exit(127);
    }

    ::close(stdout_pipe[1]);
    ::close(stderr_pipe[1]);

    int status = 0;
    bool timed_out = false;
    if (!wait_captured_child(
            pid,
            stdout_pipe[0],
            stderr_pipe[0],
            result.stdout_tail,
            result.stderr_tail,
            failsafe_sec,
            status,
            timed_out,
            kCaptureLimit)) {
        if (timed_out) {
            result.status = TestStatus::Failed;
            result.exit_code = 137;
            result.message = "failsafe timeout after " + std::to_string(failsafe_sec) + "s";
        } else {
            result.status = TestStatus::Error;
            result.message = "waitpid() failed";
        }
        ::close(stdout_pipe[0]);
        ::close(stderr_pipe[0]);
        result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start);
        return result;
    }

    ::close(stdout_pipe[0]);
    ::close(stderr_pipe[0]);

    result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start);

    if (WIFEXITED(status)) {
        result.exit_code = WEXITSTATUS(status);
        if (result.exit_code == 0) {
            result.status = TestStatus::Passed;
        } else {
            result.status = TestStatus::Failed;
            result.message = "exit code " + std::to_string(result.exit_code);
        }
    } else if (WIFSIGNALED(status)) {
        result.status = TestStatus::Failed;
        result.exit_code = 128 + WTERMSIG(status);
        result.message = "terminated by signal " + std::to_string(WTERMSIG(status));
    } else {
        result.status = TestStatus::Error;
        result.message = "unknown child status";
    }

    if (verbose && result.status == TestStatus::Passed && !result.stdout_tail.empty()) {
        std::cout << result.stdout_tail;
        if (result.stdout_tail.back() != '\n') {
            std::cout << '\n';
        }
    }

    return result;
}

TestResult run_test(const TestEntry& entry, const bool verbose, const int failsafe_sec) {
    TestResult result = execute_command(entry.command, entry.args, verbose, failsafe_sec);
    result.entry = entry;
    return result;
}

TestResult run_test_with_args(const fs::path& command,
                              const std::vector<std::string>& args,
                              const bool verbose,
                              const int failsafe_sec)
{
    return execute_command(command, args, verbose, failsafe_sec);
}

std::vector<TestResult> run_tests(const std::vector<TestEntry>& tests,
                                  const RunOptions& opts,
                                  bool verbose)
{
    std::vector<TestEntry> selected;
    selected.reserve(tests.size());
    for (const auto& test : tests) {
        if (matches_filter(test.name, opts.filter)) {
            selected.push_back(test);
        }
    }

    if (selected.empty()) {
        std::cout << "No tests match";
        if (!opts.filter.empty()) {
            std::cout << " filter \"" << opts.filter << '"';
        }
        std::cout << '\n';
        return {};
    }

    std::vector<TestResult> results;
    results.reserve(selected.size());

    const int total = static_cast<int>(selected.size());
    int index = 0;

    for (const auto& test : selected) {
        ++index;
        std::cout << "[" << format_count(index) << "/" << format_count(total) << "] "
                  << test.name << " ... ";
        std::cout.flush();

        TestResult result = run_test(test, verbose, effective_failsafe_sec(opts));

        switch (result.status) {
        case TestStatus::Passed:
            std::cout << "PASS (" << format_count(result.duration.count()) << " ms)\n";
            break;
        case TestStatus::Skipped:
            std::cout << "SKIP";
            if (!result.message.empty()) {
                std::cout << " — " << result.message;
            }
            std::cout << '\n';
            break;
        case TestStatus::Failed:
            std::cout << "FAIL";
            if (!result.message.empty()) {
                std::cout << " — " << result.message;
            }
            std::cout << " (" << format_count(result.duration.count()) << " ms)\n";
            if (!result.stdout_tail.empty()) {
                std::cout << "--- stdout ---\n" << result.stdout_tail << '\n';
            }
            if (!result.stderr_tail.empty()) {
                std::cout << "--- stderr ---\n" << result.stderr_tail << '\n';
            }
            break;
        case TestStatus::Error:
            std::cout << "ERROR";
            if (!result.message.empty()) {
                std::cout << " — " << result.message;
            }
            std::cout << '\n';
            break;
        }

        results.push_back(std::move(result));

        if (opts.fail_fast && result.status != TestStatus::Passed
            && result.status != TestStatus::Skipped) {
            break;
        }
    }

    return results;
}

} // namespace jac313::test_cli