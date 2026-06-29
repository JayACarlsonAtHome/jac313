# Compiler build times

_Generated from `results.db`. Compile time is hardware-specific, so each machine (`jac313-###`) is its own section. Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 20 cores · 62 GB · Red Hat Enterprise Linux 9.8 (Plow) · ssd

| test | clang21·hdr | clang21·mod | clang21·istd | gcc15·hdr | gcc15·mod | gcc15·istd |
|---|:--:|:--:|:--:|:--:|:--:|:--:|
| jac313_contracts_disabled_test | 0.66s · built · 27.12 KB | - | - | 0.62s · built · 46.78 KB | - | - |
| jac313_contracts_enforce_test | 0.87s · built · 531.55 KB | - | - | 0.98s · built · 806.74 KB | - | - |
| jac313_contracts_test | 0.83s · built · 29.54 KB | - | - | 0.61s · built · 49.54 KB | - | - |
| jac313_identity_dedup_test | 1.98s · built · 530.92 KB | - | - | 2.53s · built · 683.05 KB | - | - |
| jac313_jtext_v002_parser_validator_test | 0.76s · built · 2.12 MB | - | - | 0.79s · built · 2.82 MB | - | - |
| jac313_jtext_v002_test | 0.72s · built · 1.36 MB | - | - | 1.31s · built · 1.79 MB | - | - |
| jac313_qlite_v002_test | 0.81s · built · 495.92 KB | - | - | 1.16s · built · 605.45 KB | - | - |
| jac313_run_identity_test | 1.83s · built · 447.81 KB | - | - | 2.42s · built · 591.45 KB | - | - |
| jac313_store_001_TS | 2.20s · built · 3.28 MB | 2.18s · built · 3.28 MB | 2.26s · built · 3.28 MB | 3.79s · built · 4.22 MB | 3.78s · built · 4.22 MB | 3.84s · built · 4.22 MB |
| jac313_store_001_XS | 2.72s · built · 3.26 MB | 2.58s · built · 3.26 MB | 2.26s · built · 3.26 MB | 3.79s · built · 4.20 MB | 3.23s · built · 4.20 MB | 3.13s · built · 4.20 MB |
| jac313_store_002_TS | 2.77s · built · 3.12 MB | 2.41s · built · 3.12 MB | 2.51s · built · 3.12 MB | 3.73s · built · 4.05 MB | 3.73s · built · 4.05 MB | 3.71s · built · 4.05 MB |
| jac313_store_002_XS | 3.00s · built · 3.11 MB | 2.52s · built · 3.11 MB | 2.42s · built · 3.11 MB | 3.02s · built · 4.02 MB | 3.48s · built · 4.02 MB | 3.27s · built · 4.02 MB |
| jac313_store_003_TS | 3.02s · built · 3.31 MB | 2.74s · built · 3.31 MB | 2.85s · built · 3.31 MB | 4.04s · built · 4.24 MB | 3.22s · built · 4.24 MB | 4.05s · built · 4.24 MB |
| jac313_store_003_XS | 2.07s · built · 3.30 MB | 2.08s · built · 3.30 MB | 2.71s · built · 3.30 MB | 3.26s · built · 4.22 MB | 3.34s · built · 4.22 MB | 3.68s · built · 4.22 MB |
| jac313_store_004_TS | 2.81s · built · 3.27 MB | 2.70s · built · 3.27 MB | 2.74s · built · 3.27 MB | 3.26s · built · 4.20 MB | 3.66s · built · 4.20 MB | 3.97s · built · 4.20 MB |
| jac313_store_004_XS | 2.83s · built · 3.24 MB | 2.65s · built · 3.24 MB | 2.62s · built · 3.24 MB | 3.48s · built · 4.17 MB | 3.68s · built · 4.17 MB | 3.78s · built · 4.17 MB |
| jac313_store_005_TS | 2.63s · built · 3.15 MB | 2.39s · built · 3.15 MB | 2.53s · built · 3.15 MB | 3.40s · built · 4.08 MB | 3.63s · built · 4.08 MB | 3.67s · built · 4.08 MB |
| jac313_store_005_XS | 3.03s · built · 3.14 MB | 2.14s · built · 3.14 MB | 2.86s · built · 3.14 MB | 3.77s · built · 4.06 MB | 3.36s · built · 4.07 MB | 3.13s · built · 4.07 MB |
| jac313_store_006_TS | 3.18s · built · 3.31 MB | 2.74s · built · 3.31 MB | 2.33s · built · 3.31 MB | 4.08s · built · 4.24 MB | 3.21s · built · 4.24 MB | 3.90s · built · 4.24 MB |
| jac313_store_006_XS | 2.95s · built · 3.30 MB | 2.20s · built · 3.30 MB | 2.06s · built · 3.30 MB | 4.04s · built · 4.22 MB | 3.43s · built · 4.22 MB | 3.10s · built · 4.22 MB |
| jac313_store_007_TS | 2.79s · built · 3.16 MB | 2.13s · built · 3.16 MB | 1.99s · built · 3.16 MB | 3.87s · built · 4.09 MB | 2.92s · built · 4.09 MB | 2.90s · built · 4.09 MB |
| jac313_store_007_XS | 1.83s · built · 3.14 MB | 2.29s · built · 3.14 MB | 2.16s · built · 3.14 MB | 3.85s · built · 4.07 MB | 3.27s · built · 4.07 MB | 3.19s · built · 4.07 MB |
| jac313_store_008_TS | 2.42s · built · 2.91 MB | 2.07s · built · 2.91 MB | 2.08s · built · 2.91 MB | 2.70s · built · 3.78 MB | 2.75s · built · 3.78 MB | 2.62s · built · 3.78 MB |
| jac313_store_008_XS | 1.79s · built · 2.91 MB | 2.12s · built · 2.91 MB | 1.85s · built · 2.91 MB | 3.09s · built · 3.78 MB | 2.94s · built · 3.78 MB | 2.53s · built · 3.78 MB |
| jac313_store_bench | 2.18s · built · 3.58 MB | 2.93s · built · 3.58 MB | 2.87s · built · 3.58 MB | 2.21s · built · 4.60 MB | 4.04s · built · 4.60 MB | 3.86s · built · 4.60 MB |
| jac313_store_binary_persist_smoke | 2.25s · built · 1.35 MB | - | - | 3.01s · built · 1.79 MB | - | - |
| jac313_store_binary_reader_test | 1.60s · built · 2.50 MB | - | - | 3.02s · built · 3.30 MB | - | - |
| jac313_store_binary_to_jtext_smoke | 2.07s · built · 2.48 MB | - | - | 3.05s · built · 3.28 MB | - | - |
| jac313_store_flag_routing_smoke | 2.49s · built · 1.77 MB | - | - | 3.17s · built · 2.34 MB | - | - |
| jac313_store_flags | 0.84s · built · 194.16 KB | 1.03s · built · 194.17 KB | 0.88s · built · 194.16 KB | 0.85s · built · 240.30 KB | 1.09s · built · 240.60 KB | 1.26s · built · 240.60 KB |
| jac313_store_jtext_persist_smoke | 1.67s · built · 2.12 MB | - | - | 1.82s · built · 2.78 MB | - | - |
| jac313_store_metric_view | 1.80s · built · 728.89 KB | 1.64s · built · 728.90 KB | 1.54s · built · 728.90 KB | 2.33s · built · 1.03 MB | 2.15s · built · 1.03 MB | 2.47s · built · 1.03 MB |
| jac313_store_metric_view_test | 1.34s · built · 715.70 KB | - | - | 1.74s · built · 1.01 MB | - | - |
| jac313_store_saturating_capacity_test | 0.55s · built · 30.14 KB | - | - | 0.82s · built · 51.07 KB | - | - |
| jac313_store_sql_persist_smoke | 1.80s · built · 836.13 KB | - | - | 2.23s · built · 1.05 MB | - | - |
| jac313_store_v002_test | 1.25s · built · 424.84 KB | - | - | 1.32s · built · 575.32 KB | - | - |
