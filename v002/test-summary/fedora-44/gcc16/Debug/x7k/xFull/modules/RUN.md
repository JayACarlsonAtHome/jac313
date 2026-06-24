# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-24 18:04:29Z |
| OS | fedora-44 |
| Compiler | gcc16 |
| Build type | Debug |
| Disk | x7k |
| Size | full (xFull) |
| Group ID | 91 |
| Environment | jac313-008 · Fedora Linux 44 (Workstation Edition) (7.0.12-201.fc44.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 23 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 624,173 ms (10m 24s) |
| Build straight (no modules) | 8,829 ms (8,829 ms (8s)) |
| Build modules only | 1,756 ms (1,756 ms (1s)) |
| Build with modules (rest) | 10,587 ms (10,587 ms (10s)) |
| Peak ops/sec | 12,901,561 |
| Peak scenario | jac313_store_005_XS (jtext/off) — [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 401 | 9,593,246 | 8.38 MiB |
|  |  | jtext | 401 | 9,805,844 | 7.84 MiB |
|  |  | sql | 150 | 8,567,512 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 502 | 12,175,819 | 8.38 MiB |
|  |  | jtext | 351 | 9,683,354 | 7.85 MiB |
|  |  | sql | 1,104 | 9,155,832 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 401 | 10,987,804 | 8.38 MiB |
|  |  | jtext | 351 | 12,901,561 | 7.44 MiB |
|  |  | sql | 150 | 10,435,146 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 451 | 8,352,823 | 8.38 MiB |
|  |  | jtext | 351 | 10,674,637 | 7.44 MiB |
|  |  | sql | 1,204 | 8,934,954 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,011 | N/A | 19.81 MiB |
|  |  | jtext | 33,853 | N/A | 20.34 MiB |
|  |  | sql | 33,765 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 37,919 | N/A | 19.81 MiB |
|  |  | jtext | 37,628 | N/A | 20.33 MiB |
|  |  | sql | 38,734 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 33,516 | N/A | 19.81 MiB |
|  |  | jtext | 33,257 | N/A | 19.96 MiB |
|  |  | sql | 33,157 | N/A | 22.65 MiB |
| jac313_store_006_XS | on | binary | 36,884 | N/A | 19.81 MiB |
|  |  | jtext | 36,669 | N/A | 19.96 MiB |
|  |  | sql | 37,732 | N/A | 22.65 MiB |
| jac313_store_007_TS | off | binary | 652 | 8,928,571 | 19.82 MiB |
|  |  | jtext | 1,003 | 10,044,194 | 20.38 MiB |
|  |  | sql | 251 | 9,599,693 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 452 | 9,315,324 | 19.82 MiB |
|  |  | jtext | 1,005 | 6,817,097 | 20.39 MiB |
|  |  | sql | 2,260 | 7,850,526 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 552 | 9,321,402 | 19.82 MiB |
|  |  | jtext | 1,003 | 9,581,297 | 19.98 MiB |
|  |  | sql | 251 | 9,803,922 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 602 | 5,365,383 | 19.82 MiB |
|  |  | jtext | 1,003 | 9,540,164 | 19.98 MiB |
|  |  | sql | 2,158 | 9,541,074 | 22.66 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 351 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 94.22 KiB |
|  |  | sql | 200 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 251 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 94.22 KiB |
|  |  | sql | 1,054 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 351 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.22 KiB |
|  |  | sql | 201 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 301 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.22 KiB |
|  |  | sql | 1,104 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 351 | - | 156.07 KiB |
|  |  | jtext | 151 | - | 139.84 KiB |
|  |  | sql | 151 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 201 | - | 156.07 KiB |
|  |  | jtext | 151 | - | 139.85 KiB |
|  |  | sql | 1,004 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 251 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 138.34 KiB |
|  |  | sql | 150 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 251 | - | 156.07 KiB |
|  |  | jtext | 151 | - | 138.34 KiB |
|  |  | sql | 1,004 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 603 | - | 311.94 KiB |
|  |  | jtext | 552 | - | 280.27 KiB |
|  |  | sql | 553 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 1,154 | - | 311.94 KiB |
|  |  | jtext | 603 | - | 280.21 KiB |
|  |  | sql | 1,506 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 652 | - | 311.94 KiB |
|  |  | jtext | 552 | - | 276.60 KiB |
|  |  | sql | 552 | - | 352.00 KiB |
| jac313_store_003_XS | on | binary | 704 | - | 311.94 KiB |
|  |  | jtext | 603 | - | 276.60 KiB |
|  |  | sql | 1,455 | - | 352.00 KiB |
| jac313_store_004_TS | off | binary | 653 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 469.94 KiB |
|  |  | sql | 502 | - | 596.00 KiB |
| jac313_store_004_TS | on | binary | 753 | - | 519.80 KiB |
|  |  | jtext | 603 | - | 469.85 KiB |
|  |  | sql | 1,606 | - | 596.00 KiB |
| jac313_store_004_XS | off | binary | 603 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 463.28 KiB |
|  |  | sql | 502 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 703 | - | 519.80 KiB |
|  |  | jtext | 553 | - | 463.28 KiB |
|  |  | sql | 1,355 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 401 | 9,593,246 | 8.38 MiB |
|  |  | jtext | 401 | 9,805,844 | 7.84 MiB |
|  |  | sql | 150 | 8,567,512 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 502 | 12,175,819 | 8.38 MiB |
|  |  | jtext | 351 | 9,683,354 | 7.85 MiB |
|  |  | sql | 1,104 | 9,155,832 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 401 | 10,987,804 | 8.38 MiB |
|  |  | jtext | 351 | 12,901,561 | 7.44 MiB |
|  |  | sql | 150 | 10,435,146 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 451 | 8,352,823 | 8.38 MiB |
|  |  | jtext | 351 | 10,674,637 | 7.44 MiB |
|  |  | sql | 1,204 | 8,934,954 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,011 | N/A | 19.81 MiB |
|  |  | jtext | 33,853 | N/A | 20.34 MiB |
|  |  | sql | 33,765 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 37,919 | N/A | 19.81 MiB |
|  |  | jtext | 37,628 | N/A | 20.33 MiB |
|  |  | sql | 38,734 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 33,516 | N/A | 19.81 MiB |
|  |  | jtext | 33,257 | N/A | 19.96 MiB |
|  |  | sql | 33,157 | N/A | 22.65 MiB |
| jac313_store_006_XS | on | binary | 36,884 | N/A | 19.81 MiB |
|  |  | jtext | 36,669 | N/A | 19.96 MiB |
|  |  | sql | 37,732 | N/A | 22.65 MiB |
| jac313_store_007_TS | off | binary | 652 | 8,928,571 | 19.82 MiB |
|  |  | jtext | 1,003 | 10,044,194 | 20.38 MiB |
|  |  | sql | 251 | 9,599,693 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 452 | 9,315,324 | 19.82 MiB |
|  |  | jtext | 1,005 | 6,817,097 | 20.39 MiB |
|  |  | sql | 2,260 | 7,850,526 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 552 | 9,321,402 | 19.82 MiB |
|  |  | jtext | 1,003 | 9,581,297 | 19.98 MiB |
|  |  | sql | 251 | 9,803,922 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 602 | 5,365,383 | 19.82 MiB |
|  |  | jtext | 1,003 | 9,540,164 | 19.98 MiB |
|  |  | sql | 2,158 | 9,541,074 | 22.66 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 351 | - | 104.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 251 | - | 104.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 201 | - | 94.22 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 201 | - | 94.22 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 201 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 201 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 200 | - | 152.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 1,054 | - | 152.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 351 | - | 104.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 301 | - | 104.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 201 | - | 93.22 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 201 | - | 93.22 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 201 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 201 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 201 | - | 148.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 1,104 | - | 148.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 351 | - | 156.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 201 | - | 156.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 151 | - | 139.84 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 151 | - | 139.85 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 150 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 150 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 151 | - | 200.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 1,004 | - | 200.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 251 | - | 156.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 251 | - | 156.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 150 | - | 138.34 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 151 | - | 138.34 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 151 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 151 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 150 | - | 192.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 1,004 | - | 192.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 603 | - | 311.94 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 1,154 | - | 311.94 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 552 | - | 280.27 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 603 | - | 280.21 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 553 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 603 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 553 | - | 368.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 1,506 | - | 368.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 652 | - | 311.94 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 704 | - | 311.94 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 552 | - | 276.60 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 603 | - | 276.60 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 553 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 603 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 552 | - | 352.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 1,455 | - | 352.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 653 | - | 519.80 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 753 | - | 519.80 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 502 | - | 469.94 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 603 | - | 469.85 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 502 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 602 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 502 | - | 596.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 1,606 | - | 596.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 603 | - | 519.80 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 703 | - | 519.80 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 502 | - | 463.28 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 553 | - | 463.28 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 502 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 552 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 502 | - | 572.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 1,355 | - | 572.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 401 | 9,593,246 | 8.38 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 502 | 12,175,819 | 8.38 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 401 | 9,805,844 | 7.84 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 351 | 9,683,354 | 7.85 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 100 | 11,357,183 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 100 | 10,222,858 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 150 | 8,567,512 | 7.42 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 1,104 | 9,155,832 | 7.42 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 401 | 10,987,804 | 8.38 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 451 | 8,352,823 | 8.38 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 351 | 12,901,561 | 7.44 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 351 | 10,674,637 | 7.44 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 100 | 10,287,008 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 100 | 12,732,366 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 150 | 10,435,146 | 7.13 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 1,204 | 8,934,954 | 7.13 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 34,011 | N/A | 19.81 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 37,919 | N/A | 19.81 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 33,853 | N/A | 20.34 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 37,628 | N/A | 20.33 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 33,661 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 37,479 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 33,765 | N/A | 23.50 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 38,734 | N/A | 23.50 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 33,516 | N/A | 19.81 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 36,884 | N/A | 19.81 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 33,257 | N/A | 19.96 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 36,669 | N/A | 19.96 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 33,055 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 36,528 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 33,157 | N/A | 22.65 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 37,732 | N/A | 22.65 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 652 | 8,928,571 | 19.82 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 452 | 9,315,324 | 19.82 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 1,003 | 10,044,194 | 20.38 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 1,005 | 6,817,097 | 20.39 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 101 | 9,954,211 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 101 | 8,828,463 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 251 | 9,599,693 | 23.51 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 2,260 | 7,850,526 | 23.51 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 552 | 9,321,402 | 19.82 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 602 | 5,365,383 | 19.82 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 1,003 | 9,581,297 | 19.98 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 1,003 | 9,540,164 | 19.98 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 100 | 10,472,301 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 100 | 8,611,781 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 251 | 9,803,922 | 22.66 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 2,158 | 9,541,074 | 22.66 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 3,116 | 11,433,798 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc16_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 2,463 | 12,583,048 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc16_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc16_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 100 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc16_unit_off.log) |
