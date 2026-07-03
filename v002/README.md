# jac313

A carefully crafted C++ ecosystem built around one compelling idea: **Store**, a high-performance, fully configurable time-series event logger that is genuinely greater than the sum of its parts.

At its core are two elegant supporting libraries:

- **Qlite** — a clean, modern SQLite wrapper with rich ergonomics
- **jText** — a self-describing, human-readable structured text format

Together they deliver multiple persistence backends while keeping the hot path extremely fast.

What makes jac313 special is the complete, serious testing and benchmarking world built around it. v002 is the evolving C++26 frontier (currently compiled as C++26 with limited feature adoption so far) while still sharing the same rigorous validation approach that has already helped expose real toolchain issues.

This is modern C++ done with care: real code, real scale, real results.

The whole is bigger than the parts — and it shows.

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

---

## Documentation

| Doc | Contents |
|-----|----------|
| [This README (Quick start)](#quick-start) | **Start here** — the 5 lines to bootstrap + run tests |
| [docs/Benchmarks.md](docs/Benchmarks.md) | Throughput benchmarks |
| [docs/Results.md](docs/Results.md) | Results per platform |
| [docs/Modules.md](docs/Modules.md) | C++23 modules + the `import std;` story (quirks / blessings / curses) |
| [docs/Architecture.md](docs/Architecture.md) | Layout, the umbrella model, the test pipeline, customization, design principles |
| [docs/store/](docs/store/) | Store by feature — logging, categories, bitmaps, persistence, reporting |
| [docs/verify.md](docs/verify.md) | valgrind memcheck + helgrind/DRD — method, the tail-reader data race found & fixed, results, and honest limits |
| [docs/Maturity.md](docs/Maturity.md) | Honest self-assessment on a 12-level scale — where jac313 sits (7.5), and why that's a deliberate ceiling |
| [docs/Benchmarks.md](docs/Benchmarks.md) | **How to run the throughput suite** — `store_bench --suite` / `--report`, the curated 10 configs, the `--dry-run` command list, and a DB-backed [results page](test-summary/) |
| [docs/Results.md](docs/Results.md) | v002 results — **pending its first matrix run** (faithful copy of v001; validated green, but no per-platform throughput recorded yet) |
| Package READMEs | [Qlite](Qlite/README.md) · [jText](jText/README.md) · [Store](Store/README.md) |

> **🚧 Results are being simplified.** The old matrix's `results_db` + committed
> `test-summary/` report pages are being **retired** — those numbers were never
> trustworthy (lucky-max headlines, smeared durable rows), so nothing of value is lost.
> Going forward, throughput comes from the lean [`store_bench` suite](docs/Benchmarks.md)
> (median + low–high band) and the new results land in **one small database**. The old
> convoluted reports are going away and the new simple ones are the way forward —
> **within a few days.**

**Status:** v002 is a **faithful copy of v001**, rebranded to `jac313::*::v002`. It builds clean
(g++-15, modules + textual) and passed validation (smoke **116/116**; `matrix verify` **60/60**
valgrind-clean), but has **not** recorded its own per-platform results matrix yet — see
[docs/Results.md](docs/Results.md). The ISO **C++26** baseline is now in (`cxx_std_26`). We currently use a small number of C++26-era
library facilities (via shims or workarounds) and a contracts shim that provides runtime checks
today. Most new C++26 language features are not yet used in the main code. We expect to
re-evaluate and adopt more C++26 features within the next 12 months. `import std;` carries over
from v001 (opt-in; gcc + clang; needs the pinned CMake 4.3.3). v002 is staged and
isolation-verified, continuing to diverge. The legacy repos (`jacQlite`, `jText`, `ts_store`)
remain only as historical origin.

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

