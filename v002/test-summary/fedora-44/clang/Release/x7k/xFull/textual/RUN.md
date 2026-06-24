# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-24 17:48:51Z |
| OS | fedora-44 |
| Compiler | clang |
| Build type | Release |
| Disk | x7k |
| Size | full (xFull) |
| Group ID | 90 |
| Environment | jac313-008 · Fedora Linux 44 (Workstation Edition) (7.0.12-201.fc44.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 23 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 618,831 ms (10m 18s) |
| Peak ops/sec | 24,539,877 |
| Peak scenario | jac313_store_005_TS (jtext/off) — [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 401 | 15,398,830 | 8.38 MiB |
|  |  | jtext | 100 | 24,539,877 | 7.82 MiB |
|  |  | sql | 103 | 22,002,200 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 351 | 13,442,667 | 8.38 MiB |
|  |  | jtext | 100 | 15,559,359 | 7.82 MiB |
|  |  | sql | 1,404 | 19,323,671 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 803 | 19,413,706 | 8.38 MiB |
|  |  | jtext | 100 | 10,858,942 | 7.44 MiB |
|  |  | sql | 100 | 21,088,148 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 703 | 18,857,251 | 8.38 MiB |
|  |  | jtext | 100 | 22,153,301 | 7.44 MiB |
|  |  | sql | 1,304 | 19,516,003 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 33,857 | N/A | 19.81 MiB |
|  |  | jtext | 33,704 | N/A | 20.33 MiB |
|  |  | sql | 33,708 | N/A | 23.49 MiB |
| jac313_store_006_TS | on | binary | 37,825 | N/A | 19.81 MiB |
|  |  | jtext | 37,523 | N/A | 20.32 MiB |
|  |  | sql | 38,831 | N/A | 23.49 MiB |
| jac313_store_006_XS | off | binary | 33,412 | N/A | 19.81 MiB |
|  |  | jtext | 33,104 | N/A | 19.96 MiB |
|  |  | sql | 33,105 | N/A | 22.62 MiB |
| jac313_store_006_XS | on | binary | 36,871 | N/A | 19.81 MiB |
|  |  | jtext | 36,569 | N/A | 19.96 MiB |
|  |  | sql | 37,676 | N/A | 22.62 MiB |
| jac313_store_007_TS | off | binary | 501 | 14,353,380 | 19.82 MiB |
|  |  | jtext | 250 | 14,164,306 | 20.36 MiB |
|  |  | sql | 200 | 15,144,631 | 23.49 MiB |
| jac313_store_007_TS | on | binary | 451 | 17,041,581 | 19.82 MiB |
|  |  | jtext | 200 | 15,112,589 | 20.36 MiB |
|  |  | sql | 2,258 | 15,227,653 | 23.49 MiB |
| jac313_store_007_XS | off | binary | 552 | 19,634,793 | 19.82 MiB |
|  |  | jtext | 251 | 14,031,149 | 19.98 MiB |
|  |  | sql | 201 | 20,445,717 | 22.63 MiB |
| jac313_store_007_XS | on | binary | 502 | 12,795,905 | 19.82 MiB |
|  |  | jtext | 200 | 14,836,795 | 19.98 MiB |
|  |  | sql | 2,559 | 13,368,984 | 22.63 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 251 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 94.23 KiB |
|  |  | sql | 201 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 301 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 94.23 KiB |
|  |  | sql | 1,154 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 251 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.23 KiB |
|  |  | sql | 201 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 251 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.23 KiB |
|  |  | sql | 1,405 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 251 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 139.86 KiB |
|  |  | sql | 150 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 251 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 140.17 KiB |
|  |  | sql | 1,706 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 502 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 138.36 KiB |
|  |  | sql | 150 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 251 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 138.36 KiB |
|  |  | sql | 1,054 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 703 | - | 311.94 KiB |
|  |  | jtext | 552 | - | 279.67 KiB |
|  |  | sql | 552 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 703 | - | 311.94 KiB |
|  |  | jtext | 602 | - | 279.68 KiB |
|  |  | sql | 1,656 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 652 | - | 311.94 KiB |
|  |  | jtext | 552 | - | 276.62 KiB |
|  |  | sql | 552 | - | 356.00 KiB |
| jac313_store_003_XS | on | binary | 653 | - | 311.94 KiB |
|  |  | jtext | 602 | - | 276.62 KiB |
|  |  | sql | 1,556 | - | 356.00 KiB |
| jac313_store_004_TS | off | binary | 653 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 469.70 KiB |
|  |  | sql | 501 | - | 596.00 KiB |
| jac313_store_004_TS | on | binary | 702 | - | 519.80 KiB |
|  |  | jtext | 602 | - | 470.25 KiB |
|  |  | sql | 1,505 | - | 596.00 KiB |
| jac313_store_004_XS | off | binary | 753 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 463.30 KiB |
|  |  | sql | 501 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 602 | - | 519.80 KiB |
|  |  | jtext | 552 | - | 463.30 KiB |
|  |  | sql | 1,558 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 401 | 15,398,830 | 8.38 MiB |
|  |  | jtext | 100 | 24,539,877 | 7.82 MiB |
|  |  | sql | 103 | 22,002,200 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 351 | 13,442,667 | 8.38 MiB |
|  |  | jtext | 100 | 15,559,359 | 7.82 MiB |
|  |  | sql | 1,404 | 19,323,671 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 803 | 19,413,706 | 8.38 MiB |
|  |  | jtext | 100 | 10,858,942 | 7.44 MiB |
|  |  | sql | 100 | 21,088,148 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 703 | 18,857,251 | 8.38 MiB |
|  |  | jtext | 100 | 22,153,301 | 7.44 MiB |
|  |  | sql | 1,304 | 19,516,003 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 33,857 | N/A | 19.81 MiB |
|  |  | jtext | 33,704 | N/A | 20.33 MiB |
|  |  | sql | 33,708 | N/A | 23.49 MiB |
| jac313_store_006_TS | on | binary | 37,825 | N/A | 19.81 MiB |
|  |  | jtext | 37,523 | N/A | 20.32 MiB |
|  |  | sql | 38,831 | N/A | 23.49 MiB |
| jac313_store_006_XS | off | binary | 33,412 | N/A | 19.81 MiB |
|  |  | jtext | 33,104 | N/A | 19.96 MiB |
|  |  | sql | 33,105 | N/A | 22.62 MiB |
| jac313_store_006_XS | on | binary | 36,871 | N/A | 19.81 MiB |
|  |  | jtext | 36,569 | N/A | 19.96 MiB |
|  |  | sql | 37,676 | N/A | 22.62 MiB |
| jac313_store_007_TS | off | binary | 501 | 14,353,380 | 19.82 MiB |
|  |  | jtext | 250 | 14,164,306 | 20.36 MiB |
|  |  | sql | 200 | 15,144,631 | 23.49 MiB |
| jac313_store_007_TS | on | binary | 451 | 17,041,581 | 19.82 MiB |
|  |  | jtext | 200 | 15,112,589 | 20.36 MiB |
|  |  | sql | 2,258 | 15,227,653 | 23.49 MiB |
| jac313_store_007_XS | off | binary | 552 | 19,634,793 | 19.82 MiB |
|  |  | jtext | 251 | 14,031,149 | 19.98 MiB |
|  |  | sql | 201 | 20,445,717 | 22.63 MiB |
| jac313_store_007_XS | on | binary | 502 | 12,795,905 | 19.82 MiB |
|  |  | jtext | 200 | 14,836,795 | 19.98 MiB |
|  |  | sql | 2,559 | 13,368,984 | 22.63 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 251 | - | 104.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 301 | - | 104.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 201 | - | 94.23 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 201 | - | 94.23 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 201 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 201 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 201 | - | 152.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 1,154 | - | 152.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 251 | - | 104.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 251 | - | 104.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 201 | - | 93.23 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 201 | - | 93.23 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 200 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 201 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 201 | - | 148.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 1,405 | - | 148.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 251 | - | 156.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 251 | - | 156.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 150 | - | 139.86 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 150 | - | 140.17 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 152 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 150 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 150 | - | 200.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 1,706 | - | 200.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 502 | - | 156.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 251 | - | 156.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 150 | - | 138.36 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 150 | - | 138.36 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 151 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 150 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 150 | - | 192.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 1,054 | - | 192.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 703 | - | 311.94 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 703 | - | 311.94 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 552 | - | 279.67 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 602 | - | 279.68 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 552 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 602 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 552 | - | 368.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 1,656 | - | 368.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 652 | - | 311.94 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 653 | - | 311.94 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 552 | - | 276.62 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 602 | - | 276.62 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 552 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 602 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 552 | - | 356.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 1,556 | - | 356.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 653 | - | 519.80 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 702 | - | 519.80 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 502 | - | 469.70 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 602 | - | 470.25 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 502 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 602 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 501 | - | 596.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 1,505 | - | 596.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 753 | - | 519.80 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 602 | - | 519.80 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 502 | - | 463.30 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 552 | - | 463.30 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 502 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 553 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 501 | - | 572.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 1,558 | - | 572.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 401 | 15,398,830 | 8.38 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 351 | 13,442,667 | 8.38 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 100 | 24,539,877 | 7.82 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 100 | 15,559,359 | 7.82 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 50 | 12,550,201 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 50 | 24,073,182 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 103 | 22,002,200 | 7.42 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 1,404 | 19,323,671 | 7.42 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 803 | 19,413,706 | 8.38 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 703 | 18,857,251 | 8.38 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 100 | 10,858,942 | 7.44 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 100 | 22,153,301 | 7.44 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 50 | 22,810,219 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 50 | 15,232,292 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 100 | 21,088,148 | 7.13 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 1,304 | 19,516,003 | 7.13 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 33,857 | N/A | 19.81 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 37,825 | N/A | 19.81 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 33,704 | N/A | 20.33 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 37,523 | N/A | 20.32 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 33,653 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 37,478 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 33,708 | N/A | 23.49 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 38,831 | N/A | 23.49 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 33,412 | N/A | 19.81 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 36,871 | N/A | 19.81 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 33,104 | N/A | 19.96 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 36,569 | N/A | 19.96 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 33,053 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 36,518 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 33,105 | N/A | 22.62 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 37,676 | N/A | 22.62 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 501 | 14,353,380 | 19.82 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 451 | 17,041,581 | 19.82 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 250 | 14,164,306 | 20.36 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 200 | 15,112,589 | 20.36 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 50 | 14,297,970 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 50 | 14,184,397 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 200 | 15,144,631 | 23.49 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 2,258 | 15,227,653 | 23.49 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 552 | 19,634,793 | 19.82 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 502 | 12,795,905 | 19.82 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 251 | 14,031,149 | 19.98 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 200 | 14,836,795 | 19.98 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 50 | 19,496,978 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 50 | 15,151,515 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 201 | 20,445,717 | 22.63 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 2,559 | 13,368,984 | 22.63 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 1,910 | 21,327,419 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 1,965 | 21,056,621 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/clang_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 50 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/clang_unit_off.log) |
