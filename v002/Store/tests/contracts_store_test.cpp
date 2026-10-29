// Death-test for the contracts in Store: a store filled past max_threads x events_per_thread
// must terminate (JAC313_ASSERT in save_event) instead of writing past its preallocated rows.
// Native contract_assert on gcc16 (-fcontracts, enforce); the runtime fail-fast check elsewhere.

#include <jac313/Store/v002.hpp>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using Config = jac313::Store::v002::ts_store_config<true, 6, 20, 43, 2, 2, false>;

// Run `fn` in a forked child; true iff the child did not exit cleanly (a contract fired).
template <class Fn>
static bool child_terminates(Fn fn) {
    std::fflush(nullptr);
    const pid_t pid = fork();
    if (pid < 0) { std::perror("fork"); std::_Exit(2); }
    if (pid == 0) {
        fn();
        std::_Exit(0);
    }
    int status = 0;
    while (waitpid(pid, &status, 0) < 0) { /* retry on EINTR */ }
    return !(WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

// Save `events` events into a store sized for exactly 2.
static void fill(const int events) {
    jac313::Store::v002::ts_store<Config> store(1, 2);
    for (int i = 0; i < events; ++i)
        (void)store.save_event(0, static_cast<size_t>(i), Config::ValueT("x"));
}

int main() {
    int failures = 0;
    if (child_terminates([] { fill(2); })) {
        std::cerr << "FAIL: a store filled to capacity terminated\n";
        ++failures;
    }
    if (!child_terminates([] { fill(3); })) {
        std::cerr << "FAIL: a store filled past capacity did NOT terminate\n";
        ++failures;
    }
    if (failures == 0)
        std::cout << "jac313::Store::v002 contracts — save_event past capacity terminates (native="
                  << JAC313_CONTRACTS_NATIVE << ")\n";
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
