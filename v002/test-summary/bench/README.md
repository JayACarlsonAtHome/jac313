<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# bench — throughput by machine

_Generated from `results.db`. Throughput is hardware-specific, so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, and footprint; latest run per compiler. Split into non-durable / durable @1M / durable @10M, each sorted by median ops/sec (descending)._

## jac313-001 — Intel(R) Core(TM) Ultra 7 265 · 4 cores · 19 GB · Red Hat Enterprise Linux 10.2 (Coughlan) · x7k


### Non-durable (flag sweep)  _(median ops/sec, descending)_

| config | [clang21](Run_045.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_044.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| 6 flags, non-durable | 5,281,031 | 5.02M–5.37M | 0 B | 4,988,147 | 4.83M–5.20M | 0 B |
| 2 flags, non-durable | 5,275,949 | 5.13M–5.38M | 0 B | 5,141,792 | 4.91M–5.23M | 0 B |
| 4 flags, non-durable | 5,273,734 | 4.90M–5.39M | 0 B | 5,069,533 | 4.87M–5.21M | 0 B |
| 0 flags, non-durable | 5,249,568 | 5.02M–5.32M | 0 B | 5,015,605 | 4.87M–5.24M | 0 B |

### Durable @ 1M events  _(median ops/sec, descending)_

| config | [clang21](Run_045.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_044.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 779,655 | 776.49K–795.32K | 83.78 MB | 780,879 | 747.46K–798.49K | 83.78 MB |
| durable jtext | 693,743 | 673.63K–739.58K | 80.81 MB | 713,143 | 681.21K–713.65K | 80.79 MB |
| durable sql | 483,659 | 463.10K–491.19K | 75.02 MB | 460,620 | 426.48K–489.42K | 75.02 MB |

### Durable @ 10M events  _(median ops/sec, descending)_

| config | [clang21](Run_045.md)<br>median ops/sec | clang21<br>band | clang21<br>size | [gcc15](Run_044.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size |
|---|--:|:--:|--:|--:|:--:|--:|
| durable binary | 838,833 | 757.16K–838.97K | 837.80 MB | 839,741 | 773.07K–840.19K | 837.80 MB |
| durable jtext | 677,373 | 657.98K–680.33K | 836.57 MB | 715,669 | 680.63K–718.30K | 836.54 MB |
| durable sql | 646,741 | 643.41K–659.83K | 772.14 MB | 652,933 | 651.70K–658.45K | 772.14 MB |

**Runs** (newest first):

| run | compiler · recorded |
|---|---|
| [Run_045](Run_045.md) | clang21 · 2026-07-01T07:14:01Z |
| [Run_044](Run_044.md) | gcc15 · 2026-07-01T07:10:19Z |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
