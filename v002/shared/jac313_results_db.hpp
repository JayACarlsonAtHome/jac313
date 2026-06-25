// jac313 results.db — shared schema + dimension helpers.
//
// Both writers (Store/tests/matrix/store_bench.cpp and tools/jac313_test_cli/main.cpp) include this
// so the schema DDL and the identical compiler/lookup/run helpers live in ONE place (no drift). The
// per-test-kind parameter resolvers and the recorders themselves stay in each writer — they genuinely
// differ (bench writes ops stats; the CLI writes status/duration; the parameter projections vary).
#pragma once

#include <jac313/Qlite/v002.hpp>

#include <cstdint>
#include <cstdio>
#include <string>

namespace jac313::results {

using Sqlite = jac313::Qlite::v002::Sqlite;

// ---- schema (the single source of truth for results.db) ----
inline void ensure_schema(Sqlite& db) {
    db.exec("CREATE TABLE IF NOT EXISTS testType (id INTEGER PRIMARY KEY, name TEXT UNIQUE NOT NULL, description TEXT)");
    db.exec("INSERT OR IGNORE INTO testType(name, description) VALUES"
            " ('ctest','ctest unit suite'),('smoke','persist x output smoke matrix'),"
            " ('bench','throughput benchmark suite'),('verify-lite','valgrind memcheck gate'),"
            " ('verify','valgrind memcheck + helgrind + DRD')");
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
    db.exec("CREATE TABLE IF NOT EXISTS run ("
            "run_id INTEGER PRIMARY KEY, ts_utc TEXT, group_id INTEGER, host TEXT, cpu TEXT, cores INTEGER, "
            "ram_gb INTEGER, os TEXT, store_ver TEXT, qlite_ver TEXT, jtext_ver TEXT)");
    db.exec("CREATE TABLE IF NOT EXISTS testRun ("
            "id INTEGER PRIMARY KEY, run_id INTEGER, test_type_id INTEGER, test_list_id INTEGER, parameter_id INTEGER, "
            "status TEXT, duration_ms INTEGER, median_ops INTEGER, low_ops INTEGER, high_ops INTEGER, "
            "avg_ops INTEGER, stddev_ops INTEGER, bytes INTEGER)");
}

// single-id lookup (e.g. "SELECT id FROM testList WHERE name=?")
inline std::int64_t lookup_id(Sqlite& db, const char* sql, const std::string& name) {
    std::int64_t id = 0; auto st = db.prepare(sql); st.bind(name); if (st.step()) st.get(id); return id;
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
struct HostId { std::string cpu; std::int64_t cores = 0; std::int64_t ram_gb = 0; std::string os; };

inline std::string host_label(std::int64_t group_id) {
    char b[24]; std::snprintf(b, sizeof b, "jac313-%03lld", static_cast<long long>(group_id)); return b;
}

// group_id is keyed on hardware+os only (host is recorded as the anonymized label). Both writers must
// build HostId identically (same /proc parsing) so a machine resolves to one group across the DB.
inline std::int64_t group_id(Sqlite& db, const HostId& h) {
    std::int64_t gid = 0;
    { auto st = db.prepare("SELECT group_id FROM run WHERE cpu=? AND cores=? AND ram_gb=? AND os=? LIMIT 1");
      st.bind(h.cpu, h.cores, h.ram_gb, h.os); if (st.step()) st.get(gid); }
    if (gid == 0) { auto st = db.prepare("SELECT COALESCE(MAX(group_id),0)+1 FROM run"); if (st.step()) st.get(gid); }
    return gid;
}

inline std::int64_t next_run_id(Sqlite& db) {
    std::int64_t n = 1;
    { auto st = db.prepare("SELECT COALESCE(MAX(run_id),0)+1 FROM run"); if (st.step()) st.get(n); }
    return n;
}

// Insert a run row (idempotent on run_id). store_ver/qlite_ver/jtext_ver default empty (functional runs).
inline void insert_run(Sqlite& db, std::int64_t run_id, std::int64_t gid, const std::string& host,
                       const HostId& h, const std::string& store_ver = "",
                       const std::string& qlite_ver = "", const std::string& jtext_ver = "") {
    db.exec("INSERT OR IGNORE INTO run(run_id, ts_utc, group_id, host, cpu, cores, ram_gb, os, "
            "store_ver, qlite_ver, jtext_ver) VALUES(?, strftime('%Y-%m-%dT%H:%M:%SZ','now'), ?,?,?,?,?,?, ?,?,?)",
            run_id, gid, host, h.cpu, h.cores, h.ram_gb, h.os, store_ver, qlite_ver, jtext_ver);
}

} // namespace jac313::results
