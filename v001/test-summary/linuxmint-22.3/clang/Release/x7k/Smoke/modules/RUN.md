# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-20 23:00:30Z |
| OS | linuxmint-22.3 |
| Compiler | clang |
| Build type | Release |
| Disk | x7k |
| Size | smoke (Smoke) |
| Group ID | 69 |
| Environment | jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 21,635 ms (21s) |
| Build straight (no modules) | 96,445 ms (96,445 ms (1m 36s)) |
| Build modules only | 6,815 ms (6,815 ms (6s)) |
| Build with modules (rest) | 101,634 ms (101,634 ms (1m 41s)) |
| Peak ops/sec | 571,429 |
| Peak scenario | jac313_store_007_TS (none/on) — [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_on.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 101 | 334,896 | 85.80 KiB |
|  |  | jtext | 51 | 150,602 | 76.77 KiB |
|  |  | sql | 51 | 335,570 | 84.00 KiB |
| jac313_store_005_TS | on | binary | 101 | 322,373 | 85.80 KiB |
|  |  | jtext | 50 | 322,477 | 76.77 KiB |
|  |  | sql | 301 | 332,336 | 84.00 KiB |
| jac313_store_005_XS | off | binary | 101 | 352,361 | 85.80 KiB |
|  |  | jtext | 51 | 373,832 | 73.84 KiB |
|  |  | sql | 51 | 360,360 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 101 | 350,754 | 85.80 KiB |
|  |  | jtext | 51 | 389,105 | 73.84 KiB |
|  |  | sql | 201 | 354,233 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 453 | N/A | 202.99 KiB |
|  |  | jtext | 352 | N/A | 182.39 KiB |
|  |  | sql | 352 | N/A | 256.00 KiB |
| jac313_store_006_TS | on | binary | 504 | N/A | 202.99 KiB |
|  |  | jtext | 403 | N/A | 182.39 KiB |
|  |  | sql | 755 | N/A | 256.00 KiB |
| jac313_store_006_XS | off | binary | 403 | N/A | 202.99 KiB |
|  |  | jtext | 355 | N/A | 179.46 KiB |
|  |  | sql | 352 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 403 | N/A | 202.99 KiB |
|  |  | jtext | 353 | N/A | 179.46 KiB |
|  |  | sql | 704 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 101 | 152,439 | 202.99 KiB |
|  |  | jtext | 50 | 315,956 | 182.39 KiB |
|  |  | sql | 50 | 326,904 | 256.00 KiB |
| jac313_store_007_TS | on | binary | 101 | 320,718 | 202.99 KiB |
|  |  | jtext | 51 | 300,933 | 182.39 KiB |
|  |  | sql | 301 | 290,867 | 256.00 KiB |
| jac313_store_007_XS | off | binary | 101 | 340,832 | 202.99 KiB |
|  |  | jtext | 50 | 328,192 | 179.46 KiB |
|  |  | sql | 51 | 318,269 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 101 | 348,432 | 202.99 KiB |
|  |  | jtext | 50 | 326,264 | 179.46 KiB |
|  |  | sql | 352 | 290,444 | 248.00 KiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 101 | - | 20.40 KiB |
|  |  | jtext | 51 | - | 20.68 KiB |
|  |  | sql | 51 | - | 52.00 KiB |
| jac313_store_001_TS | on | binary | 101 | - | 20.40 KiB |
|  |  | jtext | 51 | - | 20.68 KiB |
|  |  | sql | 352 | - | 52.00 KiB |
| jac313_store_001_XS | off | binary | 51 | - | 20.40 KiB |
|  |  | jtext | 51 | - | 20.39 KiB |
|  |  | sql | 51 | - | 52.00 KiB |
| jac313_store_001_XS | on | binary | 101 | - | 20.40 KiB |
|  |  | jtext | 51 | - | 20.39 KiB |
|  |  | sql | 302 | - | 52.00 KiB |
| jac313_store_002_TS | off | binary | 101 | - | 20.40 KiB |
|  |  | jtext | 52 | - | 20.68 KiB |
|  |  | sql | 51 | - | 52.00 KiB |
| jac313_store_002_TS | on | binary | 101 | - | 20.40 KiB |
|  |  | jtext | 51 | - | 20.68 KiB |
|  |  | sql | 302 | - | 52.00 KiB |
| jac313_store_002_XS | off | binary | 51 | - | 20.40 KiB |
|  |  | jtext | 51 | - | 20.39 KiB |
|  |  | sql | 52 | - | 52.00 KiB |
| jac313_store_002_XS | on | binary | 51 | - | 20.40 KiB |
|  |  | jtext | 51 | - | 20.39 KiB |
|  |  | sql | 353 | - | 52.00 KiB |
| jac313_store_003_TS | off | binary | 403 | - | 202.99 KiB |
|  |  | jtext | 353 | - | 182.39 KiB |
|  |  | sql | 352 | - | 256.00 KiB |
| jac313_store_003_TS | on | binary | 453 | - | 202.99 KiB |
|  |  | jtext | 403 | - | 182.39 KiB |
|  |  | sql | 655 | - | 256.00 KiB |
| jac313_store_003_XS | off | binary | 403 | - | 202.99 KiB |
|  |  | jtext | 352 | - | 179.46 KiB |
|  |  | sql | 352 | - | 248.00 KiB |
| jac313_store_003_XS | on | binary | 403 | - | 202.99 KiB |
|  |  | jtext | 352 | - | 179.46 KiB |
|  |  | sql | 654 | - | 248.00 KiB |
| jac313_store_004_TS | off | binary | 101 | - | 20.40 KiB |
|  |  | jtext | 51 | - | 20.68 KiB |
|  |  | sql | 51 | - | 52.00 KiB |
| jac313_store_004_TS | on | binary | 101 | - | 20.40 KiB |
|  |  | jtext | 51 | - | 20.68 KiB |
|  |  | sql | 303 | - | 52.00 KiB |
| jac313_store_004_XS | off | binary | 101 | - | 20.40 KiB |
|  |  | jtext | 51 | - | 20.39 KiB |
|  |  | sql | 51 | - | 52.00 KiB |
| jac313_store_004_XS | on | binary | 101 | - | 20.40 KiB |
|  |  | jtext | 51 | - | 20.39 KiB |
|  |  | sql | 302 | - | 52.00 KiB |
| jac313_store_005_TS | off | binary | 101 | 334,896 | 85.80 KiB |
|  |  | jtext | 51 | 150,602 | 76.77 KiB |
|  |  | sql | 51 | 335,570 | 84.00 KiB |
| jac313_store_005_TS | on | binary | 101 | 322,373 | 85.80 KiB |
|  |  | jtext | 50 | 322,477 | 76.77 KiB |
|  |  | sql | 301 | 332,336 | 84.00 KiB |
| jac313_store_005_XS | off | binary | 101 | 352,361 | 85.80 KiB |
|  |  | jtext | 51 | 373,832 | 73.84 KiB |
|  |  | sql | 51 | 360,360 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 101 | 350,754 | 85.80 KiB |
|  |  | jtext | 51 | 389,105 | 73.84 KiB |
|  |  | sql | 201 | 354,233 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 453 | N/A | 202.99 KiB |
|  |  | jtext | 352 | N/A | 182.39 KiB |
|  |  | sql | 352 | N/A | 256.00 KiB |
| jac313_store_006_TS | on | binary | 504 | N/A | 202.99 KiB |
|  |  | jtext | 403 | N/A | 182.39 KiB |
|  |  | sql | 755 | N/A | 256.00 KiB |
| jac313_store_006_XS | off | binary | 403 | N/A | 202.99 KiB |
|  |  | jtext | 355 | N/A | 179.46 KiB |
|  |  | sql | 352 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 403 | N/A | 202.99 KiB |
|  |  | jtext | 353 | N/A | 179.46 KiB |
|  |  | sql | 704 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 101 | 152,439 | 202.99 KiB |
|  |  | jtext | 50 | 315,956 | 182.39 KiB |
|  |  | sql | 50 | 326,904 | 256.00 KiB |
| jac313_store_007_TS | on | binary | 101 | 320,718 | 202.99 KiB |
|  |  | jtext | 51 | 300,933 | 182.39 KiB |
|  |  | sql | 301 | 290,867 | 256.00 KiB |
| jac313_store_007_XS | off | binary | 101 | 340,832 | 202.99 KiB |
|  |  | jtext | 50 | 328,192 | 179.46 KiB |
|  |  | sql | 51 | 318,269 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 101 | 348,432 | 202.99 KiB |
|  |  | jtext | 50 | 326,264 | 179.46 KiB |
|  |  | sql | 352 | 290,444 | 248.00 KiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 101 | - | 20.40 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 101 | - | 20.40 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 51 | - | 20.68 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 51 | - | 20.68 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 51 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 51 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 51 | - | 52.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 352 | - | 52.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 51 | - | 20.40 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 101 | - | 20.40 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 51 | - | 20.39 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 51 | - | 20.39 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 51 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 51 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 51 | - | 52.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 302 | - | 52.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 101 | - | 20.40 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 101 | - | 20.40 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 52 | - | 20.68 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 51 | - | 20.68 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 51 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 51 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 51 | - | 52.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 302 | - | 52.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 51 | - | 20.40 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 51 | - | 20.40 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 51 | - | 20.39 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 51 | - | 20.39 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 51 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 51 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 52 | - | 52.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 353 | - | 52.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 403 | - | 202.99 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 453 | - | 202.99 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 353 | - | 182.39 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 403 | - | 182.39 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 353 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 403 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 352 | - | 256.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 655 | - | 256.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 403 | - | 202.99 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 403 | - | 202.99 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 352 | - | 179.46 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 352 | - | 179.46 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 353 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 353 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 352 | - | 248.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 654 | - | 248.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 101 | - | 20.40 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 101 | - | 20.40 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 51 | - | 20.68 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 51 | - | 20.68 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 51 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 51 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 51 | - | 52.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 303 | - | 52.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 101 | - | 20.40 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 101 | - | 20.40 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 51 | - | 20.39 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 51 | - | 20.39 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 51 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 51 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 51 | - | 52.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 302 | - | 52.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 101 | 334,896 | 85.80 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 101 | 322,373 | 85.80 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 51 | 150,602 | 76.77 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 50 | 322,477 | 76.77 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 51 | 421,585 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 51 | 480,538 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 51 | 335,570 | 84.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 301 | 332,336 | 84.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 101 | 352,361 | 85.80 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 101 | 350,754 | 85.80 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 51 | 373,832 | 73.84 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 51 | 389,105 | 73.84 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 51 | 135,355 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 51 | 528,541 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 51 | 360,360 | 84.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 201 | 354,233 | 84.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 453 | N/A | 202.99 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 504 | N/A | 202.99 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 352 | N/A | 182.39 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 403 | N/A | 182.39 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 353 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 404 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 352 | N/A | 256.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 755 | N/A | 256.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 403 | N/A | 202.99 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 403 | N/A | 202.99 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 355 | N/A | 179.46 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 353 | N/A | 179.46 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 352 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 402 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 352 | N/A | 248.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 704 | N/A | 248.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 101 | 152,439 | 202.99 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 101 | 320,718 | 202.99 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 50 | 315,956 | 182.39 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 51 | 300,933 | 182.39 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 50 | 501,756 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 50 | 571,429 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 50 | 326,904 | 256.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 301 | 290,867 | 256.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 101 | 340,832 | 202.99 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 101 | 348,432 | 202.99 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 50 | 328,192 | 179.46 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 50 | 326,264 | 179.46 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 51 | 383,289 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 51 | 480,538 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 51 | 318,269 | 248.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 352 | 290,444 | 248.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 301 | 123,686 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 251 | 118,596 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 51 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/clang_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 51 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/clang_unit_off.log) |
