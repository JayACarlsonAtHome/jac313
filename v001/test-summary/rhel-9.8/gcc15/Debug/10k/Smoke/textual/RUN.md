# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-20 04:41:07Z |
| OS | rhel-9.8 |
| Compiler | gcc15 |
| Build type | Debug |
| Disk | 10k |
| Size | smoke (Smoke) |
| Group ID | 7 |
| Environment | jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 17,647 ms (17s) |
| Peak ops/sec | 803,213 |
| Peak scenario | jac313_store_007_XS (none/off) — [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 50 | 327,011 | 85.80 KiB |
|  |  | jtext | 50 | 325,203 | 76.73 KiB |
|  |  | sql | 50 | 352,734 | 84.00 KiB |
| jac313_store_005_TS | on | binary | 50 | 337,041 | 85.80 KiB |
|  |  | jtext | 50 | 368,732 | 76.73 KiB |
|  |  | sql | 100 | 323,311 | 84.00 KiB |
| jac313_store_005_XS | off | binary | 50 | 341,064 | 85.80 KiB |
|  |  | jtext | 50 | 342,114 | 73.80 KiB |
|  |  | sql | 50 | 321,958 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 50 | 315,756 | 85.80 KiB |
|  |  | jtext | 50 | 346,981 | 73.80 KiB |
|  |  | sql | 100 | 339,098 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 351 | N/A | 202.98 KiB |
|  |  | jtext | 402 | N/A | 182.33 KiB |
|  |  | sql | 351 | N/A | 256.00 KiB |
| jac313_store_006_TS | on | binary | 402 | N/A | 202.98 KiB |
|  |  | jtext | 402 | N/A | 182.33 KiB |
|  |  | sql | 452 | N/A | 256.00 KiB |
| jac313_store_006_XS | off | binary | 351 | N/A | 202.98 KiB |
|  |  | jtext | 401 | N/A | 179.40 KiB |
|  |  | sql | 351 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 351 | N/A | 202.98 KiB |
|  |  | jtext | 401 | N/A | 179.40 KiB |
|  |  | sql | 452 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 50 | 425,713 | 202.98 KiB |
|  |  | jtext | 100 | 406,835 | 182.33 KiB |
|  |  | sql | 50 | 324,465 | 256.00 KiB |
| jac313_store_007_TS | on | binary | 50 | 407,997 | 202.98 KiB |
|  |  | jtext | 50 | 417,885 | 182.33 KiB |
|  |  | sql | 150 | 418,585 | 256.00 KiB |
| jac313_store_007_XS | off | binary | 50 | 349,162 | 202.98 KiB |
|  |  | jtext | 100 | 342,466 | 179.40 KiB |
|  |  | sql | 50 | 384,172 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 50 | 350,754 | 202.98 KiB |
|  |  | jtext | 100 | 369,004 | 179.40 KiB |
|  |  | sql | 150 | 390,168 | 248.00 KiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 50 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.62 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_001_TS | on | binary | 50 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.62 KiB |
|  |  | sql | 150 | - | 52.00 KiB |
| jac313_store_001_XS | off | binary | 50 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.33 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_001_XS | on | binary | 50 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.33 KiB |
|  |  | sql | 100 | - | 52.00 KiB |
| jac313_store_002_TS | off | binary | 50 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.62 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_002_TS | on | binary | 50 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.62 KiB |
|  |  | sql | 100 | - | 52.00 KiB |
| jac313_store_002_XS | off | binary | 50 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.33 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_002_XS | on | binary | 50 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.33 KiB |
|  |  | sql | 151 | - | 52.00 KiB |
| jac313_store_003_TS | off | binary | 351 | - | 202.98 KiB |
|  |  | jtext | 402 | - | 182.33 KiB |
|  |  | sql | 351 | - | 256.00 KiB |
| jac313_store_003_TS | on | binary | 402 | - | 202.98 KiB |
|  |  | jtext | 402 | - | 182.33 KiB |
|  |  | sql | 502 | - | 256.00 KiB |
| jac313_store_003_XS | off | binary | 351 | - | 202.98 KiB |
|  |  | jtext | 401 | - | 179.40 KiB |
|  |  | sql | 351 | - | 248.00 KiB |
| jac313_store_003_XS | on | binary | 352 | - | 202.98 KiB |
|  |  | jtext | 401 | - | 179.40 KiB |
|  |  | sql | 502 | - | 248.00 KiB |
| jac313_store_004_TS | off | binary | 50 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.62 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_004_TS | on | binary | 50 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.62 KiB |
|  |  | sql | 150 | - | 52.00 KiB |
| jac313_store_004_XS | off | binary | 50 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.33 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_004_XS | on | binary | 50 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.33 KiB |
|  |  | sql | 150 | - | 52.00 KiB |
| jac313_store_005_TS | off | binary | 50 | 327,011 | 85.80 KiB |
|  |  | jtext | 50 | 325,203 | 76.73 KiB |
|  |  | sql | 50 | 352,734 | 84.00 KiB |
| jac313_store_005_TS | on | binary | 50 | 337,041 | 85.80 KiB |
|  |  | jtext | 50 | 368,732 | 76.73 KiB |
|  |  | sql | 100 | 323,311 | 84.00 KiB |
| jac313_store_005_XS | off | binary | 50 | 341,064 | 85.80 KiB |
|  |  | jtext | 50 | 342,114 | 73.80 KiB |
|  |  | sql | 50 | 321,958 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 50 | 315,756 | 85.80 KiB |
|  |  | jtext | 50 | 346,981 | 73.80 KiB |
|  |  | sql | 100 | 339,098 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 351 | N/A | 202.98 KiB |
|  |  | jtext | 402 | N/A | 182.33 KiB |
|  |  | sql | 351 | N/A | 256.00 KiB |
| jac313_store_006_TS | on | binary | 402 | N/A | 202.98 KiB |
|  |  | jtext | 402 | N/A | 182.33 KiB |
|  |  | sql | 452 | N/A | 256.00 KiB |
| jac313_store_006_XS | off | binary | 351 | N/A | 202.98 KiB |
|  |  | jtext | 401 | N/A | 179.40 KiB |
|  |  | sql | 351 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 351 | N/A | 202.98 KiB |
|  |  | jtext | 401 | N/A | 179.40 KiB |
|  |  | sql | 452 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 50 | 425,713 | 202.98 KiB |
|  |  | jtext | 100 | 406,835 | 182.33 KiB |
|  |  | sql | 50 | 324,465 | 256.00 KiB |
| jac313_store_007_TS | on | binary | 50 | 407,997 | 202.98 KiB |
|  |  | jtext | 50 | 417,885 | 182.33 KiB |
|  |  | sql | 150 | 418,585 | 256.00 KiB |
| jac313_store_007_XS | off | binary | 50 | 349,162 | 202.98 KiB |
|  |  | jtext | 100 | 342,466 | 179.40 KiB |
|  |  | sql | 50 | 384,172 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 50 | 350,754 | 202.98 KiB |
|  |  | jtext | 100 | 369,004 | 179.40 KiB |
|  |  | sql | 150 | 390,168 | 248.00 KiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 50 | - | 20.39 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc15_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 50 | - | 20.39 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc15_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 50 | - | 20.62 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc15_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 50 | - | 20.62 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc15_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc15_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc15_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc15_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 150 | - | 52.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc15_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 50 | - | 20.39 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc15_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 50 | - | 20.39 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc15_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 50 | - | 20.33 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc15_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 50 | - | 20.33 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc15_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 50 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc15_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 50 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc15_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc15_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 100 | - | 52.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc15_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 50 | - | 20.39 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc15_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 50 | - | 20.39 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc15_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 50 | - | 20.62 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc15_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 50 | - | 20.62 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc15_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc15_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc15_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc15_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 100 | - | 52.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc15_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 50 | - | 20.39 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc15_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 50 | - | 20.39 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc15_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 50 | - | 20.33 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc15_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 50 | - | 20.33 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc15_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 50 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc15_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 50 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc15_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc15_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 151 | - | 52.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc15_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 351 | - | 202.98 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc15_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 402 | - | 202.98 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc15_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 402 | - | 182.33 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc15_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 402 | - | 182.33 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc15_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 351 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc15_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 402 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc15_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 351 | - | 256.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc15_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 502 | - | 256.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc15_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 351 | - | 202.98 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc15_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 352 | - | 202.98 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc15_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 401 | - | 179.40 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc15_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 401 | - | 179.40 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc15_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 351 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc15_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 351 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc15_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 351 | - | 248.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc15_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 502 | - | 248.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc15_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 50 | - | 20.39 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc15_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 50 | - | 20.39 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc15_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 50 | - | 20.62 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc15_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 50 | - | 20.62 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc15_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc15_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc15_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc15_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 150 | - | 52.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc15_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 50 | - | 20.39 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc15_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 50 | - | 20.39 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc15_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 50 | - | 20.33 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc15_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 50 | - | 20.33 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc15_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 50 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc15_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 50 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc15_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc15_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 150 | - | 52.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc15_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 50 | 327,011 | 85.80 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 50 | 337,041 | 85.80 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 50 | 325,203 | 76.73 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 50 | 368,732 | 76.73 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 50 | 672,495 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 50 | 748,503 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 50 | 352,734 | 84.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 100 | 323,311 | 84.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 50 | 341,064 | 85.80 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 50 | 315,756 | 85.80 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 50 | 342,114 | 73.80 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 50 | 346,981 | 73.80 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 50 | 757,002 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 50 | 759,301 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 50 | 321,958 | 84.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc15_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 100 | 339,098 | 84.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc15_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 351 | N/A | 202.98 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc15_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 402 | N/A | 202.98 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc15_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 402 | N/A | 182.33 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc15_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 402 | N/A | 182.33 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc15_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 351 | N/A | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc15_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 402 | N/A | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc15_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 351 | N/A | 256.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc15_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 452 | N/A | 256.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc15_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 351 | N/A | 202.98 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc15_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 351 | N/A | 202.98 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc15_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 401 | N/A | 179.40 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc15_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 401 | N/A | 179.40 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc15_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 351 | N/A | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc15_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 351 | N/A | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc15_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 351 | N/A | 248.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc15_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 452 | N/A | 248.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc15_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 50 | 425,713 | 202.98 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 50 | 407,997 | 202.98 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 100 | 406,835 | 182.33 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 50 | 417,885 | 182.33 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 50 | 750,188 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 50 | 678,426 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 50 | 324,465 | 256.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 150 | 418,585 | 256.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 50 | 349,162 | 202.98 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 50 | 350,754 | 202.98 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 100 | 342,466 | 179.40 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 100 | 369,004 | 179.40 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 50 | 803,213 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 50 | 791,766 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 50 | 384,172 | 248.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 150 | 390,168 | 248.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 100 | 377,644 | - | - | [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 151 | 398,724 | - | - | [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [gcc15_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc15_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 50 | - | - | - | [gcc15_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc15_unit_off.log) |
