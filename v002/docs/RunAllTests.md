//File:    docs/RunAllTests.md
//Date:    2026-06-25
//Purpose: One-stop runbook — the full jac313 test battery (correctness gate + throughput + valgrind)

# RunAllTests — the full jac313 test battery

There are **three gates**, run separately (there is no longer a single `run-all` driver):

1. **Functional correctness** — `matrix run` / `matrix runner` (ctest + the persist×output grid).
   A pass/fail gate; it opens **no database** and writes **nothing** to `test-summary/`.
2. **Throughput** — `store_bench` (the curated 10-config suite). `--smoke` is a fast correctness
   gate; `--suite` records the numbers. See [Benchmarks.md](Benchmarks.md) for the detail.
3. **Memory / thread** — `matrix verify-lite` / `verify` (valgrind memcheck + helgrind/DRD).
   `verify-lite` is also the pre-push hook.

> **gcc15 + clang are the gate on every platform** (gcc14 is excluded by design).
> The old `matrix run-all` / `matrix render` / `jac313_results.db` were **retired** — throughput
> and the committed report now live in `store_bench` + `test-summary/bench_results.db`. See
> [Forward.md](Forward.md).

See also: [Setup.md](Setup.md) · [Benchmarks.md](Benchmarks.md) · [README.md](../README.md)

---

## 0. Prerequisites (one-time)

Run everything **from `v002/`**.

```bash
cd v002 && ./bootstrap.sh        # senses toolchain, REQUIRES valgrind (+ dev headers), builds the runner
```

The runner lives at `./build-bootstrap/tools/jac313_test_cli`. Set these once per shell (RHEL
needs the toolset activated; Mint has `g++-15`/`clang` on `PATH`):

```bash
ACT="scl enable gcc-toolset-15 --"   # Linux Mint: ACT=""
CLI="./build-bootstrap/tools/jac313_test_cli"
```

---

## 1. Functional correctness — `matrix runner`

`matrix runner` builds + runs **one explicit configuration** (ctest + the persist×output
matrix) and reports pass/fail. The build dir is auto-named; nothing is recorded to a DB.

```bash
$ACT $CLI matrix runner --compiler gcc15 --build-type Debug --modules off --size smoke
```

| Option | Values |
|---|---|
| `--compiler` | `gcc15` \| `clang` (any `compilers.conf` label) |
| `--build-type` | `Debug` \| `Release` |
| `--modules` | `on` (module-native) \| `off` (textual) — default `on` |
| `--size` | `smoke` \| `full` |
| `--build-dir` | optional; default `build-<compiler>-<buildtype>-<modules>` |

`matrix run` (no dimensions) is the quick variant: it runs the matrix for the **current/auto**
compiler against an existing build dir — handy mid-edit. `runner` is the workhorse for a clean,
explicit cell.

### Every functional combo, spelled out

The full functional sweep is **{gcc15, clang} × {Debug, Release} × {modules on, off} × {smoke,
full}** — 16 cells. There's no `run-all` anymore, so run the ones you need (each auto-builds its
own `build-<compiler>-<buildtype>-<modules>` tree; `modules on` vs `off` never collide):

```bash
# --- gcc15 ---
$ACT $CLI matrix runner --compiler gcc15 --build-type Debug   --modules on  --size smoke
$ACT $CLI matrix runner --compiler gcc15 --build-type Debug   --modules on  --size full
$ACT $CLI matrix runner --compiler gcc15 --build-type Debug   --modules off --size smoke
$ACT $CLI matrix runner --compiler gcc15 --build-type Debug   --modules off --size full
$ACT $CLI matrix runner --compiler gcc15 --build-type Release --modules on  --size smoke
$ACT $CLI matrix runner --compiler gcc15 --build-type Release --modules on  --size full
$ACT $CLI matrix runner --compiler gcc15 --build-type Release --modules off --size smoke
$ACT $CLI matrix runner --compiler gcc15 --build-type Release --modules off --size full

# --- clang (textual only; clang module builds need clang-scan-deps-20 on PATH) ---
$ACT $CLI matrix runner --compiler clang --build-type Debug   --modules off --size smoke
$ACT $CLI matrix runner --compiler clang --build-type Debug   --modules off --size full
$ACT $CLI matrix runner --compiler clang --build-type Release --modules off --size smoke
$ACT $CLI matrix runner --compiler clang --build-type Release --modules off --size full
```

The heavy `--size full` cells are I/O-intensive — start day-to-day with the `smoke` lines.

---

## 2. Throughput — `store_bench`

`store_bench` is **not** built by `bootstrap` or the functional matrix — build it explicitly
(`-DJAC313_BUILD_STORE_TESTS=ON`, **Release** for real numbers), once per compiler:

```bash
$ACT cmake -G Ninja -S . -B build-bench -DCMAKE_BUILD_TYPE=Release -DJAC313_BUILD_STORE_TESTS=ON \
  -DCMAKE_CXX_COMPILER=/opt/rh/gcc-toolset-15/root/usr/bin/g++     # or g++-15 / /usr/bin/clang++
$ACT cmake --build build-bench --target jac313_store_bench
B=build-bench/Store/tests/matrix/jac313_store_bench
```

Then:

```bash
echo jac313-NNN > host_label.local                 # this box's label — BEFORE the first record
$ACT $B --suite --smoke                             # 10 configs ≤10k events — correctness gate (expect 10/10)
$ACT $B --suite --db test-summary/bench_results.db --jtext-ver v002.002   # recorded full run → group N / Run_NNN
$ACT $B --report --db test-summary/bench_results.db                       # render README index + Run_NNN.md
```

**For a second compiler:** rebuild `store_bench` with it (clang on RHEL must configure/build
**under `scl enable gcc-toolset-15 --`** for a C++26 libstdc++) and re-run `--suite` into the
**same** DB — the `compiler` column splits the rows; `--report` then renders the clang-vs-gcc
tables. There is no automatic gcc+clang sweep — it's manual by decision (the suite is short).

> **Set `host_label.local` before the first record, and never anonymize mid-group.** `group_id`
> is keyed on the full `(cpu, cores, ram_gb, host, os)` identity — record under the real hostname,
> anonymize, then run a second compiler, and its row no longer matches that identity and opens a
> *new* group. (`store_bench --anonymize` scrubs hostnames → `jac313-<group_id>`; run it **last**.)

The recorded test matrix is therefore: `--suite --smoke` in **Debug and Release**, and the
recorded `--suite` in **Release only**, each × `{gcc15, clang}`.

---

## 3. Memory / thread gate — `verify`

Valgrind **memcheck + helgrind + DRD** (all ship in the one valgrind package, which `bootstrap`
*requires*):

```bash
$ACT $CLI matrix verify-lite     # ctest representative set        (~31 checks, ~3 min)
$ACT $CLI matrix verify          # + full smoke-matrix sink sweep  (~60 checks, ~6 min)
```

Verdict-only (no DB write) — non-zero exit on any error. `verify-lite` is the pre-push gate
(installed by `bootstrap` into the shared parent `.git/hooks`); bypass once with
`git push --no-verify`.

---

## What the battery covers

| Gate | Command | Scope |
|------|---------|-------|
| Functional | `matrix runner …` | one explicit cell: ctest + persist×output matrix (pass/fail) |
| Functional (quick) | `matrix run` | the matrix for the current/auto config |
| Throughput (gate) | `store_bench --suite --smoke` | 10 configs ≤10k events, pass/fail, not recorded |
| Throughput (recorded) | `store_bench --suite --db …` | the curated 10, median + low–high band → DB |
| Report | `store_bench --report --db …` | DB → `test-summary/README.md` + `Run_NNN.md` |
| Memory/thread | `matrix verify[-lite]` | valgrind memcheck + helgrind/DRD |

Functional and valgrind gates are pass/fail only — they record nothing. The only persisted
results are the throughput numbers in the tracked `test-summary/bench_results.db` and the
markdown pages rendered from it.
