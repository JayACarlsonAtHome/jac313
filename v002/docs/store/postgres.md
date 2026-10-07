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
| Database | `jac313` (role `jac313` — one role per git project, owner of the database) |
| Creds | `/etc/gfs/pgpass` (libpq password file, mirror `<GFS>/etc/gfs/pgpass`) — **not in git** |

libpq finds the password itself (`PGPASSFILE=/etc/gfs/pgpass`, set in
`/etc/profile.d/gfs-pgpass.sh`); nothing in this repo holds it.

```bash
psql "host=192.168.1.26 dbname=jac313 user=jac313" -c '\dt store.*'
```

Schema files:

- [`schema/jac313_pg_v0_1.sql`](schema/jac313_pg_v0_1.sql) — first landing pad (`store.persist`)
- [`schema/jac313_pg_v0_2.sql`](schema/jac313_pg_v0_2.sql) — full `results.*` mirror of `results.db` + `store.event`

Sync this checkout’s SQLite files onto the lab (`JAC313_PG_DSN` overrides the target):

```bash
python3 v002/docs/store/schema/sync_sqlite_to_pg.py
```

C++ `SqlEventSink` still writes SQLite files (Qlite; no in-place Qlite edits).
The `jac313` database is the multi-user SoR so GS Family tools can join runs
without a file lock. `store.event` holds persist rows as jsonb metric arrays,
scoped by `world` (`v001` / `v002`).
