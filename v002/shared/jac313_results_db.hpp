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
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace jac313::results {

using Sqlite = jac313::Qlite::v002::Sqlite;

// Reusable scalar-query helpers live in Qlite (canonical, shared); surface them here for results code.
using jac313::Qlite::v002::get_one_long;
using jac313::Qlite::v002::get_one_string;
using jac313::Qlite::v002::get_one_double;

// Debug variants: get_one_xxx(db, sql, true, binds...) or the _debug shorthands
// When debug=true they print [SQL] ... and result to stderr.
using jac313::Qlite::v002::get_one_long_debug;
using jac313::Qlite::v002::get_one_string_debug;
using jac313::Qlite::v002::get_one_double_debug;
using jac313::Qlite::v002::get_one_long_or_debug;
using jac313::Qlite::v002::get_one_string_or_debug;

// Convenience helpers for testControl table (the central store for per-test limits)
inline int get_test_timeout(Sqlite& db, const std::string& name, bool debug = false) {
    return static_cast<int>(get_one_long(db, "SELECT timeout_sec FROM testControl WHERE name=?", debug, name));
}
inline int get_test_memory_mb(Sqlite& db, const std::string& name, bool debug = false) {
    return static_cast<int>(get_one_long(db, "SELECT memory_mb FROM testControl WHERE name=?", debug, name));
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
    // host_spec = the hardware identity, ONE row per machine (group_id). cpu/cores/ram_gb/os/disk/
    // instance_hash are the group_id key (disk in the key: x7k vs 10k vs ssd on one box -> distinct
    // groups; instance_hash = SHA256(uname nodename) distinguishes same-hw/same-os VMs). p_cores /
    // cpu_mhz_min / cpu_mhz_max are display-only (machines table).
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
            "  , instance_hash TEXT NOT NULL\n"
            ")");
    // current_host = per JAC313_VERSION pin (group_id) for THIS checkout. v001 and v002 on the same
    // machine share host_spec (one jac313-###) but each tree keeps its own pin row so neither
    // overwrites the other's committed pin in results.db. store_bench READS its version's row.
    db.exec("CREATE TABLE IF NOT EXISTS current_host (\n"
            "    version TEXT PRIMARY KEY\n"
            "  , group_id INTEGER NOT NULL\n"
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

    // runEvent: live status and progress log for tracking runs in real time (e.g. in DataGrip).
    // Inserted during execution so you can watch started/ended/error events while the CLI is running.
    db.exec("CREATE TABLE IF NOT EXISTS runEvent (\n"
            "    id INTEGER PRIMARY KEY\n"
            "  , run_id INTEGER\n"
            "  , ts_utc TEXT\n"
            "  , test_name TEXT\n"
            "  , event TEXT\n"
            "  , message TEXT\n"
            "  , duration_ms INTEGER\n"
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
    // Migration for the safeness version column (JAC313_VERSION for v001/v002 isolation).
    // A plain `ALTER TABLE ... ADD COLUMN version` CANNOT fold version into the primary key, so an
    // older DB keeps PK(group_id, gate, compiler). rebuild_safeness() writes rows keyed by
    // (group_id, version, gate, compiler): the moment one hardware group holds two versions (e.g. a
    // legacy version='' run alongside a new 'v002' run) those rows collide on the stale 3-col PK, the
    // INSERT aborts AFTER the table-wide DELETE already committed, and safeness is left EMPTY — every
    // gate then reads 0 and the run-everything verdict says NOT SAFE. So if version is not already
    // part of the PK, REBUILD the table with the correct 4-col PK (SQLite can't alter a PK in place).
    {
        std::int64_t version_in_pk = 0;
        { auto st = db.prepare("SELECT COUNT(*) FROM pragma_table_info('safeness') WHERE name='version' AND pk>0");
          if (st.step()) st.get(version_in_pk); }
        if (version_in_pk == 0) {
            std::int64_t has_col = 0;
            { auto st = db.prepare("SELECT COUNT(*) FROM pragma_table_info('safeness') WHERE name='version'");
              if (st.step()) st.get(has_col); }
            db.exec("CREATE TABLE safeness_new (\n"
                    "    group_id INTEGER\n"
                    "  , version TEXT\n"
                    "  , gate TEXT\n"
                    "  , compiler TEXT\n"
                    "  , pass INTEGER\n"
                    "  , fail INTEGER\n"
                    "  , PRIMARY KEY(group_id, version, gate, compiler)\n"
                    ")");
            // Copy whatever the old table had; COALESCE the version so the new PK never sees NULL.
            // INSERT OR IGNORE absorbs any collision the old rows may already carry.
            if (has_col > 0)
                db.exec("INSERT OR IGNORE INTO safeness_new(group_id, version, gate, compiler, pass, fail) "
                        "SELECT group_id, COALESCE(version, ''), gate, compiler, pass, fail FROM safeness");
            else
                db.exec("INSERT OR IGNORE INTO safeness_new(group_id, version, gate, compiler, pass, fail) "
                        "SELECT group_id, '', gate, compiler, pass, fail FROM safeness");
            db.exec("DROP TABLE safeness");
            db.exec("ALTER TABLE safeness_new RENAME TO safeness");
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
// Identity (group_id key) = cpu, cores, ram_gb, os, disk, instance_hash. The trailing p_cores /
// cpu_mhz_min / cpu_mhz_max are DISPLAY-ONLY (machines table) — carried here for convenience,
// NOT part of the key.
struct HostId { std::string cpu; std::int64_t cores = 0; std::int64_t ram_gb = 0; std::string os; std::string disk;
                std::string instance_hash;
                std::int64_t p_cores = 0; std::int64_t cpu_mhz_min = 0; std::int64_t cpu_mhz_max = 0; };

inline std::string host_label(std::int64_t group_id) {
    char b[24]; std::snprintf(b, sizeof b, "jac313-%03lld", static_cast<long long>(group_id)); return b;
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

inline void upsert_host_spec(Sqlite& db, std::int64_t gid, const HostId& h) {
    db.exec("INSERT OR REPLACE INTO host_spec(group_id, cpu, cores, p_cores, ram_gb, cpu_mhz_min, cpu_mhz_max, "
            "disk, os, instance_hash) VALUES(?,?,?,?,?,?,?,?,?,?)",
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

inline HostPinResult assign_new_group(Sqlite& db, const HostId& h, std::int64_t gid) {
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
