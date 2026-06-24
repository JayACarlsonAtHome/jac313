# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-24 17:32:47Z |
| OS | fedora-44 |
| Compiler | clang |
| Build type | Release |
| Disk | x7k |
| Size | full (xFull) |
| Group ID | 89 |
| Environment | jac313-008 · Fedora Linux 44 (Workstation Edition) (7.0.12-201.fc44.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 23 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 617,750 ms (10m 17s) |
| Build straight (no modules) | 11,336 ms (11,336 ms (11s)) |
| Build modules only | 1,656 ms (1,656 ms (1s)) |
| Build with modules (rest) | 12,786 ms (12,786 ms (12s)) |
| Peak ops/sec | 21,390,832 |
| Peak scenario | jac313_store_008_TS (flags/off) — [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 401 | 15,323,322 | 8.38 MiB |
|  |  | jtext | 100 | 12,763,242 | 7.82 MiB |
|  |  | sql | 100 | 17,047,392 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 502 | 19,912,386 | 8.38 MiB |
|  |  | jtext | 100 | 16,377,334 | 7.82 MiB |
|  |  | sql | 1,405 | 16,498,928 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 351 | 20,946,795 | 8.38 MiB |
|  |  | jtext | 100 | 16,439,257 | 7.44 MiB |
|  |  | sql | 100 | 21,110,407 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 402 | 18,331,806 | 8.38 MiB |
|  |  | jtext | 100 | 17,388,280 | 7.44 MiB |
|  |  | sql | 1,305 | 16,772,895 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,060 | N/A | 19.81 MiB |
|  |  | jtext | 33,710 | N/A | 20.33 MiB |
|  |  | sql | 33,692 | N/A | 23.49 MiB |
| jac313_store_006_TS | on | binary | 37,686 | N/A | 19.81 MiB |
|  |  | jtext | 37,523 | N/A | 20.32 MiB |
|  |  | sql | 38,623 | N/A | 23.49 MiB |
| jac313_store_006_XS | off | binary | 33,394 | N/A | 19.81 MiB |
|  |  | jtext | 33,099 | N/A | 19.96 MiB |
|  |  | sql | 33,108 | N/A | 22.63 MiB |
| jac313_store_006_XS | on | binary | 36,925 | N/A | 19.81 MiB |
|  |  | jtext | 36,506 | N/A | 19.96 MiB |
|  |  | sql | 37,777 | N/A | 22.63 MiB |
| jac313_store_007_TS | off | binary | 552 | 12,496,876 | 19.82 MiB |
|  |  | jtext | 251 | 12,933,264 | 20.36 MiB |
|  |  | sql | 200 | 14,827,995 | 23.48 MiB |
| jac313_store_007_TS | on | binary | 501 | 17,370,158 | 19.82 MiB |
|  |  | jtext | 200 | 17,863,523 | 20.36 MiB |
|  |  | sql | 2,509 | 16,118,633 | 23.48 MiB |
| jac313_store_007_XS | off | binary | 502 | 18,112,661 | 19.82 MiB |
|  |  | jtext | 250 | 14,192,450 | 19.98 MiB |
|  |  | sql | 200 | 11,842,729 | 22.64 MiB |
| jac313_store_007_XS | on | binary | 451 | 10,939,722 | 19.82 MiB |
|  |  | jtext | 200 | 18,621,974 | 19.98 MiB |
|  |  | sql | 2,258 | 14,570,887 | 22.64 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 251 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 94.23 KiB |
|  |  | sql | 201 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 301 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 94.23 KiB |
|  |  | sql | 1,054 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 301 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.23 KiB |
|  |  | sql | 201 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 251 | - | 104.07 KiB |
|  |  | jtext | 201 | - | 93.23 KiB |
|  |  | sql | 1,055 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 552 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 139.86 KiB |
|  |  | sql | 150 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 201 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 139.86 KiB |
|  |  | sql | 1,003 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 301 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 138.36 KiB |
|  |  | sql | 150 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 201 | - | 156.07 KiB |
|  |  | jtext | 150 | - | 138.36 KiB |
|  |  | sql | 1,505 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 754 | - | 311.94 KiB |
|  |  | jtext | 552 | - | 279.94 KiB |
|  |  | sql | 553 | - | 368.00 KiB |
| jac313_store_003_TS | on | binary | 653 | - | 311.94 KiB |
|  |  | jtext | 603 | - | 279.74 KiB |
|  |  | sql | 1,606 | - | 368.00 KiB |
| jac313_store_003_XS | off | binary | 652 | - | 311.94 KiB |
|  |  | jtext | 552 | - | 276.62 KiB |
|  |  | sql | 552 | - | 356.00 KiB |
| jac313_store_003_XS | on | binary | 703 | - | 311.94 KiB |
|  |  | jtext | 603 | - | 276.62 KiB |
|  |  | sql | 1,656 | - | 356.00 KiB |
| jac313_store_004_TS | off | binary | 653 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 469.50 KiB |
|  |  | sql | 502 | - | 592.00 KiB |
| jac313_store_004_TS | on | binary | 703 | - | 519.80 KiB |
|  |  | jtext | 602 | - | 469.34 KiB |
|  |  | sql | 1,657 | - | 592.00 KiB |
| jac313_store_004_XS | off | binary | 553 | - | 519.80 KiB |
|  |  | jtext | 502 | - | 463.30 KiB |
|  |  | sql | 502 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 703 | - | 519.80 KiB |
|  |  | jtext | 552 | - | 463.30 KiB |
|  |  | sql | 1,556 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 401 | 15,323,322 | 8.38 MiB |
|  |  | jtext | 100 | 12,763,242 | 7.82 MiB |
|  |  | sql | 100 | 17,047,392 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 502 | 19,912,386 | 8.38 MiB |
|  |  | jtext | 100 | 16,377,334 | 7.82 MiB |
|  |  | sql | 1,405 | 16,498,928 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 351 | 20,946,795 | 8.38 MiB |
|  |  | jtext | 100 | 16,439,257 | 7.44 MiB |
|  |  | sql | 100 | 21,110,407 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 402 | 18,331,806 | 8.38 MiB |
|  |  | jtext | 100 | 17,388,280 | 7.44 MiB |
|  |  | sql | 1,305 | 16,772,895 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,060 | N/A | 19.81 MiB |
|  |  | jtext | 33,710 | N/A | 20.33 MiB |
|  |  | sql | 33,692 | N/A | 23.49 MiB |
| jac313_store_006_TS | on | binary | 37,686 | N/A | 19.81 MiB |
|  |  | jtext | 37,523 | N/A | 20.32 MiB |
|  |  | sql | 38,623 | N/A | 23.49 MiB |
| jac313_store_006_XS | off | binary | 33,394 | N/A | 19.81 MiB |
|  |  | jtext | 33,099 | N/A | 19.96 MiB |
|  |  | sql | 33,108 | N/A | 22.63 MiB |
| jac313_store_006_XS | on | binary | 36,925 | N/A | 19.81 MiB |
|  |  | jtext | 36,506 | N/A | 19.96 MiB |
|  |  | sql | 37,777 | N/A | 22.63 MiB |
| jac313_store_007_TS | off | binary | 552 | 12,496,876 | 19.82 MiB |
|  |  | jtext | 251 | 12,933,264 | 20.36 MiB |
|  |  | sql | 200 | 14,827,995 | 23.48 MiB |
| jac313_store_007_TS | on | binary | 501 | 17,370,158 | 19.82 MiB |
|  |  | jtext | 200 | 17,863,523 | 20.36 MiB |
|  |  | sql | 2,509 | 16,118,633 | 23.48 MiB |
| jac313_store_007_XS | off | binary | 502 | 18,112,661 | 19.82 MiB |
|  |  | jtext | 250 | 14,192,450 | 19.98 MiB |
|  |  | sql | 200 | 11,842,729 | 22.64 MiB |
| jac313_store_007_XS | on | binary | 451 | 10,939,722 | 19.82 MiB |
|  |  | jtext | 200 | 18,621,974 | 19.98 MiB |
|  |  | sql | 2,258 | 14,570,887 | 22.64 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 251 | - | 104.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 301 | - | 104.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/clang_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 201 | - | 94.23 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 201 | - | 94.23 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/clang_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 201 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 201 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/clang_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 201 | - | 152.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 1,054 | - | 152.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/clang_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 301 | - | 104.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 251 | - | 104.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/clang_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 201 | - | 93.23 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 201 | - | 93.23 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/clang_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 201 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 201 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/clang_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 201 | - | 148.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 1,055 | - | 148.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/clang_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 552 | - | 156.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 201 | - | 156.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/clang_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 150 | - | 139.86 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 150 | - | 139.86 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/clang_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 151 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 151 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/clang_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 150 | - | 200.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 1,003 | - | 200.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/clang_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 301 | - | 156.07 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 201 | - | 156.07 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/clang_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 150 | - | 138.36 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 150 | - | 138.36 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/clang_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 150 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 150 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/clang_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 150 | - | 192.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 1,505 | - | 192.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/clang_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 754 | - | 311.94 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 653 | - | 311.94 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/clang_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 552 | - | 279.94 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 603 | - | 279.74 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/clang_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 552 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 602 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/clang_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 553 | - | 368.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 1,606 | - | 368.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/clang_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 652 | - | 311.94 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 703 | - | 311.94 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/clang_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 552 | - | 276.62 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 603 | - | 276.62 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/clang_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 553 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 602 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/clang_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 552 | - | 356.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 1,656 | - | 356.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/clang_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 653 | - | 519.80 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 703 | - | 519.80 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/clang_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 502 | - | 469.50 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 602 | - | 469.34 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/clang_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 502 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 603 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/clang_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 502 | - | 592.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 1,657 | - | 592.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/clang_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 553 | - | 519.80 KiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 703 | - | 519.80 KiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/clang_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 502 | - | 463.30 KiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 552 | - | 463.30 KiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/clang_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 502 | - | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 552 | - | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/clang_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 502 | - | 572.00 KiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 1,556 | - | 572.00 KiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/clang_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 401 | 15,323,322 | 8.38 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 502 | 19,912,386 | 8.38 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 100 | 12,763,242 | 7.82 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 100 | 16,377,334 | 7.82 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 50 | 16,020,506 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 50 | 13,406,623 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 100 | 17,047,392 | 7.42 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 1,405 | 16,498,928 | 7.42 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 351 | 20,946,795 | 8.38 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 402 | 18,331,806 | 8.38 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 100 | 16,439,257 | 7.44 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 100 | 17,388,280 | 7.44 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 50 | 14,731,880 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 50 | 15,323,322 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 100 | 21,110,407 | 7.13 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 1,305 | 16,772,895 | 7.13 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 34,060 | N/A | 19.81 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 37,686 | N/A | 19.81 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/clang_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 33,710 | N/A | 20.33 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 37,523 | N/A | 20.32 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/clang_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 33,650 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 37,465 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/clang_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 33,692 | N/A | 23.49 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 38,623 | N/A | 23.49 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/clang_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 33,394 | N/A | 19.81 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 36,925 | N/A | 19.81 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/clang_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 33,099 | N/A | 19.96 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 36,506 | N/A | 19.96 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/clang_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 33,062 | N/A | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 36,478 | N/A | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/clang_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 33,108 | N/A | 22.63 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 37,777 | N/A | 22.63 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/clang_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 552 | 12,496,876 | 19.82 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 501 | 17,370,158 | 19.82 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 251 | 12,933,264 | 20.36 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 200 | 17,863,523 | 20.36 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 50 | 17,998,560 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 100 | 12,453,300 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 200 | 14,827,995 | 23.48 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 2,509 | 16,118,633 | 23.48 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 502 | 18,112,661 | 19.82 MiB | - | [clang_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 451 | 10,939,722 | 19.82 MiB | - | [clang_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 250 | 14,192,450 | 19.98 MiB | - | [clang_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 200 | 18,621,974 | 19.98 MiB | - | [clang_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/clang_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 50 | 19,813,751 | - | - | [clang_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 50 | 21,048,200 | - | - | [clang_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 200 | 11,842,729 | 22.64 MiB | - | [clang_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 2,258 | 14,570,887 | 22.64 MiB | - | [clang_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 2,008 | 21,390,832 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 2,012 | 16,765,584 | - | - | [clang_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/clang_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 50 | - | - | - | [clang_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/clang_unit_off.log) |
