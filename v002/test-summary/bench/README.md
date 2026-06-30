<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler. Split into non-durable / durable @1M / durable @10M, each sorted by median ops/sec (descending)._

## jac313-001 — Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz · 8 cores · 31 GB · Linux Mint 22.3 · ssd


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang20](Run_005.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_004.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 0 flags, non-durable | 3,777,429 | 3.56M–3.79M | 0 B | 3,385,680 | 3.16M–3.46M | 0 B |
| 2 flags, non-durable | 3,773,863 | 3.55M–3.80M | 0 B | 3,409,308 | 3.14M–3.50M | 0 B |
| 4 flags, non-durable | 3,771,912 | 3.55M–3.79M | 0 B | 3,408,688 | 3.17M–3.56M | 0 B |
| 6 flags, non-durable | 3,771,117 | 3.56M–3.80M | 0 B | 3,471,648 | 3.31M–3.54M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang20](Run_005.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_004.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 1,560,462 | 1.53M–1.67M | 83.78 MB | 1,522,151 | 1.43M–1.63M | 83.78 MB |
| durable jtext | 801,468 | 743.24K–810.40K | 80.86 MB | 722,941 | 670.70K–729.10K | 80.88 MB |
| durable sql | 477,992 | 469.57K–556.68K | 75.01 MB | 506,861 | 498.58K–521.55K | 75.04 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang20](Run_005.md)<br>median ops/sec | clang20<br>band | clang20<br>size | [gcc15](Run_004.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 1,764,305 | 1.70M–1.78M | 837.80 MB | 1,812,570 | 1.77M–1.82M | 837.80 MB |
| durable jtext | 823,589 | 809.26K–825.37K | 837.13 MB | 717,938 | 688.84K–721.83K | 837.52 MB |
| durable sql | 541,127 | 538.04K–552.73K | 772.53 MB | 544,368 | 536.06K–564.76K | 772.59 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_005](Run_005.md) | clang20 · 2026-06-30T16:47:35Z |
| [Run_004](Run_004.md) | gcc15 · 2026-06-30T16:43:22Z |
| [Run_002](Run_002.md) | gcc15 · 2026-06-30T15:16:42Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
