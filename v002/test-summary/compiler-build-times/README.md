# Compiler build times

_Generated from `results.db`. Compile time is hardware-specific, so each machine (`jac313-###`) is its own section. Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 12 cores · 23 GB · Fedora Linux 44 (Workstation Edition)

| test | clang22·hdr | clang22·mod | clang22·istd | gcc16·hdr | gcc16·mod | gcc16·istd |
|---|:--:|:--:|:--:|:--:|:--:|:--:|
| jac313_store_001_TS | 1.84s · pass · 3.43 MB | 1.91s · pass · 3.43 MB | 1.88s · pass · 3.43 MB | 2.24s · pass · 4.14 MB | 2.48s · pass · 4.14 MB | 2.48s · pass · 4.14 MB |
| jac313_store_002_TS | 1.78s · pass · 3.28 MB | 1.81s · pass · 3.28 MB | 1.96s · pass · 3.28 MB | 2.18s · pass · 3.97 MB | 2.27s · pass · 3.98 MB | 2.42s · pass · 3.98 MB |

## jac313-003 — Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz · 8 cores · 31 GB · Linux Mint 22.3

| test | clang20·hdr | clang20·mod | clang20·istd | gcc15·hdr | gcc15·mod | gcc15·istd |
|---|:--:|:--:|:--:|:--:|:--:|:--:|
| jac313_store_001_TS | 9.59s · built · 3.23 MB | 10.55s · built · 3.23 MB | 8.49s · built · 3.23 MB | 10.55s · built · 4.10 MB | 10.45s · built · 4.10 MB | 10.44s · built · 4.10 MB |
| jac313_store_001_XS | 10.08s · built · 3.22 MB | 11.33s · built · 3.22 MB | 9.57s · built · 3.22 MB | 9.37s · built · 4.08 MB | 10.25s · built · 4.09 MB | 11.61s · built · 4.09 MB |
| jac313_store_002_TS | 8.74s · built · 3.08 MB | 11.02s · built · 3.08 MB | 9.62s · built · 3.08 MB | 8.71s · built · 3.93 MB | 10.26s · built · 3.93 MB | 8.56s · built · 3.93 MB |
| jac313_store_002_XS | 10.21s · built · 3.06 MB | 9.25s · built · 3.06 MB | 9.16s · built · 3.06 MB | 11.50s · built · 3.91 MB | 9.80s · built · 3.91 MB | 10.01s · built · 3.91 MB |
| jac313_store_003_TS | 10.32s · built · 3.27 MB | 10.89s · built · 3.27 MB | 10.10s · built · 3.27 MB | 10.28s · built · 4.13 MB | 12.10s · built · 4.13 MB | 11.92s · built · 4.13 MB |
| jac313_store_003_XS | 9.63s · built · 3.25 MB | 10.64s · built · 3.25 MB | 11.05s · built · 3.25 MB | 10.93s · built · 4.11 MB | 11.24s · built · 4.11 MB | 10.32s · built · 4.11 MB |
| jac313_store_004_TS | 9.65s · built · 3.22 MB | 8.03s · built · 3.22 MB | 9.31s · built · 3.22 MB | 9.86s · built · 4.09 MB | 11.19s · built · 4.09 MB | 11.12s · built · 4.09 MB |
| jac313_store_004_XS | 9.83s · built · 3.20 MB | 11.94s · built · 3.20 MB | 10.16s · built · 3.20 MB | 9.51s · built · 4.06 MB | 9.85s · built · 4.06 MB | 12.14s · built · 4.06 MB |
| jac313_store_005_TS | 9.30s · built · 3.11 MB | 10.46s · built · 3.11 MB | 9.55s · built · 3.11 MB | 9.71s · built · 3.97 MB | 10.05s · built · 3.97 MB | 9.50s · built · 3.97 MB |
| jac313_store_005_XS | 6.89s · built · 3.09 MB | 9.63s · built · 3.09 MB | 8.27s · built · 3.09 MB | 8.23s · built · 3.95 MB | 9.03s · built · 3.95 MB | 9.19s · built · 3.95 MB |
| jac313_store_006_TS | 7.66s · built · 3.27 MB | 9.30s · built · 3.27 MB | 9.36s · built · 3.27 MB | 8.54s · built · 4.13 MB | 9.17s · built · 4.13 MB | 8.98s · built · 4.13 MB |
| jac313_store_006_XS | 6.96s · built · 3.25 MB | 7.97s · built · 3.25 MB | 8.00s · built · 3.25 MB | 8.42s · built · 4.11 MB | 8.67s · built · 4.11 MB | 8.76s · built · 4.11 MB |
| jac313_store_007_TS | 7.43s · built · 3.11 MB | 7.94s · built · 3.11 MB | 6.82s · built · 3.11 MB | 7.48s · built · 3.97 MB | 8.02s · built · 3.97 MB | 8.25s · built · 3.97 MB |
| jac313_store_007_XS | 6.74s · built · 3.10 MB | 8.20s · built · 3.10 MB | 7.28s · built · 3.10 MB | 7.57s · built · 3.95 MB | 8.07s · built · 3.95 MB | 8.21s · built · 3.95 MB |
| jac313_store_008_TS | 6.22s · built · 2.87 MB | 7.79s · built · 2.87 MB | 6.44s · built · 2.87 MB | 6.47s · built · 3.66 MB | 6.68s · built · 3.66 MB | 6.93s · built · 3.66 MB |
| jac313_store_008_XS | 6.12s · built · 2.86 MB | 6.80s · built · 2.86 MB | 5.85s · built · 2.86 MB | 6.54s · built · 3.66 MB | 5.65s · built · 3.66 MB | 6.46s · built · 3.66 MB |
| jac313_store_bench | 8.75s · built · 3.45 MB | 10.09s · built · 3.45 MB | 8.13s · built · 3.45 MB | 11.80s · built · 4.38 MB | 12.32s · built · 4.38 MB | 11.00s · built · 4.38 MB |
| jac313_store_flags | 3.56s · built · 201.01 KB | 4.53s · built · 201.01 KB | 4.20s · built · 201.00 KB | 2.83s · built · 243.39 KB | 2.71s · built · 243.68 KB | 3.31s · built · 243.68 KB |
| jac313_store_metric_view | 6.89s · built · 725.94 KB | 6.93s · built · 725.94 KB | 7.37s · built · 725.94 KB | 7.32s · built · 922.65 KB | 6.21s · built · 922.97 KB | 5.56s · built · 922.97 KB |
