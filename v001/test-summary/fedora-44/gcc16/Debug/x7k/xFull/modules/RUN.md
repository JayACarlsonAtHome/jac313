# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-20 18:25:12Z |
| OS | fedora-44 |
| Compiler | gcc16 |
| Build type | Debug |
| Disk | x7k |
| Size | full (xFull) |
| Group ID | 60 |
| Environment | jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 645,369 ms (10m 45s) |
| Build straight (no modules) | 20,067 ms (20,067 ms (20s)) |
| Build modules only | 5,470 ms (5,470 ms (5s)) |
| Build with modules (rest) | 22,569 ms (22,569 ms (22s)) |
| Build modules only (import std) | 15,806 ms (15,806 ms (15s)) |
| Build with modules (import std) | 24,473 ms (24,473 ms (24s)) |
| import std vs textual modules | +1,904 ms (-8% slower) |
| Peak ops/sec | 4,437,384 |
| Peak scenario | jac313_store_008_XS (flags/off) — [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc16_flags_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 656 | 2,674,369 | 8.38 MiB |
|  |  | jtext | 1,411 | 2,464,268 | 7.92 MiB |
|  |  | sql | 502 | 2,371,185 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 706 | 2,318,464 | 8.38 MiB |
|  |  | jtext | 1,456 | 2,804,892 | 7.92 MiB |
|  |  | sql | 956 | 2,467,552 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 704 | 3,051,013 | 8.38 MiB |
|  |  | jtext | 1,557 | 2,816,267 | 7.44 MiB |
|  |  | sql | 554 | 3,004,537 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 659 | 2,474,696 | 8.38 MiB |
|  |  | jtext | 1,455 | 2,822,228 | 7.44 MiB |
|  |  | sql | 1,106 | 2,448,100 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,284 | N/A | 19.81 MiB |
|  |  | jtext | 34,583 | N/A | 20.42 MiB |
|  |  | sql | 34,237 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 38,057 | N/A | 19.81 MiB |
|  |  | jtext | 38,354 | N/A | 20.42 MiB |
|  |  | sql | 38,411 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 33,628 | N/A | 19.81 MiB |
|  |  | jtext | 33,929 | N/A | 19.96 MiB |
|  |  | sql | 33,628 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 36,905 | N/A | 19.81 MiB |
|  |  | jtext | 37,306 | N/A | 19.96 MiB |
|  |  | sql | 37,356 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 703 | 2,318,410 | 19.82 MiB |
|  |  | jtext | 3,612 | 2,458,029 | 20.45 MiB |
|  |  | sql | 1,006 | 2,482,745 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 808 | 2,196,499 | 19.82 MiB |
|  |  | jtext | 4,121 | 2,377,386 | 20.45 MiB |
|  |  | sql | 1,858 | 2,122,151 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 703 | 2,858,204 | 19.82 MiB |
|  |  | jtext | 4,121 | 2,695,781 | 19.98 MiB |
|  |  | sql | 956 | 3,156,765 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 704 | 2,475,615 | 19.82 MiB |
|  |  | jtext | 3,864 | 2,705,335 | 19.98 MiB |
|  |  | sql | 1,760 | 2,599,969 | 22.66 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 203 | - | 104.05 KiB |
|  |  | jtext | 251 | - | 94.61 KiB |
|  |  | sql | 201 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 302 | - | 104.05 KiB |
|  |  | jtext | 251 | - | 94.61 KiB |
|  |  | sql | 402 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 201 | - | 104.05 KiB |
|  |  | jtext | 252 | - | 93.11 KiB |
|  |  | sql | 201 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 252 | - | 104.05 KiB |
|  |  | jtext | 251 | - | 93.11 KiB |
|  |  | sql | 352 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 151 | - | 156.05 KiB |
|  |  | jtext | 201 | - | 140.49 KiB |
|  |  | sql | 151 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 151 | - | 156.05 KiB |
|  |  | jtext | 201 | - | 140.49 KiB |
|  |  | sql | 301 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 151 | - | 156.05 KiB |
|  |  | jtext | 201 | - | 138.24 KiB |
|  |  | sql | 151 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 201 | - | 156.05 KiB |
|  |  | jtext | 201 | - | 138.24 KiB |
|  |  | sql | 302 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 553 | - | 311.93 KiB |
|  |  | jtext | 604 | - | 281.13 KiB |
|  |  | sql | 553 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 604 | - | 311.93 KiB |
|  |  | jtext | 654 | - | 281.18 KiB |
|  |  | sql | 804 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 553 | - | 311.93 KiB |
|  |  | jtext | 604 | - | 276.49 KiB |
|  |  | sql | 503 | - | 356.00 KiB |
| jac313_store_003_XS | on | binary | 604 | - | 311.93 KiB |
|  |  | jtext | 654 | - | 276.49 KiB |
|  |  | sql | 1,055 | - | 356.00 KiB |
| jac313_store_004_TS | off | binary | 603 | - | 519.79 KiB |
|  |  | jtext | 653 | - | 471.45 KiB |
|  |  | sql | 553 | - | 592.00 KiB |
| jac313_store_004_TS | on | binary | 654 | - | 519.79 KiB |
|  |  | jtext | 754 | - | 471.44 KiB |
|  |  | sql | 854 | - | 592.00 KiB |
| jac313_store_004_XS | off | binary | 553 | - | 519.79 KiB |
|  |  | jtext | 653 | - | 463.17 KiB |
|  |  | sql | 553 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 654 | - | 519.79 KiB |
|  |  | jtext | 704 | - | 463.17 KiB |
|  |  | sql | 804 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 656 | 2,674,369 | 8.38 MiB |
|  |  | jtext | 1,411 | 2,464,268 | 7.92 MiB |
|  |  | sql | 502 | 2,371,185 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 706 | 2,318,464 | 8.38 MiB |
|  |  | jtext | 1,456 | 2,804,892 | 7.92 MiB |
|  |  | sql | 956 | 2,467,552 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 704 | 3,051,013 | 8.38 MiB |
|  |  | jtext | 1,557 | 2,816,267 | 7.44 MiB |
|  |  | sql | 554 | 3,004,537 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 659 | 2,474,696 | 8.38 MiB |
|  |  | jtext | 1,455 | 2,822,228 | 7.44 MiB |
|  |  | sql | 1,106 | 2,448,100 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,284 | N/A | 19.81 MiB |
|  |  | jtext | 34,583 | N/A | 20.42 MiB |
|  |  | sql | 34,237 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 38,057 | N/A | 19.81 MiB |
|  |  | jtext | 38,354 | N/A | 20.42 MiB |
|  |  | sql | 38,411 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 33,628 | N/A | 19.81 MiB |
|  |  | jtext | 33,929 | N/A | 19.96 MiB |
|  |  | sql | 33,628 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 36,905 | N/A | 19.81 MiB |
|  |  | jtext | 37,306 | N/A | 19.96 MiB |
|  |  | sql | 37,356 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 703 | 2,318,410 | 19.82 MiB |
|  |  | jtext | 3,612 | 2,458,029 | 20.45 MiB |
|  |  | sql | 1,006 | 2,482,745 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 808 | 2,196,499 | 19.82 MiB |
|  |  | jtext | 4,121 | 2,377,386 | 20.45 MiB |
|  |  | sql | 1,858 | 2,122,151 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 703 | 2,858,204 | 19.82 MiB |
|  |  | jtext | 4,121 | 2,695,781 | 19.98 MiB |
|  |  | sql | 956 | 3,156,765 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 704 | 2,475,615 | 19.82 MiB |
|  |  | jtext | 3,864 | 2,705,335 | 19.98 MiB |
|  |  | sql | 1,760 | 2,599,969 | 22.66 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 203 | - | 104.05 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 302 | - | 104.05 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 251 | - | 94.61 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 251 | - | 94.61 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 201 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 201 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 201 | - | 152.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 402 | - | 152.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 201 | - | 104.05 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 252 | - | 104.05 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 252 | - | 93.11 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 251 | - | 93.11 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 201 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 201 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 201 | - | 148.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 352 | - | 148.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 151 | - | 156.05 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 151 | - | 156.05 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 201 | - | 140.49 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 201 | - | 140.49 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 151 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 151 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 151 | - | 200.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 301 | - | 200.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 151 | - | 156.05 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 201 | - | 156.05 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 201 | - | 138.24 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 201 | - | 138.24 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 151 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 151 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 151 | - | 192.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 302 | - | 192.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 553 | - | 311.93 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 604 | - | 311.93 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 604 | - | 281.13 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 654 | - | 281.18 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 554 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 603 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 553 | - | 368.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 804 | - | 368.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 553 | - | 311.93 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 604 | - | 311.93 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 604 | - | 276.49 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 654 | - | 276.49 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 503 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 553 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 503 | - | 356.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 1,055 | - | 356.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 603 | - | 519.79 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 654 | - | 519.79 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 653 | - | 471.45 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 754 | - | 471.44 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 553 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 603 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 553 | - | 592.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 854 | - | 592.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 553 | - | 519.79 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 654 | - | 519.79 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 653 | - | 463.17 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 704 | - | 463.17 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 503 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 603 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 553 | - | 572.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 804 | - | 572.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 656 | 2,674,369 | 8.38 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 706 | 2,318,464 | 8.38 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 1,411 | 2,464,268 | 7.92 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 1,456 | 2,804,892 | 7.92 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 267 | 2,712,306 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 253 | 2,469,502 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 502 | 2,371,185 | 7.42 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 956 | 2,467,552 | 7.42 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 704 | 3,051,013 | 8.38 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 659 | 2,474,696 | 8.38 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 1,557 | 2,816,267 | 7.44 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 1,455 | 2,822,228 | 7.44 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 263 | 2,555,976 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 210 | 2,933,412 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 554 | 3,004,537 | 7.13 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 1,106 | 2,448,100 | 7.13 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 34,284 | N/A | 19.81 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 38,057 | N/A | 19.81 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 34,583 | N/A | 20.42 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 38,354 | N/A | 20.42 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 33,739 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 37,562 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 34,237 | N/A | 23.50 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 38,411 | N/A | 23.50 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 33,628 | N/A | 19.81 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 36,905 | N/A | 19.81 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 33,929 | N/A | 19.96 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 37,306 | N/A | 19.96 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 33,181 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 36,647 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 33,628 | N/A | 22.64 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 37,356 | N/A | 22.64 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 703 | 2,318,410 | 19.82 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 808 | 2,196,499 | 19.82 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 3,612 | 2,458,029 | 20.45 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 4,121 | 2,377,386 | 20.45 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 252 | 2,767,170 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 258 | 2,436,766 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 1,006 | 2,482,745 | 23.51 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 1,858 | 2,122,151 | 23.51 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 703 | 2,858,204 | 19.82 MiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 704 | 2,475,615 | 19.82 MiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 4,121 | 2,695,781 | 19.98 MiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 3,864 | 2,705,335 | 19.98 MiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 253 | 2,962,963 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 259 | 2,749,292 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 956 | 3,156,765 | 22.66 MiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 1,760 | 2,599,969 | 22.66 MiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 4,230 | 3,889,401 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc16_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 4,026 | 4,437,384 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc16_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc16_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 201 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc16_unit_off.log) |
