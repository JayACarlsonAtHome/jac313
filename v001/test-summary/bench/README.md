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

## jac313-002 — Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz · 8 cores · 31 GB · Linux Mint 22.3 · ssd


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang20](Run_032.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_031.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 2 flags, non-durable | 3,768,222 | 3.54M–3.79M | 0 B | 3,756,040 | 3.56M–3.78M | 0 B |
| 6 flags, non-durable | 3,766,247 | 3.53M–3.78M | 0 B | 3,758,766 | 3.53M–3.79M | 0 B |
| 4 flags, non-durable | 3,765,936 | 3.54M–3.78M | 0 B | 3,756,984 | 3.54M–3.77M | 0 B |
| 0 flags, non-durable | 3,756,520 | 3.52M–3.77M | 0 B | 3,760,012 | 3.54M–3.79M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang20](Run_032.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_031.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 1,608,415 | 1.57M–1.63M | 83.78 MB | 1,630,965 | 1.58M–1.63M | 83.78 MB |
| durable jtext | 819,295 | 813.80K–829.34K | 80.85 MB | 822,329 | 783.18K–833.10K | 80.85 MB |
| durable sql | 573,968 | 573.77K–581.38K | 75.00 MB | 597,348 | 580.42K–600.60K | 75.01 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang20](Run_032.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_031.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 1,711,228 | 1.68M–1.73M | 837.80 MB | 1,750,561 | 1.70M–1.76M | 837.80 MB |
| durable jtext | 864,506 | 846.37K–867.11K | 837.07 MB | 831,881 | 829.29K–838.95K | 837.20 MB |
| durable sql | 591,407 | 588.27K–593.64K | 772.45 MB | 605,214 | 591.99K–607.81K | 772.45 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_032](Run_032.md) | clang20 · 2026-07-02T07:59:18Z |
| [Run_031](Run_031.md) | gcc15 · 2026-07-02T07:55:36Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
