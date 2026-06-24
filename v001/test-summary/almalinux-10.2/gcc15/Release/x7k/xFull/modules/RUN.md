# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-21 00:13:29Z |
| OS | almalinux-10.2 |
| Compiler | gcc15 |
| Build type | Release |
| Disk | x7k |
| Size | full (xFull) |
| Group ID | 97 |
| Environment | jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 631,766 ms (10m 31s) |
| Build straight (no modules) | 42,182 ms (42,182 ms (42s)) |
| Build modules only | 5,533 ms (5,533 ms (5s)) |
| Build with modules (rest) | 42,651 ms (42,651 ms (42s)) |
| Peak ops/sec | 7,562,180 |
| Peak scenario | jac313_store_008_XS (flags/off) — [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 408 | 2,578,649 | 8.38 MiB |
|  |  | jtext | 359 | 2,827,575 | 7.91 MiB |
|  |  | sql | 408 | 2,690,848 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 409 | 2,786,757 | 8.38 MiB |
|  |  | jtext | 410 | 2,544,724 | 7.91 MiB |
|  |  | sql | 782 | 2,435,698 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 407 | 2,815,078 | 8.38 MiB |
|  |  | jtext | 360 | 2,990,341 | 7.44 MiB |
|  |  | sql | 402 | 2,660,707 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 410 | 2,543,170 | 8.38 MiB |
|  |  | jtext | 361 | 3,149,309 | 7.44 MiB |
|  |  | sql | 729 | 2,847,867 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 35,030 | N/A | 19.81 MiB |
|  |  | jtext | 35,321 | N/A | 20.41 MiB |
|  |  | sql | 34,991 | N/A | 23.51 MiB |
| jac313_store_006_TS | on | binary | 39,180 | N/A | 19.81 MiB |
|  |  | jtext | 39,165 | N/A | 20.40 MiB |
|  |  | sql | 39,382 | N/A | 23.51 MiB |
| jac313_store_006_XS | off | binary | 34,339 | N/A | 19.81 MiB |
|  |  | jtext | 34,311 | N/A | 19.96 MiB |
|  |  | sql | 34,297 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 37,857 | N/A | 19.81 MiB |
|  |  | jtext | 37,976 | N/A | 19.96 MiB |
|  |  | sql | 38,060 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 514 | 3,031,405 | 19.82 MiB |
|  |  | jtext | 845 | 2,980,715 | 20.45 MiB |
|  |  | sql | 746 | 3,089,376 | 23.50 MiB |
| jac313_store_007_TS | on | binary | 505 | 3,125,684 | 19.82 MiB |
|  |  | jtext | 795 | 3,829,804 | 20.44 MiB |
|  |  | sql | 1,396 | 2,720,274 | 23.50 MiB |
| jac313_store_007_XS | off | binary | 516 | 2,859,267 | 19.82 MiB |
|  |  | jtext | 777 | 3,336,893 | 19.98 MiB |
|  |  | sql | 786 | 2,916,642 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 520 | 3,143,369 | 19.82 MiB |
|  |  | jtext | 739 | 3,383,293 | 19.98 MiB |
|  |  | sql | 1,373 | 2,940,830 | 22.66 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 209 | - | 104.06 KiB |
|  |  | jtext | 213 | - | 94.70 KiB |
|  |  | sql | 209 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 208 | - | 104.06 KiB |
|  |  | jtext | 214 | - | 94.70 KiB |
|  |  | sql | 367 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 209 | - | 104.06 KiB |
|  |  | jtext | 206 | - | 93.20 KiB |
|  |  | sql | 211 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 209 | - | 104.06 KiB |
|  |  | jtext | 208 | - | 93.20 KiB |
|  |  | sql | 355 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 154 | - | 156.06 KiB |
|  |  | jtext | 162 | - | 140.58 KiB |
|  |  | sql | 158 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 156 | - | 156.06 KiB |
|  |  | jtext | 158 | - | 140.58 KiB |
|  |  | sql | 213 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 159 | - | 156.06 KiB |
|  |  | jtext | 162 | - | 138.33 KiB |
|  |  | sql | 161 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 161 | - | 156.06 KiB |
|  |  | jtext | 162 | - | 138.33 KiB |
|  |  | sql | 266 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 514 | - | 311.94 KiB |
|  |  | jtext | 576 | - | 281.08 KiB |
|  |  | sql | 538 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 650 | - | 311.94 KiB |
|  |  | jtext | 572 | - | 281.08 KiB |
|  |  | sql | 805 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 533 | - | 311.94 KiB |
|  |  | jtext | 521 | - | 276.58 KiB |
|  |  | sql | 529 | - | 352.00 KiB |
| jac313_store_003_XS | on | binary | 594 | - | 311.94 KiB |
|  |  | jtext | 592 | - | 276.58 KiB |
|  |  | sql | 769 | - | 352.00 KiB |
| jac313_store_004_TS | off | binary | 541 | - | 519.80 KiB |
|  |  | jtext | 534 | - | 470.76 KiB |
|  |  | sql | 539 | - | 596.00 KiB |
| jac313_store_004_TS | on | binary | 627 | - | 519.80 KiB |
|  |  | jtext | 591 | - | 470.76 KiB |
|  |  | sql | 746 | - | 596.00 KiB |
| jac313_store_004_XS | off | binary | 542 | - | 519.80 KiB |
|  |  | jtext | 538 | - | 463.26 KiB |
|  |  | sql | 535 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 649 | - | 519.80 KiB |
|  |  | jtext | 595 | - | 463.26 KiB |
|  |  | sql | 739 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 408 | 2,578,649 | 8.38 MiB |
|  |  | jtext | 359 | 2,827,575 | 7.91 MiB |
|  |  | sql | 408 | 2,690,848 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 409 | 2,786,757 | 8.38 MiB |
|  |  | jtext | 410 | 2,544,724 | 7.91 MiB |
|  |  | sql | 782 | 2,435,698 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 407 | 2,815,078 | 8.38 MiB |
|  |  | jtext | 360 | 2,990,341 | 7.44 MiB |
|  |  | sql | 402 | 2,660,707 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 410 | 2,543,170 | 8.38 MiB |
|  |  | jtext | 361 | 3,149,309 | 7.44 MiB |
|  |  | sql | 729 | 2,847,867 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 35,030 | N/A | 19.81 MiB |
|  |  | jtext | 35,321 | N/A | 20.41 MiB |
|  |  | sql | 34,991 | N/A | 23.51 MiB |
| jac313_store_006_TS | on | binary | 39,180 | N/A | 19.81 MiB |
|  |  | jtext | 39,165 | N/A | 20.40 MiB |
|  |  | sql | 39,382 | N/A | 23.51 MiB |
| jac313_store_006_XS | off | binary | 34,339 | N/A | 19.81 MiB |
|  |  | jtext | 34,311 | N/A | 19.96 MiB |
|  |  | sql | 34,297 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 37,857 | N/A | 19.81 MiB |
|  |  | jtext | 37,976 | N/A | 19.96 MiB |
|  |  | sql | 38,060 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 514 | 3,031,405 | 19.82 MiB |
|  |  | jtext | 845 | 2,980,715 | 20.45 MiB |
|  |  | sql | 746 | 3,089,376 | 23.50 MiB |
| jac313_store_007_TS | on | binary | 505 | 3,125,684 | 19.82 MiB |
|  |  | jtext | 795 | 3,829,804 | 20.44 MiB |
|  |  | sql | 1,396 | 2,720,274 | 23.50 MiB |
| jac313_store_007_XS | off | binary | 516 | 2,859,267 | 19.82 MiB |
|  |  | jtext | 777 | 3,336,893 | 19.98 MiB |
|  |  | sql | 786 | 2,916,642 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 520 | 3,143,369 | 19.82 MiB |
|  |  | jtext | 739 | 3,383,293 | 19.98 MiB |
|  |  | sql | 1,373 | 2,940,830 | 22.66 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 209 | - | 104.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc15_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 208 | - | 104.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc15_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 213 | - | 94.70 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc15_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 214 | - | 94.70 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc15_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 209 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc15_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 211 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc15_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 209 | - | 152.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc15_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 367 | - | 152.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc15_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 209 | - | 104.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc15_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 209 | - | 104.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc15_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 206 | - | 93.20 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc15_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 208 | - | 93.20 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc15_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 211 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc15_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 211 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc15_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 211 | - | 148.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc15_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 355 | - | 148.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc15_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 154 | - | 156.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc15_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 156 | - | 156.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc15_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 162 | - | 140.58 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc15_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 158 | - | 140.58 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc15_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 158 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc15_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 163 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc15_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 158 | - | 200.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc15_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 213 | - | 200.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc15_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 159 | - | 156.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc15_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 161 | - | 156.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc15_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 162 | - | 138.33 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc15_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 162 | - | 138.33 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc15_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 162 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc15_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 162 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc15_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 161 | - | 192.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc15_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 266 | - | 192.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc15_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 514 | - | 311.94 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc15_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 650 | - | 311.94 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc15_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 576 | - | 281.08 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc15_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 572 | - | 281.08 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc15_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 595 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc15_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 647 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc15_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 538 | - | 368.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc15_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 805 | - | 368.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc15_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 533 | - | 311.94 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc15_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 594 | - | 311.94 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc15_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 521 | - | 276.58 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc15_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 592 | - | 276.58 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc15_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 528 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc15_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 567 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc15_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 529 | - | 352.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc15_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 769 | - | 352.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc15_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 541 | - | 519.80 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc15_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 627 | - | 519.80 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc15_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 534 | - | 470.76 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc15_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 591 | - | 470.76 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc15_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 541 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc15_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 594 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc15_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 539 | - | 596.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc15_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 746 | - | 596.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc15_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 542 | - | 519.80 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc15_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 649 | - | 519.80 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc15_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 538 | - | 463.26 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc15_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 595 | - | 463.26 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc15_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 540 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc15_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 593 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc15_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 535 | - | 572.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc15_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 739 | - | 572.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc15_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 408 | 2,578,649 | 8.38 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 409 | 2,786,757 | 8.38 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 359 | 2,827,575 | 7.91 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 410 | 2,544,724 | 7.91 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 210 | 2,912,904 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 208 | 2,842,282 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 408 | 2,690,848 | 7.42 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 782 | 2,435,698 | 7.42 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 407 | 2,815,078 | 8.38 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 410 | 2,543,170 | 8.38 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 360 | 2,990,341 | 7.44 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 361 | 3,149,309 | 7.44 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 155 | 3,015,591 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 203 | 2,912,226 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 402 | 2,660,707 | 7.13 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc15_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 729 | 2,847,867 | 7.13 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc15_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 35,030 | N/A | 19.81 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc15_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 39,180 | N/A | 19.81 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc15_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 35,321 | N/A | 20.41 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc15_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 39,165 | N/A | 20.40 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc15_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 34,907 | N/A | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc15_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 38,715 | N/A | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc15_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 34,991 | N/A | 23.51 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc15_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 39,382 | N/A | 23.51 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc15_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 34,339 | N/A | 19.81 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc15_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 37,857 | N/A | 19.81 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc15_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 34,311 | N/A | 19.96 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc15_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 37,976 | N/A | 19.96 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc15_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 34,195 | N/A | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc15_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 37,591 | N/A | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc15_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 34,297 | N/A | 22.64 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc15_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 38,060 | N/A | 22.64 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc15_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 514 | 3,031,405 | 19.82 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 505 | 3,125,684 | 19.82 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 845 | 2,980,715 | 20.45 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 795 | 3,829,804 | 20.44 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 154 | 3,787,735 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 153 | 3,358,409 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 746 | 3,089,376 | 23.50 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 1,396 | 2,720,274 | 23.50 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 516 | 2,859,267 | 19.82 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 520 | 3,143,369 | 19.82 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 777 | 3,336,893 | 19.98 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 739 | 3,383,293 | 19.98 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 151 | 3,060,631 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 156 | 2,774,233 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 786 | 2,916,642 | 22.66 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 1,373 | 2,940,830 | 22.66 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 2,373 | 5,198,208 | - | - | [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 2,273 | 7,562,180 | - | - | [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 53 | - | - | - | [gcc15_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc15_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 52 | - | - | - | [gcc15_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc15_unit_off.log) |
