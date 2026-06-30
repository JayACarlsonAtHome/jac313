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

Beyond sensing, `Setup` is also the project's **installer**. Per-platform install commands live in
[`recipes.conf`](recipes.conf) (data, like `compilers.conf`): one line per `<component>.<family>`,
plus optional `check` (idempotency test) and `required` (readiness-gate) fields. `bootstrap.sh`
senses the host, writes a handoff (`.setup_handoff`), and runs the **committed, prebuilt
`jac313_setup`** binary, which reads the handoff, resolves each component's command from
`recipes.conf`, and executes them with real per-step error handling — plan preview, `[y/N]` confirm,
`--dry-run`, and a continue-on-failure summary (`plan_for_components` / `execute_plan` in the public
header). If that static binary can't run on the host, `bootstrap.sh` falls back to a generated,
**resilient** `Setup.sh`.

| Path | Purpose |
|------|---------|
| `recipes.conf` | Per-`(component, family)` install commands (+ `check` / `required`) |
| `setup_main.cpp` | `main()` for the provisioner (handoff parser + executor) |
| `jac313_setup` | **Committed, fully-static** provisioner binary — ships with the clone, runs before any compiler exists |
| `build_setup_exe.sh` | Rebuild + re-stamp `jac313_setup` (static, C++23) when the library changes |

`jac313_setup` is committed **static** so one binary runs across the fleet's distros/glibc
versions; rebuilding it needs `glibc-static`/`libstdc++-static` on the build host (the
`static_runtime` recipe provisions them on demand — on RHEL via the CodeReady Builder repo). Clone
hosts need none of this.
