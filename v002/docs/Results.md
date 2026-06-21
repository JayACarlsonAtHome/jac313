# Results — what a build + test run actually produces

> **v002 has not recorded a results matrix yet.** v002 begins as a *faithful copy of v001*
> (rebranded to `::v002`) and has been **structurally and memory validated** — but it has
> **not** run the full per-platform throughput battery that v001 has. It does **not** inherit
> v001's recorded numbers; it produces its own.

## Current state

| Check | Status |
|-------|--------|
| Builds (g++-15, modules + textual) | ✅ clean |
| Smoke matrix (116 scenarios, Debug) | ✅ 116/116 during validation |
| `matrix verify` (valgrind memcheck + helgrind/DRD, ctest + smoke, all sinks) | ✅ 60/60 clean during validation |
| Full per-platform throughput matrix (Debug + Release, multi-OS) | ⏳ **not yet run / not recorded** |

The validation runs above were used only to prove the rebranded v002 builds and is clean;
their results were intentionally **not** committed (v002 keeps an empty results world until a
deliberate matrix run). For v001's full, measured per-platform numbers see
[v001/docs/Results.md](../../v001/docs/Results.md).

## Recording v002's own results

Everything regenerates from scratch, written under `v002/` only:

```bash
cd ..              # the v002 root
./bootstrap.sh                                   # sense toolchain → build the runner → hand off
jac313_test_cli matrix run-all                   # drive the matrix; results land in v002/test-summary/
```

Per-run detail — every `(OS × compiler × build-type)` row — will appear in the
[test-summary hub](../test-summary/) and `jac313_results.db` as runs complete.

Part of the [jac313](../../README.md) project.
