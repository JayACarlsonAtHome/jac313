# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-24 18:52:29Z |
| OS | fedora-44 |
| Compiler | gcc16 |
| Build type | Release |
| Disk | x7k |
| Size | full (xFull) |
| Group ID | 94 |
| Environment | jac313-008 · Fedora Linux 44 (Workstation Edition) (7.0.12-201.fc44.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 23 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 618,778 ms (10m 18s) |
| Peak ops/sec | 25,201,613 |
| Peak scenario | jac313_store_005_XS (sql/off) — [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 451 | 19,379,845 | 8.38 MiB |
|  |  | jtext | 100 | 18,552,876 | 7.82 MiB |
|  |  | sql | 100 | 19,308,747 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 301 | 20,990,764 | 8.38 MiB |
|  |  | jtext | 101 | 23,369,946 | 7.82 MiB |
|  |  | sql | 1,154 | 12,594,458 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 401 | 16,658,337 | 8.38 MiB |
|  |  | jtext | 100 | 17,513,135 | 7.44 MiB |
|  |  | sql | 100 | 25,201,613 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 351 | 23,468,669 | 8.38 MiB |
|  |  | jtext | 100 | 21,968,366 | 7.44 MiB |
|  |  | sql | 1,505 | 16,854,879 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,117 | N/A | 19.81 MiB |
|  |  | jtext | 33,713 | N/A | 20.33 MiB |
|  |  | sql | 33,710 | N/A | 23.48 MiB |
| jac313_store_006_TS | on | binary | 37,678 | N/A | 19.81 MiB |
|  |  | jtext | 37,528 | N/A | 20.32 MiB |
|  |  | sql | 38,788 | N/A | 23.48 MiB |
| jac313_store_006_XS | off | binary | 33,466 | N/A | 19.81 MiB |
|  |  | jtext | 33,107 | N/A | 19.96 MiB |
|  |  | sql | 33,106 | N/A | 22.62 MiB |
| jac313_store_006_XS | on | binary | 36,982 | N/A | 19.81 MiB |
|  |  | jtext | 36,577 | N/A | 19.96 MiB |
|  |  | sql | 37,877 | N/A | 22.62 MiB |
| jac313_store_007_TS | off | binary | 452 | 17,143,837 | 19.82 MiB |
|  |  | jtext | 251 | 12,924,906 | 20.36 MiB |
|  |  | sql | 200 | 17,739,933 | 23.48 MiB |
| jac313_store_007_TS | on | binary | 451 | 13,234,516 | 19.82 MiB |
|  |  | jtext | 200 | 18,497,965 | 20.36 MiB |
|  |  | sql | 2,510 | 15,429,718 | 23.48 MiB |
| jac313_store_007_XS | off | binary | 552 | 23,736,055 | 19.82 MiB |
|  |  | jtext | 251 | 21,454,623 | 19.98 MiB |
|  |  | sql | 200 | 14,096,420 | 22.63 MiB |
| jac313_store_007_XS | on | binary | 552 | 18,138,944 | 19.82 MiB |
|  |  | jtext | 200 | 19,357,336 | 19.98 MiB |
|  |  | sql | 2,358 | 13,941,168 | 22.63 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 251 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 94.23 KiB |
|  |  | sql | 201 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 351 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 94.23 KiB |
|  |  | sql | 1,203 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 301 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.23 KiB |
|  |  | sql | 201 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 301 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.23 KiB |
|  |  | sql | 1,105 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 251 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 139.86 KiB |
|  |  | sql | 150 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 251 | - | 156.07 KiB |
|  |  | jtext | 151 | - | 139.86 KiB |
|  |  | sql | 1,104 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 250 | - | 156.07 KiB |
|  |  | jtext | 151 | - | 138.36 KiB |
|  |  | sql | 151 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 201 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 138.36 KiB |
|  |  | sql | 1,104 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 602 | - | 311.94 KiB |
|  |  | jtext | 552 | - | 279.90 KiB |
|  |  | sql | 553 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 752 | - | 311.94 KiB |
|  |  | jtext | 602 | - | 279.62 KiB |
|  |  | sql | 1,505 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 552 | - | 311.94 KiB |
|  |  | jtext | 553 | - | 276.62 KiB |
|  |  | sql | 552 | - | 352.00 KiB |
| jac313_store_003_XS | on | binary | 702 | - | 311.94 KiB |
|  |  | jtext | 603 | - | 276.62 KiB |
|  |  | sql | 1,406 | - | 352.00 KiB |
| jac313_store_004_TS | off | binary | 652 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 469.52 KiB |
|  |  | sql | 502 | - | 596.00 KiB |
| jac313_store_004_TS | on | binary | 703 | - | 519.80 KiB |
|  |  | jtext | 602 | - | 469.30 KiB |
|  |  | sql | 1,556 | - | 596.00 KiB |
| jac313_store_004_XS | off | binary | 652 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 463.30 KiB |
|  |  | sql | 502 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 1,354 | - | 519.80 KiB |
|  |  | jtext | 552 | - | 463.30 KiB |
|  |  | sql | 1,555 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 451 | 19,379,845 | 8.38 MiB |
|  |  | jtext | 100 | 18,552,876 | 7.82 MiB |
|  |  | sql | 100 | 19,308,747 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 301 | 20,990,764 | 8.38 MiB |
|  |  | jtext | 101 | 23,369,946 | 7.82 MiB |
|  |  | sql | 1,154 | 12,594,458 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 401 | 16,658,337 | 8.38 MiB |
|  |  | jtext | 100 | 17,513,135 | 7.44 MiB |
|  |  | sql | 100 | 25,201,613 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 351 | 23,468,669 | 8.38 MiB |
|  |  | jtext | 100 | 21,968,366 | 7.44 MiB |
|  |  | sql | 1,505 | 16,854,879 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,117 | N/A | 19.81 MiB |
|  |  | jtext | 33,713 | N/A | 20.33 MiB |
|  |  | sql | 33,710 | N/A | 23.48 MiB |
| jac313_store_006_TS | on | binary | 37,678 | N/A | 19.81 MiB |
|  |  | jtext | 37,528 | N/A | 20.32 MiB |
|  |  | sql | 38,788 | N/A | 23.48 MiB |
| jac313_store_006_XS | off | binary | 33,466 | N/A | 19.81 MiB |
|  |  | jtext | 33,107 | N/A | 19.96 MiB |
|  |  | sql | 33,106 | N/A | 22.62 MiB |
| jac313_store_006_XS | on | binary | 36,982 | N/A | 19.81 MiB |
|  |  | jtext | 36,577 | N/A | 19.96 MiB |
|  |  | sql | 37,877 | N/A | 22.62 MiB |
| jac313_store_007_TS | off | binary | 452 | 17,143,837 | 19.82 MiB |
|  |  | jtext | 251 | 12,924,906 | 20.36 MiB |
|  |  | sql | 200 | 17,739,933 | 23.48 MiB |
| jac313_store_007_TS | on | binary | 451 | 13,234,516 | 19.82 MiB |
|  |  | jtext | 200 | 18,497,965 | 20.36 MiB |
|  |  | sql | 2,510 | 15,429,718 | 23.48 MiB |
| jac313_store_007_XS | off | binary | 552 | 23,736,055 | 19.82 MiB |
|  |  | jtext | 251 | 21,454,623 | 19.98 MiB |
|  |  | sql | 200 | 14,096,420 | 22.63 MiB |
| jac313_store_007_XS | on | binary | 552 | 18,138,944 | 19.82 MiB |
|  |  | jtext | 200 | 19,357,336 | 19.98 MiB |
|  |  | sql | 2,358 | 13,941,168 | 22.63 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 251 | - | 104.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 351 | - | 104.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 201 | - | 94.23 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 201 | - | 94.23 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 201 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 201 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 201 | - | 152.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 1,203 | - | 152.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 301 | - | 104.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 301 | - | 104.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 201 | - | 93.23 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 201 | - | 93.23 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 201 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 201 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 201 | - | 148.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 1,105 | - | 148.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 251 | - | 156.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 251 | - | 156.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 150 | - | 139.86 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 151 | - | 139.86 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 150 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 150 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 150 | - | 200.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 1,104 | - | 200.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 250 | - | 156.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 201 | - | 156.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 151 | - | 138.36 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 150 | - | 138.36 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 150 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 150 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 151 | - | 192.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 1,104 | - | 192.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 602 | - | 311.94 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 752 | - | 311.94 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 552 | - | 279.90 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 602 | - | 279.62 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 552 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 602 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 553 | - | 368.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 1,505 | - | 368.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 552 | - | 311.94 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 702 | - | 311.94 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 553 | - | 276.62 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 603 | - | 276.62 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 552 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 602 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 552 | - | 352.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 1,406 | - | 352.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 652 | - | 519.80 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 703 | - | 519.80 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 502 | - | 469.52 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 602 | - | 469.30 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 502 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 602 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 502 | - | 596.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 1,556 | - | 596.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 652 | - | 519.80 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 1,354 | - | 519.80 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 502 | - | 463.30 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 552 | - | 463.30 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 502 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 552 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 502 | - | 572.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 1,555 | - | 572.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 451 | 19,379,845 | 8.38 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 301 | 20,990,764 | 8.38 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 100 | 18,552,876 | 7.82 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 101 | 23,369,946 | 7.82 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 50 | 22,471,910 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 50 | 14,120,305 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 100 | 19,308,747 | 7.42 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 1,154 | 12,594,458 | 7.42 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 401 | 16,658,337 | 8.38 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 351 | 23,468,669 | 8.38 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 100 | 17,513,135 | 7.44 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 100 | 21,968,366 | 7.44 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 50 | 19,611,689 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 50 | 15,337,423 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 100 | 25,201,613 | 7.13 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 1,505 | 16,854,879 | 7.13 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 34,117 | N/A | 19.81 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 37,678 | N/A | 19.81 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 33,713 | N/A | 20.33 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 37,528 | N/A | 20.32 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 33,662 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 37,479 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 33,710 | N/A | 23.48 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 38,788 | N/A | 23.48 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 33,466 | N/A | 19.81 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 36,982 | N/A | 19.81 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 33,107 | N/A | 19.96 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 36,577 | N/A | 19.96 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 33,063 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 36,534 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 33,106 | N/A | 22.62 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 37,877 | N/A | 22.62 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 452 | 17,143,837 | 19.82 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 451 | 13,234,516 | 19.82 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 251 | 12,924,906 | 20.36 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 200 | 18,497,965 | 20.36 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 50 | 19,794,141 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 50 | 15,137,754 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 200 | 17,739,933 | 23.48 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 2,510 | 15,429,718 | 23.48 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 552 | 23,736,055 | 19.82 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 552 | 18,138,944 | 19.82 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 251 | 21,454,623 | 19.98 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 200 | 19,357,336 | 19.98 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 50 | 14,334,862 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 50 | 18,828,846 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 200 | 14,096,420 | 22.63 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 2,358 | 13,941,168 | 22.63 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 2,671 | 15,705,984 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc16_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 2,059 | 19,977,625 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc16_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc16_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc16_unit_off.log) |
