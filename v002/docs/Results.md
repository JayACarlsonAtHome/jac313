# Results — what a build + test run actually produces

Current live results live in the structured reports under `test-summary/` (per version),
generated from `results.db` by `jac313_test_cli --report`. See:

- The version's `test-summary/README.md` for safeness summary + links to ctest/smoke/bench/verify
- `test-summary/bench/README.md` for throughput (per-machine, median + band for non-durable
  and durable configs at 1M/10M scales)

Current reports use **median ops/sec + low–high band** (durable flushes counted inside the
timed region). Older "Peak ops/sec (fastest-of-N)" figures from functional-matrix logs are
historical/legacy.

The two-standard design continues to be valuable for surfacing real toolchain differences
(e.g. C++26 `std::print` dynamic-width interactions that required `runtime_format`).

See the live `test-summary/bench/` reports for current per-platform numbers. No single
"headline" table is maintained here because data is now per-machine in the rendered reports.

## Toolchain and build findings

The two-standard (and now multi-machine) design continues to surface real issues, such as
C++26 `std::print` dynamic spec interactions (worked around with `runtime_format`) and module
include guard issues. See [Cpp26_Adoption.md](Cpp26_Adoption.md) for details.

## Current state

Live status, throughput, and verification results are in the version's `test-summary/`
reports (see top of this file and `test-summary/README.md`).

For v001's historical numbers see [v001/docs/Results.md](../../v001/docs/Results.md).

## Recording / viewing results

Results (including throughput from `--bench --report`) are written to the shared
`results.db` and rendered per-version under `test-summary/`.

```bash
./jac313_test_cli --ctest --smoke    # functional
./jac313_test_cli --bench --report   # throughput + render to test-summary/
```

See [test-summary/README.md](../test-summary/README.md) (per version) for the current
rendered reports.

Part of the [jac313](../../README.md) project.
