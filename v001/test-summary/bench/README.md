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

## jac313-002 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · rhel-10.2 · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang21](Run_127.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_126.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 0 flags, non-durable | 9,242,117 | 8.90M–9.74M | 0 B | 6,280,291 | 5.87M–6.55M | 0 B |
| 2 flags, non-durable | 9,234,381 | 9.00M–9.69M | 0 B | 6,562,452 | 6.23M–6.73M | 0 B |
| 4 flags, non-durable | 9,182,549 | 8.91M–9.57M | 0 B | 7,475,593 | 6.55M–7.87M | 0 B |
| 6 flags, non-durable | 9,155,985 | 8.66M–9.55M | 0 B | 7,608,716 | 7.31M–7.89M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang21](Run_127.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_126.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 853,714 | 841.67K–900.48K | 83.78 MB | 852,227 | 850.69K–853.65K | 83.78 MB |
| durable jtext | 797,118 | 762.86K–804.03K | 80.78 MB | 775,695 | 706.10K–798.57K | 80.75 MB |
| durable sql | 574,099 | 558.24K–575.46K | 75.05 MB | 559,121 | 558.02K–572.48K | 75.01 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang21](Run_127.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_126.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 939,580 | 825.50K–947.33K | 837.80 MB | 958,542 | 886.91K–965.73K | 837.80 MB |
| durable jtext | 790,111 | 788.61K–826.08K | 836.12 MB | 773,727 | 720.84K–791.10K | 836.06 MB |
| durable sql | 688,598 | 679.17K–701.73K | 772.31 MB | 705,485 | 694.48K–760.47K | 772.04 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_127](Run_127.md) | clang21 · 2026-07-03T21:39:06Z |
| [Run_126](Run_126.md) | gcc15 · 2026-07-03T21:36:03Z |

## jac313-003 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · linuxmint-22.3 · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang20](Run_054.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_053.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 2 flags, non-durable | 4,634,592 | 4.42M–4.68M | 0 B | 4,500,495 | 4.34M–4.55M | 0 B |
| 4 flags, non-durable | 4,608,735 | 4.40M–4.70M | 0 B | 4,470,598 | 4.35M–4.56M | 0 B |
| 6 flags, non-durable | 4,592,722 | 4.43M–4.66M | 0 B | 4,515,227 | 4.39M–4.54M | 0 B |
| 0 flags, non-durable | 4,571,279 | 4.45M–4.63M | 0 B | 4,485,203 | 4.42M–4.55M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang20](Run_054.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_053.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 510,979 | 490.79K–520.94K | 83.78 MB | 504,852 | 492.78K–517.74K | 83.78 MB |
| durable jtext | 500,440 | 477.55K–502.73K | 80.81 MB | 417,523 | 405.19K–475.14K | 80.79 MB |
| durable sql | 357,638 | 343.56K–375.81K | 75.07 MB | 341,379 | 332.58K–347.44K | 75.07 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang20](Run_054.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_053.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 743,266 | 738.43K–745.53K | 837.80 MB | 733,927 | 718.66K–737.12K | 837.80 MB |
| durable sql | 456,657 | 419.13K–471.72K | 772.65 MB | 516,640 | 479.52K–522.90K | 772.58 MB |
| durable jtext | 483,848 | 483.12K–514.39K | 836.67 MB | 493,886 | 467.22K–499.13K | 836.61 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_054](Run_054.md) | clang20 · 2026-07-03T08:30:56Z |
| [Run_053](Run_053.md) | gcc15 · 2026-07-03T08:26:16Z |

## jac313-004 — Intel(R) Core(TM) Ultra 7 265 · 20 cores · 62 GB · rhel-9.8 · ssd


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang21](Run_090.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_089.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 6 flags, non-durable | 12,065,873 | 11.00M–15.01M | 0 B | 15,043,162 | 11.32M–15.93M | 0 B |
| 4 flags, non-durable | 14,871,199 | 11.81M–15.18M | 0 B | 11,620,606 | 11.13M–15.97M | 0 B |
| 0 flags, non-durable | 14,214,262 | 11.78M–15.37M | 0 B | 12,638,618 | 11.29M–15.57M | 0 B |
| 2 flags, non-durable | 12,056,406 | 11.00M–15.58M | 0 B | 11,370,100 | 10.50M–15.72M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang21](Run_090.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_089.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 2,542,110 | 2.49M–2.54M | 83.78 MB | 2,568,970 | 2.57M–2.60M | 83.78 MB |
| durable jtext | 1,984,107 | 1.94M–2.04M | 80.81 MB | 2,053,047 | 2.02M–2.09M | 80.80 MB |
| durable sql | 1,442,929 | 1.44M–1.45M | 75.05 MB | 1,492,332 | 1.48M–1.51M | 75.03 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang21](Run_090.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_089.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 2,696,195 | 2.66M–2.70M | 837.80 MB | 2,767,865 | 2.68M–2.78M | 837.80 MB |
| durable jtext | 2,117,705 | 2.10M–2.12M | 836.34 MB | 2,149,572 | 2.12M–2.16M | 836.23 MB |
| durable sql | 1,542,954 | 1.54M–1.55M | 773.22 MB | 1,560,672 | 1.56M–1.56M | 773.18 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_090](Run_090.md) | clang21 · 2026-07-03T10:22:13Z |
| [Run_089](Run_089.md) | gcc15 · 2026-07-03T10:20:53Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
