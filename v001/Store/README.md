# jac313::Store

The jac313 **logging feature** — a high-throughput time-series event store. A consumer logs
events carrying **integers and floats in the exact count they need** (fixed at compile time),
through a lock-free hot path out to pluggable persistence. It **uses [jText](../jText/README.md)**
for human-readable logs and **[Qlite](../Qlite/README.md)** for SQL persistence.

Implementation lives **in this package** under `jac313::Store::v001` (migrated from the legacy `jac::ts_store::inline_v001` repo — no sibling checkout needed).

## Store by feature

Store is the largest part of jac313 — documented one feature at a time (each doc ends with honest
strengths + weaknesses; a good target for AI-assisted exploration against your workload):

- [**Use cases**](../docs/store/use-cases.md) — when & for what: metric count = event dimensionality (robot 3-D / fleet 2-D / pure logging 0-D), the Event as intent, and a gallery of domains
- [**Logging**](../docs/store/logging.md) — the core: event/record model, exact int/float field counts, the lock-free hot path
- [**Categories**](../docs/store/categories.md) — `bounded_string`, UTF-8 codepoint sizing of category + payload
- [**Flags**](../docs/store/flags.md) — the complete `StoreFlags` catalog: every bit + meaning + honest wired/advisory/placeholder status
- [**Routing**](../docs/store/bitmaps.md) — how flags + `PersistMode` route events (keeper/database masks)
- [**Persistence**](../docs/store/persistence.md) — the sinks (binary, jText, SQL, flag-routing) + double-buffered drain
- [**Reporting**](../docs/store/reporting.md) — reading the logs back: human-readable jText, queryable SQL, binary + reader

## Quick start

```cpp
#include <jac313/Store/v001.hpp>

using Config = jac313::Store::v001::ts_store_config<true, 6, 20, 43, 9, 6, false>;
jac313::Store::v001::ts_store<Config> store(threads, events_per_thread);
store.save_event(0, 0, Config::ValueT("hello"), 0, Config::CategoryT("demo"));
```

`jac313::Store::v001::version()` returns the package's `"major.minor"` version (`"v001.005"`
now): major is the `v001` API line, minor bumps on each small code change (a real break moves
it to `"v002.001"`). The `version-check` pre-push gate enforces the bump
(see [docs/Setup.md](../docs/Setup.md)).

## Layout

| Path | Purpose |
|------|---------|
| `include/jac313/Store/v001.hpp` | Public entry |
| `include/jac313/Store/v001/headers/` | Core store implementation (header-only) |
| `modules/jac313.store.v001.cppm` | Optional C++23 module |

Core includes in-memory `ts_store`, `ts_store_config`, `bounded_string`, and persistence hooks:

- **Binary** — `BinaryEventSink`, `BinaryEventLog` (header-only), `BinaryEventLogReader` (`jac313::store_binary_read`, `convert_to_jtext` via `jac313::store_jtext_persist`)
- **jText** — `JTextEventSink`, `JTextSplitEventLog` (`jac313::store_jtext_persist`, links in-tree `jtext_core`)
- **Routing** — `FlagRoutingEventSink` (header-only)

## Build

Self-contained — no sibling `ts_store` required for headers.

```bash
cmake -G Ninja -S . -B build -DCMAKE_CXX_COMPILER=g++-14
cmake --build build
./build/examples/jac313_store_v001_example
```

Optional in-tree module (Ninja; `g++-14` or `g++-15`):

```bash
cmake -G Ninja -S . -B build -DCMAKE_CXX_COMPILER=g++-14 -DJAC313_STORE_BUILD_MODULES=ON
```

## Tests

When built with `-DJAC313_BUILD_STORE_TESTS=ON` from the monorepo root:

- **Unit** — `jac313_store_v001_test`, `jac313_store_binary_reader_test`
- **Examples** — binary/jText/SQL/flag-routing persistence smokes (ctest)
- **Matrix** — ported ts_store stress tests `001`–`008` (TS/XS) plus `flags`

Run the full matrix locally via [`jac313_test_cli`](../tools/jac313_test_cli/) (see [docs/Setup.md → Testing](../docs/Setup.md#4-testing)).

For **throughput benchmarking** — the curated `store_bench --suite` (median + low–high band, durable flush inside the clock, results recorded to a SQLite DB via `--db` and rendered by `--report`), kept separate from the correctness matrix — see [docs/Benchmarks.md](../docs/Benchmarks.md). Quick: `./jac313_store_bench --suite --dry-run` from the build dir.

## Modules / `import std` (C++23)

`-DJAC313_BUILD_MODULES=ON` builds the textual-wrapper `jac313.store.v001` module
(Ninja). `-DJAC313_STORE_IMPORT_STD=ON` (g++-15, **CMake 4.3.3** — see below) builds it
**module-native** against `import std;` instead: one interface unit pastes the store headers
and `.cpp`, made std-free by guarding their `#include <…>` and `namespace` behind
`JAC313_STORE_IMPORT_STD`, into an `export namespace`; the POSIX headers store uses
(`mmap`/`sysinfo`/…), the C-compat headers, and Qlite + jText stay textual in the global
module fragment. Same public API; the compiled sink libs and textual headers are unchanged
for clang and non-module consumers. See the root README *`import std`* sections.

> **CMake version is exact, not a floor.** The native std-module gate is an experimental,
> **version-specific UUID** pinned to **4.3.3** in `CMakeLists.txt`. `≥ 3.30` is necessary but
> not sufficient — CMake 3.31.8 (≥ 3.30) still fails to configure; only the pinned 4.3.3
> activates it. Empirically confirmed on RHEL 9.8 (break-even vs textual modules); see
> [docs/Results.md → CMake 3.31.8 vs 4.3.3](../docs/Results.md#cmake-3318-vs-433).

## Performance tuning (optional)

The store is **flexible by design** — template metrics, payload caps, persist backends, and matrix scaling are starting points. The monorepo stays on **Debug builds for now**; tuning for your hardware (core count, NVMe vs SSD) and domain (fixed grids vs streaming events, keep-all vs keeper-only) is a deployment choice when you are ready.

See [Customization and performance](../docs/Architecture.md#customization-and-performance) for a speedup menu.

Part of the [jac313](../) monorepo.