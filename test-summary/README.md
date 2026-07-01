# jac313 — shared results store

This directory holds **`results.db`**, the single shared results database that
every version (v001, v002, …) and — later — every machine writes to. It is the
one source of truth; the path is resolved once into the `JAC313_RESULTS_DB`
constant and read everywhere (the single seam to swap for an external/Postgres
store later).

The human-readable reports are **rendered per version** from this one DB by
`jac313_test_cli --report` (each `--report` filters the shared DB to its own
version), and written under each version's own `test-summary/`:

## Rendered reports

- [**v001** test results →](../v001/test-summary/README.md)
- [**v002** test results →](../v002/test-summary/README.md)

## Layout

| location | contents |
|---|---|
| `test-summary/results.db` (here) | the shared database — data for all versions/machines |
| `v001/test-summary/` | v001-scoped rendered `.md` report pages |
| `v002/test-summary/` | v002-scoped rendered `.md` report pages |

Data lives once, here; each version's `test-summary/` is a version-scoped,
rendered view of it. Runs are keyed by `group_id` (machine) and `version`, so
one version's run never overwrites another's for the same machine.
