<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# Compiler build times

_Generated from `results.db`. Compile time is hardware-specific, so each machine (`jac313-###`) is its own section. Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · fedora-44 · x7k

| test | clang22·hdr | gcc16·hdr |
|---|:--:|:--:|
| jac313_contracts_disabled_test | 0.77s · built · 29.96 KB | 0.61s · built · 47.55 KB |
| jac313_contracts_enforce_test | 1.29s · built · 561.69 KB | 0.98s · built · 730.27 KB |
| jac313_contracts_test | 1.07s · built · 32.53 KB | 0.87s · built · 87.05 KB |
| jac313_host_group_test | 1.95s · built · 395.30 KB | 1.62s · built · 458.80 KB |
| jac313_identity_dedup_test | 3.68s · built · 567.21 KB | 1.73s · built · 681.16 KB |
| jac313_instance_hash_test | 0.80s · built · 130.23 KB | 0.70s · built · 164.05 KB |
| jac313_jtext_v002_parser_validator_test | 1.27s · built · 2.23 MB | 1.01s · built · 2.75 MB |
| jac313_jtext_v002_test | 1.42s · built · 1.43 MB | 1.19s · built · 1.68 MB |
| jac313_qlite_v002_test | 1.10s · built · 580.16 KB | 0.83s · built · 638.93 KB |
| jac313_run_identity_test | 2.73s · built · 467.34 KB | 1.81s · built · 591.10 KB |
| jac313_store_001_TS | 3.70s · built · 3.44 MB | 3.50s · built · 4.14 MB |
| jac313_store_001_XS | 2.99s · built · 3.42 MB | 3.41s · built · 4.12 MB |
| jac313_store_002_TS | 3.52s · built · 3.28 MB | 4.34s · built · 3.98 MB |
| jac313_store_002_XS | 3.30s · built · 3.26 MB | 3.69s · built · 3.95 MB |
| jac313_store_003_TS | 3.71s · built · 3.47 MB | 3.27s · built · 4.17 MB |
| jac313_store_003_XS | 3.55s · built · 3.45 MB | 3.76s · built · 4.14 MB |
| jac313_store_004_TS | 3.97s · built · 3.42 MB | 3.64s · built · 4.12 MB |
| jac313_store_004_XS | 2.57s · built · 3.40 MB | 3.81s · built · 4.09 MB |
| jac313_store_005_TS | 2.86s · built · 3.31 MB | 3.02s · built · 4.01 MB |
| jac313_store_005_XS | 3.60s · built · 3.30 MB | 3.39s · built · 3.99 MB |
| jac313_store_006_TS | 3.58s · built · 3.47 MB | 3.68s · built · 4.17 MB |
| jac313_store_006_XS | 3.06s · built · 3.45 MB | 4.16s · built · 4.14 MB |
| jac313_store_007_TS | 2.95s · built · 3.31 MB | 3.26s · built · 4.01 MB |
| jac313_store_007_XS | 3.42s · built · 3.30 MB | 3.57s · built · 3.99 MB |
| jac313_store_008_TS | 3.06s · built · 3.07 MB | 4.27s · built · 3.70 MB |
| jac313_store_008_XS | 2.82s · built · 3.06 MB | 2.72s · built · 3.70 MB |
| jac313_store_bench | 3.34s · built · 3.77 MB | 4.42s · built · 4.54 MB |
| jac313_store_binary_persist_smoke | 3.59s · built · 1.41 MB | 2.29s · built · 1.66 MB |
| jac313_store_binary_reader_test | 2.67s · built · 2.62 MB | 2.52s · built · 3.22 MB |
| jac313_store_binary_to_jtext_smoke | 3.34s · built · 2.59 MB | 3.20s · built · 3.20 MB |
| jac313_store_flag_routing_smoke | 2.81s · built · 1.86 MB | 3.21s · built · 2.21 MB |
| jac313_store_flags | 1.64s · built · 202.70 KB | 0.72s · built · 235.80 KB |
| jac313_store_jtext_persist_smoke | 2.76s · built · 2.22 MB | 2.09s · built · 2.68 MB |
| jac313_store_metric_view | 1.81s · built · 774.52 KB | 1.68s · built · 913.10 KB |
| jac313_store_metric_view_test | 1.57s · built · 766.18 KB | 1.99s · built · 900.95 KB |
| jac313_store_saturating_capacity_test | 1.15s · built · 33.16 KB | 1.06s · built · 51.75 KB |
| jac313_store_sql_persist_smoke | 1.66s · built · 898.84 KB | 1.67s · built · 1.04 MB |
| jac313_store_v002_test | 1.98s · built · 457.48 KB | 1.32s · built · 566.55 KB |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
