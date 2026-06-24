# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-21 00:55:21Z |
| OS | linuxmint-22.3 |
| Compiler | gcc15 |
| Build type | Release |
| Disk | x7k |
| Size | full (xFull) |
| Group ID | 82 |
| Environment | jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 618,446 ms (10m 18s) |
| Peak ops/sec | 3,143,962 |
| Peak scenario | jac313_store_005_XS (none/off) — [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 460 | 2,532,928 | 8.38 MiB |
|  |  | jtext | 465 | 1,943,295 | 7.92 MiB |
|  |  | sql | 415 | 2,533,506 | 7.41 MiB |
| jac313_store_005_TS | on | binary | 512 | 2,206,434 | 8.38 MiB |
|  |  | jtext | 462 | 2,113,003 | 7.92 MiB |
|  |  | sql | 919 | 1,946,017 | 7.41 MiB |
| jac313_store_005_XS | off | binary | 461 | 2,426,654 | 8.38 MiB |
|  |  | jtext | 458 | 1,969,939 | 7.44 MiB |
|  |  | sql | 411 | 2,475,676 | 7.12 MiB |
| jac313_store_005_XS | on | binary | 509 | 2,562,657 | 8.38 MiB |
|  |  | jtext | 416 | 1,596,857 | 7.44 MiB |
|  |  | sql | 870 | 2,702,995 | 7.12 MiB |
| jac313_store_006_TS | off | binary | 33,924 | N/A | 19.81 MiB |
|  |  | jtext | 34,021 | N/A | 20.38 MiB |
|  |  | sql | 33,900 | N/A | 23.47 MiB |
| jac313_store_006_TS | on | binary | 37,753 | N/A | 19.81 MiB |
|  |  | jtext | 37,848 | N/A | 20.38 MiB |
|  |  | sql | 38,164 | N/A | 23.47 MiB |
| jac313_store_006_XS | off | binary | 33,226 | N/A | 19.81 MiB |
|  |  | jtext | 33,377 | N/A | 19.96 MiB |
|  |  | sql | 33,366 | N/A | 22.62 MiB |
| jac313_store_006_XS | on | binary | 36,874 | N/A | 19.81 MiB |
|  |  | jtext | 36,834 | N/A | 19.96 MiB |
|  |  | sql | 37,267 | N/A | 22.62 MiB |
| jac313_store_007_TS | off | binary | 557 | 2,205,655 | 19.82 MiB |
|  |  | jtext | 821 | 2,214,251 | 20.45 MiB |
|  |  | sql | 709 | 2,225,734 | 23.49 MiB |
| jac313_store_007_TS | on | binary | 615 | 1,989,139 | 19.82 MiB |
|  |  | jtext | 869 | 2,487,438 | 20.45 MiB |
|  |  | sql | 1,777 | 2,676,230 | 23.49 MiB |
| jac313_store_007_XS | off | binary | 577 | 1,670,844 | 19.82 MiB |
|  |  | jtext | 879 | 2,972,917 | 19.98 MiB |
|  |  | sql | 805 | 2,184,121 | 22.61 MiB |
| jac313_store_007_XS | on | binary | 560 | 2,644,803 | 19.82 MiB |
|  |  | jtext | 812 | 2,204,488 | 19.98 MiB |
|  |  | sql | 1,720 | 3,006,615 | 22.61 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 202 | - | 104.06 KiB |
|  |  | jtext | 204 | - | 94.70 KiB |
|  |  | sql | 202 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 252 | - | 104.06 KiB |
|  |  | jtext | 202 | - | 94.70 KiB |
|  |  | sql | 402 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 201 | - | 104.06 KiB |
|  |  | jtext | 201 | - | 93.20 KiB |
|  |  | sql | 152 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 252 | - | 104.06 KiB |
|  |  | jtext | 201 | - | 93.20 KiB |
|  |  | sql | 452 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 151 | - | 156.06 KiB |
|  |  | jtext | 151 | - | 140.58 KiB |
|  |  | sql | 151 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 201 | - | 156.06 KiB |
|  |  | jtext | 152 | - | 140.58 KiB |
|  |  | sql | 402 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 202 | - | 156.06 KiB |
|  |  | jtext | 151 | - | 138.33 KiB |
|  |  | sql | 151 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 202 | - | 156.06 KiB |
|  |  | jtext | 156 | - | 138.33 KiB |
|  |  | sql | 453 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 554 | - | 311.94 KiB |
|  |  | jtext | 503 | - | 281.81 KiB |
|  |  | sql | 554 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 656 | - | 311.94 KiB |
|  |  | jtext | 605 | - | 281.08 KiB |
|  |  | sql | 855 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 554 | - | 311.94 KiB |
|  |  | jtext | 506 | - | 276.58 KiB |
|  |  | sql | 505 | - | 356.00 KiB |
| jac313_store_003_XS | on | binary | 603 | - | 311.94 KiB |
|  |  | jtext | 554 | - | 276.58 KiB |
|  |  | sql | 806 | - | 356.00 KiB |
| jac313_store_004_TS | off | binary | 554 | - | 519.80 KiB |
|  |  | jtext | 503 | - | 470.89 KiB |
|  |  | sql | 504 | - | 596.00 KiB |
| jac313_store_004_TS | on | binary | 605 | - | 519.80 KiB |
|  |  | jtext | 556 | - | 470.76 KiB |
|  |  | sql | 855 | - | 596.00 KiB |
| jac313_store_004_XS | off | binary | 553 | - | 519.80 KiB |
|  |  | jtext | 503 | - | 463.26 KiB |
|  |  | sql | 504 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 657 | - | 519.80 KiB |
|  |  | jtext | 555 | - | 463.26 KiB |
|  |  | sql | 805 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 460 | 2,532,928 | 8.38 MiB |
|  |  | jtext | 465 | 1,943,295 | 7.92 MiB |
|  |  | sql | 415 | 2,533,506 | 7.41 MiB |
| jac313_store_005_TS | on | binary | 512 | 2,206,434 | 8.38 MiB |
|  |  | jtext | 462 | 2,113,003 | 7.92 MiB |
|  |  | sql | 919 | 1,946,017 | 7.41 MiB |
| jac313_store_005_XS | off | binary | 461 | 2,426,654 | 8.38 MiB |
|  |  | jtext | 458 | 1,969,939 | 7.44 MiB |
|  |  | sql | 411 | 2,475,676 | 7.12 MiB |
| jac313_store_005_XS | on | binary | 509 | 2,562,657 | 8.38 MiB |
|  |  | jtext | 416 | 1,596,857 | 7.44 MiB |
|  |  | sql | 870 | 2,702,995 | 7.12 MiB |
| jac313_store_006_TS | off | binary | 33,924 | N/A | 19.81 MiB |
|  |  | jtext | 34,021 | N/A | 20.38 MiB |
|  |  | sql | 33,900 | N/A | 23.47 MiB |
| jac313_store_006_TS | on | binary | 37,753 | N/A | 19.81 MiB |
|  |  | jtext | 37,848 | N/A | 20.38 MiB |
|  |  | sql | 38,164 | N/A | 23.47 MiB |
| jac313_store_006_XS | off | binary | 33,226 | N/A | 19.81 MiB |
|  |  | jtext | 33,377 | N/A | 19.96 MiB |
|  |  | sql | 33,366 | N/A | 22.62 MiB |
| jac313_store_006_XS | on | binary | 36,874 | N/A | 19.81 MiB |
|  |  | jtext | 36,834 | N/A | 19.96 MiB |
|  |  | sql | 37,267 | N/A | 22.62 MiB |
| jac313_store_007_TS | off | binary | 557 | 2,205,655 | 19.82 MiB |
|  |  | jtext | 821 | 2,214,251 | 20.45 MiB |
|  |  | sql | 709 | 2,225,734 | 23.49 MiB |
| jac313_store_007_TS | on | binary | 615 | 1,989,139 | 19.82 MiB |
|  |  | jtext | 869 | 2,487,438 | 20.45 MiB |
|  |  | sql | 1,777 | 2,676,230 | 23.49 MiB |
| jac313_store_007_XS | off | binary | 577 | 1,670,844 | 19.82 MiB |
|  |  | jtext | 879 | 2,972,917 | 19.98 MiB |
|  |  | sql | 805 | 2,184,121 | 22.61 MiB |
| jac313_store_007_XS | on | binary | 560 | 2,644,803 | 19.82 MiB |
|  |  | jtext | 812 | 2,204,488 | 19.98 MiB |
|  |  | sql | 1,720 | 3,006,615 | 22.61 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 202 | - | 104.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc15_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 252 | - | 104.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc15_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 204 | - | 94.70 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc15_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 202 | - | 94.70 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc15_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 202 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc15_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 202 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc15_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 202 | - | 152.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc15_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 402 | - | 152.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc15_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 201 | - | 104.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc15_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 252 | - | 104.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc15_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 201 | - | 93.20 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc15_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 201 | - | 93.20 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc15_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 201 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc15_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 202 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc15_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 152 | - | 148.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc15_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 452 | - | 148.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc15_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 151 | - | 156.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc15_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 201 | - | 156.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc15_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 151 | - | 140.58 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc15_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 152 | - | 140.58 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc15_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 151 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc15_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 151 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc15_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 151 | - | 200.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc15_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 402 | - | 200.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc15_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 202 | - | 156.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc15_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 202 | - | 156.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc15_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 151 | - | 138.33 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc15_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 156 | - | 138.33 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc15_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 152 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc15_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 151 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc15_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 151 | - | 192.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc15_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 453 | - | 192.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc15_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 554 | - | 311.94 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc15_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 656 | - | 311.94 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc15_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 503 | - | 281.81 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc15_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 605 | - | 281.08 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc15_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 504 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc15_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 604 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc15_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 554 | - | 368.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc15_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 855 | - | 368.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc15_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 554 | - | 311.94 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc15_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 603 | - | 311.94 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc15_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 506 | - | 276.58 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc15_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 554 | - | 276.58 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc15_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 503 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc15_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 606 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc15_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 505 | - | 356.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc15_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 806 | - | 356.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc15_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 554 | - | 519.80 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc15_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 605 | - | 519.80 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc15_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 503 | - | 470.89 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc15_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 556 | - | 470.76 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc15_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 504 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc15_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 555 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc15_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 504 | - | 596.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc15_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 855 | - | 596.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc15_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 553 | - | 519.80 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc15_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 657 | - | 519.80 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc15_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 503 | - | 463.26 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc15_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 555 | - | 463.26 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc15_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 504 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc15_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 555 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc15_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 504 | - | 572.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc15_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 805 | - | 572.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc15_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 460 | 2,532,928 | 8.38 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 512 | 2,206,434 | 8.38 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 465 | 1,943,295 | 7.92 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 462 | 2,113,003 | 7.92 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 278 | 2,488,119 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 285 | 2,516,990 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 415 | 2,533,506 | 7.41 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 919 | 1,946,017 | 7.41 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 461 | 2,426,654 | 8.38 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 509 | 2,562,657 | 8.38 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 458 | 1,969,939 | 7.44 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 416 | 1,596,857 | 7.44 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 256 | 3,143,962 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 257 | 2,561,344 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 411 | 2,475,676 | 7.12 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc15_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 870 | 2,702,995 | 7.12 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc15_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 33,924 | N/A | 19.81 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc15_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 37,753 | N/A | 19.81 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc15_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 34,021 | N/A | 20.38 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc15_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 37,848 | N/A | 20.38 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc15_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 33,742 | N/A | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc15_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 37,693 | N/A | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc15_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 33,900 | N/A | 23.47 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc15_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 38,164 | N/A | 23.47 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc15_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 33,226 | N/A | 19.81 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc15_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 36,874 | N/A | 19.81 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc15_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 33,377 | N/A | 19.96 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc15_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 36,834 | N/A | 19.96 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc15_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 33,166 | N/A | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc15_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 36,582 | N/A | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc15_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 33,366 | N/A | 22.62 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc15_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 37,267 | N/A | 22.62 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc15_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 557 | 2,205,655 | 19.82 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 615 | 1,989,139 | 19.82 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 821 | 2,214,251 | 20.45 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 869 | 2,487,438 | 20.45 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 271 | 2,547,511 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 265 | 2,965,951 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 709 | 2,225,734 | 23.49 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 1,777 | 2,676,230 | 23.49 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 577 | 1,670,844 | 19.82 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 560 | 2,644,803 | 19.82 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 879 | 2,972,917 | 19.98 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 812 | 2,204,488 | 19.98 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 262 | 2,648,866 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 253 | 2,874,059 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 805 | 2,184,121 | 22.61 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 1,720 | 3,006,615 | 22.61 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 3,102 | 2,796,773 | - | - | [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 2,717 | 2,913,286 | - | - | [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 51 | - | - | - | [gcc15_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc15_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 51 | - | - | - | [gcc15_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc15_unit_off.log) |
