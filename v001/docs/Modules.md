# C++23 modules and `import std;`

How jac313 uses C++23 modules, the `import std;` conversion of all three umbrella
libraries, and the toolchain friction it surfaced — quirks, blessings, and curses.

> **An open invitation.** This is a small but real codebase exercising *experimental*
> toolchain features. If you work on **CMake**, **GCC/libstdc++**, or **Clang/libc++**:
> you are welcome to use jac313 as a reproducible example to harden your `import std;` /
> C++ modules support. The friction documented here *is* the contribution.

---

## Modules: headers first, modules second

Headers remain the compatibility layer; modules are **additive**. Each package ships an
optional C++23 module that re-exports the same public API:

| Module | `g++-14` | `g++-15` | `clang++-20` |
|--------|----------|----------|--------------|
| `jac313.qlite.v001` | yes | yes | yes\* |
| `jac313.jtext.v001` | yes | yes | yes\* |
| `jac313.store.v001` | yes | yes | yes\* |

\* Clang modules need `clang-scan-deps-20`; `jac313_test_cli` passes it automatically with
`--modules --clang`. Module builds require the **Ninja ≥ 1.11** generator
(`-DJAC313_BUILD_MODULES=ON`).

All three module front-ends export their API **correctly qualified** under
`jac313::<pkg>::v001` via `export namespace` — see the *curse* below about `export using`.

---

## `import std;` — from "the right thing to do" to "an example for the toolchains"

The `import std;` work started from an assumption: parse the standard library once, not in
every translation unit — that *sounds* like the obviously-modern thing, and it is where C++ is
heading. So we did it across all three libraries, each a different shape:

| Library | Shape | Conversion approach |
|---------|-------|--------------------|
| **Qlite** | header-only | single shared impl body (`Sqlite.body.hpp`) used by both the textual header and the module |
| **jText** | compiled lib | interface unit + implementation unit over shared fragments (`jText.api.inc` / `jText.impl.inc`) |
| **Store** | template + compiled sinks, POSIX-heavy, deps on Qlite **and** jText | one interface unit over ~24 headers + 4 `.cpp`, each made std-free by guarding its `#include <…>` + `namespace` behind `JAC313_STORE_IMPORT_STD`; POSIX/C-compat/Qlite/jText stay textual in the global module fragment |

Each is opt-in and **gcc-only** (`-DJAC313_QLITE_IMPORT_STD=ON` / `_JTEXT_` / `_STORE_`), via
CMake's native std-module support. That support is gated behind a **CMake-version-specific
UUID**, so it needs the **exact pinned CMake (4.3.3)** — `≥ 3.30` is the floor where the
feature exists but is **not sufficient**: any other version (older or newer) carries a
different UUID and fails to configure (see the [UUID note](#the-curses-concrete-toolchain-friction) below and
[Setup.md](Setup.md#newer-cmake-only-for-import-std); empirically confirmed in
[Results.md → CMake 3.31.8 vs 4.3.3](Results.md#cmake-3318-vs-433)). Clang keeps the textual
modules.

### The blessing: it works, and it's measured

All three convert cleanly and stay green (ctest 31/31 import-std + textual, smoke 115/115).

### The honest result: break-even

Then we measured it, and the assumption didn't hold — at this codebase's size, `import std`
is **break-even no matter how much we convert**:

| Converted | Full-build delta vs textual |
|-----------|-----------------------------|
| Qlite only | +162 ms |
| Qlite + jText | −121 ms |
| all three | +340 ms |

All inside the noise (~0%). The std-library module BMI costs seconds to build up front — it
dominates the *modules-only* number (which grew from ~3.7 s textual to ~9.9 s once all three
modules pull `std`) — and that cost only amortizes once the translation-unit count is far
larger than a small umbrella repo has. The numbers live in `results.db` (the build benchmark
records straight / modules-only / with-modules / with-modules-import-std per compiler), so this
is a recorded result, not a vibe.

So if it isn't faster here, what is it *for*? **A real example for the build tools and the
standard-library implementers to chew on** — three differently-shaped libraries exercising
features that are still experimental, with the friction documented and reproducible.

---

## The curses (concrete toolchain friction)

Every one of these is a real thing we hit, and a real data point for implementers:

- **`export using` injects into the global namespace.** `export using
  jac313::X::v001::Foo;` at module scope silently put `Foo` in the *global* namespace, not
  `jac313::X::v001::Foo` — diverging from the header spelling, and caught only once a test
  referenced a symbol by qualified name. Fixed everywhere with `export namespace`.
- **`import std;` exports `std::` but not the global C names.** Real code leans on `int64_t`,
  `stderr`, `::toupper`, `::memcpy`, `::strcmp`, bare `size_t`/`uint*_t`. `import std;` gives
  you `std::int64_t` but not `::int64_t`. Handle it by qualifying to `std::`, or by pulling the
  C-compat header into the **global module fragment** textually (store does the latter —
  `<cstring>`/`<cctype>`/… in the GMF declare the globals, so the header bodies need no
  requalification).
- **A compiled library can't drop `import std` into a wrapper.** Global-module vs module
  linkage means re-declaring the API in the module purview won't link against the externally-
  compiled definitions — the library has to actually become *module-native* (jText, store).
- **Exporting internal-linkage entities is an error.** `constexpr`/anon-namespace helpers in a
  `.cpp` can't sit inside `export namespace` — store keeps declarations exported and the `.cpp`
  definitions in a separate non-exported `namespace` block.
- **The CMake gate UUID changes every release.** `import std;` is gated behind
  `CMAKE_EXPERIMENTAL_CXX_IMPORT_STD`, whose value is version-specific. It's pinned in the
  CMakeLists to CMake **4.3.3**'s value (`451f2fe2-a8a2-47c3-bc32-94786d8fc91b`); a different
  CMake needs the matching UUID from its `Help/dev/experimental.rst`.
- **Mixing textual std and `import std` in one TU.** gcc-15 permits textual C/std headers in
  the global module fragment alongside `import std;` in the purview — which is what makes the
  GMF-for-globals trick work. (It's how the C names get declared without re-parsing all of std.)

---

## The clang column is a feature request with a number attached

Clang here has no usable `std` module (it uses libstdc++, which ships the module for GCC, not a
libc++ one), so the import-std build is gcc-only **by design**. The empty clang column in the
benchmark is a standing request: the day clang + libc++ ships a usable `std` module, that
column fills in with **no code change** — just widen the gcc gate. Keeping the whole thing
opt-in and gcc-gated is the point: a sandbox that hands the toolchains a small, honest,
reproducible example, kept truthful by the numbers rather than by enthusiasm.
