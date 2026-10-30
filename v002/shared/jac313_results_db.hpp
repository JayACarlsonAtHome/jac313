// jac313 results.db — shared schema + dimension helpers.
//
// Both writers (Store/tests/matrix/store_bench.cpp and tools/jac313_test_cli/main.cpp) include this
// so the schema DDL and the identical compiler/lookup/run helpers live in ONE place (no drift). The
// per-test-kind parameter resolvers and the recorders themselves stay in each writer — they genuinely
// differ (bench writes ops stats; the CLI writes status/duration; the parameter projections vary).
#pragma once

#include <jac313/Qlite/v002.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace jac313::results {

using Sqlite = jac313::Qlite::v002::Sqlite;

// Reusable scalar-query helpers live in Qlite (canonical, shared); surface them here for results code.
using jac313::Qlite::v002::get_top_one_long;
using jac313::Qlite::v002::get_top_one_string;
using jac313::Qlite::v002::get_top_one_double;
using jac313::Qlite::v002::get_top_one_string_or;
using jac313::Qlite::v002::get_top_one;       // get_top_one<T>(db, sql, [debug,] binds...): the top row's first value
using jac313::Qlite::v002::get_top_one_or;
using jac313::Qlite::v002::get_top_one_bool;
using jac313::Qlite::v002::get_top_one_bool_or;
using jac313::Qlite::v002::get_top_one_id;      // 0 = found none
using jac313::Qlite::v002::get_top_one_id_or;   // your own "found none", e.g. -1

// Debug variants: get_top_one_xxx(db, sql, true, binds...) or the _debug shorthands
// When debug=true they print [SQL] ... and result to stderr.
using jac313::Qlite::v002::get_top_one_long_debug;
using jac313::Qlite::v002::get_top_one_string_debug;
using jac313::Qlite::v002::get_top_one_double_debug;
using jac313::Qlite::v002::get_top_one_long_or_debug;
using jac313::Qlite::v002::get_top_one_string_or_debug;

// Convenience helpers for testControl table (the central store for per-test limits)
inline int get_test_timeout(Sqlite& db, const std::string& name, bool debug = false) {
    return static_cast<int>(get_top_one_long(db, "SELECT timeout_sec FROM testControl WHERE name=?", debug, name));
}
inline int get_test_memory_mb(Sqlite& db, const std::string& name, bool debug = false) {
    return static_cast<int>(get_top_one_long(db, "SELECT memory_mb FROM testControl WHERE name=?", debug, name));
}

// The one shared results DB — full ABSOLUTE path, resolved ONCE at startup, then read everywhere by
// name. It lives at the monorepo root (parent of every version dir), so v001/v002 (and later every
// machine) post to the same store. Deterministic — a fixed location, not an env var. Each process calls
// resolve_results_db() once from main(); every other site just reads JAC313_RESULTS_DB. This is the
// single seam to swap for an external DB later (resolve to a DSN / open an adapter here).
inline std::string JAC313_RESULTS_DB;   // set once by resolve_results_db(); read everywhere

inline const std::string& resolve_results_db(const std::filesystem::path& source_dir) {
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::path base = fs::weakly_canonical(fs::absolute(source_dir, ec), ec);
    if (base.empty()) base = source_dir;
    fs::path p = base.parent_path() / "test-summary" / "results.db";
    fs::create_directories(p.parent_path(), ec);
    JAC313_RESULTS_DB = p.string();
    return JAC313_RESULTS_DB;
}

// ---- schema (the single source of truth for results.db) ----
//
// No NULL is ever stored: every column is NOT NULL with a DEFAULT ('' = no text, 0 = no number).
// NULL can only appear in a query result, from a LEFT JOIN with no match or an aggregate over no rows.
//
// Schema version rule: PRAGMA user_version holds kResultsSchemaVersion. results.db holds test data
// only, so there are no migrations: a DB stamped with any other version (0 = an unversioned older
// layout) is dropped and created fresh on first open. Change any table or index below -> bump this.
// 2: testRun.hot_ops and drain_keeper_ms / drain_database_ms / drain_binary_ms (the production bench run).
// 3: parameter.contracts (the contract evaluation mode the binaries were built under).
// 4: run_target (what each run wrote where); parameter.strict / hardened; testRun.batch_keeper /
//    batch_database / batch_binary (the batch each sink used).
inline constexpr int kResultsSchemaVersion = 4;

namespace detail {
// Every table and index, all idempotent (IF NOT EXISTS): cheap to run on every open.
inline void create_results_tables(Sqlite& db) {
    db.exec("CREATE TABLE IF NOT EXISTS testType (\n"
            "    id INTEGER PRIMARY KEY NOT NULL\n"
            "  , name TEXT NOT NULL UNIQUE\n"
            "  , description TEXT NOT NULL DEFAULT ''\n"
            ")");
    db.exec("CREATE TABLE IF NOT EXISTS testList (\n"
            "    id INTEGER PRIMARY KEY NOT NULL\n"
            "  , name TEXT NOT NULL UNIQUE\n"
            ")");
    db.exec("CREATE TABLE IF NOT EXISTS compiler (\n"
            "    id INTEGER PRIMARY KEY NOT NULL\n"
            "  , name TEXT NOT NULL DEFAULT ''\n"
            "  , version TEXT NOT NULL DEFAULT ''\n"
            "  , major INTEGER NOT NULL DEFAULT 0\n"
            "  , UNIQUE(name, version)\n"
            ")");
    // An axis that does not apply to a test kind is '' or 0 (never NULL), so the plain table-level
    // UNIQUE over all 16 axes really dedupes (SQLite treats NULLs as distinct; there are none).
    // build_type is CMAKE_BUILD_TYPE as built (Debug | Release); strict / hardened are JAC313_STRICT /
    // JAC313_HARDENED of that tree (on | off). batch: the persistence batch (rows per sink call) the
    // run used, 0 when nothing persisted (the per-sink values are on testRun).
    db.exec("CREATE TABLE IF NOT EXISTS parameter (\n"
            "    id INTEGER PRIMARY KEY NOT NULL\n"
            "  , compiler_id INTEGER NOT NULL DEFAULT 0\n"
            "  , build_type TEXT NOT NULL DEFAULT ''\n"
            "  , modules TEXT NOT NULL DEFAULT ''\n"
            "  , import_std TEXT NOT NULL DEFAULT ''\n"
            "  , size TEXT NOT NULL DEFAULT ''\n"
            "  , persist TEXT NOT NULL DEFAULT ''\n"            // none | sql | jtext ... | the production pair "jtext+sql"
            "  , output_mode TEXT NOT NULL DEFAULT ''\n"
            "  , threads INTEGER NOT NULL DEFAULT 0\n"
            "  , events_per_thread INTEGER NOT NULL DEFAULT 0\n"
            "  , runs INTEGER NOT NULL DEFAULT 0\n"
            "  , batch INTEGER NOT NULL DEFAULT 0\n"
            "  , flag_count INTEGER NOT NULL DEFAULT 0\n"
            "  , valgrind_tool TEXT NOT NULL DEFAULT ''\n"
            "  , contracts TEXT NOT NULL DEFAULT 'enforce'\n"   // enforce | observe | quick_enforce | ignore (JAC313_CONTRACTS)
            "  , strict TEXT NOT NULL DEFAULT 'off'\n"           // on | off (JAC313_STRICT)
            "  , hardened TEXT NOT NULL DEFAULT 'off'\n"         // on | off (JAC313_HARDENED)
            "  , UNIQUE(compiler_id, build_type, modules, import_std, size, persist, output_mode, threads,\n"
            "           events_per_thread, runs, batch, flag_count, valgrind_tool, contracts, strict, hardened)\n"
            ")");
    // run = per-run metadata only. The hardware lives in host_spec (one row per machine), so it is
    // NOT duplicated across every run; run references the machine via group_id.
    db.exec("CREATE TABLE IF NOT EXISTS run (\n"
            "    run_id INTEGER PRIMARY KEY NOT NULL\n"
            "  , ts_utc TEXT NOT NULL DEFAULT ''\n"
            "  , group_id INTEGER NOT NULL DEFAULT 0\n"
            "  , host TEXT NOT NULL DEFAULT ''\n"
            "  , version TEXT NOT NULL DEFAULT ''\n"   // JAC313_VERSION e.g. "v001" / "v002" — scopes runs, deletes, reports, safeness
            "  , store_ver TEXT NOT NULL DEFAULT ''\n"
            "  , qlite_ver TEXT NOT NULL DEFAULT ''\n"
            "  , jtext_ver TEXT NOT NULL DEFAULT ''\n"
            ")");
    // host_spec = the hardware identity, ONE row per machine (group_id). cpu/cores/ram_gb/os/disk/
    // instance_hash are the group_id key (disk in the key: x7k vs 10k vs ssd on one box -> distinct
    // groups; instance_hash = SHA256(uname nodename) distinguishes same-hw/same-os VMs), UNIQUE as on
    // the fleet server (schema/fleet_v0_1.sql). p_cores / cpu_mhz_min / cpu_mhz_max are display-only.
    db.exec("CREATE TABLE IF NOT EXISTS host_spec (\n"
            "    group_id INTEGER PRIMARY KEY NOT NULL\n"
            "  , cpu TEXT NOT NULL DEFAULT ''\n"
            "  , cores INTEGER NOT NULL DEFAULT 0\n"
            "  , p_cores INTEGER NOT NULL DEFAULT 0\n"
            "  , ram_gb INTEGER NOT NULL DEFAULT 0\n"
            "  , cpu_mhz_min INTEGER NOT NULL DEFAULT 0\n"
            "  , cpu_mhz_max INTEGER NOT NULL DEFAULT 0\n"
            "  , disk TEXT NOT NULL DEFAULT ''\n"
            "  , os TEXT NOT NULL DEFAULT ''\n"
            "  , instance_hash TEXT NOT NULL DEFAULT ''\n"
            "  , UNIQUE(cpu, cores, ram_gb, os, disk, instance_hash)\n"
            ")");
    // current_host = per JAC313_VERSION pin (group_id) for THIS checkout. v001 and v002 on the same
    // machine share host_spec (one jac313-###) but each tree keeps its own pin row so neither
    // overwrites the other's committed pin in results.db. store_bench READS its version's row.
    db.exec("CREATE TABLE IF NOT EXISTS current_host (\n"
            "    version TEXT PRIMARY KEY NOT NULL\n"
            "  , group_id INTEGER NOT NULL DEFAULT 0\n"
            ")");
    // fleet_sync = the local group_ids the shared (pg-remote) server has confirmed, and which server
    // (password-free conninfo). A confirmed number needs no round trip on later runs; one taken
    // offline (no row here) is checked with the server, and renumbered to its number, next time.
    db.exec("CREATE TABLE IF NOT EXISTS fleet_sync (\n"
            "    group_id INTEGER PRIMARY KEY NOT NULL\n"
            "  , server TEXT NOT NULL DEFAULT ''\n"
            "  , ts_utc TEXT NOT NULL DEFAULT ''\n"
            ")");
    // testRun = one fact per test. status: pass | fail | skip | error (ctest/smoke/verify), 'built' /
    // 'NA' (build), 'pass' (bench: a row exists only on success). The read-back columns are 0 / ''
    // for tests that persist nothing.
    db.exec("CREATE TABLE IF NOT EXISTS testRun (\n"
            "    id INTEGER PRIMARY KEY NOT NULL\n"
            "  , run_id INTEGER NOT NULL DEFAULT 0\n"
            "  , test_type_id INTEGER NOT NULL DEFAULT 0\n"
            "  , test_list_id INTEGER NOT NULL DEFAULT 0\n"
            "  , parameter_id INTEGER NOT NULL DEFAULT 0\n"
            "  , status TEXT NOT NULL DEFAULT ''\n"
            "  , duration_ms INTEGER NOT NULL DEFAULT 0\n"
            "  , median_ops INTEGER NOT NULL DEFAULT 0\n"
            "  , low_ops INTEGER NOT NULL DEFAULT 0\n"
            "  , high_ops INTEGER NOT NULL DEFAULT 0\n"
            "  , avg_ops INTEGER NOT NULL DEFAULT 0\n"
            "  , stddev_ops INTEGER NOT NULL DEFAULT 0\n"
            "  , bytes INTEGER NOT NULL DEFAULT 0\n"
            "  , max_persist_rows INTEGER NOT NULL DEFAULT 0\n"   // the cap the run used (0 = none)
            "  , rows_persisted INTEGER NOT NULL DEFAULT 0\n"     // rows the dataset should hold (min(events, cap))
            "  , readback_mode TEXT NOT NULL DEFAULT ''\n"        // off | count | sample | all
            "  , readback_status TEXT NOT NULL DEFAULT ''\n"      // PASS | FAIL
            "  , rows_checked INTEGER NOT NULL DEFAULT 0\n"       // rows compared field by field
            "  , read_ms REAL NOT NULL DEFAULT 0\n"               // read-back wall time
            "  , pg_connect_ms REAL NOT NULL DEFAULT 0\n"         // PostgreSQL only: connection time
            "  , pg_ping_ms REAL NOT NULL DEFAULT 0\n"            //   median SELECT 1 round trip (latency)
            "  , pg_write_ms REAL NOT NULL DEFAULT 0\n"           //   total COPY write time
            // The production bench run (0 for every other row): hot-path ops/sec (producers until they
            // join, before finalize) and each sink's drain time (attach -> its finalize done), medians.
            "  , hot_ops INTEGER NOT NULL DEFAULT 0\n"
            "  , drain_keeper_ms INTEGER NOT NULL DEFAULT 0\n"
            "  , drain_database_ms INTEGER NOT NULL DEFAULT 0\n"
            "  , drain_binary_ms INTEGER NOT NULL DEFAULT 0\n"
            // The persistence batch each sink used (rows per sink call; 0 = no such sink). A one-sink
            // run fills the one that applies: keeper = a file (jtext | html | json), database = sql |
            // pg-local | pg-remote, binary = the binary log.
            "  , batch_keeper INTEGER NOT NULL DEFAULT 0\n"
            "  , batch_database INTEGER NOT NULL DEFAULT 0\n"
            "  , batch_binary INTEGER NOT NULL DEFAULT 0\n"
            ")");
    // run_target = what a run wrote where, many rows per run: one per table or file a sink wrote,
    // plus a 'registry' row when the run's process reached the fleet registry server.
    //   role:     persist (a one-sink run) | keeper | database | binary (the production run's sinks) | registry
    //   backend:  sql | pg-local | pg-remote | jtext | html | json | binary
    //   server:   a PostgreSQL server: its connections-file id ("#2") when the server came from that
    //             file, else its host name; '' for a file or SQLite
    //   address / port: the address and port connected to ('' / 0 for a file or SQLite)
    //   dbname:   the database (SQLite: the .db path); '' for a file sink
    //   table_name: the table (PostgreSQL: schema-qualified), or the file path for a file sink
    db.exec("CREATE TABLE IF NOT EXISTS run_target (\n"
            "    id INTEGER PRIMARY KEY NOT NULL\n"
            "  , run_id INTEGER NOT NULL DEFAULT 0\n"
            "  , role TEXT NOT NULL DEFAULT ''\n"
            "  , backend TEXT NOT NULL DEFAULT ''\n"
            "  , server TEXT NOT NULL DEFAULT ''\n"
            "  , address TEXT NOT NULL DEFAULT ''\n"
            "  , port INTEGER NOT NULL DEFAULT 0\n"
            "  , dbname TEXT NOT NULL DEFAULT ''\n"
            "  , table_name TEXT NOT NULL DEFAULT ''\n"
            ")");
    // runEvent: live status and progress log for tracking runs in real time (e.g. in DataGrip).
    // Inserted during execution so you can watch started/ended/error events while the CLI is running.
    db.exec("CREATE TABLE IF NOT EXISTS runEvent (\n"
            "    id INTEGER PRIMARY KEY NOT NULL\n"
            "  , run_id INTEGER NOT NULL DEFAULT 0\n"
            "  , ts_utc TEXT NOT NULL DEFAULT ''\n"
            "  , test_name TEXT NOT NULL DEFAULT ''\n"
            "  , event TEXT NOT NULL DEFAULT ''\n"
            "  , message TEXT NOT NULL DEFAULT ''\n"
            "  , duration_ms INTEGER NOT NULL DEFAULT 0\n"
            ")");
    // safeness = per (machine x version x gate x compiler) pass/fail summary, recomputed from testRun
    // at record time by rebuild_safeness() so the report + run-everything verdict READ it (never recount).
    db.exec("CREATE TABLE IF NOT EXISTS safeness (\n"
            "    group_id INTEGER NOT NULL DEFAULT 0\n"
            "  , version TEXT NOT NULL DEFAULT ''\n"  // JAC313_VERSION for isolation (v001 vs v002)
            "  , gate TEXT NOT NULL DEFAULT ''\n"
            "  , compiler TEXT NOT NULL DEFAULT ''\n"
            "  , pass INTEGER NOT NULL DEFAULT 0\n"
            "  , fail INTEGER NOT NULL DEFAULT 0\n"
            "  , PRIMARY KEY(group_id, version, gate, compiler)\n"
            ")");
    // io_best_fit = the per-machine durable persistence batch calibration for each backend. The peak of a
    // batch sweep is noisy, so we record the PLATEAU: low_size/high_size are the batch sizes at the
    // bottom/top of the near-peak band, and useThis is a robust pick in the MIDDLE of that band (what
    // the recorded bench actually runs at). best_size/best_ops are the raw peak (batch + median ops/sec)
    // kept for reference. One row per (group_id, type).
    db.exec("CREATE TABLE IF NOT EXISTS io_best_fit (\n"
            "    id INTEGER PRIMARY KEY NOT NULL\n"
            "  , group_id INTEGER NOT NULL DEFAULT 0\n"
            "  , type TEXT NOT NULL DEFAULT ''\n"
            "  , low_size INTEGER NOT NULL DEFAULT 0\n"
            "  , high_size INTEGER NOT NULL DEFAULT 0\n"
            "  , best_size INTEGER NOT NULL DEFAULT 0\n"
            "  , best_ops INTEGER NOT NULL DEFAULT 0\n"
            "  , useThis INTEGER NOT NULL DEFAULT 0\n"
            "  , UNIQUE(group_id, type)\n"
            ")");
    // package_version: the code version() of each package (Store/Qlite/jText) for THIS machine (group)
    // and world, sourced from the version() literals at host-pin time. Version-scoped like run/current_host
    // because v001 and v002 share a hardware group_id — without the world key their rows would collide
    // (last pin wins). Columns: group_id (machine), version (JAC313_VERSION world 'v001'|'v002', matches
    // run.version), package ('Store'|'Qlite'|'jText'), pkg_version (the code version() literal, e.g.
    // 'v002.008'). One row per (group_id, version, package) so any tool reads current versions straight
    // from the DB — store_bench included, which can't reach jText's header from its TU — without a run row.
    db.exec("CREATE TABLE IF NOT EXISTS package_version (\n"
            "    group_id INTEGER NOT NULL DEFAULT 0\n"
            "  , version TEXT NOT NULL DEFAULT ''\n"
            "  , package TEXT NOT NULL DEFAULT ''\n"
            "  , pkg_version TEXT NOT NULL DEFAULT ''\n"
            "  , UNIQUE(group_id, version, package)\n"
            ")");
    // testControl: per-test runtime controls (timeouts, memory limits, etc.) that drive the test harness.
    // Values live here in the DB (not hardcoded in source or flat files at runtime).
    // The human-editable tests/test_limits.jtext seeds/refreshes this table (jac313_test_cli run).
    db.exec("CREATE TABLE IF NOT EXISTS testControl (\n"
            "    name TEXT PRIMARY KEY NOT NULL\n"
            "  , timeout_sec INTEGER NOT NULL DEFAULT 300\n"
            "  , memory_mb INTEGER NOT NULL DEFAULT 0\n"
            "  , description TEXT NOT NULL DEFAULT ''\n"
            ")");
    db.exec("CREATE INDEX IF NOT EXISTS ix_testRun_run ON testRun(run_id)");
    db.exec("CREATE INDEX IF NOT EXISTS ix_testRun_parameter ON testRun(parameter_id)");
    db.exec("CREATE INDEX IF NOT EXISTS ix_runEvent_run ON runEvent(run_id)");
    db.exec("CREATE INDEX IF NOT EXISTS ix_run_target_run ON run_target(run_id)");
    db.exec("CREATE INDEX IF NOT EXISTS ix_run_group_version ON run(group_id, version)");
}

inline std::int64_t results_user_version(Sqlite& db) {
    std::int64_t v = 0;
    auto st = db.prepare("PRAGMA user_version");
    if (st.step()) st.get(v);
    return v;
}
} // namespace detail

// Open-time schema check (see the version rule above). The common case — the DB already carries
// kResultsSchemaVersion — only runs the idempotent CREATE ... IF NOT EXISTS statements.
inline void ensure_schema(Sqlite& db) {
    if (detail::results_user_version(db) != kResultsSchemaVersion) {
        // Decide and rebuild under a write lock, so a sibling process creating the same fresh DB
        // at the same moment is seen (re-read inside the lock) and not dropped again.
        db.exec("BEGIN IMMEDIATE");
        try {
            const std::int64_t have = detail::results_user_version(db);
            if (have != kResultsSchemaVersion) {
                std::vector<std::string> tables;
                { auto st = db.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%'");
                  while (st.step()) { std::string t; st.get(t); tables.push_back(t); } }
                if (!tables.empty()) {
                    std::fprintf(stderr, "results.db schema %lld is not %d: rebuilding (test data only)\n",
                                 static_cast<long long>(have), kResultsSchemaVersion);
                    for (const auto& t : tables) db.exec("DROP TABLE IF EXISTS \"" + t + "\"");
                }
                detail::create_results_tables(db);
                db.exec("PRAGMA user_version = " + std::to_string(kResultsSchemaVersion));
            }
            db.exec("COMMIT");
        } catch (...) {
            try { db.exec("ROLLBACK"); } catch (...) { /* keep the first error */ }
            throw;
        }
    } else {
        detail::create_results_tables(db);
    }
    db.exec("INSERT OR IGNORE INTO testType(name, description) VALUES\n"
            "    ('ctest', 'ctest unit suite')\n"
            "  , ('smoke', 'persist x output smoke matrix')\n"
            "  , ('bench', 'throughput benchmark suite')\n"
            "  , ('verify-lite', 'valgrind memcheck gate')\n"
            "  , ('verify', 'valgrind memcheck + helgrind + DRD')\n"
            "  , ('build', 'tree compile wall-clock')\n"
            "");
}

// single-id lookup (e.g. "SELECT id FROM testList WHERE name=?")
inline std::int64_t lookup_id(Sqlite& db, const char* sql, const std::string& name) {
    std::int64_t id = 0; auto st = db.prepare(sql); st.bind(name); if (st.step()) st.get(id); return id;
}

// Recompute the safeness summary from testRun. Called at the END of every recorder so the report +
// run-everything verdict just READ the table (never recount). Keyed on JAC313_VERSION too, so v001
// and v002 data stay isolated even for the same group/hardware. One transaction: its own, or the
// recorder's when the caller already opened one (then the caller commits).
inline void rebuild_safeness(Sqlite& db) {
    std::optional<Sqlite::Transaction> tx;
    if (sqlite3_get_autocommit(db.handle()) != 0) tx.emplace(db.transaction());
    db.exec("DELETE FROM safeness");
    // Only 'fail'/'error' count as failures; pass/skip (and bench rows, always 'pass') count as passes.
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
            "  , r.version\n"
            "  , tt.name\n"
            "  , c.name || c.major\n"
            "  , SUM(CASE WHEN tr.status IN ('fail', 'error') THEN 0 ELSE 1 END)\n"
            "  , SUM(CASE WHEN tr.status IN ('fail', 'error') THEN 1 ELSE 0 END)\n"
            "FROM testRun tr\n"
            "INNER JOIN run r ON r.run_id = tr.run_id\n"
            "INNER JOIN testType tt ON tt.id = tr.test_type_id\n"
            "INNER JOIN parameter p ON p.id = tr.parameter_id\n"
            "INNER JOIN compiler c ON c.id = p.compiler_id\n"
            "WHERE tt.name IN ('ctest', 'smoke', 'bench')\n"
            "GROUP BY r.group_id, r.version, tt.name, c.name || c.major");
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
            "  , r.version\n"
            "  , p.valgrind_tool\n"
            "  , c.name || c.major\n"
            "  , SUM(CASE WHEN tr.status IN ('fail', 'error') THEN 0 ELSE 1 END)\n"
            "  , SUM(CASE WHEN tr.status IN ('fail', 'error') THEN 1 ELSE 0 END)\n"
            "FROM testRun tr\n"
            "INNER JOIN run r ON r.run_id = tr.run_id\n"
            "INNER JOIN testType tt ON tt.id = tr.test_type_id\n"
            "INNER JOIN parameter p ON p.id = tr.parameter_id\n"
            "INNER JOIN compiler c ON c.id = p.compiler_id\n"
            "WHERE tt.name IN ('verify', 'verify-lite') AND p.valgrind_tool <> ''\n"
            "GROUP BY r.group_id, r.version, p.valgrind_tool, c.name || c.major");
    if (tx) tx->commit();
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
// Identity (group_id key) = cpu, cores, ram_gb, os, disk, instance_hash. The trailing p_cores /
// cpu_mhz_min / cpu_mhz_max are DISPLAY-ONLY (machines table) — carried here for convenience,
// NOT part of the key.
struct HostId { std::string cpu; std::int64_t cores = 0; std::int64_t ram_gb = 0; std::string os; std::string disk;
                std::string instance_hash;
                std::int64_t p_cores = 0; std::int64_t cpu_mhz_min = 0; std::int64_t cpu_mhz_max = 0; };

inline std::string host_label(std::int64_t group_id) {
    // 32: "jac313-" + up to 20 characters of a long long + NUL (24 could cut a big id short)
    char b[32]; std::snprintf(b, sizeof b, "jac313-%03lld", static_cast<long long>(group_id)); return b;
}

inline std::string hash_prefix(const std::string& hash, std::size_t n = 8) {
    return hash.size() <= n ? hash : hash.substr(0, n);
}

inline std::int64_t parse_group_label(const std::string& label) {
    std::string s = label;
    if (s.rfind("jac313-", 0) == 0) {
        s = s.substr(7);
    }
    if (s.empty()) {
        throw std::invalid_argument("empty group label");
    }
    for (char c : s) {
        if (c < '0' || c > '9') {
            throw std::invalid_argument("group label must be numeric: " + label);
        }
    }
    return std::stoll(s);
}

inline bool host_identity_matches(const HostId& a, const HostId& b) {
    return a.cpu == b.cpu && a.cores == b.cores && a.ram_gb == b.ram_gb
        && a.os == b.os && a.disk == b.disk && a.instance_hash == b.instance_hash;
}

inline bool same_os_hw(const HostId& a, const HostId& b) {
    return a.cpu == b.cpu && a.cores == b.cores && a.ram_gb == b.ram_gb
        && a.os == b.os && a.disk == b.disk;
}

inline bool load_host_spec(Sqlite& db, std::int64_t gid, HostId& out) {
    auto st = db.prepare("SELECT cpu, cores, ram_gb, os, disk, instance_hash, p_cores, cpu_mhz_min, cpu_mhz_max "
                         "FROM host_spec WHERE group_id=? LIMIT 1");
    st.bind(gid);
    if (!st.step()) return false;
    st.get(out.cpu, out.cores, out.ram_gb, out.os, out.disk, out.instance_hash,
           out.p_cores, out.cpu_mhz_min, out.cpu_mhz_max);
    return true;
}

inline std::int64_t find_exact_group(Sqlite& db, const HostId& h) {
    std::int64_t gid = 0;
    auto st = db.prepare("SELECT group_id FROM host_spec WHERE cpu=? AND cores=? AND ram_gb=? AND os=? AND disk=? "
                         "AND instance_hash=? LIMIT 1");
    st.bind(h.cpu, h.cores, h.ram_gb, h.os, h.disk, h.instance_hash);
    if (st.step()) st.get(gid);
    return gid;
}

inline std::vector<std::int64_t> find_same_os_hw_candidates(Sqlite& db, const HostId& h) {
    std::vector<std::int64_t> out;
    auto st = db.prepare("SELECT group_id FROM host_spec WHERE cpu=? AND cores=? AND ram_gb=? AND os=? AND disk=? "
                         "AND instance_hash<>? ORDER BY group_id");
    st.bind(h.cpu, h.cores, h.ram_gb, h.os, h.disk, h.instance_hash);
    while (st.step()) {
        std::int64_t gid = 0;
        st.get(gid);
        out.push_back(gid);
    }
    return out;
}

inline std::int64_t next_auto_group_id(Sqlite& db) {
    std::int64_t gid = 1;
    auto st = db.prepare("SELECT COALESCE(MAX(group_id),0)+1 FROM host_spec");
    if (st.step()) st.get(gid);
    return gid;
}

inline bool group_id_taken(Sqlite& db, std::int64_t gid) {
    std::int64_t n = 0;
    auto st = db.prepare("SELECT COUNT(*) FROM host_spec WHERE group_id=?");
    st.bind(gid);
    if (st.step()) st.get(n);
    return n > 0;
}

// File machine `h` under group `gid`. host_spec is unique on both group_id and the identity, so an
// identity already filed under ANOTHER group (--assign-new-### / --claim for a known machine) moves
// to gid: gid's old row (if any) gives way, then the identity row is renumbered. Other tables keep
// their group_id (renumber_group() moves those).
inline void upsert_host_spec(Sqlite& db, std::int64_t gid, const HostId& h) {
    if (const std::int64_t old = find_exact_group(db, h); old != 0 && old != gid) {
        db.exec("DELETE FROM host_spec WHERE group_id=?", gid);
        db.exec("UPDATE host_spec SET group_id=? WHERE group_id=?", gid, old);
    }
    db.exec("INSERT INTO host_spec(group_id, cpu, cores, p_cores, ram_gb, cpu_mhz_min, cpu_mhz_max, "
            "disk, os, instance_hash) VALUES(?,?,?,?,?,?,?,?,?,?) "
            "ON CONFLICT(group_id) DO UPDATE SET cpu=excluded.cpu, cores=excluded.cores, "
            "p_cores=excluded.p_cores, ram_gb=excluded.ram_gb, cpu_mhz_min=excluded.cpu_mhz_min, "
            "cpu_mhz_max=excluded.cpu_mhz_max, disk=excluded.disk, os=excluded.os, "
            "instance_hash=excluded.instance_hash",
            gid, h.cpu, h.cores, h.p_cores, h.ram_gb, h.cpu_mhz_min, h.cpu_mhz_max, h.disk, h.os, h.instance_hash);
}

inline void set_current_host(Sqlite& db, std::int64_t gid, const std::string& version) {
    db.exec("INSERT OR REPLACE INTO current_host(version, group_id) VALUES(?, ?)", version, gid);
}

inline std::int64_t current_host(Sqlite& db, const std::string& version) {
    std::int64_t gid = 0;
    { auto st = db.prepare("SELECT group_id FROM current_host WHERE version=?");
      st.bind(version); if (st.step()) st.get(gid); }
    return gid;
}

inline bool current_host_is_stale(Sqlite& db, const HostId& h, const std::string& version) {
    const std::int64_t pinned = current_host(db, version);
    if (pinned == 0) return false;
    HostId stored{};
    if (!load_host_spec(db, pinned, stored)) return true;
    return !host_identity_matches(h, stored);
}

enum class HostPinError {
    None, Ambiguous, InvalidLabel, ClaimNotFound, ClaimOsMismatch, ClaimHwMismatch,
    ClaimHashConflict, AssignTaken, ConflictingFlags,
};

struct HostPinOptions {
    std::string claim;
    std::int64_t assign_new = 0;
};

struct HostPinResult {
    HostPinError error = HostPinError::None;
    std::int64_t gid = 0;
    std::string message;
    std::vector<std::int64_t> ambiguous_candidates;
    bool generic_nodename = false;
};

inline HostPinResult propose_group_id(Sqlite& db, const HostId& h, bool generic_nodename) {
    HostPinResult r;
    r.generic_nodename = generic_nodename;
    if (const std::int64_t exact = find_exact_group(db, h); exact != 0) {
        r.gid = exact;
        return r;
    }
    r.ambiguous_candidates = find_same_os_hw_candidates(db, h);
    if (!r.ambiguous_candidates.empty() || generic_nodename) {
        r.error = HostPinError::Ambiguous;
        r.message = generic_nodename
            ? "generic nodename — use --claim jac313-### or --assign-new-###"
            : "same hardware+OS, different instance_hash — use --claim jac313-### or --assign-new-###";
        return r;
    }
    r.gid = next_auto_group_id(db);
    return r;
}

inline HostPinResult claim_group(Sqlite& db, const HostId& h, const std::string& label) {
    HostPinResult r;
    std::int64_t gid = 0;
    try {
        gid = parse_group_label(label);
    } catch (const std::exception& e) {
        r.error = HostPinError::InvalidLabel;
        r.message = e.what();
        return r;
    }
    HostId stored{};
    if (!load_host_spec(db, gid, stored)) {
        r.error = HostPinError::ClaimNotFound;
        r.message = "no host_spec row for " + host_label(gid);
        return r;
    }
    if (stored.os != h.os) {
        r.error = HostPinError::ClaimOsMismatch;
        r.message = host_label(gid) + " is " + stored.os + ", this machine is " + h.os;
        return r;
    }
    if (!same_os_hw(h, stored)) {
        r.error = HostPinError::ClaimHwMismatch;
        r.message = host_label(gid) + " hardware does not match this machine";
        return r;
    }
    if (!stored.instance_hash.empty() && stored.instance_hash != h.instance_hash) {
        r.error = HostPinError::ClaimHashConflict;
        r.message = host_label(gid) + " is bound to instance_hash " + hash_prefix(stored.instance_hash)
                  + " (this machine is " + hash_prefix(h.instance_hash) + ")";
        return r;
    }
    r.gid = gid;
    return r;
}

inline HostPinResult assign_new_group(Sqlite& db, const HostId& /*h*/, std::int64_t gid) {
    HostPinResult r;
    if (gid <= 0) {
        r.error = HostPinError::InvalidLabel;
        r.message = "assign-new requires a positive group number";
        return r;
    }
    if (group_id_taken(db, gid)) {
        r.error = HostPinError::AssignTaken;
        r.message = host_label(gid) + " is already taken";
        return r;
    }
    r.gid = gid;
    return r;
}

inline HostPinResult resolve_group_id(Sqlite& db, const HostId& h, const HostPinOptions& opts,
                                     bool generic_nodename) {
    HostPinResult r;
    if (!opts.claim.empty() && opts.assign_new > 0) {
        r.error = HostPinError::ConflictingFlags;
        r.message = "use only one of --claim or --assign-new-###";
        return r;
    }
    if (!opts.claim.empty()) {
        return claim_group(db, h, opts.claim);
    }
    if (opts.assign_new > 0) {
        return assign_new_group(db, h, opts.assign_new);
    }
    return propose_group_id(db, h, generic_nodename);
}

inline HostPinResult resolve_and_pin(Sqlite& db, const HostId& h, const HostPinOptions& opts,
                                     bool generic_nodename, const std::string& version) {
    HostPinResult r = resolve_group_id(db, h, opts, generic_nodename);
    if (r.error != HostPinError::None) {
        return r;
    }
    upsert_host_spec(db, r.gid, h);
    set_current_host(db, r.gid, version);
    return r;
}

inline std::int64_t pin_host(Sqlite& db, const HostId& h, const std::string& version) {
    const HostPinResult r = resolve_and_pin(db, h, {}, false, version);
    if (r.error != HostPinError::None) {
        throw std::runtime_error(r.message.empty() ? "host pin failed" : r.message);
    }
    return r.gid;
}

inline std::int64_t group_id(Sqlite& db, const HostId& h) {
    return find_exact_group(db, h);
}

// ---- fleet numbering (the shared server's jac313-###) ----
// Every local table that holds a group_id. renumber_group() moves all of them together.
inline constexpr const char* kGroupTables[] = {
    "host_spec", "current_host", "run", "safeness", "io_best_fit", "package_version", "fleet_sync"};

// True when any local table already uses this group_id.
inline bool group_in_use(Sqlite& db, std::int64_t gid) {
    for (const char* t : kGroupTables) {
        std::int64_t n = 0;
        auto st = db.prepare(std::string("SELECT COUNT(*) FROM ") + t + " WHERE group_id=?");
        st.bind(gid);
        if (st.step()) st.get(n);
        if (n > 0) return true;
    }
    return false;
}

// One past the highest group_id in any local table.
inline std::int64_t next_free_group_id(Sqlite& db) {
    std::int64_t top = 0;
    for (const char* t : kGroupTables) {
        std::int64_t m = 0;
        auto st = db.prepare(std::string("SELECT COALESCE(MAX(group_id),0) FROM ") + t);
        if (st.step()) st.get(m);
        top = std::max(top, m);
    }
    return top + 1;
}

namespace detail {
inline void renumber_group_rows(Sqlite& db, std::int64_t from, std::int64_t to) {
    if (from == to) return;
    // `to` belongs to another machine's local rows (e.g. one that has not registered yet): move
    // those out of the way first; that machine is renumbered when it registers.
    if (group_in_use(db, to)) renumber_group_rows(db, to, next_free_group_id(db));
    for (const char* t : kGroupTables)
        db.exec(std::string("UPDATE ") + t + " SET group_id=? WHERE group_id=?", to, from);
    db.exec("UPDATE run SET host=? WHERE group_id=?", host_label(to), to);
}
} // namespace detail

// Move every local row of group `from` to group `to` (run.host relabelled too), in one transaction.
inline void renumber_group(Sqlite& db, std::int64_t from, std::int64_t to) {
    auto tx = db.transaction();
    detail::renumber_group_rows(db, from, to);
    tx.commit();
}

inline bool fleet_synced(Sqlite& db, std::int64_t gid, const std::string& server) {
    std::int64_t n = 0;
    auto st = db.prepare("SELECT COUNT(*) FROM fleet_sync WHERE group_id=? AND server=?");
    st.bind(gid, server);
    if (st.step()) st.get(n);
    return n > 0;
}

// Confirmed by this server within the last `hours` (ts_utc is utc_now_iso()). An older confirmation
// is checked again, so a server that was wiped or replaced makes the machine register again.
inline bool fleet_synced_within(Sqlite& db, std::int64_t gid, const std::string& server, int hours) {
    std::int64_t n = 0;
    auto st = db.prepare("SELECT COUNT(*) FROM fleet_sync WHERE group_id=? AND server=? "
                         "AND julianday(ts_utc) > julianday('now') - ? / 24.0");
    st.bind(gid, server, static_cast<std::int64_t>(hours));
    if (st.step()) st.get(n);
    return n > 0;
}

inline void mark_fleet_synced(Sqlite& db, std::int64_t gid, const std::string& server, const std::string& ts_utc) {
    db.exec("INSERT OR REPLACE INTO fleet_sync(group_id, server, ts_utc) VALUES(?, ?, ?)", gid, server, ts_utc);
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
        st.bind(gid, version); if (st.step()) st.get(prior_run);  // ONE bind() call: a 2nd clears the 1st
    }
    if (prior_run > 0) {
        db.exec("DELETE FROM testRun WHERE run_id=?", prior_run);
        db.exec("DELETE FROM runEvent WHERE run_id=?", prior_run);
        db.exec("DELETE FROM run_target WHERE run_id=?", prior_run);
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

// ---- run_target: what a run wrote where ----
// One table or file a run wrote, as a sink reported it (Store's IEventSink::targets(), printed by the
// test / bench worker as a TARGET line), or the fleet registry server the run's process reached.
struct RunTarget {
    std::string role;        // persist | keeper | database | binary | registry
    std::string backend;     // sql | pg-local | pg-remote | jtext | html | json | binary
    std::string host;        // the server's host name as connected ('' for a file / SQLite)
    std::string address;     // the address connected to
    std::int64_t port = 0;
    std::string dbname;
    std::string table;       // a table, or the file path for a file sink
};

// One "TARGET key=value ..." line (Store/tests/matrix/test_common.hpp print_targets; values may be
// "quoted"). False when the line is not one.
inline bool parse_target_line(const std::string& line, RunTarget& out) {
    if (line.rfind("TARGET ", 0) != 0) return false;
    out = RunTarget{};
    std::size_t i = 7;
    while (i < line.size()) {
        while (i < line.size() && line[i] == ' ') ++i;
        const auto eq = line.find('=', i);
        if (eq == std::string::npos) break;
        const std::string key = line.substr(i, eq - i);
        std::string val;
        std::size_t j = eq + 1;
        if (j < line.size() && line[j] == '"') {
            const auto close = line.find('"', j + 1);
            val = line.substr(j + 1, (close == std::string::npos ? line.size() : close) - j - 1);
            j = (close == std::string::npos) ? line.size() : close + 1;
        } else {
            const auto sp = line.find(' ', j);
            val = line.substr(j, (sp == std::string::npos ? line.size() : sp) - j);
            j = (sp == std::string::npos) ? line.size() : sp;
        }
        if (key == "role") out.role = val;
        else if (key == "backend") out.backend = val;
        else if (key == "host") out.host = val;
        else if (key == "address") out.address = val;
        else if (key == "port") out.port = std::atoll(val.c_str());
        else if (key == "dbname") out.dbname = val;
        else if (key == "table") out.table = val;
        i = j;
    }
    return !out.backend.empty();
}

// Every TARGET line in a process's output.
inline std::vector<RunTarget> parse_target_lines(const std::string& text) {
    std::vector<RunTarget> v;
    for (std::size_t at = 0; at < text.size();) {
        const auto nl = text.find('\n', at);
        const std::string line = text.substr(at, nl == std::string::npos ? std::string::npos : nl - at);
        at = nl == std::string::npos ? text.size() : nl + 1;
        RunTarget t;
        if (parse_target_line(line, t)) v.push_back(std::move(t));
    }
    return v;
}

// The server name recorded for a PostgreSQL target: "#<id>" when $JAC313_PG_REMOTE_ID /
// $JAC313_PG_LOCAL_ID says the server came from that row of the connections file (jac313_test_cli
// sets it with the conninfo), else the host name.
inline std::string pg_server_name(const std::string& backend, const std::string& host) {
    const char* id = std::getenv(backend == "pg-local" ? "JAC313_PG_LOCAL_ID" : "JAC313_PG_REMOTE_ID");
    return (id && *id) ? "#" + std::string(id) : host;
}

// Record what run `run_id` wrote where: one run_target row per distinct target (a target already
// recorded for this run is not repeated). PostgreSQL and registry targets get their server name
// (pg_server_name); files and SQLite have none. The one writer for the CLI and the bench.
inline void insert_run_targets(Sqlite& db, std::int64_t run_id, const std::vector<RunTarget>& targets) {
    for (const auto& t : targets) {
        const bool server = t.backend == "pg-local" || t.backend == "pg-remote";
        const std::string name = server ? pg_server_name(t.backend, t.host) : std::string();
        db.exec("INSERT INTO run_target(run_id, role, backend, server, address, port, dbname, table_name) "
                "SELECT ?,?,?,?,?,?,?,? WHERE NOT EXISTS (SELECT 1 FROM run_target WHERE run_id=? AND role=? "
                "AND backend=? AND server=? AND address=? AND port=? AND dbname=? AND table_name=?)",
                run_id, t.role, t.backend, name, t.address, t.port, t.dbname, t.table,
                run_id, t.role, t.backend, name, t.address, t.port, t.dbname, t.table);
    }
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

// ---- package_version (per-machine code versions, sourced from the version() literals at pin time) ----
// Record (or refresh) one package's code version for this machine. Called at host-pin time so the DB
// always reflects what THIS checkout is running.
inline void upsert_package_version(Sqlite& db, std::int64_t gid, const std::string& version,
                                   const std::string& package, const std::string& pkg_version) {
    db.exec("INSERT INTO package_version(group_id, version, package, pkg_version) VALUES(?,?,?,?) "
            "ON CONFLICT(group_id, version, package) DO UPDATE SET pkg_version=excluded.pkg_version",
            gid, version, package, pkg_version);
}

// Read this machine+world's recorded version for a package; returns fallback if not yet pinned.
inline std::string package_version_get(Sqlite& db, std::int64_t gid, const std::string& version,
                                       const std::string& package, const std::string& fallback = "") {
    return get_top_one_string_or(db,
        "SELECT pkg_version FROM package_version WHERE group_id=? AND version=? AND package=? LIMIT 1",
        fallback, gid, version, package);
}

// ---- testControl: runtime controls per test name (timeouts etc.) stored in DB ----

// Upsert a control row for a test. Call this when loading tests/test_limits.jtext or from admin tools.
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

// Simple UTC timestamp for logs.
inline std::string utc_now_iso() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    gmtime_r(&t, &tm);
    std::ostringstream out;
    out << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return out.str();
}

// log_run_event: record live status events so you can query progress in DataGrip while the test runner is active.
// event examples: "started", "ended", "error"
inline void log_run_event(Sqlite& db, std::int64_t run_id, const std::string& test_name,
                          const std::string& event, const std::string& message = "",
                          std::int64_t duration_ms = 0) {
    const std::string ts = utc_now_iso();
    db.exec("INSERT INTO runEvent(run_id, ts_utc, test_name, event, message, duration_ms) "
            "VALUES(?,?,?,?,?,?)",
            run_id, ts, test_name, event, message, duration_ms);
}

} // namespace jac313::results
