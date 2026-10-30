# jac313::Setup

Standalone, data-driven, **activation-aware** toolchain sensing for the jac313 family
(`jac313::Setup::v001`).

Most build tooling assumes a compiler is a plain binary on `PATH`. That breaks on Red
Hat, where gcc-toolset compilers need an **activation launcher** that differs by OS
(RHEL 9 `scl`, RHEL 10 `gcc-toolset-NN-env`, Ubuntu/Mint direct). `Setup` models every
mechanism as a **command template** (`{CMD}` placeholder) in data, version-sniffs
`--version` (so it's terminology-independent), and captures the activation's
**environment delta** so consumers can apply it to their own build/run children.

Adding a new platform is a few rows in [`compilers.jtext`](compilers.jtext), not code.

The settings are jText tables (full profile, an OS column where they depend on the OS):

| File | Tables | Read by |
|------|--------|---------|
| [`compilers.jtext`](compilers.jtext) | `toolchains` (label, kind, version, min_version, build_dir, flag, cc_names, scan_deps); `toolchain_os` (os, toolchain, required, activation_seq, activation) | `jac313_test_cli`, `bsir.sh` |
| [`recipes.jtext`](recipes.jtext) | `components` (component, desc, for, required, check); `recipe_os` (os, component, command) | `jac313_test_cli`, `bsir.sh` |
| `compilers.pin.jtext` | `pins` (label, cpu_model, cores, ram_gb, os_pretty, gcc_label, clang_label) | `jac313_test_cli pin` (reads and writes it) |
| [`contracts.jtext`](contracts.jtext) | `contract_modes` (os, toolchain, enforce, observe, quick_enforce, ignore) | `jac313_test_cli`, Jac313Runner |

`Setup` itself reads none of them: it links nothing but the standard library. `jac313_test_cli`
reads them with jText's parser and validator (`tools/jac313_test_cli/settings.cpp`) and hands the
toolchains and recipes to the functions here. `bsir.sh` runs before anything is built, so it reads
`compilers.jtext` and `recipes.jtext` with [`jtext_table.sh`](jtext_table.sh) (`jtext_rows`,
`jtext_fields`, `jtext_select`: POSIX sh + awk). That is why those two files use the formatter
`^~` on their data rows (the commands hold `|`, `'`, `&&`, `$(...)`, `;` and `{}`), one row per
line, with no comments on a data row. `jtext_table_test.sh` checks the helper (ctest).

This package is **self-contained** — its own command-runner, no dependencies beyond
the C++23 standard library — so it builds first and is consumed by the rest of the
monorepo (e.g. `jac313_test_cli`), as a library or via the module.

```cpp
#include <jac313/Setup/v001.hpp>          // or: import jac313.setup.v001;
using namespace jac313::Setup::v001;

scrub_exported_shell_functions();
// toolchains: the rows of Setup/compilers.jtext for this OS (jac313_test_cli: load_toolchains)
for (const ResolvedToolchain& tc : resolve_registry(toolchains)) {
    if (tc.available)
        // tc.cc_path  -> CMAKE_CXX_COMPILER
        // tc.env_delta -> apply_env_delta(tc.env_delta) before build + run
        ;
}
```

## Layout

| Path | Purpose |
|------|---------|
| `include/jac313/Setup/v001.hpp` | Public API (toolchain model, sensing, env helpers, runner) |
| `src/v001.cpp` | Implementation incl. the self-contained `run_capture` |
| `modules/jac313.setup.v001.cppm` | C++23 module re-export |
| `compilers.jtext` | The registry: toolchains and their activations per OS family |
| `jtext_table.sh` | Reads a jText table from sh + awk (sourced by `bsir.sh`) |

## Build options

| Option | Default | Purpose |
|--------|---------|---------|
| `JAC313_SETUP_BUILD_MODULES` | OFF | Build the `jac313.setup.v001` module (Ninja only) |
| `JAC313_SETUP_BUILD_TESTS` | OFF | Build and register unit tests |

## Provisioning

Per-platform install commands live in [`recipes.jtext`](recipes.jtext) (data, like
`compilers.jtext`): one `components` row per prerequisite, with its `check` (idempotency test,
'' = none) and `required` (readiness gate) columns, and one `recipe_os` row per component and OS
family with its install command. `bsir.sh` finds what is missing and installs it
itself, item by item, from these recipes, after the one "Install everything?" answer and one sudo
password. One failing item never stops the others; anything still missing is named at the end.
`jac313_test_cli setup` reports readiness and names anything not installed yet.

A recipe with neither `for` nor `check` is used only on request. `upp_gui` (the U++ GUI build
libraries) is one: only the bootstrap's GUI step uses it (skipped with `--no-gui`), after its own
pkg-config check.

| Path | Purpose |
|------|---------|
| `recipes.jtext` | Per-`(component, family)` install commands (+ `check` / `required`) |
