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
| **Qlite** | `jac313::Qlite::v002` | a thin, safe **SQLite** wrapper (variadic bind/exec/step/get, RAII transactions, stmt cache, `column<int>`, `try_*` when `<expected>` is available) |

They compose — Store uses Qlite + jText under the hood — but you can take just the one you need.

## What could I use this for?

**Store** — timestamped `{floats/ints + category + event-name}` records, persisted (binary / jText / SQL / in-mem):
- **Robot arm** — joint angles + gripper state · cat `manipulation` · *"Moving arm to weld seam 3"*
- **AGV / fleet, 2D map** — x, y, heading, load_id · cat `logistics` · *"Forklift picking up load at dock 7"*, *"Truck arrived, gate 4"*
- **Drone / vehicle telemetry** — lat/lon/alt, or rpm/speed/temp · *"Waypoint 5 reached"*, *"Hard brake −0.8 g"*
- **Manufacturing / lab** — sensor values + counts · *"Station 4 cycle complete"*, *"Sample 12 measured"*

**jText** — structured text you write fast *and* parse + validate: audit/app logging that's greppable by a
human yet machine-parseable; small config/DSL formats with validation; log normalization into one record
shape; a human-readable export of a Store stream.

**Qlite** — a thin, safe SQLite wrapper: local app state/settings; a results/analytics DB (it's exactly what
jac313 does to itself — `results.db`, reviewed in a tool like DataGrip); an SQL query layer over recorded
Store events. Additive helpers include `prepare_cached`, `Transaction`, column-by-name, blob/`string_view`
binds, scalar `get_one_*` query helpers (debug logging via `fprintf`), and non-throwing `try_exec`/`try_step`
when the toolchain provides `<expected>`.

**They compose.** A forklift logs `{x, y, load_id}` + events to **Store** → persisted via **Qlite** (SQL) so
you can ask "how long did dock 7 take this week?" → exported via **jText** as a human shift report. One
capture path, three consumption shapes.

**Your data isn't locked in.** Store's `sql` backend writes a standard **SQLite 3** database (`persist.db`) —
open it as-is in **DataGrip, DBeaver, or the `sqlite3` CLI**, no export step — alongside a human-readable
`.sql` schema sidecar. (jac313's own `results.db` is the same format; it's literally how these report pages
get reviewed.)

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
6. **A GUI editor for jText files** — a visual open / edit / *validate* tool (against the section +
   template schema), so jText files aren't only hand-edited text. *(Parked — not near-term.)*

---

*Framework-side ideas (new test axes, sanitizer/optimization matrices) are a different audience —
they live in [FutureTestAxes.md](FutureTestAxes.md).*
