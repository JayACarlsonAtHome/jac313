// results.db schema: versioned (PRAGMA user_version), every column NOT NULL, the parameter UNIQUE
// really dedupes, ensure_schema is idempotent, and a DB of another schema version is rebuilt.

#include "jac313_results_db.hpp"

#include <jac313/Qlite/v002.hpp>

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <source_location>
#include <string>
#include <string_view>
#include <unistd.h>
#include <vector>

using Sqlite = jac313::Qlite::v002::Sqlite;
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
        check(tables.size() == 16, "16 tables");
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
        check(R::kResultsSchemaVersion == 5, "kResultsSchemaVersion == 5");
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

        // Schema 5: os and machine replace host_spec; run carries os_id + machine_id (NOT NULL, no
        // default: a run row always names both); the per-machine tables key on machine_id.
        check(one_int(db, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='host_spec'") == 0,
              "no host_spec table");
        check(one_int(db, "SELECT COUNT(*) FROM pragma_table_info('os')") == 6, "os has 6 columns");
        check(one_int(db, "SELECT COUNT(*) FROM pragma_table_info('machine')") == 9, "machine has 9 columns");
        check(one_int(db, "SELECT COUNT(*) FROM pragma_table_info('machine') WHERE name='os'") == 0,
              "machine has no OS column");
        for (const char* col : {"os_id", "machine_id"})
            check(one_int(db, std::string("SELECT COUNT(*) FROM pragma_table_info('run') WHERE name='") + col
                              + "' AND \"notnull\"=1 AND dflt_value IS NULL") == 1,
                  std::string("run.") + col + " NOT NULL without a default");
        check(one_int(db, "SELECT COUNT(*) FROM pragma_foreign_key_list('run') WHERE \"table\"='os' AND \"from\"='os_id'") == 1,
              "run.os_id references os");
        check(one_int(db, "SELECT COUNT(*) FROM pragma_foreign_key_list('run') WHERE \"table\"='machine' "
                          "AND \"from\"='machine_id'") == 1, "run.machine_id references machine");
        for (const char* col : {"host", "group_id"})
            check(one_int(db, std::string("SELECT COUNT(*) FROM pragma_table_info('run') WHERE name='") + col + "'") == 0,
                  std::string("run has no ") + col);
        for (const char* t : {"current_host", "safeness", "io_best_fit", "package_version", "fleet_sync"})
            check(one_int(db, std::string("SELECT COUNT(*) FROM pragma_table_info('") + t + "') WHERE name='machine_id'") == 1
                  && one_int(db, std::string("SELECT COUNT(*) FROM pragma_table_info('") + t + "') WHERE name='group_id'") == 0,
                  std::string(t) + " keys on machine_id");
        check(one_int(db, "SELECT COUNT(*) FROM pragma_table_info('safeness') WHERE name='os_id' AND pk > 0") == 1,
              "safeness.os_id is in its key");
        threw = false;
        try { db.exec("INSERT INTO run(run_id, machine_id) VALUES(900, 1)"); } catch (const std::exception&) { threw = true; }
        check(threw, "a run without os_id is refused");

        // The UNIQUE keys: an identical os row and an identical machine are refused.
        const R::OsInfo fedora{"fedora", "Fedora Linux 44", "44", "7.2.13", "x86_64"};
        const R::OsInfo rhel{"rhel", "Red Hat Enterprise Linux 10.0", "10.0", "6.12.0", "x86_64"};
        const std::int64_t os_fedora = R::os_id(db, fedora);
        check(R::os_id(db, fedora) == os_fedora && one_int(db, "SELECT COUNT(*) FROM os") == 1, "os is found, not duplicated");
        threw = false;
        try { db.exec("INSERT INTO os(family, name, version, kernel, arch) VALUES('fedora', 'Fedora Linux 44', '44', "
                      "'7.2.13', 'x86_64')"); } catch (const std::exception&) { threw = true; }
        check(threw, "os UNIQUE(family, name, version, kernel, arch)");
        threw = false;
        try { db.exec("INSERT INTO machine(id, cpu, cores, ram_gb, disk, instance_hash) VALUES(1, 'c', 1, 1, 'xssd', 'h'), "
                      "(2, 'c', 1, 1, 'xssd', 'h')"); } catch (const std::exception&) { threw = true; }
        check(threw, "machine UNIQUE(cpu, cores, ram_gb, disk, instance_hash)");
        check(one_int(db, "SELECT COUNT(*) FROM machine") == 0, "the refused insert left no machine");

        // A second OS for the same hardware does not create a second machine.
        const R::MachineId h{"cpu", 8, 32, "ssd", "hash", 4, 1000, 2000};
        check(R::pin_host(db, h, "v002") == 1, "first pin is machine 1");
        R::insert_run(db, 901, os_fedora, 1, "v002");
        const std::int64_t os_rhel = R::os_id(db, rhel);
        check(R::pin_host(db, h, "v002") == 1, "the same hardware under RHEL is still machine 1");
        R::insert_run(db, 902, os_rhel, 1, "v002");
        check(one_int(db, "SELECT COUNT(*) FROM machine") == 1, "one machine row for two OSes");
        check(one_int(db, "SELECT COUNT(*) FROM os") == 2, "two os rows");
        check(one_int(db, "SELECT COUNT(DISTINCT os_id) FROM run WHERE machine_id=1") == 2, "its runs name both OSes");

        // machine is unique on the identity too: --assign-new-### for a machine already filed
        // under another number moves its row instead of failing on (or duplicating) the identity.
        R::HostPinOptions opts;
        opts.assign_new = 5;
        const auto moved = R::resolve_and_pin(db, h, opts, false, "v002");
        check(moved.error == R::HostPinError::None && moved.mid == 5, "assign-new 5 succeeds");
        check(one_int(db, "SELECT COUNT(*) FROM machine") == 1, "one machine row after the move");
        check(one_int(db, "SELECT id FROM machine") == 5, "the row is now machine 5");
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
        check(table_names(db).size() == 16, "16 tables after rebuild");
    }

    fs::remove(path);
    if (failures) { std::cerr << failures << " check(s) failed\n"; return 1; }
    std::cout << "results_schema_test: OK\n";
    return 0;
}
