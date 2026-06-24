# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-24 16:54:03Z |
| OS | fedora-44 |
| Compiler | gcc16 |
| Build type | Release |
| Disk | x7k |
| Size | smoke (Smoke) |
| Group ID | 86 |
| Environment | jac313-008 · Fedora Linux 44 (Workstation Edition) (7.0.12-201.fc44.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 23 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 34,773 ms (34s) |
| Peak ops/sec | 3,289,474 |
| Peak scenario | jac313_store_007_XS (none/on) — [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_on.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 201 | 1,138,952 | 85.81 KiB |
|  |  | jtext | 50 | 576,037 | 76.00 KiB |
|  |  | sql | 50 | 641,026 | 84.00 KiB |
| jac313_store_005_TS | on | binary | 150 | 1,721,170 | 85.81 KiB |
|  |  | jtext | 50 | 676,133 | 75.83 KiB |
|  |  | sql | 602 | 769,231 | 84.00 KiB |
| jac313_store_005_XS | off | binary | 251 | 539,084 | 85.81 KiB |
|  |  | jtext | 50 | 361,402 | 73.87 KiB |
|  |  | sql | 50 | 909,918 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 201 | 850,340 | 85.81 KiB |
|  |  | jtext | 50 | 790,514 | 73.87 KiB |
|  |  | sql | 502 | 712,251 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 502 | N/A | 203.00 KiB |
|  |  | jtext | 351 | N/A | 181.93 KiB |
|  |  | sql | 352 | N/A | 256.00 KiB |
| jac313_store_006_TS | on | binary | 452 | N/A | 203.00 KiB |
|  |  | jtext | 402 | N/A | 181.45 KiB |
|  |  | sql | 1,355 | N/A | 256.00 KiB |
| jac313_store_006_XS | off | binary | 502 | N/A | 203.00 KiB |
|  |  | jtext | 351 | N/A | 179.49 KiB |
|  |  | sql | 351 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 452 | N/A | 203.00 KiB |
|  |  | jtext | 402 | N/A | 179.49 KiB |
|  |  | sql | 1,406 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 150 | 674,764 | 203.00 KiB |
|  |  | jtext | 50 | 1,009,082 | 181.45 KiB |
|  |  | sql | 50 | 1,117,318 | 256.00 KiB |
| jac313_store_007_TS | on | binary | 100 | 1,369,863 | 203.00 KiB |
|  |  | jtext | 50 | 630,120 | 181.84 KiB |
|  |  | sql | 1,055 | 659,196 | 256.00 KiB |
| jac313_store_007_XS | off | binary | 702 | 1,283,697 | 203.00 KiB |
|  |  | jtext | 50 | 980,392 | 179.49 KiB |
|  |  | sql | 50 | 759,301 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 201 | 816,993 | 203.00 KiB |
|  |  | jtext | 50 | 856,898 | 179.49 KiB |
|  |  | sql | 1,054 | 984,252 | 248.00 KiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 201 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.62 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_001_TS | on | binary | 150 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.62 KiB |
|  |  | sql | 904 | - | 52.00 KiB |
| jac313_store_001_XS | off | binary | 201 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.43 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_001_XS | on | binary | 150 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.43 KiB |
|  |  | sql | 1,054 | - | 52.00 KiB |
| jac313_store_002_TS | off | binary | 201 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.62 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_002_TS | on | binary | 150 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.62 KiB |
|  |  | sql | 1,004 | - | 52.00 KiB |
| jac313_store_002_XS | off | binary | 251 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.43 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_002_XS | on | binary | 150 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.43 KiB |
|  |  | sql | 1,004 | - | 52.00 KiB |
| jac313_store_003_TS | off | binary | 502 | - | 203.00 KiB |
|  |  | jtext | 351 | - | 181.45 KiB |
|  |  | sql | 351 | - | 256.00 KiB |
| jac313_store_003_TS | on | binary | 552 | - | 203.00 KiB |
|  |  | jtext | 402 | - | 181.64 KiB |
|  |  | sql | 1,305 | - | 256.00 KiB |
| jac313_store_003_XS | off | binary | 602 | - | 203.00 KiB |
|  |  | jtext | 351 | - | 179.49 KiB |
|  |  | sql | 351 | - | 248.00 KiB |
| jac313_store_003_XS | on | binary | 452 | - | 203.00 KiB |
|  |  | jtext | 402 | - | 179.49 KiB |
|  |  | sql | 1,406 | - | 248.00 KiB |
| jac313_store_004_TS | off | binary | 200 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.62 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_004_TS | on | binary | 100 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.62 KiB |
|  |  | sql | 1,055 | - | 52.00 KiB |
| jac313_store_004_XS | off | binary | 201 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.43 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_004_XS | on | binary | 150 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.43 KiB |
|  |  | sql | 1,004 | - | 52.00 KiB |
| jac313_store_005_TS | off | binary | 201 | 1,138,952 | 85.81 KiB |
|  |  | jtext | 50 | 576,037 | 76.00 KiB |
|  |  | sql | 50 | 641,026 | 84.00 KiB |
| jac313_store_005_TS | on | binary | 150 | 1,721,170 | 85.81 KiB |
|  |  | jtext | 50 | 676,133 | 75.83 KiB |
|  |  | sql | 602 | 769,231 | 84.00 KiB |
| jac313_store_005_XS | off | binary | 251 | 539,084 | 85.81 KiB |
|  |  | jtext | 50 | 361,402 | 73.87 KiB |
|  |  | sql | 50 | 909,918 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 201 | 850,340 | 85.81 KiB |
|  |  | jtext | 50 | 790,514 | 73.87 KiB |
|  |  | sql | 502 | 712,251 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 502 | N/A | 203.00 KiB |
|  |  | jtext | 351 | N/A | 181.93 KiB |
|  |  | sql | 352 | N/A | 256.00 KiB |
| jac313_store_006_TS | on | binary | 452 | N/A | 203.00 KiB |
|  |  | jtext | 402 | N/A | 181.45 KiB |
|  |  | sql | 1,355 | N/A | 256.00 KiB |
| jac313_store_006_XS | off | binary | 502 | N/A | 203.00 KiB |
|  |  | jtext | 351 | N/A | 179.49 KiB |
|  |  | sql | 351 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 452 | N/A | 203.00 KiB |
|  |  | jtext | 402 | N/A | 179.49 KiB |
|  |  | sql | 1,406 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 150 | 674,764 | 203.00 KiB |
|  |  | jtext | 50 | 1,009,082 | 181.45 KiB |
|  |  | sql | 50 | 1,117,318 | 256.00 KiB |
| jac313_store_007_TS | on | binary | 100 | 1,369,863 | 203.00 KiB |
|  |  | jtext | 50 | 630,120 | 181.84 KiB |
|  |  | sql | 1,055 | 659,196 | 256.00 KiB |
| jac313_store_007_XS | off | binary | 702 | 1,283,697 | 203.00 KiB |
|  |  | jtext | 50 | 980,392 | 179.49 KiB |
|  |  | sql | 50 | 759,301 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 201 | 816,993 | 203.00 KiB |
|  |  | jtext | 50 | 856,898 | 179.49 KiB |
|  |  | sql | 1,054 | 984,252 | 248.00 KiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 201 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 150 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 50 | - | 20.62 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 50 | - | 20.62 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 904 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 201 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 150 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 50 | - | 20.43 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 50 | - | 20.43 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 1,054 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 201 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 150 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 50 | - | 20.62 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 50 | - | 20.62 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 1,004 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 251 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 150 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 50 | - | 20.43 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 50 | - | 20.43 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 1,004 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 502 | - | 203.00 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 552 | - | 203.00 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 351 | - | 181.45 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 402 | - | 181.64 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 352 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 402 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 351 | - | 256.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 1,305 | - | 256.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 602 | - | 203.00 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 452 | - | 203.00 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 351 | - | 179.49 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 402 | - | 179.49 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 351 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 402 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 351 | - | 248.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 1,406 | - | 248.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 200 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 100 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 50 | - | 20.62 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 50 | - | 20.62 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 1,055 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 201 | - | 20.40 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 150 | - | 20.40 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 50 | - | 20.43 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 50 | - | 20.43 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 1,004 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 201 | 1,138,952 | 85.81 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 150 | 1,721,170 | 85.81 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 50 | 576,037 | 76.00 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 50 | 676,133 | 75.83 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 50 | 2,178,649 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 50 | 2,638,522 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 50 | 641,026 | 84.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 602 | 769,231 | 84.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 251 | 539,084 | 85.81 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 201 | 850,340 | 85.81 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 50 | 361,402 | 73.87 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 50 | 790,514 | 73.87 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 50 | 625,391 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 50 | 1,912,046 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 50 | 909,918 | 84.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 502 | 712,251 | 84.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 502 | N/A | 203.00 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 452 | N/A | 203.00 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 351 | N/A | 181.93 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 402 | N/A | 181.45 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 351 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 401 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 352 | N/A | 256.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 1,355 | N/A | 256.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 502 | N/A | 203.00 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 452 | N/A | 203.00 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 351 | N/A | 179.49 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 402 | N/A | 179.49 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 351 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 402 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 351 | N/A | 248.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 1,406 | N/A | 248.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 150 | 674,764 | 203.00 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 100 | 1,369,863 | 203.00 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 50 | 1,009,082 | 181.45 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 50 | 630,120 | 181.84 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 50 | 1,956,947 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 50 | 1,479,290 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 50 | 1,117,318 | 256.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 1,055 | 659,196 | 256.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 702 | 1,283,697 | 203.00 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 201 | 816,993 | 203.00 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 50 | 980,392 | 179.49 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 50 | 856,898 | 179.49 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 50 | 905,797 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 50 | 3,289,474 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 50 | 759,301 | 248.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 1,054 | 984,252 | 248.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 953 | 1,027,749 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc16_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 1,004 | 973,710 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc16_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc16_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc16_unit_off.log) |
