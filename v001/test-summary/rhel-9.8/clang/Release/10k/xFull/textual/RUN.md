# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-20 05:17:51Z |
| OS | rhel-9.8 |
| Compiler | clang |
| Build type | Release |
| Disk | 10k |
| Size | full (xFull) |
| Group ID | 13 |
| Environment | jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 571,604 ms (9m 31s) |
| Peak ops/sec | 10,458,610 |
| Peak scenario | jac313_store_008_XS (flags/off) — [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 301 | 6,974,473 | 8.38 MiB |
|  |  | jtext | 351 | 7,112,376 | 7.87 MiB |
|  |  | sql | 351 | 5,934,366 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 351 | 5,767,678 | 8.38 MiB |
|  |  | jtext | 301 | 6,450,781 | 7.87 MiB |
|  |  | sql | 602 | 6,248,047 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 301 | 6,605,020 | 8.38 MiB |
|  |  | jtext | 401 | 7,185,457 | 7.44 MiB |
|  |  | sql | 301 | 6,943,480 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 301 | 7,130,125 | 8.38 MiB |
|  |  | jtext | 301 | 6,285,355 | 7.44 MiB |
|  |  | sql | 551 | 5,564,211 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 31,836 | N/A | 19.81 MiB |
|  |  | jtext | 31,834 | N/A | 20.37 MiB |
|  |  | sql | 31,834 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 35,405 | N/A | 19.81 MiB |
|  |  | jtext | 35,398 | N/A | 20.37 MiB |
|  |  | sql | 35,701 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 31,281 | N/A | 19.81 MiB |
|  |  | jtext | 31,281 | N/A | 19.96 MiB |
|  |  | sql | 31,232 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 34,498 | N/A | 19.81 MiB |
|  |  | jtext | 34,494 | N/A | 19.96 MiB |
|  |  | sql | 34,745 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 351 | 6,710,959 | 19.82 MiB |
|  |  | jtext | 601 | 8,751,203 | 20.40 MiB |
|  |  | sql | 651 | 6,290,099 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 351 | 7,442,691 | 19.82 MiB |
|  |  | jtext | 952 | 9,885,330 | 20.39 MiB |
|  |  | sql | 1,153 | 6,253,518 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 351 | 6,380,399 | 19.82 MiB |
|  |  | jtext | 752 | 7,476,636 | 19.98 MiB |
|  |  | sql | 651 | 6,530,399 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 351 | 6,640,547 | 19.82 MiB |
|  |  | jtext | 601 | 6,330,316 | 19.98 MiB |
|  |  | sql | 1,102 | 6,832,468 | 22.66 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 201 | - | 104.06 KiB |
|  |  | jtext | 151 | - | 94.65 KiB |
|  |  | sql | 202 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 201 | - | 104.06 KiB |
|  |  | jtext | 201 | - | 94.60 KiB |
|  |  | sql | 252 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 152 | - | 104.06 KiB |
|  |  | jtext | 202 | - | 93.15 KiB |
|  |  | sql | 152 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 202 | - | 104.06 KiB |
|  |  | jtext | 202 | - | 93.15 KiB |
|  |  | sql | 252 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 152 | - | 156.06 KiB |
|  |  | jtext | 102 | - | 140.53 KiB |
|  |  | sql | 101 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 152 | - | 156.06 KiB |
|  |  | jtext | 152 | - | 140.45 KiB |
|  |  | sql | 201 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 101 | - | 156.06 KiB |
|  |  | jtext | 101 | - | 138.28 KiB |
|  |  | sql | 101 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 151 | - | 156.06 KiB |
|  |  | jtext | 152 | - | 138.28 KiB |
|  |  | sql | 202 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 504 | - | 311.93 KiB |
|  |  | jtext | 504 | - | 281.04 KiB |
|  |  | sql | 503 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 554 | - | 311.93 KiB |
|  |  | jtext | 555 | - | 281.04 KiB |
|  |  | sql | 604 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 504 | - | 311.93 KiB |
|  |  | jtext | 503 | - | 276.54 KiB |
|  |  | sql | 504 | - | 356.00 KiB |
| jac313_store_003_XS | on | binary | 554 | - | 311.93 KiB |
|  |  | jtext | 504 | - | 276.54 KiB |
|  |  | sql | 754 | - | 356.00 KiB |
| jac313_store_004_TS | off | binary | 503 | - | 519.79 KiB |
|  |  | jtext | 504 | - | 470.74 KiB |
|  |  | sql | 504 | - | 596.00 KiB |
| jac313_store_004_TS | on | binary | 603 | - | 519.79 KiB |
|  |  | jtext | 553 | - | 470.64 KiB |
|  |  | sql | 654 | - | 596.00 KiB |
| jac313_store_004_XS | off | binary | 453 | - | 519.79 KiB |
|  |  | jtext | 452 | - | 463.22 KiB |
|  |  | sql | 452 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 554 | - | 519.79 KiB |
|  |  | jtext | 554 | - | 463.22 KiB |
|  |  | sql | 602 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 301 | 6,974,473 | 8.38 MiB |
|  |  | jtext | 351 | 7,112,376 | 7.87 MiB |
|  |  | sql | 351 | 5,934,366 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 351 | 5,767,678 | 8.38 MiB |
|  |  | jtext | 301 | 6,450,781 | 7.87 MiB |
|  |  | sql | 602 | 6,248,047 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 301 | 6,605,020 | 8.38 MiB |
|  |  | jtext | 401 | 7,185,457 | 7.44 MiB |
|  |  | sql | 301 | 6,943,480 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 301 | 7,130,125 | 8.38 MiB |
|  |  | jtext | 301 | 6,285,355 | 7.44 MiB |
|  |  | sql | 551 | 5,564,211 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 31,836 | N/A | 19.81 MiB |
|  |  | jtext | 31,834 | N/A | 20.37 MiB |
|  |  | sql | 31,834 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 35,405 | N/A | 19.81 MiB |
|  |  | jtext | 35,398 | N/A | 20.37 MiB |
|  |  | sql | 35,701 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 31,281 | N/A | 19.81 MiB |
|  |  | jtext | 31,281 | N/A | 19.96 MiB |
|  |  | sql | 31,232 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 34,498 | N/A | 19.81 MiB |
|  |  | jtext | 34,494 | N/A | 19.96 MiB |
|  |  | sql | 34,745 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 351 | 6,710,959 | 19.82 MiB |
|  |  | jtext | 601 | 8,751,203 | 20.40 MiB |
|  |  | sql | 651 | 6,290,099 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 351 | 7,442,691 | 19.82 MiB |
|  |  | jtext | 952 | 9,885,330 | 20.39 MiB |
|  |  | sql | 1,153 | 6,253,518 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 351 | 6,380,399 | 19.82 MiB |
|  |  | jtext | 752 | 7,476,636 | 19.98 MiB |
|  |  | sql | 651 | 6,530,399 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 351 | 6,640,547 | 19.82 MiB |
|  |  | jtext | 601 | 6,330,316 | 19.98 MiB |
|  |  | sql | 1,102 | 6,832,468 | 22.66 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 201 | - | 104.06 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 201 | - | 104.06 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 151 | - | 94.65 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 201 | - | 94.60 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 152 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 202 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 202 | - | 152.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 252 | - | 152.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 152 | - | 104.06 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 202 | - | 104.06 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 202 | - | 93.15 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 202 | - | 93.15 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 151 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 202 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 152 | - | 148.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 252 | - | 148.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 152 | - | 156.06 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 152 | - | 156.06 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 102 | - | 140.53 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 152 | - | 140.45 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 102 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 151 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 101 | - | 200.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 201 | - | 200.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 101 | - | 156.06 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 151 | - | 156.06 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 101 | - | 138.28 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 152 | - | 138.28 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 102 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 153 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 101 | - | 192.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 202 | - | 192.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 504 | - | 311.93 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 554 | - | 311.93 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 504 | - | 281.04 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 555 | - | 281.04 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 504 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 555 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 503 | - | 368.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 604 | - | 368.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 504 | - | 311.93 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 554 | - | 311.93 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 503 | - | 276.54 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 504 | - | 276.54 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 504 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 554 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 504 | - | 356.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 754 | - | 356.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 503 | - | 519.79 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 603 | - | 519.79 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 504 | - | 470.74 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 553 | - | 470.64 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 503 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 553 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 504 | - | 596.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 654 | - | 596.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 453 | - | 519.79 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 554 | - | 519.79 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 452 | - | 463.22 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 554 | - | 463.22 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 452 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 552 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 452 | - | 572.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 602 | - | 572.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 301 | 6,974,473 | 8.38 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 351 | 5,767,678 | 8.38 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 351 | 7,112,376 | 7.87 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 301 | 6,450,781 | 7.87 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 151 | 6,288,517 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 100 | 6,636,581 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 351 | 5,934,366 | 7.42 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 602 | 6,248,047 | 7.42 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 301 | 6,605,020 | 8.38 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 301 | 7,130,125 | 8.38 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 401 | 7,185,457 | 7.44 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 301 | 6,285,355 | 7.44 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 151 | 8,394,191 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 150 | 6,722,237 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 301 | 6,943,480 | 7.13 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 551 | 5,564,211 | 7.13 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 31,836 | N/A | 19.81 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 35,405 | N/A | 19.81 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 31,834 | N/A | 20.37 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 35,398 | N/A | 20.37 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 31,688 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 35,300 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 31,834 | N/A | 23.50 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 35,701 | N/A | 23.50 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 31,281 | N/A | 19.81 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 34,498 | N/A | 19.81 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 31,281 | N/A | 19.96 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 34,494 | N/A | 19.96 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 31,130 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 34,342 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 31,232 | N/A | 22.64 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 34,745 | N/A | 22.64 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 351 | 6,710,959 | 19.82 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 351 | 7,442,691 | 19.82 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 601 | 8,751,203 | 20.40 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 952 | 9,885,330 | 20.39 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 150 | 6,808,742 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 150 | 6,696,130 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 651 | 6,290,099 | 23.51 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 1,153 | 6,253,518 | 23.51 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 351 | 6,380,399 | 19.82 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 351 | 6,640,547 | 19.82 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 752 | 7,476,636 | 19.98 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 601 | 6,330,316 | 19.98 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 150 | 6,745,818 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 100 | 7,369,197 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 651 | 6,530,399 | 22.66 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 1,102 | 6,832,468 | 22.66 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 1,804 | 9,604,948 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 1,954 | 10,458,610 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/clang_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 50 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/clang_unit_off.log) |
