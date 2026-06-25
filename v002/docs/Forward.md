//File:    docs/Forward.md
//Date:    2026-06-25
//Purpose: What's next — open threads parked at the close of the store_bench / identity session.

# Forward — open threads

This session made **store_bench the test driver** (the 10 curated configs *are* the tests)
and **retired the old matrix reporting**. State at close, then what's next.

## Done this session (v002)

- **Legacy matrix machinery retired.** Deleted `results_db`, `test_summary_render`,
  `build_benchmark`, `matrix_log_metrics`, `matrix_manifest`. `matrix` is now a
  **correctness gate only**: `run | runner | verify | verify-lite`. It opens no database
  and writes nothing to `test-summary/`. (Closes the old "retire the matrix runner" thread:
  the *reporting/DB* half is gone; the runner stays as the functional + valgrind gate.)
- **store_bench = the test driver: same 10 configs, two scales × two builds.**
  - `--smoke` (≡ `--suite --smoke`): the 10 configs capped to **≤10k events**, run as a
    **pass/fail correctness gate** (all 10 must pass; structural verify included). **Not
    recorded.** Run in **debug AND release**.
  - `--suite`: the **recorded** full-numbers benchmark (median + low–high band). **Release only.**
  - So the test matrix is `smoke{debug,release} + full{release}`, × `{gcc15, clang}`.
- **Machine identity + anonymization.**
  - `group_id` is keyed on the **full `(cpu, cores, ram_gb, host, os)` identity** — hostname
    alone is unsafe (four boxes could all be `localhost.localdomain`). `group_id_for`, the
    backfill, and `--clear` all use this identity.
  - **`--anonymize`** scrubs `host` → `jac313-<group_id>` (zero-padded) and **auto-re-renders**.
  - **`host_label.local`** (gitignored, searched from CWD *up* the tree) / `$JAC313_HOST_LABEL`
    set the label **at record time**, so a box records `jac313-NNN` from the start — no real
    hostname ever enters the DB, and re-runs keep matching the same group.
- **Report = index + per-run pages.** `test-summary/README.md` is the summary index
  (`group_id · HW Details · Run_NNN · Max Ops/Sec`); `test-summary/Run_NNN.md` holds the full
  clang-vs-gcc tables. `store_bench --report` **writes the files** (no stdout redirect),
  defaulting to the DB's dir (or `--out`), and **prunes stale `Run_NNN.md`** for cleared groups.
- **Recorded & anonymized:** `Run_001` = `jac313-001` (Fedora 44, gcc16/clang22),
  `Run_002` = `jac313-002` (RHEL 10.2, gcc15/clang21). Finding: **jText is the fastest durable
  backend** (~1.8–2.0M ops/sec vs binary ~0.8M, sql ~0.66M) — credible because the flush is
  timed *inside* the clock (Bloopers #1).

## On a NEW machine (the destination)

1. `git clone` → `cd v002 && ./bootstrap.sh`. It senses the toolchain; if the baseline is
   missing it generates a reviewable `Setup.sh` and **stops** (run it with sudo, re-run
   bootstrap). Recipes cover **rhel-family / fedora / linuxmint**; other distros (Ubuntu,
   Debian, Arch, …) get a "no recipe" skeleton → **manual install** (it tells you what's
   missing — it does not crash). Hard floor: g++ ≥ 14 (15 ideal), clang ≥ 20 (gcc-only works
   without it), CMake ≥ 3.26, Ninja ≥ 1.11, sqlite3 dev header, valgrind, a UTF-8 locale.
2. Drop a `host_label.local` with this box's label (e.g. `jac313-003`) so it records anonymized.
3. Build store_bench in a **Debug** tree and a **Release** tree (per compiler), then:
   - `--smoke` from each tree → correctness (expect **10/10**).
   - `--suite` from the Release tree(s) → records **Run_003**.
   - `--report` → regenerates the index + `Run_003.md` (drops in alongside Run_001/002).
4. `matrix verify-lite` = the valgrind gate (also the pre-push hook).

> Running across debug/release/compilers is **manual** — we dumped the bench CLI orchestration
> command by decision (the suite is short). A thin driver could be revived if the manual dance
> gets old; not needed yet.

## Open threads

### Carry v002 → v001
The entire store_bench world is **v002-only by request** — v001 is untouched, and is now far
behind. The carry is large: `store_bench.cpp` (suite/smoke/report/anonymize/host-label +
`group_id` schema + `(hardware,host,os)` identity), `bench_stats.hpp`, the index/Run-page
layout, and the legacy-matrix removal in `tools/jac313_test_cli`. Swap `Store::v002`→`v001`,
`cxx_std_26`→`23`, add the CMake target; the instrument is C++20-clean.

### Stale docs / leftovers to fix
- **`docs/RunAllTests.md`** (dated 2026-06-19) documents the **removed** `matrix run-all` +
  the old DB/render — now wrong. Rewrite as the store_bench smoke/full runbook, or delete it.
- **`time_util.hpp`** is orphaned (not in CMakeLists, referenced by nothing) — safe to delete.

### Nice-to-haves
- **"Why jText wins durable"** — a short doc note: streaming `JTextWriter` does big sequential
  appends with a cheaper flush than binary's per-record sync. (Owner's design; worth recording.)
- Index **`group_id` column** — could drop it (`Run_NNN` already encodes the number); kept for now.

### Still open from earlier sessions
- **Vendor the jText CLI tools** (`jtext_xtox_sql` / `jtext_from_sql`) into v001/v002; wire
  their ctests. `from_sql` is **PostgreSQL-only** (`psql`) — gate its live test (arg-validation
  otherwise). _(Qlite is fine — its unit tests already came over.)_
- **jText ↔ SQL round-trip ctest** — add a SQLite→jText emitter **upstream in the jText repo
  first** (per `PROCESS.md`), then forward-port, so the round-trip (`jText → SQL → SQLite →
  jText`) is a self-contained ctest, not a psql-gated skip.

## Kept by decision
- **10M durable grid** — keep; it proved scaling is **super-linear** (durable throughput rises
  at 10×, SQL ~2×, as the fixed front edge amortizes). Make it opt-in (`--suite-scaling`) only
  if the SQL@10M populate leg (~100 s/compiler) becomes annoying.
