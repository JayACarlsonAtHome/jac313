//File:    docs/RunAllTests.md
//Date:    2026-06-25
//Purpose: One-stop runbook — the full jac313 test battery (correctness gate + throughput + valgrind)

# RunAllTests — the full jac313 test battery

One entry point: **`./jac313_test_cli`** at the `v002/` root (a symlink `bootstrap.sh` drops to the
built runner — no `scl enable …` prefix needed; the runner self-activates the toolchain). There are
**three gates**, selected by composable preset flags:

1. **Functional correctness** — `--ctest` (unit suite) + `--smoke` (persist×output grid). Pass/fail;
   opens **no database**, writes **nothing** to `test-summary/`.
2. **Throughput** — `--bench` (numbers only) / `--bench --report` (records + renders). See
   [Benchmarks.md](Benchmarks.md).
3. **Memory / thread** — `--verify-lite` (memcheck) / `--verify` (+ helgrind/DRD). `--verify-lite` is
   also the pre-push hook.

Every preset invocation writes the exact commands to **`./run_latest_config.sh`** and runs that
script; it's left in place so you can re-run it with `bash run_latest_config.sh`.

> **Just want a fast green check?** **[QuickStart.md](QuickStart.md)** — `./jac313_test_cli --ctest
> --smoke`, gcc15, ~20 s. This page is the *full* battery (clang, `--size full`, modules), driven by
> the explicit **`runner`** subcommand for the cells presets don't cover.

> The old `matrix run-all` / `matrix render` / `jac313_results.db` were **retired** — throughput and
> the committed report now live in the unified `test-summary/results.db` (`jac313_test_cli --report`).

See also: [QuickStart.md](QuickStart.md) · [Setup.md](Setup.md) · [Benchmarks.md](Benchmarks.md) · [README.md](../README.md)

---

## 0. Prerequisites (one-time)

Run everything **from `v002/`**.

```bash
cd v002 && ./bootstrap.sh        # senses toolchain, REQUIRES valgrind, builds the runner + drops the symlink
```

After bootstrap, the everyday surface is just `./jac313_test_cli <flags>`.

**First-time machine pin** (clean `results.db`): `./jac313_test_cli host` auto-assigns `jac313-###`
when unambiguous. Same hardware+OS siblings need `--assign-new-###`; reclaiming an existing slot
uses `--claim jac313-###`. Identity is cpu+cores+ram+disk+os+`instance_hash` (SHA256 of `uname`
nodename — never committed raw). `host_spec` is fleet-shared; `current_host` is pinned per tree
(`v001`/`v002`) so neither overwrites the other on the same machine. See [Setup.md](Setup.md).

---

## 1. Functional correctness

**Everyday (gcc15 / Debug / textual):**

```bash
./jac313_test_cli --ctest --smoke        # ctest unit suite (36) + persist×output smoke grid (116)
./jac313_test_cli --ctest                # just the unit suite
./jac313_test_cli --smoke                # just the smoke grid
```

**Explicit single cell — the `runner` subcommand** (for a specific compiler / build-type / modules /
full size; the preset flags above are the smoke-oriented gcc15 path):

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

The full functional sweep is **{gcc15, clang} × {Debug, Release} × {modules on, off} × {smoke,
full}** — 16 cells. There's no `run-all` anymore, so run the ones you need (each auto-builds its own
`build-<compiler>-<buildtype>-<modules>` tree; `modules on` vs `off` never collide):

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

## 2. Throughput

```bash
./jac313_test_cli --group-id         # PRECHECK: existing groups + this machine's proposed group_id (read-only)
./jac313_test_cli --bench            # build Release, run the curated suite — numbers to stdout, NOT recorded
./jac313_test_cli --bench --report   # record → test-summary/results.db + render the comparison pages
```

Run `--group-id` first to see whether recording reuses an existing group or creates a new one (a
machine recorded before the registry existed resolves to a new id unless you pin `host_label.local`).

`--bench` configures a Release `build-bench`, builds the `jac313_store_bench` target, and runs the
curated 10-config suite. `--report` additionally records to the tracked DB and renders the report.

**Host label is auto-resolved** — no manual `host_label.local`. A run's group is its **hardware + OS**
identity `(cpu, cores, ram_gb, os)`, compared against the recorded groups: same hardware+OS reuses
its `jac313-<group_id>`, new hardware/OS gets the next number — so no real hostname ever enters the
DB. A `host_label.local` / `$JAC313_HOST_LABEL` still overrides. Preview with `--group-id`.

**A second compiler:** add `--clang`, re-run `--bench --report` into the same DB — the report's
bench page then shows **clang ↔ gcc head-to-head** (median ops/sec · band · size per compiler). No
automatic gcc+clang sweep (manual by decision — the suite is short).

> Under the hood this drives `store_bench` (`--suite` / `--suite --db …` / `--report --db …`); see
> [Benchmarks.md](Benchmarks.md) to run a single config or use `--dry-run` / `--anonymize` directly.

---

## 3. Memory / thread gate

Valgrind **memcheck + helgrind + DRD** (all ship in the one valgrind package, which `bootstrap`
*requires*):

```bash
./jac313_test_cli --verify-lite     # memcheck over ctest + smoke surface  (~31 checks, ~3 min)
./jac313_test_cli --verify          # + helgrind/DRD + full sink sweep      (~60 checks, ~6 min)
```

Verdict-only (no DB write) — non-zero exit on any error. `--verify-lite` is the pre-push gate
(installed by `bootstrap` into the shared parent `.git/hooks`); bypass once with
`git push --no-verify`.

---

## What the battery covers

| Gate | Command | Scope |
|------|---------|-------|
| Functional | `--ctest` / `--smoke` (or `matrix runner …`) | ctest suite + persist×output matrix (pass/fail) |
| Throughput (gate) | `--bench` | curated suite, numbers to stdout, not recorded |
| Throughput (recorded) | `--bench --report` | median + low–high band → DB + rendered report |
| Memory/thread | `--verify-lite` / `--verify` | valgrind memcheck + helgrind/DRD |

Functional and valgrind gates are pass/fail only — they record nothing. The only persisted results
are the throughput numbers in the tracked `test-summary/results.db` and the markdown pages
rendered from it.
