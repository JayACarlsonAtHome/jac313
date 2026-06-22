// File:    Setup/include/jac313/Setup/contracts.hpp
// Purpose: jac313 contracts shim — pre/post/assert with the spelling we will migrate
//          to native ISO C++26 contracts (P2900) once a contracts-capable compiler
//          is available. As of 2026-06 neither gcc-toolset-15 (GCC 15.2) nor clang-21
//          define __cpp_contracts, so these expand to runtime checks today.
// Migration: when __cpp_contracts is defined, JAC313_PRE/POST become native pre()/post()
//          on the function declarator and JAC313_ASSERT becomes contract_assert(); the
//          call sites keep the same intent and can be ported mechanically.
#pragma once

#include <cstdio>
#include <cstdlib>
#include <source_location>

namespace jac313::contracts {

// A contract violation aborts after reporting — a fail-fast safety net, deterministic
// for tests. (A future build option can downgrade this to log-and-continue.)
[[noreturn]] inline void
violation(const char* kind, const char* expr,
          const std::source_location loc = std::source_location::current()) {
    std::fprintf(stderr, "jac313 contract %s violated: (%s) at %s:%u in %s\n",
                 kind, expr, loc.file_name(),
                 static_cast<unsigned>(loc.line()), loc.function_name());
    std::abort();
}

} // namespace jac313::contracts

// NOTE: when native contracts arrive (__cpp_contracts), prefer porting to:
//   ret f(args) pre(cond) post(r: cond);   and   contract_assert(cond);
// Until then these runtime macros carry the same guarantees at call sites.

// JAC313_CONTRACTS_ENABLED: 1 (default) = runtime fail-fast checks; define to 0
// (e.g. -DJAC313_CONTRACTS_ENABLED=0 for a release/perf build) to compile the checks
// out to zero cost. Default behaviour is unchanged unless explicitly disabled.
#ifndef JAC313_CONTRACTS_ENABLED
#  define JAC313_CONTRACTS_ENABLED 1
#endif

#if JAC313_CONTRACTS_ENABLED

#define JAC313_ASSERT(expr) \
    ((expr) ? (void)0 : ::jac313::contracts::violation("assert", #expr))

// Precondition: place at the top of a function body.
#define JAC313_PRE(expr) \
    ((expr) ? (void)0 : ::jac313::contracts::violation("precondition", #expr))

// Postcondition (point check). True return-value postconditions need native contracts
// or an explicit scope guard; this checks the condition at the point of use.
#define JAC313_POST(expr) \
    ((expr) ? (void)0 : ::jac313::contracts::violation("postcondition", #expr))

#else // contracts disabled — zero-cost no-ops (release/perf builds)

#define JAC313_ASSERT(expr) ((void)0)
#define JAC313_PRE(expr)    ((void)0)
#define JAC313_POST(expr)   ((void)0)

#endif
