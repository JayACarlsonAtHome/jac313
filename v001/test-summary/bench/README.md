<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler. Split into non-durable / durable @1M / durable @10M, each sorted by median ops/sec (descending)._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 19 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang21](Run_032.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_031.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 6 flags, non-durable | 5,293,584 | 5.16M–5.35M | 0 B | 5,083,044 | 4.94M–5.19M | 0 B |
| 2 flags, non-durable | 5,252,878 | 5.11M–5.31M | 0 B | 4,995,085 | 4.74M–5.24M | 0 B |
| 4 flags, non-durable | 5,220,185 | 4.98M–5.34M | 0 B | 5,043,449 | 4.86M–5.20M | 0 B |
| 0 flags, non-durable | 5,209,350 | 5.11M–5.32M | 0 B | 5,169,906 | 4.84M–5.21M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang21](Run_032.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_031.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 787,187 | 769.98K–831.80K | 83.78 MB | 799,022 | 760.41K–813.74K | 83.78 MB |
| durable jtext | 656,991 | 647.61K–708.93K | 80.79 MB | 665,465 | 660.47K–720.25K | 80.76 MB |
| durable sql | 482,908 | 461.82K–507.82K | 75.05 MB | 484,869 | 483.50K–509.44K | 75.05 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang21](Run_032.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_031.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 813,681 | 793.90K–829.97K | 837.80 MB | 807,539 | 804.05K–831.06K | 837.80 MB |
| durable jtext | 714,882 | 671.50K–719.52K | 836.57 MB | 709,697 | 707.31K–725.48K | 836.40 MB |
| durable sql | 644,345 | 635.03K–645.98K | 772.27 MB | 647,997 | 630.49K–648.69K | 772.23 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_032](Run_032.md) | clang21 · 2026-07-01T06:58:33Z |
| [Run_031](Run_031.md) | gcc15 · 2026-07-01T06:54:50Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
