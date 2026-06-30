<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# Test results

_Generated from `results.db` by `jac313_test_cli --report`._

## Machines

| machine | CPU | Speed | P.Cores | T.Cores | RAM | Disk | OS |
|---|---|--:|--:|--:|--:|---|---|
| jac313-001 | Intel(R) Core(TM) Ultra 7 265 | - | 12 | 12 | 23 GB | x7k | Fedora Linux 44 (Workstation Edition) |
| jac313-002 | Intel(R) Core(TM) Ultra 7 265 | - | 12 | 12 | 38 GB | x7k | Red Hat Enterprise Linux 10.2 (Coughlan) |

\* P.Cores = Physical Cores; T.Cores = Threading Cores

## Safeness

| machine | verdict | ctest | smoke | bench | mem | hel | drd |
|---|---|--:|--:|--:|:--:|:--:|:--:|
| jac313-001 | ✅ PRESUMED SAFE | 72/0 | 232/0 | 20/0 | ✅ | ✅ | ✅ |
| jac313-002 | ✅ PRESUMED SAFE | 72/0 | 232/0 | 20/0 | ✅ | ✅ | ✅ |

\* ✅ PRESUMED SAFE = builds + ctest/smoke/bench 0-fail + memcheck/helgrind/drd clean, both compilers. Presumed, not proven.

### jac313-001 — gate detail

| gate | pass | fail | memcheck | helgrind | drd |
|---|--:|--:|:--:|:--:|:--:|
| ctest | 72 | 0 | ✅ | ✅ | ✅ |
| smoke | 232 | 0 | n/a | n/a | n/a |
| bench | 20 | 0 | n/a | n/a | n/a |

### jac313-002 — gate detail

| gate | pass | fail | memcheck | helgrind | drd |
|---|--:|--:|:--:|:--:|:--:|
| ctest | 72 | 0 | ✅ | ✅ | ✅ |
| smoke | 232 | 0 | n/a | n/a | n/a |
| bench | 20 | 0 | n/a | n/a | n/a |

| area | runs | rows |
|---|--:|--:|
| [compilers](compiler/README.md) | — | — |
| [ctest](ctest/README.md) | 4 | 144 |
| [smoke](smoke/README.md) | 4 | 464 |
| [bench](bench/README.md) | 4 | 40 |
| [verify-lite](verify-lite/README.md) | 1 | 31 |
| [verify](verify/README.md) | 4 | 240 |
| [compiler-build-times](compiler-build-times/README.md) | 12 | 296 |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
