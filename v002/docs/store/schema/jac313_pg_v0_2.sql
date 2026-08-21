-- File:    v002/docs/store/schema/jac313_pg_v0_2.sql
-- Date:    2026-08-20
-- Purpose: Full results mirror of jac313_results_db.hpp + store.event landing pad.
-- Related: v0_1 landing pad (store.persist); SQLite remains the in-process sink.
-- Apply:   psql "$JAC313_PG_DSN" -v ON_ERROR_STOP=1 -f jac313_pg_v0_2.sql

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
    description text
);

CREATE TABLE IF NOT EXISTS results.test_list (
    id   bigint PRIMARY KEY,
    name text UNIQUE NOT NULL
);

CREATE TABLE IF NOT EXISTS results.compiler (
    id      bigint PRIMARY KEY,
    name    text,
    version text,
    major   integer,
    UNIQUE (name, version)
);

CREATE TABLE IF NOT EXISTS results.parameter (
    id                 bigint PRIMARY KEY,
    compiler_id        bigint,
    build_type         text,
    modules            text,
    import_std         text,
    size               text,
    persist            text,
    output_mode        text,
    threads            integer,
    events_per_thread  integer,
    runs               integer,
    batch              integer,
    flag_count         integer,
    valgrind_tool      text
);

-- v0_1 host_spec was a stub (hostname/kernel). Widen to sqlite host_spec.
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS cpu text;
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS cores integer;
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS p_cores integer;
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS ram_gb integer;
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS cpu_mhz_min integer;
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS cpu_mhz_max integer;
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS disk text;
ALTER TABLE results.host_spec ADD COLUMN IF NOT EXISTS instance_hash text;
ALTER TABLE results.host_spec ALTER COLUMN instance_hash SET DEFAULT '';

-- v0_1 run was a stub. Widen to sqlite run.
ALTER TABLE results.run ADD COLUMN IF NOT EXISTS ts_utc text;
ALTER TABLE results.run ADD COLUMN IF NOT EXISTS host text;
ALTER TABLE results.run ADD COLUMN IF NOT EXISTS version text;
ALTER TABLE results.run ADD COLUMN IF NOT EXISTS store_ver text;
ALTER TABLE results.run ADD COLUMN IF NOT EXISTS qlite_ver text;
ALTER TABLE results.run ADD COLUMN IF NOT EXISTS jtext_ver text;

CREATE TABLE IF NOT EXISTS results.current_host (
    version  text PRIMARY KEY,
    group_id bigint NOT NULL
);

CREATE TABLE IF NOT EXISTS results.test_run (
    id            bigint PRIMARY KEY,
    run_id        bigint,
    test_type_id  bigint,
    test_list_id  bigint,
    parameter_id  bigint,
    status        text,
    duration_ms   bigint,
    median_ops    bigint,
    low_ops       bigint,
    high_ops      bigint,
    avg_ops       bigint,
    stddev_ops    bigint,
    bytes         bigint
);

CREATE INDEX IF NOT EXISTS test_run_run ON results.test_run (run_id);
CREATE INDEX IF NOT EXISTS test_run_status ON results.test_run (status);

CREATE TABLE IF NOT EXISTS results.run_event (
    id           bigint PRIMARY KEY,
    run_id       bigint,
    ts_utc       text,
    test_name    text,
    event        text,
    message      text,
    duration_ms  bigint
);

CREATE INDEX IF NOT EXISTS run_event_run ON results.run_event (run_id);

CREATE TABLE IF NOT EXISTS results.safeness (
    group_id  bigint NOT NULL,
    version   text NOT NULL DEFAULT '',
    gate      text NOT NULL,
    compiler  text NOT NULL,
    pass      integer,
    fail      integer,
    PRIMARY KEY (group_id, version, gate, compiler)
);

CREATE TABLE IF NOT EXISTS results.io_best_fit (
    id         bigint PRIMARY KEY,
    group_id   bigint,
    type       text,
    low_size   integer,
    high_size  integer,
    best_size  integer,
    best_ops   bigint,
    use_this   integer,
    UNIQUE (group_id, type)
);

CREATE TABLE IF NOT EXISTS results.package_version (
    group_id    bigint NOT NULL,
    version     text NOT NULL,
    package     text NOT NULL,
    pkg_version text,
    PRIMARY KEY (group_id, version, package)
);

CREATE TABLE IF NOT EXISTS results.test_control (
    name         text PRIMARY KEY,
    timeout_sec  integer NOT NULL DEFAULT 300,
    memory_mb    integer NOT NULL DEFAULT 0,
    description  text
);

-- GS Family join helper: latest safeness per machine/world/gate
CREATE OR REPLACE VIEW results.v_safeness AS
SELECT group_id, version, gate, compiler, pass, fail,
       (fail = 0 AND pass > 0) AS all_pass
FROM results.safeness;

COMMENT ON VIEW results.v_safeness IS
  'jac313 gate verdicts mirrored from results.db — join from gsf tools over LAN.';

COMMIT;
