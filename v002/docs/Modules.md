# C++23 modules

How jac313 uses C++23 modules, and the module friction it surfaced.

---

## Modules: headers first, modules second

Headers remain the compatibility layer; modules are **additive**. Each package ships an
optional C++23 module that re-exports the same public API:

| Module | `g++-14` | `g++-15` | `clang++-20` |
|--------|----------|----------|--------------|
| `jac313.qlite.v002` | yes | yes | yes\* |
| `jac313.jtext.v002` | yes | yes | yes\* |
| `jac313.store.v002` | yes | yes | yes\* |

\* Clang modules need `clang-scan-deps-20`; `jac313_test_cli` passes it automatically with
`--modules --clang`. Module builds require the **Ninja ≥ 1.11** generator and **CMake ≥ 3.28**
(`-DJAC313_BUILD_MODULES=ON`); the distro's CMake package is enough.

All three module front-ends export their API **correctly qualified** under
`jac313::<pkg>::v002` via `export namespace` — see the first pitfall below.

---

## Pitfalls we hit

- **`export using` injects into the global namespace.** `export using
  jac313::X::v002::Foo;` at module scope silently put `Foo` in the *global* namespace, not
  `jac313::X::v002::Foo` — diverging from the header spelling, and caught only once a test
  referenced a symbol by qualified name. Fixed everywhere with `export namespace`.
- **Exporting internal-linkage entities is an error.** `constexpr`/anon-namespace helpers in a
  `.cpp` can't sit inside `export namespace` — keep declarations exported and the `.cpp`
  definitions in a separate non-exported `namespace` block.

---

## `import std;`: tried, measured, removed (2026-10-22)

All three libraries once had an opt-in `import std;` build (`-DJAC313_*_IMPORT_STD=ON`). It
converted cleanly and stayed green, but measured **break-even** at this codebase's size
(full-build delta vs textual modules: Qlite only +162 ms, Qlite + jText −121 ms, all three
+340 ms — all inside the noise): the std-module BMI costs seconds up front and only pays off
with far more translation units than this repo has. It also needed **exactly CMake 4.3.3**,
because CMake arms the feature with a version-specific experimental UUID. It was removed so
that a plain distro CMake builds everything, and a new machine needs no special CMake.
