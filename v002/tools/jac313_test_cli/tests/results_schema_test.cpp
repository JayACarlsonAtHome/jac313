// results.db schema: versioned (PRAGMA user_version), every column NOT NULL, the parameter UNIQUE
// really dedupes, ensure_schema is idempotent, and a DB of another schema version is rebuilt.

#include "jac313_results_db.hpp"

#include <jac313/Qlite/v002.hpp>

#include <cstdint>
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
        check(tables.size() == 14, "14 tables");
        for (const auto& t : tables) {
            const std::int64_t cols = one_int(db, "SELECT COUNT(*) FROM pragma_table_info('" + t + "')");
            const std::int64_t nn   = one_int(db, "SELECT COUNT(*) FROM pragma_table_info('" + t + "') WHERE \"notnull\"=1");
            check(cols > 0 && cols == nn, "every column NOT NULL in " + t);
        }

        // Schema 2: the production bench run's hot-path ops/sec and its three drain times.
        for (const char* col : {"hot_ops", "drain_keeper_ms", "drain_database_ms", "drain_binary_ms"})
            check(one_int(db, std::string("SELECT COUNT(*) FROM pragma_table_info('testRun') WHERE name='") + col + "'") == 1,
                  std::string("testRun has ") + col);

        bool threw = false;
        try {
            db.exec("INSERT INTO parameter(compiler_id, size) VALUES(1, NULL)");
        } catch (const std::exception&) { threw = true; }
        check(threw, "explicit NULL into parameter.size throws");

        db.exec(kParamInsert);
        db.exec(kParamInsert);
        check(one_int(db, "SELECT COUNT(*) FROM parameter") == 1, "identical parameter rows dedupe to one");

        // host_spec is unique on the identity too: --assign-new-### for a machine already filed
        // under another number moves its row instead of failing on (or duplicating) the identity.
        const R::HostId h{"cpu", 8, 32, "os", "ssd", "hash", 4, 1000, 2000};
        check(R::pin_host(db, h, "v002") == 1, "first pin is group 1");
        R::HostPinOptions opts;
        opts.assign_new = 5;
        const auto moved = R::resolve_and_pin(db, h, opts, false, "v002");
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
        check(table_names(db).size() == 14, "14 tables after rebuild");
    }

    fs::remove(path);
    if (failures) { std::cerr << failures << " check(s) failed\n"; return 1; }
    std::cout << "results_schema_test: OK\n";
    return 0;
}
