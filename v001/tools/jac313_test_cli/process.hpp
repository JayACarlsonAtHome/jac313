#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace jac313::test_cli {

struct ProcessResult {
    int exit_code{-1};
    std::string stdout_text;
    std::string stderr_text;
    bool started{false};
    bool failsafe_killed{false};
};

// Run a command without shell interpretation. Returns ProcessResult with captured output.
// failsafe_sec: kill the child after this many seconds (0 = no limit).
// cwd: if non-empty, chdir the child to this directory before exec (so a wrapped test
// binary writes its persist_* output there, not into the caller's working directory).
ProcessResult run_process(const std::vector<std::string>& args, int failsafe_sec = 0,
                          const std::string& cwd = {}, int memory_mb = 0);

// Wait on a forked child, capturing stdout/stderr tails. Kills with SIGKILL on timeout.
bool wait_captured_child(int pid,
                         int stdout_fd,
                         int stderr_fd,
                         std::string& stdout_tail,
                         std::string& stderr_tail,
                         int failsafe_sec,
                         int& status_out,
                         bool& timed_out_out,
                         std::size_t capture_limit = 16 * 1024);

} // namespace jac313::test_cli