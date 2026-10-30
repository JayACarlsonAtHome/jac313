-- File:    v002/docs/store/schema/jac313_pg_v0_2.sql
-- Date:    2026-08-20
-- Purpose: Full results mirror of jac313_results_db.hpp + store.event landing pad.
-- Related: v0_1 landing pad (store.persist); SQLite remains the in-process sink.
-- Apply:   psql "$(python3 sync_sqlite_to_pg.py --show-dsn)" -v ON_ERROR_STOP=1 -f jac313_pg_v0_2.sql
--          (the server comes from SQL_Secrets/jac313_connections.jtext, outside git)

BEGIN;

CREATE SCHEMA IF NOT EXISTS store;
CREATE SCHEMA IF NOT EXISTS results;

-- ---------------------------------------------------------------------------
-- store.event — shared SoR for persist rows (world-scoped; jsonb metrics)
-- v0_1 store.persist remains as the tiny smoke table.
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS store.event (
    world                text NOT NULL,
    source_table         text NOT NULL DEFAULT 'persist',
    id                   bigint NOT NULL,
    thread_id            bigint,
    per_thread_event_id  bigint,
    flags_raw            bigint,
    category             text,
    payload              text,
    timestamp_us         bigint,
    ints                 jsonb NOT NULL DEFAULT '[]'::jsonb,
    floats               jsonb NOT NULL DEFAULT '[]'::jsonb,
    ingested_at          timestamptz NOT NULL DEFAULT now(),
    PRIMARY KEY (world, source_table, id)
);

CREATE INDEX IF NOT EXISTS event_world_ts ON store.event (world, timestamp_us);
CREATE INDEX IF NOT EXISTS event_category ON store.event (world, category);

COMMENT ON TABLE store.event IS
  'jac313 Store persist mirror. Hot sink stays SQLite; this is the LAN SoR.';

-- ---------------------------------------------------------------------------
-- results — column names match sqlite results.db (snake_case table names)
-- ---------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS results.test_type (
    id          bigint PRIMARY KEY,
    name        text UNIQUE NOT NULL,
    description text NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS results.test_list (
    id   bigint PRIMARY KEY,
    name text UNIQUE NOT NULL
);

CREATE TABLE IF NOT EXISTS results.compiler (
    id      bigint PRIMARY KEY,
    name    text NOT NULL DEFAULT '',
    version text NOT NULL DEFAULT '',
    major   integer NOT NULL DEFAULT 0,
    UNIQUE (name, version)
);

-- An axis that does not apply is '' or 0 (results.db stores no NULL), so the UNIQUE really dedupes.
CREATE TABLE IF NOT EXISTS results.parameter (
    id                 bigint PRIMARY KEY,
    compiler_id        bigint NOT NULL DEFAULT 0,
    build_type         text NOT NULL DEFAULT '',
    modules            text NOT NULL DEFAULT '',
    import_std         text NOT NULL DEFAULT '',
    size               text NOT NULL DEFAULT '',
    persist            text NOT NULL DEFAULT '',
    output_mode        text NOT NULL DEFAULT '',
    threads            integer NOT NULL DEFAULT 0,
    events_per_thread  integer NOT NULL DEFAULT 0,
    runs               integer NOT NULL DEFAULT 0,
    batch              integer NOT NULL DEFAULT 0,
    flag_count         integer NOT NULL DEFAULT 0,
    valgrind_tool      text NOT NULL DEFAULT '',
    UNIQUE (compiler_id, build_type, modules, import_std, size, persist, output_mode, threads,
            events_per_thread, runs, batch, flag_count, valgrind_tool)
);

-- v0_1 host_spec was a stub (hostname/kernel). Widen to sqlite host_spec.
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS cpu text NOT NULL DEFAULT '';
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS cores integer NOT NULL DEFAULT 0;
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS p_cores integer NOT NULL DEFAULT 0;
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS ram_gb integer NOT NULL DEFAULT 0;
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS cpu_mhz_min integer NOT NULL DEFAULT 0;
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS cpu_mhz_max integer NOT NULL DEFAULT 0;
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS disk text NOT NULL DEFAULT '';
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS instance_hash text NOT NULL DEFAULT '';

-- v0_1 run was a stub. Widen to sqlite run.
ALTER TABLE results.run ADD COLUMN IF NOT EXISTS ts_utc text NOT NULL DEFAULT '';
ALTER TABLE results.run ADD COLUMN IF NOT EXISTS host text NOT NULL DEFAULT '';
ALTER TABLE results.run ADD COLUMN IF NOT EXISTS version text NOT NULL DEFAULT '';
ALTER TABLE results.run ADD COLUMN IF NOT EXISTS store_ver text NOT NULL DEFAULT '';
ALTER TABLE results.run ADD COLUMN IF NOT EXISTS qlite_ver text NOT NULL DEFAULT '';
ALTER TABLE results.run ADD COLUMN IF NOT EXISTS jtext_ver text NOT NULL DEFAULT '';

CREATE TABLE IF NOT EXISTS results.current_host (
    version  text PRIMARY KEY,
    group_id bigint NOT NULL
);

-- status: pass | fail | skip | error, 'built' / 'NA' (build), 'pass' (bench: rows exist only on success).
CREATE TABLE IF NOT EXISTS results.test_run (
    id            bigint PRIMARY KEY,
    run_id        bigint NOT NULL DEFAULT 0,
    test_type_id  bigint NOT NULL DEFAULT 0,
    test_list_id  bigint NOT NULL DEFAULT 0,
    parameter_id  bigint NOT NULL DEFAULT 0,
    status        text NOT NULL DEFAULT '',
    duration_ms   bigint NOT NULL DEFAULT 0,
    median_ops    bigint NOT NULL DEFAULT 0,
    low_ops       bigint NOT NULL DEFAULT 0,
    high_ops      bigint NOT NULL DEFAULT 0,
    avg_ops       bigint NOT NULL DEFAULT 0,
    stddev_ops    bigint NOT NULL DEFAULT 0,
    bytes         bigint NOT NULL DEFAULT 0
);

CREATE INDEX IF NOT EXISTS test_run_run ON results.test_run (run_id);
CREATE INDEX IF NOT EXISTS test_run_status ON results.test_run (status);

CREATE TABLE IF NOT EXISTS results.run_event (
    id           bigint PRIMARY KEY,
    run_id       bigint NOT NULL DEFAULT 0,
    ts_utc       text NOT NULL DEFAULT '',
    test_name    text NOT NULL DEFAULT '',
    event        text NOT NULL DEFAULT '',
    message      text NOT NULL DEFAULT '',
    duration_ms  bigint NOT NULL DEFAULT 0
);

CREATE INDEX IF NOT EXISTS run_event_run ON results.run_event (run_id);

CREATE TABLE IF NOT EXISTS results.safeness (
    group_id  bigint NOT NULL,
    version   text NOT NULL DEFAULT '',
    gate      text NOT NULL,
    compiler  text NOT NULL,
    pass      integer NOT NULL DEFAULT 0,
    fail      integer NOT NULL DEFAULT 0,
    PRIMARY KEY (group_id, version, gate, compiler)
);

CREATE TABLE IF NOT EXISTS results.io_best_fit (
    id         bigint PRIMARY KEY,
    group_id   bigint NOT NULL DEFAULT 0,
    type       text NOT NULL DEFAULT '',
    low_size   integer NOT NULL DEFAULT 0,
    high_size  integer NOT NULL DEFAULT 0,
    best_size  integer NOT NULL DEFAULT 0,
    best_ops   bigint NOT NULL DEFAULT 0,
    use_this   integer NOT NULL DEFAULT 0,
    UNIQUE (group_id, type)
);

CREATE TABLE IF NOT EXISTS results.package_version (
    group_id    bigint NOT NULL,
    version     text NOT NULL,
    package     text NOT NULL,
    pkg_version text NOT NULL DEFAULT '',
    PRIMARY KEY (group_id, version, package)
);

CREATE TABLE IF NOT EXISTS results.test_control (
    name         text PRIMARY KEY,
    timeout_sec  integer NOT NULL DEFAULT 300,
    memory_mb    integer NOT NULL DEFAULT 0,
    description  text NOT NULL DEFAULT ''
);

-- GS Family join helper: latest safeness per machine/world/gate
CREATE OR REPLACE VIEW results.v_safeness AS
SELECT group_id, version, gate, compiler, pass, fail,
       (fail = 0 AND pass > 0) AS all_pass
FROM results.safeness;

COMMENT ON VIEW results.v_safeness IS
  'jac313 gate verdicts mirrored from results.db — join from gsf tools over LAN.';

COMMIT;
