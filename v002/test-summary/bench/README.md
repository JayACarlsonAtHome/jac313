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

## jac313-002 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · rhel-10.2 · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang21](Run_044.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_043.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 0 flags, non-durable | 9,330,359 | 8.85M–9.50M | 0 B | 8,209,467 | 7.76M–8.64M | 0 B |
| 2 flags, non-durable | 9,069,558 | 8.38M–9.49M | 0 B | 8,309,740 | 7.76M–8.67M | 0 B |
| 6 flags, non-durable | 8,990,317 | 8.44M–9.53M | 0 B | 8,505,948 | 7.65M–8.97M | 0 B |
| 4 flags, non-durable | 8,967,805 | 8.29M–9.33M | 0 B | 8,226,655 | 7.80M–8.57M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang21](Run_044.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_043.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 605,047 | 567.20K–720.51K | 83.78 MB | 593,421 | 564.29K–731.59K | 83.78 MB |
| durable jtext | 522,698 | 443.66K–529.97K | 80.77 MB | 533,643 | 455.21K–557.08K | 80.72 MB |
| durable sql | 462,640 | 452.67K–484.23K | 75.00 MB | 489,932 | 480.39K–499.46K | 75.00 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang21](Run_044.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_043.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 573,690 | 572.58K–628.13K | 837.80 MB | 614,510 | 585.25K–655.39K | 837.80 MB |
| durable jtext | 552,850 | 496.04K–562.75K | 836.12 MB | 520,617 | 498.15K–558.64K | 836.15 MB |
| durable sql | 535,562 | 519.21K–545.71K | 772.06 MB | 528,814 | 525.48K–531.44K | 771.96 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_044](Run_044.md) | clang21 · 2026-07-03T06:21:00Z |
| [Run_043](Run_043.md) | gcc15 · 2026-07-03T06:17:11Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
