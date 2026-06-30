# Test results

_Generated from `results.db` by `jac313_test_cli --report`._

## Machines

| machine | CPU | Speed | P.Cores | T.Cores | RAM | Disk | OS |
|---|---|--:|--:|--:|--:|---|---|
| jac313-001 | Intel(R) Core(TM) Ultra 7 265 | - | 12 | 12 | 38 GB | x7k | Red Hat Enterprise Linux 10.2 (Coughlan) |

\* P.Cores = Physical Cores; T.Cores = Threading Cores

## Safeness

| machine | verdict | ctest | smoke | bench | mem | hel | drd |
|---|---|--:|--:|--:|:--:|:--:|:--:|
| jac313-001 | ✅ PRESUMED SAFE | 64/0 | 232/0 | 20/0 | ✅ | ✅ | ✅ |

\* ✅ PRESUMED SAFE = builds + ctest/smoke/bench 0-fail + memcheck/helgrind/drd clean, both compilers. Presumed, not proven.

### jac313-001 — gate detail

| gate | pass | fail | memcheck | helgrind | drd |
|---|--:|--:|:--:|:--:|:--:|
| ctest | 64 | 0 | ✅ | ✅ | ✅ |
| smoke | 232 | 0 | n/a | n/a | n/a |
| bench | 20 | 0 | n/a | n/a | n/a |

| area | runs | rows |
|---|--:|--:|
| [compilers](compiler/README.md) | — | — |
| [ctest](ctest/README.md) | 2 | 64 |
| [smoke](smoke/README.md) | 2 | 232 |
| [bench](bench/README.md) | 2 | 20 |
| [verify-lite](verify-lite/README.md) | 1 | 31 |
| [verify](verify/README.md) | 2 | 120 |
| [compiler-build-times](compiler-build-times/README.md) | 5 | 121 |
