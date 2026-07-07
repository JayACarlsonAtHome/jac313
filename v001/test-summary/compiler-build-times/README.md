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
| jac313_host_group_test | 1.88s · built · 393.77 KB | - | 1.55s · built · 448.75 KB | - | - |
| jac313_identity_dedup_test | 2.57s · built · 564.56 KB | - | 1.95s · built · 657.41 KB | - | - |
| jac313_instance_hash_test | 1.23s · built · 128.72 KB | - | 0.68s · built · 161.52 KB | - | - |
| jac313_jtext_v001_parser_validator_test | 0.98s · built · 2.22 MB | - | 0.82s · built · 2.68 MB | - | - |
| jac313_jtext_v001_test | 0.93s · built · 1.42 MB | - | 0.71s · built · 1.64 MB | - | - |
| jac313_qlite_v001_test | 1.00s · built · 580.23 KB | - | 0.75s · built · 639.05 KB | - | - |
| jac313_run_identity_test | 2.18s · built · 464.42 KB | - | 2.19s · built · 556.12 KB | - | - |
| jac313_store_001_TS | 2.60s · built · 3.45 MB | 2.82s · built · 3.45 MB | 3.17s · built · 4.07 MB | 3.60s · built · 4.07 MB | 3.82s · built · 4.07 MB |
| jac313_store_001_XS | 4.11s · built · 3.43 MB | 2.79s · built · 3.43 MB | 3.68s · built · 4.05 MB | 4.13s · built · 4.05 MB | 3.36s · built · 4.05 MB |
| jac313_store_002_TS | 3.14s · built · 3.28 MB | 2.63s · built · 3.28 MB | 3.18s · built · 3.89 MB | 3.54s · built · 3.89 MB | 3.11s · built · 3.89 MB |
| jac313_store_002_XS | 2.16s · built · 3.26 MB | 3.20s · built · 3.26 MB | 3.79s · built · 3.87 MB | 2.91s · built · 3.87 MB | 4.13s · built · 3.87 MB |
| jac313_store_003_TS | 3.65s · built · 3.48 MB | 3.25s · built · 3.48 MB | 3.53s · built · 4.09 MB | 4.14s · built · 4.09 MB | 3.33s · built · 4.09 MB |
| jac313_store_003_XS | 2.79s · built · 3.47 MB | 3.59s · built · 3.47 MB | 4.08s · built · 4.08 MB | 3.64s · built · 4.08 MB | 3.34s · built · 4.08 MB |
| jac313_store_004_TS | 2.79s · built · 3.44 MB | 3.06s · built · 3.44 MB | 4.61s · built · 4.05 MB | 3.55s · built · 4.06 MB | 4.59s · built · 4.06 MB |
| jac313_store_004_XS | 2.62s · built · 3.41 MB | 2.49s · built · 3.41 MB | 3.23s · built · 4.02 MB | 3.15s · built · 4.02 MB | 3.59s · built · 4.02 MB |
| jac313_store_005_TS | 2.53s · built · 3.31 MB | 3.18s · built · 3.31 MB | 2.86s · built · 3.92 MB | 3.69s · built · 3.93 MB | 3.47s · built · 3.93 MB |
| jac313_store_005_XS | 2.60s · built · 3.29 MB | 2.77s · built · 3.29 MB | 3.36s · built · 3.91 MB | 4.22s · built · 3.91 MB | 3.11s · built · 3.91 MB |
| jac313_store_006_TS | 2.94s · built · 3.48 MB | 3.90s · built · 3.48 MB | 3.31s · built · 4.09 MB | 4.66s · built · 4.09 MB | 4.19s · built · 4.09 MB |
| jac313_store_006_XS | 3.82s · built · 3.47 MB | 2.67s · built · 3.47 MB | 3.24s · built · 4.08 MB | 3.30s · built · 4.08 MB | 4.00s · built · 4.08 MB |
| jac313_store_007_TS | 2.44s · built · 3.31 MB | 2.63s · built · 3.31 MB | 2.82s · built · 3.93 MB | 3.54s · built · 3.93 MB | 3.09s · built · 3.93 MB |
| jac313_store_007_XS | 3.31s · built · 3.30 MB | 2.72s · built · 3.30 MB | 4.50s · built · 3.91 MB | 3.24s · built · 3.91 MB | 3.53s · built · 3.91 MB |
| jac313_store_008_TS | 2.41s · built · 3.06 MB | 3.09s · built · 3.06 MB | 3.85s · built · 3.62 MB | 2.81s · built · 3.62 MB | 2.68s · built · 3.62 MB |
| jac313_store_008_XS | 2.64s · built · 3.06 MB | 2.57s · built · 3.06 MB | 2.76s · built · 3.62 MB | 2.74s · built · 3.62 MB | 2.72s · built · 3.62 MB |
| jac313_store_bench | 2.70s · built · 3.77 MB | 2.91s · built · 3.77 MB | 4.03s · built · 4.46 MB | 3.44s · built · 4.46 MB | 4.63s · built · 4.46 MB |
| jac313_store_binary_persist_smoke | 2.60s · built · 1.39 MB | - | 2.26s · built · 1.61 MB | - | - |
| jac313_store_binary_reader_test | 2.88s · built · 2.59 MB | - | 2.78s · built · 3.13 MB | - | - |
| jac313_store_binary_to_jtext_smoke | 2.36s · built · 2.58 MB | - | 2.29s · built · 3.11 MB | - | - |
| jac313_store_flag_routing_smoke | 2.47s · built · 1.85 MB | - | 2.60s · built · 2.15 MB | - | - |
| jac313_store_flags | 1.48s · built · 201.18 KB | 1.10s · built · 201.20 KB | 0.79s · built · 232.02 KB | 1.22s · built · 232.31 KB | 1.14s · built · 232.31 KB |
| jac313_store_jtext_persist_smoke | 1.53s · built · 2.22 MB | - | 2.06s · built · 2.61 MB | - | - |
| jac313_store_metric_view | 1.51s · built · 772.16 KB | 2.27s · built · 772.17 KB | 2.27s · built · 888.08 KB | 2.05s · built · 888.41 KB | 2.01s · built · 888.41 KB |
| jac313_store_metric_view_test | 1.85s · built · 763.83 KB | - | 2.61s · built · 876.23 KB | - | - |
| jac313_store_sql_persist_smoke | 1.90s · built · 898.73 KB | - | 1.59s · built · 1.02 MB | - | - |
| jac313_store_v001_test | 1.92s · built · 454.12 KB | - | 1.54s · built · 558.77 KB | - | - |

## jac313-002 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · rhel-10.2 · x7k

| test | clang21·hdr | clang21·mod | gcc15·hdr | gcc15·mod | gcc15·istd |
|---|:--:|:--:|:--:|:--:|:--:|
| jac313_jtext_v001_parser_validator_test | 1.01s · built · 2.08 MB | - | 1.11s · built · 2.65 MB | - | - |
| jac313_jtext_v001_test | 1.14s · built · 1.34 MB | - | 0.68s · built · 1.62 MB | - | - |
| jac313_qlite_v001_test | 0.57s · built · 538.66 KB | - | 0.92s · built · 655.26 KB | - | - |
| jac313_store_001_TS | - | 2.48s · built · 3.24 MB | - | 3.06s · built · 4.07 MB | 3.61s · built · 4.07 MB |
| jac313_store_001_XS | - | 2.71s · built · 3.22 MB | - | 4.20s · built · 4.05 MB | 3.42s · built · 4.05 MB |
| jac313_store_002_TS | - | 2.94s · built · 3.07 MB | - | 3.29s · built · 3.87 MB | 3.36s · built · 3.87 MB |
| jac313_store_002_XS | - | 2.64s · built · 3.05 MB | - | 4.17s · built · 3.85 MB | 4.46s · built · 3.85 MB |
| jac313_store_003_TS | - | 3.19s · built · 3.27 MB | - | 3.50s · built · 4.08 MB | 4.56s · built · 4.08 MB |
| jac313_store_003_XS | - | 2.66s · built · 3.25 MB | - | 4.45s · built · 4.07 MB | 3.44s · built · 4.07 MB |
| jac313_store_004_TS | - | 2.71s · built · 3.22 MB | - | 3.66s · built · 4.04 MB | 3.75s · built · 4.04 MB |
| jac313_store_004_XS | - | 2.62s · built · 3.20 MB | - | 3.08s · built · 4.02 MB | 3.31s · built · 4.02 MB |
| jac313_store_005_TS | - | 2.56s · built · 3.10 MB | - | 3.30s · built · 3.91 MB | 3.26s · built · 3.91 MB |
| jac313_store_005_XS | - | 3.17s · built · 3.09 MB | - | 4.62s · built · 3.89 MB | 3.27s · built · 3.89 MB |
| jac313_store_006_TS | - | 2.65s · built · 3.27 MB | - | 3.29s · built · 4.08 MB | 3.44s · built · 4.08 MB |
| jac313_store_006_XS | - | 3.42s · built · 3.25 MB | - | 3.46s · built · 4.07 MB | 3.94s · built · 4.07 MB |
| jac313_store_007_TS | - | 2.58s · built · 3.10 MB | - | 3.57s · built · 3.91 MB | 3.72s · built · 3.91 MB |
| jac313_store_007_XS | - | 2.00s · built · 3.09 MB | - | 3.44s · built · 3.90 MB | 3.56s · built · 3.90 MB |
| jac313_store_008_TS | - | 2.02s · built · 2.85 MB | - | 3.06s · built · 3.61 MB | 3.29s · built · 3.61 MB |
| jac313_store_008_XS | - | 2.00s · built · 2.85 MB | - | 2.71s · built · 3.61 MB | 2.80s · built · 3.61 MB |
| jac313_store_bench | - | 3.11s · built · 3.55 MB | - | 4.55s · built · 4.45 MB | 3.58s · built · 4.45 MB |
| jac313_store_flags | - | 1.17s · built · 194.67 KB | - | 0.99s · built · 239.98 KB | 0.69s · built · 239.98 KB |
| jac313_store_metric_view | - | 1.41s · built · 719.30 KB | - | 1.80s · built · 888.86 KB | 2.14s · built · 888.86 KB |
| jac313_store_v001_test | 1.32s · built · 423.98 KB | - | 1.78s · built · 575.77 KB | - | - |

## jac313-003 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · linuxmint-22.3 · x7k

| test | clang20·hdr | clang20·mod | gcc15·hdr | gcc15·mod |
|---|:--:|:--:|:--:|:--:|
| jac313_jtext_v001_parser_validator_test | 1.20s · built · 2.12 MB | - | 0.93s · built · 2.69 MB | - |
| jac313_jtext_v001_test | 1.11s · built · 1.33 MB | - | 0.85s · built · 1.63 MB | - |
| jac313_qlite_v001_test | 0.60s · built · 547.71 KB | - | 0.82s · built · 671.06 KB | - |
| jac313_store_001_TS | - | 3.30s · built · 3.25 MB | - | 3.22s · built · 4.10 MB |
| jac313_store_001_XS | - | 3.33s · built · 3.23 MB | - | 4.17s · built · 4.08 MB |
| jac313_store_002_TS | - | 3.08s · built · 3.08 MB | - | 4.67s · built · 3.90 MB |
| jac313_store_002_XS | - | 2.60s · built · 3.07 MB | - | 3.11s · built · 3.88 MB |
| jac313_store_003_TS | - | 2.88s · built · 3.28 MB | - | 3.85s · built · 4.12 MB |
| jac313_store_003_XS | - | 3.02s · built · 3.27 MB | - | 3.47s · built · 4.10 MB |
| jac313_store_004_TS | - | 2.63s · built · 3.24 MB | - | 3.52s · built · 4.08 MB |
| jac313_store_004_XS | - | 3.32s · built · 3.22 MB | - | 3.54s · built · 4.05 MB |
| jac313_store_005_TS | - | 3.48s · built · 3.12 MB | - | 3.41s · built · 3.94 MB |
| jac313_store_005_XS | - | 3.14s · built · 3.10 MB | - | 4.04s · built · 3.92 MB |
| jac313_store_006_TS | - | 3.15s · built · 3.28 MB | - | 3.53s · built · 4.12 MB |
| jac313_store_006_XS | - | 2.85s · built · 3.27 MB | - | 3.78s · built · 4.10 MB |
| jac313_store_007_TS | - | 2.72s · built · 3.12 MB | - | 3.36s · built · 3.94 MB |
| jac313_store_007_XS | - | 2.27s · built · 3.10 MB | - | 3.34s · built · 3.92 MB |
| jac313_store_008_TS | - | 2.28s · built · 2.87 MB | - | 3.09s · built · 3.63 MB |
| jac313_store_008_XS | - | 2.29s · built · 2.87 MB | - | 2.24s · built · 3.63 MB |
| jac313_store_bench | - | 3.03s · built · 3.57 MB | - | 3.99s · built · 4.48 MB |
| jac313_store_flags | - | 0.92s · built · 200.91 KB | - | 1.35s · built · 243.38 KB |
| jac313_store_metric_view | - | 1.95s · built · 725.03 KB | - | 2.27s · built · 908.95 KB |
| jac313_store_v001_test | 2.21s · built · 430.41 KB | - | 1.73s · built · 587.63 KB | - |

## jac313-004 — Intel(R) Core(TM) Ultra 7 265 · 20 cores · 62 GB · rhel-9.8 · ssd

| test | clang21·hdr | clang21·mod | gcc15·hdr | gcc15·mod | gcc15·istd |
|---|:--:|:--:|:--:|:--:|:--:|
| jac313_jtext_v001_parser_validator_test | 0.69s · built · 2.12 MB | - | 0.78s · built · 2.80 MB | - | - |
| jac313_jtext_v001_test | 0.62s · built · 1.36 MB | - | 1.28s · built · 1.77 MB | - | - |
| jac313_qlite_v001_test | 0.53s · built · 542.69 KB | - | 0.68s · built · 655.59 KB | - | - |
| jac313_store_001_TS | - | 2.48s · built · 3.30 MB | - | 3.56s · built · 4.21 MB | 3.59s · built · 4.21 MB |
| jac313_store_001_XS | - | 2.00s · built · 3.28 MB | - | 3.52s · built · 4.19 MB | 3.09s · built · 4.19 MB |
| jac313_store_002_TS | - | 2.46s · built · 3.13 MB | - | 3.14s · built · 4.02 MB | 2.78s · built · 4.02 MB |
| jac313_store_002_XS | - | 2.62s · built · 3.11 MB | - | 2.70s · built · 3.99 MB | 3.36s · built · 3.99 MB |
| jac313_store_003_TS | - | 2.90s · built · 3.33 MB | - | 3.49s · built · 4.23 MB | 2.90s · built · 4.23 MB |
| jac313_store_003_XS | - | 2.02s · built · 3.31 MB | - | 3.06s · built · 4.21 MB | 3.73s · built · 4.21 MB |
| jac313_store_004_TS | - | 1.98s · built · 3.28 MB | - | 3.58s · built · 4.19 MB | 3.56s · built · 4.19 MB |
| jac313_store_004_XS | - | 1.98s · built · 3.26 MB | - | 3.61s · built · 4.16 MB | 3.45s · built · 4.16 MB |
| jac313_store_005_TS | - | 2.70s · built · 3.16 MB | - | 3.47s · built · 4.05 MB | 3.36s · built · 4.05 MB |
| jac313_store_005_XS | - | 2.64s · built · 3.14 MB | - | 3.37s · built · 4.04 MB | 3.22s · built · 4.04 MB |
| jac313_store_006_TS | - | 2.92s · built · 3.33 MB | - | 3.55s · built · 4.23 MB | 3.16s · built · 4.23 MB |
| jac313_store_006_XS | - | 2.05s · built · 3.31 MB | - | 3.42s · built · 4.21 MB | 3.02s · built · 4.21 MB |
| jac313_store_007_TS | - | 1.82s · built · 3.16 MB | - | 2.69s · built · 4.06 MB | 2.72s · built · 4.06 MB |
| jac313_store_007_XS | - | 2.09s · built · 3.15 MB | - | 3.16s · built · 4.04 MB | 3.22s · built · 4.04 MB |
| jac313_store_008_TS | - | 1.68s · built · 2.91 MB | - | 2.43s · built · 3.75 MB | 2.98s · built · 3.75 MB |
| jac313_store_008_XS | - | 1.91s · built · 2.91 MB | - | 2.36s · built · 3.74 MB | 2.50s · built · 3.74 MB |
| jac313_store_bench | - | 2.12s · built · 3.61 MB | - | 3.86s · built · 4.59 MB | 3.90s · built · 4.59 MB |
| jac313_store_flags | - | 0.90s · built · 194.09 KB | - | 1.05s · built · 240.30 KB | 0.79s · built · 240.30 KB |
| jac313_store_metric_view | - | 1.92s · built · 728.02 KB | - | 1.58s · built · 1.02 MB | 2.14s · built · 1.02 MB |
| jac313_store_v001_test | 1.07s · built · 429.30 KB | - | 1.20s · built · 580.02 KB | - | - |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
