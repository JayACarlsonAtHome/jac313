//File:    docs/QuickStart.md
//Date:    2026-06-25
//Purpose: Get a newcomer to a green base check with one command, via the runner CLI.

# QuickStart — base checks (fast)

The fastest way to prove a fresh checkout (or a mid-edit tree) is healthy. One entry point, a
handful of composable flags. No `scl enable …` prefix, no deep build path, no raw `cmake`/`ctest`
to remember.

```bash
cd v002 && ./bootstrap.sh        # one-time: sense toolchain, build the runner, drop the symlink
./jac313_test_cli --ctest --smoke
```

That's the base check: the **ctest unit suite (36)** + the **persist×output smoke matrix (116)**,
gcc15/Debug, ~20 s. It prints what it's doing and ends green.

> **How it works:** every preset invocation writes the exact commands to **`./run_latest_config.sh`**
> and runs that script. It's left in place, so you can re-run the identical commands by hand with
> `bash run_latest_config.sh`. The next preset invocation overwrites it.

## The preset gates (compose freely)

| Flag | Gate | Notes |
|------|------|-------|
| `--ctest` | ctest unit suite (Qlite, jText, Store, contracts, …) | ~36 tests |
| `--smoke` | persist×output smoke matrix (`binary/jtext/sql/none/flags` × `on/off`) | ~116 scenarios |
| `--bench` | throughput benchmark suite | numbers to stdout, **not** recorded |
| `--report` | *(with `--bench`)* also record the DB + render the report | → `test-summary/` |
| `--verify-lite` | valgrind **memcheck** over the ctest+smoke surface | the pre-push gate |
| `--verify` | valgrind **memcheck + helgrind + DRD** | adds the thread checks |
| `--group-id` | **read-only** precheck: existing bench groups + this machine's proposed `group_id` | run before `--bench --report` |

They compose — order doesn't matter:

```bash
./jac313_test_cli --ctest                 # just the unit suite
./jac313_test_cli --smoke                 # just the smoke matrix
./jac313_test_cli --ctest --smoke         # the everyday base check
./jac313_test_cli --bench                 # throughput numbers, nothing recorded
./jac313_test_cli --bench --report        # record + render the committed report
./jac313_test_cli --verify-lite           # valgrind memcheck gate
```

Steer the default cell with the usual flags (`--clang`, `--release`, `--modules`) — they ride along
and get baked into the generated script.

## `--bench --report` and the host label

`--bench --report` records throughput into the tracked `test-summary/bench_results.db` and renders
`test-summary/README.md` + `Run_NNN.md`. The machine label is **auto-resolved** — no manual
`host_label.local` step:

- A run's group is its **hardware + OS** identity — `(cpu, cores, ram_gb, os)`. The recorder
  compares that against the existing groups in the DB: same hardware+OS **reuses** its group; new
  hardware/OS gets the next number. It records as `jac313-<group_id>` (`jac313-001`, `jac313-002`, …),
  so no real hostname enters the DB.
- A `host_label.local` / `$JAC313_HOST_LABEL` still overrides if you want to pin a specific label.

Run **`./jac313_test_cli --group-id`** first to see the existing groups and which `group_id` this
machine would record under — so you know up front whether you'd add to an existing group or make a
new one.

## What this does *not* cover

The base check is correctness-only and gcc15-only. For the full battery — **clang**, **`--size full`**,
modules on/off, and the explicit single-cell `runner` — see **[RunAllTests.md](RunAllTests.md)**.
Toolchain / install detail: [Setup.md](Setup.md). Benchmark detail:
[Benchmarks.md](Benchmarks.md) · [HowToReadResults.md](HowToReadResults.md).
