# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-24 17:17:09Z |
| OS | fedora-44 |
| Compiler | clang |
| Build type | Debug |
| Disk | x7k |
| Size | full (xFull) |
| Group ID | 88 |
| Environment | jac313-008 · Fedora Linux 44 (Workstation Edition) (7.0.12-201.fc44.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 23 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 625,853 ms (10m 25s) |
| Peak ops/sec | 10,738,716 |
| Peak scenario | jac313_store_008_TS (flags/off) — [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 453 | 9,639,483 | 8.38 MiB |
|  |  | jtext | 401 | 10,332,713 | 7.84 MiB |
|  |  | sql | 200 | 8,610,298 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 451 | 9,754,194 | 8.38 MiB |
|  |  | jtext | 351 | 8,919,811 | 7.84 MiB |
|  |  | sql | 1,506 | 9,755,146 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 551 | 9,250,694 | 8.38 MiB |
|  |  | jtext | 351 | 9,069,472 | 7.44 MiB |
|  |  | sql | 150 | 9,710,623 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 351 | 9,244,707 | 8.38 MiB |
|  |  | jtext | 351 | 10,080,645 | 7.44 MiB |
|  |  | sql | 1,255 | 9,416,196 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,006 | N/A | 19.81 MiB |
|  |  | jtext | 33,803 | N/A | 20.34 MiB |
|  |  | sql | 33,754 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 37,872 | N/A | 19.81 MiB |
|  |  | jtext | 37,671 | N/A | 20.33 MiB |
|  |  | sql | 38,872 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 33,561 | N/A | 19.81 MiB |
|  |  | jtext | 33,253 | N/A | 19.96 MiB |
|  |  | sql | 33,153 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 36,927 | N/A | 19.81 MiB |
|  |  | jtext | 36,667 | N/A | 19.96 MiB |
|  |  | sql | 37,770 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 602 | 6,880,418 | 19.82 MiB |
|  |  | jtext | 1,003 | 7,050,197 | 20.39 MiB |
|  |  | sql | 252 | 8,438,106 | 23.52 MiB |
| jac313_store_007_TS | on | binary | 554 | 7,317,966 | 19.82 MiB |
|  |  | jtext | 953 | 7,818,608 | 20.38 MiB |
|  |  | sql | 2,710 | 9,521,089 | 23.52 MiB |
| jac313_store_007_XS | off | binary | 503 | 8,663,259 | 19.82 MiB |
|  |  | jtext | 1,006 | 9,324,009 | 19.98 MiB |
|  |  | sql | 250 | 8,823,789 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 551 | 7,423,905 | 19.82 MiB |
|  |  | jtext | 1,003 | 8,448,087 | 19.98 MiB |
|  |  | sql | 2,358 | 8,759,636 | 22.66 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 301 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 94.22 KiB |
|  |  | sql | 201 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 301 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 94.22 KiB |
|  |  | sql | 1,155 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 301 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.22 KiB |
|  |  | sql | 201 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 452 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.22 KiB |
|  |  | sql | 1,155 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 301 | - | 156.07 KiB |
|  |  | jtext | 151 | - | 139.85 KiB |
|  |  | sql | 151 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 301 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 139.84 KiB |
|  |  | sql | 1,205 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 301 | - | 156.07 KiB |
|  |  | jtext | 151 | - | 138.34 KiB |
|  |  | sql | 150 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 352 | - | 156.07 KiB |
|  |  | jtext | 151 | - | 138.34 KiB |
|  |  | sql | 1,004 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 703 | - | 311.94 KiB |
|  |  | jtext | 553 | - | 280.74 KiB |
|  |  | sql | 552 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 703 | - | 311.94 KiB |
|  |  | jtext | 602 | - | 280.53 KiB |
|  |  | sql | 1,506 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 653 | - | 311.94 KiB |
|  |  | jtext | 552 | - | 276.60 KiB |
|  |  | sql | 552 | - | 356.00 KiB |
| jac313_store_003_XS | on | binary | 804 | - | 311.94 KiB |
|  |  | jtext | 602 | - | 276.60 KiB |
|  |  | sql | 1,506 | - | 356.00 KiB |
| jac313_store_004_TS | off | binary | 703 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 470.22 KiB |
|  |  | sql | 502 | - | 592.00 KiB |
| jac313_store_004_TS | on | binary | 753 | - | 519.80 KiB |
|  |  | jtext | 603 | - | 470.29 KiB |
|  |  | sql | 1,607 | - | 592.00 KiB |
| jac313_store_004_XS | off | binary | 653 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 463.28 KiB |
|  |  | sql | 502 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 703 | - | 519.80 KiB |
|  |  | jtext | 552 | - | 463.28 KiB |
|  |  | sql | 1,606 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 453 | 9,639,483 | 8.38 MiB |
|  |  | jtext | 401 | 10,332,713 | 7.84 MiB |
|  |  | sql | 200 | 8,610,298 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 451 | 9,754,194 | 8.38 MiB |
|  |  | jtext | 351 | 8,919,811 | 7.84 MiB |
|  |  | sql | 1,506 | 9,755,146 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 551 | 9,250,694 | 8.38 MiB |
|  |  | jtext | 351 | 9,069,472 | 7.44 MiB |
|  |  | sql | 150 | 9,710,623 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 351 | 9,244,707 | 8.38 MiB |
|  |  | jtext | 351 | 10,080,645 | 7.44 MiB |
|  |  | sql | 1,255 | 9,416,196 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,006 | N/A | 19.81 MiB |
|  |  | jtext | 33,803 | N/A | 20.34 MiB |
|  |  | sql | 33,754 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 37,872 | N/A | 19.81 MiB |
|  |  | jtext | 37,671 | N/A | 20.33 MiB |
|  |  | sql | 38,872 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 33,561 | N/A | 19.81 MiB |
|  |  | jtext | 33,253 | N/A | 19.96 MiB |
|  |  | sql | 33,153 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 36,927 | N/A | 19.81 MiB |
|  |  | jtext | 36,667 | N/A | 19.96 MiB |
|  |  | sql | 37,770 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 602 | 6,880,418 | 19.82 MiB |
|  |  | jtext | 1,003 | 7,050,197 | 20.39 MiB |
|  |  | sql | 252 | 8,438,106 | 23.52 MiB |
| jac313_store_007_TS | on | binary | 554 | 7,317,966 | 19.82 MiB |
|  |  | jtext | 953 | 7,818,608 | 20.38 MiB |
|  |  | sql | 2,710 | 9,521,089 | 23.52 MiB |
| jac313_store_007_XS | off | binary | 503 | 8,663,259 | 19.82 MiB |
|  |  | jtext | 1,006 | 9,324,009 | 19.98 MiB |
|  |  | sql | 250 | 8,823,789 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 551 | 7,423,905 | 19.82 MiB |
|  |  | jtext | 1,003 | 8,448,087 | 19.98 MiB |
|  |  | sql | 2,358 | 8,759,636 | 22.66 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 301 | - | 104.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 301 | - | 104.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 201 | - | 94.22 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 201 | - | 94.22 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 201 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 201 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 201 | - | 152.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 1,155 | - | 152.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 301 | - | 104.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 452 | - | 104.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 201 | - | 93.22 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 201 | - | 93.22 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 201 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 201 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 201 | - | 148.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 1,155 | - | 148.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 301 | - | 156.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 301 | - | 156.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 151 | - | 139.85 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 150 | - | 139.84 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 151 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 151 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 151 | - | 200.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 1,205 | - | 200.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 301 | - | 156.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 352 | - | 156.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 151 | - | 138.34 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 151 | - | 138.34 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 150 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 150 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 150 | - | 192.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 1,004 | - | 192.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 703 | - | 311.94 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 703 | - | 311.94 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 553 | - | 280.74 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 602 | - | 280.53 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 553 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 603 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 552 | - | 368.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 1,506 | - | 368.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 653 | - | 311.94 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 804 | - | 311.94 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 552 | - | 276.60 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 602 | - | 276.60 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 502 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 603 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 552 | - | 356.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 1,506 | - | 356.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 703 | - | 519.80 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 753 | - | 519.80 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 502 | - | 470.22 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 603 | - | 470.29 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 502 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 603 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 502 | - | 592.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 1,607 | - | 592.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 653 | - | 519.80 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 703 | - | 519.80 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 502 | - | 463.28 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 552 | - | 463.28 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 502 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 552 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 502 | - | 572.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 1,606 | - | 572.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 453 | 9,639,483 | 8.38 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 451 | 9,754,194 | 8.38 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 401 | 10,332,713 | 7.84 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 351 | 8,919,811 | 7.84 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 103 | 9,083,477 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 100 | 8,967,001 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 200 | 8,610,298 | 7.42 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 1,506 | 9,755,146 | 7.42 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 551 | 9,250,694 | 8.38 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 351 | 9,244,707 | 8.38 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 351 | 9,069,472 | 7.44 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 351 | 10,080,645 | 7.44 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 100 | 10,279,605 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 100 | 10,633,773 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 150 | 9,710,623 | 7.13 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 1,255 | 9,416,196 | 7.13 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 34,006 | N/A | 19.81 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 37,872 | N/A | 19.81 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 33,803 | N/A | 20.34 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 37,671 | N/A | 20.33 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 33,651 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 37,469 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 33,754 | N/A | 23.50 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 38,872 | N/A | 23.50 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 33,561 | N/A | 19.81 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 36,927 | N/A | 19.81 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 33,253 | N/A | 19.96 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 36,667 | N/A | 19.96 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 33,061 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 36,528 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 33,153 | N/A | 22.64 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 37,770 | N/A | 22.64 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 602 | 6,880,418 | 19.82 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 554 | 7,317,966 | 19.82 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 1,003 | 7,050,197 | 20.39 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 953 | 7,818,608 | 20.38 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 102 | 7,627,765 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 100 | 6,679,134 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 252 | 8,438,106 | 23.52 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 2,710 | 9,521,089 | 23.52 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 503 | 8,663,259 | 19.82 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 551 | 7,423,905 | 19.82 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 1,006 | 9,324,009 | 19.98 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 1,003 | 8,448,087 | 19.98 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 100 | 9,556,575 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 100 | 9,625,566 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 250 | 8,823,789 | 22.66 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 2,358 | 8,759,636 | 22.66 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 2,519 | 10,738,716 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 2,704 | 10,172,630 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/clang_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 100 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/clang_unit_off.log) |
