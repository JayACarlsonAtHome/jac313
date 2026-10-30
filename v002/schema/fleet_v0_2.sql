-- File:    v002/schema/fleet_v0_2.sql
-- Purpose: PostgreSQL DDL for the fleet registry on the shared (pg-remote) server: one row per
--          machine (the hardware), so every machine that shares the server gets its own jac313-###
--          (machine.id) instead of each one numbering itself 001 in its own results.db.
--          The registry only numbers machines: the OS a run ran under is recorded per run in each
--          machine's local results.db (os table), so it is not part of the identity here, and one box
--          that boots two OSes is one machine.
--          Idempotent: jac313_test_cli runs this file before it registers a machine, and it can be
--          run by hand:  psql "host=... dbname=jac313 user=jac313" -f v002/schema/fleet_v0_2.sql
--          v0_2 replaces v0_1 (fleet.host_spec, whose identity included the OS); a server that
--          still holds fleet.host_spec keeps it untouched, and machines register again here.
-- Related: shared/jac313_results_db.hpp (machine: the local copy, same identity columns),
--          tools/jac313_test_cli/fleet_registry.cpp (the register / look-up transaction).

CREATE SCHEMA IF NOT EXISTS fleet;

-- fleet.machine — one row per machine. The identity is cpu, cores, ram_gb, disk, instance_hash:
-- instance_hash (SHA-256 of the uname nodename) tells apart VMs and reinstalls on the same
-- hardware; disk tells apart checkouts on different drives.
CREATE TABLE IF NOT EXISTS fleet.machine (
    id             bigint       NOT NULL,
    cpu            text         NOT NULL,
    cores          bigint       NOT NULL,
    ram_gb         bigint       NOT NULL,
    disk           text         NOT NULL,
    instance_hash  text         NOT NULL,
    p_cores        bigint       NOT NULL DEFAULT 0,
    cpu_mhz_min    bigint       NOT NULL DEFAULT 0,
    cpu_mhz_max    bigint       NOT NULL DEFAULT 0,
    first_seen     timestamptz  NOT NULL DEFAULT now(),
    last_seen      timestamptz  NOT NULL DEFAULT now(),
    CONSTRAINT machine_pk PRIMARY KEY (id),
    CONSTRAINT machine_identity_uq UNIQUE (cpu, cores, ram_gb, disk, instance_hash),
    CONSTRAINT machine_id_positive CHECK (id > 0)
);
COMMENT ON TABLE fleet.machine IS 'One row per machine sharing this server; id is its jac313-### label. Written by jac313_test_cli (fleet_registry.cpp).';
COMMENT ON COLUMN fleet.machine.id IS 'the ### in jac313-###; a machine keeps its local number when the server has it free';
COMMENT ON COLUMN fleet.machine.instance_hash IS 'SHA-256 of the uname nodename: tells apart VMs / reinstalls on the same hardware';
COMMENT ON COLUMN fleet.machine.disk IS 'disk tier of the checkout (xssd, xx7k, ...): one machine with checkouts on two drives is two machines';
COMMENT ON COLUMN fleet.machine.p_cores IS 'display only (not part of the identity)';
COMMENT ON COLUMN fleet.machine.last_seen IS 'the last time this machine registered or looked itself up';
