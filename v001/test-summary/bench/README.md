<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler. Split into non-durable / durable @1M / durable @10M, each sorted by median ops/sec (descending)._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang21](Run_008.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_007.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 2 flags, non-durable | 9,153,495 | 8.87M–9.49M | 0 B | 8,527,602 | 8.01M–8.86M | 0 B |
| 0 flags, non-durable | 9,138,888 | 8.21M–9.44M | 0 B | 8,605,279 | 7.77M–8.93M | 0 B |
| 6 flags, non-durable | 9,081,974 | 8.33M–9.33M | 0 B | 8,386,394 | 8.09M–9.08M | 0 B |
| 4 flags, non-durable | 8,999,272 | 8.69M–9.42M | 0 B | 8,752,683 | 8.09M–8.97M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang21](Run_008.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_007.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 821,718 | 799.70K–874.32K | 83.78 MB | 811,898 | 772.10K–825.89K | 83.78 MB |
| durable jtext | 688,053 | 674.32K–701.70K | 80.77 MB | 728,341 | 720.87K–802.32K | 80.75 MB |
| durable sql | 547,794 | 531.24K–556.08K | 75.02 MB | 543,021 | 537.00K–559.69K | 75.00 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang21](Run_008.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_007.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 897,742 | 886.89K–901.93K | 837.80 MB | 888,402 | 885.46K–894.48K | 837.80 MB |
| durable jtext | 754,917 | 746.51K–785.52K | 836.24 MB | 751,265 | 739.42K–793.28K | 836.16 MB |
| durable sql | 656,091 | 648.14K–668.74K | 772.14 MB | 653,605 | 653.44K–668.05K | 771.98 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_008](Run_008.md) | clang21 · 2026-07-01T15:46:28Z |
| [Run_007](Run_007.md) | gcc15 · 2026-07-01T15:43:27Z |

## jac313-002 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · Fedora Linux 44 (Workstation Edition) · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang22](Run_056.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_055.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 6 flags, non-durable | 5,309,670 | 5.14M–5.40M | 0 B | 5,214,279 | 4.95M–5.56M | 0 B |
| 0 flags, non-durable | 5,253,647 | 5.08M–5.43M | 0 B | 5,306,213 | 5.09M–5.58M | 0 B |
| 2 flags, non-durable | 5,304,165 | 4.96M–5.36M | 0 B | 5,240,443 | 5.13M–5.55M | 0 B |
| 4 flags, non-durable | 5,275,583 | 5.05M–5.44M | 0 B | 5,248,176 | 4.99M–5.55M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang22](Run_056.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_055.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 869,429 | 804.99K–941.95K | 80.80 MB | 774,518 | 767.30K–882.13K | 80.77 MB |
| durable binary | 751,634 | 740.15K–773.90K | 83.78 MB | 775,529 | 738.88K–788.47K | 83.78 MB |
| durable sql | 466,551 | 453.17K–473.40K | 75.02 MB | 417,799 | 399.02K–460.41K | 75.02 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang22](Run_056.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_055.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 1,125,752 | 1.12M–1.18M | 836.60 MB | 1,129,819 | 1.09M–1.17M | 836.65 MB |
| durable sql | 737,355 | 580.46K–759.25K | 772.26 MB | 825,502 | 779.82K–877.82K | 772.15 MB |
| durable binary | 778,499 | 703.84K–800.32K | 837.80 MB | 780,801 | 775.58K–798.46K | 837.80 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_056](Run_056.md) | clang22 · 2026-07-02T05:12:25Z |
| [Run_055](Run_055.md) | gcc16 · 2026-07-02T05:09:07Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
