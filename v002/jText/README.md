# jac313::jText

Versioned umbrella namespace for the jac313 structured-text library family.

`jac313::jText::v002` is an **in-tree** structured-text writer/reader (migrated from
the legacy `jText` repo — no sibling checkout needed). The compiled core (`jtext_core`)
lives here and the public API is declared directly in namespace `jac313::jText::v002`
(`jText.h`). Public symbols: `JTextWriter`, `JTextEntry`, `JTextSection`, `JTextFile`,
`CaseMode`, `JTextProfile`, `version`, and the `write_*` / `jtext_*` free helpers.

`version()` returns the package's `"major.minor"` version (`"v002.005"` now): major is the
`v002` API line, minor bumps on each small code change (a real break moves it to `"v003.001"`).
The `version-check` pre-push gate enforces the bump (see [docs/Setup.md](../docs/Setup.md)).

Part of the [jac313](../) monorepo.

## Quick start

```cpp
#include <jac313/jText/v002.hpp>

jac313::jText::v002::JTextWriter writer("out.jtext");
writer.set_purpose("my app");
writer.write_header();
writer.begin_section("Events");
writer.append_row(1, {std::string("hello")});
writer.finalize();
```

## Build

Self-contained — no sibling checkout. Requires a C++23 compiler with `<print>` /
`<expected>` support (GCC 14+ or Clang 20+).

```bash
cmake -S . -B build -DCMAKE_CXX_COMPILER=g++-15
cmake --build build
ctest --test-dir build
./build/examples/jac313_jtext_v002_example
```

## Layout

| Path | Purpose |
|------|---------|
| `jText.h` / `jText.cpp` | Public API (declarations / definitions) in `jac313::jText::v002` |
| `jText.api.inc` / `jText.impl.inc` | Include-free decl/def fragments — single source shared by the textual header and the `import std` module |
| `src/parser/`, `src/validator/` | Internal parser + validator (namespace `jtext`), compiled into `jtext_core` |
| `include/jac313/jText/v002.hpp` | Public entry point (`#include <jText.h>`) |
| `examples/v002_example.cpp` | Minimal streaming writer demo |
| `tests/v002_test.cpp` | Smoke test |

## CMake target

- `jac313::jText` — INTERFACE library (headers + link to `jtext_core`)

## Options

| Option | Default | Effect |
|--------|---------|--------|
| `JAC313_JTEXT_BUILD_EXAMPLES` | ON | Build example programs |
| `JAC313_JTEXT_BUILD_TESTS` | ON | Build and register tests |
| `JAC313_JTEXT_BUILD_MODULES` | OFF | Build the `jac313.jtext.v002` C++23 module (Ninja) |
| `JAC313_JTEXT_IMPORT_STD` | OFF | Build the module **module-native** against `import std;` (g++-15, **CMake 4.3.3** — the version-pinned gate UUID, not merely ≥ 3.30) |

## Modules / `import std` (C++23)

`-DJAC313_JTEXT_BUILD_MODULES=ON` builds the textual-wrapper `jac313.jtext.v002` module
(core / reader / writer partitions; Ninja). `-DJAC313_JTEXT_IMPORT_STD=ON` (g++-15,
**CMake 4.3.3** — see below) instead builds it **module-native** against `import std;` — an
interface unit + implementation unit over the shared `jText.api.inc` / `jText.impl.inc`
fragments, needing no `jtext_core` link (the public API is self-contained; the
parser/validator stay textual). Clang keeps the textual partitions (leave OFF). The compiled
`jtext_core` and textual `jText.h` are unchanged for non-module consumers. See the root README
*`import std`* sections.

> **CMake version is exact, not a floor.** The native std-module gate is an experimental,
> **version-specific UUID** pinned to **4.3.3** in `CMakeLists.txt`. `≥ 3.30` is necessary but
> not sufficient — CMake 3.31.8 (≥ 3.30) still fails to configure; only the pinned 4.3.3
> activates it. Empirically confirmed on RHEL 9.8 (break-even vs textual modules); see
> [docs/Results.md → CMake 3.31.8 vs 4.3.3](../docs/Results.md#cmake-3318-vs-433).
