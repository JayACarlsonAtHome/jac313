<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# Compiler build times

_Generated from `results.db`. Compile time is hardware-specific, so each machine (`jac313-###`) is its own section. Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · fedora-44 · x7k

| test | clang22·hdr | clang22·mod | gcc16·hdr | gcc16·mod | gcc16·istd |
|---|:--:|:--:|:--:|:--:|:--:|
| jac313_host_group_test | 2.25s · built · 393.61 KB | - | 1.49s · built · 448.41 KB | - | - |
| jac313_identity_dedup_test | 2.24s · built · 564.49 KB | - | 2.03s · built · 657.29 KB | - | - |
| jac313_instance_hash_test | 1.02s · built · 128.72 KB | - | 1.08s · built · 161.52 KB | - | - |
| jac313_jtext_v001_parser_validator_test | 1.63s · built · 2.22 MB | - | 1.61s · built · 2.68 MB | - | - |
| jac313_jtext_v001_test | 0.94s · built · 1.42 MB | - | 1.16s · built · 1.64 MB | - | - |
| jac313_qlite_v001_test | 0.87s · built · 580.16 KB | - | 0.83s · built · 638.92 KB | - | - |
| jac313_run_identity_test | 2.71s · built · 464.42 KB | - | 2.73s · built · 556.12 KB | - | - |
| jac313_store_001_TS | 2.87s · built · 3.44 MB | 2.79s · built · 3.44 MB | 3.53s · built · 4.07 MB | 3.51s · built · 4.07 MB | 4.24s · built · 4.07 MB |
| jac313_store_001_XS | 3.98s · built · 3.43 MB | 2.80s · built · 3.43 MB | 4.18s · built · 4.05 MB | 3.50s · built · 4.05 MB | 3.87s · built · 4.05 MB |
| jac313_store_002_TS | 2.77s · built · 3.27 MB | 2.90s · built · 3.27 MB | 4.05s · built · 3.89 MB | 3.28s · built · 3.89 MB | 2.98s · built · 3.89 MB |
| jac313_store_002_XS | 2.77s · built · 3.26 MB | 3.75s · built · 3.26 MB | 3.48s · built · 3.87 MB | 3.58s · built · 3.87 MB | 3.64s · built · 3.87 MB |
| jac313_store_003_TS | 3.17s · built · 3.48 MB | 4.37s · built · 3.48 MB | 5.21s · built · 4.09 MB | 3.91s · built · 4.09 MB | 3.61s · built · 4.09 MB |
| jac313_store_003_XS | 2.82s · built · 3.46 MB | 2.77s · built · 3.46 MB | 5.03s · built · 4.07 MB | 3.16s · built · 4.07 MB | 4.06s · built · 4.07 MB |
| jac313_store_004_TS | 2.61s · built · 3.43 MB | 2.89s · built · 3.43 MB | 2.93s · built · 4.05 MB | 3.86s · built · 4.05 MB | 4.00s · built · 4.05 MB |
| jac313_store_004_XS | 4.34s · built · 3.41 MB | 3.55s · built · 3.41 MB | 3.75s · built · 4.02 MB | 3.60s · built · 4.02 MB | 4.04s · built · 4.02 MB |
| jac313_store_005_TS | 2.98s · built · 3.30 MB | 2.56s · built · 3.30 MB | 3.90s · built · 3.92 MB | 3.60s · built · 3.92 MB | 3.60s · built · 3.92 MB |
| jac313_store_005_XS | 2.51s · built · 3.29 MB | 2.53s · built · 3.29 MB | 3.40s · built · 3.90 MB | 3.55s · built · 3.90 MB | 3.28s · built · 3.90 MB |
| jac313_store_006_TS | 3.73s · built · 3.48 MB | 3.46s · built · 3.48 MB | 5.49s · built · 4.09 MB | 3.75s · built · 4.09 MB | 3.51s · built · 4.09 MB |
| jac313_store_006_XS | 2.72s · built · 3.46 MB | 3.79s · built · 3.46 MB | 4.57s · built · 4.07 MB | 4.29s · built · 4.07 MB | 3.58s · built · 4.07 MB |
| jac313_store_007_TS | 2.46s · built · 3.30 MB | 2.57s · built · 3.30 MB | 4.26s · built · 3.92 MB | 3.81s · built · 3.93 MB | 3.28s · built · 3.93 MB |
| jac313_store_007_XS | 2.86s · built · 3.29 MB | 2.67s · built · 3.29 MB | 4.36s · built · 3.91 MB | 2.78s · built · 3.91 MB | 3.49s · built · 3.91 MB |
| jac313_store_008_TS | 2.95s · built · 3.05 MB | 2.91s · built · 3.05 MB | 2.40s · built · 3.61 MB | 3.55s · built · 3.62 MB | 3.16s · built · 3.62 MB |
| jac313_store_008_XS | 2.46s · built · 3.05 MB | 2.27s · built · 3.05 MB | 2.60s · built · 3.61 MB | 2.88s · built · 3.62 MB | 3.02s · built · 3.62 MB |
| jac313_store_bench | 3.05s · built · 3.76 MB | 3.33s · built · 3.76 MB | 4.96s · built · 4.44 MB | 4.03s · built · 4.44 MB | 3.89s · built · 4.44 MB |
| jac313_store_binary_persist_smoke | 2.89s · built · 1.39 MB | - | 2.28s · built · 1.61 MB | - | - |
| jac313_store_binary_reader_test | 2.40s · built · 2.59 MB | - | 3.73s · built · 3.12 MB | - | - |
| jac313_store_binary_to_jtext_smoke | 2.11s · built · 2.58 MB | - | 2.11s · built · 3.11 MB | - | - |
| jac313_store_flag_routing_smoke | 2.09s · built · 1.84 MB | - | 3.03s · built · 2.15 MB | - | - |
| jac313_store_flags | 1.56s · built · 201.18 KB | 1.21s · built · 201.20 KB | 1.11s · built · 232.02 KB | 1.15s · built · 232.31 KB | 1.01s · built · 232.31 KB |
| jac313_store_jtext_persist_smoke | 1.86s · built · 2.22 MB | - | 2.63s · built · 2.61 MB | - | - |
| jac313_store_metric_view | 1.67s · built · 772.16 KB | 2.48s · built · 772.17 KB | 2.37s · built · 888.08 KB | 2.80s · built · 888.41 KB | 2.06s · built · 888.41 KB |
| jac313_store_metric_view_test | 2.65s · built · 763.83 KB | - | 2.43s · built · 876.23 KB | - | - |
| jac313_store_sql_persist_smoke | 2.38s · built · 895.18 KB | - | 2.87s · built · 1.01 MB | - | - |
| jac313_store_v001_test | 1.82s · built · 454.12 KB | - | 2.04s · built · 558.73 KB | - | - |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
