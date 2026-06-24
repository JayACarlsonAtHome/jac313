# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-20 22:52:36Z |
| OS | almalinux-10.2 |
| Compiler | clang |
| Build type | Release |
| Disk | x7k |
| Size | smoke (Smoke) |
| Group ID | 86 |
| Environment | jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 19,746 ms (19s) |
| Peak ops/sec | 650,618 |
| Peak scenario | jac313_store_007_XS (none/on) — [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_on.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 104 | 320,307 | 85.80 KiB |
|  |  | jtext | 54 | 319,489 | 76.77 KiB |
|  |  | sql | 50 | 319,795 | 88.00 KiB |
| jac313_store_005_TS | on | binary | 103 | 308,356 | 85.80 KiB |
|  |  | jtext | 52 | 316,156 | 76.77 KiB |
|  |  | sql | 104 | 319,591 | 88.00 KiB |
| jac313_store_005_XS | off | binary | 104 | 329,272 | 85.80 KiB |
|  |  | jtext | 54 | 347,464 | 73.84 KiB |
|  |  | sql | 54 | 331,455 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 107 | 327,225 | 85.80 KiB |
|  |  | jtext | 53 | 308,642 | 73.84 KiB |
|  |  | sql | 158 | 352,858 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 373 | N/A | 202.99 KiB |
|  |  | jtext | 375 | N/A | 182.39 KiB |
|  |  | sql | 371 | N/A | 256.00 KiB |
| jac313_store_006_TS | on | binary | 427 | N/A | 202.99 KiB |
|  |  | jtext | 434 | N/A | 182.39 KiB |
|  |  | sql | 587 | N/A | 256.00 KiB |
| jac313_store_006_XS | off | binary | 380 | N/A | 202.99 KiB |
|  |  | jtext | 370 | N/A | 179.46 KiB |
|  |  | sql | 372 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 426 | N/A | 202.99 KiB |
|  |  | jtext | 379 | N/A | 179.46 KiB |
|  |  | sql | 537 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 52 | 294,551 | 202.99 KiB |
|  |  | jtext | 52 | 307,031 | 182.39 KiB |
|  |  | sql | 53 | 283,206 | 260.00 KiB |
| jac313_store_007_TS | on | binary | 51 | 307,314 | 202.99 KiB |
|  |  | jtext | 53 | 323,102 | 182.39 KiB |
|  |  | sql | 266 | 291,290 | 260.00 KiB |
| jac313_store_007_XS | off | binary | 106 | 273,673 | 202.99 KiB |
|  |  | jtext | 52 | 285,470 | 179.46 KiB |
|  |  | sql | 53 | 284,091 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 103 | 311,526 | 202.99 KiB |
|  |  | jtext | 52 | 301,568 | 179.46 KiB |
|  |  | sql | 209 | 279,720 | 248.00 KiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.68 KiB |
|  |  | sql | 54 | - | 52.00 KiB |
| jac313_store_001_TS | on | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.68 KiB |
|  |  | sql | 214 | - | 52.00 KiB |
| jac313_store_001_XS | off | binary | 53 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.39 KiB |
|  |  | sql | 53 | - | 52.00 KiB |
| jac313_store_001_XS | on | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.39 KiB |
|  |  | sql | 212 | - | 52.00 KiB |
| jac313_store_002_TS | off | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 52 | - | 20.68 KiB |
|  |  | sql | 54 | - | 52.00 KiB |
| jac313_store_002_TS | on | binary | 53 | - | 20.40 KiB |
|  |  | jtext | 51 | - | 20.68 KiB |
|  |  | sql | 160 | - | 52.00 KiB |
| jac313_store_002_XS | off | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 53 | - | 20.39 KiB |
|  |  | sql | 54 | - | 52.00 KiB |
| jac313_store_002_XS | on | binary | 51 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.39 KiB |
|  |  | sql | 210 | - | 52.00 KiB |
| jac313_store_003_TS | off | binary | 372 | - | 202.99 KiB |
|  |  | jtext | 370 | - | 182.39 KiB |
|  |  | sql | 372 | - | 256.00 KiB |
| jac313_store_003_TS | on | binary | 374 | - | 202.99 KiB |
|  |  | jtext | 431 | - | 182.39 KiB |
|  |  | sql | 531 | - | 256.00 KiB |
| jac313_store_003_XS | off | binary | 426 | - | 202.99 KiB |
|  |  | jtext | 377 | - | 179.46 KiB |
|  |  | sql | 377 | - | 248.00 KiB |
| jac313_store_003_XS | on | binary | 378 | - | 202.99 KiB |
|  |  | jtext | 375 | - | 179.46 KiB |
|  |  | sql | 588 | - | 248.00 KiB |
| jac313_store_004_TS | off | binary | 51 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.68 KiB |
|  |  | sql | 53 | - | 52.00 KiB |
| jac313_store_004_TS | on | binary | 107 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.68 KiB |
|  |  | sql | 213 | - | 52.00 KiB |
| jac313_store_004_XS | off | binary | 104 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.39 KiB |
|  |  | sql | 54 | - | 52.00 KiB |
| jac313_store_004_XS | on | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.39 KiB |
|  |  | sql | 213 | - | 52.00 KiB |
| jac313_store_005_TS | off | binary | 104 | 320,307 | 85.80 KiB |
|  |  | jtext | 54 | 319,489 | 76.77 KiB |
|  |  | sql | 50 | 319,795 | 88.00 KiB |
| jac313_store_005_TS | on | binary | 103 | 308,356 | 85.80 KiB |
|  |  | jtext | 52 | 316,156 | 76.77 KiB |
|  |  | sql | 104 | 319,591 | 88.00 KiB |
| jac313_store_005_XS | off | binary | 104 | 329,272 | 85.80 KiB |
|  |  | jtext | 54 | 347,464 | 73.84 KiB |
|  |  | sql | 54 | 331,455 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 107 | 327,225 | 85.80 KiB |
|  |  | jtext | 53 | 308,642 | 73.84 KiB |
|  |  | sql | 158 | 352,858 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 373 | N/A | 202.99 KiB |
|  |  | jtext | 375 | N/A | 182.39 KiB |
|  |  | sql | 371 | N/A | 256.00 KiB |
| jac313_store_006_TS | on | binary | 427 | N/A | 202.99 KiB |
|  |  | jtext | 434 | N/A | 182.39 KiB |
|  |  | sql | 587 | N/A | 256.00 KiB |
| jac313_store_006_XS | off | binary | 380 | N/A | 202.99 KiB |
|  |  | jtext | 370 | N/A | 179.46 KiB |
|  |  | sql | 372 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 426 | N/A | 202.99 KiB |
|  |  | jtext | 379 | N/A | 179.46 KiB |
|  |  | sql | 537 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 52 | 294,551 | 202.99 KiB |
|  |  | jtext | 52 | 307,031 | 182.39 KiB |
|  |  | sql | 53 | 283,206 | 260.00 KiB |
| jac313_store_007_TS | on | binary | 51 | 307,314 | 202.99 KiB |
|  |  | jtext | 53 | 323,102 | 182.39 KiB |
|  |  | sql | 266 | 291,290 | 260.00 KiB |
| jac313_store_007_XS | off | binary | 106 | 273,673 | 202.99 KiB |
|  |  | jtext | 52 | 285,470 | 179.46 KiB |
|  |  | sql | 53 | 284,091 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 103 | 311,526 | 202.99 KiB |
|  |  | jtext | 52 | 301,568 | 179.46 KiB |
|  |  | sql | 209 | 279,720 | 248.00 KiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 54 | - | 20.40 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 54 | - | 20.40 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 54 | - | 20.68 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 54 | - | 20.68 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 54 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 54 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 54 | - | 52.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 214 | - | 52.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 53 | - | 20.40 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 54 | - | 20.40 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 54 | - | 20.39 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 54 | - | 20.39 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 54 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 54 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 53 | - | 52.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 212 | - | 52.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 54 | - | 20.40 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 53 | - | 20.40 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 52 | - | 20.68 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 51 | - | 20.68 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 54 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 53 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 54 | - | 52.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 160 | - | 52.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 54 | - | 20.40 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 51 | - | 20.40 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 53 | - | 20.39 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 54 | - | 20.39 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 54 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 51 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 54 | - | 52.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 210 | - | 52.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 372 | - | 202.99 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 374 | - | 202.99 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 370 | - | 182.39 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 431 | - | 182.39 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 373 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 432 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 372 | - | 256.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 531 | - | 256.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 426 | - | 202.99 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 378 | - | 202.99 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 377 | - | 179.46 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 375 | - | 179.46 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 380 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 377 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 377 | - | 248.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 588 | - | 248.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 51 | - | 20.40 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 107 | - | 20.40 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 54 | - | 20.68 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 54 | - | 20.68 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 52 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 52 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 53 | - | 52.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 213 | - | 52.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 104 | - | 20.40 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 54 | - | 20.40 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 54 | - | 20.39 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 54 | - | 20.39 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 54 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 54 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 54 | - | 52.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 213 | - | 52.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 104 | 320,307 | 85.80 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 103 | 308,356 | 85.80 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 54 | 319,489 | 76.77 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 52 | 316,156 | 76.77 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 54 | 536,193 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 53 | 568,505 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 50 | 319,795 | 88.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 104 | 319,591 | 88.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 104 | 329,272 | 85.80 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 107 | 327,225 | 85.80 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 54 | 347,464 | 73.84 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 53 | 308,642 | 73.84 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 54 | 626,174 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 54 | 573,723 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 54 | 331,455 | 84.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 158 | 352,858 | 84.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 373 | N/A | 202.99 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 427 | N/A | 202.99 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 375 | N/A | 182.39 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 434 | N/A | 182.39 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 380 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 428 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 371 | N/A | 256.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 587 | N/A | 256.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 380 | N/A | 202.99 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 426 | N/A | 202.99 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 370 | N/A | 179.46 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 379 | N/A | 179.46 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 376 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 378 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 372 | N/A | 248.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 537 | N/A | 248.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 52 | 294,551 | 202.99 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 51 | 307,314 | 202.99 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 52 | 307,031 | 182.39 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 53 | 323,102 | 182.39 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 51 | 575,705 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 54 | 459,982 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 53 | 283,206 | 260.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 266 | 291,290 | 260.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 106 | 273,673 | 202.99 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 103 | 311,526 | 202.99 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 52 | 285,470 | 179.46 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 52 | 301,568 | 179.46 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 54 | 573,394 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 54 | 650,618 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 53 | 284,091 | 248.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 209 | 279,720 | 248.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 211 | 297,442 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 206 | 318,573 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 53 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/clang_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 54 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/clang_unit_off.log) |
