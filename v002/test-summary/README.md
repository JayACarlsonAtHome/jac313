<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# Test results

_Generated from `results.db` by `jac313_test_cli --report`._

## Machines

| machine | CPU | Speed | P.Cores | T.Cores | RAM | Disk | OS |
|---|---|--:|--:|--:|--:|---|---|
| jac313-001 | Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz | 0.8–3.9 GHz | 4 | 8 | 31 GB | ssd | Linux Mint 22.3 |

\* P.Cores = Physical Cores; T.Cores = Threading Cores

## Safeness

| machine | verdict | ctest | smoke | bench | mem | hel | drd |
|---|---|--:|--:|--:|:--:|:--:|:--:|
| jac313-001 | ❌ NOT SAFE | - | 116/0 | 30/0 | ✅ | ✅ | ✅ |

\* ✅ PRESUMED SAFE = builds + ctest/smoke/bench 0-fail + memcheck/helgrind/drd clean, both compilers. Presumed, not proven.

### jac313-001 — gate detail

| gate | pass | fail | memcheck | helgrind | drd |
|---|--:|--:|:--:|:--:|:--:|
| ctest | 0 | 0 | ✅ | ✅ | ✅ |
| smoke | 116 | 0 | n/a | n/a | n/a |
| bench | 30 | 0 | n/a | n/a | n/a |

| area | runs | rows |
|---|--:|--:|
| [compilers](compiler/README.md) | — | — |
| [smoke](smoke/README.md) | 1 | 116 |
| [bench](bench/README.md) | 3 | 30 |
| [verify-lite](verify-lite/README.md) | 1 | 31 |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
