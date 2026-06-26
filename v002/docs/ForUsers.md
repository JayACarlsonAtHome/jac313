//File:    docs/ForUsers.md
//Purpose: For people who want to USE the libraries (jText / Store / Qlite) — not run the test harness.

# For users — using the libraries

jac313 is really **two things**: a family of small, usable C++ libraries, and a test/benchmark
harness that keeps them honest. **Most people want the first.** If your goal is to log text, persist
a time-series, or wrap SQLite — you do **not** need the compiler matrix, the build-time numbers, the
valgrind gates, or anything in [Setup.md](Setup.md)'s testing section. This page is for you.

## The libraries

| Package | Namespace | What it's for |
|---|---|---|
| **jText** | `jac313::jText::v002` | structured text / log records — fast formatting, parse + validate |
| **Store** | `jac313::Store::v002` | a time-series logging **store** with pluggable persistence (binary / jText / SQL / in-mem) |
| **Qlite** | `jac313::Qlite::v002` | a thin, safe **SQLite** wrapper (variadic bind/exec/step/get) |

They compose — Store uses Qlite + jText under the hood — but you can take just the one you need.

## Using one in your build

Everything is **in-tree** (no sibling checkouts) and exposed as a namespaced CMake target. The only
external dependency is **system SQLite3**, and only if you use Qlite (or Store's SQL persistence).

```cmake
add_subdirectory(jac313/v002/Qlite)         # or jText / Store
target_link_libraries(your_app PRIVATE jac313::Qlite)
```
```cpp
#include <jac313/Qlite/v002.hpp>             // textual front-end (always works)
// ...or `import jac313.qlite.v002;`         // the C++23 module front-end (gcc-15 / clang-21)
auto db = jac313::Qlite::v002::Sqlite("app.db");
```

Each package ships **standalone examples** you can copy from — see `Store/examples/`,
`jText/examples/`, `Qlite/examples/`. Header front-end works everywhere; the module + `import std`
front-ends are validated on **GNU (g++-15) and Clang (clang-21)** (see [Modules.md](Modules.md)).

## What a *user* might want next (consumer-facing ideas)

Ideas that serve consumers, not the harness — captured, not committed:

1. **Real install / `find_package(jac313)`** — export the targets so a consumer can depend on a
   built/installed jac313 instead of vendoring the source tree.
2. **Single-package builds** — make "I only want jText" a first-class, dependency-minimal build.
3. **A documented, stable public API surface** — the `v002` line is the API contract; spell out what's
   public vs internal, and the versioning/ABI promise, so users can upgrade with confidence.
4. **More drop-in examples + a 5-minute quickstart per library** — the fastest path from `#include` to
   working code, without reading the test docs.
5. **An amalgamated / single-header option** for quick drop-in evaluation.

---

*Framework-side ideas (new test axes, sanitizer/optimization matrices) are a different audience —
they live in [FutureTestAxes.md](FutureTestAxes.md).*
