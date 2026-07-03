//File:    docs/RunAllTests.md
//Date:    2026-07-03
//Purpose: One-stop runbook — the full jac313 test battery (correctness gate + throughput + valgrind)

# RunAllTests — the full jac313 test battery

```bash
./bootstrap.sh                      # (you may have to run this more than once)

./jac313_test_cli --ctest           # the smallest of all testing
./jac313_test_cli --smoke           # mid level testing
./jac313_test_cli --run-everything  # just like it says, including ctests, smoke tests, benchtests, verify, verify-lite
```

One entry point: **`./jac313_test_cli`** at the `v002/` root (a symlink `bootstrap.sh` drops to the
built runner — the runner self-activates the toolchain; usually no `scl enable …` prefix needed).

There are three main gates, selected by composable preset flags:

1. **Functional correctness** — `--ctest` (unit suite) + `--smoke` (persist×output grid). Pass/fail;
   opens **no database**, writes **nothing** to `test-summary/`.
2. **Throughput** — `--bench` (numbers only) / `--bench --report` (records + renders). See
   [Benchmarks.md](Benchmarks.md).
3. **Memory / thread** — `--verify-lite` (memcheck) / `--verify` (+ helgrind/DRD). `--verify-lite` is
   also the pre-push hook.

`--run-everything` runs the full battery on both compilers (ctest + smoke + bench + verify + build-times + report).

Every preset invocation writes the exact commands to **`./run_latest_config.sh`** and runs that
script; it's left in place so you can re-run it with `bash run_latest_config.sh`.

> **Just want a fast green check?** See **[QuickStart.md](QuickStart.md)** — `./jac313_test_cli --ctest --smoke`.

> The old `matrix run-all` / `matrix render` / `jac313_results.db` were **retired** — throughput and
> the committed report now live in the unified `test-summary/results.db` (`jac313_test_cli --report`).

See also: [QuickStart.md](QuickStart.md) · [Setup.md](Setup.md) · [Benchmarks.md](Benchmarks.md) · [README.md](../README.md)

---

## Functional correctness — matrix runner for any cell

To test a specific configuration, use the explicit runner:

```bash
./jac313_test_cli matrix runner --compiler gcc15 --build-type Debug --modules off --size smoke
```

| Option | Values |
|---|---|
| `--compiler` | `gcc15` \| `clang` (any `compilers.conf` label) |
| `--build-type` | `Debug` \| `Release` |
| `--modules` | `on` (module-native) \| `off` (textual) — default `on` |
| `--size` | `smoke` \| `full` |
| `--build-dir` | optional; default `build-<compiler>-<buildtype>-<modules>` |

### Every functional combo, spelled out

The full functional sweep is **{gcc15, clang} × {Debug, Release} × {modules on, off} × {smoke, full}** — 16 cells (clang modules may require clang-scan-deps-20).

```bash
# --- gcc15 ---
./jac313_test_cli matrix runner --compiler gcc15 --build-type Debug   --modules on  --size smoke
./jac313_test_cli matrix runner --compiler gcc15 --build-type Debug   --modules on  --size full
./jac313_test_cli matrix runner --compiler gcc15 --build-type Debug   --modules off --size smoke
./jac313_test_cli matrix runner --compiler gcc15 --build-type Debug   --modules off --size full
./jac313_test_cli matrix runner --compiler gcc15 --build-type Release --modules on  --size smoke
./jac313_test_cli matrix runner --compiler gcc15 --build-type Release --modules on  --size full
./jac313_test_cli matrix runner --compiler gcc15 --build-type Release --modules off --size smoke
./jac313_test_cli matrix runner --compiler gcc15 --build-type Release --modules off --size full

# --- clang (textual only; clang module builds need clang-scan-deps-20 on PATH) ---
./jac313_test_cli matrix runner --compiler clang --build-type Debug   --modules off --size smoke
./jac313_test_cli matrix runner --compiler clang --build-type Debug   --modules off --size full
./jac313_test_cli matrix runner --compiler clang --build-type Release --modules off --size smoke
./jac313_test_cli matrix runner --compiler clang --build-type Release --modules off --size full
```

The heavy `--size full` cells are I/O-intensive — start day-to-day with `--ctest --smoke`.

---

## Throughput

```bash
./jac313_test_cli --group-id         # PRECHECK: existing groups + this machine's proposed group_id
./jac313_test_cli --bench            # build Release, run curated suite — numbers to stdout (not recorded)
./jac313_test_cli --bench --report   # record → test-summary/results.db + render the comparison pages
```

Run `--group-id` first to see whether recording reuses an existing group or creates a new one.

`--bench --report` records throughput (median + band) into the tracked DB and renders the report pages.

You can steer with `--clang` for a second compiler in the same report (clang ↔ gcc head-to-head).

See [Benchmarks.md](Benchmarks.md) for more.

---

## Memory / thread gate — verify

Valgrind **memcheck + helgrind + DRD** (required by bootstrap):

```bash
./jac313_test_cli --verify-lite     # memcheck over ctest + smoke surface  (~31 checks, ~3 min)
./jac313_test_cli --verify          # + helgrind/DRD + full sink sweep      (~60 checks, ~6 min)
```

Verdict-only (no DB write) — non-zero exit on any error. `--verify-lite` is the pre-push gate
(installed by `bootstrap` into the shared parent `.git/hooks`); bypass once with `git push --no-verify`.

---

## Render the report

```bash
./jac313_test_cli --report
```

`--run-everything` renders at the end; `--report` is for a manual refresh.

Pages land under `test-summary/` (compiler/, ctest/, smoke/, bench/, verify/, etc.), each host-scoped.

---

## What the battery covers

| Gate | Command | Scope |
|------|---------|-------|
| Functional (small) | `--ctest` | ctest unit suite |
| Functional (mid) | `--smoke` | persist×output smoke matrix |
| Full functional | `matrix runner …` or `--run-everything` | all compilers × Debug/Release × modules × smoke/full |
| Throughput | `--bench` / `--bench --report` | curated suite → stdout or DB + report |
| Memory/thread | `--verify-lite` / `--verify` | valgrind memcheck + helgrind/DRD |
| Full battery | `--run-everything` | both compilers + ctest + smoke + bench + verify + build-times + report |

Functional and valgrind gates are pass/fail only (no DB write by default). Throughput numbers and build-time data go into the tracked `test-summary/results.db`.

See [QuickStart.md](QuickStart.md) for the everyday base check and [Setup.md](Setup.md) for toolchain, host pinning, and more.