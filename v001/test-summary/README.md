<a id="top"></a>
[← back](../README.md) · [↓ bottom](#bottom)

# Test results

_Generated from `results.db` by `jac313_test_cli --report`._

**jac313 version:** v001 (runs, deletes, and safeness are scoped by this)

## Machines

| machine | CPU | Speed | P.Cores | T.Cores | RAM | Disk | OS |
|---|---|--:|--:|--:|--:|---|---|
| jac313-001 | Intel(R) Core(TM) Ultra 7 265 | 0.8–5.3 GHz | 20 | 20 | 62 GB | xssd | rhel-9.8 |
| jac313-002 | Intel(R) Core(TM) Ultra 7 265 | 0.8–5.3 GHz | 20 | 20 | 62 GB | ssd | fedora-44 |

\* P.Cores = Physical Cores; T.Cores = Threading Cores

## Safeness

| machine | verdict | ctest | smoke | bench | mem | hel | drd |
|---|---|--:|--:|--:|:--:|:--:|:--:|
| jac313-001 | ❌ NOT SAFE: tests missing | - | - | - | ✅ | ✅ | ✅ |
| jac313-002 | ❌ NOT SAFE: tests missing | - | - | - | ✅ | ✅ | ✅ |

\* ✅ PRESUMED SAFE = builds + ctest/smoke/bench 0-fail + memcheck/helgrind/drd clean, both compilers. Presumed, not proven.

❌ **jac313-001** is NOT SAFE: not every test has run (no results for: ctest, smoke, bench). PRESUMED SAFE needs the full test: ctest, smoke, bench and valgrind (memcheck, helgrind, drd) on both gcc and clang. Run `jac313_test_cli --run-everything`, or in Jac313Runner tick every gate with Compiler: both.

❌ **jac313-002** is NOT SAFE: not every test has run (no results for: ctest, smoke, bench). PRESUMED SAFE needs the full test: ctest, smoke, bench and valgrind (memcheck, helgrind, drd) on both gcc and clang. Run `jac313_test_cli --run-everything`, or in Jac313Runner tick every gate with Compiler: both.

### jac313-001 — gate detail

| gate | pass | fail | memcheck | helgrind | drd |
|---|--:|--:|:--:|:--:|:--:|
| ctest | 0 | 0 | ✅ | ✅ | ✅ |
| smoke | 0 | 0 | n/a | n/a | n/a |
| bench | 0 | 0 | n/a | n/a | n/a |

### jac313-002 — gate detail

| gate | pass | fail | memcheck | helgrind | drd |
|---|--:|--:|:--:|:--:|:--:|
| ctest | 0 | 0 | ✅ | ✅ | ✅ |
| smoke | 0 | 0 | n/a | n/a | n/a |
| bench | 0 | 0 | n/a | n/a | n/a |

| area | runs | rows |
|---|--:|--:|
| [compilers](compiler/README.md) | — | — |
| [verify-lite](verify-lite/README.md) | 12 | 372 |
| [compiler-build-times](compiler-build-times/README.md) | 9 | 171 |

---
[← back](../README.md) · [↑ top](#top)
<a id="bottom"></a>
