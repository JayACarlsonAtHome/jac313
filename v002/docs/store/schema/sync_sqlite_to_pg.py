#!/usr/bin/env python3
"""Mirror jac313 SQLite persist + results.db onto lab Postgres (R430).

Hot Store sink stays SQLite (Qlite). This is the LAN SoR so GS Family can join.

Usage (creds from ~/.jac314-pg.env, not git):

    set -a; source ~/.jac314-pg.env; set +a
    python3 v002/docs/store/schema/sync_sqlite_to_pg.py

Env:
    JAC313_PG_DSN     required (or PGDATABASE/PGHOST/…)
    JAC313_ROOT       default: repo root inferred from this file
"""

from __future__ import annotations

import csv
import json
import os
import sqlite3
import subprocess
import sys
import tempfile
from pathlib import Path

SCHEMA_DIR = Path(__file__).resolve().parent
ROOT = Path(os.environ.get("JAC313_ROOT", SCHEMA_DIR.parents[3]))


def dsn() -> str:
    d = os.environ.get("JAC313_PG_DSN") or os.environ.get("DATABASE_URL")
    if not d:
        sys.exit("sync_sqlite_to_pg: set JAC313_PG_DSN (source ~/.jac314-pg.env)")
    return d


def psql(args: list[str], stdin: str | None = None) -> None:
    cmd = ["psql", "-v", "ON_ERROR_STOP=1", dsn(), *args]
    r = subprocess.run(cmd, input=stdin, text=True, capture_output=True)
    if r.returncode != 0:
        sys.stderr.write(r.stderr)
        raise SystemExit(f"psql failed: {' '.join(args[:4])}")
    if r.stdout and not any(a.startswith("\\copy") or a.startswith("-f") for a in args):
        sys.stdout.write(r.stdout)


def copy_table(sqlite_path: Path, sqlite_sql: str, pg_table: str, columns: str) -> int:
    with tempfile.TemporaryDirectory() as td:
        csv_path = Path(td) / "t.csv"
        con = sqlite3.connect(f"file:{sqlite_path}?mode=ro", uri=True)
        try:
            cur = con.execute(sqlite_sql)
            headers = [d[0] for d in cur.description]
            with csv_path.open("w", newline="") as f:
                w = csv.writer(f)
                w.writerow(headers)
                n = 0
                for row in cur:
                    w.writerow(["" if c is None else c for c in row])
                    n += 1
        finally:
            con.close()
        if n == 0:
            return 0
        psql(["-c", f"\\copy {pg_table} ({columns}) FROM '{csv_path}' CSV HEADER"])
        return n


def sync_results(db: Path) -> None:
    if not db.is_file():
        print(f"  skip results (missing {db})")
        return
    print(f"  results.db {db}")
    psql(
        [
            "-c",
            "TRUNCATE results.run_event, results.test_run, results.safeness, "
            "results.io_best_fit, results.package_version, results.current_host, "
            "results.test_control, results.parameter, results.compiler, "
            "results.test_list, results.test_type, results.run, results.host_spec "
            "RESTART IDENTITY",
        ]
    )

    jobs = [
        (
            "SELECT id, name, description FROM testType",
            "results.test_type",
            "id, name, description",
        ),
        ("SELECT id, name FROM testList", "results.test_list", "id, name"),
        (
            "SELECT id, name, version, major FROM compiler",
            "results.compiler",
            "id, name, version, major",
        ),
        (
            """SELECT id, compiler_id, build_type, modules, import_std, size, persist,
                      output_mode, threads, events_per_thread, runs, batch,
                      flag_count, valgrind_tool FROM parameter""",
            "results.parameter",
            "id, compiler_id, build_type, modules, import_std, size, persist, output_mode, threads, events_per_thread, runs, batch, flag_count, valgrind_tool",
        ),
        (
            """SELECT group_id, cpu, cores, p_cores, ram_gb, cpu_mhz_min, cpu_mhz_max,
                      disk, os, instance_hash FROM host_spec""",
            "results.host_spec",
            "group_id, cpu, cores, p_cores, ram_gb, cpu_mhz_min, cpu_mhz_max, disk, os, instance_hash",
        ),
        (
            "SELECT run_id, ts_utc, group_id, host, version, store_ver, qlite_ver, jtext_ver FROM run",
            "results.run",
            "run_id, ts_utc, group_id, host, version, store_ver, qlite_ver, jtext_ver",
        ),
        (
            "SELECT version, group_id FROM current_host",
            "results.current_host",
            "version, group_id",
        ),
        (
            """SELECT id, run_id, test_type_id, test_list_id, parameter_id, status,
                      duration_ms, median_ops, low_ops, high_ops, avg_ops, stddev_ops, bytes
               FROM testRun""",
            "results.test_run",
            "id, run_id, test_type_id, test_list_id, parameter_id, status, duration_ms, median_ops, low_ops, high_ops, avg_ops, stddev_ops, bytes",
        ),
        (
            "SELECT id, run_id, ts_utc, test_name, event, message, duration_ms FROM runEvent",
            "results.run_event",
            "id, run_id, ts_utc, test_name, event, message, duration_ms",
        ),
        (
            "SELECT group_id, version, gate, compiler, pass, fail FROM safeness",
            "results.safeness",
            "group_id, version, gate, compiler, pass, fail",
        ),
        (
            "SELECT id, group_id, type, low_size, high_size, best_size, best_ops, useThis FROM io_best_fit",
            "results.io_best_fit",
            "id, group_id, type, low_size, high_size, best_size, best_ops, use_this",
        ),
        (
            "SELECT group_id, version, package, pkg_version FROM package_version",
            "results.package_version",
            "group_id, version, package, pkg_version",
        ),
        (
            "SELECT name, timeout_sec, memory_mb, description FROM testControl",
            "results.test_control",
            "name, timeout_sec, memory_mb, description",
        ),
    ]
    for sql, table, cols in jobs:
        n = copy_table(db, sql, table, cols)
        print(f"    {table:32} {n}")


def _metric_cols(con: sqlite3.Connection, table: str, prefix: str) -> list[str]:
    cols = []
    for cid, name, *_ in con.execute(f"PRAGMA table_info({table})"):
        if name.startswith(prefix) and name[len(prefix) :].isdigit():
            cols.append(name)
    return sorted(cols, key=lambda n: int(n[len(prefix) :]))


def sync_persist(db: Path, world: str) -> None:
    if not db.is_file():
        print(f"  skip persist {world} (missing {db})")
        return
    con = sqlite3.connect(f"file:{db}?mode=ro", uri=True)
    try:
        mains = [
            r[0]
            for r in con.execute(
                "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE '%_ints' AND name NOT LIKE '%_floats'"
            )
        ]
        rows_sql = []
        for main in mains:
            info = {r[1] for r in con.execute(f"PRAGMA table_info({main})")}
            need = {
                "id",
                "thread_id",
                "per_thread_event_id",
                "flags_raw",
                "category",
                "payload",
                "timestamp_us",
            }
            if not need <= info:
                continue
            ints_t = f"{main}_ints"
            floats_t = f"{main}_floats"
            int_cols = _metric_cols(con, ints_t, "int") if _table_exists(con, ints_t) else []
            flt_cols = _metric_cols(con, floats_t, "dbl") if _table_exists(con, floats_t) else []
            q = f"SELECT id, thread_id, per_thread_event_id, flags_raw, category, payload, timestamp_us FROM {main}"
            for id_, tid, peid, flags, cat, payload, ts in con.execute(q):
                ints = []
                if int_cols:
                    row = con.execute(
                        f"SELECT {', '.join(int_cols)} FROM {ints_t} WHERE id=?", (id_,)
                    ).fetchone()
                    if row:
                        ints = [int(x) if x is not None else None for x in row]
                floats = []
                if flt_cols:
                    row = con.execute(
                        f"SELECT {', '.join(flt_cols)} FROM {floats_t} WHERE id=?", (id_,)
                    ).fetchone()
                    if row:
                        floats = [float(x) if x is not None else None for x in row]
                rows_sql.append(
                    (
                        world,
                        main,
                        int(id_),
                        tid,
                        peid,
                        flags,
                        cat,
                        payload,
                        ts,
                        json.dumps(ints),
                        json.dumps(floats),
                    )
                )
    finally:
        con.close()

    psql(["-c", "DELETE FROM store.event WHERE world = %s" % _lit(world)])
    if not rows_sql:
        print(f"    store.event {world:4} 0")
        return
    with tempfile.TemporaryDirectory() as td:
        csv_path = Path(td) / "e.csv"
        with csv_path.open("w", newline="") as f:
            w = csv.writer(f)
            w.writerow(
                [
                    "world",
                    "source_table",
                    "id",
                    "thread_id",
                    "per_thread_event_id",
                    "flags_raw",
                    "category",
                    "payload",
                    "timestamp_us",
                    "ints",
                    "floats",
                ]
            )
            for r in rows_sql:
                w.writerow(r)
        psql(
            [
                "-c",
                f"\\copy store.event (world, source_table, id, thread_id, per_thread_event_id, flags_raw, category, payload, timestamp_us, ints, floats) FROM '{csv_path}' CSV HEADER",
            ]
        )
    print(f"    store.event {world:4} {len(rows_sql)}")


def _table_exists(con: sqlite3.Connection, name: str) -> bool:
    r = con.execute(
        "SELECT 1 FROM sqlite_master WHERE type='table' AND name=?", (name,)
    ).fetchone()
    return r is not None


def _lit(s: str) -> str:
    return "'" + s.replace("'", "''") + "'"


def main() -> int:
    print(f"jac313 sqlite → postgres  root={ROOT}")
    schema = SCHEMA_DIR / "jac313_pg_v0_2.sql"
    print(f"  apply {schema.name}")
    psql(["-f", str(schema)])
    sync_results(ROOT / "test-summary" / "results.db")
    sync_persist(ROOT / "v001" / "persist_database.db", "v001")
    sync_persist(ROOT / "v002" / "persist_database.db", "v002")
    psql(
        [
            "-c",
            "SELECT (SELECT COUNT(*) FROM results.test_run) AS test_run, "
            "(SELECT COUNT(*) FROM results.run_event) AS run_event, "
            "(SELECT COUNT(*) FROM store.event) AS persist_events;",
        ]
    )
    print("sync_sqlite_to_pg: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
