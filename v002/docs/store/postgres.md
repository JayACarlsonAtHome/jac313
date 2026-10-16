//File:    v002/docs/store/postgres.md
//Date:    2026-08-20
//Purpose: Lab Postgres for jac313 results + Store persist SoR (sync from SQLite).
//Related: type=jac313; pg=<the pg-remote row of SQL_Secrets/jac313_connections.jtext>; db=jac313

# jac313 ↔ lab PostgreSQL

Store’s hot SQL sink remains **SQLite** (Qlite). The lab **SoR** for shared
results and a Postgres-shaped persist schema lives on your lab server:

| | |
|---|---|
| Host | the `pg-remote` row of the connections file (below); never written in git |
| Database | `jac313` (role `jac313` — one role per git project, owner of the database) |
| Creds | `/etc/gfs/pgpass` (libpq password file, mirror `<GFS>/etc/gfs/pgpass`) — **not in git** |

libpq finds the password itself (`PGPASSFILE=/etc/gfs/pgpass`, set in
`/etc/profile.d/gfs-pgpass.sh`); nothing in this repo holds it.

The server's details live in a jText **connections file** kept outside every git project:
`<projects>/SQL_Secrets/jac313_connections.jtext` (a `SQL_Secrets` folder beside the jac313
checkout; `$JAC313_CONNECTIONS` chooses another). Jac313Runner (Connections > Save) and
`jac313_test_cli connections --init` write it; by hand it looks like this (one record per
server; write every value: an empty one is jText's NULL, the Unit Separator byte 0x1F):

```
=== jText File ===
 1. #?# jac313_connections


=== Section: connections ===


=== Fields ===
 1. #/# id/Identity(1,1)/Not Null     # record id
 2. #/# backend/String/Not Null       # pg-local | pg-remote
 3. #/# host/String/Not Null          # host name or IP address
 4. #/# port/Number
 5. #/# dbname/String
 6. #/# user/String
 7. #/# connect_timeout/Number        # seconds
 8. #/# notes/String
=== End Fields ===


=== Data ===
1. #|# 1|pg-local|localhost|5432|jac313|jac313|10|this machine

1. #|# 2|pg-remote|<your-server>|5432|jac313|jac313|10|lab server

=== End Data ===
=== End Section ===
=== End File ===
```

Never put a password in it: libpq reads it from `$PGPASSFILE` / `~/.pgpass`. Keep the folder
private (`chmod 700`).

```bash
psql "$(python3 v002/docs/store/schema/sync_sqlite_to_pg.py --show-dsn)" -c '\dt store.*'
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
