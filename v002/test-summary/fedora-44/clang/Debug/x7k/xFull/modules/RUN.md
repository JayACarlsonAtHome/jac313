# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-24 17:00:56Z |
| OS | fedora-44 |
| Compiler | clang |
| Build type | Debug |
| Disk | x7k |
| Size | full (xFull) |
| Group ID | 87 |
| Environment | jac313-008 · Fedora Linux 44 (Workstation Edition) (7.0.12-201.fc44.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 23 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 625,741 ms (10m 25s) |
| Build straight (no modules) | 8,526 ms (8,526 ms (8s)) |
| Build modules only | 1,654 ms (1,654 ms (1s)) |
| Build with modules (rest) | 10,227 ms (10,227 ms (10s)) |
| Peak ops/sec | 10,867,203 |
| Peak scenario | jac313_store_005_TS (binary/on) — [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_on.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 603 | 9,609,840 | 8.38 MiB |
|  |  | jtext | 358 | 5,838,734 | 7.84 MiB |
|  |  | sql | 150 | 8,492,569 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 401 | 10,867,203 | 8.38 MiB |
|  |  | jtext | 352 | 8,388,558 | 7.84 MiB |
|  |  | sql | 1,104 | 8,975,855 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 452 | 9,506,607 | 8.38 MiB |
|  |  | jtext | 351 | 9,105,810 | 7.44 MiB |
|  |  | sql | 150 | 8,486,803 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 352 | 7,225,434 | 8.38 MiB |
|  |  | jtext | 353 | 9,682,417 | 7.44 MiB |
|  |  | sql | 1,506 | 9,615,385 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,215 | N/A | 19.81 MiB |
|  |  | jtext | 33,813 | N/A | 20.34 MiB |
|  |  | sql | 33,757 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 37,979 | N/A | 19.81 MiB |
|  |  | jtext | 37,627 | N/A | 20.33 MiB |
|  |  | sql | 38,886 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 33,409 | N/A | 19.81 MiB |
|  |  | jtext | 33,259 | N/A | 19.96 MiB |
|  |  | sql | 33,153 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 36,879 | N/A | 19.81 MiB |
|  |  | jtext | 36,719 | N/A | 19.96 MiB |
|  |  | sql | 37,878 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 602 | 7,089,685 | 19.82 MiB |
|  |  | jtext | 954 | 8,340,284 | 20.38 MiB |
|  |  | sql | 251 | 7,711,884 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 602 | 7,227,000 | 19.82 MiB |
|  |  | jtext | 953 | 8,228,421 | 20.39 MiB |
|  |  | sql | 2,612 | 8,357,012 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 603 | 9,132,420 | 19.82 MiB |
|  |  | jtext | 1,003 | 8,657,259 | 19.98 MiB |
|  |  | sql | 252 | 8,573,388 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 503 | 8,907,099 | 19.82 MiB |
|  |  | jtext | 1,003 | 8,740,495 | 19.98 MiB |
|  |  | sql | 2,459 | 8,191,350 | 22.66 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 301 | - | 104.07 KiB |
|  |  | jtext | 200 | - | 94.22 KiB |
|  |  | sql | 200 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 251 | - | 104.07 KiB |
|  |  | jtext | 200 | - | 94.28 KiB |
|  |  | sql | 702 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 251 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.22 KiB |
|  |  | sql | 201 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 401 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.22 KiB |
|  |  | sql | 1,054 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 251 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 140.15 KiB |
|  |  | sql | 150 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 301 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 139.84 KiB |
|  |  | sql | 1,204 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 301 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 138.34 KiB |
|  |  | sql | 150 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 301 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 138.34 KiB |
|  |  | sql | 1,004 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 753 | - | 311.94 KiB |
|  |  | jtext | 552 | - | 280.62 KiB |
|  |  | sql | 552 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 804 | - | 311.94 KiB |
|  |  | jtext | 603 | - | 280.54 KiB |
|  |  | sql | 2,009 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 653 | - | 311.94 KiB |
|  |  | jtext | 552 | - | 276.60 KiB |
|  |  | sql | 552 | - | 356.00 KiB |
| jac313_store_003_XS | on | binary | 703 | - | 311.94 KiB |
|  |  | jtext | 603 | - | 276.60 KiB |
|  |  | sql | 1,556 | - | 356.00 KiB |
| jac313_store_004_TS | off | binary | 652 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 470.64 KiB |
|  |  | sql | 502 | - | 596.00 KiB |
| jac313_store_004_TS | on | binary | 753 | - | 519.80 KiB |
|  |  | jtext | 602 | - | 470.18 KiB |
|  |  | sql | 1,607 | - | 596.00 KiB |
| jac313_store_004_XS | off | binary | 653 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 463.28 KiB |
|  |  | sql | 502 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 703 | - | 519.80 KiB |
|  |  | jtext | 552 | - | 463.28 KiB |
|  |  | sql | 1,556 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 603 | 9,609,840 | 8.38 MiB |
|  |  | jtext | 358 | 5,838,734 | 7.84 MiB |
|  |  | sql | 150 | 8,492,569 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 401 | 10,867,203 | 8.38 MiB |
|  |  | jtext | 352 | 8,388,558 | 7.84 MiB |
|  |  | sql | 1,104 | 8,975,855 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 452 | 9,506,607 | 8.38 MiB |
|  |  | jtext | 351 | 9,105,810 | 7.44 MiB |
|  |  | sql | 150 | 8,486,803 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 352 | 7,225,434 | 8.38 MiB |
|  |  | jtext | 353 | 9,682,417 | 7.44 MiB |
|  |  | sql | 1,506 | 9,615,385 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,215 | N/A | 19.81 MiB |
|  |  | jtext | 33,813 | N/A | 20.34 MiB |
|  |  | sql | 33,757 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 37,979 | N/A | 19.81 MiB |
|  |  | jtext | 37,627 | N/A | 20.33 MiB |
|  |  | sql | 38,886 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 33,409 | N/A | 19.81 MiB |
|  |  | jtext | 33,259 | N/A | 19.96 MiB |
|  |  | sql | 33,153 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 36,879 | N/A | 19.81 MiB |
|  |  | jtext | 36,719 | N/A | 19.96 MiB |
|  |  | sql | 37,878 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 602 | 7,089,685 | 19.82 MiB |
|  |  | jtext | 954 | 8,340,284 | 20.38 MiB |
|  |  | sql | 251 | 7,711,884 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 602 | 7,227,000 | 19.82 MiB |
|  |  | jtext | 953 | 8,228,421 | 20.39 MiB |
|  |  | sql | 2,612 | 8,357,012 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 603 | 9,132,420 | 19.82 MiB |
|  |  | jtext | 1,003 | 8,657,259 | 19.98 MiB |
|  |  | sql | 252 | 8,573,388 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 503 | 8,907,099 | 19.82 MiB |
|  |  | jtext | 1,003 | 8,740,495 | 19.98 MiB |
|  |  | sql | 2,459 | 8,191,350 | 22.66 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 301 | - | 104.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 251 | - | 104.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 200 | - | 94.22 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 200 | - | 94.28 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 201 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 201 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 200 | - | 152.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 702 | - | 152.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 251 | - | 104.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 401 | - | 104.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 201 | - | 93.22 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 201 | - | 93.22 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 200 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 201 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 201 | - | 148.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 1,054 | - | 148.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 251 | - | 156.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 301 | - | 156.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 150 | - | 140.15 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 150 | - | 139.84 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 150 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 150 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 150 | - | 200.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 1,204 | - | 200.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 301 | - | 156.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 301 | - | 156.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 150 | - | 138.34 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 150 | - | 138.34 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 150 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 150 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 150 | - | 192.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 1,004 | - | 192.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 753 | - | 311.94 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 804 | - | 311.94 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 552 | - | 280.62 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 603 | - | 280.54 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 552 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 603 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 552 | - | 368.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 2,009 | - | 368.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 653 | - | 311.94 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 703 | - | 311.94 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 552 | - | 276.60 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 603 | - | 276.60 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 552 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 602 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 552 | - | 356.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 1,556 | - | 356.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 652 | - | 519.80 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 753 | - | 519.80 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 502 | - | 470.64 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 602 | - | 470.18 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 502 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 602 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 502 | - | 596.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 1,607 | - | 596.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 653 | - | 519.80 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 703 | - | 519.80 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 502 | - | 463.28 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 552 | - | 463.28 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 502 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 553 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 502 | - | 572.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 1,556 | - | 572.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 603 | 9,609,840 | 8.38 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 401 | 10,867,203 | 8.38 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 358 | 5,838,734 | 7.84 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 352 | 8,388,558 | 7.84 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 103 | 9,337,940 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 101 | 9,322,271 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 150 | 8,492,569 | 7.42 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 1,104 | 8,975,855 | 7.42 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 452 | 9,506,607 | 8.38 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 352 | 7,225,434 | 8.38 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 351 | 9,105,810 | 7.44 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 353 | 9,682,417 | 7.44 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 100 | 9,672,115 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 100 | 9,955,202 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 150 | 8,486,803 | 7.13 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 1,506 | 9,615,385 | 7.13 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 34,215 | N/A | 19.81 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 37,979 | N/A | 19.81 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 33,813 | N/A | 20.34 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 37,627 | N/A | 20.33 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 33,662 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 37,485 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 33,757 | N/A | 23.50 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 38,886 | N/A | 23.50 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 33,409 | N/A | 19.81 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 36,879 | N/A | 19.81 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 33,259 | N/A | 19.96 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 36,719 | N/A | 19.96 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 33,059 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 36,522 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 33,153 | N/A | 22.64 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 37,878 | N/A | 22.64 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 602 | 7,089,685 | 19.82 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 602 | 7,227,000 | 19.82 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 954 | 8,340,284 | 20.38 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 953 | 8,228,421 | 20.39 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 100 | 8,433,125 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 103 | 8,931,761 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 251 | 7,711,884 | 23.51 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 2,612 | 8,357,012 | 23.51 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 603 | 9,132,420 | 19.82 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 503 | 8,907,099 | 19.82 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 1,003 | 8,657,259 | 19.98 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 1,003 | 8,740,495 | 19.98 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 100 | 9,514,748 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 101 | 9,323,140 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 252 | 8,573,388 | 22.66 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 2,459 | 8,191,350 | 22.66 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 2,516 | 10,267,362 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 2,762 | 10,381,198 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/clang_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 100 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/clang_unit_off.log) |
