# jac313

A versioned **C++23 umbrella** over a small family of in-tree libraries. Each package
re-exports a clean, namespaced API under `jac313::<Name>::v001`; together they're more
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
| [**Qlite**](Qlite/README.md) | header-only **SQLite wrapper** (`jac313::Qlite::v001`) | RAII connection + variadic bind/column over `sqlite3`; Store uses it for SQL persistence |
| [**jText**](jText/README.md) | compiled **structured-text** library (`jac313::jText::v001`) | a self-describing text log format (light/full profiles) + streaming `JTextWriter`; Store uses it for human-readable logs |
| [**Store**](Store/README.md) | time-series **logging store** (`jac313::Store::v001`) | the headline feature — see below |

### Store — the logging feature

**Store is a high-throughput event logger.** A consumer logs events that carry **integers and
floats in the exact count they need** — the field widths are fixed at compile time to the
record shape you specify, so there's no per-event allocation or wasted space. Events flow
through a hot path and out to one or more persistence backends. It **uses jText** to emit
self-describing, human-readable logs and **uses Qlite** to persist to SQL.

Store is the largest part of jac313 — too much to absorb in one read, so it's documented by
feature (and is a good candidate for AI-assisted exploration of its strengths and trade-offs):

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
| [docs/Setup.md](docs/Setup.md) | Toolchain, **bootstrapping**, building, and **running the tests** |
| [docs/RunAllTests.md](docs/RunAllTests.md) | One-stop **runbook** — the full battery (gcc15 + clang; smoke + full Debug + full Release; modules + no-modules; optional valgrind) |
| [docs/Modules.md](docs/Modules.md) | C++23 modules + the `import std;` story (quirks / blessings / curses) |
| [docs/Architecture.md](docs/Architecture.md) | Layout, the umbrella model, the test pipeline, customization, design principles |
| [docs/store/](docs/store/) | Store by feature — logging, categories, bitmaps, persistence, reporting |
| [docs/Memory-And-Concurrency.md](docs/Memory-And-Concurrency.md) | valgrind memcheck + helgrind/DRD — method, the tail-reader data race found & fixed, results, and honest limits |
| [docs/Maturity.md](docs/Maturity.md) | Honest self-assessment on a 12-level scale — where jac313 sits (~7), and why that's a deliberate ceiling |
| [docs/Benchmarks.md](docs/Benchmarks.md) | **How to run the throughput suite** — `store_bench` + `bench_suite.sh`, the curated 7 configs, `--dry-run` copy-paste commands, and a sample report |
| [docs/HowToReadResults.md](docs/HowToReadResults.md) | **Read this before trusting any `ops/sec`** — what each number measures: in-memory ceiling (~15–25M) vs durable rate (~2.1–2.7M), why the headline is the **median + low–high band** (not lucky-max), and how flushing durable writes inside the clock corrects the ranking |
| [docs/Results.md](docs/Results.md) | What a clean build + test run actually produces, **per platform** — Linux Mint (116/116, ~2.9M events/s, ~611 MB), RHEL 9.8 (116/116, up to ~24.3M ops/s; CMake 3.31.8-vs-4.3.3 import-std comparison), and RHEL 10.2 (116/116 on **both g++-15 and clang**, up to ~24.3M ops/s, on a 7200 RPM HDD) |
| [docs/Bloopers.md](docs/Bloopers.md) | The bugs we shipped to ourselves, owned and explained — newest first. **#1 (today): "1,856 Tests, Not One You'd Quote"** — how the sprawling benchmark matrix became the honest `store_bench` 7-config suite |
| Package READMEs | [Qlite](Qlite/README.md) · [jText](jText/README.md) · [Store](Store/README.md) |

---

## Quick start

```bash
./bootstrap.sh                                  # fresh box: sense → build the test runner → hand off
cmake -G Ninja -S . -B build-gcc15 -DCMAKE_CXX_COMPILER=g++-15 -DJAC313_BUILD_STORE_TESTS=ON
cmake --build build-gcc15 && ctest --test-dir build-gcc15
```

C++23 required (`g++-15` / Clang 20 standard); full prerequisites and the test tiers are in
[docs/Setup.md](docs/Setup.md).

**Status:** all three packages in-tree (no sibling repos); ctest + 116/116 smoke matrix green on
GCC and Clang on **Linux Mint**, **RHEL 9.8**, and **RHEL 10.2** — on **both RHEL 9.8 and RHEL 10.2,
g++-15 and clang each ran the full Debug + Release matrix green** (116/116; Release peaks ~24.3M
(gcc) / ~20.9M (clang) on 9.8 and ~23.9M (gcc) / ~24.3M (clang) on 10.2; see
[docs/Results.md](docs/Results.md), throughput rows are the g++-15 reference per platform);
`import std;` done for all three modules (opt-in, gcc-only; needs the pinned CMake 4.3.3). The
legacy repos (`jacQlite`, `jText`, `ts_store`) remain only as historical origin.

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

