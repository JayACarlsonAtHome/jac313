# jac313::Qlite

Versioned umbrella namespace for the jac313 SQLite wrapper family.

`jac313::Qlite::v002` is an **in-tree, header-only** SQLite3 wrapper (migrated from
the legacy `jacQlite` / `jac::qlite` repo — no sibling checkout needed). It depends
only on system SQLite3. Public symbols: `Sqlite` (with `Statement`, `Transaction`,
`prepare`/`prepare_cached`, `get_one_*` helpers), `SqliteError`, `bind_value`, `column`,
`version`.

`version()` returns the package's `"major.minor"` version (`"v002.005"` now): major is the
`v002` API line, minor bumps on each small code change (a real break moves it to `"v003.001"`).
The `version-check` pre-push gate enforces the bump (see [docs/Setup.md](../docs/Setup.md)).

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

Module builds require the Ninja generator. The module exports the same public API,
qualified under `jac313::Qlite::v002`, as the textual header.

### `import std;` (pilot, g++-15)

`-DJAC313_QLITE_IMPORT_STD=ON` builds the module **module-native** against
`import std;` instead of textually including the standard library:

```bash
cmake -G Ninja -S . -B build-importstd \
  -DJAC313_QLITE_BUILD_MODULES=ON -DJAC313_QLITE_IMPORT_STD=ON \
  -DCMAKE_CXX_COMPILER=g++-15
cmake --build build-importstd
```

Notes:
- Uses CMake's **native** std-module support (`CXX_MODULE_STD`), which is
  **experimental and gated behind a CMake-version-specific UUID**. `≥ 3.30` is
  necessary but **not sufficient**: the UUID pinned in `CMakeLists.txt` is **4.3.3's**,
  so you need *that exact* CMake — a different version (even a newer one) carries a
  different UUID and the configure fails with *"Experimental `import std` support not
  enabled … `__CMAKE::CXX23` … not provided by the toolchain"*. Confirmed empirically:
  CMake **3.31.8 (≥ 3.30) fails**; **4.3.3 builds and times at break-even** (see
  [docs/Results.md → CMake 3.31.8 vs 4.3.3](../docs/Results.md#cmake-3318-vs-433)).
  Install 4.3.3 user-locally (no root) per the root README / [docs/Setup.md](../docs/Setup.md).
- The experimental feature is armed by `CMAKE_EXPERIMENTAL_CXX_IMPORT_STD` (set
  before `project()` in both the root and standalone CMakeLists); update the pinned
  UUID if you standardize on a different CMake version.
- Validated on **GNU (g++-15) and Clang (clang-21 + libstdc++)** (guarded in CMake).
  Leave OFF if your toolchain lacks native `import std;` support.
- Public API and behaviour are identical to the default build; this only changes
  how the standard library is consumed internally.

## CMake target

- `jac313::Qlite` — INTERFACE library (headers + link to `SQLite::SQLite3`).
- `jac313::Qlite_module` — C++23 module object library (when `JAC313_QLITE_BUILD_MODULES=ON`).
