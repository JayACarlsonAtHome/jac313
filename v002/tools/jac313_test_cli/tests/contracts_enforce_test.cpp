// Death-test for the ENFORCE/abort path of the jac313 contracts shim.
//
// The happy-path test (contracts_test.cpp) only checks that satisfied contracts do
// NOT fire. This test closes the gap: it verifies that a VIOLATED contract actually
// terminates the program. It forks a child that violates a contract and asserts the
// child did not exit cleanly.
//
//   * Native mode  (gcc16 -std=c++26 -fcontracts -fcontract-evaluation-semantic=enforce):
//       JAC313_* map to contract_assert and `must_be_positive` carries a real P2900
//       pre() — a violation invokes the contract-violation handler and terminates.
//   * Runtime mode (clang / no native contracts): the shim's violation() aborts.
//
// Either way a violated contract must make the child terminate abnormally / non-zero.

#include <jac313/Setup/contracts.hpp>

#include <print>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Carries a NATIVE declarator precondition when the compiler provides contracts, so
// this test genuinely exercises P2900 pre() (not only contract_assert). Falls back to
// the portable macro form otherwise.
#if defined(__cpp_contracts)
int must_be_positive(const int n) pre (n > 0) { return n; }
#else
int must_be_positive(const int n) { JAC313_PRE(n > 0); return n; }
#endif

// Run `fn` in a forked child; return true iff the child terminated abnormally or with
// a non-zero status (i.e. a contract fired). A clean exit(0) means no violation.
template <class Fn>
static bool child_terminates(Fn fn) {
    std::fflush(nullptr);
    const pid_t pid = fork();
    if (pid < 0) { std::perror("fork"); std::_Exit(2); }  // fork failed -> test error
    if (pid == 0) {          // child
        fn();
        std::_Exit(0);       // reached only if NO contract fired
    }
    int status = 0;
    while (waitpid(pid, &status, 0) < 0) { /* retry on EINTR */ }
    const bool clean = WIFEXITED(status) && WEXITSTATUS(status) == 0;
    return !clean;
}

int main() {
    int failures = 0;

    // 1) satisfied precondition must NOT terminate
    if (child_terminates([] { (void)must_be_positive(5); })) {
        std::println(stderr, "FAIL: satisfied pre() terminated the child");
        ++failures;
    }
    // 2) violated precondition MUST terminate
    if (!child_terminates([] { (void)must_be_positive(0); })) {
        std::println(stderr, "FAIL: violated pre() did NOT terminate");
        ++failures;
    }
    // 3) violated JAC313_ASSERT MUST terminate
    if (!child_terminates([] { JAC313_ASSERT(1 == 2); })) {
        std::println(stderr, "FAIL: violated JAC313_ASSERT did NOT terminate");
        ++failures;
    }

    if (failures == 0) {
        std::println("jac313 contracts — enforce/abort death-test OK "
                     "(native={})", JAC313_CONTRACTS_NATIVE);
    }
    return failures == 0 ? 0 : 1;
}
