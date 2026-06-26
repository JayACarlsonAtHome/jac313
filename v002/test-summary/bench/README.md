# bench — compiler comparison

_Generated from `results.db`. Cell = median ops/sec; latest run per compiler._

| config | [clang21](Run_008.md) | [gcc15](Run_007.md) |
|---|--:|--:|
| 0 flags, non-durable @10M | 6030096 | 6079996 |
| 2 flags, non-durable @10M | 6011908 | 6098843 |
| 4 flags, non-durable @10M | 6008544 | 6095109 |
| 6 flags, non-durable @10M | 6014066 | 6043925 |
| durable binary @1M | 2104554 | 2094504 |
| durable binary @10M | 2129169 | 2147080 |
| durable jtext @1M | 1764655 | 1784777 |
| durable jtext @10M | 1730539 | 1761337 |
| durable sql @1M | 1243027 | 1271396 |
| durable sql @10M | 1278442 | 1324148 |
