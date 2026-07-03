<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler. Split into non-durable / durable @1M / durable @10M, each sorted by median ops/sec (descending)._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · fedora-44 · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang22](Run_021.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_020.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 2 flags, non-durable | 5,272,514 | 5.19M–5.51M | 0 B | 5,434,634 | 5.13M–5.63M | 0 B |
| 4 flags, non-durable | 5,387,611 | 5.26M–5.48M | 0 B | 5,319,420 | 4.97M–5.56M | 0 B |
| 0 flags, non-durable | 5,373,761 | 5.23M–5.49M | 0 B | 5,282,734 | 4.94M–5.51M | 0 B |
| 6 flags, non-durable | 5,299,688 | 5.17M–5.48M | 0 B | 5,233,738 | 5.15M–5.52M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang22](Run_021.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_020.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 787,704 | 658.93K–892.37K | 80.80 MB | 858,666 | 738.05K–866.71K | 80.80 MB |
| durable binary | 733,628 | 553.53K–772.91K | 83.78 MB | 628,792 | 553.25K–728.37K | 83.78 MB |
| durable sql | 446,796 | 428.19K–453.86K | 75.04 MB | 434,091 | 423.37K–467.39K | 75.01 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang22](Run_021.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_020.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 1,195,506 | 1.16M–1.23M | 836.64 MB | 1,250,018 | 1.24M–1.27M | 836.46 MB |
| durable sql | 814,887 | 750.52K–838.64K | 772.40 MB | 822,004 | 777.99K–851.46K | 772.07 MB |
| durable binary | 747,771 | 711.52K–806.75K | 837.80 MB | 753,895 | 657.23K–796.92K | 837.80 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_021](Run_021.md) | clang22 · 2026-07-03T05:15:56Z |
| [Run_020](Run_020.md) | gcc16 · 2026-07-03T05:12:37Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
