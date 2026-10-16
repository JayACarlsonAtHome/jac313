// test_readback.cpp — proves read-back verification (readback.hpp) catches damage.
//
// For every backend built in (binary, jtext, html, json, sql; pg-local / pg-remote only when
// asked with --pg, since they need a reachable server), it:
//   1. persists a small run (with integer and double metrics) and checks read-back PASSES,
//   2. damages the stored data (edits a value, or removes a row), and checks read-back FAILS.
// A read-back that cannot fail would prove nothing; this test keeps it honest.

#include "test_common.hpp"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace jac313::Store::v001;
using namespace jac313::Store::v001::matrix_test;
namespace fs = std::filesystem;

using RbConfig = ts_store_config<true, 6, 20, 43, 3, 2, false>;
using RbStore  = ts_store<RbConfig>;

namespace {

int failures = 0;

void check(bool cond, const std::string& what) {
    std::cout << (cond ? "  ok    " : "  FAIL  ") << what << "\n";
    if (!cond) ++failures;
}

std::string slurp(const fs::path& p) {
    std::ifstream in(p, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), {}};
}

void spit(const fs::path& p, const std::string& s) {
    std::ofstream out(p, std::ios::binary | std::ios::trunc);
    out << s;
}

// Change one character inside the first data row of a text file: the first digit after
// `marker` becomes a different digit. Returns false if nothing was found to change.
bool damage_text(const fs::path& p, std::string_view marker) {
    std::string s = slurp(p);
    auto at = s.find(marker);
    if (at == std::string::npos) return false;
    for (auto i = at + marker.size(); i < s.size(); ++i) {
        if (s[i] >= '0' && s[i] <= '9') { s[i] = (s[i] == '7') ? '8' : '7'; spit(p, s); return true; }
    }
    return false;
}

// Drop the last data row of a line-oriented text file whose rows start with `row_prefix`.
bool drop_last_row(const fs::path& p, std::string_view row_prefix) {
    std::string s = slurp(p);
    auto at = s.rfind(std::string("\n") + std::string(row_prefix));
    if (at == std::string::npos) return false;
    auto end = s.find('\n', at + 1);
    s.erase(at, (end == std::string::npos ? s.size() : end) - at);
    spit(p, s);
    return true;
}

ReadbackReport readback(const RbStore& store, const std::string& backend, const std::string& base) {
    return run_readback(store, backend, base, RbConfig::the_IntMetrics, RbConfig::the_DblMetrics, "all");
}

void run_backend(const std::string& backend, const fs::path& dir) {
    std::cout << "[" << backend << "]\n";
    fs::create_directories(dir);
    const std::string base = (dir / "persist").string();

    RbStore store(3, 40);
    TestOptions opts;
    opts.persist = backend;
    opts.base_name = base;
    opts.readback = "all";
    if (!attach_persistence_from_opts<RbConfig>(store, opts)) { check(false, "attach " + backend); return; }
    store.test_run();
    store.finalize_persistence();

    auto rep = readback(store, backend, base);
    check(rep.ok && rep.found == 120 && rep.checked == 120, "untouched dataset reads back (120 rows)");
    if (!rep.ok) std::cout << "        " << rep.error << "\n";

    // Damage 1: change a value.
    bool damaged = false;
    if (backend == "binary") {
        std::string s = slurp(base + ".bin");
        if (s.size() > 64) { s[s.size() - 5] ^= 0x01; spit(base + ".bin", s); damaged = true; }   // last record's tail
    } else if (backend == "jtext") {
        // the row for event id 0, wherever the writer threads put it (not always the first row)
        damaged = damage_text(base + ".jtext", "\n1. #|# 0|");
    } else if (backend == "html") {
        damaged = damage_text(base + ".html", "<tr><td>0</td><td>");
    } else if (backend == "json") {
        damaged = damage_text(base + ".json", "\"timestamp_us\":");
    }
#ifdef JAC313_STORE_HAS_SQL_PERSIST
    if (backend == "sql") {
        Sqlite db(base + ".db");
        db.exec("UPDATE persist SET payload = payload || 'x' WHERE id = 17");
        damaged = true;
    }
#endif
#ifdef JAC313_STORE_HAS_PG_PERSIST
    if (is_pg_backend(backend)) {
        PgConn c(pg_conninfo_for(backend));
        c.exec("UPDATE " + std::string(kPgTestSchema) + "." + pg_table_base(base) +
               "_floats SET dbl1 = dbl1 + 0.5 WHERE id = 33");
        damaged = true;
    }
#endif
    check(damaged, "damaged a stored value");
    rep = readback(store, backend, base);
    check(!rep.ok, "read-back catches the changed value: " + (rep.error.empty() ? std::string("(no error)") : rep.error));

    // Damage 2 (fresh dataset): remove a row.
    RbStore store2(3, 40);
    if (!attach_persistence_from_opts<RbConfig>(store2, opts)) { check(false, "attach " + backend); return; }
    store2.test_run();
    store2.finalize_persistence();
    bool removed = false;
    if (backend == "jtext")      removed = drop_last_row(base + ".jtext", "1. #|# ");
    else if (backend == "html")  removed = drop_last_row(base + ".html", "<tr><td>");
    else if (backend == "json")  removed = drop_last_row(base + ".json", "{\"id\":");
    else if (backend == "binary") {
        std::string s = slurp(base + ".bin");
        s.resize(s.size() * 9 / 10);   // truncate: the last records are gone
        spit(base + ".bin", s);
        removed = true;
    }
#ifdef JAC313_STORE_HAS_SQL_PERSIST
    if (backend == "sql") {
        Sqlite db(base + ".db");
        db.exec("DELETE FROM persist WHERE id = 99");
        removed = true;
    }
#endif
#ifdef JAC313_STORE_HAS_PG_PERSIST
    if (is_pg_backend(backend)) {
        PgConn c(pg_conninfo_for(backend));
        c.exec("DELETE FROM " + std::string(kPgTestSchema) + "." + pg_table_base(base) + " WHERE id = 99");
        removed = true;
    }
#endif
    check(removed, "removed a stored row");
    for (const auto* m : {"count", "sample", "all"}) {
        const bool last = std::string_view(m) == "all";   // pg: drop the test tables after the last read-back
        rep = run_readback(store2, backend, base, RbConfig::the_IntMetrics, RbConfig::the_DblMetrics, m,
                           last ? "delete" : "keep-last");
        check(!rep.ok && rep.count == "FAIL",
              std::string("read-back (") + m + ") catches the missing row: " + rep.error);
    }
}

} // namespace

int main(int argc, char** argv) {
    bool with_pg = false;
    fs::path dir = fs::temp_directory_path() / "jac313_readback_selftest";
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--pg") with_pg = true;
        else if (a.starts_with("--dir=")) dir = a.substr(6);
    }
    std::error_code ec;
    fs::remove_all(dir, ec);

    std::vector<std::string> backends = {"binary", "html", "json"};
#ifdef JAC313_STORE_HAS_JTEXT_PERSIST
    backends.push_back("jtext");
#endif
#ifdef JAC313_STORE_HAS_SQL_PERSIST
    backends.push_back("sql");
#endif
#ifdef JAC313_STORE_HAS_PG_PERSIST
    if (with_pg) {
        backends.push_back("pg-local");
        const char* remote = std::getenv("JAC313_PG_REMOTE");
        if (remote && *remote) backends.push_back("pg-remote");
        else std::cout << "[pg-remote] skipped: no remote server set ($JAC313_PG_REMOTE)\n";
    }
#endif
    for (const auto& b : backends) run_backend(b, dir / b);

    fs::remove_all(dir, ec);
    if (failures == 0) {
        std::cout << "read-back self-test: all passed (" << backends.size() << " backends)\n";
        return 0;
    }
    std::cout << failures << " read-back self-test check(s) failed\n";
    return 1;
}
