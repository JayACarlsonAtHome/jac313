# verify — compiler comparison

_Generated from `results.db`. Columns are compilers (latest run each); cell = ms (pass) or status._

| scenario | [clang21](Run_010.md) | [gcc15](Run_009.md) |
|---|--:|--:|
| jac313_store_001_TS · binary · drd | 501 | 501 |
| jac313_store_001_TS · binary · helgrind | 652 | 651 |
| jac313_store_001_TS · binary · memcheck | 651 | 651 |
| jac313_store_001_TS · inmem · memcheck | 802 | 802 |
| jac313_store_001_TS · jtext · drd | 601 | 601 |
| jac313_store_001_TS · jtext · helgrind | 702 | 702 |
| jac313_store_001_TS · jtext · memcheck | 802 | 802 |
| jac313_store_001_TS · none · memcheck | 601 | 601 |
| jac313_store_001_TS · sql · drd | 651 | 652 |
| jac313_store_001_TS · sql · helgrind | 802 | 802 |
| jac313_store_001_TS · sql · memcheck | 902 | 902 |
| jac313_store_002_TS · binary · memcheck | 551 | 551 |
| jac313_store_002_TS · inmem · memcheck | 702 | 702 |
| jac313_store_002_TS · jtext · memcheck | 702 | 701 |
| jac313_store_002_TS · none · memcheck | 451 | 451 |
| jac313_store_002_TS · sql · memcheck | 752 | 752 |
| jac313_store_003_TS · binary · memcheck | 702 | 702 |
| jac313_store_003_TS · inmem · memcheck | 852 | 802 |
| jac313_store_003_TS · jtext · memcheck | 852 | 852 |
| jac313_store_003_TS · none · drd | fail | 501 |
| jac313_store_003_TS · none · helgrind | fail | 551 |
| jac313_store_003_TS · none · memcheck | 652 | 601 |
| jac313_store_003_TS · sql · memcheck | 902 | 902 |
| jac313_store_004_TS · binary · memcheck | 702 | 651 |
| jac313_store_004_TS · inmem · memcheck | 852 | 802 |
| jac313_store_004_TS · jtext · memcheck | 802 | 802 |
| jac313_store_004_TS · none · memcheck | 601 | 601 |
| jac313_store_004_TS · sql · memcheck | 902 | 902 |
| jac313_store_005_TS · binary · memcheck | 501 | 501 |
| jac313_store_005_TS · inmem · memcheck | 601 | 602 |
| jac313_store_005_TS · jtext · memcheck | 601 | 601 |
| jac313_store_005_TS · none · memcheck | 451 | 451 |
| jac313_store_005_TS · sql · memcheck | 702 | 702 |
| jac313_store_006_TS · binary · memcheck | 702 | 702 |
| jac313_store_006_TS · inmem · memcheck | 852 | 852 |
| jac313_store_006_TS · jtext · memcheck | 852 | 852 |
| jac313_store_006_TS · none · drd | fail | 501 |
| jac313_store_006_TS · none · helgrind | fail | 551 |
| jac313_store_006_TS · none · memcheck | 652 | 601 |
| jac313_store_006_TS · sql · memcheck | 952 | 902 |
| jac313_store_007_TS · binary · memcheck | 501 | 551 |
| jac313_store_007_TS · inmem · memcheck | 652 | 651 |
| jac313_store_007_TS · jtext · memcheck | 651 | 651 |
| jac313_store_007_TS · none · memcheck | 451 | 451 |
| jac313_store_007_TS · sql · memcheck | 752 | 752 |
| jac313_store_008_TS · binary · memcheck | 802 | 852 |
| jac313_store_008_TS · inmem · memcheck | 852 | 852 |
| jac313_store_008_TS · jtext · memcheck | 852 | 852 |
| jac313_store_008_TS · none · memcheck | 852 | 852 |
| jac313_store_008_TS · sql · memcheck | 802 | 852 |
| jac313_store_binary_persist_smoke · memcheck | 351 | 351 |
| jac313_store_binary_reader_test · memcheck | 451 | 451 |
| jac313_store_binary_to_jtext_smoke · memcheck | 451 | 451 |
| jac313_store_flag_routing_smoke · memcheck | 651 | 652 |
| jac313_store_flags · memcheck | 250 | 250 |
| jac313_store_jtext_persist_smoke · memcheck | 401 | 401 |
| jac313_store_metric_view · memcheck | 351 | 351 |
| jac313_store_sql_persist_smoke · memcheck | 551 | 551 |
| jac313_store_v002_example · memcheck | 250 | 250 |
| jac313_store_v002_test · memcheck | 250 | 250 |
