# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-24 16:34:42Z |
| OS | fedora-44 |
| Compiler | gcc16 |
| Build type | Debug |
| Disk | x7k |
| Size | smoke (Smoke) |
| Group ID | 83 |
| Environment | jac313-008 · Fedora Linux 44 (Workstation Edition) (7.0.12-201.fc44.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 23 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 34,875 ms (34s) |
| Build straight (no modules) | 9,031 ms (9,031 ms (9s)) |
| Build modules only | 1,756 ms (1,756 ms (1s)) |
| Build with modules (rest) | 10,579 ms (10,579 ms (10s)) |
| Peak ops/sec | 2,564,103 |
| Peak scenario | jac313_store_005_XS (none/on) — [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_on.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 201 | 768,049 | 85.81 KiB |
|  |  | jtext | 50 | 637,349 | 76.20 KiB |
|  |  | sql | 50 | 801,925 | 84.00 KiB |
| jac313_store_005_TS | on | binary | 201 | 868,810 | 85.81 KiB |
|  |  | jtext | 50 | 1,057,082 | 76.02 KiB |
|  |  | sql | 502 | 874,891 | 84.00 KiB |
| jac313_store_005_XS | off | binary | 100 | 977,517 | 85.81 KiB |
|  |  | jtext | 50 | 781,250 | 73.86 KiB |
|  |  | sql | 50 | 930,233 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 151 | 803,213 | 85.81 KiB |
|  |  | jtext | 50 | 901,713 | 73.86 KiB |
|  |  | sql | 502 | 870,322 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 502 | N/A | 202.99 KiB |
|  |  | jtext | 352 | N/A | 181.66 KiB |
|  |  | sql | 351 | N/A | 256.00 KiB |
| jac313_store_006_TS | on | binary | 502 | N/A | 202.99 KiB |
|  |  | jtext | 402 | N/A | 181.61 KiB |
|  |  | sql | 1,657 | N/A | 256.00 KiB |
| jac313_store_006_XS | off | binary | 803 | N/A | 202.99 KiB |
|  |  | jtext | 352 | N/A | 179.48 KiB |
|  |  | sql | 351 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 502 | N/A | 202.99 KiB |
|  |  | jtext | 402 | N/A | 179.48 KiB |
|  |  | sql | 1,455 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 201 | 944,287 | 202.99 KiB |
|  |  | jtext | 50 | 761,615 | 181.68 KiB |
|  |  | sql | 50 | 644,330 | 256.00 KiB |
| jac313_store_007_TS | on | binary | 150 | 926,784 | 202.99 KiB |
|  |  | jtext | 50 | 729,395 | 181.82 KiB |
|  |  | sql | 1,004 | 741,290 | 256.00 KiB |
| jac313_store_007_XS | off | binary | 251 | 741,290 | 202.99 KiB |
|  |  | jtext | 50 | 946,970 | 179.48 KiB |
|  |  | sql | 50 | 865,052 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 151 | 1,035,197 | 202.99 KiB |
|  |  | jtext | 50 | 707,714 | 179.48 KiB |
|  |  | sql | 1,054 | 825,083 | 248.00 KiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 201 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.64 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_001_TS | on | binary | 100 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 953 | - | 52.00 KiB |
| jac313_store_001_XS | off | binary | 201 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.41 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_001_XS | on | binary | 150 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.41 KiB |
|  |  | sql | 904 | - | 52.00 KiB |
| jac313_store_002_TS | off | binary | 201 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_002_TS | on | binary | 150 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 1,004 | - | 52.00 KiB |
| jac313_store_002_XS | off | binary | 402 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.41 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_002_XS | on | binary | 150 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.41 KiB |
|  |  | sql | 1,004 | - | 52.00 KiB |
| jac313_store_003_TS | off | binary | 502 | - | 202.99 KiB |
|  |  | jtext | 351 | - | 181.76 KiB |
|  |  | sql | 351 | - | 256.00 KiB |
| jac313_store_003_TS | on | binary | 553 | - | 202.99 KiB |
|  |  | jtext | 401 | - | 181.66 KiB |
|  |  | sql | 1,305 | - | 256.00 KiB |
| jac313_store_003_XS | off | binary | 703 | - | 202.99 KiB |
|  |  | jtext | 351 | - | 179.48 KiB |
|  |  | sql | 351 | - | 248.00 KiB |
| jac313_store_003_XS | on | binary | 452 | - | 202.99 KiB |
|  |  | jtext | 402 | - | 179.48 KiB |
|  |  | sql | 1,305 | - | 248.00 KiB |
| jac313_store_004_TS | off | binary | 200 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_004_TS | on | binary | 100 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 1,005 | - | 52.00 KiB |
| jac313_store_004_XS | off | binary | 200 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.41 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_004_XS | on | binary | 100 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.41 KiB |
|  |  | sql | 1,054 | - | 52.00 KiB |
| jac313_store_005_TS | off | binary | 201 | 768,049 | 85.81 KiB |
|  |  | jtext | 50 | 637,349 | 76.20 KiB |
|  |  | sql | 50 | 801,925 | 84.00 KiB |
| jac313_store_005_TS | on | binary | 201 | 868,810 | 85.81 KiB |
|  |  | jtext | 50 | 1,057,082 | 76.02 KiB |
|  |  | sql | 502 | 874,891 | 84.00 KiB |
| jac313_store_005_XS | off | binary | 100 | 977,517 | 85.81 KiB |
|  |  | jtext | 50 | 781,250 | 73.86 KiB |
|  |  | sql | 50 | 930,233 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 151 | 803,213 | 85.81 KiB |
|  |  | jtext | 50 | 901,713 | 73.86 KiB |
|  |  | sql | 502 | 870,322 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 502 | N/A | 202.99 KiB |
|  |  | jtext | 352 | N/A | 181.66 KiB |
|  |  | sql | 351 | N/A | 256.00 KiB |
| jac313_store_006_TS | on | binary | 502 | N/A | 202.99 KiB |
|  |  | jtext | 402 | N/A | 181.61 KiB |
|  |  | sql | 1,657 | N/A | 256.00 KiB |
| jac313_store_006_XS | off | binary | 803 | N/A | 202.99 KiB |
|  |  | jtext | 352 | N/A | 179.48 KiB |
|  |  | sql | 351 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 502 | N/A | 202.99 KiB |
|  |  | jtext | 402 | N/A | 179.48 KiB |
|  |  | sql | 1,455 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 201 | 944,287 | 202.99 KiB |
|  |  | jtext | 50 | 761,615 | 181.68 KiB |
|  |  | sql | 50 | 644,330 | 256.00 KiB |
| jac313_store_007_TS | on | binary | 150 | 926,784 | 202.99 KiB |
|  |  | jtext | 50 | 729,395 | 181.82 KiB |
|  |  | sql | 1,004 | 741,290 | 256.00 KiB |
| jac313_store_007_XS | off | binary | 251 | 741,290 | 202.99 KiB |
|  |  | jtext | 50 | 946,970 | 179.48 KiB |
|  |  | sql | 50 | 865,052 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 151 | 1,035,197 | 202.99 KiB |
|  |  | jtext | 50 | 707,714 | 179.48 KiB |
|  |  | sql | 1,054 | 825,083 | 248.00 KiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 201 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 100 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 50 | - | 20.64 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 953 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 201 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 150 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 50 | - | 20.41 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 50 | - | 20.41 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 904 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 201 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 150 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 1,004 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 402 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 150 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 50 | - | 20.41 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 50 | - | 20.41 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 1,004 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 502 | - | 202.99 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 553 | - | 202.99 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 351 | - | 181.76 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 401 | - | 181.66 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 351 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 402 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 351 | - | 256.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 1,305 | - | 256.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 703 | - | 202.99 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 452 | - | 202.99 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 351 | - | 179.48 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 402 | - | 179.48 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 351 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 402 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 351 | - | 248.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 1,305 | - | 248.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 200 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 100 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 1,005 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 200 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 100 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 50 | - | 20.41 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 50 | - | 20.41 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 1,054 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 201 | 768,049 | 85.81 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 201 | 868,810 | 85.81 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 50 | 637,349 | 76.20 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 50 | 1,057,082 | 76.02 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 50 | 2,123,142 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 50 | 624,220 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 50 | 801,925 | 84.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 502 | 874,891 | 84.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 100 | 977,517 | 85.81 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 151 | 803,213 | 85.81 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 50 | 781,250 | 73.86 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 50 | 901,713 | 73.86 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 50 | 2,145,923 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 50 | 2,564,103 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 50 | 930,233 | 84.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 502 | 870,322 | 84.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 502 | N/A | 202.99 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 502 | N/A | 202.99 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 352 | N/A | 181.66 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 402 | N/A | 181.61 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 352 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 402 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 351 | N/A | 256.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 1,657 | N/A | 256.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 803 | N/A | 202.99 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 502 | N/A | 202.99 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 352 | N/A | 179.48 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 402 | N/A | 179.48 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 351 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 401 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 351 | N/A | 248.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 1,455 | N/A | 248.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 201 | 944,287 | 202.99 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 150 | 926,784 | 202.99 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 50 | 761,615 | 181.68 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 50 | 729,395 | 181.82 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 50 | 1,107,420 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 50 | 2,070,393 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 50 | 644,330 | 256.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 1,004 | 741,290 | 256.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 251 | 741,290 | 202.99 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 151 | 1,035,197 | 202.99 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 50 | 946,970 | 179.48 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 50 | 707,714 | 179.48 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 50 | 2,217,295 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 50 | 1,366,120 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 50 | 865,052 | 248.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 1,054 | 825,083 | 248.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 1,004 | 833,333 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc16_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 1,054 | 633,312 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc16_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc16_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc16_unit_off.log) |
