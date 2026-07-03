# jac313

> 🚧 **Work in progress — `dev-work`.** We're still making changes, but it is not cluster-yuck anymore.
> The test/results reporting is actively being polished, so please **be patient** —
> expect some rough edges and things shifting under you for a bit.

> 🔁 **Heavy results churn — next few days (as of 2026-06-29).** We're tweaking settings and
> running the **full battery repeatedly across the fleet, including fresh virtual machines**,
> so expect frequent **runs and DB wipes**: `test-summary/results.db` and the rendered pages
> change often, and machines come and go (via the `jac313_wipe_all` / `wipe_one` / `wipe_jac`
> tools). Treat anything under `test-summary/` as **in flux** until this settles.

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
| [**Qlite**](Qlite/README.md) | header-only **SQLite wrapper** (`jac313::Qlite::v002`) | RAII connection, variadic bind/column, stmt cache, transactions, `get_one_*` helpers; Store uses it for SQL persistence |
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
  Actions, no shell scripts. See [docs/Architecture.md](docs/Architecture.md). Testing details are in this README's Quick start section.
- **C++23 modules and `import std;`, with all its quirks, blessings, and curses.** All three
  umbrella modules build module-native against `import std;` (opt-in; gcc and clang on libstdc++). It measured
  **break-even** here — and the real value is as a worked, reproducible example for **CMake,
  GCC, and Clang** to harden their toolchains against. The full story, the numbers, and the
  friction: [docs/Modules.md](docs/Modules.md).

---

## Documentation

| Doc | Contents |
|-----|----------|
| This README (Quick start) | **Start here** — the 5 lines to bootstrap + run tests |
| [docs/Benchmarks.md](docs/Benchmarks.md) | Throughput benchmarks |
| [docs/Results.md](docs/Results.md) | Results per platform |
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

Run from inside the `v002/` directory.

```bash
./bootstrap.sh                      # (you may have to run this more than once)

./jac313_test_cli --ctest           # the smallest of all testing
./jac313_test_cli --smoke           # mid level testing
./jac313_test_cli --run-everything  # just like it says, including ctests, smoke tests, benchtests, verify, verify-lite
```

`./bootstrap.sh` prepares everything (compilers, headers, valgrind, etc.). You may need sudo once and will likely run it twice the first time.

`./jac313_test_cli` is the one main command:

- `--ctest` — smallest check
- `--smoke` — typical daily test (recommended starting point)
- `--run-everything` — full battery on both compilers (ctest + smoke + bench + verify)

This is the minimum to get set up and see results. The tools hide the complexity so you can start working immediately.

For more details on the project, see the other docs in this directory (Benchmarks.md, Results.md, Architecture.md, etc.).

**Status:** v002 is a **faithful copy of v001**, rebranded to `jac313::*::v002`. It builds clean
(g++-15, modules + textual) and passed validation (smoke **116/116**; `matrix verify` **60/60**
valgrind-clean), but has **not** recorded its own per-platform results matrix yet — see
[docs/Results.md](docs/Results.md). The ISO **C++26** baseline is now in: the build is on
`cxx_std_26` (compiles clean on gcc-toolset-15 / GCC 15.2) and a `jac313::contracts`
pre/post/assert shim has landed (runtime checks today; migrates to native P2900 contracts
once `__cpp_contracts` is available — gcc-15/clang-21 don't define it yet). v002 is
staged and isolation-verified, continuing to diverge. `import std;` carries over from v001
(opt-in; gcc + clang; needs the pinned CMake 4.3.3). The legacy repos (`jacQlite`, `jText`,
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

