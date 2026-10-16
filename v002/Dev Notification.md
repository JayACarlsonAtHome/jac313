# Dev Notification

_`dev-work` — 2026-06-26 · still in progress (see the README banner)_

The **test/results reporting** has been substantially rebuilt (no more cluster-yuck).
We're still making changes and polishing the rendered pages.
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
- **Compile-time matrix** (`compiler-build-times/`): per smoke+bench test, the **compile+link
  time** · status · binary size across **(front-end × compiler)**, **host-scoped** (one section
  per `jac313-###` with its cpu/cores/ram/os). Folded into the CLI as `jac313_test_cli build-times`
  — `.ninja_log` timing, gap-filled per host (builds only what's missing), and it **runs at push**
  (a no-op once this host is complete).

## Verify / valgrind
- Known **false positives are scrubbed before recording** via `suppressions/jac313.supp`
  (the std::atomic helgrind/DRD blind spot). Only real errors reach the DB.

## What this means for you
- `results.db` is tracked but **regenerable** — re-run the gates to repopulate.
- The **pre-push gate is unchanged and fast** (`version-check` + `verify-lite`).
- Things may still shift under you for a bit — patience appreciated.

## 2026-10-16 — PostgreSQL sink, read-back, limits, runner GUI (ported from v001)
- **PostgreSQL persist backends** `pg-local` / `pg-remote` (optional build, libpq 17+);
  tables in `store_test`, one "last run" per host + configuration. See `docs/store/postgres.md`.
- **Read-back** of every persisted dataset (`--readback off|count|sample|all`) plus a
  self-test that damages each backend and must be caught (`jac313_store_readback_selftest`).
- **Limits** `--max-runs`, `--max-records`, `--max-persist-rows`; `--persist a,b,...`;
  `--summary-json` for the **Jac313Runner** GUI (`gui/`, built by `jac313_test_cli gui-build`).
- **SQLite sink fix:** re-running into an existing `.db` kept the old rows; tables are now
  dropped on open.
- **testRun** gains read-back and PG-latency columns (not mirrored to the R430 yet).
- **C++26 re-check** recorded in `docs/Cpp26_Adoption.md` and `tools/conformance/baseline.md`.
