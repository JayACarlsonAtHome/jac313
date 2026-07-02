<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# verify — by machine

_Generated from `results.db`. Each machine (`jac313-###`) is its own section. Columns are that host's compilers (latest run each); cell = ms (pass) or status._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k

| scenario | [clang21](Run_010.md) | [gcc15](Run_009.md) |
|---|--:|--:|
| jac313_store_001_TS · binary · drd | pass · 602 | pass · 603 |
| jac313_store_001_TS · binary · helgrind | pass · 601 | pass · 651 |
| jac313_store_001_TS · binary · memcheck | pass · 801 | pass · 702 |
| jac313_store_001_TS · inmem · memcheck | pass · 853 | pass · 852 |
| jac313_store_001_TS · jtext · drd | pass · 702 | pass · 702 |
| jac313_store_001_TS · jtext · helgrind | pass · 702 | pass · 702 |
| jac313_store_001_TS · jtext · memcheck | pass · 902 | pass · 852 |
| jac313_store_001_TS · none · memcheck | pass · 602 | pass · 601 |
| jac313_store_001_TS · sql · drd | pass · 602 | pass · 652 |
| jac313_store_001_TS · sql · helgrind | pass · 1,003 | pass · 903 |
| jac313_store_001_TS · sql · memcheck | pass · 1,154 | pass · 1,102 |
| jac313_store_002_TS · binary · memcheck | pass · 602 | pass · 601 |
| jac313_store_002_TS · inmem · memcheck | pass · 752 | pass · 701 |
| jac313_store_002_TS · jtext · memcheck | pass · 752 | pass · 702 |
| jac313_store_002_TS · none · memcheck | pass · 451 | pass · 451 |
| jac313_store_002_TS · sql · memcheck | pass · 953 | pass · 953 |
| jac313_store_003_TS · binary · memcheck | pass · 752 | pass · 752 |
| jac313_store_003_TS · inmem · memcheck | pass · 902 | pass · 852 |
| jac313_store_003_TS · jtext · memcheck | pass · 902 | pass · 903 |
| jac313_store_003_TS · none · drd | pass · 501 | pass · 552 |
| jac313_store_003_TS · none · helgrind | pass · 501 | pass · 551 |
| jac313_store_003_TS · none · memcheck | pass · 601 | pass · 601 |
| jac313_store_003_TS · sql · memcheck | pass · 1,155 | pass · 1,455 |
| jac313_store_004_TS · binary · memcheck | pass · 802 | pass · 803 |
| jac313_store_004_TS · inmem · memcheck | pass · 1,254 | pass · 852 |
| jac313_store_004_TS · jtext · memcheck | pass · 903 | pass · 852 |
| jac313_store_004_TS · none · memcheck | pass · 602 | pass · 601 |
| jac313_store_004_TS · sql · memcheck | pass · 1,152 | pass · 1,555 |
| jac313_store_005_TS · binary · memcheck | pass · 601 | pass · 601 |
| jac313_store_005_TS · inmem · memcheck | pass · 602 | pass · 651 |
| jac313_store_005_TS · jtext · memcheck | pass · 702 | pass · 651 |
| jac313_store_005_TS · none · memcheck | pass · 401 | pass · 451 |
| jac313_store_005_TS · sql · memcheck | pass · 801 | pass · 1,304 |
| jac313_store_006_TS · binary · memcheck | pass · 2,157 | pass · 752 |
| jac313_store_006_TS · inmem · memcheck | pass · 902 | pass · 902 |
| jac313_store_006_TS · jtext · memcheck | pass · 1,002 | pass · 852 |
| jac313_store_006_TS · none · drd | pass · 501 | pass · 502 |
| jac313_store_006_TS · none · helgrind | pass · 501 | pass · 501 |
| jac313_store_006_TS · none · memcheck | pass · 652 | pass · 601 |
| jac313_store_006_TS · sql · memcheck | pass · 1,153 | pass · 2,409 |
| jac313_store_007_TS · binary · memcheck | pass · 601 | pass · 652 |
| jac313_store_007_TS · inmem · memcheck | pass · 703 | pass · 702 |
| jac313_store_007_TS · jtext · memcheck | pass · 701 | pass · 752 |
| jac313_store_007_TS · none · memcheck | pass · 451 | pass · 451 |
| jac313_store_007_TS · sql · memcheck | pass · 953 | pass · 953 |
| jac313_store_008_TS · binary · memcheck | pass · 1,053 | pass · 1,103 |
| jac313_store_008_TS · inmem · memcheck | pass · 1,153 | pass · 1,052 |
| jac313_store_008_TS · jtext · memcheck | pass · 1,153 | pass · 1,103 |
| jac313_store_008_TS · none · memcheck | pass · 1,154 | pass · 1,103 |
| jac313_store_008_TS · sql · memcheck | pass · 1,153 | pass · 1,053 |
| jac313_store_binary_persist_smoke · memcheck | pass · 351 | pass · 351 |
| jac313_store_binary_reader_test · memcheck | pass · 551 | pass · 501 |
| jac313_store_binary_to_jtext_smoke · memcheck | pass · 551 | pass · 501 |
| jac313_store_flag_routing_smoke · memcheck | pass · 952 | pass · 903 |
| jac313_store_flags · memcheck | pass · 250 | pass · 251 |
| jac313_store_jtext_persist_smoke · memcheck | pass · 502 | pass · 451 |
| jac313_store_metric_view · memcheck | pass · 351 | pass · 351 |
| jac313_store_sql_persist_smoke · memcheck | pass · 753 | pass · 803 |
| jac313_store_v001_example · memcheck | pass · 250 | pass · 251 |
| jac313_store_v001_test · memcheck | pass · 250 | pass · 300 |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_010](Run_010.md) | clang21 · 2026-07-01T15:50:49Z |
| [Run_009](Run_009.md) | gcc15 · 2026-07-01T15:49:35Z |

## jac313-002 — Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz · 8 cores · 31 GB · Linux Mint 22.3 · ssd

| scenario | [clang20](Run_034.md) | [gcc15](Run_033.md) |
|---|--:|--:|
| jac313_store_001_TS · binary · drd | pass · 1,152 | pass · 1,202 |
| jac313_store_001_TS · binary · helgrind | pass · 1,152 | pass · 1,152 |
| jac313_store_001_TS · binary · memcheck | pass · 1,503 | pass · 1,553 |
| jac313_store_001_TS · inmem · memcheck | pass · 1,754 | pass · 1,753 |
| jac313_store_001_TS · jtext · drd | pass · 1,352 | pass · 1,403 |
| jac313_store_001_TS · jtext · helgrind | pass · 1,302 | pass · 1,303 |
| jac313_store_001_TS · jtext · memcheck | pass · 1,753 | pass · 1,753 |
| jac313_store_001_TS · none · memcheck | pass · 1,353 | pass · 1,352 |
| jac313_store_001_TS · sql · drd | pass · 1,453 | pass · 1,453 |
| jac313_store_001_TS · sql · helgrind | pass · 1,554 | pass · 1,553 |
| jac313_store_001_TS · sql · memcheck | pass · 2,004 | pass · 2,004 |
| jac313_store_002_TS · binary · memcheck | pass · 1,253 | pass · 1,252 |
| jac313_store_002_TS · inmem · memcheck | pass · 1,503 | pass · 1,503 |
| jac313_store_002_TS · jtext · memcheck | pass · 1,553 | pass · 1,503 |
| jac313_store_002_TS · none · memcheck | pass · 1,103 | pass · 1,102 |
| jac313_store_002_TS · sql · memcheck | pass · 2,155 | pass · 1,754 |
| jac313_store_003_TS · binary · memcheck | pass · 1,603 | pass · 1,553 |
| jac313_store_003_TS · inmem · memcheck | pass · 1,803 | pass · 1,803 |
| jac313_store_003_TS · jtext · memcheck | pass · 1,854 | pass · 1,804 |
| jac313_store_003_TS · none · drd | pass · 1,152 | pass · 1,153 |
| jac313_store_003_TS · none · helgrind | pass · 1,053 | pass · 1,052 |
| jac313_store_003_TS · none · memcheck | pass · 1,502 | pass · 1,402 |
| jac313_store_003_TS · sql · memcheck | pass · 2,104 | pass · 2,104 |
| jac313_store_004_TS · binary · memcheck | pass · 1,553 | pass · 1,553 |
| jac313_store_004_TS · inmem · memcheck | pass · 1,803 | pass · 1,804 |
| jac313_store_004_TS · jtext · memcheck | pass · 1,803 | pass · 1,804 |
| jac313_store_004_TS · none · memcheck | pass · 1,402 | pass · 1,403 |
| jac313_store_004_TS · sql · memcheck | pass · 2,053 | pass · 2,054 |
| jac313_store_005_TS · binary · memcheck | pass · 1,202 | pass · 1,253 |
| jac313_store_005_TS · inmem · memcheck | pass · 1,352 | pass · 1,403 |
| jac313_store_005_TS · jtext · memcheck | pass · 1,352 | pass · 1,403 |
| jac313_store_005_TS · none · memcheck | pass · 1,051 | pass · 1,103 |
| jac313_store_005_TS · sql · memcheck | pass · 1,702 | pass · 1,703 |
| jac313_store_006_TS · binary · memcheck | pass · 1,603 | pass · 1,553 |
| jac313_store_006_TS · inmem · memcheck | pass · 1,803 | pass · 1,803 |
| jac313_store_006_TS · jtext · memcheck | pass · 1,802 | pass · 1,803 |
| jac313_store_006_TS · none · drd | pass · 1,153 | pass · 1,153 |
| jac313_store_006_TS · none · helgrind | pass · 1,053 | pass · 1,052 |
| jac313_store_006_TS · none · memcheck | pass · 1,402 | pass · 1,403 |
| jac313_store_006_TS · sql · memcheck | pass · 2,053 | pass · 2,054 |
| jac313_store_007_TS · binary · memcheck | pass · 1,252 | pass · 1,252 |
| jac313_store_007_TS · inmem · memcheck | pass · 1,453 | pass · 1,503 |
| jac313_store_007_TS · jtext · memcheck | pass · 1,503 | pass · 1,503 |
| jac313_store_007_TS · none · memcheck | pass · 1,052 | pass · 1,103 |
| jac313_store_007_TS · sql · memcheck | pass · 1,754 | pass · 1,753 |
| jac313_store_008_TS · binary · memcheck | pass · 1,905 | pass · 1,904 |
| jac313_store_008_TS · inmem · memcheck | pass · 1,905 | pass · 1,904 |
| jac313_store_008_TS · jtext · memcheck | pass · 1,904 | pass · 1,955 |
| jac313_store_008_TS · none · memcheck | pass · 1,904 | pass · 1,904 |
| jac313_store_008_TS · sql · memcheck | pass · 1,904 | pass · 1,904 |
| jac313_store_binary_persist_smoke · memcheck | pass · 852 | pass · 851 |
| jac313_store_binary_reader_test · memcheck | pass · 1,002 | pass · 1,052 |
| jac313_store_binary_to_jtext_smoke · memcheck | pass · 1,003 | pass · 1,052 |
| jac313_store_flag_routing_smoke · memcheck | pass · 1,452 | pass · 1,453 |
| jac313_store_flags · memcheck | pass · 651 | pass · 651 |
| jac313_store_jtext_persist_smoke · memcheck | pass · 953 | pass · 952 |
| jac313_store_metric_view · memcheck | pass · 801 | pass · 802 |
| jac313_store_sql_persist_smoke · memcheck | pass · 1,303 | pass · 1,303 |
| jac313_store_v001_example · memcheck | pass · 651 | pass · 651 |
| jac313_store_v001_test · memcheck | pass · 651 | pass · 651 |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_034](Run_034.md) | clang20 · 2026-07-02T08:05:17Z |
| [Run_033](Run_033.md) | gcc15 · 2026-07-02T08:03:10Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
