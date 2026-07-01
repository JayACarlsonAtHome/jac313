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

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
