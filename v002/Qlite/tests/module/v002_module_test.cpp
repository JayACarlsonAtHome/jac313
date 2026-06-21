#include <cstdlib>
#include <string>
#include <cstdint>

import jac313.qlite.v002;

// Functional module smoke. This consumer is an ordinary TU (textual std includes)
// that imports the module and EXERCISES the exported symbols — Sqlite, Statement,
// bind/column, SqliteError — end to end against an in-memory database.
//
// The original test only checked that `import jac313.qlite.v002;` resolved; it
// never called anything, so a module that exported no usable definitions would
// still pass. Driving a real round-trip proves both module front-ends actually
// export working definitions: the textual export-using path (clang/fallback) AND
// the import-std export-namespace path (g++-15 pilot, JAC313_QLITE_IMPORT_STD).
int main() {
    using namespace jac313::Qlite::v002;
    try {
        Sqlite db(":memory:");
        db.exec("CREATE TABLE t(id INTEGER, name TEXT);");
        db.exec("INSERT INTO t VALUES(?, ?);", std::int64_t{42}, std::string("alpha"));

        auto st = db.prepare("SELECT id, name FROM t WHERE id = ?;");
        st.bind(std::int64_t{42});
        if (!st.step()) return EXIT_FAILURE;

        std::int64_t id = 0;
        std::string name;
        st.get(id, name);
        if (id != 42 || name != "alpha") return EXIT_FAILURE;
    } catch (const SqliteError&) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
