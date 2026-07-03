<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler. Split into non-durable / durable @1M / durable @10M, each sorted by median ops/sec (descending)._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · fedora-44 · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang22](Run_008.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_007.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 6 flags, non-durable | 5,303,706 | 5.21M–5.50M | 0 B | 5,609,261 | 5.32M–5.71M | 0 B |
| 0 flags, non-durable | 5,407,860 | 5.14M–5.50M | 0 B | 5,306,797 | 3.15M–5.71M | 0 B |
| 4 flags, non-durable | 5,287,670 | 5.05M–5.41M | 0 B | 5,357,789 | 5.13M–5.64M | 0 B |
| 2 flags, non-durable | 5,323,338 | 5.12M–5.51M | 0 B | 5,343,377 | 5.14M–5.68M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang22](Run_008.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_007.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 806,519 | 779.53K–850.75K | 80.82 MB | 819,525 | 757.10K–876.89K | 80.78 MB |
| durable binary | 546,611 | 539.72K–694.08K | 83.78 MB | 769,717 | 681.34K–799.88K | 83.78 MB |
| durable sql | 433,968 | 411.63K–437.24K | 75.03 MB | 421,885 | 374.69K–446.39K | 75.02 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang22](Run_008.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_007.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 1,155,761 | 1.15M–1.28M | 836.61 MB | 1,202,127 | 1.16M–1.20M | 836.51 MB |
| durable sql | 775,025 | 749.89K–776.52K | 772.06 MB | 751,067 | 621.14K–812.73K | 771.75 MB |
| durable binary | 642,937 | 594.89K–725.44K | 837.80 MB | 648,639 | 645.83K–724.96K | 837.80 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_008](Run_008.md) | clang22 · 2026-07-03T04:47:45Z |
| [Run_007](Run_007.md) | gcc16 · 2026-07-03T04:44:17Z |

## jac313-002 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · rhel-10.2 · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang21](Run_031.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_030.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 4 flags, non-durable | 9,249,419 | 8.69M–9.62M | 0 B | 8,877,728 | 7.87M–9.05M | 0 B |
| 6 flags, non-durable | 9,214,058 | 8.27M–9.46M | 0 B | 8,398,778 | 8.20M–8.95M | 0 B |
| 2 flags, non-durable | 9,150,812 | 8.46M–9.37M | 0 B | 8,467,365 | 8.15M–8.90M | 0 B |
| 0 flags, non-durable | 9,090,803 | 8.43M–9.50M | 0 B | 8,321,865 | 3.66M–8.92M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang21](Run_031.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_030.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 878,779 | 842.02K–903.18K | 83.78 MB | 910,453 | 810.36K–941.95K | 83.78 MB |
| durable jtext | 746,595 | 629.34K–794.76K | 80.77 MB | 785,311 | 757.18K–847.56K | 80.77 MB |
| durable sql | 525,342 | 513.66K–533.04K | 75.03 MB | 530,686 | 515.20K–540.16K | 75.01 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang21](Run_031.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_030.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 939,190 | 897.31K–954.00K | 837.80 MB | 998,842 | 847.81K–1.02M | 837.80 MB |
| durable jtext | 797,754 | 773.06K–798.41K | 836.19 MB | 779,552 | 742.09K–787.38K | 836.17 MB |
| durable sql | 669,713 | 644.89K–697.49K | 772.01 MB | 683,286 | 673.70K–689.40K | 771.95 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_031](Run_031.md) | clang21 · 2026-07-03T06:00:59Z |
| [Run_030](Run_030.md) | gcc15 · 2026-07-03T05:58:01Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
