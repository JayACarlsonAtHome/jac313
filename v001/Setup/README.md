# jac313::Setup

Standalone, data-driven, **activation-aware** toolchain sensing for the jac313 family
(`jac313::Setup::v001`).

Most build tooling assumes a compiler is a plain binary on `PATH`. That breaks on Red
Hat, where gcc-toolset compilers need an **activation launcher** that differs by OS
(RHEL 9 `scl`, RHEL 10 `gcc-toolset-NN-env`, Ubuntu/Mint direct). `Setup` models every
mechanism as a **command template** (`{CMD}` placeholder) in data, version-sniffs
`--version` (so it's terminology-independent), and captures the activation's
**environment delta** so consumers can apply it to their own build/run children.

Adding a new platform is a config line in [`compilers.conf`](compilers.conf), not code.

This package is **self-contained** — its own command-runner, no dependencies beyond
the C++23 standard library — so it builds first and is consumed by the rest of the
monorepo (e.g. `jac313_test_cli`), as a library or via the module.

```cpp
#include <jac313/Setup/v001.hpp>          // or: import jac313.setup.v001;
using namespace jac313::Setup::v001;

scrub_exported_shell_functions();
for (const ResolvedToolchain& tc : resolve_registry(source_dir)) {
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
| `compilers.conf` | Tracked default registry (per-platform activation templates) |

A machine-local `compilers.local.conf` at the repo root (gitignored) overrides the
tracked default.

## Build options

| Option | Default | Purpose |
|--------|---------|---------|
| `JAC313_SETUP_BUILD_MODULES` | OFF | Build the `jac313.setup.v001` module (Ninja only) |
| `JAC313_SETUP_BUILD_TESTS` | OFF | Build and register unit tests |

## Provisioning

Per-platform install commands live in [`recipes.conf`](recipes.conf) (data, like
`compilers.conf`): one line per `<component>.<family>`, plus optional `check` (idempotency test)
and `required` (readiness-gate) fields. `bootstrap.sh` finds what is missing and installs it
itself, item by item, from these recipes, after the one "Install everything?" answer and one sudo
password. One failing item never stops the others; anything still missing is named at the end.
`jac313_test_cli setup` reports readiness and names anything not installed yet.

A recipe with neither `for` nor `check` is used only on request. `upp_gui` (the U++ GUI build
libraries) is one: only the bootstrap's GUI step uses it (skipped with `--no-gui`), after its own
pkg-config check.

| Path | Purpose |
|------|---------|
| `recipes.conf` | Per-`(component, family)` install commands (+ `check` / `required`) |
