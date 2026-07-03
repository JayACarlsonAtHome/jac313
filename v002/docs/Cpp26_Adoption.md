// File:    docs/Cpp26_Adoption.md
// Purpose: What C++26 is available on our toolchains, what v002 has adopted, and the
//          migration plan for features that aren't compiler-ready yet (contracts).

# C++26 Adoption — v002

**Current state (as of 2026-07):** v002 is compiled as C++26 (`cxx_std_26`), but adoption of new C++26 language and library features remains limited and deliberate. We are mostly "compiling higher" to prepare for future features (especially contracts) while using only a small number of C++26-era facilities today, often through compatibility shims or workarounds.

We are not actively expanding C++26 usage at this time. Sometime within the next 12 months we plan to re-evaluate the toolchain landscape and adopt additional C++26 features where they provide clear value.

v002's build baseline is **`cxx_std_26`** (see the per-module `target_compile_features`).
The code compiles clean under `-std=c++26` on the toolchains below; this page records
*what the compilers actually support*, what we've adopted, and what's deferred.

## Current reality

- We request C++26 via `target_compile_features( ... cxx_std_26 )`.
- Real use of new *language* features (pack indexing, placeholder `_`, `= delete("reason")`, native `pre()/post()` on declarators outside of tests, etc.) is minimal to none in the main libraries.
- We do use a few C++26 *library* facilities or behaviors:
  - Saturating arithmetic (via a portable shim that matches the C++26 semantics).
  - `std::runtime_format` (required as a workaround for changes in format string evaluation rules under C++26).
  - A contracts shim (`jac313::contracts`) that provides `JAC313_PRE` / `JAC313_POST` / `JAC313_ASSERT`. These currently expand to runtime checks on our supported compilers. One test function exercises the native `pre (expr)` syntax when `__cpp_contracts` is available.
- Most of what enables "C++26 mode" today is preparation and a few targeted safety improvements rather than broad use of new language capabilities.

## Toolchains & feature availability (measured 2026-06)

Detected via `g++ -std=c++26 -dM -E -x c++ <(echo '#include <version>')` and direct
compiles — **not** from memory.

| Toolchain | C++26 (`-std=c++26`) | Notes |
|---|---|---|
| system g++ 11.5 | ❌ | C++20 max — not used for v002 |
| **gcc-toolset-15 (GCC 15.2.1)** | ✅ | `__cplusplus = 202400`; the standard build compiler |
| **clang 21.1.8** | ⚠️ ✅ | second gate — builds **only after a workaround**; see [the dynamic-width `std::print` finding](#toolchain-finding-clang-rejects-dynamic-width-stdprint-in-c26) below |
| gcc-toolset-16 / GCC 16 | n/a here | not packaged for RHEL 9 (Fedora-only); would add contracts/reflection |

**Available now (feature-test macros present on GCC 15.2):** `__cpp_pack_indexing`,
`__cpp_placeholder_variables`, `__cpp_deleted_function` (`= delete("reason")`),
`__cpp_static_assert` (user-generated messages), `__cpp_constexpr = 202406`,
`__cpp_structured_bindings = 202403`, `__cpp_lib_saturation_arithmetic`,
`__cpp_lib_span` (`span::at`), `__cpp_lib_format/print/ranges`, `__cpp_lib_text_encoding`.

**NOT available yet:** ISO **contracts** (P2900) — no `__cpp_contracts`; the
`pre()/post()/contract_assert()` syntax errors on GCC 15.2 and clang 21. Needs GCC 16
or a contracts-enabled build.

## Toolchain finding: clang rejects dynamic-width `std::print` in C++26

Going to two standards in one real codebase is supposed to surface compiler differences
that hello-world never reaches. It did. **The same `std::print` source that v001 (C++23)
compiles under clang fails to compile in v002 (C++26) under clang** — gcc takes both.

Minimal reproducer: [`findings/clang_cpp26_dynamic_format.cpp`](findings/clang_cpp26_dynamic_format.cpp)

```cpp
std::string msg = "hi";
int width = 8;
std::print("{:^{}}\n", msg, width);   // center 'msg' in a *runtime-width* field
```

| Compiler | `-std=c++23` | `-std=c++26` |
|---|---|---|
| gcc 15.2.1 | ✅ | ✅ |
| clang 21.1.8 | ✅ | ❌ **error** |

clang 21 + libstdc++ 15, `-std=c++26`:

```
error: call to consteval function 'basic_format_string<...>' is not a constant expression
note:  undefined function '__check_dynamic_spec<int, unsigned, long long, unsigned long long>'
       cannot be used in a constant expression
```

**Root cause.** libstdc++'s consteval format-string check calls a *declared-but-undefined*
helper, `__check_dynamic_spec`, for dynamic-width/precision specs (`{:>{}}`, `{:^{}}`). gcc's
constant evaluator handles it; clang's C++26 evaluator refuses to call an undefined function
during constant evaluation. The C++23 path never reaches that helper — which is exactly why
only the C++26 build breaks. A clang ↔ libstdc++ interaction defect, not invalid code.

**Workaround (what v002 ships).** Wrap the dynamic-width format strings in
`std::runtime_format(...)`, which opts those strings out of the consteval check:

```cpp
std::print(std::runtime_format("{:^{}}\n"), msg, width);   // builds on clang C++26
```

Applied to the ~21 dynamic-width call sites in `Store/.../impl_details/{printing,diagnostic}.hpp`.
Trade-off: those strings lose compile-time format checking (they're now validated at runtime);
they are fixed internal literals, so the risk is negligible. With the workaround, v002 builds
**green on both gcc 15 and clang 21** at C++26, and the full matrix runs on both.

## Adopted so far (conservative / safety-focused)

Adoption has been deliberately limited. The main things pulled in from the C++26 world
are safety-related library facilities (sometimes via shims) and the contracts preparation
layer. We are not yet using most new C++26 language features in production code.

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

## Current contracts situation

We ship a portable `jac313::contracts` shim (`JAC313_PRE` / `JAC313_POST` / `JAC313_ASSERT`).
On current supported toolchains (GCC 15 toolset, clang 21) these expand to runtime checks.
A small number of test functions exercise the native `pre (expr)` syntax when the compiler
defines `__cpp_contracts` (currently only on GCC 16 builds).

We are not expanding contract usage at this time.

## Future plans

Sometime within the next 12 months we will re-evaluate the C++26 feature and toolchain
landscape and decide what additional features to adopt.

## Building with C++26

```bash
# RHEL: activate the toolset first (system g++ is too old)
source /opt/rh/gcc-toolset-15/enable
cmake -G Ninja -S . -B build-gcc15 -DCMAKE_CXX_COMPILER=g++ -DJAC313_BUILD_STORE_TESTS=ON
cmake --build build-gcc15 && ctest --test-dir build-gcc15
# clang is the second gate (clang 21 supports -std=c++26)
```

## Candidate future adoptions (not yet decided)
`= delete("reason")`, pack indexing, placeholder variables (`_`), broader use of native
contracts, and other C++26 language features will be reconsidered in the next 12 months.
