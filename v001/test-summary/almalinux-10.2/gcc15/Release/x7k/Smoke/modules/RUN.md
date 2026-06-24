# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-20 22:56:56Z |
| OS | almalinux-10.2 |
| Compiler | gcc15 |
| Build type | Release |
| Disk | x7k |
| Size | smoke (Smoke) |
| Group ID | 89 |
| Environment | jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 20,105 ms (20s) |
| Build straight (no modules) | 42,830 ms (42,830 ms (42s)) |
| Build modules only | 4,977 ms (4,977 ms (4s)) |
| Build with modules (rest) | 43,040 ms (43,040 ms (43s)) |
| Peak ops/sec | 667,111 |
| Peak scenario | jac313_store_007_XS (none/off) — [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 53 | 316,256 | 85.80 KiB |
|  |  | jtext | 53 | 302,755 | 76.77 KiB |
|  |  | sql | 53 | 320,307 | 84.00 KiB |
| jac313_store_005_TS | on | binary | 53 | 341,647 | 85.80 KiB |
|  |  | jtext | 53 | 312,305 | 76.77 KiB |
|  |  | sql | 107 | 319,693 | 84.00 KiB |
| jac313_store_005_XS | off | binary | 53 | 334,336 | 85.80 KiB |
|  |  | jtext | 54 | 335,121 | 73.84 KiB |
|  |  | sql | 54 | 348,918 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 105 | 304,599 | 85.80 KiB |
|  |  | jtext | 54 | 352,858 | 73.84 KiB |
|  |  | sql | 104 | 342,114 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 379 | N/A | 202.99 KiB |
|  |  | jtext | 371 | N/A | 182.39 KiB |
|  |  | sql | 379 | N/A | 256.00 KiB |
| jac313_store_006_TS | on | binary | 431 | N/A | 202.99 KiB |
|  |  | jtext | 435 | N/A | 182.39 KiB |
|  |  | sql | 696 | N/A | 256.00 KiB |
| jac313_store_006_XS | off | binary | 373 | N/A | 202.99 KiB |
|  |  | jtext | 380 | N/A | 179.46 KiB |
|  |  | sql | 379 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 431 | N/A | 202.99 KiB |
|  |  | jtext | 431 | N/A | 179.46 KiB |
|  |  | sql | 540 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 53 | 307,692 | 202.99 KiB |
|  |  | jtext | 50 | 314,762 | 182.39 KiB |
|  |  | sql | 52 | 308,452 | 256.00 KiB |
| jac313_store_007_TS | on | binary | 52 | 290,023 | 202.99 KiB |
|  |  | jtext | 52 | 313,972 | 182.39 KiB |
|  |  | sql | 211 | 337,154 | 256.00 KiB |
| jac313_store_007_XS | off | binary | 52 | 312,207 | 202.99 KiB |
|  |  | jtext | 53 | 299,312 | 179.46 KiB |
|  |  | sql | 52 | 331,236 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 50 | 336,587 | 202.99 KiB |
|  |  | jtext | 52 | 320,616 | 179.46 KiB |
|  |  | sql | 209 | 336,474 | 248.00 KiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 51 | - | 20.68 KiB |
|  |  | sql | 54 | - | 52.00 KiB |
| jac313_store_001_TS | on | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.68 KiB |
|  |  | sql | 210 | - | 52.00 KiB |
| jac313_store_001_XS | off | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 52 | - | 20.39 KiB |
|  |  | sql | 54 | - | 52.00 KiB |
| jac313_store_001_XS | on | binary | 107 | - | 20.40 KiB |
|  |  | jtext | 52 | - | 20.39 KiB |
|  |  | sql | 270 | - | 52.00 KiB |
| jac313_store_002_TS | off | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.68 KiB |
|  |  | sql | 53 | - | 52.00 KiB |
| jac313_store_002_TS | on | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.68 KiB |
|  |  | sql | 212 | - | 52.00 KiB |
| jac313_store_002_XS | off | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.39 KiB |
|  |  | sql | 54 | - | 52.00 KiB |
| jac313_store_002_XS | on | binary | 52 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.39 KiB |
|  |  | sql | 266 | - | 52.00 KiB |
| jac313_store_003_TS | off | binary | 416 | - | 202.99 KiB |
|  |  | jtext | 379 | - | 182.39 KiB |
|  |  | sql | 379 | - | 256.00 KiB |
| jac313_store_003_TS | on | binary | 426 | - | 202.99 KiB |
|  |  | jtext | 430 | - | 182.39 KiB |
|  |  | sql | 804 | - | 256.00 KiB |
| jac313_store_003_XS | off | binary | 379 | - | 202.99 KiB |
|  |  | jtext | 377 | - | 179.46 KiB |
|  |  | sql | 375 | - | 248.00 KiB |
| jac313_store_003_XS | on | binary | 376 | - | 202.99 KiB |
|  |  | jtext | 379 | - | 179.46 KiB |
|  |  | sql | 537 | - | 248.00 KiB |
| jac313_store_004_TS | off | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.68 KiB |
|  |  | sql | 54 | - | 52.00 KiB |
| jac313_store_004_TS | on | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 54 | - | 20.68 KiB |
|  |  | sql | 266 | - | 52.00 KiB |
| jac313_store_004_XS | off | binary | 54 | - | 20.40 KiB |
|  |  | jtext | 53 | - | 20.39 KiB |
|  |  | sql | 54 | - | 52.00 KiB |
| jac313_store_004_XS | on | binary | 53 | - | 20.40 KiB |
|  |  | jtext | 50 | - | 20.39 KiB |
|  |  | sql | 216 | - | 52.00 KiB |
| jac313_store_005_TS | off | binary | 53 | 316,256 | 85.80 KiB |
|  |  | jtext | 53 | 302,755 | 76.77 KiB |
|  |  | sql | 53 | 320,307 | 84.00 KiB |
| jac313_store_005_TS | on | binary | 53 | 341,647 | 85.80 KiB |
|  |  | jtext | 53 | 312,305 | 76.77 KiB |
|  |  | sql | 107 | 319,693 | 84.00 KiB |
| jac313_store_005_XS | off | binary | 53 | 334,336 | 85.80 KiB |
|  |  | jtext | 54 | 335,121 | 73.84 KiB |
|  |  | sql | 54 | 348,918 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 105 | 304,599 | 85.80 KiB |
|  |  | jtext | 54 | 352,858 | 73.84 KiB |
|  |  | sql | 104 | 342,114 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 379 | N/A | 202.99 KiB |
|  |  | jtext | 371 | N/A | 182.39 KiB |
|  |  | sql | 379 | N/A | 256.00 KiB |
| jac313_store_006_TS | on | binary | 431 | N/A | 202.99 KiB |
|  |  | jtext | 435 | N/A | 182.39 KiB |
|  |  | sql | 696 | N/A | 256.00 KiB |
| jac313_store_006_XS | off | binary | 373 | N/A | 202.99 KiB |
|  |  | jtext | 380 | N/A | 179.46 KiB |
|  |  | sql | 379 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 431 | N/A | 202.99 KiB |
|  |  | jtext | 431 | N/A | 179.46 KiB |
|  |  | sql | 540 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 53 | 307,692 | 202.99 KiB |
|  |  | jtext | 50 | 314,762 | 182.39 KiB |
|  |  | sql | 52 | 308,452 | 256.00 KiB |
| jac313_store_007_TS | on | binary | 52 | 290,023 | 202.99 KiB |
|  |  | jtext | 52 | 313,972 | 182.39 KiB |
|  |  | sql | 211 | 337,154 | 256.00 KiB |
| jac313_store_007_XS | off | binary | 52 | 312,207 | 202.99 KiB |
|  |  | jtext | 53 | 299,312 | 179.46 KiB |
|  |  | sql | 52 | 331,236 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 50 | 336,587 | 202.99 KiB |
|  |  | jtext | 52 | 320,616 | 179.46 KiB |
|  |  | sql | 209 | 336,474 | 248.00 KiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 54 | - | 20.40 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc15_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 54 | - | 20.40 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc15_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 51 | - | 20.68 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc15_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 54 | - | 20.68 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc15_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc15_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 54 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc15_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 54 | - | 52.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc15_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 210 | - | 52.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc15_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 54 | - | 20.40 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc15_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 107 | - | 20.40 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc15_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 52 | - | 20.39 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc15_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 52 | - | 20.39 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc15_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 54 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc15_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 52 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc15_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 54 | - | 52.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc15_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 270 | - | 52.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc15_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 54 | - | 20.40 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc15_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 54 | - | 20.40 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc15_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 54 | - | 20.68 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc15_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 54 | - | 20.68 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc15_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 54 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc15_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 54 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc15_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 53 | - | 52.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc15_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 212 | - | 52.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc15_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 54 | - | 20.40 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc15_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 52 | - | 20.40 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc15_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 54 | - | 20.39 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc15_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 54 | - | 20.39 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc15_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 54 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc15_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 51 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc15_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 54 | - | 52.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc15_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 266 | - | 52.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc15_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 416 | - | 202.99 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc15_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 426 | - | 202.99 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc15_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 379 | - | 182.39 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc15_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 430 | - | 182.39 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc15_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 376 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc15_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 433 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc15_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 379 | - | 256.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc15_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 804 | - | 256.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc15_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 379 | - | 202.99 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc15_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 376 | - | 202.99 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc15_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 377 | - | 179.46 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc15_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 379 | - | 179.46 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc15_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 379 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc15_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 435 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc15_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 375 | - | 248.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc15_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 537 | - | 248.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc15_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 54 | - | 20.40 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc15_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 54 | - | 20.40 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc15_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 54 | - | 20.68 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc15_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 54 | - | 20.68 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc15_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 54 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc15_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 54 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc15_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 54 | - | 52.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc15_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 266 | - | 52.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc15_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 54 | - | 20.40 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc15_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 53 | - | 20.40 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc15_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 53 | - | 20.39 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc15_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 50 | - | 20.39 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc15_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 52 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc15_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 54 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc15_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 54 | - | 52.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc15_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 216 | - | 52.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc15_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 53 | 316,256 | 85.80 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 53 | 341,647 | 85.80 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 53 | 302,755 | 76.77 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 53 | 312,305 | 76.77 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 54 | 537,924 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 54 | 497,018 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 53 | 320,307 | 84.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 107 | 319,693 | 84.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 53 | 334,336 | 85.80 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 105 | 304,599 | 85.80 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 54 | 335,121 | 73.84 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 54 | 352,858 | 73.84 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 54 | 278,164 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 54 | 604,230 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 54 | 348,918 | 84.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc15_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 104 | 342,114 | 84.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc15_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 379 | N/A | 202.99 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc15_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 431 | N/A | 202.99 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc15_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 371 | N/A | 182.39 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc15_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 435 | N/A | 182.39 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc15_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 376 | N/A | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc15_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 435 | N/A | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc15_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 379 | N/A | 256.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc15_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 696 | N/A | 256.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc15_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 373 | N/A | 202.99 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc15_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 431 | N/A | 202.99 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc15_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 380 | N/A | 179.46 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc15_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 431 | N/A | 179.46 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc15_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 381 | N/A | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc15_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 430 | N/A | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc15_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 379 | N/A | 248.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc15_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 540 | N/A | 248.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc15_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 53 | 307,692 | 202.99 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 52 | 290,023 | 202.99 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 50 | 314,762 | 182.39 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 52 | 313,972 | 182.39 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 53 | 661,376 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 54 | 584,112 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 52 | 308,452 | 256.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 211 | 337,154 | 256.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 52 | 312,207 | 202.99 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 50 | 336,587 | 202.99 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 53 | 299,312 | 179.46 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 52 | 320,616 | 179.46 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 54 | 667,111 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 54 | 600,240 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 52 | 331,236 | 248.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 209 | 336,474 | 248.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 211 | 319,285 | - | - | [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 160 | 290,951 | - | - | [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 54 | - | - | - | [gcc15_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc15_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 54 | - | - | - | [gcc15_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc15_unit_off.log) |
