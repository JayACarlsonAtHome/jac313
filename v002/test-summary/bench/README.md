# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler._

## jac313-001 — Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz · 8 cores · 31 GB · Linux Mint 22.3 · ssd

| config | [clang20](Run_008.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_007.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 0 flags, non-durable @10M | 3,991,723 | 3.96M–4.01M | 0 B | 3,892,594 | 3.78M–3.98M | 0 B |
| 2 flags, non-durable @10M | 3,977,686 | 3.96M–4.01M | 0 B | 3,960,027 | 3.67M–4.00M | 0 B |
| 4 flags, non-durable @10M | 3,994,852 | 3.97M–4.01M | 0 B | 3,790,583 | 3.68M–3.88M | 0 B |
| 6 flags, non-durable @10M | 3,953,698 | 3.83M–4.01M | 0 B | 3,967,245 | 3.92M–3.99M | 0 B |
| durable binary @1M | 1,727,065 | 1.67M–1.74M | 83.78 MB | 1,853,633 | 1.83M–1.87M | 83.78 MB |
| durable binary @10M | 1,795,478 | 1.73M–1.80M | 837.80 MB | 1,901,270 | 1.85M–1.92M | 837.80 MB |
| durable jtext @1M | 800,935 | 769.22K–812.79K | 83.91 MB | 879,988 | 861.10K–888.75K | 83.91 MB |
| durable jtext @10M | 810,038 | 791.79K–820.08K | 858.15 MB | 861,623 | 835.77K–895.53K | 858.15 MB |
| durable sql @1M | 574,184 | 560.92K–575.44K | 76.04 MB | 599,132 | 594.62K–600.83K | 76.04 MB |
| durable sql @10M | 568,424 | 565.77K–572.98K | 782.32 MB | 566,988 | 566.96K–575.47K | 782.79 MB |
