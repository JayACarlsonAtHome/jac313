# verify — compiler comparison

_Generated from `results.db`. Columns are compilers (latest run each); cell = ms (pass) or status._

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
