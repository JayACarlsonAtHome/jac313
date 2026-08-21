-- jac313 lab Postgres (R430) — results + Store persist landing pad
-- SQLite remains the in-process sink; this is the shared SoR.

BEGIN;

CREATE SCHEMA IF NOT EXISTS store;
CREATE SCHEMA IF NOT EXISTS results;

CREATE TABLE IF NOT EXISTS results.test_type (
    id          bigserial PRIMARY KEY,
    name        text UNIQUE NOT NULL,
    description text
);
INSERT INTO results.test_type(name, description) VALUES
    ('ctest', 'ctest unit suite'),
    ('smoke', 'persist x output smoke matrix'),
    ('bench', 'throughput benchmark suite'),
    ('verify-lite', 'valgrind memcheck gate'),
    ('verify', 'valgrind memcheck + helgrind + DRD'),
    ('build', 'tree compile wall-clock')
ON CONFLICT (name) DO NOTHING;

CREATE TABLE IF NOT EXISTS results.host_spec (
    group_id    bigserial PRIMARY KEY,
    hostname    text,
    os          text,
    kernel      text,
    cpu_model   text,
    UNIQUE (hostname, kernel)
);

CREATE TABLE IF NOT EXISTS results.run (
    run_id      bigserial PRIMARY KEY,
    group_id    bigint REFERENCES results.host_spec(group_id),
    started_at  timestamptz NOT NULL DEFAULT now(),
    git_head    text,
    world       text,
    notes       text
);

CREATE TABLE IF NOT EXISTS store.persist (
    id                   bigint PRIMARY KEY,
    thread_id            bigint,
    per_thread_event_id  bigint,
    flags_raw            bigint,
    category             text,
    payload              text,
    timestamp_us         bigint
);

CREATE TABLE IF NOT EXISTS store.persist_ints (
    id                   bigint PRIMARY KEY REFERENCES store.persist(id),
    thread_id            bigint,
    per_thread_event_id  bigint
);

CREATE TABLE IF NOT EXISTS store.persist_floats (
    id                   bigint PRIMARY KEY REFERENCES store.persist(id),
    thread_id            bigint,
    per_thread_event_id  bigint
);

COMMIT;
