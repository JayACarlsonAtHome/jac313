// results.db schema: versioned (PRAGMA user_version), every column NOT NULL, the parameter UNIQUE
// really dedupes, ensure_schema is idempotent, a DB of an older schema version is rebuilt and a newer one refused.

#include "jac313_results_db.hpp"

#include <jac313/Qlite/v002.hpp>

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <source_location>
#include <stdexcept>
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
        check(tables.size() == 18, "18 tables");
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
        check(R::kResultsSchemaVersion == 7, "kResultsSchemaVersion == 7");
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
        // connections-file name when there is one, else by its host; a repeated target is one row.
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
        ::unsetenv("JAC313_PG_REMOTE_NAME");
        R::insert_run_targets(db, 7, lines);
        R::insert_run_targets(db, 7, lines);
        check(one_int(db, "SELECT COUNT(*) FROM run_target WHERE run_id=7") == 2, "a repeated target is recorded once");
        check(one_int(db, "SELECT COUNT(*) FROM run_target WHERE backend='pg-remote' AND server='db1'") == 1,
              "a PostgreSQL server without a connections-file name is named by its host");
        check(one_int(db, "SELECT COUNT(*) FROM run_target WHERE backend='jtext' AND server=''") == 1,
              "a file has no server");
        ::setenv("JAC313_PG_REMOTE_NAME", "r430", 1);
        R::insert_run_targets(db, 8, {lines[1]});
        ::unsetenv("JAC313_PG_REMOTE_NAME");
        check(one_int(db, "SELECT COUNT(*) FROM run_target WHERE run_id=8 AND server='r430'") == 1,
              "a server from the connections file is named by its name");

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
        for (const char* table : {"current_host", "safeness", "io_best_fit", "package_version", "fleet_sync"})
            check(one_int(db, std::string("SELECT COUNT(*) FROM pragma_table_info('") + table + "') WHERE name='machine_id'") == 1
                  && one_int(db, std::string("SELECT COUNT(*) FROM pragma_table_info('") + table + "') WHERE name='group_id'") == 0,
                  std::string(table) + " keys on machine_id");
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

        // Schema 7: run_result, one row per recorded run, standing on its own (pin, machine, OS, options,
        // commit, pass/fail and the counts).
        db.exec("INSERT INTO testRun(run_id, status) VALUES(901, 'pass'), (901, 'fail'), (901, 'skip'), (901, 'error'), "
                "(902, 'pass'), (902, 'built')");
        R::record_run_result(db, 901, "ctest", "abc123", 0);
        R::record_run_result(db, 902, "build", "abc123", 1);
        check(one_int(db, "SELECT COUNT(*) FROM run_result") == 2, "one run_result row per run");
        check(one_int(db, "SELECT pass_count FROM run_result WHERE run_id=901") == 1
              && one_int(db, "SELECT fail_count FROM run_result WHERE run_id=901") == 2
              && one_int(db, "SELECT skip_count FROM run_result WHERE run_id=901") == 1,
              "run_result counts: pass 1, fail + error 2, skip 1");
        check(one_int(db, "SELECT COUNT(*) FROM run_result WHERE run_id=901 AND result='fail'") == 1, "a failed row fails the run");
        check(one_int(db, "SELECT COUNT(*) FROM run_result WHERE run_id=902 AND result='pass' AND pass_count=2 "
                          "AND git_dirty=1 AND gate='build'") == 1, "built rows pass, git_dirty kept");
        check(one_int(db, "SELECT COUNT(*) FROM run_result WHERE run_id=901 AND pin='jac313-001' AND cpu='cpu' "
                          "AND cores=8 AND ram_gb=32 AND disk='ssd' AND instance_hash='hash' AND os_family='fedora' "
                          "AND os_version='44' AND kernel='7.2.13' AND git_commit='abc123' AND version=(SELECT version FROM run WHERE run_id=901)") == 1,
              "run_result copies the pin, what made it, the OS and the commit");
        db.exec("DELETE FROM testRun WHERE run_id=901 AND status IN ('fail', 'error')");
        R::record_run_result(db, 901, "ctest", "abc123", 0);
        check(one_int(db, "SELECT COUNT(*) FROM run_result WHERE run_id=901 AND result='pass' AND fail_count=0") == 1
              && one_int(db, "SELECT COUNT(*) FROM run_result") == 2, "recording again refreshes the row");
        const std::int64_t id901 = one_int(db, "SELECT id FROM run_result WHERE run_id=901");
        const std::int64_t change_id = R::add_run_change(db, 901, "raised Clang to 21 on Linux Mint");
        R::add_run_change(db, 901, "CMake 4.3.3");
        R::record_run_result(db, 901, "ctest", "abc123", 0);
        check(one_int(db, "SELECT id FROM run_result WHERE run_id=901") == id901, "recording again keeps the id");
        check(one_int(db, "SELECT COUNT(*) FROM run_change WHERE run_result_id=" + std::to_string(id901)) == 2
              && one_int(db, "SELECT COUNT(*) FROM run_change WHERE id=" + std::to_string(change_id)
                             + " AND change_text='raised Clang to 21 on Linux Mint' AND ts_utc<>''") == 1,
              "run_change rows belong to their run_result");
        check(one_int(db, "SELECT COUNT(*) FROM pragma_foreign_key_list('run_change') WHERE \"table\"='run_result' "
                          "AND \"from\"='run_result_id' AND \"to\"='id'") == 1, "run_change.run_result_id references run_result(id)");
        threw = false;
        try { R::add_run_change(db, 999, "x"); } catch (const std::exception&) { threw = true; }
        check(threw, "a change for a run with no run_result is refused");
        db.exec("INSERT INTO run(run_id, os_id, machine_id) VALUES(903, " + std::to_string(os_fedora) + ", 1)");
        R::record_run_result(db, 903, "ctest", "", 0);
        check(one_int(db, "SELECT COUNT(*) FROM run_result WHERE run_id=903 AND result='fail' AND pass_count=0") == 1,
              "a run with no rows does not pass");
        db.exec("DELETE FROM run WHERE run_id=903");

        // machine is unique on the identity too: --assign-new-### for a machine already filed
        // under another number moves its row instead of failing on (or duplicating) the identity.
        R::HostPinOptions opts;
        opts.assign_new = 5;
        const auto moved = R::resolve_and_pin(db, h, opts, false, "v002");
        check(moved.error == R::HostPinError::None && moved.mid == 5, "assign-new 5 succeeds");
        check(one_int(db, "SELECT COUNT(*) FROM machine") == 1, "one machine row after the move");
        check(one_int(db, "SELECT id FROM machine") == 5, "the row is now machine 5");

        // Schema 6: every pickable setting is recorded. run carries the run-wide settings, parameter
        // the per-configuration ones (in its UNIQUE key), testRun the limits each test ran under; all
        // NOT NULL with a default ('' / 0 = none).
        for (const char* col : {"command_line", "filter", "params_file", "selection", "keep_logs_dir", "pg_cleanup",
                                "disk_source"})
            check(one_int(db, std::string("SELECT COUNT(*) FROM pragma_table_info('run') WHERE name='") + col
                              + "' AND \"notnull\"=1 AND dflt_value=''''''") == 1,
                  std::string("run.") + col + " NOT NULL DEFAULT ''");
        for (const char* col : {"jobs", "fail_fast", "keep_logs", "failsafe_sec", "store_tests"})
            check(one_int(db, std::string("SELECT COUNT(*) FROM pragma_table_info('run') WHERE name='") + col
                              + "' AND \"notnull\"=1 AND dflt_value='0'") == 1,
                  std::string("run.") + col + " NOT NULL DEFAULT 0");
        for (const char* col : {"flags_raw", "keeper_pct", "database_pct", "binary_log", "max_runs", "max_records"})
            check(one_int(db, std::string("SELECT COUNT(*) FROM pragma_table_info('parameter') WHERE name='") + col
                              + "' AND \"notnull\"=1 AND dflt_value='0'") == 1,
                  std::string("parameter.") + col + " NOT NULL DEFAULT 0");
        for (const char* col : {"timeout_sec", "memory_mb"})
            check(one_int(db, std::string("SELECT COUNT(*) FROM pragma_table_info('testRun') WHERE name='") + col
                              + "' AND \"notnull\"=1 AND dflt_value='0'") == 1,
                  std::string("testRun.") + col + " NOT NULL DEFAULT 0");
        check(one_int(db, "SELECT COUNT(*) FROM pragma_table_info('current_host') WHERE name='disk_source' "
                          "AND \"notnull\"=1") == 1, "current_host.disk_source NOT NULL");

        // insert_run writes the settings given; parameter_id finds-or-inserts over the whole key, so a
        // configuration that differs only in a schema-6 axis is its own row.
        R::RunSettings rs;
        rs.command_line = "jac313_test_cli matrix run --filter 001";
        rs.filter = "001"; rs.params_file = "tests/test_params.jtext"; rs.selection = "store,001";
        rs.jobs = 4; rs.fail_fast = 1; rs.keep_logs = 1; rs.keep_logs_dir = "/tmp/k"; rs.failsafe_sec = 60;
        rs.pg_cleanup = "delete"; rs.store_tests = 1; rs.disk_source = "detected";
        R::insert_run(db, 903, os_fedora, 5, "v002", "", "", "", rs);
        check(one_int(db, "SELECT COUNT(*) FROM run WHERE run_id=903 AND filter='001' AND jobs=4 AND fail_fast=1 "
                          "AND keep_logs=1 AND keep_logs_dir='/tmp/k' AND failsafe_sec=60 AND pg_cleanup='delete' "
                          "AND store_tests=1 AND disk_source='detected' AND selection='store,001' "
                          "AND params_file='tests/test_params.jtext' AND command_line LIKE 'jac313_test_cli%'") == 1,
              "insert_run records every run setting");
        R::ParameterRow pr;
        pr.compiler_id = 1; pr.build_type = "Release"; pr.persist = "jtext+sql"; pr.keeper_pct = 10;
        pr.database_pct = 10; pr.binary_log = 1;
        const std::int64_t p1 = R::parameter_id(db, pr);
        check(p1 > 0 && R::parameter_id(db, pr) == p1, "parameter_id finds the row it inserted");
        for (auto change : {+[](R::ParameterRow& r) { r.binary_log = 0; }, +[](R::ParameterRow& r) { r.keeper_pct = 20; },
                            +[](R::ParameterRow& r) { r.database_pct = 5; }, +[](R::ParameterRow& r) { r.flags_raw = 3; },
                            +[](R::ParameterRow& r) { r.max_runs = 2; }, +[](R::ParameterRow& r) { r.max_records = 10; }}) {
            R::ParameterRow other = pr;
            change(other);
            check(R::parameter_id(db, other) != p1, "a schema-6 axis makes its own parameter row");
        }
        char arg0[] = "prog", arg1[] = "--filter", arg2[] = "a b", arg3[] = "it's";
        char* argv[] = {arg0, arg1, arg2, arg3};
        check(R::command_line_of(4, argv) == "prog --filter 'a b' 'it'\\''s'", "command_line_of quotes what needs it");
    }

    // A NEWER schema version (here 99): refused with both versions named, the file untouched.
    {
        Sqlite db(path.string());
        db.exec("CREATE TABLE stray (x INTEGER NOT NULL DEFAULT 0)");
        db.exec("PRAGMA user_version = 99");
    }
    {
        Sqlite db(path.string());
        std::string msg;
        try { R::ensure_schema(db); } catch (const std::exception& e) { msg = e.what(); }
        check(msg == "results.db is schema 99; this program knows schema " + std::to_string(R::kResultsSchemaVersion)
                     + ": rebuild this program", "newer schema refused with both versions named");
        check(one_int(db, "PRAGMA user_version") == 99, "newer schema: user_version untouched");
        check(one_int(db, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='stray'") == 1, "newer schema: tables untouched");
    }

    // An OLDER schema version (here 1) plus a stray table: rebuilt from scratch on open.
    {
        Sqlite db(path.string());
        db.exec("PRAGMA user_version = 1");
    }
    {
        Sqlite db(path.string());
        R::ensure_schema(db);
        check(one_int(db, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='stray'") == 0, "stray table dropped");
        check(one_int(db, "PRAGMA user_version") == R::kResultsSchemaVersion, "user_version back to kResultsSchemaVersion");
        check(one_int(db, "SELECT COUNT(*) FROM parameter") == 0, "rebuilt DB starts empty");
        check(table_names(db).size() == 18, "18 tables after rebuild");
    }

    // Schema 6 -> 7 only added run_result: it is created in place and every row is kept.
    {
        Sqlite db(path.string());
        db.exec("INSERT INTO testList(name) VALUES('kept')");
        db.exec("DROP TABLE run_change");
        db.exec("DROP TABLE run_result");
        db.exec("PRAGMA user_version = 6");
    }
    {
        Sqlite db(path.string());
        R::ensure_schema(db);
        check(one_int(db, "PRAGMA user_version") == R::kResultsSchemaVersion, "schema 6 brought up to date");
        check(one_int(db, "SELECT COUNT(*) FROM testList WHERE name='kept'") == 1, "schema 6 rows kept");
        check(one_int(db, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='run_result'") == 1,
              "schema 6 gains run_result");
        check(one_int(db, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='run_change'") == 1,
              "schema 6 gains run_change");
    }

    fs::remove(path);
    if (failures) { std::cerr << failures << " check(s) failed\n"; return 1; }
    std::cout << "results_schema_test: OK\n";
    return 0;
}
