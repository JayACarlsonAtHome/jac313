# bench — compiler comparison

_Generated from `results.db`. Per compiler: median ops/sec and the low–high band, head to head; latest run per compiler._

| config | [gcc15](Run_001.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|
| 0 flags, non-durable @10M | 6,091,864 | 6.05M–6.12M | 0 B |
| 2 flags, non-durable @10M | 6,108,376 | 6.03M–6.12M | 0 B |
| 4 flags, non-durable @10M | 6,066,487 | 6.00M–6.14M | 0 B |
| 6 flags, non-durable @10M | 6,039,868 | 6.00M–6.12M | 0 B |
| durable binary @1M | 2,089,122 | 2.08M–2.10M | 83.780 MB |
| durable jtext @1M | 1,770,842 | 1.76M–1.78M | 82.955 MB |
| durable jtext @10M | 1,800,595 | 1.80M–1.82M | 848.613 MB |
| durable sql @1M | 1,257,726 | 1.22M–1.27M | 76.121 MB |
