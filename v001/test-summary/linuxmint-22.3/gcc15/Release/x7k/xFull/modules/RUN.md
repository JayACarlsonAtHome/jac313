# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-21 00:40:33Z |
| OS | linuxmint-22.3 |
| Compiler | gcc15 |
| Build type | Release |
| Disk | x7k |
| Size | full (xFull) |
| Group ID | 81 |
| Environment | jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 620,173 ms (10m 20s) |
| Build straight (no modules) | 102,079 ms (102,079 ms (1m 42s)) |
| Build modules only | 5,932 ms (5,932 ms (5s)) |
| Build with modules (rest) | 107,375 ms (107,375 ms (1m 47s)) |
| Peak ops/sec | 3,234,487 |
| Peak scenario | jac313_store_008_XS (flags/off) — [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 461 | 1,974,217 | 8.38 MiB |
|  |  | jtext | 509 | 2,388,744 | 7.92 MiB |
|  |  | sql | 513 | 1,699,495 | 7.41 MiB |
| jac313_store_005_TS | on | binary | 563 | 2,236,436 | 8.38 MiB |
|  |  | jtext | 416 | 2,327,693 | 7.92 MiB |
|  |  | sql | 920 | 1,911,023 | 7.41 MiB |
| jac313_store_005_XS | off | binary | 505 | 1,840,739 | 8.38 MiB |
|  |  | jtext | 405 | 2,391,258 | 7.44 MiB |
|  |  | sql | 459 | 2,128,928 | 7.12 MiB |
| jac313_store_005_XS | on | binary | 491 | 1,915,122 | 8.38 MiB |
|  |  | jtext | 461 | 2,588,059 | 7.44 MiB |
|  |  | sql | 1,010 | 2,687,450 | 7.12 MiB |
| jac313_store_006_TS | off | binary | 33,964 | N/A | 19.81 MiB |
|  |  | jtext | 34,006 | N/A | 20.40 MiB |
|  |  | sql | 34,077 | N/A | 23.47 MiB |
| jac313_store_006_TS | on | binary | 37,903 | N/A | 19.81 MiB |
|  |  | jtext | 37,811 | N/A | 20.40 MiB |
|  |  | sql | 38,415 | N/A | 23.47 MiB |
| jac313_store_006_XS | off | binary | 33,371 | N/A | 19.81 MiB |
|  |  | jtext | 33,315 | N/A | 19.96 MiB |
|  |  | sql | 33,307 | N/A | 22.59 MiB |
| jac313_store_006_XS | on | binary | 36,837 | N/A | 19.81 MiB |
|  |  | jtext | 36,873 | N/A | 19.96 MiB |
|  |  | sql | 37,187 | N/A | 22.59 MiB |
| jac313_store_007_TS | off | binary | 569 | 2,355,879 | 19.82 MiB |
|  |  | jtext | 815 | 1,922,116 | 20.45 MiB |
|  |  | sql | 809 | 2,331,546 | 23.49 MiB |
| jac313_store_007_TS | on | binary | 623 | 1,984,363 | 19.82 MiB |
|  |  | jtext | 967 | 2,281,022 | 20.45 MiB |
|  |  | sql | 1,780 | 2,508,340 | 23.49 MiB |
| jac313_store_007_XS | off | binary | 556 | 2,352,997 | 19.82 MiB |
|  |  | jtext | 857 | 2,227,817 | 19.98 MiB |
|  |  | sql | 764 | 2,456,942 | 22.61 MiB |
| jac313_store_007_XS | on | binary | 611 | 2,158,475 | 19.82 MiB |
|  |  | jtext | 760 | 2,043,861 | 19.98 MiB |
|  |  | sql | 1,678 | 2,475,554 | 22.61 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 203 | - | 104.06 KiB |
|  |  | jtext | 203 | - | 94.70 KiB |
|  |  | sql | 206 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 253 | - | 104.06 KiB |
|  |  | jtext | 202 | - | 94.70 KiB |
|  |  | sql | 455 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 453 | - | 104.06 KiB |
|  |  | jtext | 152 | - | 93.20 KiB |
|  |  | sql | 202 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 253 | - | 104.06 KiB |
|  |  | jtext | 202 | - | 93.20 KiB |
|  |  | sql | 453 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 202 | - | 156.06 KiB |
|  |  | jtext | 152 | - | 140.58 KiB |
|  |  | sql | 151 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 204 | - | 156.06 KiB |
|  |  | jtext | 153 | - | 140.58 KiB |
|  |  | sql | 453 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 202 | - | 156.06 KiB |
|  |  | jtext | 152 | - | 138.33 KiB |
|  |  | sql | 152 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 202 | - | 156.06 KiB |
|  |  | jtext | 152 | - | 138.33 KiB |
|  |  | sql | 403 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 555 | - | 311.94 KiB |
|  |  | jtext | 455 | - | 281.08 KiB |
|  |  | sql | 505 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 604 | - | 311.94 KiB |
|  |  | jtext | 607 | - | 281.08 KiB |
|  |  | sql | 862 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 554 | - | 311.94 KiB |
|  |  | jtext | 504 | - | 276.58 KiB |
|  |  | sql | 508 | - | 356.00 KiB |
| jac313_store_003_XS | on | binary | 604 | - | 311.94 KiB |
|  |  | jtext | 554 | - | 276.58 KiB |
|  |  | sql | 856 | - | 356.00 KiB |
| jac313_store_004_TS | off | binary | 556 | - | 519.80 KiB |
|  |  | jtext | 504 | - | 470.89 KiB |
|  |  | sql | 503 | - | 596.00 KiB |
| jac313_store_004_TS | on | binary | 658 | - | 519.80 KiB |
|  |  | jtext | 606 | - | 470.85 KiB |
|  |  | sql | 855 | - | 596.00 KiB |
| jac313_store_004_XS | off | binary | 553 | - | 519.80 KiB |
|  |  | jtext | 504 | - | 463.26 KiB |
|  |  | sql | 503 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 604 | - | 519.80 KiB |
|  |  | jtext | 553 | - | 463.26 KiB |
|  |  | sql | 907 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 461 | 1,974,217 | 8.38 MiB |
|  |  | jtext | 509 | 2,388,744 | 7.92 MiB |
|  |  | sql | 513 | 1,699,495 | 7.41 MiB |
| jac313_store_005_TS | on | binary | 563 | 2,236,436 | 8.38 MiB |
|  |  | jtext | 416 | 2,327,693 | 7.92 MiB |
|  |  | sql | 920 | 1,911,023 | 7.41 MiB |
| jac313_store_005_XS | off | binary | 505 | 1,840,739 | 8.38 MiB |
|  |  | jtext | 405 | 2,391,258 | 7.44 MiB |
|  |  | sql | 459 | 2,128,928 | 7.12 MiB |
| jac313_store_005_XS | on | binary | 491 | 1,915,122 | 8.38 MiB |
|  |  | jtext | 461 | 2,588,059 | 7.44 MiB |
|  |  | sql | 1,010 | 2,687,450 | 7.12 MiB |
| jac313_store_006_TS | off | binary | 33,964 | N/A | 19.81 MiB |
|  |  | jtext | 34,006 | N/A | 20.40 MiB |
|  |  | sql | 34,077 | N/A | 23.47 MiB |
| jac313_store_006_TS | on | binary | 37,903 | N/A | 19.81 MiB |
|  |  | jtext | 37,811 | N/A | 20.40 MiB |
|  |  | sql | 38,415 | N/A | 23.47 MiB |
| jac313_store_006_XS | off | binary | 33,371 | N/A | 19.81 MiB |
|  |  | jtext | 33,315 | N/A | 19.96 MiB |
|  |  | sql | 33,307 | N/A | 22.59 MiB |
| jac313_store_006_XS | on | binary | 36,837 | N/A | 19.81 MiB |
|  |  | jtext | 36,873 | N/A | 19.96 MiB |
|  |  | sql | 37,187 | N/A | 22.59 MiB |
| jac313_store_007_TS | off | binary | 569 | 2,355,879 | 19.82 MiB |
|  |  | jtext | 815 | 1,922,116 | 20.45 MiB |
|  |  | sql | 809 | 2,331,546 | 23.49 MiB |
| jac313_store_007_TS | on | binary | 623 | 1,984,363 | 19.82 MiB |
|  |  | jtext | 967 | 2,281,022 | 20.45 MiB |
|  |  | sql | 1,780 | 2,508,340 | 23.49 MiB |
| jac313_store_007_XS | off | binary | 556 | 2,352,997 | 19.82 MiB |
|  |  | jtext | 857 | 2,227,817 | 19.98 MiB |
|  |  | sql | 764 | 2,456,942 | 22.61 MiB |
| jac313_store_007_XS | on | binary | 611 | 2,158,475 | 19.82 MiB |
|  |  | jtext | 760 | 2,043,861 | 19.98 MiB |
|  |  | sql | 1,678 | 2,475,554 | 22.61 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 203 | - | 104.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc15_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 253 | - | 104.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc15_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 203 | - | 94.70 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc15_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 202 | - | 94.70 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc15_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 202 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc15_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 202 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc15_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 206 | - | 152.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc15_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 455 | - | 152.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc15_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 453 | - | 104.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc15_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 253 | - | 104.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc15_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 152 | - | 93.20 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc15_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 202 | - | 93.20 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc15_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 202 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc15_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 204 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc15_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 202 | - | 148.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc15_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 453 | - | 148.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc15_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 202 | - | 156.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc15_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 204 | - | 156.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc15_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 152 | - | 140.58 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc15_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 153 | - | 140.58 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc15_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 153 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc15_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 152 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc15_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 151 | - | 200.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc15_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 453 | - | 200.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc15_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 202 | - | 156.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc15_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 202 | - | 156.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc15_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 152 | - | 138.33 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc15_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 152 | - | 138.33 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc15_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 152 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc15_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 152 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc15_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 152 | - | 192.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc15_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 403 | - | 192.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc15_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 555 | - | 311.94 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc15_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 604 | - | 311.94 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc15_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 455 | - | 281.08 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc15_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 607 | - | 281.08 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc15_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 504 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc15_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 606 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc15_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 505 | - | 368.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc15_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 862 | - | 368.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc15_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 554 | - | 311.94 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc15_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 604 | - | 311.94 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc15_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 504 | - | 276.58 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc15_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 554 | - | 276.58 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc15_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 507 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc15_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 608 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc15_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 508 | - | 356.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc15_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 856 | - | 356.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc15_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 556 | - | 519.80 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc15_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 658 | - | 519.80 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc15_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 504 | - | 470.89 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc15_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 606 | - | 470.85 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc15_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 508 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc15_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 554 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc15_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 503 | - | 596.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc15_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 855 | - | 596.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc15_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 553 | - | 519.80 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc15_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 604 | - | 519.80 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc15_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 504 | - | 463.26 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc15_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 553 | - | 463.26 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc15_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 503 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc15_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 554 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc15_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 503 | - | 572.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc15_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 907 | - | 572.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc15_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 461 | 1,974,217 | 8.38 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 563 | 2,236,436 | 8.38 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 509 | 2,388,744 | 7.92 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 416 | 2,327,693 | 7.92 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 262 | 2,175,190 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 263 | 2,681,684 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 513 | 1,699,495 | 7.41 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 920 | 1,911,023 | 7.41 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 505 | 1,840,739 | 8.38 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 491 | 1,915,122 | 8.38 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 405 | 2,391,258 | 7.44 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 461 | 2,588,059 | 7.44 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 253 | 2,448,820 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 214 | 2,562,854 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 459 | 2,128,928 | 7.12 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc15_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 1,010 | 2,687,450 | 7.12 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc15_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 33,964 | N/A | 19.81 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc15_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 37,903 | N/A | 19.81 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc15_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 34,006 | N/A | 20.40 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc15_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 37,811 | N/A | 20.40 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc15_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 33,777 | N/A | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc15_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 37,644 | N/A | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc15_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 34,077 | N/A | 23.47 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc15_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 38,415 | N/A | 23.47 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc15_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 33,371 | N/A | 19.81 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc15_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 36,837 | N/A | 19.81 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc15_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 33,315 | N/A | 19.96 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc15_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 36,873 | N/A | 19.96 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc15_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 33,233 | N/A | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc15_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 36,561 | N/A | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc15_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 33,307 | N/A | 22.59 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc15_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 37,187 | N/A | 22.59 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc15_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 569 | 2,355,879 | 19.82 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 623 | 1,984,363 | 19.82 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 815 | 1,922,116 | 20.45 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 967 | 2,281,022 | 20.45 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 311 | 2,152,992 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 258 | 3,071,442 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 809 | 2,331,546 | 23.49 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 1,780 | 2,508,340 | 23.49 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 556 | 2,352,997 | 19.82 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 611 | 2,158,475 | 19.82 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 857 | 2,227,817 | 19.98 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 760 | 2,043,861 | 19.98 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 288 | 2,299,273 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 258 | 2,886,836 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 764 | 2,456,942 | 22.61 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 1,678 | 2,475,554 | 22.61 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 3,073 | 3,010,389 | - | - | [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 3,056 | 3,234,487 | - | - | [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 51 | - | - | - | [gcc15_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc15_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 51 | - | - | - | [gcc15_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc15_unit_off.log) |
