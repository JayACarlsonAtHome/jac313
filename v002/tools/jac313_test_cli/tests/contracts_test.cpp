// Unit test for the jac313 contracts shim (jac313::contracts).
// Verifies the happy path: pre/post/assert are usable as statements and do NOT
// fire on satisfied conditions. (Death-test of the abort path is a follow-up; it
// needs a fork/_exit harness this minimal suite doesn't yet have.)

#include <jac313/Setup/contracts.hpp>

#include <iostream>

namespace {

int add_checked(int a, int b) {
    JAC313_PRE(a >= 0);
    JAC313_PRE(b >= 0);
    const int r = a + b;
    JAC313_POST(r >= a); // point check
    return r;
}

} // namespace

int main() {
    JAC313_ASSERT(1 + 1 == 2);

    const int r = add_checked(20, 22);
    JAC313_ASSERT(r == 42);

    if (r != 42) {
        std::cerr << "FAIL: add_checked(20,22) = " << r << " want 42\n";
        return 1;
    }
    std::cout << "jac313 contracts shim — pre/post/assert happy-path OK\n";
    return 0;
}
