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
`pinned_tools` (Setup/recipes.jtext, 4.3.3) (`-DJAC313_BUILD_MODULES=ON`). CMake 4.3.3 or newer
is required: see [Resolved: CMake's Clang BMI-only step](#resolved-cmakes-clang-bmi-only-step-fixed-in-cmake-433).

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

## Resolved: CMake's Clang BMI-only step (fixed in CMake 4.3.3)

**Status:** fixed upstream in CMake 4.3.3, which is the pinned CMake (`pinned_tools` in
`Setup/recipes.jtext`) since 2026-11-11. The workaround is gone, and 4.3.3 is required: `CMakeLists.txt` (v001 and v002) refuses to
configure with any older CMake.

**What CMake 4.3.0 did.** CMake 4 builds a BMI for each target that imports a module (a
"synthetic target", one per module-providing target that another target uses). In 4.3.0 that step
reused the target's normal compile line, `-c` included, and added
`CMAKE_CXX_MODULE_BMI_ONLY_FLAG` (`share/cmake-4.3/Modules/Compiler/`):

| Compiler | BMI-only flag | Works with CMake's `-c`? |
|----------|---------------|--------------------------|
| GCC   | `-fmodule-only` | yes: GCC's flag means "write the BMI, no object" |
| Clang | `--precompile`  | no: it stops the driver before the compile, so `-c` is unused |

So every Clang module build printed `argument unused during compilation: '-c'` from both `clang++`
and `clang-scan-deps`. That came from CMake's own command line, and with `-Werror` and our strict
`clang-scan-deps-strict` wrapper it failed the build. `clang-scan-deps` ignores
`-Wno-unused-command-line-argument`. A ten-line project with one module and one importer showed the
same thing, so it was not our CMake code.

**What 4.3.3 changed.** `Clang-CXX.cmake` no longer sets a BMI-only flag. It has its own BMI rule,
`CMAKE_CXX_COMPILE_BMI` = `<CMAKE_CXX_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -o <OBJECT> --precompile
<SOURCE>`, with no `-c`. The 4.3.3 release notes do not mention it. On the same ten-line project 4.3.3
made no synthetic targets and printed nothing. On Linux Mint 22.3 the full v001 tree (Release,
hardened, modules, `-Werror`, strict scan-deps) built with no warning and passed ctest with both
`clang++-21` and `g++-15`.

**Our workarounds before 4.3.3, for the record.**
1. *Until 2026-11-11 (Clang 20):* `-Xclang -emit-module-interface` as the BMI-only flag: the front
   end wrote the BMI and the driver kept a normal `-c` compile, so nothing warned. On **Clang 21**,
   `-c` on a module unit runs two front-end jobs (source to `.pcm`, then `.pcm` to `.o`). `-Xclang`
   passed the flag to both, and the second failed: `fatal error: module interface compilation does
   not support input file format of file '….bmi': 'Precompiled'`.
2. *Briefly, on 4.3.0 with Clang 21:* `--precompile;-Qunused-arguments`. It worked, but it silenced
   the unused-argument warning on the BMI-only steps. Dropped with the move to 4.3.3.

**If it comes back.** A new CMake or Clang that brings back an unused-argument warning on the BMI
step fails the Clang module build at once (every warning is an error). Check the `Clang-CXX.cmake`
of that CMake first.

---

## `import std;`: tried, measured, removed (2026-10-22)

All three libraries once had an opt-in `import std;` build (`-DJAC313_*_IMPORT_STD=ON`). It
converted cleanly and stayed green, but measured **break-even** at this codebase's size
(full-build delta vs textual modules: Qlite only +162 ms, Qlite + jText −121 ms, all three
+340 ms — all inside the noise): the std-module BMI costs seconds up front and only pays off
with far more translation units than this repo has. It also needed **exactly CMake 4.3.3**,
because CMake arms the feature with a version-specific experimental UUID. It was removed so
that a plain distro CMake builds everything, and a new machine needs no special CMake.
