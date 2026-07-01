<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# Compiler build times

_Generated from `results.db`. Compile time is hardware-specific, so each machine (`jac313-###`) is its own section. Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k

| test | clang21·hdr | clang21·mod | gcc15·hdr | gcc15·mod | gcc15·istd |
|---|:--:|:--:|:--:|:--:|:--:|
| jac313_identity_dedup_test | 1.60s · built · 529.13 KB | - | 2.15s · built · 668.21 KB | - | - |
| jac313_jtext_v001_parser_validator_test | 0.95s · built · 2.08 MB | - | 1.37s · built · 2.65 MB | - | - |
| jac313_jtext_v001_test | 0.70s · built · 1.34 MB | - | 0.59s · built · 1.62 MB | - | - |
| jac313_qlite_v001_test | 0.70s · built · 538.59 KB | - | 0.56s · built · 655.13 KB | - | - |
| jac313_run_identity_test | 1.79s · built · 444.98 KB | - | 1.84s · built · 570.98 KB | - | - |
| jac313_store_001_TS | 2.66s · built · 3.23 MB | 2.57s · built · 3.23 MB | 4.41s · built · 4.06 MB | 3.79s · built · 4.06 MB | 3.99s · built · 4.06 MB |
| jac313_store_001_XS | 3.00s · built · 3.21 MB | 2.50s · built · 3.21 MB | 3.78s · built · 4.04 MB | 3.55s · built · 4.04 MB | 3.90s · built · 4.04 MB |
| jac313_store_002_TS | 1.77s · built · 3.06 MB | 2.21s · built · 3.06 MB | 3.87s · built · 3.87 MB | 2.98s · built · 3.87 MB | 2.96s · built · 3.87 MB |
| jac313_store_002_XS | 2.62s · built · 3.05 MB | 3.29s · built · 3.05 MB | 3.52s · built · 3.85 MB | 2.84s · built · 3.85 MB | 3.01s · built · 3.85 MB |
| jac313_store_003_TS | 2.96s · built · 3.26 MB | 3.25s · built · 3.26 MB | 3.38s · built · 4.08 MB | 4.02s · built · 4.08 MB | 4.61s · built · 4.08 MB |
| jac313_store_003_XS | 2.52s · built · 3.25 MB | 2.82s · built · 3.25 MB | 3.87s · built · 4.07 MB | 4.25s · built · 4.07 MB | 3.49s · built · 4.07 MB |
| jac313_store_004_TS | 2.28s · built · 3.22 MB | 2.30s · built · 3.22 MB | 3.29s · built · 4.04 MB | 3.31s · built · 4.04 MB | 3.11s · built · 4.04 MB |
| jac313_store_004_XS | 2.94s · built · 3.20 MB | 2.24s · built · 3.20 MB | 4.58s · built · 4.01 MB | 3.26s · built · 4.01 MB | 3.38s · built · 4.01 MB |
| jac313_store_005_TS | 2.08s · built · 3.10 MB | 2.06s · built · 3.10 MB | 4.06s · built · 3.91 MB | 3.23s · built · 3.91 MB | 3.19s · built · 3.91 MB |
| jac313_store_005_XS | 2.53s · built · 3.08 MB | 3.16s · built · 3.08 MB | 3.03s · built · 3.89 MB | 4.16s · built · 3.89 MB | 3.87s · built · 3.89 MB |
| jac313_store_006_TS | 2.78s · built · 3.26 MB | 3.06s · built · 3.26 MB | 4.09s · built · 4.08 MB | 3.42s · built · 4.08 MB | 3.69s · built · 4.08 MB |
| jac313_store_006_XS | 2.84s · built · 3.25 MB | 2.41s · built · 3.25 MB | 3.79s · built · 4.07 MB | 3.21s · built · 4.07 MB | 3.46s · built · 4.07 MB |
| jac313_store_007_TS | 2.44s · built · 3.10 MB | 2.09s · built · 3.10 MB | 3.91s · built · 3.91 MB | 3.58s · built · 3.91 MB | 2.96s · built · 3.91 MB |
| jac313_store_007_XS | 2.38s · built · 3.08 MB | 2.04s · built · 3.08 MB | 3.52s · built · 3.89 MB | 3.12s · built · 3.89 MB | 3.18s · built · 3.89 MB |
| jac313_store_008_TS | 2.45s · built · 2.85 MB | 2.32s · built · 2.85 MB | 3.11s · built · 3.60 MB | 2.89s · built · 3.60 MB | 3.35s · built · 3.60 MB |
| jac313_store_008_XS | 2.23s · built · 2.85 MB | 1.96s · built · 2.85 MB | 3.44s · built · 3.59 MB | 3.01s · built · 3.60 MB | 2.91s · built · 3.60 MB |
| jac313_store_bench | 2.20s · built · 3.52 MB | 2.45s · built · 3.52 MB | 4.52s · built · 4.43 MB | 3.90s · built · 4.43 MB | 3.80s · built · 4.43 MB |
| jac313_store_binary_persist_smoke | 1.70s · built · 1.31 MB | - | 2.29s · built · 1.61 MB | - | - |
| jac313_store_binary_reader_test | 2.29s · built · 2.43 MB | - | 2.88s · built · 3.10 MB | - | - |
| jac313_store_binary_to_jtext_smoke | 1.57s · built · 2.41 MB | - | 2.56s · built · 3.09 MB | - | - |
| jac313_store_flag_routing_smoke | 2.48s · built · 1.73 MB | - | 2.67s · built · 2.16 MB | - | - |
| jac313_store_flags | 0.69s · built · 194.66 KB | 0.69s · built · 194.67 KB | 0.97s · built · 239.69 KB | 0.90s · built · 239.98 KB | 1.08s · built · 239.98 KB |
| jac313_store_jtext_persist_smoke | 1.81s · built · 2.08 MB | - | 1.96s · built · 2.60 MB | - | - |
| jac313_store_metric_view | 1.30s · built · 719.29 KB | 2.13s · built · 719.30 KB | 2.34s · built · 888.53 KB | 2.08s · built · 888.86 KB | 1.82s · built · 888.86 KB |
| jac313_store_metric_view_test | 1.31s · built · 705.87 KB | - | 2.33s · built · 872.94 KB | - | - |
| jac313_store_sql_persist_smoke | 1.64s · built · 829.66 KB | - | 1.94s · built · 1.03 MB | - | - |
| jac313_store_v001_test | 1.30s · built · 423.98 KB | - | 1.05s · built · 575.74 KB | - | - |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
