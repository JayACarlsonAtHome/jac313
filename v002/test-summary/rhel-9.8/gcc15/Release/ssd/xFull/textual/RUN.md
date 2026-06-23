# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-23 06:27:02Z |
| OS | rhel-9.8 |
| Compiler | gcc15 |
| Build type | Release |
| Disk | ssd |
| Size | full (xFull) |
| Group ID | 32 |
| Environment | j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 589,074 ms (9m 49s) |
| Peak ops/sec | 24,631,755 |
| Peak scenario | jac313_store_008_TS (flags/off) — [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 100 | 20,052,136 | 8.38 MiB |
|  |  | jtext | 100 | 14,019,347 | 7.82 MiB |
|  |  | sql | 100 | 14,731,880 | 7.38 MiB |
| jac313_store_005_TS | on | binary | 100 | 18,175,209 | 8.38 MiB |
|  |  | jtext | 100 | 15,232,292 | 7.82 MiB |
|  |  | sql | 150 | 14,021,312 | 7.38 MiB |
| jac313_store_005_XS | off | binary | 100 | 15,176,810 | 8.38 MiB |
|  |  | jtext | 100 | 17,975,912 | 7.44 MiB |
|  |  | sql | 100 | 14,210,601 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 100 | 21,570,319 | 8.38 MiB |
|  |  | jtext | 100 | 13,902,405 | 7.44 MiB |
|  |  | sql | 150 | 15,211,439 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 33,692 | N/A | 19.81 MiB |
|  |  | jtext | 33,680 | N/A | 20.33 MiB |
|  |  | sql | 33,625 | N/A | 23.30 MiB |
| jac313_store_006_TS | on | binary | 37,149 | N/A | 19.81 MiB |
|  |  | jtext | 37,462 | N/A | 20.32 MiB |
|  |  | sql | 36,781 | N/A | 23.30 MiB |
| jac313_store_006_XS | off | binary | 33,047 | N/A | 19.81 MiB |
|  |  | jtext | 32,944 | N/A | 19.96 MiB |
|  |  | sql | 33,041 | N/A | 22.63 MiB |
| jac313_store_006_XS | on | binary | 36,496 | N/A | 19.81 MiB |
|  |  | jtext | 36,400 | N/A | 19.96 MiB |
|  |  | sql | 36,518 | N/A | 22.63 MiB |
| jac313_store_007_TS | off | binary | 100 | 13,987,970 | 19.82 MiB |
|  |  | jtext | 200 | 14,675,668 | 20.36 MiB |
|  |  | sql | 200 | 21,244,954 | 23.45 MiB |
| jac313_store_007_TS | on | binary | 100 | 17,780,939 | 19.82 MiB |
|  |  | jtext | 200 | 20,165,356 | 20.36 MiB |
|  |  | sql | 250 | 15,762,926 | 23.45 MiB |
| jac313_store_007_XS | off | binary | 100 | 20,644,096 | 19.82 MiB |
|  |  | jtext | 200 | 14,744,913 | 19.98 MiB |
|  |  | sql | 200 | 14,803,849 | 22.64 MiB |
| jac313_store_007_XS | on | binary | 100 | 14,082,524 | 19.82 MiB |
|  |  | jtext | 200 | 14,566,642 | 19.98 MiB |
|  |  | sql | 250 | 18,351,991 | 22.64 MiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 200 | - | 104.06 KiB |
|  |  | jtext | 200 | - | 94.20 KiB |
|  |  | sql | 200 | - | 152.00 KiB |
| jac313_store_001_TS | on | binary | 200 | - | 104.06 KiB |
|  |  | jtext | 200 | - | 94.20 KiB |
|  |  | sql | 200 | - | 152.00 KiB |
| jac313_store_001_XS | off | binary | 200 | - | 104.06 KiB |
|  |  | jtext | 200 | - | 93.20 KiB |
|  |  | sql | 200 | - | 148.00 KiB |
| jac313_store_001_XS | on | binary | 200 | - | 104.06 KiB |
|  |  | jtext | 200 | - | 93.20 KiB |
|  |  | sql | 200 | - | 148.00 KiB |
| jac313_store_002_TS | off | binary | 150 | - | 156.06 KiB |
|  |  | jtext | 150 | - | 139.83 KiB |
|  |  | sql | 150 | - | 200.00 KiB |
| jac313_store_002_TS | on | binary | 150 | - | 156.06 KiB |
|  |  | jtext | 150 | - | 139.83 KiB |
|  |  | sql | 150 | - | 200.00 KiB |
| jac313_store_002_XS | off | binary | 150 | - | 156.06 KiB |
|  |  | jtext | 150 | - | 138.33 KiB |
|  |  | sql | 150 | - | 192.00 KiB |
| jac313_store_002_XS | on | binary | 150 | - | 156.06 KiB |
|  |  | jtext | 150 | - | 138.33 KiB |
|  |  | sql | 150 | - | 192.00 KiB |
| jac313_store_003_TS | off | binary | 502 | - | 311.94 KiB |
|  |  | jtext | 502 | - | 280.20 KiB |
|  |  | sql | 501 | - | 360.00 KiB |
| jac313_store_003_TS | on | binary | 602 | - | 311.94 KiB |
|  |  | jtext | 552 | - | 280.02 KiB |
|  |  | sql | 602 | - | 360.00 KiB |
| jac313_store_003_XS | off | binary | 502 | - | 311.94 KiB |
|  |  | jtext | 502 | - | 276.58 KiB |
|  |  | sql | 502 | - | 352.00 KiB |
| jac313_store_003_XS | on | binary | 552 | - | 311.94 KiB |
|  |  | jtext | 552 | - | 276.58 KiB |
|  |  | sql | 552 | - | 352.00 KiB |
| jac313_store_004_TS | off | binary | 502 | - | 519.80 KiB |
|  |  | jtext | 501 | - | 469.69 KiB |
|  |  | sql | 502 | - | 588.00 KiB |
| jac313_store_004_TS | on | binary | 602 | - | 519.80 KiB |
|  |  | jtext | 552 | - | 469.49 KiB |
|  |  | sql | 602 | - | 588.00 KiB |
| jac313_store_004_XS | off | binary | 502 | - | 519.80 KiB |
|  |  | jtext | 501 | - | 463.26 KiB |
|  |  | sql | 501 | - | 572.00 KiB |
| jac313_store_004_XS | on | binary | 552 | - | 519.80 KiB |
|  |  | jtext | 552 | - | 463.26 KiB |
|  |  | sql | 551 | - | 572.00 KiB |
| jac313_store_005_TS | off | binary | 100 | 20,052,136 | 8.38 MiB |
|  |  | jtext | 100 | 14,019,347 | 7.82 MiB |
|  |  | sql | 100 | 14,731,880 | 7.38 MiB |
| jac313_store_005_TS | on | binary | 100 | 18,175,209 | 8.38 MiB |
|  |  | jtext | 100 | 15,232,292 | 7.82 MiB |
|  |  | sql | 150 | 14,021,312 | 7.38 MiB |
| jac313_store_005_XS | off | binary | 100 | 15,176,810 | 8.38 MiB |
|  |  | jtext | 100 | 17,975,912 | 7.44 MiB |
|  |  | sql | 100 | 14,210,601 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 100 | 21,570,319 | 8.38 MiB |
|  |  | jtext | 100 | 13,902,405 | 7.44 MiB |
|  |  | sql | 150 | 15,211,439 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 33,692 | N/A | 19.81 MiB |
|  |  | jtext | 33,680 | N/A | 20.33 MiB |
|  |  | sql | 33,625 | N/A | 23.30 MiB |
| jac313_store_006_TS | on | binary | 37,149 | N/A | 19.81 MiB |
|  |  | jtext | 37,462 | N/A | 20.32 MiB |
|  |  | sql | 36,781 | N/A | 23.30 MiB |
| jac313_store_006_XS | off | binary | 33,047 | N/A | 19.81 MiB |
|  |  | jtext | 32,944 | N/A | 19.96 MiB |
|  |  | sql | 33,041 | N/A | 22.63 MiB |
| jac313_store_006_XS | on | binary | 36,496 | N/A | 19.81 MiB |
|  |  | jtext | 36,400 | N/A | 19.96 MiB |
|  |  | sql | 36,518 | N/A | 22.63 MiB |
| jac313_store_007_TS | off | binary | 100 | 13,987,970 | 19.82 MiB |
|  |  | jtext | 200 | 14,675,668 | 20.36 MiB |
|  |  | sql | 200 | 21,244,954 | 23.45 MiB |
| jac313_store_007_TS | on | binary | 100 | 17,780,939 | 19.82 MiB |
|  |  | jtext | 200 | 20,165,356 | 20.36 MiB |
|  |  | sql | 250 | 15,762,926 | 23.45 MiB |
| jac313_store_007_XS | off | binary | 100 | 20,644,096 | 19.82 MiB |
|  |  | jtext | 200 | 14,744,913 | 19.98 MiB |
|  |  | sql | 200 | 14,803,849 | 22.64 MiB |
| jac313_store_007_XS | on | binary | 100 | 14,082,524 | 19.82 MiB |
|  |  | jtext | 200 | 14,566,642 | 19.98 MiB |
|  |  | sql | 250 | 18,351,991 | 22.64 MiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 200 | - | 104.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc15_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 200 | - | 104.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc15_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 200 | - | 94.20 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc15_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 200 | - | 94.20 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc15_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 200 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc15_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 201 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc15_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 200 | - | 152.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc15_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 200 | - | 152.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc15_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 200 | - | 104.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc15_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 200 | - | 104.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc15_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 200 | - | 93.20 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc15_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 200 | - | 93.20 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc15_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 200 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc15_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 200 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc15_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 200 | - | 148.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc15_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 200 | - | 148.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc15_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 150 | - | 156.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc15_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 150 | - | 156.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc15_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 150 | - | 139.83 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc15_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 150 | - | 139.83 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc15_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 150 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc15_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 150 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc15_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 150 | - | 200.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc15_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 150 | - | 200.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc15_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 150 | - | 156.06 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc15_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 150 | - | 156.06 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc15_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 150 | - | 138.33 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc15_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 150 | - | 138.33 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc15_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 150 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc15_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 150 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc15_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 150 | - | 192.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc15_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 150 | - | 192.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc15_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 502 | - | 311.94 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc15_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 602 | - | 311.94 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc15_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 502 | - | 280.20 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc15_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 552 | - | 280.02 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc15_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 552 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc15_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 602 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc15_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 501 | - | 360.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc15_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 602 | - | 360.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc15_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 502 | - | 311.94 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc15_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 552 | - | 311.94 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc15_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 502 | - | 276.58 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc15_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 552 | - | 276.58 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc15_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 502 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc15_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 552 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc15_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 502 | - | 352.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc15_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 552 | - | 352.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc15_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 502 | - | 519.80 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc15_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 602 | - | 519.80 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc15_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 501 | - | 469.69 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc15_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 552 | - | 469.49 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc15_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 501 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc15_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 602 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc15_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 502 | - | 588.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc15_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 602 | - | 588.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc15_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 502 | - | 519.80 KiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc15_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 552 | - | 519.80 KiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc15_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 501 | - | 463.26 KiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc15_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 552 | - | 463.26 KiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc15_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 501 | - | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc15_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 551 | - | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc15_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 501 | - | 572.00 KiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc15_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 551 | - | 572.00 KiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc15_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 100 | 20,052,136 | 8.38 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 100 | 18,175,209 | 8.38 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 100 | 14,019,347 | 7.82 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 100 | 15,232,292 | 7.82 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 50 | 15,477,480 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 50 | 20,120,724 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 100 | 14,731,880 | 7.38 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 150 | 14,021,312 | 7.38 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 100 | 15,176,810 | 8.38 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 100 | 21,570,319 | 8.38 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 100 | 17,975,912 | 7.44 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 100 | 13,902,405 | 7.44 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 50 | 15,239,256 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 50 | 15,394,089 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 100 | 14,210,601 | 7.13 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc15_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 150 | 15,211,439 | 7.13 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc15_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 33,692 | N/A | 19.81 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc15_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 37,149 | N/A | 19.81 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc15_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 33,680 | N/A | 20.33 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc15_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 37,462 | N/A | 20.32 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc15_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 33,538 | N/A | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc15_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 37,405 | N/A | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc15_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 33,625 | N/A | 23.30 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc15_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 36,781 | N/A | 23.30 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc15_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 33,047 | N/A | 19.81 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc15_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 36,496 | N/A | 19.81 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc15_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 32,944 | N/A | 19.96 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc15_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 36,400 | N/A | 19.96 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc15_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 32,992 | N/A | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc15_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 36,295 | N/A | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc15_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 33,041 | N/A | 22.63 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc15_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 36,518 | N/A | 22.63 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc15_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 100 | 13,987,970 | 19.82 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 100 | 17,780,939 | 19.82 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 200 | 14,675,668 | 20.36 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 200 | 20,165,356 | 20.36 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 50 | 21,034,918 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 50 | 14,122,299 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 200 | 21,244,954 | 23.45 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 250 | 15,762,926 | 23.45 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 100 | 20,644,096 | 19.82 MiB | - | [gcc15_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 100 | 14,082,524 | 19.82 MiB | - | [gcc15_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 200 | 14,744,913 | 19.98 MiB | - | [gcc15_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 200 | 14,566,642 | 19.98 MiB | - | [gcc15_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 50 | 13,846,580 | - | - | [gcc15_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 50 | 14,617,746 | - | - | [gcc15_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 200 | 14,803,849 | 22.64 MiB | - | [gcc15_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 250 | 18,351,991 | 22.64 MiB | - | [gcc15_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 651 | 24,631,755 | - | - | [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 651 | 24,608,721 | - | - | [gcc15_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [gcc15_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc15_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 50 | - | - | - | [gcc15_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc15_unit_off.log) |
