# Results — what a build + test run actually produces

Current live results (safeness, ctest, smoke, bench throughput, verify, build times) are
rendered into per-version `test-summary/` directories from the shared `results.db`. See:

- The version's `test-summary/README.md` for safeness summary + links to detailed reports
- `test-summary/bench/README.md` for throughput (per-machine median + band)

Current reports use **median + low–high band**. Older "Peak ops/sec (fastest-of-N)" figures
from functional-matrix logs are historical.

Live per-run detail lives in the test-summary reports under this version. Hardware-specific
numbers vary; see the structured per-machine tables rather than any single headline.

---

Live results are in `test-summary/`. The detailed per-platform tables below are historical
and use older "Peak" reporting. See the current structured `test-summary/bench/` and other
reports for up-to-date median+band data on current machines.

For reference, v001 has been run on various hardware (Linux Mint, RHEL, etc.). See the live
reports linked from `test-summary/README.md` (per version) and the HowToReadResults guidance.

### Toolchains (historical note)

Sensed automatically by `./bootstrap.sh`. The same source builds under g++-15, older gcc, and
clang peers. Throughput reference was typically g++-15 per platform; clang also ran full
batteries. Current data is per-machine in the reports.
**g++-15 vs clang (Linux Mint)** table below.

### Tests & throughput

| Run | Result | Wall time | Peak hot-path | Persist footprint |
|-----|--------|-----------|---------------|-------------------|
| **ctest** (units, matrix bins, RunIdentity regressions) | 30/30 ✅ | seconds | — | — |
| **Smoke matrix** — 116 scenarios, minimal scale (Debug) | 116/116 ✅ | ~16 s | 0.86M ops/s | 9 MB |
| **Full matrix** — 116 at stress scale (Debug) | 116/116 ✅ | ~9 m 52 s | 2.9M events/s | ~610 MB |
| **Full matrix** — 116 at stress scale (**Release**) | 116/116 ✅ | ~9 m 26 s | **8.8M events/s** | ~610 MB |

The matrix is `116 = matrix tests (001–008 TS/XS + flags + metric_view) × persist backend
(binary / jtext / html / json / sql / in-memory / flag-routing) × output on/off`.

**Headline:** the lock-free in-memory hot path hits **~8.8M events/sec at Release** — about
**3× the Debug figure**. The test gates run on **Debug**, so the day-to-day numbers are the
conservative ones. The **~610 MB** persist footprint is the "true size" to budget for a full
run's output (it lands in `test-results/`, gitignored, regenerable) and is **identical across
Debug/Release** — same data written. The run is **CPU-bound, not disk-bound** (~610 MB over
~9 min ≈ 1 MB/s), so disk type barely matters. (Peak is the lock-free path; with persistence
attached a per-event mutex serializes the producer — see
[memory & concurrency](Verify.md).)

### g++-15 vs clang (Linux Mint)

Same box, same source — the full battery run under both compilers. Every tier is **116/116**
on both, ctest **30/30** on both. On this 8-core 2017 laptop the two are **level**: peak ops/sec
is a single best-sample metric that varies run-to-run, and here **clang actually edges ahead at
Release** (9.18M vs 8.80M) while g++-15 leads full Debug — smoke and ctest are a wash. clang is a
real peer, fully green, tracked alongside g++-15 in the hub. `import std;` is **gcc-only**, so
clang has no import-std row.

| Tier | g++-15 | clang 20.1.2 |
|---|---|---|
| ctest | 30/30 ✅ | 30/30 ✅ |
| Smoke (Debug) | 116/116 ✅ (~16 s, 0.86M ops/s) | 116/116 ✅ (~16 s, 0.94M ops/s) |
| Full Debug | 116/116 ✅ (~9 m 52 s, peak 2.92M) | 116/116 ✅ (~9 m 52 s, peak 2.56M) |
| Full Release | 116/116 ✅ (~9 m 26 s, peak **8.80M**) | 116/116 ✅ (~9 m 30 s, peak **9.18M**) |
| Persist footprint | 610 MB (full) | 610 MB (full) |
| `import std;` | break-even (gcc-only; see [Modules.md](Modules.md)) | n/a (gcc-only) |

Both compilers' runs live in the [hub](../test-summary/) under distinct RunIdentity rows
(`…/gcc15/…` and `…/clang/…`), so they sit side by side without conflict.

---

## RHEL 9.8

> **Two CMake runs, recorded side-by-side.** The same battery was run twice on this box: once
> on the **baseline CMake 3.31.8** (RHEL 9.8 stock / appstream), once on the **pinned 4.3.3**
> (no-sudo `~/.local` install). Both are fully green. The one real difference is the optional
> `import std;` pilot, which CMake gates behind a *version-specific UUID*: the UUID pinned in
> `CMakeLists.txt` is 4.3.3's. On **3.31.8** the import-std configure fails (*"Experimental
> `import std` support not enabled … `__CMAKE::CXX23` target … not provided by the
> toolchain"*) and the runner records **no import-std timing**; on **4.3.3** it configures,
> builds, and times cleanly. Functional results and throughput are identical (within noise)
> across the two — see [CMake 3.31.8 vs 4.3.3](#cmake-3318-vs-433) below. Both runs persist in
> the results DB under distinct Group IDs (3.31.8 = groups 4/5/6, 4.3.3 = groups 7/8/9); the
> rendered hub tables show the latest per identity, so the side-by-side lives here.

### Hardware

| | |
|---|---|
| CPU | Intel Core Ultra 7 265 — **20 cores** (1 thread/core) |
| RAM | 62 GB |
| Storage | WD PC SN740 (NVMe SSD); a WD Green HDD is also present. Repo + builds on the NVMe (`ssd`) |
| OS | Red Hat Enterprise Linux 9.8 (Plow), kernel 5.14.0-687.12.1.el9_8 |

### Versions under test

Recorded at run time from each package's `version()`, not hand-typed:

| Package | Version |
|---------|---------|
| `jac313::Qlite::v001` | **v001.001** |
| `jac313::jText::v001` | **v001.001** |
| `jac313::Store::v001` | **v001.001** |

### Toolchains

Sensed automatically by `./bootstrap.sh` (activation-aware — gcc-toolset via `scl`):
**g++-15** (15.2.1, standard, via `gcc-toolset-15`) · **g++-14** (14.2.1, conservative
fallback, via `gcc-toolset-14`) · **clang++** (21.1.8, first-class peer). The throughput rows
below are the **g++-15** reference, matching the Mint convention (one reference compiler per
platform) — but **clang also ran the complete battery here** (ctest, smoke, full Debug/Release),
all **116/116**, tracked under `…/clang/…` in the [hub](../test-summary/); see the
**g++-15 vs clang (RHEL 9.8)** table below.

### Tests & throughput

Numbers below are the **CMake 4.3.3** run (the complete toolchain, import-std included). The
3.31.8 run matches them within noise — see the comparison that follows.

| Run | Result | Wall time | Peak hot-path | Persist footprint |
|-----|--------|-----------|---------------|-------------------|
| **ctest** (units, matrix bins, RunIdentity regressions) | 33/33 ✅ | seconds | — | — |
| **Smoke matrix** — 116 scenarios, minimal scale (Debug) | 116/116 ✅ | ~16 s | 3.03M ops/s | 7 MB |
| **Full matrix** — 116 at stress scale (Debug) | 116/116 ✅ | ~9 m 59 s | 16.4M ops/s | 316 MB |
| **Full matrix** — 116 at stress scale (**Release**) | 116/116 ✅ | ~9 m 51 s | **24.3M ops/s** | 316 MB |

The matrix shape is identical to Mint's (`116 = matrix tests × persist backend × output on/off`).

**Headline:** the hot path peaks at **~24.3M ops/sec at Release** (scenario `008_TS`,
flags/off) — roughly **1.5× the Debug peak** (16.4M). The test gates run on **Debug**, so the
day-to-day figure is the conservative one. This box (a 2024 Core Ultra 7) is roughly **3–5×
faster** than the Mint reference laptop across every tier, which is why the absolute numbers sit
well above the Mint rows — RunIdentity keeps each `(OS × compiler × build-type)` row distinct in
the [hub](../test-summary/) so they never conflate. The **~316 MB** per-full-run persist
footprint (Debug and Release write the same data) lands in `test-results/` (gitignored,
regenerable). The run is **CPU-bound, not disk-bound** (~316 MB over ~10 min ≈ 0.5 MB/s).

### CMake 3.31.8 vs 4.3.3

Same box, same g++-15, same source — the only variable is the CMake version. Functional and
throughput results are **identical within run-to-run noise**; the meaningful difference is
whether the optional `import std;` pilot can be configured at all.

| | CMake 3.31.8 (baseline) | CMake 4.3.3 (pinned) |
|---|---|---|
| ctest | 33/33 ✅ | 33/33 ✅ |
| Smoke (Debug) | 116/116 ✅ (~16 s) | 116/116 ✅ (~16 s) |
| Full Debug | 116/116 ✅ (~9 m 58 s, peak 14.7M) | 116/116 ✅ (~9 m 59 s, peak 16.4M) |
| Full Release | 116/116 ✅ (~9 m 51 s, peak 23.8M) | 116/116 ✅ (~9 m 51 s, peak 24.3M) |
| `import std;` pilot | ❌ **configure fails** (UUID mismatch) — no timing recorded | ✅ **configures + builds**; all three modules `import std ENABLED` |
| `import std;` vs textual modules | — (unavailable) | **+202 ms (−3%)** — effectively break-even |

The throughput deltas (e.g. 14.7M → 16.4M Debug peak) are within normal run-to-run variance,
not a CMake effect — the compiler and emitted code are unchanged. The **import std** row is the
real story: on 3.31.8 the pinned experimental UUID doesn't match, so the std-module build never
configures; on 4.3.3 it builds and measures **break-even** against textual modules — matching
the [Mint result](Modules.md). This is exactly why bootstrap *offers* the no-sudo 4.3.3 install
when CMake is older than the pilot needs.

### g++-15 vs clang (RHEL 9.8)

Same box, same source — the full battery run under both compilers (g++-15 on CMake 4.3.3, clang
on the system CMake). Every tier is **116/116** on both. On this 20-core box g++-15 holds a
**modest edge at the full tiers** (the peak ops/sec is a single best-sample metric and varies
run-to-run, but g++-15 leads here by ~15–20% at full scale); smoke and ctest are level. clang
remains a real peer — fully green, tracked alongside g++-15 in the hub. `import std;` is
**gcc-only**, so clang has no import-std row.

| Tier | g++-15 (CMake 4.3.3) | clang 21.1.8 |
|---|---|---|
| ctest | 33/33 ✅ | 33/33 ✅ |
| Smoke (Debug) | 116/116 ✅ (~16 s, 3.03M ops/s) | 116/116 ✅ (~16 s, 3.02M ops/s) |
| Full Debug | 116/116 ✅ (~9 m 59 s, peak 16.4M) | 116/116 ✅ (~9 m 58 s, peak 13.6M) |
| Full Release | 116/116 ✅ (~9 m 51 s, peak **24.3M**) | 116/116 ✅ (~9 m 51 s, peak **20.9M**) |
| Persist footprint | 316 MB (full) | 316 MB (full) |
| `import std;` | break-even (+202 ms / −3%) | n/a (gcc-only) |

Both compilers' runs live in the [hub](../test-summary/) under distinct RunIdentity rows
(`…/gcc15/…` and `…/clang/…`), so they sit side by side without conflict.

## RHEL 10.2

The **same battery** on a RHEL 10.2 box — and the first run recorded on a **7200 RPM HDD**
rather than an SSD. `os` is the full sensed version (`rhel-10.2`, so `10.1 ≠ 10.2 ≠ 9.8`), so
these rows sit beside the others in the hub without conflict.

**Both compilers are green here.** g++-15 (15.2.1) and clang (21.1.0) each ran the *complete*
battery — ctest, smoke, full Debug, and full Release — at **116/116**, with peaks of **23.9M ops/s**
(g++-15) and **24.3M ops/s** (clang) at Release. The g++-15 rows are the reference below; the full
side-by-side is in [g++-15 vs clang](#g-15-vs-clang).

### Hardware

| | |
|---|---|
| CPU | Intel Core Ultra 7 265 — **12 cores** visible to the guest (a VM partition of the 20-core part) |
| RAM | 38 GB |
| Storage | Repo + builds on a **7200 RPM HDD** (virtio-backed, `rotational=1`). RunIdentity disk label **`x7k`** |
| OS | Red Hat Enterprise Linux 10.2 (Coughlan), kernel 6.12.0-211.22.1.el10_2.x86_64 |

The `disk` dimension is a **manual** RunIdentity label, not auto-sensed — the runner defaults to
`ssd`, so this box's runs were recorded with `disk_type=x7k` to keep the spindle honest in the
[hub](../test-summary/) path (`…/x7k/…`). This is the only platform here whose results live on a
spinning disk.

### Versions under test

Recorded at run time from each package's `version()`, not hand-typed:

| Package | Version |
|---------|---------|
| `jac313::Qlite::v001` | **v001.001** |
| `jac313::jText::v001` | **v001.001** |
| `jac313::Store::v001` | **v001.001** |

### Toolchains

Sensed automatically by `./bootstrap.sh` (activation-aware — gcc-toolset via `gcc-toolset-15-env`
on RHEL 10): **g++-15** (15.2.1, standard, via `gcc-toolset-15`) · **g++-14** (14.3.1,
conservative fallback) · **clang++** (21.1.0, first-class peer). CMake is the pinned **4.3.3**, so
the optional `import std;` pilot configures and builds here. The throughput rows below are the
**g++-15** reference, matching the per-platform convention — but clang is more than a peer in name
here: it runs the **complete battery** (ctest, smoke, and full Debug/Release) all **116/116**, and,
a first for the [hub](../test-summary/), every clang run is tracked alongside g++-15 under
`…/clang/…`. The two compilers land within run-to-run noise of each other — see
[g++-15 vs clang](#g-15-vs-clang) below.

### Tests & throughput

| Run | Result | Wall time | Peak hot-path | Persist footprint |
|-----|--------|-----------|---------------|-------------------|
| **ctest** (units, matrix bins, RunIdentity regressions) | 33/33 ✅ | seconds | — | — |
| **Smoke matrix** — 116 scenarios, minimal scale (Debug) | 116/116 ✅ | ~24 s | 1.99M ops/s | 9.5 MB |
| **Full matrix** — 116 at stress scale (Debug) | 116/116 ✅ | ~10 m 8 s | 12.8M ops/s | 611 MB |
| **Full matrix** — 116 at stress scale (**Release**) | 116/116 ✅ | ~10 m 3 s | **23.9M ops/s** | 611 MB |

The matrix shape is identical to the others (`116 = matrix tests × persist backend × output on/off`).

**Headline:** the hot path peaks at **~23.9M ops/sec at Release** (scenario `007_XS`, none/off) —
roughly **1.9× the Debug peak** (12.8M, `005_XS`). The test gates run on **Debug**, so the
day-to-day figure is the conservative one. This 12-vCPU VM sits a touch below the 20-core RHEL 9.8
reference on absolute throughput, as expected, and RunIdentity keeps each
`(OS × compiler × build-type)` row distinct in the [hub](../test-summary/) so they never conflate.

**The disk did not bite.** This is the one run on a 7200 RPM spindle, and it still came out
**CPU-bound, not disk-bound**: the **~611 MB** full-run footprint written over ~10 minutes is
≈ **1 MB/s** — orders of magnitude under a 7200 RPM disk's sequential bandwidth — and the Debug/Release
wall times (~10 m) land right alongside the SSD boxes. Same data is written under Debug and Release;
it lands in `test-results/` (gitignored, regenerable). The peak figure is the lock-free in-memory
path; with persistence attached a per-event mutex serializes the producer (see
[memory & concurrency](Verify.md)).

On CMake 4.3.3 the **`import std;`** pilot configures and builds for all three modules; the
std-module build measured **≈ +0.6 s (~+6%)** against textual modules on this box's g++-15 — small,
and in the same break-even neighborhood as the [Mint](Modules.md) and RHEL 9.8 results.

### g++-15 vs clang

Same box, same source, same x7k disk — the full battery run under both compilers. Every tier is
**116/116** on both, and the throughput differences are **within run-to-run noise**, not a real
compiler gap: clang edges Release, g++-15 edges Debug, and they trade the smoke peak. (g++-15
remains the per-platform reference for the headline rows above; this is the side-by-side.)

| Tier | g++-15 | clang 21.1.0 |
|---|---|---|
| ctest | 33/33 ✅ | 33/33 ✅ |
| Smoke (Debug) | 116/116 ✅ (~24 s, 1.99M ops/s) | 116/116 ✅ (~22 s, 2.13M ops/s) |
| Full Debug | 116/116 ✅ (~10 m 8 s, peak 12.8M) | 116/116 ✅ (~10 m 8 s, peak 11.1M) |
| Full Release | 116/116 ✅ (~10 m 3 s, peak **23.9M**) | 116/116 ✅ (~10 m 2 s, peak **24.3M**) |
| Persist footprint | 611 MB (full) | 611 MB (full) |

Both compilers' runs live in the [hub](../test-summary/) under distinct RunIdentity rows
(`…/gcc15/…` and `…/clang/…`), so they sit side by side without conflict.

---

## Honest caveats

- **One reference machine per platform, g++-15.** Faster cores → higher throughput; the hub
  carries the per-compiler / per-build-type breakdown.
- These are **functional + throughput** numbers, not a tuned benchmark. Tuning for your
  hardware/workload is a deployment choice (see
  [Architecture → Customization](Architecture.md#customization-and-performance)).
- Everything regenerates from scratch: `./bootstrap.sh` → `jac313_test_cli matrix run`
  (add `--release --build-dir build-<label>-release` for the Release row).

Part of the [jac313](../README.md) monorepo.
