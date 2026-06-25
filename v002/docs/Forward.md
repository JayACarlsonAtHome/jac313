//File:    docs/Forward.md
//Date:    2026-06-25
//Purpose: What's next — open threads parked at the close of the results-DB session.

# Forward — open threads

The benchmark/results work landed in **v002**: `store_bench` → SQLite results DB (via Qlite)
→ `store_bench --report` page (clang-vs-gcc side by side, durable 1M + 10M scaling, on-disk
sizes), all script-free. These are the threads left open.

## Carry v002 → v001
The whole `store_bench` DB/report feature is **v002-only** by request — v001 is untouched.
Mirror it: copy `store_bench.cpp` + `bench_stats.hpp`, swap `Store::v002`→`v001` and
`cxx_std_26`→`23`, add the CMake target, re-populate, regenerate. Mechanical — the
instrument is C++20-clean.

## Vendor the jText CLI tools into jac313  ← sidetracked this session
jac313 has the jText *library* + a parser/validator test, but **`jtext_xtox_sql` /
`jtext_from_sql`** never came over from the standalone jText repo. Bring both into v001/v002,
wire their ctests; `from_sql` is **PostgreSQL-only** (`psql`), so gate its live test on psql
(arg-validation otherwise). _(Qlite is fine — it has no separate CLI tool; its unit tests
already came over as `Qlite/tests/v002_test.cpp`.)_

## jText ↔ SQL round-trip ctest  (do upstream first)
Open in the standalone jText repo's `TODO.md`: add a **SQLite→jText emitter** so the
round-trip (`jText → SQL → SQLite → jText`) is a self-contained ctest, not a psql-gated skip.
Per `PROCESS.md`, do it in the jText repo first, then forward-port.

## Results-page polish
- **`Benchmarks.md` sample** still shows the OLD single-table format — refresh it to the
  clang-vs-gcc + 1M/10M durable + size layout (or just point it at `test-summary/`).
- **GitHub render check** — the side-by-side compiler tables use an HTML `<table>` wrapping
  markdown `<td>` cells. Confirm GitHub renders the inner tables; if they show as raw
  `| … |`, switch the inner cells to HTML tables.

## Retire the matrix *runner* (optional)
Only the old reports + committed `test-summary/` pages were retired. `tools/jac313_test_cli`
still drives the multi-compiler/build/modules **correctness** sweep. Deciding its fate (slim
to a correctness-only runner, or replace with ctest-per-config + CI) is the next
consolidation step — needs a correctness-coverage replacement, not a straight delete.

## Kept by decision
- **10M durable grid** — keep. It answered "is scaling linear?" → **super-linear** (durable
  throughput rises at 10×, SQL ~2×, as the fixed front edge amortizes). Make it opt-in
  (`--suite-scaling`) only if the SQL@10M populate leg (~100 s/compiler) becomes annoying.
