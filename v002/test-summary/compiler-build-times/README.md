# Compiler build times

_Generated from `results.db`. Compile time is hardware-specific, so each machine (`jac313-###`) is its own section. Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._

## jac313-001 — Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz · 8 cores · 31 GB · Linux Mint 22.3 · ssd

| test | clang20·hdr | clang20·mod | clang20·istd | gcc15·hdr | gcc15·mod | gcc15·istd |
|---|:--:|:--:|:--:|:--:|:--:|:--:|
| jac313_contracts_disabled_test | 2.60s · built · 31.88 KB | - | - | 1.73s · built · 49.82 KB | - | - |
| jac313_contracts_enforce_test | 2.38s · built · 528.20 KB | - | - | 1.85s · built · 670.59 KB | - | - |
| jac313_contracts_test | 2.21s · built · 34.48 KB | - | - | 1.81s · built · 52.54 KB | - | - |
| jac313_identity_dedup_test | 6.42s · built · 538.77 KB | - | - | 5.96s · built · 690.07 KB | - | - |
| jac313_jtext_v002_parser_validator_test | 3.99s · built · 2.12 MB | - | - | 3.47s · built · 2.72 MB | - | - |
| jac313_jtext_v002_test | 3.15s · built · 1.33 MB | - | - | 2.72s · built · 1.65 MB | - | - |
| jac313_qlite_v002_test | 2.67s · built · 497.45 KB | - | - | 2.94s · built · 616.88 KB | - | - |
| jac313_run_identity_test | 6.86s · built · 457.66 KB | - | - | 5.43s · built · 598.82 KB | - | - |
| jac313_store_001_TS | 9.55s · built · 3.23 MB | 10.17s · built · 3.23 MB | 8.88s · built · 3.23 MB | 8.40s · built · 4.10 MB | 11.65s · built · 4.10 MB | 10.24s · built · 4.10 MB |
| jac313_store_001_XS | 8.40s · built · 3.22 MB | 10.46s · built · 3.22 MB | 9.86s · built · 3.22 MB | 9.87s · built · 4.08 MB | 11.77s · built · 4.09 MB | 9.79s · built · 4.09 MB |
| jac313_store_002_TS | 9.17s · built · 3.08 MB | 9.96s · built · 3.08 MB | 8.87s · built · 3.08 MB | 9.66s · built · 3.93 MB | 10.94s · built · 3.93 MB | 10.66s · built · 3.93 MB |
| jac313_store_002_XS | 8.80s · built · 3.06 MB | 8.20s · built · 3.06 MB | 9.34s · built · 3.06 MB | 9.39s · built · 3.91 MB | 9.51s · built · 3.91 MB | 9.57s · built · 3.91 MB |
| jac313_store_003_TS | 10.14s · built · 3.27 MB | 9.26s · built · 3.27 MB | 10.64s · built · 3.27 MB | 9.63s · built · 4.13 MB | 11.17s · built · 4.13 MB | 11.13s · built · 4.13 MB |
| jac313_store_003_XS | 8.58s · built · 3.25 MB | 9.68s · built · 3.25 MB | 9.86s · built · 3.25 MB | 9.59s · built · 4.11 MB | 11.23s · built · 4.11 MB | 11.12s · built · 4.11 MB |
| jac313_store_004_TS | 10.07s · built · 3.22 MB | 9.28s · built · 3.22 MB | 9.90s · built · 3.22 MB | 9.84s · built · 4.09 MB | 12.79s · built · 4.09 MB | 11.16s · built · 4.09 MB |
| jac313_store_004_XS | 9.84s · built · 3.20 MB | 9.62s · built · 3.20 MB | 9.27s · built · 3.20 MB | 9.76s · built · 4.06 MB | 10.54s · built · 4.06 MB | 11.04s · built · 4.06 MB |
| jac313_store_005_TS | 9.66s · built · 3.11 MB | 9.61s · built · 3.11 MB | 8.31s · built · 3.11 MB | 10.22s · built · 3.97 MB | 11.08s · built · 3.97 MB | 9.43s · built · 3.97 MB |
| jac313_store_005_XS | 8.78s · built · 3.09 MB | 8.14s · built · 3.09 MB | 8.00s · built · 3.09 MB | 9.47s · built · 3.95 MB | 8.85s · built · 3.95 MB | 9.67s · built · 3.95 MB |
| jac313_store_006_TS | 9.98s · built · 3.27 MB | 8.44s · built · 3.27 MB | 8.29s · built · 3.27 MB | 9.45s · built · 4.13 MB | 8.47s · built · 4.13 MB | 9.52s · built · 4.13 MB |
| jac313_store_006_XS | 9.34s · built · 3.25 MB | 7.73s · built · 3.25 MB | 7.96s · built · 3.25 MB | 8.66s · built · 4.11 MB | 9.41s · built · 4.11 MB | 8.61s · built · 4.11 MB |
| jac313_store_007_TS | 7.82s · built · 3.11 MB | 6.44s · built · 3.11 MB | 7.37s · built · 3.11 MB | 9.13s · built · 3.97 MB | 9.19s · built · 3.97 MB | 8.89s · built · 3.97 MB |
| jac313_store_007_XS | 8.52s · built · 3.10 MB | 7.38s · built · 3.10 MB | 6.29s · built · 3.10 MB | 8.42s · built · 3.95 MB | 8.65s · built · 3.95 MB | 8.80s · built · 3.95 MB |
| jac313_store_008_TS | 8.52s · built · 2.87 MB | 6.76s · built · 2.87 MB | 6.23s · built · 2.87 MB | 8.37s · built · 3.66 MB | 6.85s · built · 3.66 MB | 6.84s · built · 3.66 MB |
| jac313_store_008_XS | 8.08s · built · 2.86 MB | 5.99s · built · 2.86 MB | 5.96s · built · 2.86 MB | 7.73s · built · 3.66 MB | 6.13s · built · 3.66 MB | 6.54s · built · 3.66 MB |
| jac313_store_bench | 10.37s · built · 3.42 MB | 9.46s · built · 3.42 MB | 10.49s · built · 3.42 MB | 5.33s · built · 4.38 MB | 9.88s · built · 4.38 MB | 12.03s · built · 4.38 MB |
| jac313_store_binary_persist_smoke | 7.56s · built · 1.31 MB | - | - | 7.15s · built · 1.64 MB | - | - |
| jac313_store_binary_reader_test | 7.58s · built · 2.46 MB | - | - | 3.84s · built · 3.17 MB | - | - |
| jac313_store_binary_to_jtext_smoke | 7.73s · built · 2.44 MB | - | - | 7.08s · built · 3.14 MB | - | - |
| jac313_store_flag_routing_smoke | 7.95s · built · 1.74 MB | - | - | 8.00s · built · 2.20 MB | - | - |
| jac313_store_flags | 3.08s · built · 200.99 KB | 3.35s · built · 201.01 KB | 3.23s · built · 201.00 KB | 2.60s · built · 243.38 KB | 3.12s · built · 243.68 KB | 2.87s · built · 243.68 KB |
| jac313_store_jtext_persist_smoke | 6.39s · built · 2.08 MB | - | - | 4.58s · built · 2.64 MB | - | - |
| jac313_store_metric_view | 6.68s · built · 725.92 KB | 6.18s · built · 725.94 KB | 5.61s · built · 725.94 KB | 5.24s · built · 922.64 KB | 7.39s · built · 922.97 KB | 6.74s · built · 922.97 KB |
| jac313_store_metric_view_test | 5.94s · built · 717.05 KB | - | - | 3.38s · built · 907.07 KB | - | - |
| jac313_store_saturating_capacity_test | 2.82s · built · 35.12 KB | - | - | 1.58s · built · 54.12 KB | - | - |
| jac313_store_sql_persist_smoke | 6.14s · built · 856.02 KB | - | - | 5.42s · built · 1.06 MB | - | - |
| jac313_store_v002_test | 5.16s · built · 429.85 KB | - | - | 2.90s · built · 586.98 KB | - | - |
