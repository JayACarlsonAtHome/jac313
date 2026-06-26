# Dev Notification

_`dev-work` — 2026-06-26 · still in progress (see the README banner)_

The **test/results reporting** has been substantially rebuilt — "cluster-yuck → work of art."
Here's what changed so far.

## Unified results database
- All results now live in **one** normalized `test-summary/results.db`
  (tables: `testType`, `testList`, `compiler`, `parameter`, `run`, `testRun`).
- Every kind records here: **ctest, smoke, bench, verify, build**.
- Schema + dimension helpers consolidated into one shared header
  (`shared/jac313_results_db.hpp`) so the two writers can't drift.
- **`bench_results.db` is retired and deleted** — bench writes only `results.db`.

## Reporting — `jac313_test_cli --report`
- Writes `test-summary/`: a **top-level index** (`README.md`) linking to per-area pages —
  `compiler/`, `ctest/`, `smoke/`, `bench/`, `verify/`, `build/`.
- Per-type pages are **compiler-to-compiler comparisons** (clang ↔ gcc, side by side).
- **bench** is head-to-head: per compiler **median ops/sec · low–high band · output size**.
- Integers use **locale** thousands separators; band and sizes are human-readable.
- **verify** cells show **pass/fail** explicitly.

## New axes / features
- **`import_std`** captured — the build front-end is now *headers / modules / modules+import-std*.
- **Build-time matrix** (`build/`): per ctest test, the **compile+link time** (compile only,
  not the run) across **(front-end × compiler)**. clang import-std shows **NA** (a g++-15
  pilot only today, by design — fills in automatically when clang supports it). Drive it with
  `tools/build_matrix.sh [tests…]`. It is **separate from the pre-push gate** — never runs on push.

## Verify / valgrind
- Known **false positives are scrubbed before recording** via `suppressions/jac313.supp`
  (the std::atomic helgrind/DRD blind spot). Only real errors reach the DB.

## What this means for you
- `results.db` is tracked but **regenerable** — re-run the gates to repopulate.
- The **pre-push gate is unchanged and fast** (`version-check` + `verify-lite`).
- Things may still shift under you for a bit — patience appreciated.
