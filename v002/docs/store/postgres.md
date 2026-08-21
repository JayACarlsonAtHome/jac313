//File:    v002/docs/store/postgres.md
//Date:    2026-08-20
//Purpose: Lab Postgres (R430) for jac313 results + Store persist SoR (sync from SQLite).
//Related: type=jac313; pg=192.168.1.26; db=jac313

# jac313 ↔ lab PostgreSQL

Store’s hot SQL sink remains **SQLite** (Qlite). The lab **SoR** for shared
results and a Postgres-shaped persist schema lives on the R430:

| | |
|---|---|
| Host | `192.168.1.26` (jac313-001, Postgres 17) |
| Database | `jac313` (role `jay`) |
| Creds | `~/.jac314-pg.env` (`JAC313_PG_DSN`) — **not in git** |

```bash
set -a; source ~/.jac314-pg.env; set +a
psql "$JAC313_PG_DSN" -c '\dt store.*'
```

Schema files:

- [`schema/jac313_pg_v0_1.sql`](schema/jac313_pg_v0_1.sql) — first landing pad (`store.persist`)
- [`schema/jac313_pg_v0_2.sql`](schema/jac313_pg_v0_2.sql) — full `results.*` mirror of `results.db` + `store.event`

Sync this checkout’s SQLite files onto the lab:

```bash
set -a; source ~/.jac314-pg.env; set +a
python3 v002/docs/store/schema/sync_sqlite_to_pg.py
```

C++ `SqlEventSink` still writes SQLite files (Qlite; no in-place Qlite edits).
The `jac313` database is the multi-user SoR so GS Family tools can join runs
without a file lock. `store.event` holds persist rows as jsonb metric arrays,
scoped by `world` (`v001` / `v002`).
