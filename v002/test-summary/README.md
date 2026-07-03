<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# Test results

_Generated from `results.db` by `jac313_test_cli --report`._

**jac313 version:** v002 (runs, deletes, and safeness are scoped by this)

## Machines

| machine | CPU | Speed | P.Cores | T.Cores | RAM | Disk | OS |
|---|---|--:|--:|--:|--:|---|---|
| jac313-001 | Intel(R) Core(TM) Ultra 7 265 | 2.4 GHz | 4 | 4 | 11 GB | x7k | fedora-44 |
| jac313-002 | Intel(R) Core(TM) Ultra 7 265 | 2.4 GHz | 4 | 4 | 11 GB | x7k | rhel-10.2 |
| jac313-003 | Intel(R) Core(TM) Ultra 7 265 | 2.4 GHz | 4 | 4 | 11 GB | x7k | linuxmint-22.3 |

\* P.Cores = Physical Cores; T.Cores = Threading Cores

## Safeness

| machine | verdict | ctest | smoke | bench | mem | hel | drd |
|---|---|--:|--:|--:|:--:|:--:|:--:|
| jac313-001 | ✅ PRESUMED SAFE | 76/0 | 232/0 | 20/0 | ✅ | ✅ | ✅ |
| jac313-002 | ✅ PRESUMED SAFE | 76/0 | 232/0 | 20/0 | ✅ | ✅ | ✅ |
| jac313-003 | ✅ PRESUMED SAFE | 76/0 | 232/0 | 20/0 | ✅ | ✅ | ✅ |

\* ✅ PRESUMED SAFE = builds + ctest/smoke/bench 0-fail + memcheck/helgrind/drd clean, both compilers. Presumed, not proven.

### jac313-001 — gate detail

| gate | pass | fail | memcheck | helgrind | drd |
|---|--:|--:|:--:|:--:|:--:|
| ctest | 76 | 0 | ✅ | ✅ | ✅ |
| smoke | 232 | 0 | n/a | n/a | n/a |
| bench | 20 | 0 | n/a | n/a | n/a |

### jac313-002 — gate detail

| gate | pass | fail | memcheck | helgrind | drd |
|---|--:|--:|:--:|:--:|:--:|
| ctest | 76 | 0 | ✅ | ✅ | ✅ |
| smoke | 232 | 0 | n/a | n/a | n/a |
| bench | 20 | 0 | n/a | n/a | n/a |

### jac313-003 — gate detail

| gate | pass | fail | memcheck | helgrind | drd |
|---|--:|--:|:--:|:--:|:--:|
| ctest | 76 | 0 | ✅ | ✅ | ✅ |
| smoke | 232 | 0 | n/a | n/a | n/a |
| bench | 20 | 0 | n/a | n/a | n/a |

| area | runs | rows |
|---|--:|--:|
| [compilers](compiler/README.md) | — | — |
| [ctest](ctest/README.md) | 6 | 228 |
| [smoke](smoke/README.md) | 6 | 696 |
| [bench](bench/README.md) | 6 | 60 |
| [verify](verify/README.md) | 6 | 360 |
| [compiler-build-times](compiler-build-times/README.md) | 7 | 247 |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
