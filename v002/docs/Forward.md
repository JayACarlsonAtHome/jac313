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
  `Run_002` = `jac313-002` (RHEL 10.2, gcc15/clang21), `Run_003` = `jac313-003` (RHEL 9.8,
  20-core, gcc15/clang21 — **recorded this session**). Finding: **the durable ranking is
  hardware/disk-dependent, and that's the point of per-host recording.** On Run_001/002 jText
  wins (~1.8–2.0M vs binary ~0.8M, sql ~0.66M); on Run_003 it **flips** — binary is fastest
  (~2.1M) > jText (~1.75M) > sql (~1.3M), consistent across both compilers there. Both are
  honest: the flush is timed *inside* the clock (Bloopers #1), so the winner just tracks how
  cheap `msync` is on that box's storage.

### Lessons from the jac313-003 bring-up (this session)
- **`bootstrap.sh` builds only the test runner (Debug).** The benchmark is a separate, opt-in
  build: `-DJAC313_BUILD_STORE_TESTS=ON -DCMAKE_BUILD_TYPE=Release`, then
  `--target jac313_store_bench`. Don't expect `store_bench` after a bare bootstrap.
- **Set the host label *before the first record*.** We anonymized group 3 after the gcc run, then
  the clang `--suite` recorded the *real* hostname again — which no longer matched the
  `(cpu,cores,ram,host,os)` identity, so it would have opened **group 4**. Fix used:
  `JAC313_HOST_LABEL=jac313-003` on the clang run. Cleaner: drop `host_label.local` first and
  never anonymize mid-group. (Reinforces the "On a NEW machine" step 2 — it's load-bearing.)
- **Multi-compiler is manual for the bench.** `--suite` runs whatever compiler built the binary;
  there's no auto gcc+clang. Build once per compiler, run `--suite` into the **same** DB — the
  `compiler` column splits the rows, and `--report` renders the clang-vs-gcc tables.
- **clang on RHEL builds under the toolset.** Configure/build clang inside
  `scl enable gcc-toolset-15 --` so it picks up the gcc-15 libstdc++ (C++26); bare system clang
  has too old a libstdc++.
- **The durable-binary run leaves a big `bench.bin` (~0.8 GB) in `v002/`** — *not* gitignored.
  Delete it after a run (and consider adding `bench.bin` + stray `*.jtext`/`*.sql`/`*.db` bench
  outputs to `.gitignore`).
- **Pre-push gate works as advertised:** the hook ran version-check + valgrind verify-lite
  (memcheck 21 + helgrind 5 + drd 5 = **31/31 clean**) before each push to `dev-work`. Bypass
  with `git push --no-verify` if ever needed.

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
- ~~**`docs/RunAllTests.md`** documents the removed `matrix run-all` + old DB/render.~~
  **Done (this session):** rewritten as the three-gate runbook — functional `matrix
  run/runner`, `store_bench` throughput, valgrind `verify-lite` — matching the live CLI.
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
