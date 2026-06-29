# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 20 cores · 62 GB · Red Hat Enterprise Linux 9.8 (Plow) · ssd

| config | [clang21](Run_008.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_007.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 0 flags, non-durable @10M | 14,462,193 | 10.54M–15.11M | 0 B | 12,779,044 | 11.40M–15.27M | 0 B |
| 2 flags, non-durable @10M | 11,893,530 | 10.72M–15.13M | 0 B | 11,550,450 | 10.59M–15.67M | 0 B |
| 4 flags, non-durable @10M | 14,458,846 | 10.55M–15.07M | 0 B | 14,427,412 | 11.14M–15.94M | 0 B |
| 6 flags, non-durable @10M | 14,358,369 | 11.30M–14.86M | 0 B | 12,808,476 | 11.12M–15.40M | 0 B |
| durable binary @1M | 2,476,400 | 2.45M–2.52M | 83.78 MB | 2,526,701 | 2.51M–2.60M | 83.78 MB |
| durable binary @10M | 2,633,749 | 2.56M–2.64M | 837.80 MB | 2,597,353 | 2.57M–2.62M | 837.80 MB |
| durable jtext @1M | 2,072,479 | 2.02M–2.11M | 80.80 MB | 2,172,732 | 2.13M–2.19M | 80.80 MB |
| durable jtext @10M | 2,156,305 | 2.14M–2.18M | 836.34 MB | 2,279,662 | 2.17M–2.29M | 836.28 MB |
| durable sql @1M | 1,386,336 | 1.39M–1.42M | 75.06 MB | 1,439,485 | 1.44M–1.44M | 75.05 MB |
| durable sql @10M | 1,451,481 | 1.44M–1.47M | 773.45 MB | 1,473,418 | 1.45M–1.48M | 773.45 MB |
