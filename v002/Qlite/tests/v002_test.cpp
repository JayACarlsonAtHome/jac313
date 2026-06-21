// Unit tests for the in-tree jac313::Qlite::v002 SQLite wrapper.
// Covers exec/bind/get, prepared-statement reuse, optional/NULL handling,
// transactions (commit + rollback), and error reporting.

#include <jac313/Qlite/v002.hpp>

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>

static_assert(jac313::Qlite::v002::version() == "v002.001", "v002 version");

using jac313::Qlite::v002::Sqlite;
using jac313::Qlite::v002::SqliteError;

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

void test_exec_bind_get() {
    Sqlite db(":memory:");
    db.exec("CREATE TABLE t (id INTEGER PRIMARY KEY, name TEXT, score REAL)");
    db.exec("INSERT INTO t (name, score) VALUES (?, ?)", std::string("jac313"), 3.5);

    auto stmt = db.prepare("SELECT id, name, score FROM t");
    CHECK(stmt.step());
    std::int64_t id = 0;
    std::string name;
    double score = 0.0;
    stmt.get(id, name, score);
    CHECK(id == 1);
    CHECK(name == "jac313");
    CHECK(score == 3.5);
    CHECK(!stmt.step());
}

void test_prepared_reuse_and_rows() {
    Sqlite db(":memory:");
    db.exec("CREATE TABLE t (n INTEGER)");
    auto ins = db.prepare("INSERT INTO t (n) VALUES (?)");
    for (std::int64_t i = 1; i <= 5; ++i) {
        ins.bind(i);          // bind() resets + clears + binds, so reuse is safe
        CHECK(!ins.step());   // INSERT has no result row
    }

    auto sel = db.prepare("SELECT n FROM t ORDER BY n");
    std::int64_t sum = 0, rows = 0;
    while (sel.step()) {
        auto [n] = sel.get_row<std::int64_t>();
        sum += n;
        ++rows;
    }
    CHECK(rows == 5);
    CHECK(sum == 15);
}

void test_optional_null() {
    Sqlite db(":memory:");
    db.exec("CREATE TABLE t (id INTEGER, label TEXT)");
    db.exec("INSERT INTO t VALUES (?, ?)", 1, std::optional<std::string>{});        // NULL
    db.exec("INSERT INTO t VALUES (?, ?)", 2, std::optional<std::string>{"set"});    // value

    auto stmt = db.prepare("SELECT label FROM t ORDER BY id");
    CHECK(stmt.step());
    std::optional<std::string> a;
    stmt.get(a);
    CHECK(!a.has_value());
    CHECK(stmt.step());
    std::optional<std::string> b;
    stmt.get(b);
    CHECK(b.has_value());
    CHECK(*b == "set");
}

void test_transaction_commit_and_rollback() {
    Sqlite db(":memory:");
    db.exec("CREATE TABLE t (n INTEGER)");

    db.begin();
    db.exec("INSERT INTO t (n) VALUES (1)");
    db.commit();

    db.begin();
    db.exec("INSERT INTO t (n) VALUES (2)");
    db.rollback();

    auto stmt = db.prepare("SELECT COUNT(*) FROM t");
    CHECK(stmt.step());
    std::int64_t count = 0;
    stmt.get(count);
    CHECK(count == 1);   // committed row survives, rolled-back row does not
}

void test_error_on_bad_sql() {
    Sqlite db(":memory:");
    bool threw = false;
    try {
        db.exec("THIS IS NOT VALID SQL");
    } catch (const SqliteError&) {
        threw = true;
    }
    CHECK(threw);
}

} // namespace

int main() {
    test_exec_bind_get();
    test_prepared_reuse_and_rows();
    test_optional_null();
    test_transaction_commit_and_rollback();
    test_error_on_bad_sql();

    if (failures == 0) {
        std::cout << "jac313::Qlite::v002 — all CRUD/transaction/error tests passed\n";
        return 0;
    }
    std::cerr << failures << " check(s) failed\n";
    return 1;
}
