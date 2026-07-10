<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler. Split into non-durable / durable @1M / durable @10M, each sorted by median ops/sec (descending)._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · fedora-44 · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang22](Run_150.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_149.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 4 flags, non-durable | 5,218,671 | 4.92M–5.48M | 0 B | 5,379,538 | 5.05M–5.54M | 0 B |
| 0 flags, non-durable | 5,320,035 | 4.97M–5.42M | 0 B | 5,226,537 | 4.98M–5.48M | 0 B |
| 2 flags, non-durable | 5,306,018 | 5.07M–5.44M | 0 B | 5,270,148 | 4.86M–5.43M | 0 B |
| 6 flags, non-durable | 5,278,794 | 5.09M–5.38M | 0 B | 5,223,539 | 5.09M–5.47M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang22](Run_150.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_149.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 1,046,258 | 900.46K–1.09M | 80.81 MB | 1,040,015 | 890.98K–1.05M | 80.79 MB |
| durable binary | 787,658 | 779.00K–797.60K | 83.78 MB | 748,516 | 698.69K–807.66K | 83.78 MB |
| durable sql | 460,236 | 420.58K–488.05K | 75.03 MB | 387,325 | 386.52K–452.96K | 75.02 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang22](Run_150.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_149.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 1,296,187 | 1.27M–1.34M | 836.61 MB | 1,283,879 | 1.28M–1.32M | 836.59 MB |
| durable binary | 841,680 | 741.20K–854.15K | 837.80 MB | 854,146 | 848.71K–855.76K | 837.80 MB |
| durable sql | 818,829 | 794.71K–822.73K | 771.97 MB | 795,490 | 785.97K–804.12K | 771.69 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_150](Run_150.md) | clang22 · 2026-07-07T17:03:18Z |
| [Run_149](Run_149.md) | gcc16 · 2026-07-07T17:00:02Z |

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

## jac313-005 — Intel(R) Core(TM) Ultra 7 265 · 14 cores · 42 GB · fedora-44 · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang22](Run_241.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_240.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 4 flags, non-durable | 6,787,716 | 5.94M–6.85M | 0 B | 5,800,433 | 5.50M–6.84M | 0 B |
| 6 flags, non-durable | 6,605,199 | 5.84M–6.91M | 0 B | 5,808,093 | 5.63M–6.91M | 0 B |
| 0 flags, non-durable | 6,352,207 | 5.64M–6.85M | 0 B | 6,592,240 | 5.68M–6.97M | 0 B |
| 2 flags, non-durable | 6,577,150 | 5.90M–6.85M | 0 B | 6,164,659 | 5.68M–6.92M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang22](Run_241.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_240.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable sql | 1,724,557 | 455.01K–1.74M | 75.17 MB | 1,739,257 | 446.37K–1.74M | 75.16 MB |
| durable jtext | 1,204,169 | 1.11M–1.24M | 80.85 MB | 1,013,060 | 998.16K–1.11M | 80.82 MB |
| durable json | 1,152,823 | 1.14M–1.15M | 165.75 MB | 976,609 | 973.18K–1.02M | 165.73 MB |
| durable html | 1,091,720 | 1.05M–1.12M | 138.09 MB | 1,055,624 | 934.67K–1.17M | 138.07 MB |
| durable binary | 736,494 | 727.83K–745.33K | 83.78 MB | 758,069 | 749.43K–790.00K | 83.78 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang22](Run_241.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_240.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable sql | 1,728,073 | 789.81K–1.76M | 773.61 MB | 1,746,330 | 767.94K–1.75M | 773.49 MB |
| durable jtext | 1,483,583 | 1.44M–1.51M | 836.69 MB | 1,434,939 | 1.42M–1.48M | 836.69 MB |
| durable html | 1,212,981 | 1.18M–1.27M | 1.38 GB | 1,251,569 | 1.21M–1.30M | 1.38 GB |
| durable json | 1,216,312 | 1.19M–1.24M | 1.65 GB | 1,188,124 | 1.09M–1.22M | 1.65 GB |
| durable binary | 760,639 | 698.04K–786.65K | 837.80 MB | 790,564 | 781.67K–801.96K | 837.80 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_241](Run_241.md) | clang22 · 2026-07-09T02:14:22Z |
| [Run_240](Run_240.md) | gcc16 · 2026-07-09T02:10:36Z |

## jac313-006 — Intel(R) Core(TM) Ultra 7 265 · 14 cores · 42 GB · linuxmint-22.3 · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang20](Run_264.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_263.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 2 flags, non-durable | 5,437,667 | 5.01M–5.51M | 0 B | 5,003,939 | 4.71M–5.37M | 0 B |
| 0 flags, non-durable | 5,414,604 | 4.78M–5.48M | 0 B | 4,973,011 | 4.85M–5.45M | 0 B |
| 4 flags, non-durable | 5,205,365 | 4.78M–5.51M | 0 B | 5,179,046 | 4.75M–5.43M | 0 B |
| 6 flags, non-durable | 5,118,736 | 4.90M–5.52M | 0 B | 5,003,580 | 4.74M–5.42M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang20](Run_264.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_263.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 676,448 | 595.74K–748.44K | 83.78 MB | 708,126 | 676.71K–803.54K | 83.78 MB |
| durable jtext | 613,480 | 575.87K–640.78K | 80.82 MB | 666,525 | 653.73K–671.09K | 80.82 MB |
| durable html | 498,333 | 446.10K–507.69K | 138.04 MB | 523,316 | 522.30K–543.03K | 138.06 MB |
| durable sql | 440,400 | 420.37K–463.97K | 75.12 MB | 484,848 | 459.49K–487.01K | 75.12 MB |
| durable json | 436,392 | 413.81K–454.21K | 165.69 MB | 448,705 | 447.47K–459.83K | 165.68 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang20](Run_264.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_263.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 776,752 | 720.19K–784.20K | 836.35 MB | 753,009 | 738.75K–761.84K | 836.34 MB |
| durable binary | 742,680 | 733.06K–782.91K | 837.80 MB | 764,530 | 753.10K–797.80K | 837.80 MB |
| durable sql | 600,218 | 584.90K–604.67K | 773.22 MB | 641,112 | 621.71K–652.99K | 773.11 MB |
| durable html | 526,493 | 525.19K–536.14K | 1.38 GB | 545,610 | 526.51K–547.33K | 1.38 GB |
| durable json | 474,189 | 465.46K–484.35K | 1.65 GB | 473,628 | 473.34K–475.06K | 1.65 GB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_264](Run_264.md) | clang20 · 2026-07-10T03:47:53Z |
| [Run_263](Run_263.md) | gcc15 · 2026-07-10T03:41:54Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
