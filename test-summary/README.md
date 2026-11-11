# jac313 — shared results store

This directory holds **`results.db`**, the single shared results database that
every version (v001, v002, …) on this machine writes to. It is the one source of
truth; the path is resolved once into the `JAC313_RESULTS_DB` constant and read
everywhere (the single seam to swap for an external/Postgres store later).

**Committed here:** this page and the run results record, two jText files that
keep `results.db`'s run results when the database is wiped:

- **`results_main.jtext`**: table `run_result`, one row per recorded run, newest
  first: pass or fail with the counts, the git commit (and whether the version
  folder had uncommitted changes), the machine pin and what made it up, the OS,
  every selection the run used (compiler and its version, Debug/Release,
  headers/modules, contracts, hardened, the persist outputs, sizes, valgrind tools,
  caps, read-back) and the run's options.
- **`results_note.jtext`**: table `run_change`, the notes: what we changed for a
  run, when we remember it, linked by `run_result_id` to `run_result.id`.

The CLI loads both files into `results.db` every time it records or reports, and
saves them after every recorded run. At the end of a run it commits only these two
files on the current branch and pushes `dev-work` with `--no-verify` (no other
branch is pushed; the Runner does this once, as its last step). The pre-push gate's
own runs save without committing; the next run's commit carries them. Before a run
starts, the CLI prints whether this commit passed or failed here before.

The database itself and the rendered report pages are this machine's own and are
not committed (`.gitignore` at the repo root). The CLI creates the database when it
is missing, and `--report` renders the pages again from it.

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
| `test-summary/results_main.jtext` (here, committed) | every recorded run's result, commit and selections |
| `test-summary/results_note.jtext` (here, committed) | the notes: what we changed for a run |
| `v001/test-summary/` | v001-scoped rendered `.md` report pages |
| `v002/test-summary/` | v002-scoped rendered `.md` report pages |

Data lives once, here; each version's `test-summary/` is a version-scoped,
rendered view of it. Runs are keyed by `group_id` (machine) and `version`, so
one version's run never overwrites another's for the same machine.
