# bench — compiler comparison

_Generated from `results.db`. Per compiler: median ops/sec and the low–high band, head to head; latest run per compiler._

| config | [clang21](Run_008.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_007.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 0 flags, non-durable @10M | 5,515,476 | 5.44M–5.58M | 0 B | 6,152,596 | 6.04M–6.29M | 0 B |
| 2 flags, non-durable @10M | 5,563,359 | 5.52M–5.59M | 0 B | 6,212,216 | 6.16M–6.29M | 0 B |
| 4 flags, non-durable @10M | 5,545,245 | 5.52M–5.57M | 0 B | 6,251,809 | 6.15M–6.27M | 0 B |
| 6 flags, non-durable @10M | 5,532,729 | 5.51M–5.57M | 0 B | 6,184,309 | 6.11M–6.27M | 0 B |
| durable binary @1M | 2,111,928 | 2.07M–2.12M | 83.780 MB | 2,128,316 | 2.12M–2.13M | 83.780 MB |
| durable binary @10M | 2,156,933 | 2.13M–2.16M | 837.803 MB | 2,148,916 | 2.12M–2.15M | 837.803 MB |
| durable jtext @1M | 1,763,936 | 1.74M–1.77M | 82.955 MB | 1,769,836 | 1.74M–1.77M | 82.955 MB |
| durable jtext @10M | 1,746,406 | 1.73M–1.76M | 848.613 MB | 1,765,215 | 1.76M–1.79M | 848.613 MB |
| durable sql @1M | 1,260,478 | 1.25M–1.28M | 76.125 MB | 1,254,370 | 1.19M–1.27M | 76.125 MB |
| durable sql @10M | 1,310,517 | 1.30M–1.33M | 783.137 MB | 1,326,077 | 1.32M–1.33M | 783.121 MB |
