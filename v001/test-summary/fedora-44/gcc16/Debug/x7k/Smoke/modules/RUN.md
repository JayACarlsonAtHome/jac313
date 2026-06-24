# jac313 test run

_Rendered from [`jac313_results.db`](../../../../../../jac313_results.db)._

| Field | Value |
|-------|-------|
| Run UTC | 2026-06-20 18:22:36Z |
| OS | fedora-44 |
| Compiler | gcc16 |
| Build type | Debug |
| Disk | x7k |
| Size | smoke (Smoke) |
| Group ID | 59 |
| Environment | jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label) |
| Passed | 116 |
| Failed | 0 |
| Skipped | 0 |
| Errors | 0 |
| Duration | 21,167 ms (21s) |
| Build straight (no modules) | 24,334 ms (24,334 ms (24s)) |
| Build modules only | 6,140 ms (6,140 ms (6s)) |
| Build with modules (rest) | 23,589 ms (23,589 ms (23s)) |
| Build modules only (import std) | 15,561 ms (15,561 ms (15s)) |
| Build with modules (import std) | 24,887 ms (24,887 ms (24s)) |
| import std vs textual modules | +1,298 ms (-5% slower) |
| Peak ops/sec | 511,247 |
| Peak scenario | jac313_store_005_XS (none/off) — [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_off.log) |

## Persist backend comparison (binary vs jText vs SQL)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 101 | 181,455 | 85.79 KiB |
|  |  | jtext | 50 | 171,733 | 76.71 KiB |
|  |  | sql | 50 | 172,384 | 84.00 KiB |
| jac313_store_005_TS | on | binary | 101 | 221,779 | 85.79 KiB |
|  |  | jtext | 50 | 164,663 | 76.71 KiB |
|  |  | sql | 151 | 176,647 | 84.00 KiB |
| jac313_store_005_XS | off | binary | 101 | 169,492 | 85.79 KiB |
|  |  | jtext | 50 | 150,331 | 73.78 KiB |
|  |  | sql | 51 | 191,461 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 101 | 174,307 | 85.79 KiB |
|  |  | jtext | 50 | 177,022 | 73.78 KiB |
|  |  | sql | 151 | 186,811 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 403 | N/A | 202.98 KiB |
|  |  | jtext | 402 | N/A | 182.30 KiB |
|  |  | sql | 352 | N/A | 256.00 KiB |
| jac313_store_006_TS | on | binary | 453 | N/A | 202.98 KiB |
|  |  | jtext | 453 | N/A | 182.30 KiB |
|  |  | sql | 653 | N/A | 256.00 KiB |
| jac313_store_006_XS | off | binary | 402 | N/A | 202.98 KiB |
|  |  | jtext | 402 | N/A | 179.37 KiB |
|  |  | sql | 352 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 453 | N/A | 202.98 KiB |
|  |  | jtext | 453 | N/A | 179.37 KiB |
|  |  | sql | 603 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 101 | 177,305 | 202.98 KiB |
|  |  | jtext | 101 | 193,125 | 182.30 KiB |
|  |  | sql | 50 | 181,554 | 256.00 KiB |
| jac313_store_007_TS | on | binary | 101 | 184,094 | 202.98 KiB |
|  |  | jtext | 101 | 184,094 | 182.30 KiB |
|  |  | sql | 251 | 184,298 | 256.00 KiB |
| jac313_store_007_XS | off | binary | 101 | 185,874 | 202.98 KiB |
|  |  | jtext | 101 | 179,533 | 179.37 KiB |
|  |  | sql | 51 | 177,274 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 100 | 187,617 | 202.98 KiB |
|  |  | jtext | 101 | 178,444 | 179.37 KiB |
|  |  | sql | 251 | 203,791 | 248.00 KiB |

## Persist backend comparison (all matrix tests)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_001_TS | off | binary | 101 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_001_TS | on | binary | 101 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 301 | - | 52.00 KiB |
| jac313_store_001_XS | off | binary | 101 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.30 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_001_XS | on | binary | 101 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.30 KiB |
|  |  | sql | 301 | - | 52.00 KiB |
| jac313_store_002_TS | off | binary | 100 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_002_TS | on | binary | 101 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 201 | - | 52.00 KiB |
| jac313_store_002_XS | off | binary | 101 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.30 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_002_XS | on | binary | 101 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.30 KiB |
|  |  | sql | 201 | - | 52.00 KiB |
| jac313_store_003_TS | off | binary | 402 | - | 202.98 KiB |
|  |  | jtext | 402 | - | 182.30 KiB |
|  |  | sql | 353 | - | 256.00 KiB |
| jac313_store_003_TS | on | binary | 453 | - | 202.98 KiB |
|  |  | jtext | 453 | - | 182.30 KiB |
|  |  | sql | 553 | - | 256.00 KiB |
| jac313_store_003_XS | off | binary | 402 | - | 202.98 KiB |
|  |  | jtext | 403 | - | 179.37 KiB |
|  |  | sql | 352 | - | 248.00 KiB |
| jac313_store_003_XS | on | binary | 402 | - | 202.98 KiB |
|  |  | jtext | 453 | - | 179.37 KiB |
|  |  | sql | 553 | - | 248.00 KiB |
| jac313_store_004_TS | off | binary | 101 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_004_TS | on | binary | 101 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.60 KiB |
|  |  | sql | 251 | - | 52.00 KiB |
| jac313_store_004_XS | off | binary | 101 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.30 KiB |
|  |  | sql | 50 | - | 52.00 KiB |
| jac313_store_004_XS | on | binary | 101 | - | 20.39 KiB |
|  |  | jtext | 50 | - | 20.30 KiB |
|  |  | sql | 201 | - | 52.00 KiB |
| jac313_store_005_TS | off | binary | 101 | 181,455 | 85.79 KiB |
|  |  | jtext | 50 | 171,733 | 76.71 KiB |
|  |  | sql | 50 | 172,384 | 84.00 KiB |
| jac313_store_005_TS | on | binary | 101 | 221,779 | 85.79 KiB |
|  |  | jtext | 50 | 164,663 | 76.71 KiB |
|  |  | sql | 151 | 176,647 | 84.00 KiB |
| jac313_store_005_XS | off | binary | 101 | 169,492 | 85.79 KiB |
|  |  | jtext | 50 | 150,331 | 73.78 KiB |
|  |  | sql | 51 | 191,461 | 84.00 KiB |
| jac313_store_005_XS | on | binary | 101 | 174,307 | 85.79 KiB |
|  |  | jtext | 50 | 177,022 | 73.78 KiB |
|  |  | sql | 151 | 186,811 | 84.00 KiB |
| jac313_store_006_TS | off | binary | 403 | N/A | 202.98 KiB |
|  |  | jtext | 402 | N/A | 182.30 KiB |
|  |  | sql | 352 | N/A | 256.00 KiB |
| jac313_store_006_TS | on | binary | 453 | N/A | 202.98 KiB |
|  |  | jtext | 453 | N/A | 182.30 KiB |
|  |  | sql | 653 | N/A | 256.00 KiB |
| jac313_store_006_XS | off | binary | 402 | N/A | 202.98 KiB |
|  |  | jtext | 402 | N/A | 179.37 KiB |
|  |  | sql | 352 | N/A | 248.00 KiB |
| jac313_store_006_XS | on | binary | 453 | N/A | 202.98 KiB |
|  |  | jtext | 453 | N/A | 179.37 KiB |
|  |  | sql | 603 | N/A | 248.00 KiB |
| jac313_store_007_TS | off | binary | 101 | 177,305 | 202.98 KiB |
|  |  | jtext | 101 | 193,125 | 182.30 KiB |
|  |  | sql | 50 | 181,554 | 256.00 KiB |
| jac313_store_007_TS | on | binary | 101 | 184,094 | 202.98 KiB |
|  |  | jtext | 101 | 184,094 | 182.30 KiB |
|  |  | sql | 251 | 184,298 | 256.00 KiB |
| jac313_store_007_XS | off | binary | 101 | 185,874 | 202.98 KiB |
|  |  | jtext | 101 | 179,533 | 179.37 KiB |
|  |  | sql | 51 | 177,274 | 248.00 KiB |
| jac313_store_007_XS | on | binary | 100 | 187,617 | 202.98 KiB |
|  |  | jtext | 101 | 178,444 | 179.37 KiB |
|  |  | sql | 251 | 203,791 | 248.00 KiB |

## Scenarios

_Per-test compile times are stored in the results database (not log files)._ 

| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |
|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|
| jac313_store_001_TS | store | matrix | binary | off | pass | 101 | - | 20.39 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_off.log) |
| jac313_store_001_TS | store | matrix | binary | on | pass | 101 | - | 20.39 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_TS/gcc16_binary_on.log) |
| jac313_store_001_TS | store | matrix | jtext | off | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_off.log) |
| jac313_store_001_TS | store | matrix | jtext | on | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_TS/gcc16_jtext_on.log) |
| jac313_store_001_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_off.log) |
| jac313_store_001_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_TS/gcc16_none_on.log) |
| jac313_store_001_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_off.log) |
| jac313_store_001_TS | store | matrix | sql | on | pass | 301 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_TS/gcc16_sql_on.log) |
| jac313_store_001_XS | store | matrix | binary | off | pass | 101 | - | 20.39 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_off.log) |
| jac313_store_001_XS | store | matrix | binary | on | pass | 101 | - | 20.39 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_001_XS/gcc16_binary_on.log) |
| jac313_store_001_XS | store | matrix | jtext | off | pass | 50 | - | 20.30 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_off.log) |
| jac313_store_001_XS | store | matrix | jtext | on | pass | 50 | - | 20.30 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_001_XS/gcc16_jtext_on.log) |
| jac313_store_001_XS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_off.log) |
| jac313_store_001_XS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_001_XS/gcc16_none_on.log) |
| jac313_store_001_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_off.log) |
| jac313_store_001_XS | store | matrix | sql | on | pass | 301 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_001_XS/gcc16_sql_on.log) |
| jac313_store_002_TS | store | matrix | binary | off | pass | 100 | - | 20.39 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_off.log) |
| jac313_store_002_TS | store | matrix | binary | on | pass | 101 | - | 20.39 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_TS/gcc16_binary_on.log) |
| jac313_store_002_TS | store | matrix | jtext | off | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_off.log) |
| jac313_store_002_TS | store | matrix | jtext | on | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_TS/gcc16_jtext_on.log) |
| jac313_store_002_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_off.log) |
| jac313_store_002_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_TS/gcc16_none_on.log) |
| jac313_store_002_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_off.log) |
| jac313_store_002_TS | store | matrix | sql | on | pass | 201 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_TS/gcc16_sql_on.log) |
| jac313_store_002_XS | store | matrix | binary | off | pass | 101 | - | 20.39 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_off.log) |
| jac313_store_002_XS | store | matrix | binary | on | pass | 101 | - | 20.39 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_002_XS/gcc16_binary_on.log) |
| jac313_store_002_XS | store | matrix | jtext | off | pass | 50 | - | 20.30 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_off.log) |
| jac313_store_002_XS | store | matrix | jtext | on | pass | 50 | - | 20.30 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_002_XS/gcc16_jtext_on.log) |
| jac313_store_002_XS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_off.log) |
| jac313_store_002_XS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_002_XS/gcc16_none_on.log) |
| jac313_store_002_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_off.log) |
| jac313_store_002_XS | store | matrix | sql | on | pass | 201 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_002_XS/gcc16_sql_on.log) |
| jac313_store_003_TS | store | matrix | binary | off | pass | 402 | - | 202.98 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_off.log) |
| jac313_store_003_TS | store | matrix | binary | on | pass | 453 | - | 202.98 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_TS/gcc16_binary_on.log) |
| jac313_store_003_TS | store | matrix | jtext | off | pass | 402 | - | 182.30 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_off.log) |
| jac313_store_003_TS | store | matrix | jtext | on | pass | 453 | - | 182.30 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_TS/gcc16_jtext_on.log) |
| jac313_store_003_TS | store | matrix | none | off | pass | 352 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_off.log) |
| jac313_store_003_TS | store | matrix | none | on | pass | 402 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_TS/gcc16_none_on.log) |
| jac313_store_003_TS | store | matrix | sql | off | pass | 353 | - | 256.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_off.log) |
| jac313_store_003_TS | store | matrix | sql | on | pass | 553 | - | 256.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_TS/gcc16_sql_on.log) |
| jac313_store_003_XS | store | matrix | binary | off | pass | 402 | - | 202.98 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_off.log) |
| jac313_store_003_XS | store | matrix | binary | on | pass | 402 | - | 202.98 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_003_XS/gcc16_binary_on.log) |
| jac313_store_003_XS | store | matrix | jtext | off | pass | 403 | - | 179.37 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_off.log) |
| jac313_store_003_XS | store | matrix | jtext | on | pass | 453 | - | 179.37 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_003_XS/gcc16_jtext_on.log) |
| jac313_store_003_XS | store | matrix | none | off | pass | 352 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_off.log) |
| jac313_store_003_XS | store | matrix | none | on | pass | 402 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_003_XS/gcc16_none_on.log) |
| jac313_store_003_XS | store | matrix | sql | off | pass | 352 | - | 248.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_off.log) |
| jac313_store_003_XS | store | matrix | sql | on | pass | 553 | - | 248.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_003_XS/gcc16_sql_on.log) |
| jac313_store_004_TS | store | matrix | binary | off | pass | 101 | - | 20.39 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_off.log) |
| jac313_store_004_TS | store | matrix | binary | on | pass | 101 | - | 20.39 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_TS/gcc16_binary_on.log) |
| jac313_store_004_TS | store | matrix | jtext | off | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_off.log) |
| jac313_store_004_TS | store | matrix | jtext | on | pass | 50 | - | 20.60 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_TS/gcc16_jtext_on.log) |
| jac313_store_004_TS | store | matrix | none | off | pass | 50 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_off.log) |
| jac313_store_004_TS | store | matrix | none | on | pass | 50 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_TS/gcc16_none_on.log) |
| jac313_store_004_TS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_off.log) |
| jac313_store_004_TS | store | matrix | sql | on | pass | 251 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_TS/gcc16_sql_on.log) |
| jac313_store_004_XS | store | matrix | binary | off | pass | 101 | - | 20.39 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_off.log) |
| jac313_store_004_XS | store | matrix | binary | on | pass | 101 | - | 20.39 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_004_XS/gcc16_binary_on.log) |
| jac313_store_004_XS | store | matrix | jtext | off | pass | 50 | - | 20.30 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_off.log) |
| jac313_store_004_XS | store | matrix | jtext | on | pass | 50 | - | 20.30 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_004_XS/gcc16_jtext_on.log) |
| jac313_store_004_XS | store | matrix | none | off | pass | 51 | - | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_off.log) |
| jac313_store_004_XS | store | matrix | none | on | pass | 51 | - | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_004_XS/gcc16_none_on.log) |
| jac313_store_004_XS | store | matrix | sql | off | pass | 50 | - | 52.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_off.log) |
| jac313_store_004_XS | store | matrix | sql | on | pass | 201 | - | 52.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_004_XS/gcc16_sql_on.log) |
| jac313_store_005_TS | store | matrix | binary | off | pass | 101 | 181,455 | 85.79 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_off.log) |
| jac313_store_005_TS | store | matrix | binary | on | pass | 101 | 221,779 | 85.79 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_TS/gcc16_binary_on.log) |
| jac313_store_005_TS | store | matrix | jtext | off | pass | 50 | 171,733 | 76.71 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_off.log) |
| jac313_store_005_TS | store | matrix | jtext | on | pass | 50 | 164,663 | 76.71 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_TS/gcc16_jtext_on.log) |
| jac313_store_005_TS | store | matrix | none | off | pass | 50 | 483,559 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_off.log) |
| jac313_store_005_TS | store | matrix | none | on | pass | 50 | 422,297 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_TS/gcc16_none_on.log) |
| jac313_store_005_TS | store | matrix | sql | off | pass | 50 | 172,384 | 84.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_off.log) |
| jac313_store_005_TS | store | matrix | sql | on | pass | 151 | 176,647 | 84.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_TS/gcc16_sql_on.log) |
| jac313_store_005_XS | store | matrix | binary | off | pass | 101 | 169,492 | 85.79 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_off.log) |
| jac313_store_005_XS | store | matrix | binary | on | pass | 101 | 174,307 | 85.79 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_005_XS/gcc16_binary_on.log) |
| jac313_store_005_XS | store | matrix | jtext | off | pass | 50 | 150,331 | 73.78 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_off.log) |
| jac313_store_005_XS | store | matrix | jtext | on | pass | 50 | 177,022 | 73.78 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_005_XS/gcc16_jtext_on.log) |
| jac313_store_005_XS | store | matrix | none | off | pass | 50 | 511,247 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_off.log) |
| jac313_store_005_XS | store | matrix | none | on | pass | 50 | 379,651 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_005_XS/gcc16_none_on.log) |
| jac313_store_005_XS | store | matrix | sql | off | pass | 51 | 191,461 | 84.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_off.log) |
| jac313_store_005_XS | store | matrix | sql | on | pass | 151 | 186,811 | 84.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_005_XS/gcc16_sql_on.log) |
| jac313_store_006_TS | store | matrix | binary | off | pass | 403 | N/A | 202.98 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_off.log) |
| jac313_store_006_TS | store | matrix | binary | on | pass | 453 | N/A | 202.98 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_TS/gcc16_binary_on.log) |
| jac313_store_006_TS | store | matrix | jtext | off | pass | 402 | N/A | 182.30 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_off.log) |
| jac313_store_006_TS | store | matrix | jtext | on | pass | 453 | N/A | 182.30 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_TS/gcc16_jtext_on.log) |
| jac313_store_006_TS | store | matrix | none | off | pass | 352 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_off.log) |
| jac313_store_006_TS | store | matrix | none | on | pass | 402 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_TS/gcc16_none_on.log) |
| jac313_store_006_TS | store | matrix | sql | off | pass | 352 | N/A | 256.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_off.log) |
| jac313_store_006_TS | store | matrix | sql | on | pass | 653 | N/A | 256.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_TS/gcc16_sql_on.log) |
| jac313_store_006_XS | store | matrix | binary | off | pass | 402 | N/A | 202.98 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_off.log) |
| jac313_store_006_XS | store | matrix | binary | on | pass | 453 | N/A | 202.98 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_006_XS/gcc16_binary_on.log) |
| jac313_store_006_XS | store | matrix | jtext | off | pass | 402 | N/A | 179.37 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_off.log) |
| jac313_store_006_XS | store | matrix | jtext | on | pass | 453 | N/A | 179.37 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_006_XS/gcc16_jtext_on.log) |
| jac313_store_006_XS | store | matrix | none | off | pass | 352 | N/A | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_off.log) |
| jac313_store_006_XS | store | matrix | none | on | pass | 402 | N/A | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_006_XS/gcc16_none_on.log) |
| jac313_store_006_XS | store | matrix | sql | off | pass | 352 | N/A | 248.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_off.log) |
| jac313_store_006_XS | store | matrix | sql | on | pass | 603 | N/A | 248.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_006_XS/gcc16_sql_on.log) |
| jac313_store_007_TS | store | matrix | binary | off | pass | 101 | 177,305 | 202.98 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_off.log) |
| jac313_store_007_TS | store | matrix | binary | on | pass | 101 | 184,094 | 202.98 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_TS/gcc16_binary_on.log) |
| jac313_store_007_TS | store | matrix | jtext | off | pass | 101 | 193,125 | 182.30 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_off.log) |
| jac313_store_007_TS | store | matrix | jtext | on | pass | 101 | 184,094 | 182.30 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_TS/gcc16_jtext_on.log) |
| jac313_store_007_TS | store | matrix | none | off | pass | 50 | 421,585 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_off.log) |
| jac313_store_007_TS | store | matrix | none | on | pass | 50 | 392,157 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_TS/gcc16_none_on.log) |
| jac313_store_007_TS | store | matrix | sql | off | pass | 50 | 181,554 | 256.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_off.log) |
| jac313_store_007_TS | store | matrix | sql | on | pass | 251 | 184,298 | 256.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_TS/gcc16_sql_on.log) |
| jac313_store_007_XS | store | matrix | binary | off | pass | 101 | 185,874 | 202.98 KiB | - | [gcc16_binary_off.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_off.log) |
| jac313_store_007_XS | store | matrix | binary | on | pass | 100 | 187,617 | 202.98 KiB | - | [gcc16_binary_on.log](binary_logs/JAC313_STORE_TEST_007_XS/gcc16_binary_on.log) |
| jac313_store_007_XS | store | matrix | jtext | off | pass | 101 | 179,533 | 179.37 KiB | - | [gcc16_jtext_off.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_off.log) |
| jac313_store_007_XS | store | matrix | jtext | on | pass | 101 | 178,444 | 179.37 KiB | - | [gcc16_jtext_on.log](jText_logs/JAC313_STORE_TEST_007_XS/gcc16_jtext_on.log) |
| jac313_store_007_XS | store | matrix | none | off | pass | 50 | 424,088 | - | - | [gcc16_none_off.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_off.log) |
| jac313_store_007_XS | store | matrix | none | on | pass | 51 | 454,133 | - | - | [gcc16_none_on.log](inmem_logs/JAC313_STORE_TEST_007_XS/gcc16_none_on.log) |
| jac313_store_007_XS | store | matrix | sql | off | pass | 51 | 177,274 | 248.00 KiB | - | [gcc16_sql_off.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_off.log) |
| jac313_store_007_XS | store | matrix | sql | on | pass | 251 | 203,791 | 248.00 KiB | - | [gcc16_sql_on.log](sql_logs/JAC313_STORE_TEST_007_XS/gcc16_sql_on.log) |
| jac313_store_008_TS | store | matrix | flags | off | pass | 251 | 196,040 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_TS/gcc16_flags_off.log) |
| jac313_store_008_XS | store | matrix | flags | off | pass | 251 | 168,549 | - | - | [gcc16_flags_off.log](flags_logs/JAC313_STORE_TEST_008_XS/gcc16_flags_off.log) |
| jac313_store_flags | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_flags/gcc16_unit_off.log) |
| jac313_store_metric_view | store | matrix | unit | off | pass | 50 | - | - | - | [gcc16_unit_off.log](unit_logs/JAC313_STORE_TEST_metric_view/gcc16_unit_off.log) |
