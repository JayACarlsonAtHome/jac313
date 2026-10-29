# jac313

A carefully crafted C++23 ecosystem built around one compelling idea: **Store**, a high-performance, fully configurable time-series event logger that is genuinely greater than the sum of its parts.

At its core are two elegant supporting libraries:

- **Qlite** — a clean, modern SQLite wrapper
- **jText** — a self-describing, human-readable structured text format

Together they deliver multiple persistence backends (binary, jText, HTML, JSON, SQL, flag-routed) while keeping the hot path fast and allocation-free.

What makes this project special is the complete world built around it: a serious, from-scratch testing and benchmarking harness that actually exercises the code at scale across compilers and configurations. This is not a toy — it's a real codebase that has already helped surface issues in production toolchains.

v001 is the solid, battle-tested C++23 baseline with full matrix results recorded across multiple platforms.

The whole is bigger than the parts — and it shows in both the engineering and the results.

> **Linux only.** Uses Linux/POSIX APIs (`fork`/`exec`, `mmap`, `/proc`, etc.).


---

## The three packages

| Package | What it is | One line |
|---------|------------|----------|
| [**Qlite**](jText/README.md) (in jText) | header-only **SQLite wrapper** (`jac313::Qlite::v001`) | RAII connection + variadic bind/column over `sqlite3`; Store uses it for SQL persistence |
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
- [**Persistence**](docs/store/persistence.md) — the sinks: binary, jText, HTML, JSON, SQL, flag-routing; the binary log + reader
- [**Reporting**](docs/store/reporting.md) — reading the logs back: jText, HTML, JSON, SQL, binary + readers

---

## Bigger than the parts

- **A complete testing framework.** [`jac313_test_cli`](tools/jac313_test_cli/) is the whole
  CI — it probes compilers, configures/builds, runs ctest, and drives a **persist × scale
  matrix** (116 smoke scenarios → full stress runs), recording every metric to a tracked
  results DB keyed by a `(os, compiler, build_type, disk, size)` **RunIdentity**. No GitHub
  Actions, no shell scripts. See [docs/Architecture.md](docs/Architecture.md). Testing details are in this README's Quick start section.
- **C++23 modules**, additive over the headers, for all three libraries; they build with the
  distro CMake (>= 3.28). An `import std;` build was tried, measured break-even, and removed
  (it needed exactly CMake 4.3.3). Details and pitfalls: [docs/Modules.md](docs/Modules.md).

---

## Quick start

Run from inside the `v001/` directory.

```bash
./bsir.sh                           # one question, one sudo password; a few minutes later the Runner opens
./bsir.sh --no-gui                  # no window (a server without a screen): tests only

./jac313_test_cli --ctest           # the smallest of all testing
./jac313_test_cli --smoke           # mid level testing
./jac313_test_cli --run-everything  # just like it says, including ctests, smoke tests, benchtests, verify, verify-lite
```

`./bsir.sh` (BSIR: **B**ootstrap, **S**etup, **I**nstall & **R**un; it was called `bootstrap.sh` until 2026-10-23) prepares everything (compilers, headers, valgrind, etc.) in one run: it asks once "Install everything?", asks for the sudo password once, installs what is missing, builds the tests, sets up a PostgreSQL 18 server on this machine for the PG local gate, installs U++ and builds the Jac313Runner window, adds DBeaver (a database browser, its own pinned package), and opens the Runner. `--yes` skips the question; `--no-gui` leaves out the window.

**PostgreSQL.** The passwords live in a `SQL_Secrets` folder beside the project (`~/git/SQL_Secrets` for `~/git/jac313`), never in git. PG local works after bootstrap: it creates the role and database `jac313` on localhost and writes their random password to `SQL_Secrets/jac313_pg_local.pgpass`. PG remote is a server the test machines share: in the Runner, fill in Remote PG's Host and press **Setup Remote PG Server...**. It runs [setup/setup_pg_server.sh](../setup/setup_pg_server.sh) on that server, either over ssh in a terminal (**Do it for me**: the Runner makes the password and keeps it in `SQL_Secrets/jac313_pg_remote.pgpass`) or by hand (**Show me the steps**, then type the password it prints). The script installs PostgreSQL 18, creates the role and database `jac313` (an existing role keeps its password), and lets in only the network you confirm. When role `jac313` already exists there, **Fetch password (ssh + sudo)** reads the password from where the test machines store it on that server (`SQL_Secrets/*.pgpass`, `.pgpass`; PostgreSQL itself keeps only a hash) and keeps the one that logs in. Then **Test & Finish** tests the server and registers this machine's `jac313-###` on it. *These shortcuts (one shared role, passwords read with sudo and copied between machines) are for test machines holding throwaway test data only; never do this on a real production server.* By hand instead: create a login role and database there (e.g. both named `jac313`), write one line `host:5432:jac313:jac313:password` to `SQL_Secrets/jac313_pg_remote.pgpass` (mode 600), type the host into the Runner's Remote PG fields and press **Test**. [docs/SQL_Secrets_example](../docs/SQL_Secrets_example/README.md) is a sample folder (fake passwords) with the steps for both.

The window part installs U++ from [upp_basefix](https://github.com/JayACarlsonAtHome/upp_basefix) into `$HOME/upp_basefix` (U++ pinned to a GitHub commit plus patches; an existing install is kept) and builds `../gui/out/Jac313Runner` with GCC. Run bootstrap, the tests and the GUI as your own user, from inside this directory.

`./jac313_test_cli` is the one main command:

- `--ctest` — smallest check
- `--smoke` — typical daily test (recommended starting point)
- `--run-everything` — full battery on both compilers (ctest + smoke + bench + verify)

This is the minimum to get set up and see results. The tools hide the complexity so you can start working immediately.

For more details on the project, see the docs in [docs/](docs/) (listed under Documentation below).

### Everyday notes

- **Run as your own user.** `jac313_test_cli` (and the GUI) refuse to run as root in a tree a
  regular user owns: a root run leaves root-owned build trees, logs and `/tmp` files your later
  runs cannot overwrite, and valgrind will not start binaries that have no "other" execute bit
  when it runs as root. A tree root owns (a container, CI) runs as usual. `--allow-root` overrides.
- **Why did a gate fail?** Every valgrind task's verdict and the first 400 characters of its
  output are kept in `results.db` (table `runEvent`), so a failure can be read even when no
  logs were kept. `--keep-logs` (with the preset gates, e.g. `--ctest --smoke --verify-lite`)
  also keeps full captures under `kept-logs/`; those are local only (a full run can reach
  12+ GB), so do not commit them, and clear them with `--wipe-logs`.
- **Explain a run, never delete it.** `./jac313_test_cli note RUN_ID "TEXT"` adds a note to a
  run (say, a failure caused by the environment, not the code); `./jac313_test_cli note RUN_ID`
  lists its notes. Recorded results are never changed.
- **Rebuild the GUI** with `./jac313_test_cli gui-build [--method GCC|CLANG] [--clean]` (U++ from
  `$UPP_DIR`, default `$HOME/upp_basefix`). Use `--clean` when a link fails with undefined
  references (stale U++ cache).

---

## Documentation

| Doc | Contents |
|-----|----------|
| [docs/Modules.md](docs/Modules.md) | C++23 modules: build, pitfalls, and why `import std;` was removed |
| [docs/Architecture.md](docs/Architecture.md) | Layout, the umbrella model, the test pipeline, customization, design principles |
| [docs/Verify.md](docs/Verify.md) | valgrind memcheck + helgrind/DRD — method, the tail-reader data race found & fixed, results, and honest limits |
| [docs/Maturity.md](docs/Maturity.md) | Honest self-assessment on a 12-level scale — where jac313 sits (7.5), and why that's a deliberate ceiling |
| [docs/Benchmarks.md](docs/Benchmarks.md) | **How to run the throughput suite** — `--bench --report` (the curated `store_bench` suite), the configs, `--dry-run` copy-paste commands, and a sample report |
| [docs/Results.md](docs/Results.md) | What a clean build + test run actually produces, **per platform** — Linux Mint (116/116, ~2.9M events/s, ~611 MB), RHEL 9.8 (116/116, up to ~24.3M ops/s; CMake 3.31.8-vs-4.3.3 import-std comparison), and RHEL 10.2 (116/116 on **both g++-15 and clang**, up to ~24.3M ops/s, on a 7200 RPM HDD) |
| Package READMEs | [jText](jText/README.md) · [Store](Store/README.md) |

**Status:** all three packages in-tree (no sibling repos); ctest + 116/116 smoke matrix green on
GCC and Clang on **Linux Mint**, **RHEL 9.8**, and **RHEL 10.2** — on **both RHEL 9.8 and RHEL 10.2,
g++-15 and clang each ran the full Debug + Release matrix green** (116/116; Release peaks ~24.3M
(gcc) / ~20.9M (clang) on 9.8 and ~23.9M (gcc) / ~24.3M (clang) on 10.2; see
[docs/Results.md](docs/Results.md), throughput rows are the g++-15 reference per platform);
The legacy repos (`jacQlite`, `jText`, `ts_store`) remain only as historical origin.

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

