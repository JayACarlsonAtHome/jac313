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

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
