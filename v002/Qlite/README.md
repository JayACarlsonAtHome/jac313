# jac313::Qlite

Versioned umbrella namespace for the jac313 SQLite wrapper family.

`jac313::Qlite::v002` is an **in-tree, header-only** SQLite3 wrapper (migrated from
the legacy `jacQlite` / `jac::qlite` repo — no sibling checkout needed). It depends
only on system SQLite3. Public symbols: `Sqlite` (with `Statement`, `Transaction`,
`prepare`/`prepare_cached`, `get_one_*` helpers), `SqliteError`, `bind_value`, `column`,
`version`.

`version()` returns the package's `"major.minor"` version (`"v002.007"` now): major is the
`v002` API line, minor bumps on each small code change (a real break moves it to `"v003.001"`).
The `version-check` pre-push gate enforces the bump (see the version's top-level README.md).

Part of the [jac313](../) monorepo.

## Quick start

```cpp
#include <jac313/Qlite/v002.hpp>

jac313::Qlite::v002::Sqlite db("mydata.db");
db.exec("CREATE TABLE IF NOT EXISTS events (id INTEGER, payload TEXT)");
db.exec("INSERT INTO events VALUES (?, ?)", 1, std::string("hello"));
```

## Build

Requires SQLite3 development headers (`find_package(SQLite3)`).

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
./build/examples/jac313_qlite_v002_example
```

## Implementation layout (single source of truth)

The wrapper has one implementation, consumed two ways:

| Path | Purpose |
|------|---------|
| `include/jac313/Qlite/v002/Sqlite.body.hpp` | The implementation **body** — an include-free fragment (no guard, includes nothing). Must be `#include`d *inside* the `jac313::Qlite::v002` namespace. |
| `include/jac313/Qlite/v002/Sqlite.hpp` | Textual header for non-module consumers: textual `std` + `<sqlite3.h>` includes, then the body inside `namespace { … }`. |
| `include/jac313/Qlite/v002.hpp` | Public entry point (`#include`s `Sqlite.hpp`). |
| `modules/jac313.qlite.v002.cppm` | C++23 module interface: `<sqlite3.h>` in the global module fragment, the body inside `export namespace { … }`. See "Modules" below. |
| `examples/v002_example.cpp` | Minimal usage demo. |
| `tests/v002_test.cpp` | Header unit test. |
| `tests/module/v002_module_test.cpp` | Functional module test (round-trips an in-memory DB through the imported module). |

Both front-ends paste the *same* body; they differ only in how `std` and
`<sqlite3.h>` names are made visible to it.

## Modules (C++23)

```bash
cmake -G Ninja -S . -B build-mod -DJAC313_QLITE_BUILD_MODULES=ON
cmake --build build-mod
import jac313.qlite.v002;   // exposes jac313::Qlite::v002::{Sqlite, …}
```

Module builds require the Ninja generator and CMake >= 3.28. The module exports the same public API,
qualified under `jac313::Qlite::v002`, as the textual header.

## CMake target

- `jac313::Qlite` — INTERFACE library (headers + link to `SQLite::SQLite3`).
- `jac313::Qlite_module` — C++23 module object library (when `JAC313_QLITE_BUILD_MODULES=ON`).
