<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler. Split into non-durable / durable @1M / durable @10M, each sorted by median ops/sec (descending)._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 11 GB · fedora-44 · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang22](Run_163.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_162.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 6 flags, non-durable | 5,239,953 | 5.10M–5.36M | 0 B | 5,382,000 | 4.96M–5.51M | 0 B |
| 0 flags, non-durable | 5,308,080 | 5.09M–5.38M | 0 B | 5,259,911 | 4.86M–5.49M | 0 B |
| 2 flags, non-durable | 5,218,084 | 4.97M–5.41M | 0 B | 5,249,913 | 4.89M–5.44M | 0 B |
| 4 flags, non-durable | 5,217,306 | 5.15M–5.37M | 0 B | 5,185,509 | 4.85M–5.51M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang22](Run_163.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_162.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 1,045,130 | 1.04M–1.05M | 80.80 MB | 967,657 | 910.37K–1.01M | 80.79 MB |
| durable binary | 766,605 | 759.44K–799.88K | 83.78 MB | 758,805 | 701.01K–770.13K | 83.78 MB |
| durable sql | 460,861 | 420.36K–474.56K | 75.03 MB | 435,095 | 417.82K–472.39K | 74.98 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang22](Run_163.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_162.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable jtext | 1,286,322 | 1.26M–1.30M | 836.60 MB | 1,275,471 | 1.26M–1.36M | 836.42 MB |
| durable sql | 872,955 | 840.49K–893.54K | 772.06 MB | 890,811 | 876.98K–906.94K | 771.80 MB |
| durable binary | 857,437 | 733.12K–869.92K | 837.80 MB | 862,910 | 857.11K–879.94K | 837.80 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_163](Run_163.md) | clang22 · 2026-07-07T17:30:07Z |
| [Run_162](Run_162.md) | gcc16 · 2026-07-07T17:26:56Z |

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

## jac313-005 — Intel(R) Core(TM) Ultra 7 265 · 14 cores · 42 GB · fedora-44 · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang22](Run_251.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_250.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 6 flags, non-durable | 6,684,398 | 5.98M–6.84M | 0 B | 6,758,327 | 5.55M–6.92M | 0 B |
| 0 flags, non-durable | 6,320,925 | 5.73M–6.79M | 0 B | 6,076,898 | 5.54M–6.87M | 0 B |
| 4 flags, non-durable | 6,287,796 | 5.93M–6.86M | 0 B | 6,046,716 | 5.60M–6.74M | 0 B |
| 2 flags, non-durable | 5,975,128 | 5.71M–6.90M | 0 B | 5,773,883 | 5.45M–6.74M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang22](Run_251.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_250.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable sql | 1,641,866 | 458.57K–1.77M | 75.14 MB | 1,767,053 | 454.98K–1.81M | 75.12 MB |
| durable jtext | 1,096,654 | 956.95K–1.12M | 80.84 MB | 1,023,069 | 938.46K–1.06M | 80.85 MB |
| durable json | 1,084,594 | 1.02M–1.10M | 165.73 MB | 887,600 | 817.47K–1.01M | 165.73 MB |
| durable html | 1,067,684 | 1.04M–1.07M | 138.08 MB | 1,000,844 | 842.90K–1.02M | 138.06 MB |
| durable binary | 732,324 | 703.48K–739.76K | 83.78 MB | 475,152 | 440.25K–500.17K | 83.78 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang22](Run_251.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc16](Run_250.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable sql | 1,731,121 | 774.05K–1.77M | 773.25 MB | 1,752,771 | 830.63K–1.79M | 773.32 MB |
| durable jtext | 1,366,217 | 1.31M–1.39M | 836.94 MB | 1,367,692 | 1.31M–1.46M | 836.79 MB |
| durable html | 1,193,105 | 1.18M–1.26M | 1.38 GB | 1,250,731 | 1.22M–1.27M | 1.38 GB |
| durable json | 1,245,645 | 1.17M–1.27M | 1.65 GB | 1,130,267 | 1.12M–1.15M | 1.65 GB |
| durable binary | 750,796 | 717.30K–756.28K | 837.80 MB | 543,853 | 543.44K–750.35K | 837.80 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_251](Run_251.md) | clang22 · 2026-07-09T03:04:23Z |
| [Run_250](Run_250.md) | gcc16 · 2026-07-09T03:00:20Z |

## jac313-006 — Intel(R) Core(TM) Ultra 7 265 · 14 cores · 42 GB · linuxmint-22.3 · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang20](Run_277.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_276.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 6 flags, non-durable | 5,418,882 | 4.74M–5.51M | 0 B | 5,059,255 | 4.96M–5.45M | 0 B |
| 0 flags, non-durable | 5,398,927 | 4.80M–5.51M | 0 B | 5,321,109 | 4.77M–5.51M | 0 B |
| 2 flags, non-durable | 5,349,293 | 4.72M–5.56M | 0 B | 4,956,412 | 4.85M–5.47M | 0 B |
| 4 flags, non-durable | 5,135,703 | 4.82M–5.58M | 0 B | 5,099,650 | 4.88M–5.46M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang20](Run_277.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_276.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 656,718 | 655.74K–661.42K | 83.78 MB | 626,203 | 624.78K–642.56K | 83.78 MB |
| durable jtext | 656,599 | 647.26K–680.05K | 80.84 MB | 645,600 | 633.05K–659.56K | 80.84 MB |
| durable html | 311,621 | 303.06K–319.18K | 138.02 MB | 464,741 | 457.25K–471.52K | 138.05 MB |
| durable sql | 456,674 | 421.01K–463.64K | 75.13 MB | 410,840 | 408.25K–440.23K | 75.13 MB |
| durable json | 276,189 | 259.39K–400.79K | 165.69 MB | 409,760 | 365.63K–412.98K | 165.68 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang20](Run_277.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_276.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 779,164 | 750.54K–818.80K | 837.80 MB | 681,708 | 676.02K–692.43K | 837.80 MB |
| durable jtext | 758,180 | 734.87K–767.49K | 836.37 MB | 683,108 | 679.01K–684.31K | 836.43 MB |
| durable sql | 644,269 | 631.07K–655.76K | 773.29 MB | 580,802 | 575.85K–595.14K | 773.32 MB |
| durable html | 542,523 | 493.70K–546.32K | 1.38 GB | 471,927 | 428.66K–501.39K | 1.38 GB |
| durable json | 468,244 | 450.87K–500.52K | 1.65 GB | 392,507 | 349.21K–428.48K | 1.65 GB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_277](Run_277.md) | clang20 · 2026-07-10T04:36:02Z |
| [Run_276](Run_276.md) | gcc15 · 2026-07-10T04:29:26Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
