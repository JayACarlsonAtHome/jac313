# C++23 modules

How jac313 uses C++23 modules, and the module friction it surfaced.

---

## Modules: headers first, modules second

Headers remain the compatibility layer; modules are **additive**. Each package ships an
optional C++23 module (Qlite, header-only inside jText, has none) that re-exports the same public API:

| Module | `g++-14` | `g++-15` | `clang++-21` |
|--------|----------|----------|--------------|
| `jac313.jtext.v001` | yes | yes | yes\* |
| `jac313.store.v001` | yes | yes | yes\* |

\* Clang modules need `clang-scan-deps-21`; `jac313_test_cli` passes it automatically with
`--modules --clang`. Module builds require the **Ninja ≥ 1.11** generator and the CMake in
`pinned_tools` (Setup/recipes.jtext, 4.3.0) (`-DJAC313_BUILD_MODULES=ON`). Clang module builds
carry a workaround for a CMake/Clang gap: see [Open: CMake's Clang BMI-only step](#open-cmakes-clang-bmi-only-step-should-be-fixed-upstream).

All three module front-ends export their API **correctly qualified** under
`jac313::<pkg>::v001` via `export namespace` — see the first pitfall below.

---

## Pitfalls we hit

- **`export using` injects into the global namespace.** `export using
  jac313::X::v001::Foo;` at module scope silently put `Foo` in the *global* namespace, not
  `jac313::X::v001::Foo` — diverging from the header spelling, and caught only once a test
  referenced a symbol by qualified name. Fixed everywhere with `export namespace`.
- **Exporting internal-linkage entities is an error.** `constexpr`/anon-namespace helpers in a
  `.cpp` can't sit inside `export namespace` — keep declarations exported and the `.cpp`
  definitions in a separate non-exported `namespace` block.

---

## Open: CMake's Clang BMI-only step (should be fixed upstream)

**Status:** worked around in `CMakeLists.txt` (2026-11-11); the root cause is in CMake and
should be fixed there. Remove the workaround once a fixed CMake is in `pinned_tools`.

**What CMake does.** CMake 4 builds a BMI for each target that imports a module (a "synthetic
target"). It reuses the target's normal compile line, `-c` included, and adds
`CMAKE_CXX_MODULE_BMI_ONLY_FLAG`. In CMake 4.3 (`share/cmake-4.3/Modules/Compiler/`):

| Compiler | BMI-only flag | Works with CMake's `-c`? |
|----------|---------------|--------------------------|
| GCC   | `-fmodule-only` | yes: GCC's flag means "write the BMI, no object" |
| Clang | `--precompile`  | no: it stops the driver before the compile, so `-c` is unused |

With Clang the BMI step therefore always prints `argument unused during compilation: '-c'`,
from both `clang++` and `clang-scan-deps`. That warning comes from CMake's own command line, not
from anything the project wrote, so a warnings-are-errors build (`-Werror`, and our strict
`clang-scan-deps-strict` wrapper) fails on every Clang module build. `clang-scan-deps` also
ignores `-Wno-unused-command-line-argument`, so the usual way to silence it does not work.

**What should change.** One of:
- CMake leaves `-c` off the Clang BMI-only command (or uses a rule for that step that has no
  `-c`); or
- Clang gains a driver flag like GCC's `-fmodule-only` that writes only the BMI and accepts `-c`.

Either one lets this project drop the workaround below and keep every warning an error.

**Our workarounds, and how the first one broke.**
1. *Until 2026-11-11 (Clang 20):* `-Xclang -emit-module-interface`. It asked the front end for
   the BMI and kept a normal `-c` compile, so nothing warned. It is a front-end option, not a
   driver flag. On **Clang 21**, `-c` on a module unit runs two front-end jobs (the source
   to `.pcm`, then the `.pcm` to `.o`). `-Xclang` passes the flag to both, and the second one
   fails: `fatal error: module interface compilation does not support input file format of
   file '….bmi': 'Precompiled'`.
2. *Now (Clang 20 and 21):* `--precompile;-Qunused-arguments`. This is CMake's own
   `--precompile` plus the driver switch that silences unused-argument warnings, which
   `clang-scan-deps` respects too.

**What the workaround costs.** On the BMI-only steps alone, an argument that really is unused
would not be reported. Every other warning there is still an error. The same `.cppm` is also
compiled normally (with `-c`, without `-Qunused-arguments`), so a stray flag still fails that
compile. Recheck this on every new Clang and every new pinned CMake. The test: build with
`--modules --clang` and the workaround removed.

---

## `import std;`: tried, measured, removed (2026-10-22)

All three libraries once had an opt-in `import std;` build (`-DJAC313_*_IMPORT_STD=ON`). It
converted cleanly and stayed green, but measured **break-even** at this codebase's size
(full-build delta vs textual modules: Qlite only +162 ms, Qlite + jText −121 ms, all three
+340 ms — all inside the noise): the std-module BMI costs seconds up front and only pays off
with far more translation units than this repo has. It also needed **exactly CMake 4.3.3**,
because CMake arms the feature with a version-specific experimental UUID. It was removed so
that a plain distro CMake builds everything, and a new machine needs no special CMake.
