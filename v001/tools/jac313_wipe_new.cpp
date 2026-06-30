// jac313_wipe_new — reset results.db to a blank slate: the schema and the testType enum are kept;
// all data, the host_spec rows, and the current_host pin are cleared. A dedicated tool so the reset is
// reliable and repeatable without re-deriving the SQL. --dry-run and --yes are mutually exclusive.
#include "jac313_results_db.hpp"   // jac313::results — shared schema + helpers (pulls in Qlite + Sqlite)

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    fs::path src = ".";
    bool dry = false, yes = false;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if      (a == "--dry-run")                    dry = true;
        else if (a == "--yes" || a == "-y")           yes = true;
        else if (a == "--source-dir" && i + 1 < argc) src = argv[++i];
        else if (a == "-h" || a == "--help") {
            std::cout << "jac313_wipe_new — reset results.db to blank (schema + testType enum kept).\n"
                         "  --dry-run          preview what would be cleared; change nothing\n"
                         "  --yes              clear results + host_spec + the current_host pin\n"
                         "  --source-dir DIR   version root (default: .)\n"
                         "  (--dry-run and --yes are mutually exclusive)\n";
            return 0;
        }
        else { std::cerr << "jac313_wipe_new: unknown arg '" << a << "' (try --help)\n"; return 2; }
    }
    if (dry && yes)   { std::cerr << "jac313_wipe_new: --dry-run and --yes can't be used together.\n"; return 2; }
    if (!dry && !yes) { std::cerr << "jac313_wipe_new: pass --dry-run (preview) or --yes (do it).\n"; return 2; }

    const fs::path db_path = src / "test-summary" / "results.db";
    std::error_code ec;
    if (!fs::exists(db_path, ec)) { std::cout << "wipe-new: no results.db at " << db_path << " (already blank).\n"; return 0; }

    jac313::results::Sqlite db(db_path.string());
    jac313::results::ensure_schema(db);
    auto cnt = [&](const char* t) { std::int64_t n = 0; auto st = db.prepare(std::string("SELECT COUNT(*) FROM ") + t);
                                    if (st.step()) st.get(n); return n; };
    std::cout << "target: " << db_path << "\n  run=" << cnt("run") << " testRun=" << cnt("testRun")
              << " host_spec=" << cnt("host_spec") << " current_host=" << cnt("current_host")
              << " compiler=" << cnt("compiler") << " parameter=" << cnt("parameter")
              << " testList=" << cnt("testList") << "  (testType enum kept)\n";
    if (dry) { std::cout << "  dry-run — nothing changed; re-run with --yes to clear.\n"; return 0; }
    for (const char* t : {"testRun", "run", "parameter", "compiler", "testList", "current_host", "host_spec"})
        db.exec(std::string("DELETE FROM ") + t);
    db.exec("VACUUM");
    std::cout << "  CLEAN SLATE - results, host_spec, and the current_host pin cleared (schema + testType kept).\n";
    return 0;
}
