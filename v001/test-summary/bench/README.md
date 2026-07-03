<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler. Split into non-durable / durable @1M / durable @10M, each sorted by median ops/sec (descending)._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · fedora-44 · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang22](Run_008.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_007.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 6 flags, non-durable | 5,303,706 | 5.21M–5.50M | 0 B | 5,609,261 | 5.32M–5.71M | 0 B |
| 0 flags, non-durable | 5,407,860 | 5.14M–5.50M | 0 B | 5,306,797 | 3.15M–5.71M | 0 B |
| 4 flags, non-durable | 5,287,670 | 5.05M–5.41M | 0 B | 5,357,789 | 5.13M–5.64M | 0 B |
| 2 flags, non-durable | 5,323,338 | 5.12M–5.51M | 0 B | 5,343,377 | 5.14M–5.68M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang22](Run_008.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_007.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 806,519 | 779.53K–850.75K | 80.82 MB | 819,525 | 757.10K–876.89K | 80.78 MB |
| durable binary | 546,611 | 539.72K–694.08K | 83.78 MB | 769,717 | 681.34K–799.88K | 83.78 MB |
| durable sql | 433,968 | 411.63K–437.24K | 75.03 MB | 421,885 | 374.69K–446.39K | 75.02 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang22](Run_008.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_007.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 1,155,761 | 1.15M–1.28M | 836.61 MB | 1,202,127 | 1.16M–1.20M | 836.51 MB |
| durable sql | 775,025 | 749.89K–776.52K | 772.06 MB | 751,067 | 621.14K–812.73K | 771.75 MB |
| durable binary | 642,937 | 594.89K–725.44K | 837.80 MB | 648,639 | 645.83K–724.96K | 837.80 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_008](Run_008.md) | clang22 · 2026-07-03T04:47:45Z |
| [Run_007](Run_007.md) | gcc16 · 2026-07-03T04:44:17Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
