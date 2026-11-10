// Verifies JAC313_CONTRACTS_ENABLED=0 compiles the contract macros out to no-ops:
// a would-be-violated assertion must NOT abort — control reaches the success print — and no
// condition is evaluated.

#define JAC313_CONTRACTS_ENABLED 0
#include <jac313/Setup/contracts.hpp>

#include <iostream>

int main() {
    JAC313_ASSERT(false);  // no-op when disabled — must not abort
    JAC313_PRE(1 == 2);    // no-op
    JAC313_POST(false);    // no-op
    int evaluated = 0;
    auto mark = [&evaluated] { ++evaluated; return false; };
    JAC313_ASSERT(mark());
    JAC313_PRE(mark());
    JAC313_POST(mark());
    if (evaluated != 0) {
        std::cerr << "FAIL: a disabled contract evaluated its condition\n";
        return 1;
    }
    std::cout << "jac313 contracts shim — disabled mode compiles checks to no-ops\n";
    return 0;
}
