<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# Compiler build times

_Generated from `results.db`. Compile time is hardware-specific, so each machine (`jac313-###`) is its own section. Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 19 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k

| test | clang21·hdr | clang21·istd | gcc15·hdr |
|---|:--:|:--:|:--:|
| jac313_contracts_disabled_test | 0.53s · built · 28.73 KB | - | 0.69s · built · 46.64 KB |
| jac313_contracts_enforce_test | 0.62s · built · 526.30 KB | - | 1.15s · built · 655.04 KB |
| jac313_contracts_test | 0.79s · built · 31.22 KB | - | 0.76s · built · 49.38 KB |
| jac313_identity_dedup_test | 1.95s · built · 529.39 KB | - | 1.96s · built · 682.80 KB |
| jac313_jtext_v002_parser_validator_test | 1.28s · built · 2.08 MB | - | 1.16s · built · 2.68 MB |
| jac313_jtext_v002_test | 0.65s · built · 1.34 MB | - | 1.07s · built · 1.64 MB |
| jac313_qlite_v002_test | 0.52s · built · 493.08 KB | - | 0.90s · built · 605.12 KB |
| jac313_run_identity_test | 1.76s · built · 445.12 KB | - | 2.09s · built · 591.22 KB |
| jac313_store_001_TS | 2.36s · built · 3.21 MB | 2.45s · built · 3.21 MB | 4.33s · built · 4.07 MB |
| jac313_store_001_XS | 2.02s · built · 3.20 MB | 2.79s · built · 3.20 MB | 3.51s · built · 4.05 MB |
| jac313_store_002_TS | 2.82s · built · 3.06 MB | 2.54s · built · 3.06 MB | 3.75s · built · 3.90 MB |
| jac313_store_002_XS | 2.06s · built · 3.05 MB | 2.97s · built · 3.05 MB | 3.60s · built · 3.88 MB |
| jac313_store_003_TS | 2.77s · built · 3.25 MB | 3.34s · built · 3.25 MB | 3.95s · built · 4.09 MB |
| jac313_store_003_XS | 3.13s · built · 3.23 MB | 2.47s · built · 3.23 MB | 3.86s · built · 4.07 MB |
| jac313_store_004_TS | 2.28s · built · 3.20 MB | 2.30s · built · 3.20 MB | 3.75s · built · 4.05 MB |
| jac313_store_004_XS | 3.00s · built · 3.18 MB | 3.38s · built · 3.18 MB | 4.05s · built · 4.02 MB |
| jac313_store_005_TS | 2.11s · built · 3.09 MB | 2.99s · built · 3.09 MB | 3.12s · built · 3.94 MB |
| jac313_store_005_XS | 2.74s · built · 3.08 MB | 2.31s · built · 3.08 MB | 2.99s · built · 3.92 MB |
| jac313_store_006_TS | 2.69s · built · 3.25 MB | 2.83s · built · 3.25 MB | 5.38s · built · 4.09 MB |
| jac313_store_006_XS | 2.54s · built · 3.23 MB | 2.75s · built · 3.23 MB | 3.36s · built · 4.07 MB |
| jac313_store_007_TS | 2.55s · built · 3.09 MB | 2.99s · built · 3.09 MB | 3.87s · built · 3.94 MB |
| jac313_store_007_XS | 2.24s · built · 3.08 MB | 2.34s · built · 3.08 MB | 4.68s · built · 3.92 MB |
| jac313_store_008_TS | 2.16s · built · 2.85 MB | 1.96s · built · 2.85 MB | 2.84s · built · 3.63 MB |
| jac313_store_008_XS | 2.35s · built · 2.85 MB | 2.66s · built · 2.85 MB | 2.49s · built · 3.63 MB |
| jac313_store_bench | 2.27s · built · 3.51 MB | 2.91s · built · 3.51 MB | 3.27s · built · 4.46 MB |
| jac313_store_binary_persist_smoke | 2.30s · built · 1.32 MB | - | 2.43s · built · 1.64 MB |
| jac313_store_binary_reader_test | 1.55s · built · 2.46 MB | - | 2.63s · built · 3.16 MB |
| jac313_store_binary_to_jtext_smoke | 1.94s · built · 2.43 MB | - | 2.98s · built · 3.13 MB |
| jac313_store_flag_routing_smoke | 2.51s · built · 1.74 MB | - | 2.75s · built · 2.20 MB |
| jac313_store_flags | 0.82s · built · 194.73 KB | 0.74s · built · 194.75 KB | 1.09s · built · 239.98 KB |
| jac313_store_jtext_persist_smoke | 1.46s · built · 2.09 MB | - | 1.63s · built · 2.63 MB |
| jac313_store_metric_view | 2.11s · built · 720.16 KB | 1.38s · built · 720.17 KB | 1.90s · built · 902.54 KB |
| jac313_store_metric_view_test | 1.48s · built · 706.73 KB | - | 2.50s · built · 886.95 KB |
| jac313_store_saturating_capacity_test | 0.71s · built · 31.96 KB | - | 0.65s · built · 50.92 KB |
| jac313_store_sql_persist_smoke | 1.27s · built · 830.10 KB | - | 2.21s · built · 1.05 MB |
| jac313_store_v002_test | 1.24s · built · 423.43 KB | - | 1.60s · built · 575.03 KB |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
