# Results — what a build + test run actually produces

> **v002 recorded its first results matrix** — `rhel-9.8 / ssd`, 2026-06-23. The full
> **17-combo** battery on **both gcc15 and clang** at C++26: **17/17 combos OK, 116/116
> scenarios each, 0 failures, 0 valgrind errors.** These are v002's own numbers (it does not
> inherit v001's).

> **Before trusting any `ops/sec` figure, read [HowToReadResults.md](HowToReadResults.md).**
> The headline "Peak ops/sec" is the in-memory hot path, *not* a persist backend's write
> speed. That page explains which number is which (methodology is version-independent).
>
> **Methodology note:** these figures use the **legacy "Peak ops/sec" (fastest-of-N)** metric
> from the functional-matrix logs. Current throughput runs use the curated
> [`store_bench` suite](Benchmarks.md) — headlined by a **median + low–high band**, with durable
> writes flushed inside the timed region. New numbers will be reported that way.

## Headline numbers (rhel-9.8, ssd, full tier)

Peak ops/sec is the in-memory hot path (see HowToReadResults.md); durable-to-disk is far lower.

| Compiler | Build | Peak ops/sec (in-mem) | Avg |
|---|---|---|---|
| clang 21 | Release | **25.36M** | 17.5M |
| gcc 15 | Release | 24.84M | 17.6M |
| clang 21 | Debug | 14.8M | 10.8M |
| gcc 15 | Debug | 17.3M | 12.1M |

**C++26 v002 (~25M) matches C++23 v001 (~25M) — no throughput regression** from the standard
bump, and clang Release edges out gcc even through the `std::print` workaround below.

## Two toolchain findings from this run

The two-standard design earned its keep — building v002 on both compilers surfaced two real bugs:

1. **clang rejects valid C++26 `std::print`** (dynamic-width specs) that gcc accepts — a
   genuine compiler defect. Worked around with `std::runtime_format`. See
   [Cpp26_Adoption.md](Cpp26_Adoption.md#toolchain-finding-clang-rejects-dynamic-width-stdprint-in-c26)
   and the [reproducer](findings/clang_cpp26_dynamic_format.cpp).
2. **our `import std` + textual `#include <numeric>` bug** — a std header leaked into the
   module's `export namespace`; gcc *correctly* rejected it. Fixed by guarding the include
   under `JAC313_STORE_IMPORT_STD` like every other std header.

## Current state

| Check | Status |
|-------|--------|
| Builds — gcc15 **and clang21**, modules + textual + import-std | ✅ clean (both compilers, C++26) |
| Full matrix (gcc15 + clang × Debug/Release × modules/textual × Smoke/xFull) | ✅ **17/17 combos, 116/116 each** |
| `matrix verify-lite` (valgrind memcheck + helgrind/DRD, ctest) | ✅ 31/31 clean |

For v001's per-platform numbers see [v001/docs/Results.md](../../v001/docs/Results.md).

## Recording v002's own results

Everything regenerates from scratch, written under `v002/` only:

```bash
cd ..              # the v002 root
./bootstrap.sh                       # sense toolchain → build the runner → drop the symlink
./jac313_test_cli --ctest --smoke    # functional base check (correctness)
./jac313_test_cli --bench --report   # throughput → test-summary/bench_results.db + rendered pages
```

Per-run throughput detail — clang-vs-gcc per machine — appears in the
[test-summary hub](../test-summary/) and `test-summary/bench_results.db` as runs complete.

Part of the [jac313](../../README.md) project.
