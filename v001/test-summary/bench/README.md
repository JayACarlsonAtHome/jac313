<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler. Split into non-durable / durable @1M / durable @10M, each sorted by median ops/sec (descending)._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 12 cores · 38 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang21](Run_008.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_007.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 6 flags, non-durable | 6,357,350 | 5.79M–6.52M | 0 B | 6,204,494 | 5.52M–6.41M | 0 B |
| 0 flags, non-durable | 5,741,467 | 5.50M–6.44M | 0 B | 5,991,305 | 5.36M–6.37M | 0 B |
| 2 flags, non-durable | 5,800,941 | 5.70M–6.40M | 0 B | 5,716,564 | 5.32M–6.36M | 0 B |
| 4 flags, non-durable | 5,754,716 | 5.64M–6.57M | 0 B | 5,653,591 | 5.42M–6.42M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang21](Run_008.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_007.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 2,044,986 | 1.92M–2.10M | 80.83 MB | 2,014,273 | 1.98M–2.06M | 80.81 MB |
| durable binary | 747,873 | 747.00K–752.00K | 83.78 MB | 753,647 | 721.41K–760.72K | 83.78 MB |
| durable sql | 437,798 | 422.53K–449.31K | 75.14 MB | 439,713 | 425.51K–454.95K | 75.12 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang21](Run_008.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_007.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 2,094,429 | 2.09M–2.12M | 836.15 MB | 2,106,667 | 2.07M–2.12M | 836.18 MB |
| durable binary | 810,318 | 778.96K–813.23K | 837.80 MB | 805,842 | 717.95K–807.10K | 837.80 MB |
| durable sql | 658,557 | 648.61K–664.91K | 773.15 MB | 679,719 | 676.82K–679.75K | 772.96 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_008](Run_008.md) | clang21 · 2026-06-30T06:22:26Z |
| [Run_007](Run_007.md) | gcc15 · 2026-06-30T06:19:24Z |

## jac313-002 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 19 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang21](Run_022.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_021.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 2 flags, non-durable | 9,251,040 | 8.46M–9.54M | 0 B | 8,334,857 | 8.03M–9.03M | 0 B |
| 6 flags, non-durable | 9,080,928 | 8.57M–9.59M | 0 B | 8,688,537 | 8.08M–9.03M | 0 B |
| 4 flags, non-durable | 9,076,978 | 8.48M–9.64M | 0 B | 8,616,046 | 7.59M–8.90M | 0 B |
| 0 flags, non-durable | 8,886,307 | 8.53M–9.38M | 0 B | 8,397,652 | 7.87M–9.03M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang21](Run_022.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_021.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 805,023 | 785.85K–852.27K | 83.78 MB | 855,901 | 786.83K–866.65K | 83.78 MB |
| durable jtext | 694,323 | 682.24K–728.82K | 80.78 MB | 657,996 | 636.97K–711.21K | 80.76 MB |
| durable sql | 475,607 | 459.32K–486.41K | 75.04 MB | 472,209 | 469.99K–484.33K | 75.02 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang21](Run_022.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_021.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 878,994 | 870.77K–887.69K | 837.80 MB | 869,708 | 865.23K–879.80K | 837.80 MB |
| durable jtext | 739,960 | 684.00K–740.46K | 836.07 MB | 729,755 | 700.42K–753.44K | 836.10 MB |
| durable sql | 640,270 | 639.31K–647.32K | 772.37 MB | 645,246 | 642.39K–648.03K | 772.27 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_022](Run_022.md) | clang21 · 2026-07-01T00:24:07Z |
| [Run_021](Run_021.md) | gcc15 · 2026-07-01T00:21:00Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
