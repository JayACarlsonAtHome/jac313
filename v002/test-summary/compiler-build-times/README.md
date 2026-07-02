<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# Compiler build times

_Generated from `results.db`. Compile time is hardware-specific, so each machine (`jac313-###`) is its own section. Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k

| test | clang21·hdr | clang21·istd | gcc15·hdr |
|---|:--:|:--:|:--:|
| jac313_contracts_disabled_test | 0.72s · built · 28.73 KB | - | 0.66s · built · 46.64 KB |
| jac313_contracts_enforce_test | 0.73s · built · 526.30 KB | - | 1.42s · built · 655.04 KB |
| jac313_contracts_test | 0.53s · built · 31.22 KB | - | 0.74s · built · 49.38 KB |
| jac313_identity_dedup_test | 1.69s · built · 529.39 KB | - | 2.34s · built · 682.80 KB |
| jac313_jtext_v002_parser_validator_test | 1.00s · built · 2.08 MB | - | 1.29s · built · 2.68 MB |
| jac313_jtext_v002_test | 0.97s · built · 1.34 MB | - | 1.01s · built · 1.64 MB |
| jac313_qlite_v002_test | 0.73s · built · 538.59 KB | - | 1.16s · built · 655.14 KB |
| jac313_run_identity_test | 1.76s · built · 445.12 KB | - | 2.36s · built · 591.22 KB |
| jac313_store_001_TS | 2.55s · built · 3.21 MB | 2.24s · built · 3.21 MB | 4.63s · built · 4.07 MB |
| jac313_store_001_XS | 2.18s · built · 3.20 MB | 2.49s · built · 3.20 MB | 4.67s · built · 4.05 MB |
| jac313_store_002_TS | 2.42s · built · 3.06 MB | 2.84s · built · 3.06 MB | 4.12s · built · 3.90 MB |
| jac313_store_002_XS | 2.60s · built · 3.05 MB | 2.63s · built · 3.05 MB | 3.37s · built · 3.88 MB |
| jac313_store_003_TS | 2.75s · built · 3.25 MB | 2.54s · built · 3.25 MB | 3.96s · built · 4.09 MB |
| jac313_store_003_XS | 2.61s · built · 3.23 MB | 2.71s · built · 3.23 MB | 4.63s · built · 4.07 MB |
| jac313_store_004_TS | 2.82s · built · 3.20 MB | 3.02s · built · 3.20 MB | 3.85s · built · 4.05 MB |
| jac313_store_004_XS | 2.74s · built · 3.18 MB | 2.63s · built · 3.18 MB | 4.45s · built · 4.02 MB |
| jac313_store_005_TS | 2.43s · built · 3.09 MB | 2.36s · built · 3.09 MB | 3.58s · built · 3.94 MB |
| jac313_store_005_XS | 2.41s · built · 3.08 MB | 2.90s · built · 3.08 MB | 4.89s · built · 3.92 MB |
| jac313_store_006_TS | 2.90s · built · 3.25 MB | 2.64s · built · 3.25 MB | 3.77s · built · 4.09 MB |
| jac313_store_006_XS | 2.12s · built · 3.23 MB | 2.46s · built · 3.23 MB | 3.19s · built · 4.07 MB |
| jac313_store_007_TS | 2.30s · built · 3.09 MB | 2.53s · built · 3.09 MB | 4.27s · built · 3.94 MB |
| jac313_store_007_XS | 3.33s · built · 3.08 MB | 2.21s · built · 3.08 MB | 4.03s · built · 3.92 MB |
| jac313_store_008_TS | 2.04s · built · 2.85 MB | 2.12s · built · 2.85 MB | 3.83s · built · 3.63 MB |
| jac313_store_008_XS | 3.01s · built · 2.85 MB | 2.48s · built · 2.85 MB | 2.72s · built · 3.63 MB |
| jac313_store_bench | 2.69s · built · 3.51 MB | 3.06s · built · 3.51 MB | 3.62s · built · 4.46 MB |
| jac313_store_binary_persist_smoke | 2.23s · built · 1.32 MB | - | 2.55s · built · 1.64 MB |
| jac313_store_binary_reader_test | 1.78s · built · 2.46 MB | - | 2.71s · built · 3.16 MB |
| jac313_store_binary_to_jtext_smoke | 1.77s · built · 2.43 MB | - | 3.59s · built · 3.13 MB |
| jac313_store_flag_routing_smoke | 1.85s · built · 1.74 MB | - | 2.87s · built · 2.20 MB |
| jac313_store_flags | 1.02s · built · 194.73 KB | 0.77s · built · 194.75 KB | 0.88s · built · 239.98 KB |
| jac313_store_jtext_persist_smoke | 2.08s · built · 2.09 MB | - | 2.06s · built · 2.63 MB |
| jac313_store_metric_view | 1.61s · built · 720.16 KB | 1.65s · built · 720.17 KB | 1.88s · built · 902.54 KB |
| jac313_store_metric_view_test | 1.49s · built · 706.73 KB | - | 2.18s · built · 886.95 KB |
| jac313_store_saturating_capacity_test | 0.66s · built · 31.96 KB | - | 0.84s · built · 50.92 KB |
| jac313_store_sql_persist_smoke | 1.48s · built · 830.10 KB | - | 1.78s · built · 1.05 MB |
| jac313_store_v002_test | 1.21s · built · 423.43 KB | - | 1.73s · built · 575.03 KB |

## jac313-002 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · Fedora Linux 44 (Workstation Edition) · x7k

| test | clang22·hdr | gcc16·hdr |
|---|:--:|:--:|
| jac313_contracts_disabled_test | 1.05s · built · 29.96 KB | 0.70s · built · 47.55 KB |
| jac313_contracts_enforce_test | 1.29s · built · 561.69 KB | 1.26s · built · 730.27 KB |
| jac313_contracts_test | 0.86s · built · 32.53 KB | 0.72s · built · 87.05 KB |
| jac313_identity_dedup_test | 2.30s · built · 567.21 KB | 2.59s · built · 681.16 KB |
| jac313_jtext_v002_parser_validator_test | 1.76s · built · 2.23 MB | 0.87s · built · 2.75 MB |
| jac313_jtext_v002_test | 1.05s · built · 1.43 MB | 1.08s · built · 1.68 MB |
| jac313_qlite_v002_test | 0.80s · built · 580.16 KB | 1.01s · built · 638.93 KB |
| jac313_run_identity_test | 2.26s · built · 467.34 KB | 2.29s · built · 591.10 KB |
| jac313_store_001_TS | 3.63s · built · 3.44 MB | 3.73s · built · 4.14 MB |
| jac313_store_001_XS | 2.48s · built · 3.42 MB | 3.61s · built · 4.12 MB |
| jac313_store_002_TS | 3.17s · built · 3.28 MB | 3.52s · built · 3.98 MB |
| jac313_store_002_XS | 2.48s · built · 3.26 MB | 3.49s · built · 3.95 MB |
| jac313_store_003_TS | 3.20s · built · 3.47 MB | 3.74s · built · 4.17 MB |
| jac313_store_003_XS | 3.10s · built · 3.45 MB | 4.87s · built · 4.14 MB |
| jac313_store_004_TS | 4.03s · built · 3.42 MB | 4.20s · built · 4.12 MB |
| jac313_store_004_XS | 3.26s · built · 3.40 MB | 3.27s · built · 4.09 MB |
| jac313_store_005_TS | 3.58s · built · 3.31 MB | 3.88s · built · 4.01 MB |
| jac313_store_005_XS | 2.62s · built · 3.30 MB | 3.26s · built · 3.99 MB |
| jac313_store_006_TS | 3.15s · built · 3.47 MB | 3.69s · built · 4.17 MB |
| jac313_store_006_XS | 3.09s · built · 3.45 MB | 3.48s · built · 4.14 MB |
| jac313_store_007_TS | 2.88s · built · 3.31 MB | 3.51s · built · 4.01 MB |
| jac313_store_007_XS | 3.24s · built · 3.30 MB | 3.21s · built · 3.99 MB |
| jac313_store_008_TS | 2.85s · built · 3.07 MB | 3.45s · built · 3.70 MB |
| jac313_store_008_XS | 2.57s · built · 3.06 MB | 3.16s · built · 3.70 MB |
| jac313_store_bench | 3.52s · built · 3.75 MB | 4.65s · built · 4.54 MB |
| jac313_store_binary_persist_smoke | 2.51s · built · 1.41 MB | 2.39s · built · 1.66 MB |
| jac313_store_binary_reader_test | 3.01s · built · 2.62 MB | 2.54s · built · 3.22 MB |
| jac313_store_binary_to_jtext_smoke | 2.84s · built · 2.59 MB | 2.88s · built · 3.20 MB |
| jac313_store_flag_routing_smoke | 3.01s · built · 1.86 MB | 3.02s · built · 2.21 MB |
| jac313_store_flags | 0.93s · built · 202.70 KB | 0.97s · built · 235.80 KB |
| jac313_store_jtext_persist_smoke | 2.44s · built · 2.22 MB | 1.50s · built · 2.68 MB |
| jac313_store_metric_view | 2.73s · built · 774.52 KB | 2.07s · built · 913.10 KB |
| jac313_store_metric_view_test | 2.00s · built · 766.18 KB | 2.27s · built · 900.95 KB |
| jac313_store_saturating_capacity_test | 1.02s · built · 33.16 KB | 0.59s · built · 51.75 KB |
| jac313_store_sql_persist_smoke | 2.27s · built · 898.84 KB | 2.39s · built · 1.04 MB |
| jac313_store_v002_test | 2.00s · built · 457.48 KB | 2.18s · built · 566.55 KB |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
