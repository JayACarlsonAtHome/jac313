# SQL_Secrets — example folder

This folder is an **example** with **fake passwords**. Your real one sits **beside** the project,
never inside a git project:

```
~/git/
├── jac313/                       <- the project (git)
└── SQL_Secrets/                  <- your secrets (not git), mode 700
    ├── jac313_connections.jtext  <- which servers (no passwords)
    ├── jac313_pg_local.pgpass    <- the PG local password (bootstrap writes it), mode 600
    └── jac313_pg_remote.pgpass   <- the PG remote password (you write it), mode 600
```

`jac313_test_cli` and Jac313Runner find the folder by themselves (`../SQL_Secrets` from the
project; `$JAC313_CONNECTIONS` names another connections file, and its folder is then used).
When a password file exists, its server's connection gets `passfile=<that file>`, so libpq reads
the password from it. Nothing else needs it: not `~/.pgpass`, not an environment variable.

| File | What it holds |
|---|---|
| `jac313_connections.jtext` | One row per server: backend (`pg-local` / `pg-remote`), host, port, database, user, timeout, notes. Jac313Runner's **Save** writes it; `jac313_test_cli connections` checks it. |
| `jac313_pg_local.pgpass` | `host:port:database:user:password` for the server on this machine. |
| `jac313_pg_remote.pgpass` | The same for your remote server. |

Lines starting with `#` are comments. libpq ignores a password file that others can read, so keep
both at mode 600 (`chmod 600 ~/git/SQL_Secrets/*.pgpass`) and the folder at 700.

## PG local: the server on this machine

`./bootstrap.sh` does all of it. It installs PostgreSQL 18, creates the role and database `jac313`
with a random password, lets `jac313` log in over localhost, and writes
`~/git/SQL_Secrets/jac313_pg_local.pgpass`. You never type that password.

To choose your own password instead:

```bash
sudo -u postgres psql -c "ALTER ROLE jac313 PASSWORD 'my-new-local-password'"
```

then put the same password in the file (both lines):

```
localhost:5432:jac313:jac313:my-new-local-password
127.0.0.1:5432:jac313:jac313:my-new-local-password
```

Deleting the file and re-running `./bootstrap.sh` makes a new random password.

## PG remote: your own server

1. **On the server**, as a PostgreSQL superuser, make the role and its database:

   ```sql
   CREATE ROLE jac313 LOGIN PASSWORD 'my-remote-password';
   CREATE DATABASE jac313 OWNER jac313;
   ```

   and let your machine in with a password: a line in the server's `pg_hba.conf` such as

   ```
   host    jac313    jac313    192.168.1.0/24    scram-sha-256
   ```

   (your network instead of `192.168.1.0/24`), plus `listen_addresses = '*'` in
   `postgresql.conf`. Reload: `SELECT pg_reload_conf();` (a `listen_addresses` change needs a restart).

2. **On your machine**, write `~/git/SQL_Secrets/jac313_pg_remote.pgpass` (mode 600):

   ```
   db.example.org:5432:jac313:jac313:my-remote-password
   ```

   The host must be written exactly as you type it in the Runner (a name, or the IP address).

3. **In Jac313Runner**: Remote PG Host `db.example.org` (Database and User `jac313`), press
   **Test**. It shows the connect and ping times and the server version. **Save** stores the
   server in `jac313_connections.jtext`.

"no password supplied" means the password file is missing, is not mode 600, or its line does not
match the host / port / database / user exactly. "password authentication failed" means the
password in the file differs from the one on the server.
