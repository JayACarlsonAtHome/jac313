//File:    docs/RunAllTests.md
//Date:    2026-06-19
//Purpose: One-stop runbook — the full jac313 test battery via `--run-everything`

# RunAllTests — the full jac313 test battery

**One command runs the battery:** `--run-everything` configures, builds, and gates **both**
toolchains (gcc15 + clang) — ctest, smoke matrix, throughput bench, valgrind verify, and the
`build-times` compile-time matrix — then renders the host-scoped report. The `build-times` step
is **gap-filled per host** (builds only the compile-time cells this machine lacks); the rest
re-runs each time.

> **gcc14 is excluded by design** — gcc15 + clang are the gate on every platform.

See also: [Setup.md](Setup.md) · [README.md](../README.md) · [FORWARDING.md](../FORWARDING.md)

---

## 0. Prerequisites (one-time)

```bash
./bootstrap.sh        # senses toolchain, REQUIRES valgrind (+ dev headers), builds the runner
```

Run everything **from the repo root**. The runner lives at
`./build-bootstrap/tools/jac313_test_cli`. Set these once per shell (RHEL needs the toolset
activated; Mint has `g++-15`/`clang` on `PATH`):

```bash
ACT="scl enable gcc-toolset-15 --"   # Linux Mint: ACT=""
CLI="./build-bootstrap/tools/jac313_test_cli"
```

`DISK_TYPE=auto` is the default — the disk tier (ssd/x7k/10k) is detected from the disk the
repo lives on, so there's nothing to set per machine.

---

## 1. Run everything — `--run-everything`

```bash
$ACT $CLI --run-everything    # the full battery on both compilers, then render the report
$ACT $CLI --ctest --smoke     # quick green first: ctest + smoke, ~20 s
```

`--run-everything` runs configure → build → ctest → smoke → bench for **both** gcc15 and clang,
then valgrind verify, the `build-times` compile-time matrix, and `--report`. It's a
"leave-it-running" job; do `--ctest --smoke` first for a fast green. (There's no `run-all`
gap-fill composer anymore — to fill the full **16-combo** Debug/Release × modules × smoke/full
sweep, run the `matrix runner` grid in §3; `build-times` is the only step that gap-fills.)

---

## 2. Run ONE configuration — `matrix runner`

To test a single thing, use the `runner` primitive (the explicit-config unit the §3 grid uses).
Every dimension is explicit and the build dir is auto-named:

```bash
$ACT $CLI matrix runner --compiler gcc15 --build-type Debug --modules off --size smoke
```

That builds `build-gcc15-debug-textual`, runs the smoke matrix, and records
`gcc15 / Debug / textual / Smoke`.

| Option | Values |
|---|---|
| `--compiler` | `gcc15` \| `clang` |
| `--build-type` | `Debug` \| `Release` |
| `--modules` | `on` (module-native) \| `off` (textual) |
| `--size` | `smoke` \| `full` |
| `--build-dir` | optional; default `build-<compiler>-<buildtype>-<modules>` |

---

## 3. Every combo, spelled out (copy-paste any one line)

These are the **16 `matrix runner` invocations** for the full sweep — grab a line to test exactly one cell:

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

# --- clang ---
$CLI matrix runner --compiler clang --build-type Debug   --modules on  --size smoke
$CLI matrix runner --compiler clang --build-type Debug   --modules on  --size full
$CLI matrix runner --compiler clang --build-type Debug   --modules off --size smoke
$CLI matrix runner --compiler clang --build-type Debug   --modules off --size full
$CLI matrix runner --compiler clang --build-type Release --modules on  --size smoke
$CLI matrix runner --compiler clang --build-type Release --modules on  --size full
$CLI matrix runner --compiler clang --build-type Release --modules off --size smoke
$CLI matrix runner --compiler clang --build-type Release --modules off --size full
```

`--modules on` is the module-native build, `off` is textual; each line auto-builds its own
`build-<compiler>-<buildtype>-<modules>` tree and records a **distinct** row (the `modules`
RunIdentity dimension keeps module vs textual from colliding).

---

## 4. Render the report

```bash
$CLI --report     # regenerate the host-scoped pages from results.db
```
`--run-everything` renders at the end; `--report` is for a manual refresh. Pages land under
`test-summary/{ctest,smoke,bench,verify,verify-lite,compiler-build-times}/README.md`, each split
into `## jac313-###` sections per machine.

---

## 5. Memory / thread gate — `verify`

Valgrind **memcheck + helgrind + DRD** (all ship in the one valgrind package, which
`bootstrap` now *requires*):

```bash
$ACT $CLI matrix verify-lite     # ctest representative set        (~31 checks, ~3 min)
$ACT $CLI matrix verify          # + full smoke-matrix sink sweep  (~60 checks, ~6 min)
```

Verdict-only (no DB write) — non-zero exit on any error. `verify-lite` is the pre-push gate.

---

## What the battery covers

| Tier | Command | Scope |
|------|---------|-------|
| Full battery | `--run-everything` | both compilers: ctest + smoke + bench + verify + build-times + report |
| One cell | `matrix runner …` | a single explicit config |
| Compile times | `build-times` | the compile-time matrix, gap-filled per host |
| Render | `--report` | results.db → `test-summary/` host-scoped pages |
| Memory/thread | `matrix verify[-lite]` | valgrind memcheck + helgrind/DRD |

Results render to `test-summary/{type}/README.md`, each host-scoped into `## jac313-###`
sections, with metrics in the tracked `test-summary/results.db`.
