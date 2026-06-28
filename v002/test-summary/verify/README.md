# verify — by machine

_Generated from `results.db`. Each machine (`jac313-###`) is its own section. Columns are that host's compilers (latest run each); cell = ms (pass) or status._

## jac313-001 — Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz · 8 cores · 31 GB · Linux Mint 22.3 · ssd

| scenario | [clang20](Run_010.md) | [gcc15](Run_009.md) |
|---|--:|--:|
| jac313_store_001_TS · binary · drd | pass · 1,303 | pass · 1,253 |
| jac313_store_001_TS · binary · helgrind | pass · 1,253 | pass · 1,203 |
| jac313_store_001_TS · binary · memcheck | pass · 1,553 | pass · 1,603 |
| jac313_store_001_TS · inmem · memcheck | pass · 1,753 | pass · 1,803 |
| jac313_store_001_TS · jtext · drd | pass · 1,503 | pass · 1,453 |
| jac313_store_001_TS · jtext · helgrind | pass · 1,402 | pass · 1,302 |
| jac313_store_001_TS · jtext · memcheck | pass · 1,753 | pass · 1,803 |
| jac313_store_001_TS · none · memcheck | pass · 1,403 | pass · 1,403 |
| jac313_store_001_TS · sql · drd | pass · 1,553 | pass · 1,453 |
| jac313_store_001_TS · sql · helgrind | pass · 1,703 | pass · 1,553 |
| jac313_store_001_TS · sql · memcheck | pass · 2,054 | pass · 2,104 |
| jac313_store_002_TS · binary · memcheck | pass · 1,302 | pass · 1,353 |
| jac313_store_002_TS · inmem · memcheck | pass · 1,503 | pass · 1,553 |
| jac313_store_002_TS · jtext · memcheck | pass · 1,503 | pass · 1,503 |
| jac313_store_002_TS · none · memcheck | pass · 1,102 | pass · 1,102 |
| jac313_store_002_TS · sql · memcheck | pass · 1,803 | pass · 1,804 |
| jac313_store_003_TS · binary · memcheck | pass · 1,603 | pass · 1,604 |
| jac313_store_003_TS · inmem · memcheck | pass · 1,803 | pass · 1,854 |
| jac313_store_003_TS · jtext · memcheck | pass · 1,853 | pass · 1,854 |
| jac313_store_003_TS · none · drd | pass · 1,202 | pass · 1,152 |
| jac313_store_003_TS · none · helgrind | pass · 1,102 | pass · 1,052 |
| jac313_store_003_TS · none · memcheck | pass · 1,403 | pass · 1,453 |
| jac313_store_003_TS · sql · memcheck | pass · 2,104 | pass · 2,154 |
| jac313_store_004_TS · binary · memcheck | pass · 1,603 | pass · 1,603 |
| jac313_store_004_TS · inmem · memcheck | pass · 1,804 | pass · 1,803 |
| jac313_store_004_TS · jtext · memcheck | pass · 1,804 | pass · 1,853 |
| jac313_store_004_TS · none · memcheck | pass · 1,402 | pass · 1,402 |
| jac313_store_004_TS · sql · memcheck | pass · 2,104 | pass · 2,104 |
| jac313_store_005_TS · binary · memcheck | pass · 1,202 | pass · 1,302 |
| jac313_store_005_TS · inmem · memcheck | pass · 1,352 | pass · 1,403 |
| jac313_store_005_TS · jtext · memcheck | pass · 1,353 | pass · 1,403 |
| jac313_store_005_TS · none · memcheck | pass · 1,052 | pass · 1,102 |
| jac313_store_005_TS · sql · memcheck | pass · 1,703 | pass · 1,703 |
| jac313_store_006_TS · binary · memcheck | pass · 1,603 | pass · 1,603 |
| jac313_store_006_TS · inmem · memcheck | pass · 1,803 | pass · 1,853 |
| jac313_store_006_TS · jtext · memcheck | pass · 1,804 | pass · 1,853 |
| jac313_store_006_TS · none · drd | pass · 1,202 | pass · 1,153 |
| jac313_store_006_TS · none · helgrind | pass · 1,102 | pass · 1,052 |
| jac313_store_006_TS · none · memcheck | pass · 1,453 | pass · 1,403 |
| jac313_store_006_TS · sql · memcheck | pass · 2,104 | pass · 2,154 |
| jac313_store_007_TS · binary · memcheck | pass · 1,253 | pass · 1,302 |
| jac313_store_007_TS · inmem · memcheck | pass · 1,503 | pass · 1,503 |
| jac313_store_007_TS · jtext · memcheck | pass · 1,503 | pass · 1,503 |
| jac313_store_007_TS · none · memcheck | pass · 1,052 | pass · 1,102 |
| jac313_store_007_TS · sql · memcheck | pass · 1,753 | pass · 1,753 |
| jac313_store_008_TS · binary · memcheck | pass · 1,904 | pass · 1,954 |
| jac313_store_008_TS · inmem · memcheck | pass · 2,054 | pass · 1,953 |
| jac313_store_008_TS · jtext · memcheck | pass · 1,904 | pass · 2,004 |
| jac313_store_008_TS · none · memcheck | pass · 1,954 | pass · 1,953 |
| jac313_store_008_TS · sql · memcheck | pass · 1,904 | pass · 1,954 |
| jac313_store_binary_persist_smoke · memcheck | pass · 902 | pass · 902 |
| jac313_store_binary_reader_test · memcheck | pass · 1,052 | pass · 1,052 |
| jac313_store_binary_to_jtext_smoke · memcheck | pass · 1,002 | pass · 1,052 |
| jac313_store_flag_routing_smoke · memcheck | pass · 1,453 | pass · 1,503 |
| jac313_store_flags · memcheck | pass · 701 | pass · 651 |
| jac313_store_jtext_persist_smoke · memcheck | pass · 952 | pass · 1,002 |
| jac313_store_metric_view · memcheck | pass · 901 | pass · 852 |
| jac313_store_sql_persist_smoke · memcheck | pass · 1,353 | pass · 1,352 |
| jac313_store_v002_example · memcheck | pass · 652 | pass · 651 |
| jac313_store_v002_test · memcheck | pass · 651 | pass · 651 |
