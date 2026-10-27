-- File:    v002/schema/fleet_v0_1.sql
-- Purpose: PostgreSQL DDL for the fleet registry on the shared (pg-remote) server: one row per
--          machine identity, so every machine that shares the server gets its own jac313-###
--          (group_id) instead of each one numbering itself 001 in its own results.db.
--          Idempotent: jac313_test_cli runs this file before it registers a machine, and it can be
--          run by hand:  psql "host=... dbname=jac313 user=jac313" -f v002/schema/fleet_v0_1.sql
-- Related: shared/jac313_results_db.hpp (host_spec: the local copy, same identity columns),
--          tools/jac313_test_cli/fleet_registry.cpp (the register / look-up transaction).

CREATE SCHEMA IF NOT EXISTS fleet;

-- fleet.host_spec — one row per machine. The identity (the group_id key) is cpu, cores, ram_gb,
-- os, disk, instance_hash: instance_hash (SHA-256 of the uname nodename) tells apart VMs and
-- reinstalls on the same hardware + OS; disk tells apart checkouts on different drives.
CREATE TABLE IF NOT EXISTS fleet.host_spec (
    group_id       bigint       NOT NULL,
    cpu            text         NOT NULL,
    cores          bigint       NOT NULL,
    ram_gb         bigint       NOT NULL,
    os             text         NOT NULL,
    disk           text         NOT NULL,
    instance_hash  text         NOT NULL,
    p_cores        bigint       NOT NULL DEFAULT 0,
    cpu_mhz_min    bigint       NOT NULL DEFAULT 0,
    cpu_mhz_max    bigint       NOT NULL DEFAULT 0,
    first_seen     timestamptz  NOT NULL DEFAULT now(),
    last_seen      timestamptz  NOT NULL DEFAULT now(),
    CONSTRAINT host_spec_pk PRIMARY KEY (group_id),
    CONSTRAINT host_spec_identity_uq UNIQUE (cpu, cores, ram_gb, os, disk, instance_hash),
    CONSTRAINT host_spec_group_id_positive CHECK (group_id > 0)
);
COMMENT ON TABLE fleet.host_spec IS 'One row per machine sharing this server; group_id is its jac313-### label. Written by jac313_test_cli (fleet_registry.cpp).';
COMMENT ON COLUMN fleet.host_spec.group_id IS 'the ### in jac313-###; a machine keeps its local number when the server has it free';
COMMENT ON COLUMN fleet.host_spec.instance_hash IS 'SHA-256 of the uname nodename: tells apart VMs / reinstalls on the same hardware + OS';
COMMENT ON COLUMN fleet.host_spec.disk IS 'disk tier of the checkout (ssd, x7k, ...): one machine with checkouts on two drives is two groups';
COMMENT ON COLUMN fleet.host_spec.p_cores IS 'display only (not part of the identity)';
COMMENT ON COLUMN fleet.host_spec.last_seen IS 'the last time this machine registered or looked itself up';
