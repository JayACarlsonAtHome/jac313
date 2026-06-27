# Setup, build, and testing

Everything you need to go from a fresh box to a green test run. The [main
README](../README.md) is the overview; this is the how-to.

---

## 1. Toolchain

C++23 is required (jText needs `<print>`; modules need a recent GCC and **Ninja ≥ 1.11**).

**`g++-15` and Clang 20 are the standard toolchains** on both Linux Mint and RHEL-family.
`g++-14` is no longer required — it stays available as a conservative option
(`--force-gcc14`), but it is not part of the readiness gate.

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
C++23 compiler (+ activation), Ninja ≥ 1.11, and CMake, then **builds `jac313_test_cli` once
and hands off to it** — the data-driven [`jac313::Setup`](../Setup/) engine does the
platform-specific rest. Missing a prerequisite? It writes a reviewable `Setup.sh` from
[`Setup/recipes.conf`](../Setup/recipes.conf) and stops so you run the privileged installs by
hand. If CMake is present but not the exact pinned version `import std;` needs (e.g. RHEL's
3.31.8 or Mint's 3.28), it **offers** the no-sudo `~/.local` install above (`y/N`, advisory
only — the baseline never requires it).

```bash
./bootstrap.sh        # sense → build the runner → hand off (or write Setup.sh)
bash Setup.sh         # only if bootstrap reported missing prerequisites (review first)
./bootstrap.sh        # re-run after Setup.sh — idempotent
```

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
> plus the valgrind `--verify-lite` (pre-push) / `--verify` (full) gates.

### Tiers (not the same thing)

| Tier | Command | What runs | Wall-clock | Purpose |
|------|---------|-----------|-----------|---------|
| **ctest** | `--ctest` (preset) / `run` (explicit) | registered tests: store units, matrix bins 001–008 (one persist path each), module smoke, RunIdentity + OS-dedup regression | seconds | Compile/link sanity; one path per binary |
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
./jac313_store_bench --suite            # curated 10-config suite
./jac313_store_bench --suite --dry-run  # print the copy-paste command list
```

A curated 10-config suite (non-durable flag sweep + durable jText/SQL/binary at 1M and 10M). The headline is the
**median + low–high band** over N runs, not a single "peak". See [Benchmarks.md](Benchmarks.md).

### Everything at once

After `./bootstrap.sh`, one command runs the lot — every gate on both compilers, the build-time
matrix, then render the report:

```bash
./jac313_test_cli --run-everything      # then open test-summary/README.md
```

It's the full battery (slow — valgrind verify + bench on two compilers), orchestrated **in code**
(per-step exit codes + a failure summary at the end). For day-to-day work, run individual gates below.

### Gate commands — copy-paste (preset, and the explicit equivalent)

Run from `v002/` — `bootstrap.sh` creates the `./jac313_test_cli` symlink. Each gate has a
one-line **preset** (it writes + runs `./run_latest_config.sh`) and the **explicit** commands it
expands to. Copy either. Default compiler is **gcc15** — swap `--gcc15`→`--clang` and
`build-gcc15`→`build-clang` for the second compiler.

```bash
# ── ctest — unit suite ───────────────────────────────────────────────
./jac313_test_cli --ctest                                      # preset
./jac313_test_cli configure --build-dir build-gcc15 --gcc15    # explicit:
./jac313_test_cli build     --build-dir build-gcc15
./jac313_test_cli run       --build-dir build-gcc15

# ── smoke — persist × output matrix (115 scenarios) ──────────────────
./jac313_test_cli --smoke                                      # preset
./jac313_test_cli configure  --build-dir build-gcc15 --gcc15   # explicit:
./jac313_test_cli build       --build-dir build-gcc15
./jac313_test_cli matrix run  --build-dir build-gcc15

# ── full — same matrix under stress scaling (no preset) ──────────────
./jac313_test_cli configure  --build-dir build-gcc15 --gcc15
./jac313_test_cli build       --build-dir build-gcc15
./jac313_test_cli matrix run  --build-dir build-gcc15 --params tests/test_params_full.txt

# ── verify-lite — memcheck + helgrind + DRD over a REPRESENTATIVE set (the pre-push gate) ──
./jac313_test_cli --verify-lite                                # preset
./jac313_test_cli matrix verify-lite                           # explicit

# ── verify — same three tools, FULL sink coverage ───────────────────
./jac313_test_cli --verify                                     # preset
./jac313_test_cli matrix verify                                # explicit

# ── bench — throughput; records to results.db + renders the report ───
./jac313_test_cli --bench --report                             # preset
./jac313_test_cli configure --release --build-dir build-bench --gcc15   # explicit:
cmake --build build-bench --target jac313_store_bench
build-bench/Store/tests/matrix/jac313_store_bench --suite --db test-summary/results.db
./jac313_test_cli --report

# ── compile-time matrix — per smoke+bench test × (front-end × compiler), host-scoped ──
./jac313_test_cli build-times            # gap-filled: builds only what THIS host (jac313-###) lacks
./jac313_test_cli build-times --dry-run  # show the 6-config plan, build nothing
```

`version-check` is the **version gate**: each package (Qlite/jText/Store) exposes
`jac313::<Pkg>::v002::version()`; when its shipped code changes the literal must be bumped, and
`version-check` (git-only, no build) fails if a bump is owed.

The **pre-push hook** (installed by `bootstrap.sh` at `.git/hooks/pre-push`) runs `version-check`,
then **`build-times`** (the gap-filled compile-time matrix — a no-op once this host is complete, a
full build on a fresh machine), then **`matrix verify-lite`** (valgrind memcheck); bypass once with
`git push --no-verify`. Needs valgrind installed. Details in [docs/Memory-And-Concurrency.md](Memory-And-Concurrency.md).

### Results layout

| Path | Tracked | Contents |
|------|---------|----------|
| `test-results/` | gitignored | Raw scenario logs from local matrix runs |
| `test-summary/results.db` | tracked | The **unified** results DB — ctest / smoke / bench / verify / build, normalized (`testType`, `testList`, `compiler`, `parameter`, `run`, `testRun`). Machines keyed by `(cpu, cores, ram_gb, os)` → `jac313-<group_id>` |
| `test-summary/<area>/` | tracked | Generated by `jac313_test_cli --report`: a top-level `README.md` index + per-area **compiler-comparison** pages — `compiler/`, `ctest/`, `smoke/`, `bench/`, `verify/`, `build/` (clang ↔ gcc side by side) |

Every gate records into the one `results.db`; **`jac313_test_cli --report`** reads it back and
(re)writes the markdown — run it any time to refresh the pages. The legacy `bench_results.db` is
**retired** (bench now writes only `results.db`).

### Disk cost

Build trees are a regenerable cache (all `build-*` are gitignored). Cost scales by
`(compiler × build_type)` — smoke and full share a tree. One modules tree ≈ 160–250 MB;
the full gate (smoke+full, Debug+Release, gcc15+clang) ≈ 0.7–0.9 GB. `rm -rf build-*` anytime;
the next run self-builds. Use a build-type-suffixed dir per profile (`-debug`/`-release`) so
Debug and Release never share a CMake cache.
