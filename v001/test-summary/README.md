<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# Test results

_Generated from `results.db` by `jac313_test_cli --report`._

**jac313 version:** v001 (runs, deletes, and safeness are scoped by this)

## Machines

| machine | CPU | Speed | P.Cores | T.Cores | RAM | Disk | OS |
|---|---|--:|--:|--:|--:|---|---|
| jac313-001 | Intel(R) Core(TM) Ultra 7 265 | 2.4 GHz | 4 | 4 | 11 GB | x7k | fedora-44 |
| jac313-002 | Intel(R) Core(TM) Ultra 7 265 | 2.4 GHz | 4 | 4 | 11 GB | x7k | rhel-10.2 |

\* P.Cores = Physical Cores; T.Cores = Threading Cores

## Safeness

| machine | verdict | ctest | smoke | bench | mem | hel | drd |
|---|---|--:|--:|--:|:--:|:--:|:--:|
| jac313-001 | ✅ PRESUMED SAFE | 68/0 | 232/0 | 20/0 | ✅ | ✅ | ✅ |
| jac313-002 | ✅ PRESUMED SAFE | 68/0 | 232/0 | 20/0 | ✅ | ✅ | ✅ |

\* ✅ PRESUMED SAFE = builds + ctest/smoke/bench 0-fail + memcheck/helgrind/drd clean, both compilers. Presumed, not proven.

### jac313-001 — gate detail

| gate | pass | fail | memcheck | helgrind | drd |
|---|--:|--:|:--:|:--:|:--:|
| ctest | 68 | 0 | ✅ | ✅ | ✅ |
| smoke | 232 | 0 | n/a | n/a | n/a |
| bench | 20 | 0 | n/a | n/a | n/a |

### jac313-002 — gate detail

| gate | pass | fail | memcheck | helgrind | drd |
|---|--:|--:|:--:|:--:|:--:|
| ctest | 68 | 0 | ✅ | ✅ | ✅ |
| smoke | 232 | 0 | n/a | n/a | n/a |
| bench | 20 | 0 | n/a | n/a | n/a |

| area | runs | rows |
|---|--:|--:|
| [compilers](compiler/README.md) | — | — |
| [ctest](ctest/README.md) | 4 | 136 |
| [smoke](smoke/README.md) | 4 | 464 |
| [bench](bench/README.md) | 4 | 40 |
| [verify](verify/README.md) | 4 | 240 |
| [compiler-build-times](compiler-build-times/README.md) | 10 | 190 |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
