// results.db schema: versioned (PRAGMA user_version), every column NOT NULL, the parameter UNIQUE
// really dedupes, ensure_schema is idempotent, and a DB of another schema version is rebuilt.

#include "jac313_results_db.hpp"

#include <jac313/Qlite/v001.hpp>

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <source_location>
#include <string>
#include <string_view>
#include <unistd.h>
#include <vector>

using Sqlite = jac313::Qlite::v001::Sqlite;
namespace fs = std::filesystem;
namespace R = jac313::results;

namespace {

int failures = 0;

// A failed check says what and where, and counts toward failures.
void check(bool ok, std::string_view what, std::source_location at = std::source_location::current()) {
    if (ok) return;
    std::cerr << "FAIL: " << what << " at " << at.file_name() << ":" << at.line() << "\n";
    ++failures;
}

std::int64_t one_int(Sqlite& db, const std::string& sql) {
    std::int64_t v = -1;
    auto st = db.prepare(sql);
    if (st.step()) st.get(v);
    return v;
}

std::vector<std::string> table_names(Sqlite& db) {
    std::vector<std::string> out;
    auto st = db.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' ORDER BY name");
    while (st.step()) { std::string n; st.get(n); out.push_back(n); }
    return out;
}

// A parameter row with every axis given ('' / 0 where the axis does not apply).
const char* kParamInsert =
    "INSERT OR IGNORE INTO parameter(compiler_id, build_type, modules, import_std, size, persist, output_mode, "
    "threads, events_per_thread, runs, batch, flag_count, valgrind_tool) "
    "VALUES(1, 'Debug', 'off', 'off', '', '', '', 0, 0, 0, 0, 0, '')";

} // namespace

int main() {
    const fs::path path = fs::temp_directory_path()
                        / ("jac313_results_schema_test_" + std::to_string(::getpid()) + ".db");
    fs::remove(path);

    {
        Sqlite db(path.string());
        R::ensure_schema(db);
        R::ensure_schema(db);   // idempotent: a second open changes nothing and does not throw
        check(one_int(db, "PRAGMA user_version") == R::kResultsSchemaVersion, "user_version == kResultsSchemaVersion");
        check(one_int(db, "SELECT COUNT(*) FROM testType") == 6, "testType seeded once (6 rows)");

        const auto tables = table_names(db);
        check(tables.size() == 15, "15 tables");
        for (const auto& t : tables) {
            const std::int64_t cols = one_int(db, "SELECT COUNT(*) FROM pragma_table_info('" + t + "')");
            const std::int64_t nn   = one_int(db, "SELECT COUNT(*) FROM pragma_table_info('" + t + "') WHERE \"notnull\"=1");
            check(cols > 0 && cols == nn, "every column NOT NULL in " + t);
        }

        // Schema 2: the production bench run's hot-path ops/sec and its three drain times.
        for (const char* col : {"hot_ops", "drain_keeper_ms", "drain_database_ms", "drain_binary_ms"})
            check(one_int(db, std::string("SELECT COUNT(*) FROM pragma_table_info('testRun') WHERE name='") + col + "'") == 1,
                  std::string("testRun has ") + col);

        // Schema 3: parameter.contracts, NOT NULL DEFAULT 'enforce', part of the UNIQUE key.
        check(R::kResultsSchemaVersion == 4, "kResultsSchemaVersion == 4");
        check(one_int(db, "SELECT COUNT(*) FROM pragma_table_info('parameter') WHERE name='contracts' "
                          "AND \"notnull\"=1 AND dflt_value='''enforce'''") == 1,
              "parameter.contracts NOT NULL DEFAULT 'enforce'");

        bool threw = false;
        try {
            db.exec("INSERT INTO parameter(compiler_id, size) VALUES(1, NULL)");
        } catch (const std::exception&) { threw = true; }
        check(threw, "explicit NULL into parameter.size throws");

        db.exec(kParamInsert);
        db.exec(kParamInsert);
        check(one_int(db, "SELECT COUNT(*) FROM parameter") == 1, "identical parameter rows dedupe to one");
        check(one_int(db, "SELECT COUNT(*) FROM parameter WHERE contracts='enforce'") == 1, "contracts defaults to enforce");
        db.exec("INSERT OR IGNORE INTO parameter(compiler_id, build_type, modules, import_std, size, persist, output_mode, "
                "threads, events_per_thread, runs, batch, flag_count, valgrind_tool, contracts) "
                "VALUES(1, 'Debug', 'off', 'off', '', '', '', 0, 0, 0, 0, 0, '', 'observe')");
        check(one_int(db, "SELECT COUNT(*) FROM parameter") == 2, "another contract mode is its own parameter row");

        // Schema 4: parameter.strict / hardened (NOT NULL DEFAULT 'off', in the UNIQUE key), the batch
        // each sink used on testRun, and run_target (what a run wrote where).
        for (const char* col : {"strict", "hardened"})
            check(one_int(db, std::string("SELECT COUNT(*) FROM pragma_table_info('parameter') WHERE name='") + col
                              + "' AND \"notnull\"=1 AND dflt_value='''off'''") == 1,
                  std::string("parameter.") + col + " NOT NULL DEFAULT 'off'");
        db.exec("INSERT OR IGNORE INTO parameter(compiler_id, build_type, modules, import_std, size, persist, output_mode, "
                "threads, events_per_thread, runs, batch, flag_count, valgrind_tool, contracts, strict, hardened) "
                "VALUES(1, 'Debug', 'off', 'off', '', '', '', 0, 0, 0, 0, 0, '', 'observe', 'on', 'off')");
        check(one_int(db, "SELECT COUNT(*) FROM parameter") == 3, "a strict build is its own parameter row");
        for (const char* col : {"batch_keeper", "batch_database", "batch_binary", "max_persist_rows"})
            check(one_int(db, std::string("SELECT COUNT(*) FROM pragma_table_info('testRun') WHERE name='") + col
                              + "' AND \"notnull\"=1 AND dflt_value='0'") == 1,
                  std::string("testRun.") + col + " NOT NULL DEFAULT 0");
        check(one_int(db, "SELECT COUNT(*) FROM pragma_table_info('run_target')") == 9, "run_target has 9 columns");
        check(one_int(db, "SELECT COUNT(*) FROM sqlite_master WHERE type='index' AND name='ix_run_target_run'") == 1,
              "run_target is indexed on run_id");

        // A TARGET line (Store's print_targets) parses; a PostgreSQL target is named by its
        // connections-file id when there is one, else by its host; a repeated target is one row.
        R::RunTarget t;
        check(R::parse_target_line("TARGET role=persist backend=sql host=\"\" address=\"\" port=0 "
                                   "dbname=\"/tmp/a b.db\" table=\"persist_ints\"", t)
              && t.role == "persist" && t.backend == "sql" && t.dbname == "/tmp/a b.db" && t.table == "persist_ints",
              "parse_target_line reads a sql TARGET line");
        check(!R::parse_target_line("READBACK backend=sql", t), "a READBACK line is not a target");
        const auto lines = R::parse_target_lines(
            "noise\nTARGET role=keeper backend=jtext host=\"\" address=\"\" port=0 dbname=\"\" table=\"k.jtext\"\n"
            "TARGET role=database backend=pg-remote host=\"db1\" address=\"10.0.0.5\" port=5432 dbname=\"jac313\" "
            "table=\"store_test.x\"\n");
        check(lines.size() == 2 && lines[1].port == 5432 && lines[1].address == "10.0.0.5", "parse_target_lines finds both");
        ::unsetenv("JAC313_PG_REMOTE_ID");
        R::insert_run_targets(db, 7, lines);
        R::insert_run_targets(db, 7, lines);
        check(one_int(db, "SELECT COUNT(*) FROM run_target WHERE run_id=7") == 2, "a repeated target is recorded once");
        check(one_int(db, "SELECT COUNT(*) FROM run_target WHERE backend='pg-remote' AND server='db1'") == 1,
              "a PostgreSQL server without a connections-file id is named by its host");
        check(one_int(db, "SELECT COUNT(*) FROM run_target WHERE backend='jtext' AND server=''") == 1,
              "a file has no server");
        ::setenv("JAC313_PG_REMOTE_ID", "2", 1);
        R::insert_run_targets(db, 8, {lines[1]});
        ::unsetenv("JAC313_PG_REMOTE_ID");
        check(one_int(db, "SELECT COUNT(*) FROM run_target WHERE run_id=8 AND server='#2'") == 1,
              "a server from the connections file is named by its id");

        // host_spec is unique on the identity too: --assign-new-### for a machine already filed
        // under another number moves its row instead of failing on (or duplicating) the identity.
        const R::HostId h{"cpu", 8, 32, "os", "ssd", "hash", 4, 1000, 2000};
        check(R::pin_host(db, h, "v001") == 1, "first pin is group 1");
        R::HostPinOptions opts;
        opts.assign_new = 5;
        const auto moved = R::resolve_and_pin(db, h, opts, false, "v001");
        check(moved.error == R::HostPinError::None && moved.gid == 5, "assign-new 5 succeeds");
        check(one_int(db, "SELECT COUNT(*) FROM host_spec") == 1, "one host_spec row after the move");
        check(one_int(db, "SELECT group_id FROM host_spec") == 5, "the row is now group 5");
    }

    // Another schema version (here 99) plus a stray table: rebuilt from scratch on open.
    {
        Sqlite db(path.string());
        db.exec("CREATE TABLE stray (x INTEGER NOT NULL DEFAULT 0)");
        db.exec("PRAGMA user_version = 99");
    }
    {
        Sqlite db(path.string());
        R::ensure_schema(db);
        check(one_int(db, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='stray'") == 0, "stray table dropped");
        check(one_int(db, "PRAGMA user_version") == R::kResultsSchemaVersion, "user_version back to kResultsSchemaVersion");
        check(one_int(db, "SELECT COUNT(*) FROM parameter") == 0, "rebuilt DB starts empty");
        check(table_names(db).size() == 15, "15 tables after rebuild");
    }

    fs::remove(path);
    if (failures) { std::cerr << failures << " check(s) failed\n"; return 1; }
    std::cout << "results_schema_test: OK\n";
    return 0;
}
