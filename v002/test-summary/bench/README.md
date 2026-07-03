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

| config | [clang21](Run_140.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_139.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 2 flags, non-durable | 9,483,956 | 9.03M–9.56M | 0 B | 7,487,748 | 6.90M–8.12M | 0 B |
| 4 flags, non-durable | 9,363,599 | 8.90M–9.76M | 0 B | 7,366,864 | 7.12M–7.90M | 0 B |
| 0 flags, non-durable | 9,354,183 | 8.97M–9.60M | 0 B | 8,043,914 | 7.12M–8.33M | 0 B |
| 6 flags, non-durable | 9,263,655 | 8.53M–9.57M | 0 B | 7,320,789 | 7.09M–7.95M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang21](Run_140.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_139.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 746,481 | 714.93K–747.91K | 83.78 MB | 740,004 | 732.21K–751.86K | 83.78 MB |
| durable jtext | 653,690 | 588.66K–659.31K | 80.78 MB | 601,520 | 524.67K–701.12K | 80.77 MB |
| durable sql | 477,076 | 457.52K–488.03K | 75.03 MB | 481,706 | 464.82K–485.85K | 75.02 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang21](Run_140.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_139.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 751,695 | 708.47K–771.98K | 837.80 MB | 747,569 | 702.08K–784.82K | 837.80 MB |
| durable jtext | 624,291 | 623.62K–655.58K | 836.16 MB | 651,281 | 635.68K–679.47K | 836.63 MB |
| durable sql | 580,240 | 579.81K–580.81K | 772.25 MB | 533,834 | 406.38K–575.45K | 772.14 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_140](Run_140.md) | clang21 · 2026-07-03T21:55:36Z |
| [Run_139](Run_139.md) | gcc15 · 2026-07-03T21:51:59Z |

## jac313-003 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · linuxmint-22.3 · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang20](Run_079.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_078.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 2 flags, non-durable | 4,715,013 | 4.54M–4.79M | 0 B | 4,520,073 | 4.39M–4.57M | 0 B |
| 4 flags, non-durable | 4,712,353 | 4.46M–4.79M | 0 B | 4,550,817 | 4.42M–4.65M | 0 B |
| 0 flags, non-durable | 4,698,213 | 4.54M–4.78M | 0 B | 4,497,756 | 3.05M–4.57M | 0 B |
| 6 flags, non-durable | 4,691,016 | 4.59M–4.75M | 0 B | 4,548,102 | 4.39M–4.61M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang20](Run_079.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_078.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 640,581 | 581.13K–643.60K | 83.78 MB | 654,913 | 613.23K–675.71K | 83.78 MB |
| durable jtext | 563,847 | 500.26K–590.90K | 80.79 MB | 576,645 | 504.53K–595.11K | 80.79 MB |
| durable sql | 434,235 | 403.53K–439.06K | 75.04 MB | 432,448 | 414.31K–438.10K | 75.03 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang20](Run_079.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_078.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 723,150 | 715.34K–724.37K | 837.80 MB | 729,681 | 705.47K–753.11K | 837.80 MB |
| durable jtext | 615,208 | 605.22K–625.61K | 836.53 MB | 605,512 | 589.80K–615.78K | 836.45 MB |
| durable sql | 536,994 | 524.89K–551.20K | 772.29 MB | 544,406 | 532.89K–546.31K | 772.16 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_079](Run_079.md) | clang20 · 2026-07-03T09:30:02Z |
| [Run_078](Run_078.md) | gcc15 · 2026-07-03T09:25:47Z |

## jac313-004 — Intel(R) Core(TM) Ultra 7 265 · 20 cores · 62 GB · rhel-9.8 · ssd


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang21](Run_103.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_102.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 4 flags, non-durable | 14,649,270 | 11.19M–15.04M | 0 B | 12,765,779 | 11.25M–15.71M | 0 B |
| 2 flags, non-durable | 13,440,388 | 11.10M–15.21M | 0 B | 13,423,641 | 10.45M–15.89M | 0 B |
| 0 flags, non-durable | 11,868,855 | 10.86M–15.13M | 0 B | 13,061,789 | 11.11M–15.92M | 0 B |
| 6 flags, non-durable | 12,792,863 | 10.79M–15.04M | 0 B | 11,450,885 | 10.75M–15.56M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang21](Run_103.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_102.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 2,556,904 | 2.41M–2.58M | 83.78 MB | 2,594,714 | 2.55M–2.61M | 83.78 MB |
| durable jtext | 2,002,038 | 1.99M–2.04M | 80.79 MB | 2,043,991 | 2.03M–2.10M | 80.79 MB |
| durable sql | 1,459,447 | 1.43M–1.50M | 75.00 MB | 1,456,859 | 1.45M–1.48M | 75.01 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang21](Run_103.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_102.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 2,711,242 | 2.64M–2.72M | 837.80 MB | 2,634,850 | 2.63M–2.69M | 837.80 MB |
| durable jtext | 2,096,429 | 2.08M–2.12M | 836.38 MB | 2,150,736 | 2.14M–2.16M | 836.24 MB |
| durable sql | 1,551,152 | 1.55M–1.56M | 772.90 MB | 1,560,586 | 1.56M–1.57M | 772.77 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_103](Run_103.md) | clang21 · 2026-07-03T10:30:09Z |
| [Run_102](Run_102.md) | gcc15 · 2026-07-03T10:28:47Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
