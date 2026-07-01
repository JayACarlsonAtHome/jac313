<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# Compiler build times

_Generated from `results.db`. Compile time is hardware-specific, so each machine (`jac313-###`) is its own section. Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 19 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k

| test | clang21·hdr | clang21·mod | gcc15·hdr | gcc15·mod | gcc15·istd |
|---|:--:|:--:|:--:|:--:|:--:|
| jac313_jtext_v001_parser_validator_test | 1.04s · built · 2.08 MB | - | 0.97s · built · 2.65 MB | - | - |
| jac313_jtext_v001_test | 1.10s · built · 1.34 MB | - | 0.86s · built · 1.62 MB | - | - |
| jac313_qlite_v001_test | 0.49s · built · 493.09 KB | - | 1.01s · built · 605.12 KB | - | - |
| jac313_store_001_TS | - | 2.47s · built · 3.22 MB | - | 3.49s · built · 4.06 MB | 3.73s · built · 4.06 MB |
| jac313_store_001_XS | - | 2.67s · built · 3.21 MB | - | 4.82s · built · 4.04 MB | 5.00s · built · 4.04 MB |
| jac313_store_002_TS | - | 2.26s · built · 3.06 MB | - | 3.96s · built · 3.87 MB | 3.22s · built · 3.87 MB |
| jac313_store_002_XS | - | 2.39s · built · 3.05 MB | - | 4.51s · built · 3.85 MB | 5.07s · built · 3.85 MB |
| jac313_store_003_TS | - | 4.00s · built · 3.26 MB | - | 3.45s · built · 4.08 MB | 3.28s · built · 4.08 MB |
| jac313_store_003_XS | - | 3.76s · built · 3.25 MB | - | 3.51s · built · 4.06 MB | 3.37s · built · 4.06 MB |
| jac313_store_004_TS | - | 2.20s · built · 3.22 MB | - | 3.77s · built · 4.04 MB | 4.49s · built · 4.04 MB |
| jac313_store_004_XS | - | 2.25s · built · 3.20 MB | - | 4.22s · built · 4.01 MB | 4.25s · built · 4.01 MB |
| jac313_store_005_TS | - | 2.50s · built · 3.09 MB | - | 3.70s · built · 3.91 MB | 3.57s · built · 3.91 MB |
| jac313_store_005_XS | - | 2.31s · built · 3.08 MB | - | 3.32s · built · 3.89 MB | 3.17s · built · 3.89 MB |
| jac313_store_006_TS | - | 2.33s · built · 3.26 MB | - | 4.58s · built · 4.08 MB | 4.91s · built · 4.08 MB |
| jac313_store_006_XS | - | 3.25s · built · 3.25 MB | - | 3.50s · built · 4.06 MB | 2.98s · built · 4.06 MB |
| jac313_store_007_TS | - | 2.77s · built · 3.10 MB | - | 3.94s · built · 3.91 MB | 4.37s · built · 3.91 MB |
| jac313_store_007_XS | - | 2.21s · built · 3.08 MB | - | 3.93s · built · 3.89 MB | 3.41s · built · 3.89 MB |
| jac313_store_008_TS | - | 2.75s · built · 2.85 MB | - | 2.84s · built · 3.59 MB | 3.04s · built · 3.59 MB |
| jac313_store_008_XS | - | 2.08s · built · 2.85 MB | - | 2.91s · built · 3.59 MB | 2.92s · built · 3.59 MB |
| jac313_store_bench | - | 2.68s · built · 3.51 MB | - | 4.18s · built · 4.43 MB | 4.08s · built · 4.43 MB |
| jac313_store_flags | - | 0.70s · built · 194.67 KB | - | 0.95s · built · 239.98 KB | 0.93s · built · 239.98 KB |
| jac313_store_metric_view | - | 2.24s · built · 719.30 KB | - | 1.97s · built · 888.82 KB | 1.91s · built · 888.82 KB |
| jac313_store_v001_test | 1.54s · built · 422.07 KB | - | 1.84s · built · 568.10 KB | - | - |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
