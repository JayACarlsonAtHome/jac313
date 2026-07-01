<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# verify — by machine

_Generated from `results.db`. Each machine (`jac313-###`) is its own section. Columns are that host's compilers (latest run each); cell = ms (pass) or status._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 19 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k

| scenario | [clang21](Run_047.md) | [gcc15](Run_046.md) |
|---|--:|--:|
| jac313_store_001_TS · binary · drd | pass · 552 | pass · 551 |
| jac313_store_001_TS · binary · helgrind | pass · 601 | pass · 652 |
| jac313_store_001_TS · binary · memcheck | pass · 752 | pass · 701 |
| jac313_store_001_TS · inmem · memcheck | pass · 952 | pass · 902 |
| jac313_store_001_TS · jtext · drd | pass · 702 | pass · 702 |
| jac313_store_001_TS · jtext · helgrind | pass · 801 | pass · 752 |
| jac313_store_001_TS · jtext · memcheck | pass · 953 | pass · 902 |
| jac313_store_001_TS · none · memcheck | pass · 601 | pass · 601 |
| jac313_store_001_TS · sql · drd | pass · 651 | pass · 652 |
| jac313_store_001_TS · sql · helgrind | pass · 1,202 | pass · 1,152 |
| jac313_store_001_TS · sql · memcheck | pass · 1,304 | pass · 1,253 |
| jac313_store_002_TS · binary · memcheck | pass · 551 | pass · 551 |
| jac313_store_002_TS · inmem · memcheck | pass · 802 | pass · 802 |
| jac313_store_002_TS · jtext · memcheck | pass · 852 | pass · 851 |
| jac313_store_002_TS · none · memcheck | pass · 451 | pass · 451 |
| jac313_store_002_TS · sql · memcheck | pass · 1,153 | pass · 1,052 |
| jac313_store_003_TS · binary · memcheck | pass · 754 | pass · 752 |
| jac313_store_003_TS · inmem · memcheck | pass · 953 | pass · 953 |
| jac313_store_003_TS · jtext · memcheck | pass · 953 | pass · 902 |
| jac313_store_003_TS · none · drd | pass · 502 | pass · 502 |
| jac313_store_003_TS · none · helgrind | pass · 551 | pass · 501 |
| jac313_store_003_TS · none · memcheck | pass · 652 | pass · 601 |
| jac313_store_003_TS · sql · memcheck | pass · 1,353 | pass · 1,204 |
| jac313_store_004_TS · binary · memcheck | pass · 701 | pass · 753 |
| jac313_store_004_TS · inmem · memcheck | pass · 952 | pass · 952 |
| jac313_store_004_TS · jtext · memcheck | pass · 1,704 | pass · 902 |
| jac313_store_004_TS · none · memcheck | pass · 651 | pass · 651 |
| jac313_store_004_TS · sql · memcheck | pass · 1,253 | pass · 1,253 |
| jac313_store_005_TS · binary · memcheck | pass · 602 | pass · 602 |
| jac313_store_005_TS · inmem · memcheck | pass · 651 | pass · 1,152 |
| jac313_store_005_TS · jtext · memcheck | pass · 702 | pass · 651 |
| jac313_store_005_TS · none · memcheck | pass · 451 | pass · 450 |
| jac313_store_005_TS · sql · memcheck | pass · 852 | pass · 903 |
| jac313_store_006_TS · binary · memcheck | pass · 752 | pass · 751 |
| jac313_store_006_TS · inmem · memcheck | pass · 952 | pass · 951 |
| jac313_store_006_TS · jtext · memcheck | pass · 1,003 | pass · 902 |
| jac313_store_006_TS · none · drd | pass · 501 | pass · 501 |
| jac313_store_006_TS · none · helgrind | pass · 551 | pass · 501 |
| jac313_store_006_TS · none · memcheck | pass · 652 | pass · 601 |
| jac313_store_006_TS · sql · memcheck | pass · 1,253 | pass · 1,202 |
| jac313_store_007_TS · binary · memcheck | pass · 551 | pass · 601 |
| jac313_store_007_TS · inmem · memcheck | pass · 802 | pass · 852 |
| jac313_store_007_TS · jtext · memcheck | pass · 902 | pass · 751 |
| jac313_store_007_TS · none · memcheck | pass · 451 | pass · 451 |
| jac313_store_007_TS · sql · memcheck | pass · 1,053 | pass · 1,052 |
| jac313_store_008_TS · binary · memcheck | pass · 1,203 | pass · 1,303 |
| jac313_store_008_TS · inmem · memcheck | pass · 1,403 | pass · 1,254 |
| jac313_store_008_TS · jtext · memcheck | pass · 1,555 | pass · 1,202 |
| jac313_store_008_TS · none · memcheck | pass · 1,303 | pass · 1,253 |
| jac313_store_008_TS · sql · memcheck | pass · 1,253 | pass · 1,252 |
| jac313_store_binary_persist_smoke · memcheck | pass · 401 | pass · 401 |
| jac313_store_binary_reader_test · memcheck | pass · 702 | pass · 652 |
| jac313_store_binary_to_jtext_smoke · memcheck | pass · 601 | pass · 651 |
| jac313_store_flag_routing_smoke · memcheck | pass · 903 | pass · 902 |
| jac313_store_flags · memcheck | pass · 301 | pass · 300 |
| jac313_store_jtext_persist_smoke · memcheck | pass · 551 | pass · 551 |
| jac313_store_metric_view · memcheck | pass · 351 | pass · 351 |
| jac313_store_sql_persist_smoke · memcheck | pass · 803 | pass · 802 |
| jac313_store_v002_example · memcheck | pass · 250 | pass · 250 |
| jac313_store_v002_test · memcheck | pass · 250 | pass · 251 |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_047](Run_047.md) | clang21 · 2026-07-01T07:18:35Z |
| [Run_046](Run_046.md) | gcc15 · 2026-07-01T07:17:28Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
