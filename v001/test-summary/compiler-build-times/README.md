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

## jac313-002 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · Fedora Linux 44 (Workstation Edition) · x7k

| test | clang22·hdr | clang22·mod | gcc16·hdr | gcc16·mod | gcc16·istd |
|---|:--:|:--:|:--:|:--:|:--:|
| jac313_jtext_v001_parser_validator_test | 1.12s · built · 2.22 MB | - | 0.85s · built · 2.68 MB | - | - |
| jac313_jtext_v001_test | 0.80s · built · 1.42 MB | - | 0.98s · built · 1.64 MB | - | - |
| jac313_qlite_v001_test | 0.70s · built · 580.16 KB | - | 0.87s · built · 638.92 KB | - | - |
| jac313_store_001_TS | - | 2.88s · built · 3.44 MB | - | 4.30s · built · 4.07 MB | 3.99s · built · 4.07 MB |
| jac313_store_001_XS | - | 3.53s · built · 3.43 MB | - | 4.30s · built · 4.05 MB | 4.66s · built · 4.05 MB |
| jac313_store_002_TS | - | 3.48s · built · 3.27 MB | - | 3.15s · built · 3.89 MB | 3.29s · built · 3.89 MB |
| jac313_store_002_XS | - | 2.86s · built · 3.26 MB | - | 3.33s · built · 3.87 MB | 3.36s · built · 3.87 MB |
| jac313_store_003_TS | - | 3.27s · built · 3.48 MB | - | 4.26s · built · 4.09 MB | 4.67s · built · 4.09 MB |
| jac313_store_003_XS | - | 3.13s · built · 3.46 MB | - | 4.09s · built · 4.07 MB | 3.82s · built · 4.07 MB |
| jac313_store_004_TS | - | 3.40s · built · 3.43 MB | - | 4.01s · built · 4.05 MB | 4.21s · built · 4.05 MB |
| jac313_store_004_XS | - | 3.79s · built · 3.41 MB | - | 4.05s · built · 4.02 MB | 3.50s · built · 4.02 MB |
| jac313_store_005_TS | - | 2.74s · built · 3.30 MB | - | 3.11s · built · 3.92 MB | 3.59s · built · 3.92 MB |
| jac313_store_005_XS | - | 2.71s · built · 3.29 MB | - | 3.35s · built · 3.90 MB | 3.79s · built · 3.90 MB |
| jac313_store_006_TS | - | 3.68s · built · 3.48 MB | - | 4.05s · built · 4.09 MB | 4.02s · built · 4.09 MB |
| jac313_store_006_XS | - | 3.03s · built · 3.46 MB | - | 3.88s · built · 4.07 MB | 4.29s · built · 4.07 MB |
| jac313_store_007_TS | - | 3.56s · built · 3.30 MB | - | 4.05s · built · 3.93 MB | 3.51s · built · 3.93 MB |
| jac313_store_007_XS | - | 3.43s · built · 3.29 MB | - | 3.27s · built · 3.91 MB | 3.23s · built · 3.91 MB |
| jac313_store_008_TS | - | 2.54s · built · 3.05 MB | - | 3.44s · built · 3.62 MB | 3.88s · built · 3.62 MB |
| jac313_store_008_XS | - | 2.54s · built · 3.05 MB | - | 3.48s · built · 3.62 MB | 2.84s · built · 3.62 MB |
| jac313_store_bench | - | 4.66s · built · 3.74 MB | - | 4.59s · built · 4.44 MB | 4.15s · built · 4.44 MB |
| jac313_store_flags | - | 1.16s · built · 201.20 KB | - | 1.01s · built · 232.31 KB | 0.97s · built · 232.31 KB |
| jac313_store_metric_view | - | 2.08s · built · 772.17 KB | - | 2.17s · built · 888.41 KB | 2.17s · built · 888.41 KB |
| jac313_store_v001_test | 2.00s · built · 454.12 KB | - | 1.73s · built · 558.73 KB | - | - |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
