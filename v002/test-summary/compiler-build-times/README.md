<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# Compiler build times

_Generated from `results.db`. Compile time is hardware-specific, so each machine (`jac313-###`) is its own section.

Column headers are `compiler·fe`:
- `hdr` — code uses headers only
- `mod` — code uses modules
- `istd` — code uses modules with `import std` (gcc-only)

Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · fedora-44 · x7k

| test | clang22·hdr | gcc16·hdr |
|---|:--:|:--:|
| jac313_contracts_disabled_test | 0.77s · built · 29.96 KB | 0.61s · built · 47.55 KB |
| jac313_contracts_enforce_test | 1.29s · built · 561.69 KB | 0.98s · built · 730.27 KB |
| jac313_contracts_test | 1.07s · built · 32.53 KB | 0.87s · built · 87.05 KB |
| jac313_host_group_test | 1.95s · built · 395.30 KB | 1.62s · built · 458.80 KB |
| jac313_identity_dedup_test | 3.68s · built · 567.21 KB | 1.73s · built · 681.16 KB |
| jac313_instance_hash_test | 0.80s · built · 130.23 KB | 0.70s · built · 164.05 KB |
| jac313_jtext_v002_parser_validator_test | 1.27s · built · 2.23 MB | 1.01s · built · 2.75 MB |
| jac313_jtext_v002_test | 1.42s · built · 1.43 MB | 1.19s · built · 1.68 MB |
| jac313_qlite_v002_test | 1.10s · built · 580.16 KB | 0.83s · built · 638.93 KB |
| jac313_run_identity_test | 2.73s · built · 467.34 KB | 1.81s · built · 591.10 KB |
| jac313_store_001_TS | 3.70s · built · 3.44 MB | 3.50s · built · 4.14 MB |
| jac313_store_001_XS | 2.99s · built · 3.42 MB | 3.41s · built · 4.12 MB |
| jac313_store_002_TS | 3.52s · built · 3.28 MB | 4.34s · built · 3.98 MB |
| jac313_store_002_XS | 3.30s · built · 3.26 MB | 3.69s · built · 3.95 MB |
| jac313_store_003_TS | 3.71s · built · 3.47 MB | 3.27s · built · 4.17 MB |
| jac313_store_003_XS | 3.55s · built · 3.45 MB | 3.76s · built · 4.14 MB |
| jac313_store_004_TS | 3.97s · built · 3.42 MB | 3.64s · built · 4.12 MB |
| jac313_store_004_XS | 2.57s · built · 3.40 MB | 3.81s · built · 4.09 MB |
| jac313_store_005_TS | 2.86s · built · 3.31 MB | 3.02s · built · 4.01 MB |
| jac313_store_005_XS | 3.60s · built · 3.30 MB | 3.39s · built · 3.99 MB |
| jac313_store_006_TS | 3.58s · built · 3.47 MB | 3.68s · built · 4.17 MB |
| jac313_store_006_XS | 3.06s · built · 3.45 MB | 4.16s · built · 4.14 MB |
| jac313_store_007_TS | 2.95s · built · 3.31 MB | 3.26s · built · 4.01 MB |
| jac313_store_007_XS | 3.42s · built · 3.30 MB | 3.57s · built · 3.99 MB |
| jac313_store_008_TS | 3.06s · built · 3.07 MB | 4.27s · built · 3.70 MB |
| jac313_store_008_XS | 2.82s · built · 3.06 MB | 2.72s · built · 3.70 MB |
| jac313_store_bench | 3.34s · built · 3.77 MB | 4.42s · built · 4.54 MB |
| jac313_store_binary_persist_smoke | 3.59s · built · 1.41 MB | 2.29s · built · 1.66 MB |
| jac313_store_binary_reader_test | 2.67s · built · 2.62 MB | 2.52s · built · 3.22 MB |
| jac313_store_binary_to_jtext_smoke | 3.34s · built · 2.59 MB | 3.20s · built · 3.20 MB |
| jac313_store_flag_routing_smoke | 2.81s · built · 1.86 MB | 3.21s · built · 2.21 MB |
| jac313_store_flags | 1.64s · built · 202.70 KB | 0.72s · built · 235.80 KB |
| jac313_store_jtext_persist_smoke | 2.76s · built · 2.22 MB | 2.09s · built · 2.68 MB |
| jac313_store_metric_view | 1.81s · built · 774.52 KB | 1.68s · built · 913.10 KB |
| jac313_store_metric_view_test | 1.57s · built · 766.18 KB | 1.99s · built · 900.95 KB |
| jac313_store_saturating_capacity_test | 1.15s · built · 33.16 KB | 1.06s · built · 51.75 KB |
| jac313_store_sql_persist_smoke | 1.66s · built · 898.84 KB | 1.67s · built · 1.04 MB |
| jac313_store_v002_test | 1.98s · built · 457.48 KB | 1.32s · built · 566.55 KB |

## jac313-002 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · rhel-10.2 · x7k

| test | clang21·hdr | gcc15·hdr |
|---|:--:|:--:|
| jac313_contracts_disabled_test | 0.64s · built · 28.73 KB | 0.51s · built · 46.64 KB |
| jac313_contracts_enforce_test | 0.82s · built · 526.30 KB | 1.16s · built · 655.04 KB |
| jac313_contracts_test | 0.54s · built · 31.22 KB | 0.84s · built · 49.38 KB |
| jac313_host_group_test | 1.50s · built · 373.01 KB | 1.17s · built · 461.62 KB |
| jac313_identity_dedup_test | 1.73s · built · 529.39 KB | 1.76s · built · 682.80 KB |
| jac313_instance_hash_test | 0.78s · built · 125.63 KB | 0.71s · built · 174.45 KB |
| jac313_jtext_v002_parser_validator_test | 0.84s · built · 2.08 MB | 0.96s · built · 2.68 MB |
| jac313_jtext_v002_test | 0.96s · built · 1.34 MB | 0.89s · built · 1.64 MB |
| jac313_qlite_v002_test | 0.57s · built · 538.59 KB | 0.94s · built · 655.14 KB |
| jac313_run_identity_test | 1.86s · built · 445.12 KB | 1.95s · built · 591.22 KB |
| jac313_store_001_TS | 1.92s · built · 3.22 MB | 2.72s · built · 4.07 MB |
| jac313_store_001_XS | 2.75s · built · 3.20 MB | 3.38s · built · 4.05 MB |
| jac313_store_002_TS | 2.75s · built · 3.06 MB | 3.26s · built · 3.90 MB |
| jac313_store_002_XS | 2.63s · built · 3.05 MB | 3.11s · built · 3.88 MB |
| jac313_store_003_TS | 2.34s · built · 3.25 MB | 3.68s · built · 4.10 MB |
| jac313_store_003_XS | 3.17s · built · 3.23 MB | 4.43s · built · 4.07 MB |
| jac313_store_004_TS | 2.80s · built · 3.20 MB | 3.58s · built · 4.05 MB |
| jac313_store_004_XS | 2.08s · built · 3.18 MB | 2.82s · built · 4.03 MB |
| jac313_store_005_TS | 1.99s · built · 3.10 MB | 3.10s · built · 3.94 MB |
| jac313_store_005_XS | 2.98s · built · 3.08 MB | 3.62s · built · 3.92 MB |
| jac313_store_006_TS | 2.91s · built · 3.25 MB | 3.29s · built · 4.10 MB |
| jac313_store_006_XS | 3.11s · built · 3.23 MB | 3.33s · built · 4.07 MB |
| jac313_store_007_TS | 1.96s · built · 3.10 MB | 3.02s · built · 3.94 MB |
| jac313_store_007_XS | 2.51s · built · 3.08 MB | 3.26s · built · 3.92 MB |
| jac313_store_008_TS | 2.61s · built · 2.85 MB | 2.72s · built · 3.64 MB |
| jac313_store_008_XS | 3.14s · built · 2.85 MB | 3.96s · built · 3.63 MB |
| jac313_store_bench | 2.81s · built · 3.54 MB | 4.22s · built · 4.47 MB |
| jac313_store_binary_persist_smoke | 2.59s · built · 1.32 MB | 2.49s · built · 1.64 MB |
| jac313_store_binary_reader_test | 2.12s · built · 2.46 MB | 2.07s · built · 3.16 MB |
| jac313_store_binary_to_jtext_smoke | 2.61s · built · 2.43 MB | 2.55s · built · 3.13 MB |
| jac313_store_flag_routing_smoke | 1.89s · built · 1.74 MB | 2.94s · built · 2.20 MB |
| jac313_store_flags | 0.91s · built · 194.73 KB | 0.55s · built · 239.98 KB |
| jac313_store_jtext_persist_smoke | 1.46s · built · 2.09 MB | 2.00s · built · 2.63 MB |
| jac313_store_metric_view | 1.66s · built · 720.16 KB | 2.50s · built · 902.58 KB |
| jac313_store_metric_view_test | 1.48s · built · 706.73 KB | 2.31s · built · 886.98 KB |
| jac313_store_saturating_capacity_test | 0.76s · built · 31.96 KB | 0.41s · built · 50.92 KB |
| jac313_store_sql_persist_smoke | 1.34s · built · 830.10 KB | 1.51s · built · 1.05 MB |
| jac313_store_v002_test | 1.27s · built · 429.45 KB | 1.85s · built · 579.34 KB |

## jac313-003 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · linuxmint-22.3 · x7k

| test | clang20·hdr | gcc15·hdr | gcc15·istd |
|---|:--:|:--:|:--:|
| jac313_contracts_disabled_test | 1.18s · built · 31.88 KB | 0.81s · built · 49.82 KB | - |
| jac313_contracts_enforce_test | 1.46s · built · 528.20 KB | 1.25s · built · 670.59 KB | - |
| jac313_contracts_test | 1.29s · built · 34.48 KB | 0.84s · built · 52.54 KB | - |
| jac313_host_group_test | 2.34s · built · 380.52 KB | 1.23s · built · 473.26 KB | - |
| jac313_identity_dedup_test | 3.78s · built · 538.84 KB | 2.30s · built · 690.20 KB | - |
| jac313_instance_hash_test | 1.41s · built · 130.83 KB | 1.01s · built · 177.75 KB | - |
| jac313_jtext_v002_parser_validator_test | 1.03s · built · 2.12 MB | 1.35s · built · 2.72 MB | - |
| jac313_jtext_v002_test | 0.90s · built · 1.33 MB | 0.74s · built · 1.65 MB | - |
| jac313_qlite_v002_test | 1.95s · built · 547.78 KB | 2.70s · built · 671.20 KB | - |
| jac313_run_identity_test | 2.94s · built · 457.66 KB | 1.85s · built · 598.82 KB | - |
| jac313_store_001_TS | 6.29s · built · 3.23 MB | 10.54s · built · 4.11 MB | 3.26s · built · 4.11 MB |
| jac313_store_001_XS | 6.04s · built · 3.22 MB | 9.55s · built · 4.09 MB | 3.25s · built · 4.09 MB |
| jac313_store_002_TS | 5.44s · built · 3.08 MB | 9.83s · built · 3.93 MB | 3.39s · built · 3.93 MB |
| jac313_store_002_XS | 7.74s · built · 3.07 MB | 6.92s · built · 3.91 MB | 4.92s · built · 3.91 MB |
| jac313_store_003_TS | 8.97s · built · 3.27 MB | 10.75s · built · 4.13 MB | 4.71s · built · 4.13 MB |
| jac313_store_003_XS | 8.17s · built · 3.25 MB | 10.51s · built · 4.11 MB | 3.59s · built · 4.11 MB |
| jac313_store_004_TS | 9.10s · built · 3.23 MB | 9.59s · built · 4.09 MB | 3.54s · built · 4.09 MB |
| jac313_store_004_XS | 8.19s · built · 3.20 MB | 5.22s · built · 4.06 MB | 3.47s · built · 4.06 MB |
| jac313_store_005_TS | 8.36s · built · 3.12 MB | 6.37s · built · 3.97 MB | 3.23s · built · 3.97 MB |
| jac313_store_005_XS | 8.17s · built · 3.10 MB | 6.78s · built · 3.95 MB | 3.77s · built · 3.95 MB |
| jac313_store_006_TS | 10.74s · built · 3.27 MB | 6.13s · built · 4.13 MB | 3.99s · built · 4.13 MB |
| jac313_store_006_XS | 7.21s · built · 3.25 MB | 7.12s · built · 4.11 MB | 3.46s · built · 4.11 MB |
| jac313_store_007_TS | 9.01s · built · 3.12 MB | 6.95s · built · 3.97 MB | 3.53s · built · 3.98 MB |
| jac313_store_007_XS | 7.63s · built · 3.10 MB | 4.59s · built · 3.95 MB | 3.31s · built · 3.96 MB |
| jac313_store_008_TS | 6.64s · built · 2.87 MB | 4.31s · built · 3.66 MB | 2.75s · built · 3.67 MB |
| jac313_store_008_XS | 5.48s · built · 2.87 MB | 4.25s · built · 3.66 MB | 2.19s · built · 3.67 MB |
| jac313_store_bench | 4.25s · built · 3.57 MB | 10.80s · built · 4.51 MB | 4.05s · built · 4.51 MB |
| jac313_store_binary_persist_smoke | 3.41s · built · 1.31 MB | 2.70s · built · 1.64 MB | - |
| jac313_store_binary_reader_test | 3.75s · built · 2.47 MB | 3.23s · built · 3.17 MB | - |
| jac313_store_binary_to_jtext_smoke | 2.44s · built · 2.44 MB | 2.89s · built · 3.15 MB | - |
| jac313_store_flag_routing_smoke | 4.50s · built · 1.75 MB | 9.18s · built · 2.20 MB | - |
| jac313_store_flags | 1.50s · built · 200.99 KB | 0.99s · built · 243.38 KB | 1.07s · built · 243.68 KB |
| jac313_store_jtext_persist_smoke | 2.70s · built · 2.08 MB | 1.95s · built · 2.64 MB | - |
| jac313_store_metric_view | 2.44s · built · 725.92 KB | 2.11s · built · 922.68 KB | 1.72s · built · 923.01 KB |
| jac313_store_metric_view_test | 2.33s · built · 717.05 KB | 2.67s · built · 907.11 KB | - |
| jac313_store_saturating_capacity_test | 0.89s · built · 35.12 KB | 0.61s · built · 54.12 KB | - |
| jac313_store_sql_persist_smoke | 3.82s · built · 856.38 KB | 5.66s · built · 1.06 MB | - |
| jac313_store_v002_test | 2.43s · built · 431.88 KB | 2.06s · built · 591.23 KB | - |

## jac313-004 — Intel(R) Core(TM) Ultra 7 265 · 20 cores · 62 GB · rhel-9.8 · ssd

| test | clang21·hdr | gcc15·hdr |
|---|:--:|:--:|
| jac313_contracts_disabled_test | 0.56s · built · 27.12 KB | 0.79s · built · 46.78 KB |
| jac313_contracts_enforce_test | 0.91s · built · 531.55 KB | 0.96s · built · 806.74 KB |
| jac313_contracts_test | 0.63s · built · 29.54 KB | 0.80s · built · 49.54 KB |
| jac313_host_group_test | 1.19s · built · 373.45 KB | 1.30s · built · 461.84 KB |
| jac313_identity_dedup_test | 2.09s · built · 530.99 KB | 2.28s · built · 683.17 KB |
| jac313_instance_hash_test | 0.94s · built · 124.62 KB | 0.65s · built · 178.79 KB |
| jac313_jtext_v002_parser_validator_test | 0.65s · built · 2.12 MB | 0.76s · built · 2.82 MB |
| jac313_jtext_v002_test | 0.57s · built · 1.36 MB | 0.89s · built · 1.79 MB |
| jac313_qlite_v002_test | 0.53s · built · 542.69 KB | 0.70s · built · 655.59 KB |
| jac313_run_identity_test | 2.05s · built · 447.81 KB | 1.62s · built · 591.45 KB |
| jac313_store_001_TS | 2.29s · built · 3.28 MB | 2.99s · built · 4.22 MB |
| jac313_store_001_XS | 2.04s · built · 3.26 MB | 3.86s · built · 4.20 MB |
| jac313_store_002_TS | 1.92s · built · 3.13 MB | 2.60s · built · 4.05 MB |
| jac313_store_002_XS | 2.49s · built · 3.11 MB | 3.29s · built · 4.03 MB |
| jac313_store_003_TS | 2.99s · built · 3.31 MB | 4.10s · built · 4.24 MB |
| jac313_store_003_XS | 2.87s · built · 3.30 MB | 2.94s · built · 4.22 MB |
| jac313_store_004_TS | 2.74s · built · 3.27 MB | 3.59s · built · 4.20 MB |
| jac313_store_004_XS | 2.08s · built · 3.25 MB | 3.74s · built · 4.17 MB |
| jac313_store_005_TS | 2.67s · built · 3.16 MB | 3.05s · built · 4.08 MB |
| jac313_store_005_XS | 1.94s · built · 3.14 MB | 3.46s · built · 4.07 MB |
| jac313_store_006_TS | 2.21s · built · 3.31 MB | 3.14s · built · 4.24 MB |
| jac313_store_006_XS | 2.82s · built · 3.30 MB | 4.02s · built · 4.22 MB |
| jac313_store_007_TS | 2.65s · built · 3.16 MB | 3.64s · built · 4.09 MB |
| jac313_store_007_XS | 2.59s · built · 3.15 MB | 2.68s · built · 4.07 MB |
| jac313_store_008_TS | 2.51s · built · 2.91 MB | 3.29s · built · 3.78 MB |
| jac313_store_008_XS | 2.53s · built · 2.91 MB | 3.22s · built · 3.78 MB |
| jac313_store_bench | 3.00s · built · 3.61 MB | 4.10s · built · 4.62 MB |
| jac313_store_binary_persist_smoke | 2.28s · built · 1.35 MB | 2.77s · built · 1.79 MB |
| jac313_store_binary_reader_test | 2.31s · built · 2.51 MB | 2.76s · built · 3.31 MB |
| jac313_store_binary_to_jtext_smoke | 2.26s · built · 2.48 MB | 2.09s · built · 3.28 MB |
| jac313_store_flag_routing_smoke | 2.29s · built · 1.77 MB | 2.76s · built · 2.34 MB |
| jac313_store_flags | 0.66s · built · 194.16 KB | 1.11s · built · 240.30 KB |
| jac313_store_jtext_persist_smoke | 1.68s · built · 2.12 MB | 1.98s · built · 2.78 MB |
| jac313_store_metric_view | 1.80s · built · 728.89 KB | 1.86s · built · 1.03 MB |
| jac313_store_metric_view_test | 1.46s · built · 715.70 KB | 2.01s · built · 1.01 MB |
| jac313_store_saturating_capacity_test | 0.91s · built · 30.14 KB | 0.77s · built · 51.07 KB |
| jac313_store_sql_persist_smoke | 1.86s · built · 836.41 KB | 2.18s · built · 1.05 MB |
| jac313_store_v002_test | 1.09s · built · 430.91 KB | 1.30s · built · 583.62 KB |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
