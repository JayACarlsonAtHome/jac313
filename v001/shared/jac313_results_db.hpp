// jac313 results.db — shared schema + dimension helpers.
//
// Both writers (Store/tests/matrix/store_bench.cpp and tools/jac313_test_cli/main.cpp) include this
// so the schema DDL and the identical compiler/lookup/run helpers live in ONE place (no drift). The
// per-test-kind parameter resolvers and the recorders themselves stay in each writer — they genuinely
// differ (bench writes ops stats; the CLI writes status/duration; the parameter projections vary).
#pragma once

#include <jac313/Qlite/v001.hpp>

#include <cstdint>
#include <cstdio>
#include <string>

namespace jac313::results {

using Sqlite = jac313::Qlite::v001::Sqlite;

// Reusable scalar-query helpers live in Qlite (canonical, shared); surface them here for results code.
using jac313::Qlite::v001::get_one_long;
using jac313::Qlite::v001::get_one_string;
using jac313::Qlite::v001::get_one_double;

// ---- schema (the single source of truth for results.db) ----
inline void ensure_schema(Sqlite& db) {
    db.exec("CREATE TABLE IF NOT EXISTS testType (id INTEGER PRIMARY KEY, name TEXT UNIQUE NOT NULL, description TEXT)");
    db.exec("INSERT OR IGNORE INTO testType(name, description) VALUES"
            " ('ctest','ctest unit suite'),('smoke','persist x output smoke matrix'),"
            " ('bench','throughput benchmark suite'),('verify-lite','valgrind memcheck gate'),"
            " ('verify','valgrind memcheck + helgrind + DRD'),('build','tree compile wall-clock')");
    db.exec("CREATE TABLE IF NOT EXISTS testList (id INTEGER PRIMARY KEY, name TEXT UNIQUE NOT NULL)");
    db.exec("CREATE TABLE IF NOT EXISTS compiler ("
            "id INTEGER PRIMARY KEY, name TEXT, version TEXT, major INTEGER, UNIQUE(name, version))");
    db.exec("CREATE TABLE IF NOT EXISTS parameter ("
            "id INTEGER PRIMARY KEY, compiler_id INTEGER, build_type TEXT, modules TEXT, import_std TEXT, size TEXT, "
            "persist TEXT, output_mode TEXT, threads INTEGER, events_per_thread INTEGER, runs INTEGER, "
            "batch INTEGER, flag_count INTEGER, valgrind_tool TEXT)");
    // Migrate older parameter tables: add import_std (backfill 'off' = textual) and rebuild a stale
    // unique index. Each probe statement is finalized in its OWN scope BEFORE any DDL, so the DDL
    // never hits a read-lock from a still-open cursor.
    {
        std::int64_t has_col = 0;
        { auto st = db.prepare("SELECT COUNT(*) FROM pragma_table_info('parameter') WHERE name='import_std'");
          if (st.step()) st.get(has_col); }
        if (has_col == 0) {
            db.exec("ALTER TABLE parameter ADD COLUMN import_std TEXT");
            db.exec("UPDATE parameter SET import_std='off' WHERE import_std IS NULL");
        }
        std::int64_t stale = 0;
        { auto st = db.prepare("SELECT COUNT(*) FROM sqlite_master WHERE type='index' AND name='ux_parameter' "
                               "AND sql NOT LIKE '%import_std%'");
          if (st.step()) st.get(stale); }
        if (stale > 0) db.exec("DROP INDEX IF EXISTS ux_parameter");
    }
    // Real uniqueness: a table-level UNIQUE over these nullable columns is a no-op (SQLite treats
    // NULLs as distinct). Enforce the combo with a COALESCE'd unique index covering all 13 columns.
    db.exec("CREATE UNIQUE INDEX IF NOT EXISTS ux_parameter ON parameter("
            "COALESCE(compiler_id,-1), COALESCE(build_type,''), COALESCE(modules,''), COALESCE(import_std,''), "
            "COALESCE(size,''), COALESCE(persist,''), COALESCE(output_mode,''), COALESCE(threads,-1), "
            "COALESCE(events_per_thread,-1), COALESCE(runs,-1), COALESCE(batch,-1), COALESCE(flag_count,-1), "
            "COALESCE(valgrind_tool,''))");
    // run = per-run metadata only. The hardware lives in host_spec (one row per machine), so it is
    // NOT duplicated across every run; run references the machine via group_id.
    db.exec("CREATE TABLE IF NOT EXISTS run ("
            "run_id INTEGER PRIMARY KEY, ts_utc TEXT, group_id INTEGER, host TEXT, "
            "store_ver TEXT, qlite_ver TEXT, jtext_ver TEXT)");
    // host_spec = the hardware identity, ONE row per machine (group_id). cpu/cores/ram_gb/os/disk are
    // the group_id key (disk in the key: x7k vs 10k vs ssd on one box -> distinct groups, durable
    // throughput being disk-bound); p_cores / cpu_mhz_min / cpu_mhz_max are display-only (machines table).
    db.exec("CREATE TABLE IF NOT EXISTS host_spec ("
            "group_id INTEGER PRIMARY KEY, cpu TEXT, cores INTEGER, p_cores INTEGER, ram_gb INTEGER, "
            "cpu_mhz_min INTEGER, cpu_mhz_max INTEGER, disk TEXT, os TEXT)");
    // current_host = the single row naming which machine THIS checkout is (group_id). bootstrap/CLI
    // set it when they sense; store_bench READS it instead of self-sensing.
    db.exec("CREATE TABLE IF NOT EXISTS current_host (id INTEGER PRIMARY KEY CHECK(id=1), group_id INTEGER)");
    db.exec("CREATE TABLE IF NOT EXISTS testRun ("
            "id INTEGER PRIMARY KEY, run_id INTEGER, test_type_id INTEGER, test_list_id INTEGER, parameter_id INTEGER, "
            "status TEXT, duration_ms INTEGER, median_ops INTEGER, low_ops INTEGER, high_ops INTEGER, "
            "avg_ops INTEGER, stddev_ops INTEGER, bytes INTEGER)");
    // safeness = per (machine x gate x compiler) pass/fail summary, recomputed from testRun at record
    // time by rebuild_safeness() so the report + run-everything verdict READ it (never recount).
    db.exec("CREATE TABLE IF NOT EXISTS safeness ("
            "group_id INTEGER, gate TEXT, compiler TEXT, pass INTEGER, fail INTEGER, "
            "PRIMARY KEY(group_id, gate, compiler))");
    // io_best_fit = the per-machine durable double-buffer calibration for each backend. The peak of a
    // batch sweep is noisy, so we record the PLATEAU: low_size/high_size are the batch sizes at the
    // bottom/top of the near-peak band, and useThis is a robust pick in the MIDDLE of that band (what
    // the recorded bench actually runs at). best_size/best_ops are the raw peak (batch + median ops/sec)
    // kept for reference. One row per (group_id, type).
    db.exec("CREATE TABLE IF NOT EXISTS io_best_fit ("
            "id INTEGER PRIMARY KEY, group_id INTEGER, type TEXT, "
            "low_size INTEGER, high_size INTEGER, best_size INTEGER, best_ops INTEGER, useThis INTEGER, "
            "UNIQUE(group_id, type))");
}

// single-id lookup (e.g. "SELECT id FROM testList WHERE name=?")
inline std::int64_t lookup_id(Sqlite& db, const char* sql, const std::string& name) {
    std::int64_t id = 0; auto st = db.prepare(sql); st.bind(name); if (st.step()) st.get(id); return id;
}

// Recompute the safeness summary from testRun. Called at the END of every recorder so the report +
// run-everything verdict just READ the table (never recount). compiler label = name||major (== comp_label).
// NOTE: counts aggregate across ALL runs for a (group,gate,compiler); run-everything wipes the DB fresh
// so that's one run each. (Incremental re-runs without a wipe would accumulate — acceptable for now.)
inline void rebuild_safeness(Sqlite& db) {
    db.exec("DELETE FROM safeness");
    // pass/fail are NULL-safe: bench rows store throughput with status=NULL (a row exists only on
    // success), so COALESCE(status,'pass') treats them as pass; only 'fail'/'error' count as failures.
    db.exec("INSERT INTO safeness(group_id, gate, compiler, pass, fail) "
            "SELECT r.group_id, tt.name, c.name||c.major, "
            "SUM(CASE WHEN COALESCE(tr.status,'pass') IN ('fail','error') THEN 0 ELSE 1 END), "
            "SUM(CASE WHEN COALESCE(tr.status,'pass') IN ('fail','error') THEN 1 ELSE 0 END) "
            "FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
            "JOIN testType tt ON tt.id=tr.test_type_id "
            "JOIN parameter p ON p.id=tr.parameter_id JOIN compiler c ON c.id=p.compiler_id "
            "WHERE tt.name IN ('ctest','smoke','bench') "
            "GROUP BY r.group_id, tt.name, c.name||c.major");
    db.exec("INSERT INTO safeness(group_id, gate, compiler, pass, fail) "
            "SELECT r.group_id, p.valgrind_tool, c.name||c.major, "
            "SUM(CASE WHEN COALESCE(tr.status,'pass') IN ('fail','error') THEN 0 ELSE 1 END), "
            "SUM(CASE WHEN COALESCE(tr.status,'pass') IN ('fail','error') THEN 1 ELSE 0 END) "
            "FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
            "JOIN testType tt ON tt.id=tr.test_type_id "
            "JOIN parameter p ON p.id=tr.parameter_id JOIN compiler c ON c.id=p.compiler_id "
            "WHERE tt.name IN ('verify','verify-lite') AND p.valgrind_tool IS NOT NULL "
            "GROUP BY r.group_id, p.valgrind_tool, c.name||c.major");
}

// ---- compiler dimension ----
struct CompilerInfo { std::string name; std::string version; std::int64_t major = 0; };
inline std::int64_t compiler_id(Sqlite& db, const CompilerInfo& c) {
    db.exec("INSERT OR IGNORE INTO compiler(name, version, major) VALUES(?,?,?)", c.name, c.version, c.major);
    std::int64_t id = 0;
    { auto st = db.prepare("SELECT id FROM compiler WHERE name=? AND version=?");
      st.bind(c.name, c.version); if (st.step()) st.get(id); }
    return id;
}

// ---- machine identity + run ----
// Identity (group_id key) = cpu, cores, ram_gb, os, disk. The trailing p_cores / cpu_mhz_min /
// cpu_mhz_max are DISPLAY-ONLY (machines table) — carried here for convenience, NOT part of the key.
struct HostId { std::string cpu; std::int64_t cores = 0; std::int64_t ram_gb = 0; std::string os; std::string disk;
                std::int64_t p_cores = 0; std::int64_t cpu_mhz_min = 0; std::int64_t cpu_mhz_max = 0; };

inline std::string host_label(std::int64_t group_id) {
    char b[24]; std::snprintf(b, sizeof b, "jac313-%03lld", static_cast<long long>(group_id)); return b;
}

// group_id is keyed on hardware+os+disk (host is recorded as the anonymized label). Disk tier is in
// the key so the same box on x7k vs 10k vs ssd resolves to DISTINCT groups (durable throughput is
// disk-bound). Both writers must build HostId identically so a machine resolves to one group.
inline std::int64_t group_id(Sqlite& db, const HostId& h) {
    std::int64_t gid = 0;
    { auto st = db.prepare("SELECT group_id FROM host_spec WHERE cpu=? AND cores=? AND ram_gb=? AND os=? AND disk=? LIMIT 1");
      st.bind(h.cpu, h.cores, h.ram_gb, h.os, h.disk); if (st.step()) st.get(gid); }
    if (gid == 0) { auto st = db.prepare("SELECT COALESCE(MAX(group_id),0)+1 FROM host_spec"); if (st.step()) st.get(gid); }
    return gid;
}

// Upsert this machine's hardware row (one per group_id).
inline void upsert_host_spec(Sqlite& db, std::int64_t gid, const HostId& h) {
    db.exec("INSERT OR REPLACE INTO host_spec(group_id, cpu, cores, p_cores, ram_gb, cpu_mhz_min, cpu_mhz_max, disk, os) "
            "VALUES(?,?,?,?,?,?,?,?,?)",
            gid, h.cpu, h.cores, h.p_cores, h.ram_gb, h.cpu_mhz_min, h.cpu_mhz_max, h.disk, h.os);
}
// Pin which machine THIS checkout is (the single current_host row).
inline void set_current_host(Sqlite& db, std::int64_t gid) {
    db.exec("INSERT OR REPLACE INTO current_host(id, group_id) VALUES(1, ?)", gid);
}
// Read the pinned group_id (0 if not yet set).
inline std::int64_t current_host(Sqlite& db) {
    std::int64_t gid = 0;
    { auto st = db.prepare("SELECT group_id FROM current_host WHERE id=1"); if (st.step()) st.get(gid); }
    return gid;
}
// Resolve this machine: find-or-assign group_id, write host_spec + current_host, return group_id.
// The CLI/bootstrap call this when they sense; store_bench just reads current_host().
inline std::int64_t pin_host(Sqlite& db, const HostId& h) {
    const std::int64_t gid = group_id(db, h);
    upsert_host_spec(db, gid, h);
    set_current_host(db, gid);
    return gid;
}

inline std::int64_t next_run_id(Sqlite& db) {
    std::int64_t n = 1;
    { auto st = db.prepare("SELECT COALESCE(MAX(run_id),0)+1 FROM run"); if (st.step()) st.get(n); }
    return n;
}

// Insert a run row (idempotent on run_id). store_ver/qlite_ver/jtext_ver default empty (functional runs).
inline void insert_run(Sqlite& db, std::int64_t run_id, std::int64_t gid, const std::string& host,
                       const std::string& store_ver = "",
                       const std::string& qlite_ver = "", const std::string& jtext_ver = "") {
    db.exec("INSERT OR IGNORE INTO run(run_id, ts_utc, group_id, host, store_ver, qlite_ver, jtext_ver) "
            "VALUES(?, strftime('%Y-%m-%dT%H:%M:%SZ','now'), ?,?,?,?,?)",
            run_id, gid, host, store_ver, qlite_ver, jtext_ver);
}

// ---- io_best_fit (per-machine durable-backend batch calibration) ----
// Record (or refresh) one backend's calibration on this machine: the plateau band [low_size,high_size],
// the raw peak (best_size + best_ops), and useThis = the chosen mid-band operating size.
inline void upsert_io_best_fit(Sqlite& db, std::int64_t gid, const std::string& type,
                               std::int64_t low_size, std::int64_t high_size,
                               std::int64_t best_size, std::int64_t best_ops, std::int64_t use_this) {
    db.exec("INSERT INTO io_best_fit(group_id, type, low_size, high_size, best_size, best_ops, useThis) "
            "VALUES(?,?,?,?,?,?,?) "
            "ON CONFLICT(group_id, type) DO UPDATE SET low_size=excluded.low_size, high_size=excluded.high_size, "
            "best_size=excluded.best_size, best_ops=excluded.best_ops, useThis=excluded.useThis",
            gid, type, low_size, high_size, best_size, best_ops, use_this);
}

// Read this machine's chosen batch (useThis) for a backend; returns fallback if not yet calibrated.
inline std::int64_t io_best_fit_use(Sqlite& db, std::int64_t gid, const std::string& type,
                                    std::int64_t fallback) {
    std::int64_t v = 0;
    { auto st = db.prepare("SELECT useThis FROM io_best_fit WHERE group_id=? AND type=?");
      st.bind(gid, type); if (st.step()) st.get(v); }
    return v > 0 ? v : fallback;
}

} // namespace jac313::results
