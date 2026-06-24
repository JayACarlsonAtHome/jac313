# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-24 18:20:37Z |
| OS | fedora-44 |
| Compiler | gcc16 |
| Build type | Debug |
| Disk | x7k |
| Size | full (xFull) |
| Group ID | 92 |
| Environment | jac313-008 · Fedora Linux 44 (Workstation Edition) (7.0.12-201.fc44.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 23 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 625,672 ms (10m 25s) |
| Peak ops/sec | 11,876,485 |
| Peak scenario | jac313_store_005_XS (jtext/off) — [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 452 | 10,571,942 | 8.38 MiB |
|  |  | jtext | 351 | 11,353,315 | 7.84 MiB |
|  |  | sql | 150 | 9,305,788 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 352 | 8,805,918 | 8.38 MiB |
|  |  | jtext | 351 | 8,270,615 | 7.85 MiB |
|  |  | sql | 2,057 | 8,124,137 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 452 | 11,687,705 | 8.38 MiB |
|  |  | jtext | 351 | 11,876,485 | 7.44 MiB |
|  |  | sql | 150 | 9,386,146 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 351 | 10,007,005 | 8.38 MiB |
|  |  | jtext | 351 | 11,022,928 | 7.44 MiB |
|  |  | sql | 1,305 | 10,014,020 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,161 | N/A | 19.81 MiB |
|  |  | jtext | 33,865 | N/A | 20.34 MiB |
|  |  | sql | 33,766 | N/A | 23.51 MiB |
| jac313_store_006_TS | on | binary | 37,828 | N/A | 19.81 MiB |
|  |  | jtext | 37,677 | N/A | 20.33 MiB |
|  |  | sql | 38,787 | N/A | 23.51 MiB |
| jac313_store_006_XS | off | binary | 33,605 | N/A | 19.81 MiB |
|  |  | jtext | 33,264 | N/A | 19.96 MiB |
|  |  | sql | 33,151 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 36,917 | N/A | 19.81 MiB |
|  |  | jtext | 36,720 | N/A | 19.96 MiB |
|  |  | sql | 37,919 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 552 | 8,572,653 | 19.82 MiB |
|  |  | jtext | 1,053 | 8,470,987 | 20.38 MiB |
|  |  | sql | 251 | 8,414,675 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 503 | 7,745,333 | 19.82 MiB |
|  |  | jtext | 1,053 | 9,505,703 | 20.38 MiB |
|  |  | sql | 2,207 | 9,503,897 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 552 | 9,874,593 | 19.82 MiB |
|  |  | jtext | 1,053 | 8,472,422 | 19.98 MiB |
|  |  | sql | 250 | 8,063,216 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 501 | 8,908,686 | 19.82 MiB |
|  |  | jtext | 1,003 | 10,807,306 | 19.98 MiB |
|  |  | sql | 2,359 | 10,629,252 | 22.66 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 251 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 94.22 KiB |
|  |  | sql | 200 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 251 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 94.24 KiB |
|  |  | sql | 1,255 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 301 | - | 104.07 KiB |
|  |  | jtext | 200 | - | 93.22 KiB |
|  |  | sql | 201 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 251 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.22 KiB |
|  |  | sql | 1,054 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 351 | - | 156.07 KiB |
|  |  | jtext | 151 | - | 139.90 KiB |
|  |  | sql | 151 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 201 | - | 156.07 KiB |
|  |  | jtext | 151 | - | 139.86 KiB |
|  |  | sql | 1,104 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 251 | - | 156.07 KiB |
|  |  | jtext | 151 | - | 138.34 KiB |
|  |  | sql | 150 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 351 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 138.34 KiB |
|  |  | sql | 1,054 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 1,456 | - | 311.94 KiB |
|  |  | jtext | 552 | - | 280.51 KiB |
|  |  | sql | 552 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 652 | - | 311.94 KiB |
|  |  | jtext | 602 | - | 280.35 KiB |
|  |  | sql | 1,456 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 552 | - | 311.94 KiB |
|  |  | jtext | 502 | - | 276.60 KiB |
|  |  | sql | 552 | - | 352.00 KiB |
| jac313_store_003_XS | on | binary | 652 | - | 311.94 KiB |
|  |  | jtext | 602 | - | 276.60 KiB |
|  |  | sql | 1,506 | - | 352.00 KiB |
| jac313_store_004_TS | off | binary | 603 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 470.39 KiB |
|  |  | sql | 502 | - | 596.00 KiB |
| jac313_store_004_TS | on | binary | 703 | - | 519.80 KiB |
|  |  | jtext | 603 | - | 470.02 KiB |
|  |  | sql | 1,557 | - | 596.00 KiB |
| jac313_store_004_XS | off | binary | 653 | - | 519.80 KiB |
|  |  | jtext | 501 | - | 463.28 KiB |
|  |  | sql | 502 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 653 | - | 519.80 KiB |
|  |  | jtext | 552 | - | 463.28 KiB |
|  |  | sql | 1,557 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 452 | 10,571,942 | 8.38 MiB |
|  |  | jtext | 351 | 11,353,315 | 7.84 MiB |
|  |  | sql | 150 | 9,305,788 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 352 | 8,805,918 | 8.38 MiB |
|  |  | jtext | 351 | 8,270,615 | 7.85 MiB |
|  |  | sql | 2,057 | 8,124,137 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 452 | 11,687,705 | 8.38 MiB |
|  |  | jtext | 351 | 11,876,485 | 7.44 MiB |
|  |  | sql | 150 | 9,386,146 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 351 | 10,007,005 | 8.38 MiB |
|  |  | jtext | 351 | 11,022,928 | 7.44 MiB |
|  |  | sql | 1,305 | 10,014,020 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,161 | N/A | 19.81 MiB |
|  |  | jtext | 33,865 | N/A | 20.34 MiB |
|  |  | sql | 33,766 | N/A | 23.51 MiB |
| jac313_store_006_TS | on | binary | 37,828 | N/A | 19.81 MiB |
|  |  | jtext | 37,677 | N/A | 20.33 MiB |
|  |  | sql | 38,787 | N/A | 23.51 MiB |
| jac313_store_006_XS | off | binary | 33,605 | N/A | 19.81 MiB |
|  |  | jtext | 33,264 | N/A | 19.96 MiB |
|  |  | sql | 33,151 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 36,917 | N/A | 19.81 MiB |
|  |  | jtext | 36,720 | N/A | 19.96 MiB |
|  |  | sql | 37,919 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 552 | 8,572,653 | 19.82 MiB |
|  |  | jtext | 1,053 | 8,470,987 | 20.38 MiB |
|  |  | sql | 251 | 8,414,675 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 503 | 7,745,333 | 19.82 MiB |
|  |  | jtext | 1,053 | 9,505,703 | 20.38 MiB |
|  |  | sql | 2,207 | 9,503,897 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 552 | 9,874,593 | 19.82 MiB |
|  |  | jtext | 1,053 | 8,472,422 | 19.98 MiB |
|  |  | sql | 250 | 8,063,216 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 501 | 8,908,686 | 19.82 MiB |
|  |  | jtext | 1,003 | 10,807,306 | 19.98 MiB |
|  |  | sql | 2,359 | 10,629,252 | 22.66 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 251 | - | 104.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 251 | - | 104.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 201 | - | 94.22 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 201 | - | 94.24 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 201 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 201 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 200 | - | 152.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 1,255 | - | 152.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 301 | - | 104.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 251 | - | 104.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 200 | - | 93.22 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 201 | - | 93.22 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 200 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 201 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 201 | - | 148.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 1,054 | - | 148.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 351 | - | 156.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 201 | - | 156.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 151 | - | 139.90 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 151 | - | 139.86 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 150 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 150 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 151 | - | 200.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 1,104 | - | 200.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 251 | - | 156.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 351 | - | 156.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 151 | - | 138.34 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 150 | - | 138.34 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 150 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 150 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 150 | - | 192.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 1,054 | - | 192.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 1,456 | - | 311.94 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 652 | - | 311.94 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 552 | - | 280.51 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 602 | - | 280.35 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 553 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 603 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 552 | - | 368.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 1,456 | - | 368.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 552 | - | 311.94 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 652 | - | 311.94 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 502 | - | 276.60 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 602 | - | 276.60 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 552 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 603 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 552 | - | 352.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 1,506 | - | 352.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 603 | - | 519.80 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 703 | - | 519.80 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 502 | - | 470.39 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 603 | - | 470.02 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 502 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 602 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 502 | - | 596.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 1,557 | - | 596.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 653 | - | 519.80 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 653 | - | 519.80 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 501 | - | 463.28 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 552 | - | 463.28 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 502 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 552 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 502 | - | 572.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 1,557 | - | 572.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 452 | 10,571,942 | 8.38 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 352 | 8,805,918 | 8.38 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 351 | 11,353,315 | 7.84 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 351 | 8,270,615 | 7.85 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 103 | 9,528,347 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 103 | 10,305,029 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 150 | 9,305,788 | 7.42 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 2,057 | 8,124,137 | 7.42 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 452 | 11,687,705 | 8.38 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 351 | 10,007,005 | 8.38 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 351 | 11,876,485 | 7.44 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 351 | 11,022,928 | 7.44 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 100 | 11,152,002 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 100 | 10,461,345 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 150 | 9,386,146 | 7.13 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 1,305 | 10,014,020 | 7.13 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 34,161 | N/A | 19.81 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 37,828 | N/A | 19.81 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 33,865 | N/A | 20.34 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 37,677 | N/A | 20.33 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 33,659 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 37,479 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 33,766 | N/A | 23.51 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 38,787 | N/A | 23.51 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 33,605 | N/A | 19.81 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 36,917 | N/A | 19.81 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 33,264 | N/A | 19.96 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 36,720 | N/A | 19.96 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 33,054 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 36,519 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 33,151 | N/A | 22.64 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 37,919 | N/A | 22.64 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 552 | 8,572,653 | 19.82 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 503 | 7,745,333 | 19.82 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 1,053 | 8,470,987 | 20.38 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 1,053 | 9,505,703 | 20.38 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 100 | 9,957,184 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 104 | 9,104,151 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 251 | 8,414,675 | 23.51 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 2,207 | 9,503,897 | 23.51 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 552 | 9,874,593 | 19.82 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 501 | 8,908,686 | 19.82 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 1,053 | 8,472,422 | 19.98 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 1,003 | 10,807,306 | 19.98 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 100 | 8,785,802 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 102 | 10,472,301 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 250 | 8,063,216 | 22.66 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 2,359 | 10,629,252 | 22.66 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 2,670 | 11,502,847 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc16_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 2,480 | 11,441,909 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc16_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc16_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 100 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc16_unit_off.log) |
