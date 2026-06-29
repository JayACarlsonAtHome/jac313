# Compiler build times

_Generated from `results.db`. Compile time is hardware-specific, so each machine (`jac313-###`) is its own section. Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 12 cores · 23 GB · Fedora Linux 44 (Workstation Edition) · x7k

| test | clang22·hdr | clang22·mod | clang22·istd | gcc16·hdr | gcc16·mod | gcc16·istd |
|---|:--:|:--:|:--:|:--:|:--:|:--:|
| jac313_contracts_disabled_test | 0.84s · built · 29.96 KB | - | - | 0.95s · built · 47.55 KB | - | - |
| jac313_contracts_enforce_test | 1.25s · built · 561.69 KB | - | - | 1.14s · built · 730.27 KB | - | - |
| jac313_contracts_test | 0.83s · built · 32.53 KB | - | - | 0.63s · built · 87.05 KB | - | - |
| jac313_identity_dedup_test | 2.51s · built · 567.21 KB | - | - | 1.89s · built · 681.16 KB | - | - |
| jac313_jtext_v002_parser_validator_test | 1.12s · built · 2.23 MB | - | - | 0.85s · built · 2.75 MB | - | - |
| jac313_jtext_v002_test | 1.14s · built · 1.43 MB | - | - | 1.11s · built · 1.68 MB | - | - |
| jac313_qlite_v002_test | 0.78s · built · 532.58 KB | - | - | 0.68s · built · 590.18 KB | - | - |
| jac313_run_identity_test | 2.07s · built · 467.34 KB | - | - | 1.99s · built · 591.10 KB | - | - |
| jac313_store_001_TS | 2.54s · built · 3.43 MB | 2.83s · built · 3.43 MB | 2.82s · built · 3.43 MB | 3.14s · built · 4.14 MB | 3.73s · built · 4.14 MB | 4.63s · built · 4.14 MB |
| jac313_store_001_XS | 3.04s · built · 3.42 MB | 3.03s · built · 3.42 MB | 3.41s · built · 3.42 MB | 4.38s · built · 4.12 MB | 3.46s · built · 4.12 MB | 3.27s · built · 4.12 MB |
| jac313_store_002_TS | 3.50s · built · 3.28 MB | 3.07s · built · 3.28 MB | 2.62s · built · 3.28 MB | 3.33s · built · 3.97 MB | 3.53s · built · 3.98 MB | 3.47s · built · 3.98 MB |
| jac313_store_002_XS | 2.78s · built · 3.26 MB | 3.44s · built · 3.26 MB | 2.77s · built · 3.26 MB | 3.29s · built · 3.95 MB | 3.64s · built · 3.95 MB | 4.10s · built · 3.95 MB |
| jac313_store_003_TS | 3.02s · built · 3.47 MB | 2.58s · built · 3.47 MB | 2.66s · built · 3.47 MB | 3.56s · built · 4.16 MB | 3.86s · built · 4.16 MB | 4.50s · built · 4.16 MB |
| jac313_store_003_XS | 2.93s · built · 3.45 MB | 3.52s · built · 3.45 MB | 3.06s · built · 3.45 MB | 3.35s · built · 4.14 MB | 3.35s · built · 4.14 MB | 3.03s · built · 4.14 MB |
| jac313_store_004_TS | 2.50s · built · 3.42 MB | 3.61s · built · 3.42 MB | 2.81s · built · 3.42 MB | 3.56s · built · 4.12 MB | 3.99s · built · 4.12 MB | 3.38s · built · 4.12 MB |
| jac313_store_004_XS | 2.92s · built · 3.40 MB | 2.36s · built · 3.40 MB | 2.69s · built · 3.40 MB | 3.92s · built · 4.09 MB | 3.66s · built · 4.09 MB | 3.89s · built · 4.09 MB |
| jac313_store_005_TS | 2.96s · built · 3.31 MB | 3.13s · built · 3.31 MB | 2.39s · built · 3.31 MB | 3.00s · built · 4.01 MB | 3.31s · built · 4.01 MB | 4.17s · built · 4.01 MB |
| jac313_store_005_XS | 3.05s · built · 3.29 MB | 2.82s · built · 3.29 MB | 2.92s · built · 3.29 MB | 3.22s · built · 3.99 MB | 3.22s · built · 3.99 MB | 3.69s · built · 3.99 MB |
| jac313_store_006_TS | 3.06s · built · 3.47 MB | 2.83s · built · 3.47 MB | 2.70s · built · 3.47 MB | 3.77s · built · 4.16 MB | 3.42s · built · 4.16 MB | 3.27s · built · 4.16 MB |
| jac313_store_006_XS | 2.55s · built · 3.45 MB | 2.78s · built · 3.45 MB | 3.12s · built · 3.45 MB | 4.04s · built · 4.14 MB | 3.71s · built · 4.14 MB | 3.34s · built · 4.14 MB |
| jac313_store_007_TS | 3.67s · built · 3.31 MB | 2.48s · built · 3.31 MB | 2.81s · built · 3.31 MB | 3.60s · built · 4.01 MB | 3.10s · built · 4.01 MB | 3.17s · built · 4.01 MB |
| jac313_store_007_XS | 4.22s · built · 3.29 MB | 2.40s · built · 3.29 MB | 2.21s · built · 3.29 MB | 3.15s · built · 3.99 MB | 2.85s · built · 3.99 MB | 2.82s · built · 3.99 MB |
| jac313_store_008_TS | 2.40s · built · 3.06 MB | 2.19s · built · 3.06 MB | 2.25s · built · 3.06 MB | 3.05s · built · 3.70 MB | 2.62s · built · 3.70 MB | 2.54s · built · 3.70 MB |
| jac313_store_008_XS | 2.54s · built · 3.06 MB | 2.15s · built · 3.06 MB | 2.09s · built · 3.06 MB | 2.78s · built · 3.70 MB | 2.54s · built · 3.70 MB | 2.44s · built · 3.70 MB |
| jac313_store_bench | 2.92s · built · 3.74 MB | 3.33s · built · 3.74 MB | 3.63s · built · 3.74 MB | 3.68s · built · 4.52 MB | 3.93s · built · 4.53 MB | 3.95s · built · 4.53 MB |
| jac313_store_binary_persist_smoke | 2.09s · built · 1.40 MB | - | - | 2.79s · built · 1.66 MB | - | - |
| jac313_store_binary_reader_test | 1.88s · built · 2.61 MB | - | - | 3.19s · built · 3.22 MB | - | - |
| jac313_store_binary_to_jtext_smoke | 3.53s · built · 2.59 MB | - | - | 2.46s · built · 3.19 MB | - | - |
| jac313_store_flag_routing_smoke | 2.61s · built · 1.86 MB | - | - | 2.71s · built · 2.20 MB | - | - |
| jac313_store_flags | 1.33s · built · 202.70 KB | 1.24s · built · 202.72 KB | 1.68s · built · 202.72 KB | 0.80s · built · 235.80 KB | 1.04s · built · 236.09 KB | 1.22s · built · 236.09 KB |
| jac313_store_jtext_persist_smoke | 1.63s · built · 2.22 MB | - | - | 2.09s · built · 2.68 MB | - | - |
| jac313_store_metric_view | 2.37s · built · 774.52 KB | 2.02s · built · 774.53 KB | 2.34s · built · 774.52 KB | 1.78s · built · 913.06 KB | 2.25s · built · 913.39 KB | 2.04s · built · 913.39 KB |
| jac313_store_metric_view_test | 2.13s · built · 766.18 KB | - | - | 2.97s · built · 900.91 KB | - | - |
| jac313_store_saturating_capacity_test | 1.41s · built · 33.16 KB | - | - | 0.89s · built · 51.75 KB | - | - |
| jac313_store_sql_persist_smoke | 1.82s · built · 898.55 KB | - | - | 2.03s · built · 1.04 MB | - | - |
| jac313_store_v002_test | 1.65s · built · 455.46 KB | - | - | 1.91s · built · 562.25 KB | - | - |
