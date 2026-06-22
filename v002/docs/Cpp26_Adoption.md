// File:    docs/Cpp26_Adoption.md
// Purpose: What C++26 is available on our toolchains, what v002 has adopted, and the
//          migration plan for features that aren't compiler-ready yet (contracts).

# C++26 Adoption — v002

v002's build baseline is **`cxx_std_26`** (see the per-module `target_compile_features`).
The code compiles clean under `-std=c++26` on the toolchains below; this page records
*what the compilers actually support*, what we've adopted, and what's deferred.

## Toolchains & feature availability (measured 2026-06)

Detected via `g++ -std=c++26 -dM -E -x c++ <(echo '#include <version>')` and direct
compiles — **not** from memory.

| Toolchain | C++26 (`-std=c++26`) | Notes |
|---|---|---|
| system g++ 11.5 | ❌ | C++20 max — not used for v002 |
| **gcc-toolset-15 (GCC 15.2.1)** | ✅ | `__cplusplus = 202400`; the standard build compiler |
| **clang 21.1.8** | ✅ | second gate |
| gcc-toolset-16 / GCC 16 | n/a here | not packaged for RHEL 9 (Fedora-only); would add contracts/reflection |

**Available now (feature-test macros present on GCC 15.2):** `__cpp_pack_indexing`,
`__cpp_placeholder_variables`, `__cpp_deleted_function` (`= delete("reason")`),
`__cpp_static_assert` (user-generated messages), `__cpp_constexpr = 202406`,
`__cpp_structured_bindings = 202403`, `__cpp_lib_saturation_arithmetic`,
`__cpp_lib_span` (`span::at`), `__cpp_lib_format/print/ranges`, `__cpp_lib_text_encoding`.

**NOT available yet:** ISO **contracts** (P2900) — no `__cpp_contracts`; the
`pre()/post()/contract_assert()` syntax errors on GCC 15.2 and clang 21. Needs GCC 16
or a contracts-enabled build.

## Adopted in v002 (safety-first)

The lens for adoption has been **memory/overflow safety** — and it has already found and
fixed real bugs:

1. **Saturating arithmetic** (`std::mul_sat` / `std::add_sat`, `<numeric>`):
   - `ts_store::expected_size()` and the constructor memory guard — an overflowing
     `threads × events` or `N × per_row` estimate now saturates to `SIZE_MAX` instead of
     wrapping small and slipping past the guard into a huge allocation.
   - Used throughout the binary read/grow paths for overflow-safe offset math.
2. **`std::span` bounds-checking** on the binary **read** path
   (`BinaryEventLogReader`): every read of file-controlled lengths is gated, so a
   truncated/malicious record is **refused**, not over-read.
3. **mmap grow fix** (`BinaryEventLog`): the write path grew by a single doubling,
   which **overflowed the mapping** for any record larger than the current file size —
   now grows in a loop (overflow-safe) until the record fits. *(Real buffer-overflow bug.)*
4. **`jac313::contracts` shim** (`Setup/include/jac313/Setup/contracts.hpp`):
   `JAC313_PRE/POST/ASSERT` with runtime fail-fast checks today, written to migrate to
   native contracts without changing call sites.

Each landed with a ctest regression test; the suite is **32/32 green** at C++26.

## Migration plan — native contracts

When a contracts-capable compiler is available (`__cpp_contracts` defined):
- `JAC313_PRE(cond)` / `JAC313_POST(cond)` → native `pre(cond)` / `post(r: cond)` on the
  function declarator.
- `JAC313_ASSERT(cond)` → `contract_assert(cond)`.
The shim header is the single place to flip; call sites are written to port mechanically.

## Building with C++26

```bash
# RHEL: activate the toolset first (system g++ is too old)
source /opt/rh/gcc-toolset-15/enable
cmake -G Ninja -S . -B build-gcc15 -DCMAKE_CXX_COMPILER=g++ -DJAC313_BUILD_STORE_TESTS=ON
cmake --build build-gcc15 && ctest --test-dir build-gcc15
# clang is the second gate (clang 21 supports -std=c++26)
```

## Candidate next adoptions (not yet done)
`= delete("reason")` once a class adds deleted specials; pack indexing / `_` placeholders
in variadic code; broader contract call-sites at module boundaries; native contracts on GCC 16.
