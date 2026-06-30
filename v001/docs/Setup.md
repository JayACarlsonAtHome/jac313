# Setup, build, and testing

Everything you need to go from a fresh box to a green test run. The [main
README](../README.md) is the overview; this is the how-to.

---

## 1. Toolchain

C++23 throughout — libraries and test framework alike (jText needs `<print>`; modules need a recent
GCC and **Ninja ≥ 1.11**). v001 is the proven C++23 baseline; the C++26 work lives in v002.

**`g++-15` and Clang 20 are the standard toolchains** on both Linux Mint and RHEL-family. On Linux
Mint we install the `g++-15` PPA (it becomes the distro default within a few months); `g++-14` is no
longer used.

### Getting g++-15

- **RHEL / Fedora:** `sudo dnf install gcc-toolset-15` (activated automatically — the
  registry resolves it via `gcc-toolset-15-env` on RHEL 10 or `scl enable` on RHEL 9).
- **Linux Mint / Ubuntu:** GCC 15 is not yet in stock apt. Install it from the Ubuntu
  toolchain PPA until it lands:

  ```bash
  sudo add-apt-repository ppa:ubuntu-toolchain-r/test
  sudo apt update && sudo apt install g++-15
  ```

**Clang 20+** is a first-class peer (`--clang` / `--compiler clang++-20`). Older Clang is
rejected (C++23 `std::expected` / libstdc++ gaps). Clang module builds need
`clang-scan-deps-20` on `PATH` (`clang-20` / `clang-tools-20` on Ubuntu).

### Ninja ≥ 1.11 (modules only)

C++23 module builds (`-DJAC313_BUILD_MODULES=ON`) need **Ninja ≥ 1.11**. Recent Ubuntu/Mint
already ship it; **RHEL 9/10 stock `ninja-build` is 1.10.2** — too old. Install one without
root:

```bash
python3 -m pip install --user ninja      # Ninja ≥ 1.11 in ~/.local/bin
```

Make sure `~/.local/bin` precedes `/usr/bin` on `PATH`. The non-module tiers work with any Ninja.

### Newer CMake (only for `import std;`)

The `import std;` builds (`-DJAC313_*_IMPORT_STD=ON`) use CMake's **native** std-module
support. That feature is experimental and armed by a **CMake-version-specific gate UUID**, so
the requirement is not a floor but an **exact version**: `≥ 3.30` is where the feature first
exists, but only the **pinned 4.3.3** carries the UUID in `CMakeLists.txt` — any other version
(stock Linux Mint's 3.28, RHEL's 3.31.8, even a *newer* CMake) fails to configure with
*"Experimental `import std` support not enabled … `__CMAKE::CXX23` … not provided"*. Install
the pinned 4.3.3 **without root** (Kitware's static build) — `bootstrap.sh` will also *offer*
to do this:

```bash
V=4.3.3   # the EXACT version the import-std gate UUID is pinned to (≥ 3.30 alone is NOT enough)
cd /tmp
curl -fsSLO https://github.com/Kitware/CMake/releases/download/v${V}/cmake-${V}-linux-x86_64.tar.gz
curl -fsSLO https://github.com/Kitware/CMake/releases/download/v${V}/cmake-${V}-SHA-256.txt
grep "cmake-${V}-linux-x86_64.tar.gz" cmake-${V}-SHA-256.txt | sha256sum -c -      # verify
mkdir -p ~/.local/opt/cmake-${V}
tar -xzf cmake-${V}-linux-x86_64.tar.gz -C ~/.local/opt/cmake-${V} --strip-components=1
for t in cmake ctest cpack ccmake; do ln -sf ~/.local/opt/cmake-${V}/bin/$t ~/.local/bin/$t; done
hash -r && cmake --version                                                          # expect ${V}
```

The system CMake is untouched (side-by-side); remove with `rm -rf ~/.local/opt/cmake-${V}
~/.local/bin/{cmake,ctest,cpack,ccmake}`. Everything except the `import std;` build works on
stock CMake 3.28. See [Modules.md](Modules.md) for the version-pinned `import std;` gate UUID.

---

## 2. Bootstrap (bare metal)

On a fresh box, `./bootstrap.sh` is the one hand-run entry point. It senses the OS, finds a
C++23 compiler (+ activation), and checks the full baseline needed to **build *and* run** the
gates in **one pass**: `g++-15` + **Clang**, Ninja ≥ 1.11, CMake, the sqlite3 dev header, and
**valgrind with the memcheck/helgrind/DRD dev headers**. With everything present it **builds
`jac313_test_cli` once and hands off to it** — the data-driven [`jac313::Setup`](../Setup/)
engine does the platform-specific rest.

If a prerequisite is missing, bootstrap writes a small handoff (`Setup/.setup_handoff`) and runs
the **committed, prebuilt `Setup/jac313_setup`** provisioner — a fully-static binary that ships
with the clone, so it runs before any compiler/ninja exists. It resolves the install commands
from [`Setup/recipes.conf`](../Setup/recipes.conf) and runs them with real per-step error
handling: a plan **preview**, a `[y/N]` confirm, `--dry-run`, and a continue-on-failure summary.
If that binary isn't runnable on the host (wrong arch), bootstrap falls back to a reviewable,
**resilient `Setup.sh`** — each recipe runs independently, so one failure can no longer hide the
installs after it. Either path does privileged installs; review, then proceed.

```bash
./bootstrap.sh        # sense → provision (jac313_setup, or Setup.sh) → build the runner → hand off
# answer [y/N] when the provisioner shows its plan; re-run bootstrap when it finishes — idempotent
```

If CMake is present but not the exact pinned version `import std;` needs (RHEL's 3.31.8, Mint's
3.28), bootstrap **offers** the no-sudo `~/.local` install above (`y/N`, advisory only — the
baseline never requires it).

> **Rebuilding the provisioner.** `Setup/jac313_setup` is committed (static, so one binary runs
> across the fleet's distros/glibc versions). When the `Setup` library changes, rebuild +
> re-commit it with **`./Setup/build_setup_exe.sh`**. That needs the static C/C++ runtime archives
> on the *build* host; if they're missing it offers to install them from the `static_runtime`
> recipe (the CodeReady Builder repo + `glibc-static`/`libstdc++-static` on RHEL). Clone hosts need
> none of this.

---

## 3. Build

No sibling checkouts required — Qlite, jText, and Store are all in-tree. Building Store with
Qlite/jText enables SQL and jText persistence. Qlite depends on system SQLite3.

```bash
cmake -G Ninja -S . -B build-gcc15 -DCMAKE_CXX_COMPILER=g++-15 \
  -DJAC313_BUILD_MODULES=ON -DJAC313_BUILD_STORE_TESTS=ON
cmake --build build-gcc15
ctest --test-dir build-gcc15
```

| CMake option | Default | Purpose |
|--------------|---------|---------|
| `JAC313_BUILD_MODULES` | OFF | C++23 module targets (Ninja only) |
| `JAC313_QLITE_IMPORT_STD` | OFF | Qlite module module-native vs `import std;` (g++-15; see [Modules.md](Modules.md)) |
| `JAC313_JTEXT_IMPORT_STD` | OFF | jText module module-native vs `import std;` (g++-15) |
| `JAC313_STORE_IMPORT_STD` | OFF | Store module module-native vs `import std;` (g++-15) |
| `JAC313_BUILD_STORE_TESTS` | OFF | Store unit + matrix tests in ctest |
| `JAC313_BUILD_TEST_CLI` | ON | `jac313_test_cli` local runner |

Build one package: add `-DJAC313_BUILD_JTEXT=OFF -DJAC313_BUILD_STORE=OFF`, or configure from a
package dir (`cmake -S Qlite -B Qlite/build`).

---

## 4. Testing

Testing uses **`jac313_test_cli`** — it discovers ctest entries, runs them in-process, and
drives a ts_store-style matrix (persist × output-mode grid, smoke/full scaling). Configure and
matrix runs default to `Debug`; `--release` runs the same grid under `Release`. Build type is a
recorded identity dimension, so Debug and Release results never conflate.

> **Run the whole battery at once:** [RunAllTests.md](RunAllTests.md) is the copy-paste
> runbook — gcc15 + clang, smoke + full Debug + full Release (modules + no-modules), plus
> the optional valgrind `--verify-lite` / `--verify` gates.

### Tiers (not the same thing)

| Tier | Command | What runs | Wall-clock | Purpose |
|------|---------|-----------|-----------|---------|
| **ctest** | `--ctest` (preset) | registered tests: store units, matrix bins 001–008 (one persist path each), module smoke, RunIdentity + OS-dedup regression | seconds | Compile/link sanity; one path per binary |
| **Smoke matrix** | `matrix run` | **115 scenarios**: each matrix test × persist backend (binary, jText, SQL, inmem, flags, unit) × on/off | ~15 s | Daily gate; full persist grid, minimal scale |
| **Full matrix** | `matrix run --params tests/test_params_full.txt` | same 115 with ts_store stress scaling | **~9–10 min/compiler** | Correctness under load |

The Smoke/Full matrix above is the **functional/correctness** suite (unchanged) — it proves
every persist backend stays correct across scale. **It is not how you benchmark throughput.**

**ctest is not the matrix.** ctest runs each matrix binary once; the matrix re-runs them many
times with different persist backends and CLI scaling. `--ctest --smoke` runs ctest **then** the
smoke matrix (115), not the full matrix.

### Throughput benchmark (separate)

Throughput now has its own runner — it has moved **out** of the functional matrix. From the repo
root:

```bash
./build/tools/jac313_test_cli --bench             # curated suite, numbers to stdout
./build/tools/jac313_test_cli --bench --report    # record → results.db + render the report
```

A curated 7-config suite (non-durable flag sweep + durable jText/SQL/binary). The headline is the
**median + low–high band** over N runs, not a single "peak". See [Benchmarks.md](Benchmarks.md).

```bash
./build/tools/jac313_test_cli compilers                                  # probe toolchains
./build/tools/jac313_test_cli --ctest --smoke                           # ctest + smoke gate (~20 s)
./build/tools/jac313_test_cli matrix run --params tests/test_params_full.txt   # full matrix
./build/tools/jac313_test_cli --bench --report                          # throughput → results.db + render
./build/tools/jac313_test_cli build-times                               # compile-time matrix (gap-filled per host)
./build/tools/jac313_test_cli --verify-lite                             # valgrind memcheck gate (pre-push hook)
./build/tools/jac313_test_cli --run-everything                          # full battery: both compilers + report
./build/tools/jac313_test_cli version-check                            # version() bump gate (git, no build)
./build/tools/jac313_test_cli matrix run --filter sql                   # scenario filter
```

`version-check` is the **version gate**: each package (Qlite/jText/Store) exposes
`jac313::<Pkg>::v001::version()` — its `"major.minor"` version (`"v001.001"` now; major = the
`v001` API line). When a package's shipped code changes, its `version()` literal must be
bumped; `version-check` (git-only, no build) fails if a bump is owed. It runs **first** in the
pre-push hook (cheap check before the heavier gates).

`--verify-lite` is the **valgrind gate**: it self-builds an annotated Debug tree and runs
memcheck + helgrind/DRD over a representative smoke set, exiting non-zero on any error
(`--verify` covers the full sink set). `bootstrap.sh` installs the **pre-push hook**
(`tools/hooks/pre-push` → `.git/hooks/pre-push`), which runs `version-check → build-times →
verify-lite` before every push; bypass once with `git push --no-verify`. Needs valgrind
installed. Details in [docs/Memory-And-Concurrency.md](Memory-And-Concurrency.md).

### Results layout

| Path | Tracked | Contents |
|------|---------|----------|
| `test-results/` | gitignored | Raw scenario logs from local matrix runs |
| `test-summary/results.db` | tracked | Source of truth for metrics; host-scoped markdown pages render from it |

A run is keyed by its **RunIdentity** — `(os, compiler, build_type, disk, size)` — defined once
in [`run_identity.hpp`](../tools/jac313_test_cli/) so DB keys, the results path, and rendered
tables can't drift. `os` is the full sensed version (`rhel-10.2`, so `10.1 ≠ 10.2`). Runs that
differ in any dimension never overwrite each other:

```
test-summary/<os>/<compiler>/<build_type>/<disk>/<size>/RUN.md
```

### Resetting results

`results.db` is tracked but regenerable. Three committed tools (symlinked at the version root by
`bootstrap.sh`) reset it at different granularities — each takes `--dry-run` (preview) **or**
`--yes` (do it); the two are mutually exclusive:

| Tool | Scope | Selector |
|------|-------|----------|
| `./jac313_wipe_all` | the whole DB — every run, machine spec, and pin (schema + `testType` kept) | `--yes` |
| `./jac313_wipe_one` | one run by `run_id`; also drops that run's machine entry **if it was the machine's last run** | `--<N>` (e.g. `--5` = Run_005) |
| `./jac313_wipe_jac` | a whole machine `jac313-###` by `group_id`: all its runs + `host_spec` + `io_best_fit` + pin | `--<G>` (e.g. `--1` = jac313-001) |

Shared dimension tables (`testList`/`compiler`/`parameter`) are always kept; the `safeness`
summary is recomputed from what remains.

### Disk cost

Build trees are a regenerable cache (all `build-*` are gitignored). Cost scales by
`(compiler × build_type)` — smoke and full share a tree. One modules tree ≈ 160–250 MB;
the full gate (smoke+full, Debug+Release, gcc15+clang) ≈ 0.7–0.9 GB. `rm -rf build-*` anytime;
the next run self-builds. Use a build-type-suffixed dir per profile (`-debug`/`-release`) so
Debug and Release never share a CMake cache.
