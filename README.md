# jac313

A carefully crafted C++ ecosystem built around one compelling idea: **Store**, a high-performance, fully configurable time-series event logger that is genuinely greater than the sum of its parts.

> **Linux only.** The project uses Linux/POSIX-specific facilities (`/proc/meminfo`, `posix_fallocate` + `mmap`, `fork`/`exec`, pipes, etc.) and does not support macOS or Windows.

At its core are two elegant, self-contained in-tree libraries:

- **Qlite** — a clean, modern SQLite wrapper with transactions, prepared-statement caching, and ergonomic helpers
- **jText** — a self-describing, human-readable structured text format with a streaming writer

Together they power multiple persistence backends (binary, jText, HTML, JSON, SQL, flag-routed) while keeping the hot path allocation-free and extremely fast.

What makes jac313 special is the complete world built around it: a real testing and benchmarking harness (`jac313_test_cli`) that drives full matrices across compilers, standards, modules, and persistence strategies. This is not a toy project — it is a serious, non-trivial codebase that has already exposed real bugs in production toolchains.

Each major version is a complete, independent universe:

- **v001** — the solid, battle-tested C++23 baseline
- **v002** — the evolving C++26 frontier (compiled as C++26; limited feature adoption so far, with plans to add more within ~12 months)

This is modern C++ as it should be experienced: clean namespaced APIs, C++23 modules with `import std;`, rigorous validation, and the kind of depth that only comes from building something real end-to-end.

The whole is bigger than the parts — and it shows.

---

## 🔬 It found a real compiler bug

The reason to build a non-trivial, **two-standard** codebase is to make real toolchains
misbehave where hello-world never will. v002 (the C++26 world) did exactly that:

> **clang 21 refuses to compile well-formed C++26 `std::print` with a *dynamic-width* spec
> (`{:^{}}`) that gcc 15 accepts — and that clang itself accepts in C++23.**

| Compiler | `-std=c++23` | `-std=c++26` |
|---|---|---|
| gcc 15.2.1 | ✅ | ✅ |
| clang 21.1.8 | ✅ | ❌ **rejects valid code** |

We isolated it to a [**10-line reproducer**](v002/docs/findings/clang_cpp26_dynamic_format.cpp),
traced it to libstdc++'s declared-but-undefined consteval helper `__check_dynamic_spec`, and
**worked around it** with `std::runtime_format` so v002 builds green on *both* compilers and the
full matrix runs on both. Full writeup:
[v002/docs/Cpp26_Adoption.md](v002/docs/Cpp26_Adoption.md#toolchain-finding-clang-rejects-dynamic-width-stdprint-in-c26).

This is the project's purpose in one bug: only a real C++26 program — dynamic-width logging
output, two compilers, a real build — hits it.

---

## The headline: Store — a configurable logging tool

**Store is a high-throughput, configurable event logger.** A consumer logs events that
carry **integers and floats in the exact count they need** — the field widths are fixed
at compile time to the record shape you specify, so there's no per-event allocation or
wasted space. Events flow through a hot path and out to one or more **persistence
backends**, and what gets persisted (and where) is **configurable** via flags and a
routing model:

- **binary** — a compact binary event log (+ a reader)
- **jText** — self-describing, human-readable logs
- **SQL** — queryable rows via SQLite
- **in-memory / none** — for the hot path with no sink
- **flag-routing** — flags + a persist mode decide which events go to which sink
  (keeper vs database masks)

Store is the largest part of the project and the thing the rest exists to serve.

New here? **[Store use cases](v002/docs/store/use-cases.md)** is the best on-ramp — *when & for
what*: the int/float metric count is the event's **dimensionality** (robot 3-D, fleet 2-D, pure
logging 0-D) and the **Event** carries its intent, with a gallery of domains.

### The supporting sub-components

| Component | What it is | Role |
|-----------|------------|------|
| **Store** | the time-series logging tool (`jac313::Store::vNNN`) | the headline program — the hot path + the configurable persist sinks |
| **jText** | compiled structured-text library (`jac313::jText::vNNN`) | a self-describing text log format (light/full profiles) + a streaming `JTextWriter`; Store uses it for human-readable logs |
| **Qlite** | header-only SQLite wrapper (`jac313::Qlite::vNNN`) | RAII connection + variadic bind/column over `sqlite3` — plus a RAII transaction guard, a prepared-statement cache, `string_view`/blob binds, column-by-name, and a non-throwing `std::expected` path; Store uses it for SQL persistence |
| **Setup** | toolchain sensing (`jac313::Setup::vNNN`) | a data-driven, activation-aware compiler registry — new platforms are config, not code |

Two small libraries compose into the headline feature, and the headline feature is
validated by the testing framework below — the whole is bigger than the parts.

---

## The versions: a C++23 baseline and a C++26 frontier

Major versions are **fully duplicated** — DRY is *not* applied across them. Each `v00N/`
is a complete copy so that testing stays isolated (changing v001 only tests v001), there
is no cross-version build/test explosion, and each version can move its toolchain and
language standard independently.

| Version | Standard | State |
|---------|----------|-------|
| [**v001/**](v001/README.md) | **C++23** | The proven baseline — builds and tests green on g++-15 and Clang across multiple Linux distros; the full persist × scale matrix is recorded. Modules + `import std` work (opt-in, gcc-only). |
| [**v002/**](v002/README.md) | **C++26** | The evolving frontier — compiled as C++26. Currently using a small number of C++26 library facilities (via shims/workarounds) and a contracts shim (runtime checks today). Most new C++26 language features not yet adopted in main code. We plan to re-evaluate and add more within the next 12 months. Modules + `import std` work (opt-in). |

The one coordination cost of full duplication: a shared-infra fix made *before* the two
versions diverge must be applied to each `v00N/`. After they diverge, they are free to
differ.

---

## The testing framework — and why it's a useful window for toolchains

There is **no GitHub Actions and no shell-script CI.** The whole pipeline is a compiled
C++ program, [`jac313_test_cli`](v001/tools/jac313_test_cli/): it probes the available
compilers (via the data-driven registry), configures and builds the tree, runs `ctest`,
and drives a **persist × scale matrix** (smoke → full stress), recording every metric to
a tracked results DB keyed by a `(os, compiler, build_type, disk, size, modules)`
**RunIdentity**. It also runs valgrind gates (memcheck + helgrind/DRD).

Because this is a **non-trivial but real** codebase, it ends up being a genuinely useful
**worked example for toolchain implementers** — a window onto how **CMake, GCC, and
Clang** behave on real program setups, not on a hello-world:

- **C++23 modules + `import std`** built module-native — exactly the kind of thing CMake's
  module scanning, GCC's module BMIs, and Clang's module support need real targets to
  harden against.
- **Two standards in one realistic codebase** — a stable C++23 world *and* an evolving
  C++26 world side by side, so a toolchain can be exercised against both without leaving
  the same project.
- **A real multi-axis matrix** — compilers × build types × modules-on/off × persistence
  backends × scale × OS, on real hardware — surfacing behavior that single-config builds
  never reach.

If you implement or test a toolchain, this is a reproducible, self-contained project you
can point it at.

---

## The setup philosophy (non-standard, but flexible — and emulatable)

This layout is admittedly **not the conventional one.** But it has proven to be a
flexible structure that others could emulate:

- **A compiled runner instead of CI YAML.** All the intelligence — compiler sensing,
  build orchestration, the matrix, the results DB, valgrind gates — lives in one C++ tool,
  not in scattered shell or pipeline config. Shell is reduced to a thin `bootstrap.sh`
  that exists only because you can't run a compiled tool before you have a compiler.
- **A data-driven, activation-aware compiler registry** (`Setup/compilers.conf`) — handles
  gcc-toolset activation (scl / env-launcher / source-script) so new platforms are config,
  not code. Per-platform install commands are data too (`Setup/recipes.conf`).
- **Bootstrap → provision → hand-off.** `bootstrap.sh` senses the host; if prerequisites are
  missing it hands a small manifest to a **committed, fully-static `jac313_setup`** provisioner
  (or a resilient generated `Setup.sh` fallback) that installs them with a `[y/N]` / `--dry-run`,
  continue-on-failure flow; then it builds the runner once and hands control to it.
- **Self-contained major versions** with everything anchored to each version root, so the
  worlds never cross-write — and results are *ported*, not re-run, when the structure
  changes.

You don't have to adopt all of it; the pieces are independent enough to borrow.

---

## Quick start (for either version)

Each version is self-contained. `cd` into `v001/` or `v002/` and use these:

```bash
./bootstrap.sh                      # provisions + continues in one run (sudo may prompt inside)

./jac313_test_cli --ctest           # the smallest of all testing
./jac313_test_cli --smoke           # mid level testing
./jac313_test_cli --run-everything  # just like it says, including ctests, smoke tests, benchtests, verify, verify-lite
```

`./bootstrap.sh` prepares the environment (sudo provisioning + non-sudo steps now continue in one run).

`./jac313_test_cli` is the main command for testing.

- Use `--ctest --smoke` to start.
- Use `--run-everything` for the full suite.

Always run from **inside** the version directory.

See that version's README for more (or the docs/ inside it).

---

## Repository layout

```
jac313/
├── README.md         # this file
├── LICENSE           # governs all versions
├── AI_files/         # cross-version coordination notes (handoffs between collaborators)
├── v001/             # complete C++23 world  (libraries, Setup, tools, tests, docs, results)
└── v002/             # complete C++26 world  (currently compiled as C++26 with limited new feature adoption)
```

## License

[LICENSE](LICENSE) governs all versions. This is reference/learning software, provided
"AS IS" without warranty; see the license for the governing terms.
