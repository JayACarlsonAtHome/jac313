# test-results — local run artifacts

Runtime output from `jac313_test_cli` matrix and ctest gates. Paths are keyed on **this
machine's** identity (`<os>/<compiler>/<build_type>/<disk>/<size>/<modules>/`).

## What you can commit

| Artifact | Commit? | Notes |
|----------|---------|-------|
| `**/*.log` | **Yes** | Text-only captured stdout/stderr — small (typically KB each) |
| `persist_*`, `*.json`, `*.bin`, … | **No** | Durable capture files — gitignored; use `--keep-logs` for those |

Smoke matrix logs live next to the scenario under each persist/test folder, e.g.:

```
test-results/<os>/<compiler>/Debug/<disk>/Smoke/textual/<persist>_logs/JAC313_STORE_TEST_001_TS/gcc16_<persist>_on.log
  # e.g. jtext_logs/…/gcc16_jtext_on.log, json_logs/…/gcc16_json_on.log, html_logs/…/gcc16_html_on.log
```

Ctest logs are centralized under the `Ctest` size label:

```
test-results/<os>/<compiler>/Debug/<disk>/Ctest/textual/logs/<ctest_name>.log
```

`.log` files are **not** pushed automatically — commit them when useful (green-run reference,
debugging a failure, documenting expected output). Everything else under `test-results/` stays
local.

## Large durable captures

Bench/smoke **persist files** at scale belong under `kept-logs/` when you pass `--keep-logs`
(see [kept-logs/README.md](../kept-logs/README.md)). That tree can reach **~12 GB** on a full
run — never commit those blobs.

## Cleaning up

```bash
./jac313_test_cli --wipe-logs              # delete test-results/ + kept-logs/ (README.md kept)
./jac313_test_cli --wipe-logs --run-everything --keep-logs   # fresh tree, then full retained run
```

Or delete any machine-specific subtree by hand. Re-run `--ctest` or `matrix run` to regenerate logs.