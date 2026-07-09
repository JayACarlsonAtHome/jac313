# kept-logs — local durable capture output

This directory holds **retained** Store persistence files when you run the test battery with
`--keep-logs`. **Capture files** (`.json`, `.bin`, `.html`, …) are gitignored — they can be
**very large**. **`scenario.log`** text files under smoke runs **may** be committed (see
[test-results/README.md](../test-results/README.md)).

## Use with care — disk space

These files can be **very large**. A full `--run-everything --keep-logs` on one machine is on
the order of **~12 GB** (mostly bench 10M JSON/HTML captures). Smoke matrix output is much
smaller (~10 MB per compiler) but still adds up across compilers and options.

Only enable `--keep-logs` when you have **plenty of free disk** and you actually need the
artifacts for inspection (log viewer work, format spot-checks, readback tests). Without
`--keep-logs`, bench and smoke runs still record throughput and pass/fail to `test-summary/`
but **delete or overwrite** durable files as today.

```bash
# Full battery — default; bench/smoke durable files are NOT kept (~12 GB NOT written)
./jac313_test_cli --run-everything

# Full battery + retain all durable captures (needs ~12+ GB free)
./jac313_test_cli --run-everything --keep-logs

# Bench only, custom root
./jac313_test_cli --bench --keep-logs --keep-logs-dir /big/scratch/jac313-kept
```

## Directory layout

Everything is organized **by gate → compiler → option** (and **by test** for smoke):

```
kept-logs/
  README.md                          ← tracked (this file)
  bench/
    <compiler>/                      e.g. gcc16, clang
      <option>/                      e.g. jtext_1M, json_10M, binary_1M
        capture.json                 (base name is always "capture")
        capture_Ints.json
        capture_Floats.json
        …
  smoke/
    <compiler>/
      <test>/                        e.g. JAC313_STORE_TEST_001_TS
        <option>/                    e.g. jtext_output_on, binary_output_off
          capture.jtext / .html / .json / .bin / …
          scenario.log               (matrix runner log for that scenario)
```

**Option** encodes the matrix dimensions that used to overwrite each other:

| Gate | Option folder | Meaning |
|------|---------------|---------|
| bench | `jtext_1M` | durable jtext, 1M-event suite config |
| bench | `html_1M` | durable html, 1M-event suite config |
| bench | `json_1M` | durable json, 1M-event suite config |
| bench | `json_10M` | durable json, 10M scaling config |
| bench | `html_10M` | durable html, 10M scaling config |
| smoke | `jtext_output_on` | test 00x, jtext persist, console color on |
| smoke | `html_output_off` | test 00x, html persist, console color off |
| smoke | `json_output_on` | test 00x, json persist, console color on |
| smoke | `sql_output_on` | test 00x, sql persist, console color on |

## Cleaning up

```bash
./jac313_test_cli --wipe-logs    # removes kept-logs/ and test-results/ (this README survives)
```

Or delete the tree by hand. Regenerate with another `--keep-logs` run.

See also: [log viewer plan](docs/store/log-viewer-plan.md) (paged viewer; sidecar index).