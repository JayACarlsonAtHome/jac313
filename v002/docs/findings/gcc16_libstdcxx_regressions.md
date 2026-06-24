// File:    docs/findings/gcc16_libstdcxx_regressions.md
// Date:    2026-06-24
// Host:    jac313-008 (Fedora 44, gcc 16.1.1 "Red Hat 16.1.1-2", clang 22.1.8)
// Purpose: Two C++26 library symbols that v002 used compiled under gcc15 but are
//          ABSENT in this gcc16 libstdc++ — found when bringing up the gcc16 box.

# GCC 16 / libstdc++ 16 — two missing C++26 library symbols

Bringing v002 up on the Fedora/gcc16 box (for native P2900 contracts) surfaced two
libstdc++ regressions. Both compiled fine under gcc15; both are gone in this gcc16
build. Importantly, **clang on this box uses the same libstdc++ 16**, so clang was
broken too — these were not gcc-only.

Both are now insulated behind small jac313 shims, so the code no longer depends on
the volatile state of these bleeding-edge library features and still builds on
gcc15 / gcc16 / clang.

## 1. `std::runtime_format` (P2918) — removed

`<format>` no longer declares `std::runtime_format` (grep of the installed headers:
0 matches; `__cpp_lib_format` = `202603L`). v002 used it to feed *runtime* (dynamic-
width `{:>{}}`) format strings to `std::print`:

```cpp
std::print(std::runtime_format("{}{:>{}}{}"), color, value, width, reset);
```

That spelling existed to dodge a **clang `std::print` consteval defect**: clang's
compile-time format checker rejects dynamic width/precision specs
(`__check_dynamic_spec ... cannot be used in a constant expression`). So neither
"keep the wrapper" (gcc16 lacks `runtime_format`) nor "drop the wrapper" (clang
rejects the bare literal) works across both compilers.

**Fix:** `jac313_rt_print` / `jac313_rt_println` (Store `includes.hpp`) route through
`std::vprint_unicode(stdout, fmt, std::make_format_args(args...))` — a *runtime*
format path: no consteval check (dodges clang) and no `runtime_format` (dodges
gcc16). Args bind as `const` lvalues inside the helper so `std::make_format_args`
(post-P2905, lvalue-only) accepts caller temporaries. 21 call sites in
`Store/.../impl_details/{diagnostic,printing}.hpp` were converted.

## 2. `std::add_sat` / `std::mul_sat` (P0543) — renamed/absent

`<numeric>` advertises `__cpp_lib_saturation_arithmetic` (`202603L`) but the standard
names `std::add_sat` / `std::mul_sat` are absent. The dedicated header
`<bits/sat_arith.h>` defines only the internal spellings `saturating_add` /
`saturating_sub` / `saturating_mul` / `saturating_div` — the public `*_sat` names
were not (yet) provided in this snapshot.

v002 used `std::add_sat` / `std::mul_sat<size_t>` for overflow-safe capacity / offset
math in `ts_store.hpp` and the binary-log persistence path.

**Fix:** `jac313::add_sat` / `jac313::mul_sat` (`Store/.../headers/sat_compat.hpp`),
implemented with `__builtin_{add,mul}_overflow` and clamping to the type's max
(unsigned) or min/max (signed) — matching `std::*_sat` semantics on gcc15, gcc16 and
clang. 8 call sites across `ts_store.hpp`, `BinaryEventLog.hpp`,
`BinaryEventLogReader.cpp`, and the `saturating_capacity_test` were converted.

## Takeaway

A prerelease toolchain (Fedora ships prerelease GCC in even releases) can *remove or
rename* library symbols that an earlier prerelease shipped. Where v002 leans on the
newest C++26 library facilities, prefer a thin jac313 shim with a feature/`__builtin`
fallback over a direct `std::` dependency — it keeps the matrix green across the whole
gcc15 / gcc16 / clang spread instead of pinning to one libstdc++ snapshot.
