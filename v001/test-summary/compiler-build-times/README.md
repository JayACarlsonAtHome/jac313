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
| jac313_jtext_v001_parser_validator_test | 0.98s · built · 2.22 MB | - | 0.82s · built · 2.68 MB | - | - |
| jac313_jtext_v001_test | 0.93s · built · 1.42 MB | - | 0.71s · built · 1.64 MB | - | - |
| jac313_qlite_v001_test | 1.00s · built · 580.23 KB | - | 0.75s · built · 639.05 KB | - | - |
| jac313_store_001_TS | - | 2.82s · built · 3.45 MB | - | 3.60s · built · 4.07 MB | 3.82s · built · 4.07 MB |
| jac313_store_001_XS | - | 2.79s · built · 3.43 MB | - | 4.13s · built · 4.05 MB | 3.36s · built · 4.05 MB |
| jac313_store_002_TS | - | 2.63s · built · 3.28 MB | - | 3.54s · built · 3.89 MB | 3.11s · built · 3.89 MB |
| jac313_store_002_XS | - | 3.20s · built · 3.26 MB | - | 2.91s · built · 3.87 MB | 4.13s · built · 3.87 MB |
| jac313_store_003_TS | - | 3.25s · built · 3.48 MB | - | 4.14s · built · 4.09 MB | 3.33s · built · 4.09 MB |
| jac313_store_003_XS | - | 3.59s · built · 3.47 MB | - | 3.64s · built · 4.08 MB | 3.34s · built · 4.08 MB |
| jac313_store_004_TS | - | 3.06s · built · 3.44 MB | - | 3.55s · built · 4.06 MB | 4.59s · built · 4.06 MB |
| jac313_store_004_XS | - | 2.49s · built · 3.41 MB | - | 3.15s · built · 4.02 MB | 3.59s · built · 4.02 MB |
| jac313_store_005_TS | - | 3.18s · built · 3.31 MB | - | 3.69s · built · 3.93 MB | 3.47s · built · 3.93 MB |
| jac313_store_005_XS | - | 2.77s · built · 3.29 MB | - | 4.22s · built · 3.91 MB | 3.11s · built · 3.91 MB |
| jac313_store_006_TS | - | 3.90s · built · 3.48 MB | - | 4.66s · built · 4.09 MB | 4.19s · built · 4.09 MB |
| jac313_store_006_XS | - | 2.67s · built · 3.47 MB | - | 3.30s · built · 4.08 MB | 4.00s · built · 4.08 MB |
| jac313_store_007_TS | - | 2.63s · built · 3.31 MB | - | 3.54s · built · 3.93 MB | 3.09s · built · 3.93 MB |
| jac313_store_007_XS | - | 2.72s · built · 3.30 MB | - | 3.24s · built · 3.91 MB | 3.53s · built · 3.91 MB |
| jac313_store_008_TS | - | 3.09s · built · 3.06 MB | - | 2.81s · built · 3.62 MB | 2.68s · built · 3.62 MB |
| jac313_store_008_XS | - | 2.57s · built · 3.06 MB | - | 2.74s · built · 3.62 MB | 2.72s · built · 3.62 MB |
| jac313_store_bench | - | 2.91s · built · 3.77 MB | - | 3.44s · built · 4.46 MB | 4.63s · built · 4.46 MB |
| jac313_store_flags | - | 1.10s · built · 201.20 KB | - | 1.22s · built · 232.31 KB | 1.14s · built · 232.31 KB |
| jac313_store_metric_view | - | 2.27s · built · 772.17 KB | - | 2.05s · built · 888.41 KB | 2.01s · built · 888.41 KB |
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
| jac313_host_group_test | 1.10s · built · 373.37 KB | - | 1.22s · built · 461.15 KB | - | - |
| jac313_identity_dedup_test | 1.55s · built · 530.75 KB | - | 1.96s · built · 668.63 KB | - | - |
| jac313_instance_hash_test | 0.81s · built · 124.57 KB | - | 0.91s · built · 178.55 KB | - | - |
| jac313_jtext_v001_parser_validator_test | 0.68s · built · 2.12 MB | - | 1.25s · built · 2.80 MB | - | - |
| jac313_jtext_v001_test | 0.85s · built · 1.36 MB | - | 0.74s · built · 1.77 MB | - | - |
| jac313_qlite_v001_test | 0.57s · built · 542.71 KB | - | 1.05s · built · 655.62 KB | - | - |
| jac313_run_identity_test | 1.73s · built · 447.77 KB | - | 1.86s · built · 571.27 KB | - | - |
| jac313_store_001_TS | 2.93s · built · 4.21 MB | 2.75s · built · 4.21 MB | 4.63s · built · 5.43 MB | 4.13s · built · 5.43 MB | 3.90s · built · 5.43 MB |
| jac313_store_001_XS | 2.14s · built · 4.20 MB | 2.73s · built · 4.20 MB | 4.50s · built · 5.41 MB | 3.30s · built · 5.41 MB | 3.22s · built · 5.41 MB |
| jac313_store_002_TS | 2.75s · built · 4.05 MB | 1.98s · built · 4.05 MB | 3.61s · built · 5.24 MB | 3.83s · built · 5.24 MB | 3.74s · built · 5.24 MB |
| jac313_store_002_XS | 3.37s · built · 4.03 MB | 2.65s · built · 4.03 MB | 4.19s · built · 5.22 MB | 3.23s · built · 5.22 MB | 3.75s · built · 5.22 MB |
| jac313_store_003_TS | 2.15s · built · 4.25 MB | 2.86s · built · 4.25 MB | 4.12s · built · 5.45 MB | 3.57s · built · 5.45 MB | 3.83s · built · 5.45 MB |
| jac313_store_003_XS | 2.98s · built · 4.23 MB | 2.94s · built · 4.23 MB | 4.43s · built · 5.43 MB | 3.80s · built · 5.44 MB | 3.14s · built · 5.44 MB |
| jac313_store_004_TS | 2.91s · built · 4.20 MB | 2.76s · built · 4.20 MB | 4.27s · built · 5.41 MB | 3.41s · built · 5.41 MB | 3.98s · built · 5.41 MB |
| jac313_store_004_XS | 2.88s · built · 4.18 MB | 2.88s · built · 4.18 MB | 3.72s · built · 5.39 MB | 3.80s · built · 5.39 MB | 3.88s · built · 5.39 MB |
| jac313_store_005_TS | 2.70s · built · 4.07 MB | 1.94s · built · 4.07 MB | 3.78s · built · 5.27 MB | 3.22s · built · 5.27 MB | 3.04s · built · 5.27 MB |
| jac313_store_005_XS | 2.60s · built · 4.06 MB | 1.94s · built · 4.06 MB | 2.93s · built · 5.26 MB | 3.62s · built · 5.26 MB | 2.90s · built · 5.26 MB |
| jac313_store_006_TS | 2.80s · built · 4.25 MB | 2.64s · built · 4.25 MB | 3.19s · built · 5.45 MB | 3.48s · built · 5.45 MB | 3.18s · built · 5.45 MB |
| jac313_store_006_XS | 2.79s · built · 4.23 MB | 2.20s · built · 4.23 MB | 3.98s · built · 5.43 MB | 3.65s · built · 5.44 MB | 3.58s · built · 5.44 MB |
| jac313_store_007_TS | 1.97s · built · 4.07 MB | 1.98s · built · 4.07 MB | 3.13s · built · 5.28 MB | 3.12s · built · 5.28 MB | 2.97s · built · 5.28 MB |
| jac313_store_007_XS | 2.39s · built · 4.06 MB | 2.33s · built · 4.06 MB | 2.79s · built · 5.26 MB | 3.33s · built · 5.26 MB | 2.81s · built · 5.26 MB |
| jac313_store_008_TS | 2.43s · built · 2.98 MB | 2.24s · built · 2.98 MB | 3.28s · built · 3.84 MB | 2.80s · built · 3.85 MB | 2.92s · built · 3.85 MB |
| jac313_store_008_XS | 2.43s · built · 2.98 MB | 1.79s · built · 2.98 MB | 2.63s · built · 3.84 MB | 2.85s · built · 3.85 MB | 2.84s · built · 3.85 MB |
| jac313_store_bench | 2.58s · built · 4.53 MB | 2.19s · built · 4.53 MB | 4.91s · built · 5.81 MB | 3.78s · built · 5.81 MB | 4.25s · built · 5.81 MB |
| jac313_store_binary_persist_smoke | 2.00s · built · 1.33 MB | - | 2.53s · built · 1.76 MB | - | - |
| jac313_store_binary_reader_test | 2.09s · built · 2.56 MB | - | 2.38s · built · 3.34 MB | - | - |
| jac313_store_binary_to_jtext_smoke | 1.45s · built · 2.54 MB | - | 2.02s · built · 3.33 MB | - | - |
| jac313_store_flag_routing_smoke | 1.64s · built · 1.76 MB | - | 2.79s · built · 2.31 MB | - | - |
| jac313_store_flags | 0.67s · built · 194.12 KB | 0.92s · built · 194.12 KB | 0.81s · built · 240.05 KB | 0.76s · built · 240.35 KB | 0.82s · built · 240.35 KB |
| jac313_store_html_persist_smoke | 1.17s · built · 1.43 MB | - | 1.77s · built · 1.87 MB | - | - |
| jac313_store_json_persist_smoke | 1.21s · built · 1.40 MB | - | 1.85s · built · 1.83 MB | - | - |
| jac313_store_json_reader_test | 1.74s · built · 1.67 MB | - | 1.80s · built · 2.19 MB | - | - |
| jac313_store_jtext_persist_smoke | 1.19s · built · 2.20 MB | - | 1.44s · built · 2.84 MB | - | - |
| jac313_store_metric_view | 1.25s · built · 728.04 KB | 1.60s · built · 728.05 KB | 1.96s · built · 1.02 MB | 1.61s · built · 1.02 MB | 1.69s · built · 1.02 MB |
| jac313_store_metric_view_test | 1.80s · built · 714.85 KB | - | 1.71s · built · 1.00 MB | - | - |
| jac313_store_sql_persist_smoke | 1.28s · built · 843.77 KB | - | 2.09s · built · 1.03 MB | - | - |
| jac313_store_v001_test | 1.54s · built · 429.33 KB | - | 1.47s · built · 580.10 KB | - | - |

## jac313-005 — Intel(R) Core(TM) Ultra 7 265 · 14 cores · 42 GB · fedora-44 · x7k

| test | clang22·hdr | gcc16·hdr |
|---|:--:|:--:|
| jac313_jtext_v001_parser_validator_test | 0.98s · built · 2.22 MB | 0.82s · built · 2.68 MB |
| jac313_jtext_v001_test | 0.93s · built · 1.42 MB | 0.71s · built · 1.64 MB |
| jac313_qlite_v001_test | 1.00s · built · 580.23 KB | 0.75s · built · 639.05 KB |
| jac313_store_v001_test | 1.34s · built · 454.12 KB | 1.72s · built · 558.77 KB |

## jac313-006 — Intel(R) Core(TM) Ultra 7 265 · 14 cores · 42 GB · linuxmint-22.3 · x7k

| test | clang20·hdr | clang20·mod | gcc15·hdr | gcc15·mod | gcc15·istd |
|---|:--:|:--:|:--:|:--:|:--:|
| jac313_jtext_v001_parser_validator_test | 0.88s · built · 2.12 MB | - | 0.79s · built · 2.69 MB | - | - |
| jac313_jtext_v001_test | 0.67s · built · 1.33 MB | - | 0.91s · built · 1.63 MB | - | - |
| jac313_qlite_v001_test | 1.02s · built · 547.78 KB | - | 0.81s · built · 671.19 KB | - | - |
| jac313_store_001_TS | - | 2.93s · built · 4.15 MB | - | 4.17s · built · 5.33 MB | 4.03s · built · 5.33 MB |
| jac313_store_001_XS | - | 3.11s · built · 4.13 MB | - | 4.16s · built · 5.31 MB | 3.71s · built · 5.31 MB |
| jac313_store_002_TS | - | 2.69s · built · 3.98 MB | - | 2.88s · built · 5.13 MB | 3.85s · built · 5.13 MB |
| jac313_store_002_XS | - | 2.35s · built · 3.97 MB | - | 3.49s · built · 5.11 MB | 3.76s · built · 5.11 MB |
| jac313_store_003_TS | - | 3.22s · built · 4.19 MB | - | 3.95s · built · 5.35 MB | 3.45s · built · 5.35 MB |
| jac313_store_003_XS | - | 3.01s · built · 4.17 MB | - | 4.20s · built · 5.33 MB | 3.63s · built · 5.33 MB |
| jac313_store_004_TS | - | 2.65s · built · 4.15 MB | - | 3.31s · built · 5.31 MB | 3.68s · built · 5.31 MB |
| jac313_store_004_XS | - | 3.23s · built · 4.12 MB | - | 3.50s · built · 5.28 MB | 3.37s · built · 5.28 MB |
| jac313_store_005_TS | - | 2.98s · built · 4.02 MB | - | 2.94s · built · 5.17 MB | 3.00s · built · 5.17 MB |
| jac313_store_005_XS | - | 2.47s · built · 4.00 MB | - | 3.30s · built · 5.15 MB | 3.73s · built · 5.15 MB |
| jac313_store_006_TS | - | 2.64s · built · 4.19 MB | - | 3.14s · built · 5.35 MB | 3.47s · built · 5.35 MB |
| jac313_store_006_XS | - | 2.74s · built · 4.17 MB | - | 3.72s · built · 5.33 MB | 3.64s · built · 5.33 MB |
| jac313_store_007_TS | - | 2.83s · built · 4.02 MB | - | 3.59s · built · 5.17 MB | 3.90s · built · 5.17 MB |
| jac313_store_007_XS | - | 2.48s · built · 4.00 MB | - | 3.34s · built · 5.15 MB | 3.03s · built · 5.15 MB |
| jac313_store_008_TS | - | 2.51s · built · 2.94 MB | - | 2.72s · built · 3.72 MB | 2.65s · built · 3.72 MB |
| jac313_store_008_XS | - | 2.12s · built · 2.94 MB | - | 2.49s · built · 3.72 MB | 2.37s · built · 3.72 MB |
| jac313_store_bench | - | 3.34s · built · 4.47 MB | - | 4.38s · built · 5.71 MB | 3.55s · built · 5.71 MB |
| jac313_store_flags | - | 1.10s · built · 200.91 KB | - | 0.74s · built · 243.38 KB | 0.80s · built · 243.38 KB |
| jac313_store_metric_view | - | 2.32s · built · 725.03 KB | - | 2.51s · built · 908.95 KB | 1.98s · built · 908.95 KB |
| jac313_store_v001_test | 1.35s · built · 430.41 KB | - | 1.49s · built · 587.66 KB | - | - |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
