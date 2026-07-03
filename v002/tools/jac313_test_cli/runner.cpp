#include "runner.hpp"
#include "format.hpp"
#include "options.hpp"
#include "process.hpp"

#include <jac313/Qlite/v002.hpp>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <map>
#include <regex>
#include <unistd.h>
#include <vector>
#include <sys/resource.h>
#include "jac313_results_db.hpp"  // for jac313::results:: log and ensure when run_id provided

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
                           const int failsafe_sec, int memory_mb = 0)
{
    TestResult result;

    if (!fs::exists(command)) {
        result.status = TestStatus::Skipped;
        result.message = "executable not found (build first?): " + command.string();
        return result;
    }

    int stdout_pipe[2]{-1, -1};
    int stderr_pipe[2]{-1, -1};
    if (::pipe(stdout_pipe) != 0 || ::pipe(stderr_pipe) != 0) {
        result.status = TestStatus::Error;
        result.message = "pipe() failed for command: " + command.string();
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
        result.message = "fork() failed for: " + command.string();
        return result;
    }

    if (pid == 0) {
        ::close(stdout_pipe[0]);
        ::close(stderr_pipe[0]);
        ::dup2(stdout_pipe[1], STDOUT_FILENO);
        ::dup2(stderr_pipe[1], STDERR_FILENO);
        ::close(stdout_pipe[1]);
        ::close(stderr_pipe[1]);

        // Apply per-test memory limit (from DB) .
        if (memory_mb > 0) {
            struct rlimit rl;
            rlim_t bytes = (rlim_t)memory_mb * 1024ULL * 1024ULL;
            rl.rlim_cur = bytes;
            rl.rlim_max = bytes;
            setrlimit(RLIMIT_AS, &rl);
        }

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
            result.message = "failsafe timeout after " + std::to_string(failsafe_sec) + "s for " + command.string();
        } else {
            result.status = TestStatus::Error;
            result.message = "waitpid() failed for " + command.string();
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

TestResult run_test(const TestEntry& entry, const bool verbose, const int failsafe_sec, int memory_mb) {
    TestResult result = execute_command(entry.command, entry.args, verbose, failsafe_sec, memory_mb);
    result.entry = entry;
    return result;
}

TestResult run_test_with_args(const fs::path& command,
                              const std::vector<std::string>& args,
                              const bool verbose,
                              const int failsafe_sec, int memory_mb)
{
    return execute_command(command, args, verbose, failsafe_sec, memory_mb);
}

std::vector<TestResult> run_tests(const std::vector<TestEntry>& tests,
                                  const RunOptions& opts,
                                  bool verbose,
                                  std::int64_t run_id,
                                  const std::filesystem::path& db_path)
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

    // Right-pad the test name to the longest, and the index to the total's width, so the " ... PASS"
    // column starts in the same spot for every test.
    std::size_t name_w = 0;
    for (const auto& t : selected) if (t.name.size() > name_w) name_w = t.name.size();
    const std::size_t idx_w = format_count(total).size();

    // Load per-test control values (timeouts + memory limits) from the DB if available.
    // The DB testControl table is the runtime store for these "parameter values that control the testing".
    std::map<std::string, int> db_test_timeouts;
    std::map<std::string, int> db_test_memory;
    try {
        // Shared results DB — resolved once by main() (same process), read here by name.
        const std::string& dbp = jac313::results::JAC313_RESULTS_DB;
        if (std::filesystem::exists(dbp)) {
            jac313::Qlite::v002::Sqlite db(dbp);
            jac313::results::ensure_schema(db);
            // Load all known controls for this run (cheap for small number of tests).
            auto st = db.prepare("SELECT name, timeout_sec, memory_mb FROM testControl");
            while (st.step()) {
                std::string nm; std::int64_t secs = 0, mem = 0;
                st.get(nm, secs, mem);
                if (secs > 0) db_test_timeouts[nm] = static_cast<int>(secs);
                if (mem > 0) db_test_memory[nm] = static_cast<int>(mem);
            }
        }
    } catch (...) {}

    for (const auto& test : selected) {
        ++index;
        std::cout << "[" << format_count_padded(index, idx_w) << "/" << format_count(total) << "] "
                  << test.name << std::string(name_w - test.name.size(), ' ') << " ... ";
        std::cout.flush();

        // Live status log to DB so you can track in DataGrip while running
        if (run_id > 0 && !db_path.empty()) {
            try {
                jac313::Qlite::v002::Sqlite db(db_path.string());
                jac313::results::log_run_event(db, run_id, test.name, "started");
            } catch (...) {}
        }

        int per_test_sec = 0;
        auto it = db_test_timeouts.find(test.name);
        if (it != db_test_timeouts.end()) per_test_sec = it->second;
        if (per_test_sec <= 0) {
            per_test_sec = get_test_timeout(test.name);  // heuristic fallback
        }
        if (per_test_sec <= 0) per_test_sec = effective_failsafe_sec(opts);

        int per_test_mem = 0;
        auto mit = db_test_memory.find(test.name);
        if (mit != db_test_memory.end()) per_test_mem = mit->second;

        TestResult result = run_test(test, verbose, per_test_sec, per_test_mem);

        if (run_id > 0 && !db_path.empty()) {
            try {
                jac313::Qlite::v002::Sqlite db(db_path.string());
                std::string event = "ended";
                std::string msg = result.message;
                if (result.status == TestStatus::Failed || result.status == TestStatus::Error) {
                    event = "error";
                    if (msg.empty()) msg = "failed";
                    if (!result.stderr_tail.empty()) {
                        // truncate long tails for log
                        std::string tail = result.stderr_tail.substr(0, 500);
                        if (result.stderr_tail.size() > 500) tail += "...";
                        msg += " | " + tail;
                    }
                }
                jac313::results::log_run_event(db, run_id, test.name, event, msg, static_cast<std::int64_t>(result.duration.count()));
            } catch (...) {}
        }

        switch (result.status) {
        case TestStatus::Passed:
            std::cout << "PASS (" << format_count_padded(result.duration.count()) << " ms)\n";
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
            std::cout << " (" << format_count_padded(result.duration.count()) << " ms)\n";
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