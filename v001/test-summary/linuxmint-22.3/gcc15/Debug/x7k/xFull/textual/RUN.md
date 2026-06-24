# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-21 00:28:45Z |
| OS | linuxmint-22.3 |
| Compiler | gcc15 |
| Build type | Debug |
| Disk | x7k |
| Size | full (xFull) |
| Group ID | 80 |
| Environment | jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 657,796 ms (10m 57s) |
| Peak ops/sec | 1,488,472 |
| Peak scenario | jac313_store_005_XS (jtext/off) — [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 830 | 1,281,312 | 8.38 MiB |
|  |  | jtext | 1,756 | 1,016,611 | 7.92 MiB |
|  |  | sql | 789 | 1,209,409 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 786 | 1,028,627 | 8.38 MiB |
|  |  | jtext | 1,877 | 943,361 | 7.92 MiB |
|  |  | sql | 1,286 | 1,096,672 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 813 | 1,090,917 | 8.38 MiB |
|  |  | jtext | 1,786 | 1,488,472 | 7.44 MiB |
|  |  | sql | 738 | 1,358,917 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 793 | 1,133,144 | 8.38 MiB |
|  |  | jtext | 1,746 | 1,070,469 | 7.44 MiB |
|  |  | sql | 1,262 | 1,102,013 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,155 | N/A | 19.81 MiB |
|  |  | jtext | 34,478 | N/A | 20.42 MiB |
|  |  | sql | 34,268 | N/A | 23.48 MiB |
| jac313_store_006_TS | on | binary | 37,986 | N/A | 19.81 MiB |
|  |  | jtext | 38,435 | N/A | 20.41 MiB |
|  |  | sql | 38,534 | N/A | 23.48 MiB |
| jac313_store_006_XS | off | binary | 33,593 | N/A | 19.81 MiB |
|  |  | jtext | 34,050 | N/A | 19.96 MiB |
|  |  | sql | 33,509 | N/A | 22.62 MiB |
| jac313_store_006_XS | on | binary | 37,245 | N/A | 19.81 MiB |
|  |  | jtext | 37,338 | N/A | 19.96 MiB |
|  |  | sql | 37,533 | N/A | 22.62 MiB |
| jac313_store_007_TS | off | binary | 919 | 1,303,696 | 19.82 MiB |
|  |  | jtext | 4,121 | 1,160,685 | 20.45 MiB |
|  |  | sql | 1,222 | 1,016,291 | 23.49 MiB |
| jac313_store_007_TS | on | binary | 979 | 986,865 | 19.82 MiB |
|  |  | jtext | 4,030 | 1,099,360 | 20.45 MiB |
|  |  | sql | 2,022 | 1,421,424 | 23.49 MiB |
| jac313_store_007_XS | off | binary | 877 | 1,301,694 | 19.82 MiB |
|  |  | jtext | 4,245 | 1,253,730 | 19.98 MiB |
|  |  | sql | 1,178 | 1,238,344 | 22.63 MiB |
| jac313_store_007_XS | on | binary | 916 | 1,072,927 | 19.82 MiB |
|  |  | jtext | 3,549 | 1,100,631 | 19.98 MiB |
|  |  | sql | 2,211 | 1,276,047 | 22.63 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 252 | - | 104.06 KiB |
|  |  | jtext | 252 | - | 94.68 KiB |
|  |  | sql | 202 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 302 | - | 104.06 KiB |
|  |  | jtext | 252 | - | 94.68 KiB |
|  |  | sql | 503 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 252 | - | 104.06 KiB |
|  |  | jtext | 202 | - | 93.18 KiB |
|  |  | sql | 202 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 304 | - | 104.06 KiB |
|  |  | jtext | 252 | - | 93.18 KiB |
|  |  | sql | 453 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 201 | - | 156.06 KiB |
|  |  | jtext | 152 | - | 140.56 KiB |
|  |  | sql | 152 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 202 | - | 156.06 KiB |
|  |  | jtext | 201 | - | 140.69 KiB |
|  |  | sql | 403 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 202 | - | 156.06 KiB |
|  |  | jtext | 202 | - | 138.31 KiB |
|  |  | sql | 153 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 202 | - | 156.06 KiB |
|  |  | jtext | 202 | - | 138.31 KiB |
|  |  | sql | 404 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 605 | - | 311.94 KiB |
|  |  | jtext | 554 | - | 281.43 KiB |
|  |  | sql | 554 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 606 | - | 311.94 KiB |
|  |  | jtext | 705 | - | 282.20 KiB |
|  |  | sql | 904 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 606 | - | 311.94 KiB |
|  |  | jtext | 607 | - | 276.56 KiB |
|  |  | sql | 504 | - | 356.00 KiB |
| jac313_store_003_XS | on | binary | 656 | - | 311.94 KiB |
|  |  | jtext | 659 | - | 276.56 KiB |
|  |  | sql | 961 | - | 356.00 KiB |
| jac313_store_004_TS | off | binary | 607 | - | 519.80 KiB |
|  |  | jtext | 659 | - | 473.01 KiB |
|  |  | sql | 503 | - | 596.00 KiB |
| jac313_store_004_TS | on | binary | 656 | - | 519.80 KiB |
|  |  | jtext | 707 | - | 472.18 KiB |
|  |  | sql | 1,007 | - | 596.00 KiB |
| jac313_store_004_XS | off | binary | 606 | - | 519.80 KiB |
|  |  | jtext | 604 | - | 463.24 KiB |
|  |  | sql | 504 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 656 | - | 519.80 KiB |
|  |  | jtext | 704 | - | 463.24 KiB |
|  |  | sql | 955 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 830 | 1,281,312 | 8.38 MiB |
|  |  | jtext | 1,756 | 1,016,611 | 7.92 MiB |
|  |  | sql | 789 | 1,209,409 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 786 | 1,028,627 | 8.38 MiB |
|  |  | jtext | 1,877 | 943,361 | 7.92 MiB |
|  |  | sql | 1,286 | 1,096,672 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 813 | 1,090,917 | 8.38 MiB |
|  |  | jtext | 1,786 | 1,488,472 | 7.44 MiB |
|  |  | sql | 738 | 1,358,917 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 793 | 1,133,144 | 8.38 MiB |
|  |  | jtext | 1,746 | 1,070,469 | 7.44 MiB |
|  |  | sql | 1,262 | 1,102,013 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,155 | N/A | 19.81 MiB |
|  |  | jtext | 34,478 | N/A | 20.42 MiB |
|  |  | sql | 34,268 | N/A | 23.48 MiB |
| jac313_store_006_TS | on | binary | 37,986 | N/A | 19.81 MiB |
|  |  | jtext | 38,435 | N/A | 20.41 MiB |
|  |  | sql | 38,534 | N/A | 23.48 MiB |
| jac313_store_006_XS | off | binary | 33,593 | N/A | 19.81 MiB |
|  |  | jtext | 34,050 | N/A | 19.96 MiB |
|  |  | sql | 33,509 | N/A | 22.62 MiB |
| jac313_store_006_XS | on | binary | 37,245 | N/A | 19.81 MiB |
|  |  | jtext | 37,338 | N/A | 19.96 MiB |
|  |  | sql | 37,533 | N/A | 22.62 MiB |
| jac313_store_007_TS | off | binary | 919 | 1,303,696 | 19.82 MiB |
|  |  | jtext | 4,121 | 1,160,685 | 20.45 MiB |
|  |  | sql | 1,222 | 1,016,291 | 23.49 MiB |
| jac313_store_007_TS | on | binary | 979 | 986,865 | 19.82 MiB |
|  |  | jtext | 4,030 | 1,099,360 | 20.45 MiB |
|  |  | sql | 2,022 | 1,421,424 | 23.49 MiB |
| jac313_store_007_XS | off | binary | 877 | 1,301,694 | 19.82 MiB |
|  |  | jtext | 4,245 | 1,253,730 | 19.98 MiB |
|  |  | sql | 1,178 | 1,238,344 | 22.63 MiB |
| jac313_store_007_XS | on | binary | 916 | 1,072,927 | 19.82 MiB |
|  |  | jtext | 3,549 | 1,100,631 | 19.98 MiB |
|  |  | sql | 2,211 | 1,276,047 | 22.63 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 252 | - | 104.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc15_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 302 | - | 104.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc15_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 252 | - | 94.68 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc15_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 252 | - | 94.68 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc15_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 202 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc15_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 202 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc15_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 202 | - | 152.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc15_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 503 | - | 152.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc15_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 252 | - | 104.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc15_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 304 | - | 104.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc15_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 202 | - | 93.18 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc15_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 252 | - | 93.18 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc15_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 202 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc15_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 202 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc15_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 202 | - | 148.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc15_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 453 | - | 148.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc15_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 201 | - | 156.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc15_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 202 | - | 156.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc15_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 152 | - | 140.56 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc15_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 201 | - | 140.69 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc15_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 157 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc15_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 151 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc15_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 152 | - | 200.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc15_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 403 | - | 200.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc15_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 202 | - | 156.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc15_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 202 | - | 156.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc15_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 202 | - | 138.31 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc15_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 202 | - | 138.31 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc15_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 153 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc15_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 152 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc15_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 153 | - | 192.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc15_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 404 | - | 192.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc15_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 605 | - | 311.94 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc15_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 606 | - | 311.94 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc15_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 554 | - | 281.43 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc15_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 705 | - | 282.20 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc15_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 508 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc15_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 606 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc15_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 554 | - | 368.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc15_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 904 | - | 368.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc15_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 606 | - | 311.94 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc15_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 656 | - | 311.94 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc15_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 607 | - | 276.56 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc15_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 659 | - | 276.56 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc15_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 504 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc15_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 554 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc15_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 504 | - | 356.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc15_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 961 | - | 356.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc15_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 607 | - | 519.80 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc15_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 656 | - | 519.80 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc15_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 659 | - | 473.01 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc15_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 707 | - | 472.18 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc15_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 503 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc15_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 604 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc15_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 503 | - | 596.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc15_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 1,007 | - | 596.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc15_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 606 | - | 519.80 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc15_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 656 | - | 519.80 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc15_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 604 | - | 463.24 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc15_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 704 | - | 463.24 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc15_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 504 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc15_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 555 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc15_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 504 | - | 572.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc15_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 955 | - | 572.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc15_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 830 | 1,281,312 | 8.38 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 786 | 1,028,627 | 8.38 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 1,756 | 1,016,611 | 7.92 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 1,877 | 943,361 | 7.92 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 427 | 1,353,638 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 467 | 1,115,922 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 789 | 1,209,409 | 7.42 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 1,286 | 1,096,672 | 7.42 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 813 | 1,090,917 | 8.38 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 793 | 1,133,144 | 8.38 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 1,786 | 1,488,472 | 7.44 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 1,746 | 1,070,469 | 7.44 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 368 | 1,488,339 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 383 | 1,420,273 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 738 | 1,358,917 | 7.13 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc15_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 1,262 | 1,102,013 | 7.13 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc15_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 34,155 | N/A | 19.81 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc15_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 37,986 | N/A | 19.81 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc15_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 34,478 | N/A | 20.42 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc15_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 38,435 | N/A | 20.41 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc15_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 33,913 | N/A | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc15_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 37,750 | N/A | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc15_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 34,268 | N/A | 23.48 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc15_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 38,534 | N/A | 23.48 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc15_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 33,593 | N/A | 19.81 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc15_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 37,245 | N/A | 19.81 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc15_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 34,050 | N/A | 19.96 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc15_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 37,338 | N/A | 19.96 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc15_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 33,259 | N/A | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc15_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 36,727 | N/A | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc15_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 33,509 | N/A | 22.62 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc15_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 37,533 | N/A | 22.62 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc15_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 919 | 1,303,696 | 19.82 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 979 | 986,865 | 19.82 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 4,121 | 1,160,685 | 20.45 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 4,030 | 1,099,360 | 20.45 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 500 | 1,154,748 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 517 | 1,034,468 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 1,222 | 1,016,291 | 23.49 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 2,022 | 1,421,424 | 23.49 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 877 | 1,301,694 | 19.82 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 916 | 1,072,927 | 19.82 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 4,245 | 1,253,730 | 19.98 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 3,549 | 1,100,631 | 19.98 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 479 | 1,102,026 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 436 | 1,327,563 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 1,178 | 1,238,344 | 22.63 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 2,211 | 1,276,047 | 22.63 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 6,528 | 1,118,284 | - | - | [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 5,982 | 1,458,855 | - | - | [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 51 | - | - | - | [gcc15_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc15_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 202 | - | - | - | [gcc15_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc15_unit_off.log) |
