# verify — by machine

_Generated from `results.db`. Each machine (`jac313-###`) is its own section. Columns are that host's compilers (latest run each); cell = ms (pass) or status._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 12 cores · 23 GB · Fedora Linux 44 (Workstation Edition)

| scenario | [clang22](Run_008.md) | [gcc16](Run_007.md) |
|---|--:|--:|
| jac313_store_001_TS · binary · drd | pass · 953 | pass · 953 |
| jac313_store_001_TS · binary · helgrind | pass · 1,303 | pass · 1,254 |
| jac313_store_001_TS · binary · memcheck | pass · 1,053 | pass · 1,053 |
| jac313_store_001_TS · inmem · memcheck | pass · 1,153 | pass · 1,155 |
| jac313_store_001_TS · jtext · drd | pass · 1,053 | pass · 1,053 |
| jac313_store_001_TS · jtext · helgrind | pass · 1,304 | pass · 1,304 |
| jac313_store_001_TS · jtext · memcheck | pass · 1,153 | pass · 1,153 |
| jac313_store_001_TS · none · memcheck | pass · 952 | pass · 953 |
| jac313_store_001_TS · sql · drd | pass · 1,103 | pass · 1,102 |
| jac313_store_001_TS · sql · helgrind | pass · 1,453 | pass · 1,454 |
| jac313_store_001_TS · sql · memcheck | pass · 1,253 | pass · 1,253 |
| jac313_store_002_TS · binary · memcheck | pass · 902 | pass · 902 |
| jac313_store_002_TS · inmem · memcheck | pass · 1,003 | pass · 1,003 |
| jac313_store_002_TS · jtext · memcheck | pass · 1,003 | pass · 1,053 |
| jac313_store_002_TS · none · memcheck | pass · 802 | pass · 802 |
| jac313_store_002_TS · sql · memcheck | pass · 1,153 | pass · 1,103 |
| jac313_store_003_TS · binary · memcheck | pass · 1,053 | pass · 1,053 |
| jac313_store_003_TS · inmem · memcheck | pass · 1,152 | pass · 1,204 |
| jac313_store_003_TS · jtext · memcheck | pass · 1,153 | pass · 1,153 |
| jac313_store_003_TS · none · drd | pass · 902 | pass · 1,102 |
| jac313_store_003_TS · none · helgrind | pass · 1,205 | pass · 3,160 |
| jac313_store_003_TS · none · memcheck | pass · 1,001 | pass · 953 |
| jac313_store_003_TS · sql · memcheck | pass · 1,304 | pass · 1,254 |
| jac313_store_004_TS · binary · memcheck | pass · 1,053 | pass · 1,054 |
| jac313_store_004_TS · inmem · memcheck | pass · 1,153 | pass · 1,153 |
| jac313_store_004_TS · jtext · memcheck | pass · 1,154 | pass · 1,153 |
| jac313_store_004_TS · none · memcheck | pass · 1,005 | pass · 952 |
| jac313_store_004_TS · sql · memcheck | pass · 1,258 | pass · 1,304 |
| jac313_store_005_TS · binary · memcheck | pass · 853 | pass · 852 |
| jac313_store_005_TS · inmem · memcheck | pass · 903 | pass · 952 |
| jac313_store_005_TS · jtext · memcheck | pass · 903 | pass · 902 |
| jac313_store_005_TS · none · memcheck | pass · 803 | pass · 802 |
| jac313_store_005_TS · sql · memcheck | pass · 1,103 | pass · 1,103 |
| jac313_store_006_TS · binary · memcheck | pass · 1,053 | pass · 1,053 |
| jac313_store_006_TS · inmem · memcheck | pass · 1,156 | pass · 1,153 |
| jac313_store_006_TS · jtext · memcheck | pass · 1,153 | pass · 1,153 |
| jac313_store_006_TS · none · drd | pass · 953 | pass · 952 |
| jac313_store_006_TS · none · helgrind | pass · 1,203 | pass · 1,204 |
| jac313_store_006_TS · none · memcheck | pass · 953 | pass · 952 |
| jac313_store_006_TS · sql · memcheck | pass · 1,304 | pass · 1,306 |
| jac313_store_007_TS · binary · memcheck | pass · 852 | pass · 852 |
| jac313_store_007_TS · inmem · memcheck | pass · 1,002 | pass · 1,004 |
| jac313_store_007_TS · jtext · memcheck | pass · 1,002 | pass · 1,003 |
| jac313_store_007_TS · none · memcheck | pass · 802 | pass · 802 |
| jac313_store_007_TS · sql · memcheck | pass · 1,103 | pass · 1,103 |
| jac313_store_008_TS · binary · memcheck | pass · 1,202 | pass · 1,203 |
| jac313_store_008_TS · inmem · memcheck | pass · 1,203 | pass · 1,204 |
| jac313_store_008_TS · jtext · memcheck | pass · 1,203 | pass · 1,203 |
| jac313_store_008_TS · none · memcheck | pass · 1,204 | pass · 1,203 |
| jac313_store_008_TS · sql · memcheck | pass · 1,203 | pass · 1,204 |
| jac313_store_binary_persist_smoke · memcheck | pass · 752 | pass · 755 |
| jac313_store_binary_reader_test · memcheck | pass · 852 | pass · 853 |
| jac313_store_binary_to_jtext_smoke · memcheck | pass · 852 | pass · 803 |
| jac313_store_flag_routing_smoke · memcheck | pass · 1,054 | pass · 1,053 |
| jac313_store_flags · memcheck | pass · 651 | pass · 652 |
| jac313_store_jtext_persist_smoke · memcheck | pass · 802 | pass · 802 |
| jac313_store_metric_view · memcheck | pass · 751 | pass · 752 |
| jac313_store_sql_persist_smoke · memcheck | pass · 954 | pass · 3,410 |
| jac313_store_v002_example · memcheck | pass · 652 | pass · 652 |
| jac313_store_v002_test · memcheck | pass · 652 | pass · 5,167 |

## jac313-002 — Intel(R) Core(TM) Ultra 7 265 · 12 cores · 38 GB · Red Hat Enterprise Linux 10.2 (Coughlan)

| scenario | [gcc15](Run_024.md) |
|---|--:|
| jac313_store_001_TS · binary · drd | pass · 601 |
| jac313_store_001_TS · binary · helgrind | pass · 702 |
| jac313_store_001_TS · binary · memcheck | pass · 803 |
| jac313_store_001_TS · inmem · memcheck | pass · 752 |
| jac313_store_001_TS · jtext · drd | pass · 602 |
| jac313_store_001_TS · jtext · helgrind | pass · 601 |
| jac313_store_001_TS · jtext · memcheck | pass · 802 |
| jac313_store_001_TS · none · memcheck | pass · 552 |
| jac313_store_001_TS · sql · drd | pass · 601 |
| jac313_store_001_TS · sql · helgrind | pass · 3,161 |
| jac313_store_001_TS · sql · memcheck | pass · 1,406 |
| jac313_store_002_TS · binary · memcheck | pass · 551 |
| jac313_store_002_TS · inmem · memcheck | pass · 652 |
| jac313_store_002_TS · jtext · memcheck | pass · 652 |
| jac313_store_002_TS · none · memcheck | pass · 451 |
| jac313_store_002_TS · sql · memcheck | pass · 1,811 |
| jac313_store_003_TS · binary · memcheck | pass · 753 |
| jac313_store_003_TS · inmem · memcheck | pass · 803 |
| jac313_store_003_TS · jtext · memcheck | pass · 802 |
| jac313_store_003_TS · none · drd | pass · 501 |
| jac313_store_003_TS · none · helgrind | pass · 501 |
| jac313_store_003_TS · none · memcheck | pass · 602 |
| jac313_store_003_TS · sql · memcheck | pass · 1,206 |
| jac313_store_004_TS · binary · memcheck | pass · 1,307 |
| jac313_store_004_TS · inmem · memcheck | pass · 802 |
| jac313_store_004_TS · jtext · memcheck | pass · 802 |
| jac313_store_004_TS · none · memcheck | pass · 602 |
| jac313_store_004_TS · sql · memcheck | pass · 1,205 |
| jac313_store_005_TS · binary · memcheck | pass · 802 |
| jac313_store_005_TS · inmem · memcheck | pass · 551 |
| jac313_store_005_TS · jtext · memcheck | pass · 551 |
| jac313_store_005_TS · none · memcheck | pass · 451 |
| jac313_store_005_TS · sql · memcheck | pass · 1,054 |
| jac313_store_006_TS · binary · memcheck | pass · 753 |
| jac313_store_006_TS · inmem · memcheck | pass · 802 |
| jac313_store_006_TS · jtext · memcheck | pass · 802 |
| jac313_store_006_TS · none · drd | pass · 501 |
| jac313_store_006_TS · none · helgrind | pass · 501 |
| jac313_store_006_TS · none · memcheck | pass · 602 |
| jac313_store_006_TS · sql · memcheck | pass · 1,354 |
| jac313_store_007_TS · binary · memcheck | pass · 551 |
| jac313_store_007_TS · inmem · memcheck | pass · 652 |
| jac313_store_007_TS · jtext · memcheck | pass · 652 |
| jac313_store_007_TS · none · memcheck | pass · 401 |
| jac313_store_007_TS · sql · memcheck | pass · 1,053 |
| jac313_store_008_TS · binary · memcheck | pass · 1,153 |
| jac313_store_008_TS · inmem · memcheck | pass · 1,154 |
| jac313_store_008_TS · jtext · memcheck | pass · 1,104 |
| jac313_store_008_TS · none · memcheck | pass · 1,204 |
| jac313_store_008_TS · sql · memcheck | pass · 1,103 |
| jac313_store_binary_persist_smoke · memcheck | pass · 351 |
| jac313_store_binary_reader_test · memcheck | pass · 904 |
| jac313_store_binary_to_jtext_smoke · memcheck | pass · 502 |
| jac313_store_flag_routing_smoke · memcheck | pass · 853 |
| jac313_store_flags · memcheck | pass · 250 |
| jac313_store_jtext_persist_smoke · memcheck | pass · 403 |
| jac313_store_metric_view · memcheck | pass · 351 |
| jac313_store_sql_persist_smoke · memcheck | pass · 803 |
| jac313_store_v002_example · memcheck | pass · 250 |
| jac313_store_v002_test · memcheck | pass · 250 |

## jac313-003 — Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz · 8 cores · 31 GB · Linux Mint 22.3

| scenario | [gcc15](Run_034.md) |
|---|--:|
| jac313_store_001_TS · binary · drd | pass · 1,303 |
| jac313_store_001_TS · binary · helgrind | pass · 1,202 |
| jac313_store_001_TS · binary · memcheck | pass · 1,653 |
| jac313_store_001_TS · inmem · memcheck | pass · 1,904 |
| jac313_store_001_TS · jtext · drd | pass · 1,453 |
| jac313_store_001_TS · jtext · helgrind | pass · 1,352 |
| jac313_store_001_TS · jtext · memcheck | pass · 1,853 |
| jac313_store_001_TS · none · memcheck | pass · 1,453 |
| jac313_store_001_TS · sql · drd | pass · 1,553 |
| jac313_store_001_TS · sql · helgrind | pass · 1,603 |
| jac313_store_001_TS · sql · memcheck | pass · 2,104 |
| jac313_store_002_TS · binary · memcheck | pass · 1,353 |
| jac313_store_002_TS · inmem · memcheck | pass · 1,603 |
| jac313_store_002_TS · jtext · memcheck | pass · 1,603 |
| jac313_store_002_TS · none · memcheck | pass · 1,152 |
| jac313_store_002_TS · sql · memcheck | pass · 1,854 |
| jac313_store_003_TS · binary · memcheck | pass · 1,754 |
| jac313_store_003_TS · inmem · memcheck | pass · 1,853 |
| jac313_store_003_TS · jtext · memcheck | pass · 1,904 |
| jac313_store_003_TS · none · drd | pass · 1,203 |
| jac313_store_003_TS · none · helgrind | pass · 1,102 |
| jac313_store_003_TS · none · memcheck | pass · 1,453 |
| jac313_store_003_TS · sql · memcheck | pass · 2,204 |
| jac313_store_004_TS · binary · memcheck | pass · 1,653 |
| jac313_store_004_TS · inmem · memcheck | pass · 1,804 |
| jac313_store_004_TS · jtext · memcheck | pass · 1,803 |
| jac313_store_004_TS · none · memcheck | pass · 1,402 |
| jac313_store_004_TS · sql · memcheck | pass · 2,054 |
| jac313_store_005_TS · binary · memcheck | pass · 1,202 |
| jac313_store_005_TS · inmem · memcheck | pass · 1,353 |
| jac313_store_005_TS · jtext · memcheck | pass · 1,353 |
| jac313_store_005_TS · none · memcheck | pass · 1,052 |
| jac313_store_005_TS · sql · memcheck | pass · 1,704 |
| jac313_store_006_TS · binary · memcheck | pass · 1,553 |
| jac313_store_006_TS · inmem · memcheck | pass · 1,804 |
| jac313_store_006_TS · jtext · memcheck | pass · 1,804 |
| jac313_store_006_TS · none · drd | pass · 1,202 |
| jac313_store_006_TS · none · helgrind | pass · 1,102 |
| jac313_store_006_TS · none · memcheck | pass · 1,402 |
| jac313_store_006_TS · sql · memcheck | pass · 2,054 |
| jac313_store_007_TS · binary · memcheck | pass · 1,253 |
| jac313_store_007_TS · inmem · memcheck | pass · 1,503 |
| jac313_store_007_TS · jtext · memcheck | pass · 1,452 |
| jac313_store_007_TS · none · memcheck | pass · 1,052 |
| jac313_store_007_TS · sql · memcheck | pass · 1,754 |
| jac313_store_008_TS · binary · memcheck | pass · 1,953 |
| jac313_store_008_TS · inmem · memcheck | pass · 2,053 |
| jac313_store_008_TS · jtext · memcheck | pass · 1,903 |
| jac313_store_008_TS · none · memcheck | pass · 1,903 |
| jac313_store_008_TS · sql · memcheck | pass · 1,904 |
| jac313_store_binary_persist_smoke · memcheck | pass · 952 |
| jac313_store_binary_reader_test · memcheck | pass · 1,102 |
| jac313_store_binary_to_jtext_smoke · memcheck | pass · 1,102 |
| jac313_store_flag_routing_smoke · memcheck | pass · 1,603 |
| jac313_store_flags · memcheck | pass · 651 |
| jac313_store_jtext_persist_smoke · memcheck | pass · 1,002 |
| jac313_store_metric_view · memcheck | pass · 851 |
| jac313_store_sql_persist_smoke · memcheck | pass · 1,402 |
| jac313_store_v002_example · memcheck | pass · 651 |
| jac313_store_v002_test · memcheck | pass · 651 |
