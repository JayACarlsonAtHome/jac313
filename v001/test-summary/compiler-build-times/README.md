<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# Compiler build times

_Generated from `results.db`. Compile time is hardware-specific, so each machine (`jac313-###`) is its own section.

Column headers are `compiler·fe`:
- `hdr` — code uses headers only
- `mod` — code uses modules

Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 20 cores · 62 GB · rhel-9.8 · xssd

| test | clang21·hdr | clang21·mod | gcc15·hdr | gcc15·mod |
|---|:--:|:--:|:--:|:--:|
| jac313_store_001_TS | 2.63s · built · 6.07 MB | 3.52s · built · 6.07 MB | 8.75s · built · 7.79 MB | 9.71s · built · 7.79 MB |
| jac313_store_001_XS | 3.44s · built · 6.06 MB | 3.42s · built · 6.06 MB | 8.74s · built · 7.78 MB | 9.85s · built · 7.78 MB |
| jac313_store_002_TS | 3.65s · built · 5.93 MB | 3.38s · built · 5.93 MB | 6.87s · built · 7.62 MB | 7.26s · built · 7.63 MB |
| jac313_store_002_XS | 3.14s · built · 5.92 MB | 3.45s · built · 5.92 MB | 8.29s · built · 7.61 MB | 5.55s · built · 7.61 MB |
| jac313_store_003_TS | 3.42s · built · 6.11 MB | 3.08s · built · 6.11 MB | 8.96s · built · 7.81 MB | 5.80s · built · 7.81 MB |
| jac313_store_003_XS | 3.54s · built · 6.09 MB | 3.01s · built · 6.09 MB | 6.86s · built · 7.79 MB | 9.28s · built · 7.79 MB |
| jac313_store_004_TS | 2.52s · built · 6.06 MB | 3.26s · built · 6.06 MB | 8.12s · built · 7.77 MB | 7.16s · built · 7.78 MB |
| jac313_store_004_XS | 2.91s · built · 6.05 MB | 2.80s · built · 6.05 MB | 7.82s · built · 7.75 MB | 7.11s · built · 7.75 MB |
| jac313_store_005_TS | 2.79s · built · 5.95 MB | 2.58s · built · 5.95 MB | 8.00s · built · 7.65 MB | 6.46s · built · 7.66 MB |
| jac313_store_005_XS | 3.01s · built · 5.94 MB | 2.54s · built · 5.94 MB | 8.06s · built · 7.64 MB | 6.66s · built · 7.65 MB |
| jac313_store_006_TS | 2.75s · built · 6.11 MB | 2.76s · built · 6.11 MB | 7.61s · built · 7.81 MB | 8.73s · built · 7.81 MB |
| jac313_store_006_XS | 2.50s · built · 6.09 MB | 3.14s · built · 6.09 MB | 8.28s · built · 7.79 MB | 8.59s · built · 7.79 MB |
| jac313_store_007_TS | 2.43s · built · 5.96 MB | 2.92s · built · 5.96 MB | 8.06s · built · 7.66 MB | 8.70s · built · 7.66 MB |
| jac313_store_007_XS | 2.47s · built · 5.94 MB | 3.06s · built · 5.94 MB | 8.08s · built · 7.64 MB | 8.08s · built · 7.65 MB |
| jac313_store_008_TS | 2.43s · built · 3.09 MB | 2.18s · built · 3.09 MB | 4.82s · built · 4.06 MB | 6.46s · built · 4.06 MB |
| jac313_store_008_XS | 1.94s · built · 3.09 MB | 2.09s · built · 3.09 MB | 5.17s · built · 4.06 MB | 6.25s · built · 4.06 MB |
| jac313_store_bench | 2.69s · built · 6.30 MB | 3.71s · built · 6.30 MB | 8.99s · built · 8.02 MB | 9.95s · built · 8.03 MB |
| jac313_store_flags | 1.22s · built · 194.77 KB | 0.94s · built · 194.77 KB | 0.72s · built · 241.05 KB | 0.81s · built · 241.34 KB |
| jac313_store_metric_view | 1.58s · built · 732.69 KB | 1.69s · built · 732.69 KB | 2.02s · built · 1.02 MB | 2.47s · built · 1.02 MB |

## jac313-002 — Intel(R) Core(TM) Ultra 7 265 · 20 cores · 62 GB · fedora-44 · ssd

| test | clang22·hdr | clang22·mod | gcc16·hdr | gcc16·mod | gcc16·mod |
|---|:--:|:--:|:--:|:--:|:--:|
| jac313_store_001_TS | 3.54s · built · 6.32 MB | 4.46s · built · 6.32 MB | 5.79s · built · 7.70 MB | 5.33s · built · 7.70 MB | 5.69s · built · 7.70 MB |
| jac313_store_001_XS | 3.76s · built · 6.31 MB | 4.43s · built · 6.31 MB | 5.44s · built · 7.69 MB | 5.34s · built · 7.69 MB | 5.60s · built · 7.69 MB |
| jac313_store_002_TS | 4.14s · built · 6.17 MB | 3.60s · built · 6.17 MB | 5.06s · built · 7.54 MB | 5.20s · built · 7.54 MB | 5.14s · built · 7.54 MB |
| jac313_store_002_XS | 3.82s · built · 6.16 MB | 4.44s · built · 6.16 MB | 5.23s · built · 7.52 MB | 5.16s · built · 7.53 MB | 5.44s · built · 7.53 MB |
| jac313_store_003_TS | 4.47s · built · 6.36 MB | 4.89s · built · 6.36 MB | 5.69s · built · 7.73 MB | 4.91s · built · 7.73 MB | 5.86s · built · 7.73 MB |
| jac313_store_003_XS | 3.47s · built · 6.34 MB | 4.29s · built · 6.34 MB | 5.65s · built · 7.70 MB | 5.65s · built · 7.71 MB | 5.89s · built · 7.71 MB |
| jac313_store_004_TS | 3.38s · built · 6.31 MB | 3.69s · built · 6.31 MB | 4.34s · built · 7.68 MB | 4.95s · built · 7.69 MB | 4.73s · built · 7.69 MB |
| jac313_store_004_XS | 3.51s · built · 6.29 MB | 3.74s · built · 6.29 MB | 4.94s · built · 7.66 MB | 4.52s · built · 7.67 MB | 4.43s · built · 7.67 MB |
| jac313_store_005_TS | 2.95s · built · 6.20 MB | 3.29s · built · 6.20 MB | 4.11s · built · 7.57 MB | 4.33s · built · 7.58 MB | 4.09s · built · 7.58 MB |
| jac313_store_005_XS | 3.31s · built · 6.19 MB | 3.75s · built · 6.19 MB | 4.38s · built · 7.56 MB | 4.32s · built · 7.56 MB | 4.13s · built · 7.56 MB |
| jac313_store_006_TS | 3.65s · built · 6.36 MB | 3.51s · built · 6.36 MB | 4.20s · built · 7.73 MB | 4.32s · built · 7.73 MB | 4.91s · built · 7.73 MB |
| jac313_store_006_XS | 3.04s · built · 6.34 MB | 3.18s · built · 6.34 MB | 4.68s · built · 7.70 MB | 4.75s · built · 7.71 MB | 4.47s · built · 7.71 MB |
| jac313_store_007_TS | 2.86s · built · 6.20 MB | 3.07s · built · 6.20 MB | 3.75s · built · 7.57 MB | 4.04s · built · 7.58 MB | 3.99s · built · 7.58 MB |
| jac313_store_007_XS | 3.31s · built · 6.19 MB | 3.08s · built · 6.19 MB | 3.71s · built · 7.56 MB | 3.93s · built · 7.57 MB | 4.42s · built · 7.57 MB |
| jac313_store_008_TS | 2.39s · built · 3.27 MB | 2.87s · built · 3.27 MB | 2.79s · built · 3.95 MB | 3.73s · built · 3.96 MB | 3.80s · built · 3.96 MB |
| jac313_store_008_XS | 2.45s · built · 3.27 MB | 2.96s · built · 3.27 MB | 2.79s · built · 3.95 MB | 3.96s · built · 3.95 MB | 3.80s · built · 3.95 MB |
| jac313_store_bench | 4.41s · built · 6.55 MB | 4.42s · built · 6.55 MB | 5.50s · built · 7.94 MB | 5.91s · built · 7.94 MB | 5.99s · built · 7.94 MB |
| jac313_store_flags | 1.07s · built · 201.87 KB | 1.42s · built · 201.87 KB | 1.01s · built · 233.06 KB | 1.66s · built · 233.34 KB | 1.42s · built · 233.35 KB |
| jac313_store_metric_view | 2.14s · built · 779.91 KB | 2.47s · built · 779.91 KB | 2.44s · built · 892.08 KB | 2.55s · built · 892.40 KB | 2.72s · built · 892.40 KB |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
