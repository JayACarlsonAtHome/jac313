# Store — sample persistence output

Real output from a **smoke run** (~100 events) of `jac313::Store`'s persistence sinks, so
you can see the actual file formats without building and running anything. Small by design
(a smoke run, not a full matrix) — the whole set is ~42 KB.

| File | Sink | Browsable on GitHub? |
|------|------|----------------------|
| `persist_keeper.jtext` | jText sink — main event log (human-readable) | yes (text) |
| `persist_keeper_Ints.jtext` | jText sink — split integer-metric table | yes (text) |
| `persist_keeper_Floats.jtext` | jText sink — split float-metric table | yes (text) |
| `persist_keeper.sql` | SQL sink — `CREATE`/`INSERT` for the main table | yes (text) |
| `persist_keeper_Ints.sql` | SQL sink — integer-metric table | yes (text) |
| `persist_keeper_Floats.sql` | SQL sink — float-metric table | yes (text) |
| `persist_database.db` | SQLite database the SQL sink wrote | no (binary — download + open with `sqlite3`) |

Notes:
- These are captured artifacts, **not live files** — paths inside them (e.g. a jText
  `<#include#>` schema reference) are as-generated at run time and don't resolve from here.
- The binary keeper sink writes a length-prefixed `.bin`; it isn't included because it's
  binary (GitHub won't render it). The jText/SQL text above shows the same events.
- **Regenerate:** run a smoke matrix (`jac313_test_cli matrix run`); the keeper output lands
  at the repo root (gitignored as `/persist*`), then copy the files here.

See [Store reporting](../store/reporting.md) for how the logs are read back.
