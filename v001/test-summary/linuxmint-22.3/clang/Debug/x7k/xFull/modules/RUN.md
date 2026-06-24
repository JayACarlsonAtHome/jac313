# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-20 23:22:29Z |
| OS | linuxmint-22.3 |
| Compiler | clang |
| Build type | Debug |
| Disk | x7k |
| Size | full (xFull) |
| Group ID | 75 |
| Environment | jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 662,456 ms (11m 2s) |
| Build straight (no modules) | 69,171 ms (69,171 ms (1m 9s)) |
| Build modules only | 6,058 ms (6,058 ms (6s)) |
| Build with modules (rest) | 80,510 ms (80,510 ms (1m 20s)) |
| Peak ops/sec | 1,210,742 |
| Peak scenario | jac313_store_005_XS (none/on) — [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_on.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 884 | 882,480 | 8.38 MiB |
|  |  | jtext | 1,579 | 937,576 | 7.92 MiB |
|  |  | sql | 715 | 966,380 | 7.41 MiB |
| jac313_store_005_TS | on | binary | 887 | 869,996 | 8.38 MiB |
|  |  | jtext | 1,593 | 986,621 | 7.92 MiB |
|  |  | sql | 1,495 | 721,923 | 7.41 MiB |
| jac313_store_005_XS | off | binary | 893 | 829,084 | 8.38 MiB |
|  |  | jtext | 1,532 | 1,116,296 | 7.44 MiB |
|  |  | sql | 789 | 922,245 | 7.12 MiB |
| jac313_store_005_XS | on | binary | 885 | 1,110,396 | 8.38 MiB |
|  |  | jtext | 1,622 | 970,761 | 7.44 MiB |
|  |  | sql | 1,423 | 813,862 | 7.12 MiB |
| jac313_store_006_TS | off | binary | 34,330 | N/A | 19.81 MiB |
|  |  | jtext | 34,558 | N/A | 20.41 MiB |
|  |  | sql | 34,366 | N/A | 23.47 MiB |
| jac313_store_006_TS | on | binary | 38,054 | N/A | 19.81 MiB |
|  |  | jtext | 38,474 | N/A | 20.41 MiB |
|  |  | sql | 38,665 | N/A | 23.47 MiB |
| jac313_store_006_XS | off | binary | 33,712 | N/A | 19.81 MiB |
|  |  | jtext | 33,972 | N/A | 19.96 MiB |
|  |  | sql | 33,676 | N/A | 22.61 MiB |
| jac313_store_006_XS | on | binary | 37,246 | N/A | 19.81 MiB |
|  |  | jtext | 37,580 | N/A | 19.96 MiB |
|  |  | sql | 37,586 | N/A | 22.61 MiB |
| jac313_store_007_TS | off | binary | 1,045 | 806,250 | 19.82 MiB |
|  |  | jtext | 3,973 | 825,205 | 20.45 MiB |
|  |  | sql | 1,305 | 877,355 | 23.49 MiB |
| jac313_store_007_TS | on | binary | 1,040 | 804,149 | 19.82 MiB |
|  |  | jtext | 4,411 | 819,464 | 20.45 MiB |
|  |  | sql | 2,112 | 914,353 | 23.49 MiB |
| jac313_store_007_XS | off | binary | 1,008 | 866,964 | 19.82 MiB |
|  |  | jtext | 3,949 | 762,742 | 19.98 MiB |
|  |  | sql | 1,304 | 790,614 | 22.62 MiB |
| jac313_store_007_XS | on | binary | 972 | 866,994 | 19.82 MiB |
|  |  | jtext | 4,011 | 1,034,340 | 19.98 MiB |
|  |  | sql | 2,078 | 1,124,177 | 22.62 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 302 | - | 104.06 KiB |
|  |  | jtext | 202 | - | 94.68 KiB |
|  |  | sql | 201 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 302 | - | 104.06 KiB |
|  |  | jtext | 253 | - | 94.83 KiB |
|  |  | sql | 562 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 251 | - | 104.06 KiB |
|  |  | jtext | 252 | - | 93.18 KiB |
|  |  | sql | 203 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 303 | - | 104.06 KiB |
|  |  | jtext | 202 | - | 93.18 KiB |
|  |  | sql | 453 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 202 | - | 156.06 KiB |
|  |  | jtext | 202 | - | 140.56 KiB |
|  |  | sql | 151 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 203 | - | 156.06 KiB |
|  |  | jtext | 202 | - | 140.56 KiB |
|  |  | sql | 353 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 252 | - | 156.06 KiB |
|  |  | jtext | 203 | - | 138.31 KiB |
|  |  | sql | 151 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 201 | - | 156.06 KiB |
|  |  | jtext | 151 | - | 138.31 KiB |
|  |  | sql | 402 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 657 | - | 311.94 KiB |
|  |  | jtext | 606 | - | 281.26 KiB |
|  |  | sql | 503 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 654 | - | 311.94 KiB |
|  |  | jtext | 705 | - | 281.69 KiB |
|  |  | sql | 1,005 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 553 | - | 311.94 KiB |
|  |  | jtext | 554 | - | 276.56 KiB |
|  |  | sql | 557 | - | 352.00 KiB |
| jac313_store_003_XS | on | binary | 655 | - | 311.94 KiB |
|  |  | jtext | 655 | - | 276.56 KiB |
|  |  | sql | 855 | - | 352.00 KiB |
| jac313_store_004_TS | off | binary | 605 | - | 519.80 KiB |
|  |  | jtext | 655 | - | 472.06 KiB |
|  |  | sql | 553 | - | 596.00 KiB |
| jac313_store_004_TS | on | binary | 655 | - | 519.80 KiB |
|  |  | jtext | 705 | - | 471.75 KiB |
|  |  | sql | 905 | - | 596.00 KiB |
| jac313_store_004_XS | off | binary | 605 | - | 519.80 KiB |
|  |  | jtext | 603 | - | 463.24 KiB |
|  |  | sql | 503 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 661 | - | 519.80 KiB |
|  |  | jtext | 708 | - | 463.24 KiB |
|  |  | sql | 905 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 884 | 882,480 | 8.38 MiB |
|  |  | jtext | 1,579 | 937,576 | 7.92 MiB |
|  |  | sql | 715 | 966,380 | 7.41 MiB |
| jac313_store_005_TS | on | binary | 887 | 869,996 | 8.38 MiB |
|  |  | jtext | 1,593 | 986,621 | 7.92 MiB |
|  |  | sql | 1,495 | 721,923 | 7.41 MiB |
| jac313_store_005_XS | off | binary | 893 | 829,084 | 8.38 MiB |
|  |  | jtext | 1,532 | 1,116,296 | 7.44 MiB |
|  |  | sql | 789 | 922,245 | 7.12 MiB |
| jac313_store_005_XS | on | binary | 885 | 1,110,396 | 8.38 MiB |
|  |  | jtext | 1,622 | 970,761 | 7.44 MiB |
|  |  | sql | 1,423 | 813,862 | 7.12 MiB |
| jac313_store_006_TS | off | binary | 34,330 | N/A | 19.81 MiB |
|  |  | jtext | 34,558 | N/A | 20.41 MiB |
|  |  | sql | 34,366 | N/A | 23.47 MiB |
| jac313_store_006_TS | on | binary | 38,054 | N/A | 19.81 MiB |
|  |  | jtext | 38,474 | N/A | 20.41 MiB |
|  |  | sql | 38,665 | N/A | 23.47 MiB |
| jac313_store_006_XS | off | binary | 33,712 | N/A | 19.81 MiB |
|  |  | jtext | 33,972 | N/A | 19.96 MiB |
|  |  | sql | 33,676 | N/A | 22.61 MiB |
| jac313_store_006_XS | on | binary | 37,246 | N/A | 19.81 MiB |
|  |  | jtext | 37,580 | N/A | 19.96 MiB |
|  |  | sql | 37,586 | N/A | 22.61 MiB |
| jac313_store_007_TS | off | binary | 1,045 | 806,250 | 19.82 MiB |
|  |  | jtext | 3,973 | 825,205 | 20.45 MiB |
|  |  | sql | 1,305 | 877,355 | 23.49 MiB |
| jac313_store_007_TS | on | binary | 1,040 | 804,149 | 19.82 MiB |
|  |  | jtext | 4,411 | 819,464 | 20.45 MiB |
|  |  | sql | 2,112 | 914,353 | 23.49 MiB |
| jac313_store_007_XS | off | binary | 1,008 | 866,964 | 19.82 MiB |
|  |  | jtext | 3,949 | 762,742 | 19.98 MiB |
|  |  | sql | 1,304 | 790,614 | 22.62 MiB |
| jac313_store_007_XS | on | binary | 972 | 866,994 | 19.82 MiB |
|  |  | jtext | 4,011 | 1,034,340 | 19.98 MiB |
|  |  | sql | 2,078 | 1,124,177 | 22.62 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 302 | - | 104.06 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 302 | - | 104.06 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 202 | - | 94.68 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 253 | - | 94.83 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 202 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 202 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 201 | - | 152.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 562 | - | 152.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 251 | - | 104.06 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 303 | - | 104.06 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 252 | - | 93.18 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 202 | - | 93.18 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 203 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 201 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 203 | - | 148.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 453 | - | 148.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 202 | - | 156.06 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 203 | - | 156.06 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 202 | - | 140.56 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 202 | - | 140.56 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 152 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 151 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 151 | - | 200.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 353 | - | 200.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 252 | - | 156.06 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 201 | - | 156.06 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 203 | - | 138.31 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 151 | - | 138.31 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 151 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 151 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 151 | - | 192.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 402 | - | 192.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 657 | - | 311.94 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 654 | - | 311.94 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 606 | - | 281.26 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 705 | - | 281.69 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 553 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 604 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 503 | - | 368.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 1,005 | - | 368.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 553 | - | 311.94 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 655 | - | 311.94 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 554 | - | 276.56 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 655 | - | 276.56 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 503 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 608 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 557 | - | 352.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 855 | - | 352.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 605 | - | 519.80 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 655 | - | 519.80 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 655 | - | 472.06 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 705 | - | 471.75 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 506 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 609 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 553 | - | 596.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 905 | - | 596.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 605 | - | 519.80 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 661 | - | 519.80 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 603 | - | 463.24 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 708 | - | 463.24 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 503 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 554 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 503 | - | 572.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 905 | - | 572.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 884 | 882,480 | 8.38 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 887 | 869,996 | 8.38 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 1,579 | 937,576 | 7.92 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 1,593 | 986,621 | 7.92 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 670 | 1,034,436 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 589 | 1,114,517 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 715 | 966,380 | 7.41 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 1,495 | 721,923 | 7.41 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 893 | 829,084 | 8.38 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 885 | 1,110,396 | 8.38 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 1,532 | 1,116,296 | 7.44 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 1,622 | 970,761 | 7.44 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 522 | 1,152,140 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 534 | 1,210,742 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 789 | 922,245 | 7.12 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 1,423 | 813,862 | 7.12 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 34,330 | N/A | 19.81 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 38,054 | N/A | 19.81 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 34,558 | N/A | 20.41 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 38,474 | N/A | 20.41 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 33,978 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 37,855 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 34,366 | N/A | 23.47 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 38,665 | N/A | 23.47 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 33,712 | N/A | 19.81 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 37,246 | N/A | 19.81 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 33,972 | N/A | 19.96 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 37,580 | N/A | 19.96 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 33,316 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 36,736 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 33,676 | N/A | 22.61 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 37,586 | N/A | 22.61 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 1,045 | 806,250 | 19.82 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 1,040 | 804,149 | 19.82 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 3,973 | 825,205 | 20.45 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 4,411 | 819,464 | 20.45 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 614 | 990,011 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 603 | 923,975 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 1,305 | 877,355 | 23.49 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 2,112 | 914,353 | 23.49 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 1,008 | 866,964 | 19.82 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 972 | 866,994 | 19.82 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 3,949 | 762,742 | 19.98 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 4,011 | 1,034,340 | 19.98 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 612 | 1,133,530 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 560 | 972,526 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 1,304 | 790,614 | 22.62 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 2,078 | 1,124,177 | 22.62 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 7,067 | 1,131,140 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 6,787 | 1,064,326 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 52 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/clang_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 203 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/clang_unit_off.log) |
