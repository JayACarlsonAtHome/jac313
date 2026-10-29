// File:    docs/Cpp26_Adoption.md
// Purpose: What C++26 is available on our toolchains, what v002 has adopted, and the
//          plan for the rest; contracts are native on GCC 16 (see Contracts).

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
  - Contracts (`jac313::contracts`): `JAC313_PRE` / `JAC313_POST` / `JAC313_ASSERT`. On GCC 16 the whole v002 tree builds with `-fcontracts` and they are native `contract_assert`; on clang and gcc15 they are runtime fail-fast checks. See [Contracts](#contracts).
- Most of what enables "C++26 mode" today is preparation and a few targeted safety improvements rather than broad use of new language capabilities.

## Toolchains & feature availability (measured 2026-06)

Detected via `g++ -std=c++26 -dM -E -x c++ <(echo '#include <version>')` and direct
compiles — **not** from memory.

| Toolchain | C++26 (`-std=c++26`) | Notes |
|---|---|---|
| system g++ 11.5 | ❌ | C++20 max — not used for v002 |
| **gcc-toolset-15 (GCC 15.2.1)** | ✅ | `__cplusplus = 202400`; the standard build compiler |
| **clang 21.1.8** | ⚠️ ✅ | second gate — builds **only after a workaround**; see [the dynamic-width `std::print` finding](#toolchain-finding-clang-rejects-dynamic-width-stdprint-in-c26) below |
| **GCC 16.2.1** (Fedora 44 system g++) | ✅ | `-fcontracts` → `__cpp_contracts 202502L` (measured 2026-10-29); v002 builds with native contracts here |
| clang 22.1.8 (Fedora 44) | ✅ | no `__cpp_contracts` (measured 2026-10-29) |

**Available now (feature-test macros present on GCC 15.2):** `__cpp_pack_indexing`,
`__cpp_placeholder_variables`, `__cpp_deleted_function` (`= delete("reason")`),
`__cpp_static_assert` (user-generated messages), `__cpp_constexpr = 202406`,
`__cpp_structured_bindings = 202403`, `__cpp_lib_saturation_arithmetic`,
`__cpp_lib_span` (`span::at`), `__cpp_lib_format/print/ranges`, `__cpp_lib_text_encoding`.

**Contracts** (P2900): only GCC 16 with `-fcontracts`. GCC 15.2, clang 21 and clang 22 have no
`__cpp_contracts` and reject the `pre()/post()/contract_assert()` syntax, which is why call sites
use the `JAC313_*` macros (see [Contracts](#contracts)).

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

## Re-measured 2026-10-16 (this box: gcc-toolset-15 15.2.1, clang 21.1.8)

Same method as above (`-dM -E` of `<version>` at `-std=c++23` and `-std=c++26`). The
toolchains on RHEL 9.8 have not changed since June, but the list above was incomplete.
Also present under `-std=c++26` on **both** compilers unless marked:

- Language: `__cpp_pp_embed` (`#embed`, **gcc only**), `__cpp_variadic_friend`,
  `__cpp_constexpr 202406`, `__cpp_structured_bindings` (202403 clang / 202411 gcc).
- Library: `ranges_concat`, `ranges_cache_latest`, `ranges_to_input`, `constexpr_new`,
  `span_initializer_list`, `to_string` (P2587, `std::to_string(double)` = `format("{}")`),
  `fstream_native_handle`, `reference_wrapper`, `is_virtual_base_of`, `ratio`,
  `algorithm_default_value_type`, `string_view 202403` (`string + string_view`).

**The clang dynamic-width finding still bites new code:** the PostgreSQL / read-back port
added `jac313_store_dataset` (`Store/tests/matrix/dataset.cpp`), whose `"{:>{}}"` field
numbers built on gcc and broke clang at C++26 with the same `__check_dynamic_spec` error.
Fixed by padding with a small helper instead of a dynamic width (v001 kept identical).
Rule of thumb for v002: **no `{:..{}}` dynamic width/precision in `std::format`/`print`**;
use `jac313_rt_print` or pad by hand.

**Strict-flags probe** (not part of the build; one-off, `-Wall -Wextra -Wpedantic -Wshadow
-Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Woverloaded-virtual -Wnull-dereference
-Wimplicit-fallthrough -Wformat=2`, gcc with `-fhardened -Whardened -O1`, clang with
`-D_GLIBCXX_ASSERTIONS -ftrivial-auto-var-init=zero`): both compile with **0 errors and 0
`-Wpedantic` diagnostics**, so the code is ISO-clean at C++26. About 25 distinct warning
sites remain, all pre-existing and none correctness bugs: old-style casts (Qlite's
`Sqlite.body.hpp` sqlite3 constants, two `process`/`runner` casts), unused helpers in
`store_bench.cpp` / `compiler.cpp` / `section.cpp`, one-line `if (...) x; return y;`
lambdas (`-Wmisleading-indentation`), one `snprintf` width warning in `host_label`, and
`-Wnull-dereference` false positives inside libstdc++ `<streambuf>`.

**What GCC 16 adds when gcc-toolset-16 reaches RHEL 9** (vendor pages, 2026-10-16):
contracts (`-fcontracts`, `__cpp_contracts 202502L`), reflection (`-freflection`,
`__cpp_impl_reflection`), erroneous behaviour for uninitialized reads (zero-init by
default at C++26), constexpr exceptions, expansion statements, `std::inplace_vector`,
`std::simd`, `<debugging>`, `function_ref`, `optional<T&>`. clang 23 has none of
contracts / reflection. `std::runtime_format` is renamed `std::dynamic_format` (P3953R3)
and `std::add_sat` etc. `std::saturating_add` etc. (P4052R0) — the jac313 shims cover both.

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
4. **Contracts** (`Setup/include/jac313/Setup/contracts.hpp`): `JAC313_PRE/POST/ASSERT`,
   native `contract_assert` on GCC 16, runtime fail-fast checks on clang and gcc15.

Each landed with a ctest regression test; the suite is **32/32 green** at C++26.

## Contracts

Call sites use the `jac313::contracts` macros (`JAC313_PRE` / `JAC313_POST` / `JAC313_ASSERT`)
so one source builds on every v002 compiler:

| Compiler | What the macros are | Set by |
|---|---|---|
| **GCC 16** | native C++26 `contract_assert` (P2900), `enforce` semantic: a violation terminates | v002 `CMakeLists.txt`: `-fcontracts -fcontract-evaluation-semantic=enforce` on the whole tree (compile and link) |
| clang (22 has no `__cpp_contracts`), gcc15 | runtime check + `std::abort()` with file/line | `contracts.hpp` fallback |
| any, `-DJAC313_CONTRACTS_ENABLED=0` | no-ops | `contracts.hpp` |

Contracts in real code (programmer errors only; bad input from files keeps its own refusals):

- `ts_store::save_event` — the event id must be inside the preallocated rows (a store filled
  past `max_threads × events_per_thread` used to write past its buffer).
- `ts_store::persisted_event` — `id < rows_.size()`.
- `BinaryEventLog::append_event` — category, payload, ints and dbls each fit the record's
  16-bit lengths (a longer field wrote a truncated length ahead of the full bytes), and the
  write ends inside the mapping.

Tests: `jac313_contracts_test`, `jac313_contracts_disabled_test`, `jac313_contracts_enforce_test`
(native `pre()` on a declarator when `__cpp_contracts` is defined), and
`jac313_store_contracts_test` (a store filled past capacity terminates).

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

### Format checks (always on, 2026-10-19)

Every build, with any compiler, type or option, has `-Wformat=2 -Werror=format`
(`add_compile_options` in v001 and v002 `CMakeLists.txt`): a printf-style call whose arguments
do not match its format, such as a 64-bit value given to `%d`, is a build error instead of
undefined behaviour at run time. `-Wformat=2` used to come only with `JAC313_STRICT`.
Turning it on found one real case, now fixed: `host_label()` in `shared/jac313_results_db.hpp`
wrote `"jac313-%03lld"` into a 24-byte buffer, which a large or negative id could cut short
(GCC's `-Wformat-truncation`); the buffer is 32 bytes now. The U++ packages (Jac313Runner,
JTextGui) have the same flags as package `options` in their `.upp` files.

### Optional strictness (CMake options, all OFF by default)

| Option | Adds |
|---|---|
| `JAC313_STRICT` | `-Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Woverloaded-virtual -Wimplicit-fallthrough` (the format checks are always on, above) |
| `JAC313_STRICT_WERROR` | `-Werror` on top of `JAC313_STRICT` (fails today: about 25 pre-existing warning sites) |
| `JAC313_HARDENED` | `-D_GLIBCXX_ASSERTIONS -ftrivial-auto-var-init=zero -fstack-protector-strong -fstack-clash-protection -fcf-protection=full`, plus `-D_FORTIFY_SOURCE=3` outside Debug (gcc's `-fhardened`, spelled so clang takes it) |

```bash
cmake -G Ninja -S . -B build-strict -DJAC313_BUILD_STORE_TESTS=ON -DJAC313_STRICT=ON -DJAC313_HARDENED=ON
```

From the runner: `./jac313_test_cli --ctest --smoke --strict --hardened` (also in v001, and as
two checkboxes in Jac313Runner). These build in `build-<cc>-strict-hardened` and record the
build type as `Debug-strict-hardened`, so their results, logs and PG tables never mix with
the everyday `Debug` ones. They apply to the ctest/smoke builds only; bench and valgrind
builds ignore them.

Checked 2026-10-16: gcc 15 Debug, clang 21 Debug and gcc 15 Release with both options
build with 0 errors and pass ctest 42/42. Keep `JAC313_HARDENED` out of bench builds
(it costs speed) and valgrind builds (zero-init hides the uninitialized reads memcheck finds).

## Candidate future adoptions (not yet decided)
`= delete("reason")`, pack indexing, placeholder variables (`_`), broader use of native
contracts, and other C++26 language features will be reconsidered in the next 12 months.
