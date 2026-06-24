# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-24 18:36:14Z |
| OS | fedora-44 |
| Compiler | gcc16 |
| Build type | Release |
| Disk | x7k |
| Size | full (xFull) |
| Group ID | 93 |
| Environment | jac313-008 · Fedora Linux 44 (Workstation Edition) (7.0.12-201.fc44.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 23 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 617,723 ms (10m 17s) |
| Build straight (no modules) | 15,546 ms (15,546 ms (15s)) |
| Build modules only | 1,605 ms (1,605 ms (1s)) |
| Build with modules (rest) | 16,145 ms (16,145 ms (16s)) |
| Peak ops/sec | 24,195,500 |
| Peak scenario | jac313_store_005_XS (binary/off) — [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 351 | 21,057,065 | 8.38 MiB |
|  |  | jtext | 101 | 14,245,014 | 7.82 MiB |
|  |  | sql | 100 | 21,321,962 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 352 | 15,320,974 | 8.38 MiB |
|  |  | jtext | 100 | 14,641,288 | 7.82 MiB |
|  |  | sql | 1,305 | 22,930,521 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 501 | 24,195,500 | 8.38 MiB |
|  |  | jtext | 100 | 18,875,047 | 7.44 MiB |
|  |  | sql | 100 | 14,363,689 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 401 | 15,073,862 | 8.38 MiB |
|  |  | jtext | 100 | 23,116,043 | 7.44 MiB |
|  |  | sql | 1,355 | 20,802,996 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,004 | N/A | 19.81 MiB |
|  |  | jtext | 33,710 | N/A | 20.33 MiB |
|  |  | sql | 33,713 | N/A | 23.48 MiB |
| jac313_store_006_TS | on | binary | 37,831 | N/A | 19.81 MiB |
|  |  | jtext | 37,528 | N/A | 20.32 MiB |
|  |  | sql | 38,786 | N/A | 23.48 MiB |
| jac313_store_006_XS | off | binary | 33,506 | N/A | 19.81 MiB |
|  |  | jtext | 33,108 | N/A | 19.96 MiB |
|  |  | sql | 33,110 | N/A | 22.62 MiB |
| jac313_store_006_XS | on | binary | 36,929 | N/A | 19.81 MiB |
|  |  | jtext | 36,564 | N/A | 19.96 MiB |
|  |  | sql | 37,778 | N/A | 22.62 MiB |
| jac313_store_007_TS | off | binary | 452 | 20,665,427 | 19.82 MiB |
|  |  | jtext | 251 | 18,188,432 | 20.36 MiB |
|  |  | sql | 201 | 20,846,362 | 23.49 MiB |
| jac313_store_007_TS | on | binary | 452 | 13,982,103 | 19.82 MiB |
|  |  | jtext | 202 | 15,158,405 | 20.36 MiB |
|  |  | sql | 2,260 | 21,772,262 | 23.49 MiB |
| jac313_store_007_XS | off | binary | 502 | 20,601,566 | 19.82 MiB |
|  |  | jtext | 251 | 15,351,551 | 19.98 MiB |
|  |  | sql | 201 | 19,916,351 | 22.64 MiB |
| jac313_store_007_XS | on | binary | 451 | 13,010,669 | 19.82 MiB |
|  |  | jtext | 200 | 13,929,517 | 19.98 MiB |
|  |  | sql | 2,663 | 19,168,104 | 22.64 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 351 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 94.23 KiB |
|  |  | sql | 201 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 251 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 94.23 KiB |
|  |  | sql | 1,054 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 351 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.23 KiB |
|  |  | sql | 201 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 251 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.23 KiB |
|  |  | sql | 1,154 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 301 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 139.86 KiB |
|  |  | sql | 150 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 301 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 139.86 KiB |
|  |  | sql | 1,104 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 301 | - | 156.07 KiB |
|  |  | jtext | 151 | - | 138.36 KiB |
|  |  | sql | 151 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 201 | - | 156.07 KiB |
|  |  | jtext | 151 | - | 138.36 KiB |
|  |  | sql | 1,105 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 702 | - | 311.94 KiB |
|  |  | jtext | 552 | - | 279.76 KiB |
|  |  | sql | 552 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 753 | - | 311.94 KiB |
|  |  | jtext | 602 | - | 279.62 KiB |
|  |  | sql | 1,505 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 602 | - | 311.94 KiB |
|  |  | jtext | 551 | - | 276.62 KiB |
|  |  | sql | 552 | - | 352.00 KiB |
| jac313_store_003_XS | on | binary | 652 | - | 311.94 KiB |
|  |  | jtext | 602 | - | 276.62 KiB |
|  |  | sql | 1,505 | - | 352.00 KiB |
| jac313_store_004_TS | off | binary | 552 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 469.25 KiB |
|  |  | sql | 502 | - | 596.00 KiB |
| jac313_store_004_TS | on | binary | 853 | - | 519.80 KiB |
|  |  | jtext | 602 | - | 469.99 KiB |
|  |  | sql | 1,606 | - | 596.00 KiB |
| jac313_store_004_XS | off | binary | 653 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 463.30 KiB |
|  |  | sql | 502 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 703 | - | 519.80 KiB |
|  |  | jtext | 552 | - | 463.30 KiB |
|  |  | sql | 1,557 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 351 | 21,057,065 | 8.38 MiB |
|  |  | jtext | 101 | 14,245,014 | 7.82 MiB |
|  |  | sql | 100 | 21,321,962 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 352 | 15,320,974 | 8.38 MiB |
|  |  | jtext | 100 | 14,641,288 | 7.82 MiB |
|  |  | sql | 1,305 | 22,930,521 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 501 | 24,195,500 | 8.38 MiB |
|  |  | jtext | 100 | 18,875,047 | 7.44 MiB |
|  |  | sql | 100 | 14,363,689 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 401 | 15,073,862 | 8.38 MiB |
|  |  | jtext | 100 | 23,116,043 | 7.44 MiB |
|  |  | sql | 1,355 | 20,802,996 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,004 | N/A | 19.81 MiB |
|  |  | jtext | 33,710 | N/A | 20.33 MiB |
|  |  | sql | 33,713 | N/A | 23.48 MiB |
| jac313_store_006_TS | on | binary | 37,831 | N/A | 19.81 MiB |
|  |  | jtext | 37,528 | N/A | 20.32 MiB |
|  |  | sql | 38,786 | N/A | 23.48 MiB |
| jac313_store_006_XS | off | binary | 33,506 | N/A | 19.81 MiB |
|  |  | jtext | 33,108 | N/A | 19.96 MiB |
|  |  | sql | 33,110 | N/A | 22.62 MiB |
| jac313_store_006_XS | on | binary | 36,929 | N/A | 19.81 MiB |
|  |  | jtext | 36,564 | N/A | 19.96 MiB |
|  |  | sql | 37,778 | N/A | 22.62 MiB |
| jac313_store_007_TS | off | binary | 452 | 20,665,427 | 19.82 MiB |
|  |  | jtext | 251 | 18,188,432 | 20.36 MiB |
|  |  | sql | 201 | 20,846,362 | 23.49 MiB |
| jac313_store_007_TS | on | binary | 452 | 13,982,103 | 19.82 MiB |
|  |  | jtext | 202 | 15,158,405 | 20.36 MiB |
|  |  | sql | 2,260 | 21,772,262 | 23.49 MiB |
| jac313_store_007_XS | off | binary | 502 | 20,601,566 | 19.82 MiB |
|  |  | jtext | 251 | 15,351,551 | 19.98 MiB |
|  |  | sql | 201 | 19,916,351 | 22.64 MiB |
| jac313_store_007_XS | on | binary | 451 | 13,010,669 | 19.82 MiB |
|  |  | jtext | 200 | 13,929,517 | 19.98 MiB |
|  |  | sql | 2,663 | 19,168,104 | 22.64 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 351 | - | 104.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 251 | - | 104.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 201 | - | 94.23 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 201 | - | 94.23 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 201 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 201 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 201 | - | 152.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 1,054 | - | 152.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 351 | - | 104.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 251 | - | 104.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 201 | - | 93.23 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 201 | - | 93.23 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 201 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 201 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 201 | - | 148.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 1,154 | - | 148.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 301 | - | 156.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 301 | - | 156.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 150 | - | 139.86 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 150 | - | 139.86 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 150 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 151 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 150 | - | 200.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 1,104 | - | 200.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 301 | - | 156.07 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 201 | - | 156.07 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 151 | - | 138.36 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 151 | - | 138.36 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 150 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 150 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 151 | - | 192.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 1,105 | - | 192.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 702 | - | 311.94 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 753 | - | 311.94 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 552 | - | 279.76 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 602 | - | 279.62 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 552 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 602 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 552 | - | 368.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 1,505 | - | 368.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 602 | - | 311.94 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 652 | - | 311.94 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 551 | - | 276.62 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 602 | - | 276.62 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 552 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 602 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 552 | - | 352.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 1,505 | - | 352.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 552 | - | 519.80 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 853 | - | 519.80 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 502 | - | 469.25 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 602 | - | 469.99 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 502 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 602 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 502 | - | 596.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 1,606 | - | 596.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 653 | - | 519.80 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 703 | - | 519.80 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 502 | - | 463.30 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 552 | - | 463.30 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 502 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 552 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 502 | - | 572.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 1,557 | - | 572.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 351 | 21,057,065 | 8.38 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 352 | 15,320,974 | 8.38 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 101 | 14,245,014 | 7.82 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 100 | 14,641,288 | 7.82 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 50 | 13,564,840 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 50 | 24,026,910 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 100 | 21,321,962 | 7.42 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 1,305 | 22,930,521 | 7.42 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 501 | 24,195,500 | 8.38 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 401 | 15,073,862 | 8.38 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 100 | 18,875,047 | 7.44 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 100 | 23,116,043 | 7.44 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 50 | 20,729,685 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 50 | 21,963,541 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 100 | 14,363,689 | 7.13 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 1,355 | 20,802,996 | 7.13 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 34,004 | N/A | 19.81 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 37,831 | N/A | 19.81 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 33,710 | N/A | 20.33 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 37,528 | N/A | 20.32 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 33,655 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 37,467 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 33,713 | N/A | 23.48 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 38,786 | N/A | 23.48 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 33,506 | N/A | 19.81 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 36,929 | N/A | 19.81 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 33,108 | N/A | 19.96 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 36,564 | N/A | 19.96 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 33,053 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 36,530 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 33,110 | N/A | 22.62 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 37,778 | N/A | 22.62 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 452 | 20,665,427 | 19.82 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 452 | 13,982,103 | 19.82 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 251 | 18,188,432 | 20.36 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 202 | 15,158,405 | 20.36 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 50 | 14,009,526 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 50 | 14,624,159 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 201 | 20,846,362 | 23.49 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 2,260 | 21,772,262 | 23.49 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 502 | 20,601,566 | 19.82 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 451 | 13,010,669 | 19.82 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 251 | 15,351,551 | 19.98 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 200 | 13,929,517 | 19.98 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 50 | 22,634,676 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 50 | 15,564,202 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 201 | 19,916,351 | 22.64 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 2,663 | 19,168,104 | 22.64 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 2,067 | 21,535,480 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc16_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 2,011 | 14,659,317 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc16_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc16_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc16_unit_off.log) |
