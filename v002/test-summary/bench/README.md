# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 12 cores · 23 GB · Fedora Linux 44 (Workstation Edition) · x7k

| config | [clang22](Run_008.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_007.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 0 flags, non-durable @10M | 6,102,303 | 5.82M–6.73M | 0 B | 5,904,555 | 5.64M–6.96M | 0 B |
| 2 flags, non-durable @10M | 6,490,022 | 5.91M–6.83M | 0 B | 6,327,452 | 5.78M–6.92M | 0 B |
| 4 flags, non-durable @10M | 6,592,249 | 5.86M–6.83M | 0 B | 6,003,254 | 5.65M–6.91M | 0 B |
| 6 flags, non-durable @10M | 6,051,864 | 5.85M–6.69M | 0 B | 6,832,664 | 5.55M–6.93M | 0 B |
| durable binary @1M | 632,792 | 546.22K–673.30K | 83.78 MB | 615,374 | 578.71K–644.29K | 83.78 MB |
| durable binary @10M | 696,862 | 686.40K–804.41K | 837.80 MB | 698,688 | 613.60K–760.87K | 837.80 MB |
| durable jtext @1M | 1,988,957 | 1.93M–2.13M | 80.81 MB | 1,802,611 | 1.78M–1.97M | 80.81 MB |
| durable jtext @10M | 2,091,103 | 2.03M–2.11M | 836.37 MB | 2,002,909 | 1.95M–2.01M | 836.26 MB |
| durable sql @1M | 402,386 | 363.73K–426.46K | 75.18 MB | 369,889 | 338.67K–410.69K | 75.16 MB |
| durable sql @10M | 673,848 | 646.62K–701.79K | 773.87 MB | 677,777 | 669.93K–700.75K | 773.66 MB |
