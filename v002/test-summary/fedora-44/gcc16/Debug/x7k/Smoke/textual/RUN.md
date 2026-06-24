# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-24 16:41:15Z |
| OS | fedora-44 |
| Compiler | gcc16 |
| Build type | Debug |
| Disk | x7k |
| Size | smoke (Smoke) |
| Group ID | 84 |
| Environment | jac313-008 · Fedora Linux 44 (Workstation Edition) (7.0.12-201.fc44.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 23 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 35,530 ms (35s) |
| Peak ops/sec | 2,192,982 |
| Peak scenario | jac313_store_005_XS (none/on) — [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_on.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 201 | 680,272 | 85.81 KiB |
|  |  | jtext | 50 | 533,618 | 76.24 KiB |
|  |  | sql | 50 | 678,887 | 84.00 KiB |
| jac313_store_005_TS | on | binary | 151 | 832,639 | 85.81 KiB |
|  |  | jtext | 50 | 812,348 | 75.95 KiB |
|  |  | sql | 502 | 498,008 | 84.00 KiB |
| jac313_store_005_XS | off | binary | 201 | 864,304 | 85.81 KiB |
|  |  | jtext | 50 | 602,773 | 73.86 KiB |
|  |  | sql | 50 | 745,156 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 100 | 700,280 | 85.81 KiB |
|  |  | jtext | 50 | 722,543 | 73.86 KiB |
|  |  | sql | 552 | 803,213 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 502 | N/A | 202.99 KiB |
|  |  | jtext | 351 | N/A | 181.87 KiB |
|  |  | sql | 351 | N/A | 260.00 KiB |
| jac313_store_006_TS | on | binary | 552 | N/A | 202.99 KiB |
|  |  | jtext | 402 | N/A | 181.69 KiB |
|  |  | sql | 1,356 | N/A | 260.00 KiB |
| jac313_store_006_XS | off | binary | 452 | N/A | 202.99 KiB |
|  |  | jtext | 351 | N/A | 179.48 KiB |
|  |  | sql | 351 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 452 | N/A | 202.99 KiB |
|  |  | jtext | 402 | N/A | 179.48 KiB |
|  |  | sql | 1,356 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 703 | 916,590 | 202.99 KiB |
|  |  | jtext | 50 | 672,948 | 181.93 KiB |
|  |  | sql | 50 | 805,802 | 256.00 KiB |
| jac313_store_007_TS | on | binary | 150 | 662,252 | 202.99 KiB |
|  |  | jtext | 50 | 979,432 | 181.81 KiB |
|  |  | sql | 1,155 | 598,444 | 256.00 KiB |
| jac313_store_007_XS | off | binary | 201 | 721,501 | 202.99 KiB |
|  |  | jtext | 50 | 809,061 | 179.48 KiB |
|  |  | sql | 50 | 882,613 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 150 | 632,911 | 202.99 KiB |
|  |  | jtext | 50 | 879,507 | 179.48 KiB |
|  |  | sql | 1,405 | 573,394 | 248.00 KiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 201 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_001_TS | on | binary | 150 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 954 | - | 52.00 KiB |
| jac313_store_001_XS | off | binary | 201 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.41 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_001_XS | on | binary | 150 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.41 KiB |
|  |  | sql | 1,054 | - | 52.00 KiB |
| jac313_store_002_TS | off | binary | 201 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_002_TS | on | binary | 201 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 954 | - | 52.00 KiB |
| jac313_store_002_XS | off | binary | 603 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.41 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_002_XS | on | binary | 100 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.41 KiB |
|  |  | sql | 954 | - | 52.00 KiB |
| jac313_store_003_TS | off | binary | 452 | - | 202.99 KiB |
|  |  | jtext | 352 | - | 181.82 KiB |
|  |  | sql | 351 | - | 256.00 KiB |
| jac313_store_003_TS | on | binary | 452 | - | 202.99 KiB |
|  |  | jtext | 401 | - | 181.72 KiB |
|  |  | sql | 1,406 | - | 256.00 KiB |
| jac313_store_003_XS | off | binary | 502 | - | 202.99 KiB |
|  |  | jtext | 351 | - | 179.48 KiB |
|  |  | sql | 351 | - | 248.00 KiB |
| jac313_store_003_XS | on | binary | 452 | - | 202.99 KiB |
|  |  | jtext | 402 | - | 179.48 KiB |
|  |  | sql | 1,355 | - | 248.00 KiB |
| jac313_store_004_TS | off | binary | 201 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_004_TS | on | binary | 201 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 1,004 | - | 52.00 KiB |
| jac313_store_004_XS | off | binary | 251 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.41 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_004_XS | on | binary | 150 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.41 KiB |
|  |  | sql | 1,155 | - | 52.00 KiB |
| jac313_store_005_TS | off | binary | 201 | 680,272 | 85.81 KiB |
|  |  | jtext | 50 | 533,618 | 76.24 KiB |
|  |  | sql | 50 | 678,887 | 84.00 KiB |
| jac313_store_005_TS | on | binary | 151 | 832,639 | 85.81 KiB |
|  |  | jtext | 50 | 812,348 | 75.95 KiB |
|  |  | sql | 502 | 498,008 | 84.00 KiB |
| jac313_store_005_XS | off | binary | 201 | 864,304 | 85.81 KiB |
|  |  | jtext | 50 | 602,773 | 73.86 KiB |
|  |  | sql | 50 | 745,156 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 100 | 700,280 | 85.81 KiB |
|  |  | jtext | 50 | 722,543 | 73.86 KiB |
|  |  | sql | 552 | 803,213 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 502 | N/A | 202.99 KiB |
|  |  | jtext | 351 | N/A | 181.87 KiB |
|  |  | sql | 351 | N/A | 260.00 KiB |
| jac313_store_006_TS | on | binary | 552 | N/A | 202.99 KiB |
|  |  | jtext | 402 | N/A | 181.69 KiB |
|  |  | sql | 1,356 | N/A | 260.00 KiB |
| jac313_store_006_XS | off | binary | 452 | N/A | 202.99 KiB |
|  |  | jtext | 351 | N/A | 179.48 KiB |
|  |  | sql | 351 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 452 | N/A | 202.99 KiB |
|  |  | jtext | 402 | N/A | 179.48 KiB |
|  |  | sql | 1,356 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 703 | 916,590 | 202.99 KiB |
|  |  | jtext | 50 | 672,948 | 181.93 KiB |
|  |  | sql | 50 | 805,802 | 256.00 KiB |
| jac313_store_007_TS | on | binary | 150 | 662,252 | 202.99 KiB |
|  |  | jtext | 50 | 979,432 | 181.81 KiB |
|  |  | sql | 1,155 | 598,444 | 256.00 KiB |
| jac313_store_007_XS | off | binary | 201 | 721,501 | 202.99 KiB |
|  |  | jtext | 50 | 809,061 | 179.48 KiB |
|  |  | sql | 50 | 882,613 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 150 | 632,911 | 202.99 KiB |
|  |  | jtext | 50 | 879,507 | 179.48 KiB |
|  |  | sql | 1,405 | 573,394 | 248.00 KiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 201 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 150 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 954 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 201 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 150 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 50 | - | 20.41 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 50 | - | 20.41 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 1,054 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 201 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 201 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 954 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 603 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 100 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 50 | - | 20.41 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 50 | - | 20.41 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 954 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 452 | - | 202.99 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 452 | - | 202.99 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 352 | - | 181.82 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 401 | - | 181.72 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 351 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 401 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 351 | - | 256.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 1,406 | - | 256.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 502 | - | 202.99 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 452 | - | 202.99 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 351 | - | 179.48 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 402 | - | 179.48 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 351 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 402 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 351 | - | 248.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 1,355 | - | 248.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 201 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 201 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 1,004 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 251 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 150 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 50 | - | 20.41 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 50 | - | 20.41 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 1,155 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 201 | 680,272 | 85.81 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 151 | 832,639 | 85.81 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 50 | 533,618 | 76.24 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 50 | 812,348 | 75.95 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 50 | 759,878 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 50 | 641,026 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 50 | 678,887 | 84.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 502 | 498,008 | 84.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 201 | 864,304 | 85.81 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 100 | 700,280 | 85.81 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 50 | 602,773 | 73.86 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 50 | 722,543 | 73.86 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 50 | 807,754 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 50 | 2,192,982 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 50 | 745,156 | 84.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 552 | 803,213 | 84.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 502 | N/A | 202.99 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 552 | N/A | 202.99 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 351 | N/A | 181.87 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 402 | N/A | 181.69 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 351 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 401 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 351 | N/A | 260.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 1,356 | N/A | 260.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 452 | N/A | 202.99 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 452 | N/A | 202.99 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 351 | N/A | 179.48 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 402 | N/A | 179.48 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 352 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 402 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 351 | N/A | 248.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 1,356 | N/A | 248.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 703 | 916,590 | 202.99 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 150 | 662,252 | 202.99 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 50 | 672,948 | 181.93 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 50 | 979,432 | 181.81 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 50 | 1,184,834 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 50 | 1,051,525 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 50 | 805,802 | 256.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 1,155 | 598,444 | 256.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 201 | 721,501 | 202.99 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 150 | 632,911 | 202.99 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 50 | 809,061 | 179.48 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 50 | 879,507 | 179.48 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 50 | 681,663 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 50 | 977,517 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 50 | 882,613 | 248.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 1,405 | 573,394 | 248.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 1,004 | 683,527 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc16_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 1,054 | 564,653 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc16_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc16_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc16_unit_off.log) |
