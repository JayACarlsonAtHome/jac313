// Verifies JAC313_CONTRACTS_ENABLED=0 compiles the contract macros out to no-ops:
// a would-be-violated assertion must NOT abort — control reaches the success print.

#define JAC313_CONTRACTS_ENABLED 0
#include <jac313/Setup/contracts.hpp>

#include <iostream>

int main() {
    JAC313_ASSERT(false);  // no-op when disabled — must not abort
    JAC313_PRE(1 == 2);    // no-op
    JAC313_POST(false);    // no-op
    std::cout << "jac313 contracts shim — disabled mode compiles checks to no-ops\n";
    return 0;
}
