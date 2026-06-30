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

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
