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

## jac313-002 — Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz · 8 cores · 31 GB · Linux Mint 22.3 · ssd


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang20](Run_045.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_044.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 6 flags, non-durable | 3,769,454 | 3.52M–3.79M | 0 B | 3,759,579 | 3.55M–3.78M | 0 B |
| 0 flags, non-durable | 3,767,999 | 3.55M–3.78M | 0 B | 3,768,577 | 3.53M–3.80M | 0 B |
| 4 flags, non-durable | 3,766,176 | 3.55M–3.78M | 0 B | 3,755,003 | 3.52M–3.79M | 0 B |
| 2 flags, non-durable | 3,764,325 | 3.54M–3.78M | 0 B | 3,765,770 | 3.55M–3.78M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang20](Run_045.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_044.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 1,598,703 | 1.53M–1.63M | 83.78 MB | 1,584,249 | 1.58M–1.62M | 83.78 MB |
| durable jtext | 825,068 | 798.90K–849.23K | 80.86 MB | 816,931 | 805.84K–826.57K | 80.85 MB |
| durable sql | 579,215 | 574.49K–580.91K | 75.02 MB | 591,143 | 577.46K–593.67K | 75.01 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang20](Run_045.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_044.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 1,726,869 | 1.68M–1.76M | 837.80 MB | 1,743,341 | 1.71M–1.74M | 837.80 MB |
| durable jtext | 843,537 | 839.76K–854.21K | 837.12 MB | 850,846 | 818.01K–853.41K | 837.09 MB |
| durable sql | 593,741 | 576.83K–597.34K | 772.58 MB | 603,599 | 600.97K–610.34K | 772.58 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_045](Run_045.md) | clang20 · 2026-07-02T08:16:52Z |
| [Run_044](Run_044.md) | gcc15 · 2026-07-02T08:13:10Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
