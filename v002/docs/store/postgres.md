//File:    postgres.md
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
| Creds | `/etc/gfs/pgpass` (libpq password file; its mirror is `etc/gfs/pgpass` on the drive, beside the projects dir) — **not in git** |

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

Never put a password in it: the passwords go in `jac313_pg_local.pgpass` / `jac313_pg_remote.pgpass`
beside it (see [docs/SQL_Secrets_example](../../../docs/SQL_Secrets_example/README.md)). Keep the folder
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
Since 2026-10 there is also an optional **direct PostgreSQL sink** for the persist
tests (next section); it does not replace the SQLite sink or the sync above.
The `jac313` database is the multi-user SoR so GS Family tools can join runs
without a file lock. `store.event` holds persist rows as jsonb metric arrays,
scoped by `world` (`v001` / `v002`).

## Direct PostgreSQL sink (persist tests, read-back, latency)

`PgClient` + `PgEventSink` (`Store/include/jac313/Store/v002/headers/persistence/Pg*`)
write a run's events straight into PostgreSQL with one `COPY` transaction per batch.
It is an optional build: CMake option `JAC313_STORE_PG_PERSIST`, target
`jac313_store_pg_persist`, ON only when libpq 18+ is found (bootstrap installs libpq 18: the `pg18` component).

| | |
|---|---|
| Backends | `pg-local` (`--pg-local` / `$JAC313_PG_LOCAL`, default `host=localhost dbname=jac313 user=jac313`) and `pg-remote` (**your own server, no default**: Jac313Runner's Remote PG fields, `--pg-remote "host=... dbname=... user=..."`, or `$JAC313_PG_REMOTE`). Passwords only from `~/.pgpass` / `$PGPASSFILE`; the CLI refuses one on its command line |
| Tables | `store_test.<host>_<testdir>_<file>` plus `_ints` / `_floats`, recreated on open, so each holds the last run |
| Never touched | `results.*` and `store.*` |
| Cleanup | `--pg-cleanup keep-last` (default) or `delete` |
| Recorded | connect ms, median `SELECT 1` ping ms, total `COPY` ms (`testRun.pg_connect_ms`, `pg_ping_ms`, `pg_write_ms`) |

**PG local, set up by bootstrap.** `./bsir.sh` installs a PostgreSQL 18 server on the
machine (Fedora `postgresql-server`, RHEL PGDG `postgresql18-server`, Mint PGDG `postgresql-18`),
starts it at boot, creates the role and database `jac313` with a random password, adds two
`scram-sha-256` lines for `jac313` on 127.0.0.1 / ::1 at the top of `pg_hba.conf` (marked
`jac313 pg_local`) and writes the password to `SQL_Secrets/jac313_pg_local.pgpass` (mode 600). A working
login is left alone, so re-running bootstrap changes nothing.

**PG remote, set up by you.** On your server: a login role and a database it owns, and a
`pg_hba.conf` line that lets the client machine in with a password. On the client: one line
`host:5432:dbname:user:password` in `SQL_Secrets/jac313_pg_remote.pgpass` (mode 600), then the
Runner's Remote PG fields and **Test**. "no password supplied" means that file is missing, not mode
600, or its line does not match the host / database / user exactly. jac313_test_cli and the Runner
add `passfile=<that file>` to the server's connection when the file exists.

The remote backend exists to show the network cost to users without a local server:
on the lab LAN a local ping is about 0.015 ms and the lab server about 1.4 ms.

```bash
./jac313_test_cli --smoke --persist sql,pg-local,pg-remote --pg-remote "host=<your-server> dbname=jac313 user=jac313" \
                  --max-persist-rows 1000 --readback sample
```

**Connections file.** Both servers can live in a jText file kept outside every git project:
`<projects>/SQL_Secrets/jac313_connections.jtext` by default (`$JAC313_CONNECTIONS` or
`--connections FILE` choose another). One record per server: `id, backend, host, port, dbname,
user, connect_timeout, notes`; never a password. `jac313_test_cli connections` shows it and
its jText check, `connections --init` writes a blank one, and Jac313Runner loads and saves it.
Precedence: `--pg-remote` / `--pg-local`, then `--connections`, then `$JAC313_PG_*`, then the
default file. The CLI warns when the file sits inside a git work tree.

The chosen server is exported in `run_latest_config.sh` and recorded (never a password) as
`"pg_remote"` in the summary JSON, so the GUI's viewer reads an old run from the server it used.
`jac313_store_dataset --ping pg-remote --conninfo "..."` checks a server (Jac313Runner's Test button).

The new `testRun` columns are not in `sync_sqlite_to_pg.py`'s column list yet, so the
sync keeps working but does not mirror them.
