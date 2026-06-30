# jac313

> 🚧 **Work in progress — `dev-work`.** We're moving from **cluster-yuck to work of art**.
> Right now it's still in-work (the test/results reporting is actively being rebuilt), so
> please **be patient** — expect rough edges and things shifting under you for a bit.

A versioned **C++23 umbrella** over a small family of in-tree libraries. Each package
re-exports a clean, namespaced API under `jac313::<Name>::v002`; together they're more
than their parts — a real **time-series logging store** with a complete local **testing
framework** and a worked example of **C++23 modules + `import std;`**.

> **The whole is bigger than the parts.** Two small libraries (Qlite, jText) compose into
> the headline feature (Store), all validated by a from-scratch testing/benchmark harness,
> and the whole thing doubles as a reproducible example for toolchain implementers (see
> [docs/Modules.md](docs/Modules.md)).

---

## The three packages

| Package | What it is | One line |
|---------|------------|----------|
| [**Qlite**](Qlite/README.md) | header-only **SQLite wrapper** (`jac313::Qlite::v002`) | RAII connection + variadic bind/column over `sqlite3`; Store uses it for SQL persistence |
| [**jText**](jText/README.md) | compiled **structured-text** library (`jac313::jText::v002`) | a self-describing text log format (light/full profiles) + streaming `JTextWriter`; Store uses it for human-readable logs |
| [**Store**](Store/README.md) | time-series **logging store** (`jac313::Store::v002`) | the headline feature — see below |

### Store — the logging feature

**Store is a high-throughput event logger.** A consumer logs events that carry **integers and
floats in the exact count they need** — the field widths are fixed at compile time to the
record shape you specify, so there's no per-event allocation or wasted space. Events flow
through a hot path and out to one or more persistence backends. It **uses jText** to emit
self-describing, human-readable logs and **uses Qlite** to persist to SQL.

Store is the largest part of jac313 — too much to absorb in one read, so it's documented by
feature (and is a good candidate for AI-assisted exploration of its strengths and trade-offs):

- [**Use cases**](docs/store/use-cases.md) — when & for what: metric count = event dimensionality (robot 3-D / fleet 2-D / pure logging 0-D), the Event as intent, and a gallery of domains
- [**Logging**](docs/store/logging.md) — the core: the event/record model, exact int/float field counts, throughput (batching, double-buffering)
- [**Categories**](docs/store/categories.md) — how events are categorized and sized (`bounded_string`, payload caps)
- [**Flags**](docs/store/flags.md) — the complete `StoreFlags` catalog: every bit, its meaning, and an honest wired / advisory / placeholder status
- [**Routing**](docs/store/bitmaps.md) — how flags + `PersistMode` route events (keeper vs database masks)
- [**Persistence**](docs/store/persistence.md) — the sinks: binary, jText, SQL, flag-routing; the binary log + reader
- [**Reporting**](docs/store/reporting.md) — reading the logs back: human-readable jText, queryable SQL

---

## Bigger than the parts

- **A complete testing framework.** [`jac313_test_cli`](tools/jac313_test_cli/) is the whole
  CI — it probes compilers, configures/builds, runs ctest, and drives a **persist × scale
  matrix** (115 smoke scenarios → full stress runs), recording every metric to a tracked
  results DB keyed by a `(os, compiler, build_type, disk, size)` **RunIdentity**. No GitHub
  Actions, no shell scripts. See [docs/Architecture.md](docs/Architecture.md) and
  [docs/Setup.md](docs/Setup.md#4-testing).
- **C++23 modules and `import std;`, with all its quirks, blessings, and curses.** All three
  umbrella modules build module-native against `import std;` (opt-in, gcc-only). It measured
  **break-even** here — and the real value is as a worked, reproducible example for **CMake,
  GCC, and Clang** to harden their toolchains against. The full story, the numbers, and the
  friction: [docs/Modules.md](docs/Modules.md).

---

## Documentation

| Doc | Contents |
|-----|----------|
| [docs/QuickStart.md](docs/QuickStart.md) | **Start here** — the fast **base check**: `./jac313_test_cli --ctest --smoke` to a green tree in ~20 s (composable preset gates; writes a re-runnable `run_latest_config.sh`) |
| [docs/Setup.md](docs/Setup.md) | Toolchain, **bootstrapping**, building, and **running the tests** |
| [docs/RunAllTests.md](docs/RunAllTests.md) | One-stop **runbook** — the full battery (gcc15 + clang; smoke + full Debug + full Release; modules + no-modules; optional valgrind) |
| [docs/Modules.md](docs/Modules.md) | C++23 modules + the `import std;` story (quirks / blessings / curses) |
| [docs/Architecture.md](docs/Architecture.md) | Layout, the umbrella model, the test pipeline, customization, design principles |
| [docs/store/](docs/store/) | Store by feature — logging, categories, bitmaps, persistence, reporting |
| [docs/Memory-And-Concurrency.md](docs/Memory-And-Concurrency.md) | valgrind memcheck + helgrind/DRD — method, the tail-reader data race found & fixed, results, and honest limits |
| [docs/Maturity.md](docs/Maturity.md) | Honest self-assessment on a 12-level scale — where jac313 sits (~7), and why that's a deliberate ceiling |
| [docs/Benchmarks.md](docs/Benchmarks.md) | **How to run the throughput suite** — `store_bench --suite` / `--report`, the curated 10 configs, the `--dry-run` command list, and a DB-backed [results page](test-summary/) |
| [docs/HowToReadResults.md](docs/HowToReadResults.md) | **Read this before trusting any `ops/sec`** — what each number measures: in-memory ceiling (~15–25M) vs durable rate (~2.1–2.7M), why the headline is the **median + low–high band** (not lucky-max), and how flushing durable writes inside the clock corrects the ranking |
| [docs/Results.md](docs/Results.md) | v002 results — **pending its first matrix run** (faithful copy of v001; validated green, but no per-platform throughput recorded yet) |
| [docs/Bloopers.md](docs/Bloopers.md) | The bugs we shipped to ourselves, owned and explained — newest first. **#1 (today): "1,856 Tests, Not One You'd Quote"** — how the sprawling benchmark matrix became the honest `store_bench` suite |
| Package READMEs | [Qlite](Qlite/README.md) · [jText](jText/README.md) · [Store](Store/README.md) |

> **🚧 Results are being simplified.** The old matrix's `results_db` + committed
> `test-summary/` report pages are being **retired** — those numbers were never
> trustworthy (lucky-max headlines, smeared durable rows; see [Bloopers #1](docs/Bloopers.md)),
> so nothing of value is lost. Going forward, throughput comes from the lean
> [`store_bench` suite](docs/Benchmarks.md) (median + low–high band) and the new results
> land in **one small database**. The old convoluted reports are going away and the new
> simple ones are the way forward — **within a few days.**

---

## Quick start

Run everything from the version directory (`v002/`). The shared `.git` lives at the repo root
(`jac313/`), so `bootstrap.sh` installs the pre-push hook to the **parent** `.git/hooks` — that's
expected under the `v00N/` layout, not a bug.

```bash
cd v002 && ./bootstrap.sh          # sense toolchain → build the test runner → readiness check
```

`bootstrap.sh` senses the compiler (on RHEL it activates `gcc-toolset-15` for you), checks the full
one-pass baseline (g++ ≥ 14, Clang ≥ 20, CMake ≥ 3.26 — exactly **4.3.3** for the `import std` pilot,
Ninja ≥ 1.11, the sqlite3 dev header, **valgrind + the memcheck/helgrind/DRD headers**, a UTF-8
locale), and if anything is missing hands a manifest to the **committed, fully-static `jac313_setup`**
provisioner — which installs it with a plan preview + `[y/N]` (or, if that binary can't run here, a
resilient generated `Setup.sh`). It then builds **only the test runner** (Debug) and drops
`./jac313_test_cli` (plus `./jac313_wipe_all`/`wipe_one`/`wipe_jac`) symlinks at the `v002/` root.
See [docs/Setup.md](docs/Setup.md) for the provisioner, the wipe tools, and rebuilding `jac313_setup`.

**Base check** — one entry point, composable gate flags; no `scl` prefix, no raw cmake (see
**[docs/QuickStart.md](docs/QuickStart.md)**):

```bash
./jac313_test_cli --ctest --smoke      # ctest unit suite (36) + persist×output smoke matrix (116), ~20 s
```

Every preset invocation writes the exact commands to **`./run_latest_config.sh`** and runs them, so
you can re-run the identical run with `bash run_latest_config.sh`. The gates compose:

```bash
./jac313_test_cli --ctest              # just the unit suite
./jac313_test_cli --smoke              # just the smoke matrix
./jac313_test_cli --bench              # throughput numbers (not recorded)
./jac313_test_cli --bench --report     # record → test-summary/ + render the report
./jac313_test_cli --verify-lite        # valgrind memcheck gate (the pre-push hook)
```

`--bench --report` auto-resolves the machine to `jac313-<group_id>` by matching its **hardware + OS**
`(cpu, cores, ram_gb, os)` against the recorded groups — same hardware+OS reuses its group, no real
hostname stored, no manual `host_label.local` (run `--group-id` to preview). For a second compiler,
add `--clang` and re-run into the same DB. Full battery
(clang, `--size full`, modules) and the explicit single-cell `runner`: [docs/RunAllTests.md](docs/RunAllTests.md).

C++26 baseline (`cxx_std_26`; g++-15 / Clang 21). Full prerequisites, the toolchain story, and the
benchmark runbook are in [docs/Setup.md](docs/Setup.md) and [docs/Benchmarks.md](docs/Benchmarks.md).

**Status:** v002 is a **faithful copy of v001**, rebranded to `jac313::*::v002`. It builds clean
(g++-15, modules + textual) and passed validation (smoke **116/116**; `matrix verify` **60/60**
valgrind-clean), but has **not** recorded its own per-platform results matrix yet — see
[docs/Results.md](docs/Results.md). The ISO **C++26** baseline is now in: the build is on
`cxx_std_26` (compiles clean on gcc-toolset-15 / GCC 15.2) and a `jac313::contracts`
pre/post/assert shim has landed (runtime checks today; migrates to native P2900 contracts
once `__cpp_contracts` is available — gcc-15/clang-21 don't define it yet). v002 is
staged and isolation-verified, continuing to diverge. `import std;` carries over from v001
(opt-in, gcc-only; needs the pinned CMake 4.3.3). The legacy repos (`jacQlite`, `jText`,
`ts_store`) remain only as historical origin.

---

## Security & production use

**Maturity / scope.** This project targets a learning and reference maturity level. Additional
security work — formal security review, fuzzing, sanitizer (ASan/UBSan) runs, and vulnerability
scanning — is possible but is outside the scope of this project.

**Production use.** This is reference software, not a production-ready component. Adapting it for
production is your undertaking: you customize it for your environment and perform your own
security analysis, fuzzing, hardening, dependency and vulnerability scanning, and threat modeling
on your customized code.

**No warranty — any use.** Whether you run this in production or only as a test platform makes no
difference: it is provided "AS IS", with no warranty, express or implied. The author assumes no
liability for any damages or losses arising from the software or its use; you assume all risk.
The governing terms are in the [LICENSE](LICENSE).

---

## A note from the author

Most of this code was largely generated with AI — Grok (Build and Composer) and Claude —
working alongside me. A few honest things worth saying up front:

- **Not every feature is covered by the tests.** The matrix gate is real and exercises a lot,
  but it doesn't reach everything.
- **Builds take space, and the full tests are I/O-heavy.** A full local gate leaves on the order
  of a gigabyte of build trees, and the full / bench matrix writes enough to tax an HDD or put
  some wear on an SSD. Use the smoke gate day-to-day; reach for the full matrix deliberately.
- **My time on this is limited.** I did the most, and the best, I could in the time I allowed
  myself to work on it.

I hope you find it useful. In theory it could be used in a lot of different ways; in practice,
it's whatever you can figure out how to use it for.

