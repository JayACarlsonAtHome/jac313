<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler. Split into non-durable / durable @1M / durable @10M, each sorted by median ops/sec (descending)._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 12 cores · 23 GB · Fedora Linux 44 (Workstation Edition) · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang22](Run_008.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_007.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 6 flags, non-durable | 6,051,864 | 5.85M–6.69M | 0 B | 6,832,664 | 5.55M–6.93M | 0 B |
| 4 flags, non-durable | 6,592,249 | 5.86M–6.83M | 0 B | 6,003,254 | 5.65M–6.91M | 0 B |
| 2 flags, non-durable | 6,490,022 | 5.91M–6.83M | 0 B | 6,327,452 | 5.78M–6.92M | 0 B |
| 0 flags, non-durable | 6,102,303 | 5.82M–6.73M | 0 B | 5,904,555 | 5.64M–6.96M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang22](Run_008.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_007.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 1,988,957 | 1.93M–2.13M | 80.81 MB | 1,802,611 | 1.78M–1.97M | 80.81 MB |
| durable binary | 632,792 | 546.22K–673.30K | 83.78 MB | 615,374 | 578.71K–644.29K | 83.78 MB |
| durable sql | 402,386 | 363.73K–426.46K | 75.18 MB | 369,889 | 338.67K–410.69K | 75.16 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang22](Run_008.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_007.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 2,091,103 | 2.03M–2.11M | 836.37 MB | 2,002,909 | 1.95M–2.01M | 836.26 MB |
| durable binary | 696,862 | 686.40K–804.41K | 837.80 MB | 698,688 | 613.60K–760.87K | 837.80 MB |
| durable sql | 673,848 | 646.62K–701.79K | 773.87 MB | 677,777 | 669.93K–700.75K | 773.66 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_008](Run_008.md) | clang22 · 2026-06-29T07:40:51Z |
| [Run_007](Run_007.md) | gcc16 · 2026-06-29T07:37:39Z |

## jac313-002 — Intel(R) Core(TM) Ultra 7 265 · 12 cores · 38 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang21](Run_022.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_021.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 4 flags, non-durable | 13,498,306 | 10.82M–13.72M | 0 B | 10,543,669 | 9.50M–13.36M | 0 B |
| 0 flags, non-durable | 13,446,060 | 10.87M–13.80M | 0 B | 12,955,163 | 10.24M–13.41M | 0 B |
| 2 flags, non-durable | 13,423,268 | 10.04M–13.83M | 0 B | 10,246,243 | 9.13M–13.18M | 0 B |
| 6 flags, non-durable | 10,962,960 | 10.11M–13.72M | 0 B | 10,328,904 | 9.45M–13.28M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang21](Run_022.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_021.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 2,283,663 | 2.12M–2.37M | 80.82 MB | 2,360,907 | 2.19M–2.51M | 80.81 MB |
| durable binary | 809,079 | 803.99K–825.93K | 83.78 MB | 813,739 | 806.27K–818.28K | 83.78 MB |
| durable sql | 463,951 | 459.30K–466.61K | 75.12 MB | 455,991 | 440.99K–466.59K | 75.11 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang21](Run_022.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_021.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 2,486,352 | 2.46M–2.50M | 836.38 MB | 2,514,329 | 2.46M–2.52M | 836.41 MB |
| durable binary | 861,670 | 766.63K–865.30K | 837.80 MB | 866,138 | 850.99K–866.73K | 837.80 MB |
| durable sql | 721,094 | 682.74K–724.09K | 773.25 MB | 721,767 | 715.55K–723.75K | 772.91 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_022](Run_022.md) | clang21 · 2026-06-30T05:00:55Z |
| [Run_021](Run_021.md) | gcc15 · 2026-06-30T04:58:35Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
