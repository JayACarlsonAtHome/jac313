// Regression test for the bug that started the RunIdentity work: a finalize dedup
// keyed on an identity *missing* the OS dimension silently overwrote a different
// machine's run (RHEL 10 clobbering the host's gcc15 row). This drives the REAL
// identity_where_eq() against a real (in-memory) SQLite DB and asserts:
//   1. runs that differ ONLY in os do NOT collide (both survive the dedup), and
//   2. runs with the SAME identity still dedup (older one removed).

#include "run_identity.hpp"

#include <jac313/Qlite/v001.hpp>

#include <cstdint>
#include <iostream>
#include <string>

using jac313::Qlite::v001::Sqlite;
using namespace jac313::test_cli;

namespace {

int failures = 0;

#define CHECK(cond)                                                            \
    do {                                                                       \
        if (!(cond)) {                                                         \
            std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__  \
                      << "\n";                                                  \
            ++failures;                                                        \
        }                                                                      \
    } while (0)

std::int64_t count_rows(Sqlite& db) {
    auto s = db.prepare("SELECT COUNT(*) FROM runs");
    s.step();
    std::int64_t n = 0;
    s.get(n);
    return n;
}

// The exact DELETE finalize_matrix_run runs: keep only `keep_id` for its identity.
void dedup_for(Sqlite& db, std::int64_t keep_id) {
    auto sel = db.prepare("SELECT " + identity_select_list() + " FROM runs WHERE id=?");
    sel.bind(keep_id);
    sel.step();
    RunIdentity id;
    sel.get(id.os, id.compiler, id.build_type, id.disk_type, id.size_label, id.modules);
    db.exec("DELETE FROM runs WHERE " + identity_where_eq() + " AND id != ?",
            id.os, id.compiler, id.build_type, id.disk_type, id.size_label, id.modules, keep_id);
}

void insert(Sqlite& db, std::int64_t row_id, const RunIdentity& id) {
    db.exec("INSERT INTO runs (id, " + identity_select_list() + ") VALUES (?, ?, ?, ?, ?, ?, ?)",
            row_id, id.os, id.compiler, id.build_type, id.disk_type, id.size_label, id.modules);
}

} // namespace

int main() {
    Sqlite db(":memory:");
    db.exec("CREATE TABLE runs (id INTEGER PRIMARY KEY, os TEXT, compiler TEXT, "
            "build_type TEXT, disk_type TEXT, size_label TEXT, modules TEXT)");

    // Two runs identical except os -> the original collision case.
    insert(db, 1, {"rhel-9.8",  "gcc15", "Debug", "ssd", "Smoke", "textual"});
    insert(db, 2, {"rhel-10.2", "gcc15", "Debug", "ssd", "Smoke", "textual"});

    dedup_for(db, 1);                 // finalize the rhel-9.8 run
    CHECK(count_rows(db) == 2);       // rhel-10.2 must NOT be clobbered (the bug)

    // A second rhel-9.8 run with the SAME identity must replace the first.
    insert(db, 3, {"rhel-9.8", "gcc15", "Debug", "ssd", "Smoke", "textual"});
    dedup_for(db, 3);
    CHECK(count_rows(db) == 2);       // row 1 gone, rows 2 (10.2) and 3 (9.8) remain

    auto s = db.prepare("SELECT id FROM runs ORDER BY id");
    std::int64_t first = 0;
    CHECK(s.step());
    s.get(first);
    CHECK(first == 2);                // row 1 (same identity as 3) was removed

    if (failures == 0) {
        std::cout << "identity dedup — OS distinguishes runs; same-identity dedups\n";
        return 0;
    }
    std::cerr << failures << " check(s) failed\n";
    return 1;
}
