<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler. Split into non-durable / durable @1M / durable @10M, each sorted by median ops/sec (descending)._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang21](Run_021.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_020.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 0 flags, non-durable | 9,151,500 | 8.45M–9.49M | 0 B | 8,438,358 | 8.09M–8.97M | 0 B |
| 6 flags, non-durable | 9,145,708 | 8.24M–9.47M | 0 B | 8,251,472 | 7.52M–8.98M | 0 B |
| 2 flags, non-durable | 9,091,961 | 8.71M–9.36M | 0 B | 8,273,145 | 8.13M–8.69M | 0 B |
| 4 flags, non-durable | 8,988,648 | 8.44M–9.55M | 0 B | 8,561,883 | 8.12M–9.10M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang21](Run_021.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_020.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 857,570 | 834.72K–858.84K | 83.78 MB | 828,809 | 804.25K–836.65K | 83.78 MB |
| durable jtext | 713,826 | 675.41K–714.83K | 80.77 MB | 756,568 | 716.97K–761.08K | 80.77 MB |
| durable sql | 519,194 | 511.85K–533.13K | 75.03 MB | 525,396 | 512.38K–547.15K | 75.03 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang21](Run_021.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_020.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 889,521 | 791.96K–900.78K | 837.80 MB | 873,982 | 859.33K–888.68K | 837.80 MB |
| durable jtext | 737,851 | 728.75K–762.35K | 836.21 MB | 735,558 | 710.51K–740.26K | 836.17 MB |
| durable sql | 659,694 | 648.88K–666.44K | 772.41 MB | 659,869 | 642.75K–664.73K | 772.40 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_021](Run_021.md) | clang21 · 2026-07-01T16:02:35Z |
| [Run_020](Run_020.md) | gcc15 · 2026-07-01T15:59:31Z |

## jac313-002 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · Fedora Linux 44 (Workstation Edition) · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang22](Run_069.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_068.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 6 flags, non-durable | 5,385,343 | 5.32M–5.55M | 0 B | 5,478,450 | 4.98M–5.55M | 0 B |
| 0 flags, non-durable | 5,455,468 | 5.13M–5.55M | 0 B | 5,281,852 | 5.00M–5.53M | 0 B |
| 4 flags, non-durable | 5,413,963 | 5.31M–5.54M | 0 B | 5,226,054 | 4.89M–5.53M | 0 B |
| 2 flags, non-durable | 5,352,978 | 5.24M–5.52M | 0 B | 5,358,127 | 4.94M–5.57M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang22](Run_069.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_068.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 781,378 | 739.36K–842.35K | 80.78 MB | 729,325 | 697.99K–755.47K | 80.77 MB |
| durable binary | 714,920 | 709.84K–781.66K | 83.78 MB | 716,685 | 699.63K–777.97K | 83.78 MB |
| durable sql | 402,653 | 392.75K–414.73K | 75.03 MB | 415,955 | 376.30K–436.38K | 75.01 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang22](Run_069.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_068.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 1,064,555 | 1.06M–1.20M | 836.65 MB | 1,107,264 | 1.03M–1.15M | 836.67 MB |
| durable sql | 698,916 | 624.62K–719.97K | 772.27 MB | 845,763 | 711.70K–855.04K | 772.05 MB |
| durable binary | 770,795 | 695.68K–785.26K | 837.80 MB | 796,953 | 691.47K–799.14K | 837.80 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_069](Run_069.md) | clang22 · 2026-07-02T05:31:27Z |
| [Run_068](Run_068.md) | gcc16 · 2026-07-02T05:28:07Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
