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

| test | clang22·hdr | clang22·mod | gcc16·hdr | gcc16·mod | gcc16·istd |
|---|:--:|:--:|:--:|:--:|:--:|
| jac313_jtext_v001_parser_validator_test | 1.63s · built · 2.22 MB | - | 1.61s · built · 2.68 MB | - | - |
| jac313_jtext_v001_test | 0.94s · built · 1.42 MB | - | 1.16s · built · 1.64 MB | - | - |
| jac313_qlite_v001_test | 0.87s · built · 580.16 KB | - | 0.83s · built · 638.92 KB | - | - |
| jac313_store_001_TS | - | 2.79s · built · 3.44 MB | - | 3.51s · built · 4.07 MB | 4.24s · built · 4.07 MB |
| jac313_store_001_XS | - | 2.80s · built · 3.43 MB | - | 3.50s · built · 4.05 MB | 3.87s · built · 4.05 MB |
| jac313_store_002_TS | - | 2.90s · built · 3.27 MB | - | 3.28s · built · 3.89 MB | 2.98s · built · 3.89 MB |
| jac313_store_002_XS | - | 3.75s · built · 3.26 MB | - | 3.58s · built · 3.87 MB | 3.64s · built · 3.87 MB |
| jac313_store_003_TS | - | 4.37s · built · 3.48 MB | - | 3.91s · built · 4.09 MB | 3.61s · built · 4.09 MB |
| jac313_store_003_XS | - | 2.77s · built · 3.46 MB | - | 3.16s · built · 4.07 MB | 4.06s · built · 4.07 MB |
| jac313_store_004_TS | - | 2.89s · built · 3.43 MB | - | 3.86s · built · 4.05 MB | 4.00s · built · 4.05 MB |
| jac313_store_004_XS | - | 3.55s · built · 3.41 MB | - | 3.60s · built · 4.02 MB | 4.04s · built · 4.02 MB |
| jac313_store_005_TS | - | 2.56s · built · 3.30 MB | - | 3.60s · built · 3.92 MB | 3.60s · built · 3.92 MB |
| jac313_store_005_XS | - | 2.53s · built · 3.29 MB | - | 3.55s · built · 3.90 MB | 3.28s · built · 3.90 MB |
| jac313_store_006_TS | - | 3.46s · built · 3.48 MB | - | 3.75s · built · 4.09 MB | 3.51s · built · 4.09 MB |
| jac313_store_006_XS | - | 3.79s · built · 3.46 MB | - | 4.29s · built · 4.07 MB | 3.58s · built · 4.07 MB |
| jac313_store_007_TS | - | 2.57s · built · 3.30 MB | - | 3.81s · built · 3.93 MB | 3.28s · built · 3.93 MB |
| jac313_store_007_XS | - | 2.67s · built · 3.29 MB | - | 2.78s · built · 3.91 MB | 3.49s · built · 3.91 MB |
| jac313_store_008_TS | - | 2.91s · built · 3.05 MB | - | 3.55s · built · 3.62 MB | 3.16s · built · 3.62 MB |
| jac313_store_008_XS | - | 2.27s · built · 3.05 MB | - | 2.88s · built · 3.62 MB | 3.02s · built · 3.62 MB |
| jac313_store_bench | - | 3.33s · built · 3.76 MB | - | 4.03s · built · 4.44 MB | 3.89s · built · 4.44 MB |
| jac313_store_flags | - | 1.21s · built · 201.20 KB | - | 1.15s · built · 232.31 KB | 1.01s · built · 232.31 KB |
| jac313_store_metric_view | - | 2.48s · built · 772.17 KB | - | 2.80s · built · 888.41 KB | 2.06s · built · 888.41 KB |
| jac313_store_v001_test | 1.82s · built · 454.12 KB | - | 2.04s · built · 558.73 KB | - | - |

## jac313-002 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · rhel-10.2 · x7k

| test | clang21·hdr | clang21·mod | gcc15·hdr | gcc15·mod | gcc15·istd |
|---|:--:|:--:|:--:|:--:|:--:|
| jac313_jtext_v001_parser_validator_test | 0.92s · built · 2.08 MB | - | 1.23s · built · 2.65 MB | - | - |
| jac313_jtext_v001_test | 1.04s · built · 1.34 MB | - | 0.77s · built · 1.62 MB | - | - |
| jac313_qlite_v001_test | 0.71s · built · 538.59 KB | - | 0.71s · built · 655.13 KB | - | - |
| jac313_store_001_TS | - | 2.74s · built · 3.23 MB | - | 4.26s · built · 4.06 MB | 3.49s · built · 4.06 MB |
| jac313_store_001_XS | - | 2.72s · built · 3.21 MB | - | 3.33s · built · 4.04 MB | 3.40s · built · 4.04 MB |
| jac313_store_002_TS | - | 2.06s · built · 3.06 MB | - | 3.25s · built · 3.87 MB | 3.25s · built · 3.87 MB |
| jac313_store_002_XS | - | 2.34s · built · 3.05 MB | - | 3.36s · built · 3.85 MB | 3.62s · built · 3.85 MB |
| jac313_store_003_TS | - | 3.83s · built · 3.26 MB | - | 3.61s · built · 4.08 MB | 4.98s · built · 4.08 MB |
| jac313_store_003_XS | - | 2.71s · built · 3.25 MB | - | 3.67s · built · 4.07 MB | 3.46s · built · 4.07 MB |
| jac313_store_004_TS | - | 2.30s · built · 3.22 MB | - | 4.08s · built · 4.04 MB | 3.30s · built · 4.04 MB |
| jac313_store_004_XS | - | 2.32s · built · 3.20 MB | - | 3.79s · built · 4.01 MB | 4.33s · built · 4.01 MB |
| jac313_store_005_TS | - | 2.23s · built · 3.10 MB | - | 3.24s · built · 3.91 MB | 3.16s · built · 3.91 MB |
| jac313_store_005_XS | - | 3.02s · built · 3.08 MB | - | 2.96s · built · 3.89 MB | 2.99s · built · 3.89 MB |
| jac313_store_006_TS | - | 3.08s · built · 3.26 MB | - | 3.77s · built · 4.08 MB | 3.77s · built · 4.08 MB |
| jac313_store_006_XS | - | 2.99s · built · 3.25 MB | - | 3.38s · built · 4.07 MB | 3.42s · built · 4.07 MB |
| jac313_store_007_TS | - | 1.69s · built · 3.10 MB | - | 3.89s · built · 3.91 MB | 3.65s · built · 3.91 MB |
| jac313_store_007_XS | - | 2.06s · built · 3.08 MB | - | 3.30s · built · 3.89 MB | 2.90s · built · 3.89 MB |
| jac313_store_008_TS | - | 2.60s · built · 2.85 MB | - | 3.58s · built · 3.60 MB | 3.05s · built · 3.60 MB |
| jac313_store_008_XS | - | 2.19s · built · 2.85 MB | - | 2.62s · built · 3.60 MB | 3.58s · built · 3.60 MB |
| jac313_store_bench | - | 4.03s · built · 3.54 MB | - | 4.41s · built · 4.44 MB | 3.98s · built · 4.44 MB |
| jac313_store_flags | - | 0.81s · built · 194.67 KB | - | 1.08s · built · 239.98 KB | 0.79s · built · 239.98 KB |
| jac313_store_metric_view | - | 1.89s · built · 719.30 KB | - | 2.17s · built · 888.86 KB | 1.87s · built · 888.86 KB |
| jac313_store_v001_test | 1.34s · built · 423.98 KB | - | 1.41s · built · 575.74 KB | - | - |

## jac313-003 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · linuxmint-22.3 · x7k

| test | clang20·hdr | clang20·mod | gcc15·hdr | gcc15·mod |
|---|:--:|:--:|:--:|:--:|
| jac313_host_group_test | 1.48s · built · 380.23 KB | - | 1.18s · built · 472.18 KB | - |
| jac313_identity_dedup_test | 2.45s · built · 538.49 KB | - | 2.03s · built · 675.48 KB | - |
| jac313_instance_hash_test | 0.78s · built · 130.74 KB | - | 0.73s · built · 177.47 KB | - |
| jac313_jtext_v001_parser_validator_test | 1.20s · built · 2.12 MB | - | 0.93s · built · 2.69 MB | - |
| jac313_jtext_v001_test | 1.11s · built · 1.33 MB | - | 0.85s · built · 1.63 MB | - |
| jac313_qlite_v001_test | 0.60s · built · 547.71 KB | - | 0.82s · built · 671.06 KB | - |
| jac313_run_identity_test | 1.61s · built · 457.52 KB | - | 1.88s · built · 578.58 KB | - |
| jac313_store_001_TS | 3.03s · built · 3.25 MB | 3.30s · built · 3.25 MB | 4.28s · built · 4.10 MB | 3.22s · built · 4.10 MB |
| jac313_store_001_XS | 2.85s · built · 3.23 MB | 3.33s · built · 3.23 MB | 2.89s · built · 4.08 MB | 4.17s · built · 4.08 MB |
| jac313_store_002_TS | 3.21s · built · 3.08 MB | 3.08s · built · 3.08 MB | 3.48s · built · 3.90 MB | 4.67s · built · 3.90 MB |
| jac313_store_002_XS | 2.52s · built · 3.07 MB | 2.60s · built · 3.07 MB | 3.77s · built · 3.88 MB | 3.11s · built · 3.88 MB |
| jac313_store_003_TS | 2.94s · built · 3.28 MB | 2.88s · built · 3.28 MB | 3.34s · built · 4.12 MB | 3.85s · built · 4.12 MB |
| jac313_store_003_XS | 3.30s · built · 3.27 MB | 3.02s · built · 3.27 MB | 3.44s · built · 4.10 MB | 3.47s · built · 4.10 MB |
| jac313_store_004_TS | 2.52s · built · 3.24 MB | 2.63s · built · 3.24 MB | 3.31s · built · 4.07 MB | 3.52s · built · 4.08 MB |
| jac313_store_004_XS | 3.11s · built · 3.22 MB | 3.32s · built · 3.22 MB | 3.73s · built · 4.05 MB | 3.54s · built · 4.05 MB |
| jac313_store_005_TS | 3.67s · built · 3.12 MB | 3.48s · built · 3.12 MB | 3.35s · built · 3.94 MB | 3.41s · built · 3.94 MB |
| jac313_store_005_XS | 2.31s · built · 3.10 MB | 3.14s · built · 3.10 MB | 3.69s · built · 3.92 MB | 4.04s · built · 3.92 MB |
| jac313_store_006_TS | 2.97s · built · 3.28 MB | 3.15s · built · 3.28 MB | 3.71s · built · 4.12 MB | 3.53s · built · 4.12 MB |
| jac313_store_006_XS | 3.75s · built · 3.27 MB | 2.85s · built · 3.27 MB | 4.16s · built · 4.10 MB | 3.78s · built · 4.10 MB |
| jac313_store_007_TS | 2.23s · built · 3.12 MB | 2.72s · built · 3.12 MB | 3.12s · built · 3.94 MB | 3.36s · built · 3.94 MB |
| jac313_store_007_XS | 2.50s · built · 3.10 MB | 2.27s · built · 3.10 MB | 4.00s · built · 3.92 MB | 3.34s · built · 3.92 MB |
| jac313_store_008_TS | 2.56s · built · 2.87 MB | 2.28s · built · 2.87 MB | 2.83s · built · 3.63 MB | 3.09s · built · 3.63 MB |
| jac313_store_008_XS | 3.40s · built · 2.87 MB | 2.29s · built · 2.87 MB | 2.58s · built · 3.63 MB | 2.24s · built · 3.63 MB |
| jac313_store_bench | 3.56s · built · 3.57 MB | 3.03s · built · 3.57 MB | 4.19s · built · 4.48 MB | 3.99s · built · 4.48 MB |
| jac313_store_binary_persist_smoke | 3.06s · built · 1.30 MB | - | 4.83s · built · 1.61 MB | - |
| jac313_store_binary_reader_test | 2.29s · built · 2.44 MB | - | 2.46s · built · 3.12 MB | - |
| jac313_store_binary_to_jtext_smoke | 2.16s · built · 2.43 MB | - | 4.31s · built · 3.10 MB | - |
| jac313_store_flag_routing_smoke | 2.21s · built · 1.73 MB | - | 3.99s · built · 2.17 MB | - |
| jac313_store_flags | 0.81s · built · 200.90 KB | 0.92s · built · 200.91 KB | 1.14s · built · 243.09 KB | 1.35s · built · 243.38 KB |
| jac313_store_jtext_persist_smoke | 1.40s · built · 2.08 MB | - | 3.04s · built · 2.61 MB | - |
| jac313_store_metric_view | 1.57s · built · 725.02 KB | 1.95s · built · 725.03 KB | 1.57s · built · 908.62 KB | 2.27s · built · 908.95 KB |
| jac313_store_metric_view_test | 1.61s · built · 716.14 KB | - | 1.65s · built · 893.05 KB | - |
| jac313_store_sql_persist_smoke | 2.08s · built · 855.84 KB | - | 2.39s · built · 1.04 MB | - |
| jac313_store_v001_test | 2.21s · built · 430.41 KB | - | 1.73s · built · 587.63 KB | - |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
