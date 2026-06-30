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
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

namespace jac313::results {

using Sqlite = jac313::Qlite::v001::Sqlite;

// Reusable scalar-query helpers live in Qlite (canonical, shared); surface them here for results code.
using jac313::Qlite::v001::get_one_long;
using jac313::Qlite::v001::get_one_string;
using jac313::Qlite::v001::get_one_double;

// Debug variants: get_one_xxx(db, sql, true, binds...) or the _debug shorthands
// When debug=true they print [SQL] ... and result to stderr.
using jac313::Qlite::v001::get_one_long_debug;
using jac313::Qlite::v001::get_one_string_debug;
using jac313::Qlite::v001::get_one_double_debug;
using jac313::Qlite::v001::get_one_long_or_debug;
using jac313::Qlite::v001::get_one_string_or_debug;

// Convenience helpers for testControl table (the central store for per-test limits)
inline int get_test_timeout(Sqlite& db, const std::string& name, bool debug = false) {
    return static_cast<int>(get_one_long(db, "SELECT timeout_sec FROM testControl WHERE name=?", debug, name));
}
inline int get_test_memory_mb(Sqlite& db, const std::string& name, bool debug = false) {
    return static_cast<int>(get_one_long(db, "SELECT memory_mb FROM testControl WHERE name=?", debug, name));
}

// ---- schema (the single source of truth for results.db) ----
inline void ensure_schema(Sqlite& db) {
    db.exec("CREATE TABLE IF NOT EXISTS testType (\n"
            "    id INTEGER PRIMARY KEY\n"
            "  , name TEXT UNIQUE NOT NULL\n"
            "  , description TEXT\n"
            ")");
    db.exec("INSERT OR IGNORE INTO testType(name, description) VALUES\n"
            "    ('ctest', 'ctest unit suite')\n"
            "  , ('smoke', 'persist x output smoke matrix')\n"
            "  , ('bench', 'throughput benchmark suite')\n"
            "  , ('verify-lite', 'valgrind memcheck gate')\n"
            "  , ('verify', 'valgrind memcheck + helgrind + DRD')\n"
            "  , ('build', 'tree compile wall-clock')\n"
            "");
    db.exec("CREATE TABLE IF NOT EXISTS testList (\n"
            "    id INTEGER PRIMARY KEY\n"
            "  , name TEXT UNIQUE NOT NULL\n"
            ")");
    db.exec("CREATE TABLE IF NOT EXISTS compiler (\n"
            "    id INTEGER PRIMARY KEY\n"
            "  , name TEXT\n"
            "  , version TEXT\n"
            "  , major INTEGER\n"
            "  , UNIQUE(name, version)\n"
            ")");
    db.exec("CREATE TABLE IF NOT EXISTS parameter (\n"
            "    id INTEGER PRIMARY KEY\n"
            "  , compiler_id INTEGER\n"
            "  , build_type TEXT\n"
            "  , modules TEXT\n"
            "  , import_std TEXT\n"
            "  , size TEXT\n"
            "  , persist TEXT\n"
            "  , output_mode TEXT\n"
            "  , threads INTEGER\n"
            "  , events_per_thread INTEGER\n"
            "  , runs INTEGER\n"
            "  , batch INTEGER\n"
            "  , flag_count INTEGER\n"
            "  , valgrind_tool TEXT\n"
            ")");
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
    db.exec("CREATE UNIQUE INDEX IF NOT EXISTS ux_parameter ON parameter(\n"
            "    COALESCE(compiler_id, -1)\n"
            "  , COALESCE(build_type, '')\n"
            "  , COALESCE(modules, '')\n"
            "  , COALESCE(import_std, '')\n"
            "  , COALESCE(size, '')\n"
            "  , COALESCE(persist, '')\n"
            "  , COALESCE(output_mode, '')\n"
            "  , COALESCE(threads, -1)\n"
            "  , COALESCE(events_per_thread, -1)\n"
            "  , COALESCE(runs, -1)\n"
            "  , COALESCE(batch, -1)\n"
            "  , COALESCE(flag_count, -1)\n"
            "  , COALESCE(valgrind_tool, '')\n"
            ")");
    // run = per-run metadata only. The hardware lives in host_spec (one row per machine), so it is
    // NOT duplicated across every run; run references the machine via group_id.
    db.exec("CREATE TABLE IF NOT EXISTS run (\n"
            "    run_id INTEGER PRIMARY KEY\n"
            "  , ts_utc TEXT\n"
            "  , group_id INTEGER\n"
            "  , host TEXT\n"
            "  , version TEXT\n"   // JAC313_VERSION e.g. "v001" / "v002" — scopes runs, deletes, reports, safeness
            "  , store_ver TEXT\n"
            "  , qlite_ver TEXT\n"
            "  , jtext_ver TEXT\n"
            ")");
    // Migration: add version column (JAC313_VERSION) for scoping v001 vs v002 data
    {
        std::int64_t has_col = 0;
        { auto st = db.prepare("SELECT COUNT(*) FROM pragma_table_info('run') WHERE name='version'");
          if (st.step()) st.get(has_col); }
        if (has_col == 0) {
            db.exec("ALTER TABLE run ADD COLUMN version TEXT");
        }
    }
    // host_spec = the hardware identity, ONE row per machine (group_id). cpu/cores/ram_gb/os/disk are
    // the group_id key (disk in the key: x7k vs 10k vs ssd on one box -> distinct groups, durable
    // throughput being disk-bound); p_cores / cpu_mhz_min / cpu_mhz_max are display-only (machines table).
    db.exec("CREATE TABLE IF NOT EXISTS host_spec (\n"
            "    group_id INTEGER PRIMARY KEY\n"
            "  , cpu TEXT\n"
            "  , cores INTEGER\n"
            "  , p_cores INTEGER\n"
            "  , ram_gb INTEGER\n"
            "  , cpu_mhz_min INTEGER\n"
            "  , cpu_mhz_max INTEGER\n"
            "  , disk TEXT\n"
            "  , os TEXT\n"
            ")");
    // current_host = the single row naming which machine THIS checkout is (group_id). bootstrap/CLI
    // set it when they sense; store_bench READS it instead of self-sensing.
    db.exec("CREATE TABLE IF NOT EXISTS current_host (\n"
            "    id INTEGER PRIMARY KEY CHECK(id=1)\n"
            "  , group_id INTEGER\n"
            ")");
    db.exec("CREATE TABLE IF NOT EXISTS testRun (\n"
            "    id INTEGER PRIMARY KEY\n"
            "  , run_id INTEGER\n"
            "  , test_type_id INTEGER\n"
            "  , test_list_id INTEGER\n"
            "  , parameter_id INTEGER\n"
            "  , status TEXT\n"
            "  , duration_ms INTEGER\n"
            "  , median_ops INTEGER\n"
            "  , low_ops INTEGER\n"
            "  , high_ops INTEGER\n"
            "  , avg_ops INTEGER\n"
            "  , stddev_ops INTEGER\n"
            "  , bytes INTEGER\n"
            ")");
    // safeness = per (machine x gate x compiler) pass/fail summary, recomputed from testRun at record
    // time by rebuild_safeness() so the report + run-everything verdict READ it (never recount).
    db.exec("CREATE TABLE IF NOT EXISTS safeness (\n"
            "    group_id INTEGER\n"
            "  , version TEXT\n"  // JAC313_VERSION for isolation (v001 vs v002)
            "  , gate TEXT\n"
            "  , compiler TEXT\n"
            "  , pass INTEGER\n"
            "  , fail INTEGER\n"
            "  , PRIMARY KEY(group_id, version, gate, compiler)\n"
            ")");
    // Migration for safeness version column (JAC313_VERSION for v001/v002 isolation)
    {
        std::int64_t has_col = 0;
        { auto st = db.prepare("SELECT COUNT(*) FROM pragma_table_info('safeness') WHERE name='version'");
          if (st.step()) st.get(has_col); }
        if (has_col == 0) {
            db.exec("ALTER TABLE safeness ADD COLUMN version TEXT");
        }
    }
    // io_best_fit = the per-machine durable double-buffer calibration for each backend. The peak of a
    // batch sweep is noisy, so we record the PLATEAU: low_size/high_size are the batch sizes at the
    // bottom/top of the near-peak band, and useThis is a robust pick in the MIDDLE of that band (what
    // the recorded bench actually runs at). best_size/best_ops are the raw peak (batch + median ops/sec)
    // kept for reference. One row per (group_id, type).
    db.exec("CREATE TABLE IF NOT EXISTS io_best_fit (\n"
            "    id INTEGER PRIMARY KEY\n"
            "  , group_id INTEGER\n"
            "  , type TEXT\n"
            "  , low_size INTEGER\n"
            "  , high_size INTEGER\n"
            "  , best_size INTEGER\n"
            "  , best_ops INTEGER\n"
            "  , useThis INTEGER\n"
            "  , UNIQUE(group_id, type)\n"
            ")");

    db.exec("CREATE TABLE IF NOT EXISTS runEvent (\n"
            "    id INTEGER PRIMARY KEY\n"
            "  , run_id INTEGER\n"
            "  , ts_utc TEXT\n"
            "  , test_name TEXT\n"
            "  , event TEXT\n"
            "  , message TEXT\n"
            "  , duration_ms INTEGER\n"
            ")");

    // testControl: per-test runtime controls (timeouts, memory limits, etc.) that drive the test harness.
    // Values live here in the DB (not hardcoded in source or flat files at runtime).
    // The human-editable tests/test_limits.txt (or equivalent) is used to seed/refresh this table.
    db.exec("CREATE TABLE IF NOT EXISTS testControl ("
            "name TEXT PRIMARY KEY, "
            "timeout_sec INTEGER NOT NULL DEFAULT 300, "
            "memory_mb INTEGER NOT NULL DEFAULT 0, "
            "description TEXT)");
    // migration for memory_mb column (for DBs created before this column was added)
    {
        std::int64_t has_col = 0;
        { auto st = db.prepare("SELECT COUNT(*) FROM pragma_table_info('testControl') WHERE name='memory_mb'");
          if (st.step()) st.get(has_col); }
        if (has_col == 0) {
            db.exec("ALTER TABLE testControl ADD COLUMN memory_mb INTEGER DEFAULT 0");
        }
    }
}

// single-id lookup (e.g. "SELECT id FROM testList WHERE name=?")
inline std::int64_t lookup_id(Sqlite& db, const char* sql, const std::string& name) {
    std::int64_t id = 0; auto st = db.prepare(sql); st.bind(name); if (st.step()) st.get(id); return id;
}

// Recompute the safeness summary from testRun. Called at the END of every recorder so the report +
// run-everything verdict just READ the table (never recount). 
// Now also keys on JAC313_VERSION so v001 and v002 data stay isolated even for the same group/hardware.
inline void rebuild_safeness(Sqlite& db) {
    db.exec("DELETE FROM safeness");
    // pass/fail are NULL-safe: bench rows store throughput with status=NULL (a row exists only on
    // success), so COALESCE(status,'pass') treats them as pass; only 'fail'/'error' count as failures.
    db.exec("INSERT INTO safeness (\n"
            "    group_id\n"
            "  , version\n"
            "  , gate\n"
            "  , compiler\n"
            "  , pass\n"
            "  , fail\n"
            ")\n"
            "SELECT \n"
            "    r.group_id\n"
            "  , COALESCE(r.version, '')\n"
            "  , tt.name\n"
            "  , c.name || c.major\n"
            "  , SUM(CASE WHEN COALESCE(tr.status, 'pass') IN ('fail', 'error') THEN 0 ELSE 1 END)\n"
            "  , SUM(CASE WHEN COALESCE(tr.status, 'pass') IN ('fail', 'error') THEN 1 ELSE 0 END)\n"
            "FROM testRun tr\n"
            "INNER JOIN run r ON r.run_id = tr.run_id\n"
            "INNER JOIN testType tt ON tt.id = tr.test_type_id\n"
            "INNER JOIN parameter p ON p.id = tr.parameter_id\n"
            "INNER JOIN compiler c ON c.id = p.compiler_id\n"
            "WHERE tt.name IN ('ctest', 'smoke', 'bench')\n"
            "GROUP BY r.group_id, COALESCE(r.version, ''), tt.name, c.name || c.major");
    db.exec("INSERT INTO safeness (\n"
            "    group_id\n"
            "  , version\n"
            "  , gate\n"
            "  , compiler\n"
            "  , pass\n"
            "  , fail\n"
            ")\n"
            "SELECT \n"
            "    r.group_id\n"
            "  , COALESCE(r.version, '')\n"
            "  , p.valgrind_tool\n"
            "  , c.name || c.major\n"
            "  , SUM(CASE WHEN COALESCE(tr.status, 'pass') IN ('fail', 'error') THEN 0 ELSE 1 END)\n"
            "  , SUM(CASE WHEN COALESCE(tr.status, 'pass') IN ('fail', 'error') THEN 1 ELSE 0 END)\n"
            "FROM testRun tr\n"
            "INNER JOIN run r ON r.run_id = tr.run_id\n"
            "INNER JOIN testType tt ON tt.id = tr.test_type_id\n"
            "INNER JOIN parameter p ON p.id = tr.parameter_id\n"
            "INNER JOIN compiler c ON c.id = p.compiler_id\n"
            "WHERE tt.name IN ('verify', 'verify-lite') AND p.valgrind_tool IS NOT NULL\n"
            "GROUP BY r.group_id, COALESCE(r.version, ''), p.valgrind_tool, c.name || c.major");
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

// Delete the most recent run (and its testRun/runEvent rows) for this group + optional JAC313_VERSION.
// Used for explicit "fresh start for this machine" (e.g. top of --run-everything,
// and the jac313_wipe_* tools). 
// Recorders scope deletes to the version so v001 cannot affect v002 data (same hardware group).
inline void delete_prior_run_for_group(Sqlite& db, std::int64_t gid, const std::string& version = "") {
    std::int64_t prior_run = 0;
    if (version.empty()) {
        auto st = db.prepare("SELECT run_id FROM run WHERE group_id=? ORDER BY run_id DESC LIMIT 1");
        st.bind(gid); if (st.step()) st.get(prior_run);
    } else {
        auto st = db.prepare("SELECT run_id FROM run WHERE group_id=? AND version=? ORDER BY run_id DESC LIMIT 1");
        st.bind(gid); st.bind(version); if (st.step()) st.get(prior_run);
    }
    if (prior_run > 0) {
        db.exec("DELETE FROM testRun WHERE run_id=?", prior_run);
        db.exec("DELETE FROM runEvent WHERE run_id=?", prior_run);
        db.exec("DELETE FROM run WHERE run_id=?", prior_run);
    }
}

inline std::int64_t next_run_id(Sqlite& db) {
    std::int64_t n = 1;
    { auto st = db.prepare("SELECT COALESCE(MAX(run_id),0)+1 FROM run"); if (st.step()) st.get(n); }
    return n;
}

// Insert a run row (idempotent on run_id). version = JAC313_VERSION ("v001"/"v002").
// store_ver/qlite_ver/jtext_ver are the component versions under test (default empty for functional runs).
inline void insert_run(Sqlite& db, std::int64_t run_id, std::int64_t gid, const std::string& host,
                       const std::string& version = "",
                       const std::string& store_ver = "",
                       const std::string& qlite_ver = "", const std::string& jtext_ver = "") {
    db.exec("INSERT OR IGNORE INTO run(run_id, ts_utc, group_id, host, version, store_ver, qlite_ver, jtext_ver) "
            "VALUES(?, strftime('%Y-%m-%dT%H:%M:%SZ','now'), ?,?,?,?,?,?)",
            run_id, gid, host, version, store_ver, qlite_ver, jtext_ver);
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

// ---- testControl: runtime controls per test name (timeouts etc.) stored in DB ----

// Upsert a control row for a test. Call this when loading from test_limits.txt or admin tools.
inline void upsert_test_control(Sqlite& db, const std::string& name, int timeout_sec, int memory_mb = 0,
                                const std::string& description = "") {
    db.exec("INSERT INTO testControl(name, timeout_sec, memory_mb, description) VALUES(?,?,?,?) "
            "ON CONFLICT(name) DO UPDATE SET timeout_sec=excluded.timeout_sec, "
            "memory_mb=excluded.memory_mb, "
            "description=COALESCE(NULLIF(excluded.description,''), testControl.description)",
            name, timeout_sec, memory_mb, description);
}

// Retrieve timeout for a specific test. Falls back to the provided default (usually from size).
inline int get_test_timeout(Sqlite& db, const std::string& test_name, int fallback) {
    std::int64_t v = 0;
    { auto st = db.prepare("SELECT timeout_sec FROM testControl WHERE name=?");
      st.bind(test_name); if (st.step()) st.get(v); }
    return v > 0 ? static_cast<int>(v) : fallback;
}

// Retrieve memory limit (MB) for a specific test. 0 means unlimited. Falls back to provided default.
inline int get_test_memory_mb(Sqlite& db, const std::string& test_name, int fallback) {
    std::int64_t v = 0;
    { auto st = db.prepare("SELECT memory_mb FROM testControl WHERE name=?");
      st.bind(test_name); if (st.step()) st.get(v); }
    return v > 0 ? static_cast<int>(v) : fallback;
}

// Load/refresh controls from a limits file into the DB (source of truth for values stays editable
// in the file for humans; working data is in the DB table).
// Format: name timeout_sec [memory_mb]
inline void seed_test_controls_from_file(Sqlite& db, const std::filesystem::path& limits_file) {
    std::ifstream in(limits_file);
    if (!in) return;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::string name;
        int secs = 0;
        if (iss >> name >> secs && secs > 0) {
            int mem = 0;
            iss >> mem;  // optional
            upsert_test_control(db, name, secs, mem);
        }
    }
}

// basic utc and log for status tracking
inline std::string utc_now_iso() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    gmtime_r(&t, &tm);
    std::ostringstream out;
    out << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return out.str();
}

inline void log_run_event(Sqlite& db, std::int64_t run_id, const std::string& test_name,
                          const std::string& event, const std::string& message = "",
                          std::int64_t duration_ms = 0) {
    const std::string ts = utc_now_iso();
    db.exec("INSERT INTO runEvent(run_id, ts_utc, test_name, event, message, duration_ms) "
            "VALUES(?,?,?,?,?,?)",
            run_id, ts, test_name, event, message, duration_ms);
}

} // namespace jac313::results
