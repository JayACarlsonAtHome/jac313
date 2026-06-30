# Compiler build times

_Generated from `results.db`. Compile time is hardware-specific, so each machine (`jac313-###`) is its own section. Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 12 cores · 38 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k

| test | clang21·hdr | clang21·mod | gcc15·hdr | gcc15·mod | gcc15·istd |
|---|:--:|:--:|:--:|:--:|:--:|
| jac313_identity_dedup_test | 1.65s · built · 529.13 KB | - | 1.99s · built · 668.22 KB | - | - |
| jac313_jtext_v001_parser_validator_test | 0.87s · built · 2.08 MB | - | 1.06s · built · 2.65 MB | - | - |
| jac313_jtext_v001_test | 1.13s · built · 1.34 MB | - | 0.78s · built · 1.62 MB | - | - |
| jac313_qlite_v001_test | 1.17s · built · 493.07 KB | - | 0.71s · built · 605.12 KB | - | - |
| jac313_run_identity_test | 1.90s · built · 444.98 KB | - | 1.82s · built · 570.98 KB | - | - |
| jac313_store_001_TS | 2.29s · built · 3.22 MB | 2.18s · built · 3.22 MB | 4.59s · built · 4.06 MB | 3.38s · built · 4.06 MB | 3.47s · built · 4.06 MB |
| jac313_store_001_XS | 2.95s · built · 3.21 MB | 2.64s · built · 3.21 MB | 3.32s · built · 4.04 MB | 3.25s · built · 4.04 MB | 3.73s · built · 4.04 MB |
| jac313_store_002_TS | 2.86s · built · 3.06 MB | 2.27s · built · 3.06 MB | 4.25s · built · 3.87 MB | 3.72s · built · 3.87 MB | 3.35s · built · 3.87 MB |
| jac313_store_002_XS | 2.29s · built · 3.05 MB | 2.42s · built · 3.05 MB | 4.49s · built · 3.85 MB | 3.99s · built · 3.85 MB | 2.87s · built · 3.85 MB |
| jac313_store_003_TS | 3.14s · built · 3.26 MB | 2.69s · built · 3.26 MB | 3.99s · built · 4.08 MB | 3.02s · built · 4.08 MB | 3.75s · built · 4.08 MB |
| jac313_store_003_XS | 2.33s · built · 3.25 MB | 2.04s · built · 3.25 MB | 3.14s · built · 4.06 MB | 3.58s · built · 4.06 MB | 3.22s · built · 4.06 MB |
| jac313_store_004_TS | 2.94s · built · 3.21 MB | 2.77s · built · 3.21 MB | 3.05s · built · 4.04 MB | 3.93s · built · 4.04 MB | 3.86s · built · 4.04 MB |
| jac313_store_004_XS | 2.32s · built · 3.20 MB | 2.57s · built · 3.20 MB | 3.64s · built · 4.01 MB | 3.23s · built · 4.01 MB | 2.96s · built · 4.01 MB |
| jac313_store_005_TS | 1.99s · built · 3.09 MB | 2.27s · built · 3.09 MB | 3.83s · built · 3.90 MB | 3.00s · built · 3.90 MB | 3.72s · built · 3.90 MB |
| jac313_store_005_XS | 2.14s · built · 3.08 MB | 2.36s · built · 3.08 MB | 2.90s · built · 3.88 MB | 3.19s · built · 3.88 MB | 3.52s · built · 3.88 MB |
| jac313_store_006_TS | 2.74s · built · 3.26 MB | 2.78s · built · 3.26 MB | 4.26s · built · 4.08 MB | 3.47s · built · 4.08 MB | 3.02s · built · 4.08 MB |
| jac313_store_006_XS | 2.71s · built · 3.25 MB | 2.61s · built · 3.25 MB | 3.60s · built · 4.06 MB | 3.27s · built · 4.06 MB | 3.73s · built · 4.06 MB |
| jac313_store_007_TS | 2.50s · built · 3.09 MB | 2.61s · built · 3.09 MB | 4.08s · built · 3.91 MB | 3.12s · built · 3.91 MB | 2.94s · built · 3.91 MB |
| jac313_store_007_XS | 2.84s · built · 3.08 MB | 2.00s · built · 3.08 MB | 3.65s · built · 3.89 MB | 3.16s · built · 3.89 MB | 2.79s · built · 3.89 MB |
| jac313_store_008_TS | 2.48s · built · 2.85 MB | 1.97s · built · 2.85 MB | 3.31s · built · 3.59 MB | 2.47s · built · 3.59 MB | 2.43s · built · 3.59 MB |
| jac313_store_008_XS | 2.04s · built · 2.85 MB | 1.83s · built · 2.85 MB | 2.55s · built · 3.59 MB | 2.44s · built · 3.59 MB | 2.35s · built · 3.59 MB |
| jac313_store_bench | 2.70s · built · 3.50 MB | 2.78s · built · 3.50 MB | 3.50s · built · 4.42 MB | 4.86s · built · 4.42 MB | 4.85s · built · 4.42 MB |
| jac313_store_binary_persist_smoke | 2.01s · built · 1.30 MB | - | 2.56s · built · 1.61 MB | - | - |
| jac313_store_binary_reader_test | 1.59s · built · 2.43 MB | - | 2.31s · built · 3.10 MB | - | - |
| jac313_store_binary_to_jtext_smoke | 1.65s · built · 2.41 MB | - | 2.72s · built · 3.08 MB | - | - |
| jac313_store_flag_routing_smoke | 2.21s · built · 1.72 MB | - | 2.71s · built · 2.16 MB | - | - |
| jac313_store_flags | 1.28s · built · 194.66 KB | 0.64s · built · 194.67 KB | 0.91s · built · 239.69 KB | 0.97s · built · 239.98 KB | 0.97s · built · 239.98 KB |
| jac313_store_jtext_persist_smoke | 1.87s · built · 2.08 MB | - | 1.53s · built · 2.60 MB | - | - |
| jac313_store_metric_view | 1.35s · built · 719.29 KB | 1.37s · built · 719.30 KB | 1.75s · built · 888.49 KB | 1.84s · built · 888.82 KB | 1.88s · built · 888.82 KB |
| jac313_store_metric_view_test | 1.43s · built · 705.87 KB | - | 1.67s · built · 872.90 KB | - | - |
| jac313_store_sql_persist_smoke | 1.77s · built · 829.38 KB | - | 1.82s · built · 1.03 MB | - | - |
| jac313_store_v001_test | 1.16s · built · 422.07 KB | - | 1.43s · built · 568.10 KB | - | - |
