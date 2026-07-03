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

## 2. Bootstrap

It is assumed you already have `git` installed — otherwise you wouldn't have been able to clone the repository.

From inside the `v001/` directory, the first command you run is:

```bash
./bootstrap.sh
```

(The whole purpose of this script is to load **all the dependencies** for you.)

### Purpose

The sole job of `./bootstrap.sh` is to **load all the dependencies** you need to build and work with the project.

It detects your platform and makes sure the complete baseline is present so everything "just works" afterward.

### Sudo and running it twice

- You may be prompted for your **sudo password once** during the process (only when system packages need to be installed).
- You will typically need to **run `./bootstrap.sh` twice**.

**Why run it twice?**

The first execution checks what is missing and performs the actual installation of packages (compilers, headers, build tools). Once those packages are installed, the environment has changed — new commands exist on your `PATH` (or activation like `scl enable gcc-toolset-15 --` becomes relevant).

The script deliberately exits after provisioning so the shell environment can be refreshed. On the second run it sees that everything is now available and proceeds to:

- Build the `jac313_test_cli` helper (and a few wipe tools).
- Create the convenient top-level symlinks (`./jac313_test_cli`, `./jac313_wipe_all`, etc.).
- Install the git pre-push hook.
- Pin your machine identity so results are correctly attributed.

After the second successful run, your environment is ready.

### What gets loaded for you

`bootstrap.sh` ensures the following baseline is installed and ready:

- C++23 compilers: `g++-15` (or the equivalent via gcc-toolset on RHEL) + Clang 20+
- CMake
- Ninja ≥ 1.11
- SQLite3 development headers
- Valgrind with the full headers required for memory and concurrency checking (memcheck + helgrind + DRD)

It may optionally offer to install a pinned version of CMake into `~/.local` for experimental `import std;` builds (this is **not** required for normal development or testing).

Once bootstrap finishes, the main way you interact with the project is through the symlinked `./jac313_test_cli` command.

For running tests, see [RunAllTests.md](RunAllTests.md).

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

See [RunAllTests.md](RunAllTests.md) for how to run the full test battery (`--ctest`, `--smoke`, `--run-everything`, verify gates, etc.). 

`bootstrap.sh` sets up the convenient `./jac313_test_cli` symlink and the pre-push hook for you.

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

### Machine identity (`jac313-###`)

Each VM is keyed in `host_spec` by **cpu + cores + ram_gb + disk + os + instance_hash**, where
`instance_hash = SHA256(uname nodename)` (computed locally; raw hostname never committed).
`group_id` is the friendly label `jac313-###`. `host_spec` is shared across v001/v002 (one slot per
machine); `current_host` is pinned per tree (`v001` / `v002`) so neither overwrites the other's pin.
After a wipe or on a new VM:

1. `git pull` (code + `results.db`)
2. `./jac313_test_cli host` — auto-pins when unambiguous
3. If the fleet table appears: `--claim jac313-###` (same OS+hardware slot) or `--assign-new-###`
4. Commit + push `results.db` so other machines see the fleet map

Read-only check: `./jac313_test_cli --group-id`.

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
