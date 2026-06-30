#include "process.hpp"

#include <array>
#include <chrono>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <limits>
#include <optional>
#include <sys/resource.h>
#include <signal.h>
#include <thread>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

namespace jac313::test_cli {

namespace {

constexpr std::size_t kDefaultCaptureLimit = 16 * 1024;

void append_capped(std::string& captured, const char* data, std::size_t n, std::size_t limit) {
    captured.append(data, n);
    if (captured.size() > limit) {
        captured.erase(0, captured.size() - limit);
    }
}

void drain_fd_nonblocking(int fd, std::string& captured, std::size_t limit) {
    if (fd < 0) {
        return;
    }

    std::array<char, 4096> buf{};
    while (true) {
        const ssize_t n = ::read(fd, buf.data(), buf.size());
        if (n > 0) {
            append_capped(captured, buf.data(), static_cast<std::size_t>(n), limit);
            continue;
        }
        if (n == 0) {
            return;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return;
        }
        return;
    }
}

void set_nonblocking(int fd) {
    if (fd < 0) {
        return;
    }
    const int flags = ::fcntl(fd, F_GETFL, 0);
    if (flags >= 0) {
        ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }
}

struct ChildWaitOutcome {
    int status{0};
    bool timed_out{false};
};

ChildWaitOutcome wait_child_with_failsafe(const pid_t pid,
                                         const int stdout_fd,
                                         const int stderr_fd,
                                         std::string& stdout_text,
                                         std::string& stderr_text,
                                         const int failsafe_sec,
                                         const std::size_t capture_limit)
{
    ChildWaitOutcome outcome;
    if (pid <= 0) {
        return outcome;
    }

    set_nonblocking(stdout_fd);
    set_nonblocking(stderr_fd);

    const auto start = std::chrono::steady_clock::now();
    const auto limit = failsafe_sec > 0
        ? std::optional<std::chrono::steady_clock::time_point>(
              start + std::chrono::seconds(failsafe_sec))
        : std::nullopt;

    while (true) {
        drain_fd_nonblocking(stdout_fd, stdout_text, capture_limit);
        drain_fd_nonblocking(stderr_fd, stderr_text, capture_limit);

        int status = 0;
        const pid_t waited = ::waitpid(pid, &status, WNOHANG);
        if (waited == pid) {
            outcome.status = status;
            drain_fd_nonblocking(stdout_fd, stdout_text, capture_limit);
            drain_fd_nonblocking(stderr_fd, stderr_text, capture_limit);
            return outcome;
        }
        if (waited < 0) {
            // ECHILD: the child was already reaped elsewhere — its status is gone, so
            // there is nothing left to wait for. Any other error is also unrecoverable.
            // Either way, drain remaining output and bail instead of spinning forever
            // (a hung-child failsafe of 0 must not turn into a busy loop here).
            outcome.status = -1;
            drain_fd_nonblocking(stdout_fd, stdout_text, capture_limit);
            drain_fd_nonblocking(stderr_fd, stderr_text, capture_limit);
            return outcome;
        }

        if (limit.has_value() && std::chrono::steady_clock::now() >= *limit) {
            ::kill(pid, SIGKILL);
            ::waitpid(pid, &status, 0);
            outcome.timed_out = true;
            outcome.status = status;
            drain_fd_nonblocking(stdout_fd, stdout_text, capture_limit);
            drain_fd_nonblocking(stderr_fd, stderr_text, capture_limit);
            return outcome;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

} // namespace

bool wait_captured_child(const int pid,
                         const int stdout_fd,
                         const int stderr_fd,
                         std::string& stdout_tail,
                         std::string& stderr_tail,
                         const int failsafe_sec,
                         int& status_out,
                         bool& timed_out_out,
                         const std::size_t capture_limit)
{
    const auto outcome = wait_child_with_failsafe(
        pid, stdout_fd, stderr_fd, stdout_tail, stderr_tail, failsafe_sec, capture_limit);
    status_out = outcome.status;
    timed_out_out = outcome.timed_out;
    return !outcome.timed_out && outcome.status >= 0;
}

ProcessResult run_process(const std::vector<std::string>& args, const int failsafe_sec,
                          const std::string& cwd, int memory_mb) {
    ProcessResult result;
    if (args.empty()) {
        result.stderr_text = "empty command";
        return result;
    }

    int stdout_pipe[2]{-1, -1};
    int stderr_pipe[2]{-1, -1};
    if (::pipe(stdout_pipe) != 0 || ::pipe(stderr_pipe) != 0) {
        result.stderr_text = "pipe() failed";
        return result;
    }

    const pid_t pid = ::fork();
    if (pid < 0) {
        result.stderr_text = "fork() failed";
        return result;
    }

    if (pid == 0) {
        ::close(stdout_pipe[0]);
        ::close(stderr_pipe[0]);
        ::dup2(stdout_pipe[1], STDOUT_FILENO);
        ::dup2(stderr_pipe[1], STDERR_FILENO);
        ::close(stdout_pipe[1]);
        ::close(stderr_pipe[1]);

        if (!cwd.empty() && ::chdir(cwd.c_str()) != 0) {
            std::_Exit(126);
        }

        // Apply per-test memory limit (from DB testControl) to prevent runaway allocation
        // in ctest/smoke/verify etc. 0 = no limit. Uses RLIMIT_AS (virtual address space).
        if (memory_mb > 0) {
            struct rlimit rl;
            rlim_t bytes = (rlim_t)memory_mb * 1024ULL * 1024ULL;
            rl.rlim_cur = bytes;
            rl.rlim_max = bytes;
            setrlimit(RLIMIT_AS, &rl);
        }

        std::vector<char*> argv;
        argv.reserve(args.size() + 1);
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        ::execvp(argv[0], argv.data());
        std::_Exit(127);
    }

    result.started = true;
    ::close(stdout_pipe[1]);
    ::close(stderr_pipe[1]);

    const auto outcome = wait_child_with_failsafe(
        pid,
        stdout_pipe[0],
        stderr_pipe[0],
        result.stdout_text,
        result.stderr_text,
        failsafe_sec,
        std::numeric_limits<std::size_t>::max());

    ::close(stdout_pipe[0]);
    ::close(stderr_pipe[0]);

    if (outcome.timed_out) {
        result.failsafe_killed = true;
        result.exit_code = 137;
        if (result.stderr_text.empty()) {
            result.stderr_text = "failsafe timeout after " + std::to_string(failsafe_sec) + "s";
        } else {
            result.stderr_text += "\nfailsafe timeout after " + std::to_string(failsafe_sec) + "s";
        }
        return result;
    }

    if (outcome.status >= 0) {
        if (WIFEXITED(outcome.status)) {
            result.exit_code = WEXITSTATUS(outcome.status);
        } else if (WIFSIGNALED(outcome.status)) {
            result.exit_code = 128 + WTERMSIG(outcome.status);
        }
    }

    return result;
}

} // namespace jac313::test_cli