//File:    docs/RunAllTests.md
//Date:    2026-07-03
//Purpose: One-stop runbook — the full jac313 test battery

# RunAllTests — the full jac313 test battery

```bash
./bootstrap.sh                    # (you may have to run this more than once)

./jac313_test_cli --ctest         # the smallest of all testing
./jac313_test_cli --smoke         # mid level testing
./jac313_test_cli --run-everything # just like it says, including ctests, smoke tests, benchtests, verify, verify-lite
```

**One command runs the battery:** `--run-everything` configures, builds, and gates **both**
toolchains (gcc15 + clang) — ctest, smoke matrix, throughput bench, valgrind verify, and the
`build-times` compile-time matrix — then renders the host-scoped report.

> **gcc14 is excluded by design** — gcc15 + clang are the gate on every platform.

See also: [Setup.md](Setup.md) · [README.md](../README.md)

---

## Run ONE configuration — `matrix runner`

To test a single combo, use the explicit runner:

```bash
./jac313_test_cli matrix runner --compiler gcc15 --build-type Debug --modules off --size smoke
```

That builds the appropriate tree, runs the smoke matrix, and records the result.

| Option | Values |
|---|---|
| `--compiler` | `gcc15` \| `clang` |
| `--build-type` | `Debug` \| `Release` |
| `--modules` | `on` (module-native) \| `off` (textual) |
| `--size` | `smoke` \| `full` |
| `--build-dir` | optional override |

---

## Every combo, spelled out (copy-paste any one line)

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

# --- clang ---
./jac313_test_cli matrix runner --compiler clang --build-type Debug   --modules on  --size smoke
./jac313_test_cli matrix runner --compiler clang --build-type Debug   --modules on  --size full
./jac313_test_cli matrix runner --compiler clang --build-type Debug   --modules off --size smoke
./jac313_test_cli matrix runner --compiler clang --build-type Debug   --modules off --size full
./jac313_test_cli matrix runner --compiler clang --build-type Release --modules on  --size smoke
./jac313_test_cli matrix runner --compiler clang --build-type Release --modules on  --size full
./jac313_test_cli matrix runner --compiler clang --build-type Release --modules off --size smoke
./jac313_test_cli matrix runner --compiler clang --build-type Release --modules off --size full
```

`--modules on` is the module-native build, `off` is textual.

---

## Memory / thread gate — verify

Valgrind **memcheck + helgrind + DRD** (required by bootstrap):

```bash
./jac313_test_cli matrix verify-lite     # ctest representative set (~31 checks, ~3 min)
./jac313_test_cli matrix verify          # + full smoke-matrix sink sweep (~60 checks, ~6 min)
```

These are verdict-only (non-zero exit on any error). `verify-lite` is the pre-push gate.

---

## Render the report

```bash
./jac313_test_cli --report
```

`--run-everything` renders at the end; `--report` is for a manual refresh.

Pages land under `test-summary/{ctest,smoke,bench,verify,verify-lite,compiler-build-times}/README.md`,
each split into `## jac313-###` sections per machine.

---

## What the battery covers

| Tier | Command | Scope |
|------|---------|-------|
| Full battery | `--run-everything` | both compilers: ctest + smoke + bench + verify + build-times + report |
| One cell | `matrix runner …` | a single explicit config |
| Compile times | `build-times` | the compile-time matrix, gap-filled per host |
| Render | `--report` | results.db → `test-summary/` host-scoped pages |
| Memory/thread | `matrix verify[-lite]` | valgrind memcheck + helgrind/DRD |

Results are stored in the tracked `test-summary/results.db`. Each run is keyed by RunIdentity so Debug/Release, modules/textual, machines, etc. never collide.

See [Setup.md](Setup.md) for more on machine identity (`jac313-###`), results layout, and the pre-push hook.
