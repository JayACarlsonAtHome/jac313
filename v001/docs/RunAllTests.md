//File:    docs/RunAllTests.md
//Date:    2026-06-19
//Purpose: One-stop runbook — the full jac313 test battery via `matrix run-all`

# RunAllTests — the full jac313 test battery

**One command runs everything:** `matrix run-all` builds + runs every *missing* combo of
**{compiler} × {build_type} × {modules} × {size}** for this host, then renders the results.
It's **idempotent and resumable** — re-run it and it only does what's still missing.

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

## 1. Run everything — `matrix run-all`

```bash
$ACT $CLI matrix run-all              # every MISSING combo for this host
$ACT $CLI matrix run-all --dry-run    # list what WOULD run, then stop
$ACT $CLI matrix run-all --no-full    # smoke combos only (skip the heavy xFull)
$ACT $CLI matrix run-all --force      # re-run everything, ignore what's already done
```

`run-all` diffs the desired matrix against the results DB
(`v_desired_matrix LEFT JOIN runs WHERE r.id IS NULL`) and runs the gap. Kill it mid-sweep
and re-run — it picks up the remainder. The full sweep (16 combos incl. xFull builds) is a
~2 hr "leave-it-running" job; start with `--dry-run` to see the plan.

---

## 2. Run ONE configuration — `matrix runner`

To test a single thing, use the `runner` primitive (the same unit `run-all` composes).
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

These are the **16 invocations `run-all` composes** — grab a line to test exactly one cell:

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

## 4. Render the summary

```bash
$CLI matrix render --all     # regenerate every RUN.md + the hub README from the DB
```
`run-all` renders as it goes; this is for a manual refresh.

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
| Functional | `matrix run-all` | every combo, ctest + smoke + full, recorded to the DB |
| One cell | `matrix runner …` | a single explicit config |
| Render | `matrix render --all` | DB → `test-summary/` pages |
| Memory/thread | `matrix verify[-lite]` | valgrind memcheck + helgrind/DRD |

Results land under `test-summary/<os>/<compiler>/<build_type>/<disk>/<size>/<modules>/`,
with metrics in the tracked `test-summary/jac313_results.db`.
