# verify — compiler comparison

_Generated from `results.db`. Columns are compilers (latest run each); cell = ms (pass) or status._

| scenario | [clang21](Run_010.md) | [gcc15](Run_009.md) |
|---|--:|--:|
| jac313_store_001_TS · binary · drd | pass · 501 | pass · 501 |
| jac313_store_001_TS · binary · helgrind | pass · 651 | pass · 651 |
| jac313_store_001_TS · binary · memcheck | pass · 701 | pass · 651 |
| jac313_store_001_TS · inmem · memcheck | pass · 802 | pass · 802 |
| jac313_store_001_TS · jtext · drd | pass · 651 | pass · 601 |
| jac313_store_001_TS · jtext · helgrind | pass · 701 | pass · 702 |
| jac313_store_001_TS · jtext · memcheck | pass · 802 | pass · 802 |
| jac313_store_001_TS · none · memcheck | pass · 601 | pass · 602 |
| jac313_store_001_TS · sql · drd | pass · 651 | pass · 652 |
| jac313_store_001_TS · sql · helgrind | pass · 801 | pass · 802 |
| jac313_store_001_TS · sql · memcheck | pass · 902 | pass · 902 |
| jac313_store_002_TS · binary · memcheck | pass · 551 | pass · 551 |
| jac313_store_002_TS · inmem · memcheck | pass · 702 | pass · 702 |
| jac313_store_002_TS · jtext · memcheck | pass · 702 | pass · 702 |
| jac313_store_002_TS · none · memcheck | pass · 451 | pass · 450 |
| jac313_store_002_TS · sql · memcheck | pass · 752 | pass · 752 |
| jac313_store_003_TS · binary · memcheck | pass · 702 | pass · 702 |
| jac313_store_003_TS · inmem · memcheck | pass · 852 | pass · 852 |
| jac313_store_003_TS · jtext · memcheck | pass · 852 | pass · 852 |
| jac313_store_003_TS · none · drd | pass · 501 | pass · 501 |
| jac313_store_003_TS · none · helgrind | pass · 551 | pass · 551 |
| jac313_store_003_TS · none · memcheck | pass · 652 | pass · 601 |
| jac313_store_003_TS · sql · memcheck | pass · 902 | pass · 902 |
| jac313_store_004_TS · binary · memcheck | pass · 702 | pass · 651 |
| jac313_store_004_TS · inmem · memcheck | pass · 852 | pass · 802 |
| jac313_store_004_TS · jtext · memcheck | pass · 852 | pass · 802 |
| jac313_store_004_TS · none · memcheck | pass · 601 | pass · 601 |
| jac313_store_004_TS · sql · memcheck | pass · 902 | pass · 902 |
| jac313_store_005_TS · binary · memcheck | pass · 501 | pass · 501 |
| jac313_store_005_TS · inmem · memcheck | pass · 601 | pass · 601 |
| jac313_store_005_TS · jtext · memcheck | pass · 601 | pass · 602 |
| jac313_store_005_TS · none · memcheck | pass · 451 | pass · 451 |
| jac313_store_005_TS · sql · memcheck | pass · 701 | pass · 752 |
| jac313_store_006_TS · binary · memcheck | pass · 702 | pass · 702 |
| jac313_store_006_TS · inmem · memcheck | pass · 852 | pass · 852 |
| jac313_store_006_TS · jtext · memcheck | pass · 852 | pass · 852 |
| jac313_store_006_TS · none · drd | pass · 501 | pass · 501 |
| jac313_store_006_TS · none · helgrind | pass · 551 | pass · 551 |
| jac313_store_006_TS · none · memcheck | pass · 652 | pass · 601 |
| jac313_store_006_TS · sql · memcheck | pass · 902 | pass · 902 |
| jac313_store_007_TS · binary · memcheck | pass · 501 | pass · 551 |
| jac313_store_007_TS · inmem · memcheck | pass · 651 | pass · 651 |
| jac313_store_007_TS · jtext · memcheck | pass · 652 | pass · 651 |
| jac313_store_007_TS · none · memcheck | pass · 451 | pass · 451 |
| jac313_store_007_TS · sql · memcheck | pass · 752 | pass · 751 |
| jac313_store_008_TS · binary · memcheck | pass · 852 | pass · 852 |
| jac313_store_008_TS · inmem · memcheck | pass · 852 | pass · 852 |
| jac313_store_008_TS · jtext · memcheck | pass · 851 | pass · 852 |
| jac313_store_008_TS · none · memcheck | pass · 802 | pass · 852 |
| jac313_store_008_TS · sql · memcheck | pass · 851 | pass · 852 |
| jac313_store_binary_persist_smoke · memcheck | pass · 351 | pass · 351 |
| jac313_store_binary_reader_test · memcheck | pass · 451 | pass · 451 |
| jac313_store_binary_to_jtext_smoke · memcheck | pass · 451 | pass · 451 |
| jac313_store_flag_routing_smoke · memcheck | pass · 651 | pass · 652 |
| jac313_store_flags · memcheck | pass · 250 | pass · 250 |
| jac313_store_jtext_persist_smoke · memcheck | pass · 401 | pass · 401 |
| jac313_store_metric_view · memcheck | pass · 350 | pass · 351 |
| jac313_store_sql_persist_smoke · memcheck | pass · 551 | pass · 551 |
| jac313_store_v002_example · memcheck | pass · 250 | pass · 250 |
| jac313_store_v002_test · memcheck | pass · 250 | pass · 250 |
