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

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
