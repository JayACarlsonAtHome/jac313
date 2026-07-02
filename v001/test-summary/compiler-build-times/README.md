<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# Compiler build times

_Generated from `results.db`. Compile time is hardware-specific, so each machine (`jac313-###`) is its own section. Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k

| test | clang21·hdr | clang21·mod | gcc15·hdr | gcc15·mod | gcc15·istd |
|---|:--:|:--:|:--:|:--:|:--:|
| jac313_jtext_v001_parser_validator_test | 0.95s · built · 2.08 MB | - | 1.37s · built · 2.65 MB | - | - |
| jac313_jtext_v001_test | 0.70s · built · 1.34 MB | - | 0.59s · built · 1.62 MB | - | - |
| jac313_qlite_v001_test | 0.70s · built · 538.59 KB | - | 0.56s · built · 655.13 KB | - | - |
| jac313_store_001_TS | - | 2.57s · built · 3.23 MB | - | 3.79s · built · 4.06 MB | 3.99s · built · 4.06 MB |
| jac313_store_001_XS | - | 2.50s · built · 3.21 MB | - | 3.55s · built · 4.04 MB | 3.90s · built · 4.04 MB |
| jac313_store_002_TS | - | 2.21s · built · 3.06 MB | - | 2.98s · built · 3.87 MB | 2.96s · built · 3.87 MB |
| jac313_store_002_XS | - | 3.29s · built · 3.05 MB | - | 2.84s · built · 3.85 MB | 3.01s · built · 3.85 MB |
| jac313_store_003_TS | - | 3.25s · built · 3.26 MB | - | 4.02s · built · 4.08 MB | 4.61s · built · 4.08 MB |
| jac313_store_003_XS | - | 2.82s · built · 3.25 MB | - | 4.25s · built · 4.07 MB | 3.49s · built · 4.07 MB |
| jac313_store_004_TS | - | 2.30s · built · 3.22 MB | - | 3.31s · built · 4.04 MB | 3.11s · built · 4.04 MB |
| jac313_store_004_XS | - | 2.24s · built · 3.20 MB | - | 3.26s · built · 4.01 MB | 3.38s · built · 4.01 MB |
| jac313_store_005_TS | - | 2.06s · built · 3.10 MB | - | 3.23s · built · 3.91 MB | 3.19s · built · 3.91 MB |
| jac313_store_005_XS | - | 3.16s · built · 3.08 MB | - | 4.16s · built · 3.89 MB | 3.87s · built · 3.89 MB |
| jac313_store_006_TS | - | 3.06s · built · 3.26 MB | - | 3.42s · built · 4.08 MB | 3.69s · built · 4.08 MB |
| jac313_store_006_XS | - | 2.41s · built · 3.25 MB | - | 3.21s · built · 4.07 MB | 3.46s · built · 4.07 MB |
| jac313_store_007_TS | - | 2.09s · built · 3.10 MB | - | 3.58s · built · 3.91 MB | 2.96s · built · 3.91 MB |
| jac313_store_007_XS | - | 2.04s · built · 3.08 MB | - | 3.12s · built · 3.89 MB | 3.18s · built · 3.89 MB |
| jac313_store_008_TS | - | 2.32s · built · 2.85 MB | - | 2.89s · built · 3.60 MB | 3.35s · built · 3.60 MB |
| jac313_store_008_XS | - | 1.96s · built · 2.85 MB | - | 3.01s · built · 3.60 MB | 2.91s · built · 3.60 MB |
| jac313_store_bench | - | 2.45s · built · 3.52 MB | - | 3.90s · built · 4.43 MB | 3.80s · built · 4.43 MB |
| jac313_store_flags | - | 0.69s · built · 194.67 KB | - | 0.90s · built · 239.98 KB | 1.08s · built · 239.98 KB |
| jac313_store_metric_view | - | 2.13s · built · 719.30 KB | - | 2.08s · built · 888.86 KB | 1.82s · built · 888.86 KB |
| jac313_store_v001_test | 1.30s · built · 423.98 KB | - | 1.05s · built · 575.74 KB | - | - |

## jac313-002 — Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz · 8 cores · 31 GB · Linux Mint 22.3 · ssd

| test | clang20·hdr | clang20·mod | gcc15·hdr | gcc15·mod | gcc15·istd |
|---|:--:|:--:|:--:|:--:|:--:|
| jac313_identity_dedup_test | 5.62s · built · 538.49 KB | - | 6.03s · built · 675.48 KB | - | - |
| jac313_jtext_v001_parser_validator_test | 3.47s · built · 2.12 MB | - | 2.63s · built · 2.69 MB | - | - |
| jac313_jtext_v001_test | 3.08s · built · 1.33 MB | - | 2.87s · built · 1.63 MB | - | - |
| jac313_qlite_v001_test | 2.31s · built · 547.71 KB | - | 3.01s · built · 671.06 KB | - | - |
| jac313_run_identity_test | 6.64s · built · 457.52 KB | - | 6.72s · built · 578.58 KB | - | - |
| jac313_store_001_TS | 8.67s · built · 3.25 MB | 9.29s · built · 3.25 MB | 9.48s · built · 4.10 MB | 9.59s · built · 4.10 MB | 10.70s · built · 4.10 MB |
| jac313_store_001_XS | 9.35s · built · 3.23 MB | 9.02s · built · 3.23 MB | 9.86s · built · 4.08 MB | 10.03s · built · 4.08 MB | 10.63s · built · 4.08 MB |
| jac313_store_002_TS | 8.75s · built · 3.08 MB | 8.66s · built · 3.08 MB | 9.42s · built · 3.90 MB | 9.79s · built · 3.90 MB | 9.25s · built · 3.90 MB |
| jac313_store_002_XS | 8.98s · built · 3.07 MB | 9.08s · built · 3.07 MB | 9.08s · built · 3.88 MB | 9.36s · built · 3.88 MB | 10.29s · built · 3.88 MB |
| jac313_store_003_TS | 10.05s · built · 3.28 MB | 9.83s · built · 3.28 MB | 9.01s · built · 4.12 MB | 10.22s · built · 4.12 MB | 10.24s · built · 4.12 MB |
| jac313_store_003_XS | 9.42s · built · 3.27 MB | 9.05s · built · 3.27 MB | 9.74s · built · 4.10 MB | 9.67s · built · 4.10 MB | 10.21s · built · 4.10 MB |
| jac313_store_004_TS | 8.65s · built · 3.24 MB | 9.59s · built · 3.24 MB | 10.21s · built · 4.07 MB | 10.55s · built · 4.08 MB | 10.35s · built · 4.08 MB |
| jac313_store_004_XS | 9.84s · built · 3.22 MB | 9.76s · built · 3.22 MB | 9.17s · built · 4.05 MB | 10.35s · built · 4.05 MB | 10.36s · built · 4.05 MB |
| jac313_store_005_TS | 9.70s · built · 3.12 MB | 9.40s · built · 3.12 MB | 9.59s · built · 3.94 MB | 9.65s · built · 3.94 MB | 9.30s · built · 3.94 MB |
| jac313_store_005_XS | 8.36s · built · 3.10 MB | 7.74s · built · 3.10 MB | 9.90s · built · 3.92 MB | 8.45s · built · 3.92 MB | 8.28s · built · 3.92 MB |
| jac313_store_006_TS | 10.50s · built · 3.28 MB | 8.10s · built · 3.28 MB | 11.72s · built · 4.12 MB | 8.80s · built · 4.12 MB | 8.42s · built · 4.12 MB |
| jac313_store_006_XS | 10.49s · built · 3.27 MB | 7.18s · built · 3.27 MB | 9.68s · built · 4.10 MB | 8.73s · built · 4.10 MB | 8.64s · built · 4.10 MB |
| jac313_store_007_TS | 9.97s · built · 3.12 MB | 7.33s · built · 3.12 MB | 11.23s · built · 3.94 MB | 8.34s · built · 3.94 MB | 7.86s · built · 3.94 MB |
| jac313_store_007_XS | 9.56s · built · 3.10 MB | 6.78s · built · 3.10 MB | 9.49s · built · 3.92 MB | 7.44s · built · 3.92 MB | 7.11s · built · 3.92 MB |
| jac313_store_008_TS | 9.95s · built · 2.87 MB | 6.46s · built · 2.87 MB | 7.26s · built · 3.63 MB | 6.67s · built · 3.63 MB | 6.27s · built · 3.63 MB |
| jac313_store_008_XS | 7.93s · built · 2.87 MB | 6.36s · built · 2.87 MB | 8.75s · built · 3.63 MB | 5.80s · built · 3.63 MB | 6.01s · built · 3.63 MB |
| jac313_store_bench | 9.81s · built · 3.55 MB | 9.95s · built · 3.55 MB | 10.88s · built · 4.47 MB | 11.28s · built · 4.47 MB | 11.07s · built · 4.47 MB |
| jac313_store_binary_persist_smoke | 6.71s · built · 1.30 MB | - | 7.64s · built · 1.61 MB | - | - |
| jac313_store_binary_reader_test | 7.08s · built · 2.44 MB | - | 6.76s · built · 3.12 MB | - | - |
| jac313_store_binary_to_jtext_smoke | 6.85s · built · 2.43 MB | - | 6.58s · built · 3.10 MB | - | - |
| jac313_store_flag_routing_smoke | 7.86s · built · 1.73 MB | - | 8.13s · built · 2.17 MB | - | - |
| jac313_store_flags | 3.16s · built · 200.90 KB | 3.52s · built · 200.91 KB | 2.54s · built · 243.09 KB | 3.29s · built · 243.38 KB | 2.71s · built · 243.38 KB |
| jac313_store_jtext_persist_smoke | 6.05s · built · 2.08 MB | - | 4.92s · built · 2.61 MB | - | - |
| jac313_store_metric_view | 6.44s · built · 725.02 KB | 6.57s · built · 725.03 KB | 5.59s · built · 908.62 KB | 5.66s · built · 908.95 KB | 5.90s · built · 908.95 KB |
| jac313_store_metric_view_test | 5.78s · built · 716.14 KB | - | 5.20s · built · 893.05 KB | - | - |
| jac313_store_sql_persist_smoke | 6.22s · built · 855.84 KB | - | 4.46s · built · 1.04 MB | - | - |
| jac313_store_v001_test | 6.24s · built · 430.41 KB | - | 4.07s · built · 587.63 KB | - | - |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
