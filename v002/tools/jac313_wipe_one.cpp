// jac313_wipe_one — delete ONE run (by run_id) and its testRun rows from results.db, and clean up
// that run's machine (the jac313-### / group_id entry: host_spec + io_best_fit + the current_host
// pin) WHEN this was the machine's last run. Shared dimension tables (testList/compiler/parameter)
// are always kept; safeness is recomputed from what's left. A dedicated tool so the surgical delete
// is reliable without re-deriving the SQL. The run is named as --<N> (the run_id shown as Run_<NNN>
// in the reports). --dry-run / --yes are mutually exclusive.
//
// Why the host cleanup is conditional: one jac313-### (group_id) owns MANY runs (host_spec is keyed
// by group_id, one row per machine). Deleting that row while other runs of the same machine remain
// would orphan them — so the machine entry is removed only when its last run goes.
#include "jac313_results_db.hpp"   // jac313::results — shared schema + helpers (pulls in Qlite + Sqlite)

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    fs::path src = ".";
    bool dry = false, yes = false;
    std::int64_t run_id = -1;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if      (a == "--dry-run")                    dry = true;
        else if (a == "--yes" || a == "-y")           yes = true;
        else if (a == "--source-dir" && i + 1 < argc) src = argv[++i];
        else if (a == "-h" || a == "--help") {
            std::cout << "jac313_wipe_one — delete ONE run (and its testRun rows) from results.db.\n"
                         "  --<N>              the run to delete, by run_id (e.g. --5 = Run_005)\n"
                         "  --dry-run          preview what would be deleted; change nothing\n"
                         "  --yes              delete run N + its testRun rows, then rebuild safeness\n"
                         "  --source-dir DIR   version root (default: .)\n"
                         "  When N is its machine's LAST run, the jac313-### entry (host_spec +\n"
                         "  io_best_fit + current_host pin) is removed too; otherwise it is kept.\n"
                         "  Shared dimensions (testList/compiler/parameter) are always kept.\n"
                         "  (--dry-run and --yes are mutually exclusive)\n";
            return 0;
        }
        // --<digits> selects the run_id (the only positional-ish flag, per the tool's contract).
        else if (a.size() > 2 && a[0] == '-' && a[1] == '-'
                 && a.find_first_not_of("0123456789", 2) == std::string::npos) {
            run_id = std::stoll(a.substr(2));
        }
        else { std::cerr << "jac313_wipe_one: unknown arg '" << a << "' (try --help)\n"; return 2; }
    }
    if (dry && yes)   { std::cerr << "jac313_wipe_one: --dry-run and --yes can't be used together.\n"; return 2; }
    if (!dry && !yes) { std::cerr << "jac313_wipe_one: pass --dry-run (preview) or --yes (do it).\n"; return 2; }
    if (run_id < 0)   { std::cerr << "jac313_wipe_one: name the run to delete as --<N> (e.g. --5). See --help.\n"; return 2; }

    const fs::path db_path = src / "test-summary" / "results.db";
    std::error_code ec;
    if (!fs::exists(db_path, ec)) { std::cout << "wipe-one: no results.db at " << db_path << " (nothing to delete).\n"; return 0; }

    jac313::results::Sqlite db(db_path.string());
    jac313::results::ensure_schema(db);

    auto one_long = [&](const char* sql, std::int64_t key) {
        std::int64_t n = 0; auto st = db.prepare(sql); st.bind(key); if (st.step()) st.get(n); return n;
    };
    if (!one_long("SELECT COUNT(*) FROM run WHERE run_id=?", run_id)) {
        std::cout << "wipe-one: no run " << run_id << " in " << db_path << " (nothing to delete).\n";
        return 0;
    }

    // The run's machine (group_id + label) and how many runs that machine still has.
    std::int64_t group_id = -1;
    { auto st = db.prepare("SELECT group_id FROM run WHERE run_id=?"); st.bind(run_id); if (st.step()) st.get(group_id); }
    std::string host;
    { auto st = db.prepare("SELECT host FROM run WHERE run_id=?"); st.bind(run_id); if (st.step()) st.get(host); }
    if (host.empty()) host = "group " + std::to_string(group_id);
    const std::int64_t n_tr           = one_long("SELECT COUNT(*) FROM testRun WHERE run_id=?", run_id);
    const std::int64_t n_ev           = one_long("SELECT COUNT(*) FROM runEvent WHERE run_id=?", run_id);
    const std::int64_t runs_in_group  = one_long("SELECT COUNT(*) FROM run WHERE group_id=?", group_id);
    const bool last_for_machine       = (runs_in_group <= 1);

    std::cout << "target: " << db_path << "\n  delete run " << run_id << "  (testRun rows=" << n_tr
              << ", runEvent=" << n_ev << ", machine " << host << ")\n  machine entry (" << host << "): "
              << (last_for_machine
                      ? "REMOVE — host_spec + io_best_fit + current_host pin (this was its last run)"
                      : "keep — " + std::to_string(runs_in_group - 1) + " other run(s) remain")
              << "\n  shared dimensions (testList/compiler/parameter) kept.\n";
    if (dry) { std::cout << "  dry-run — nothing changed; re-run with --yes to delete.\n"; return 0; }

    db.exec("DELETE FROM testRun WHERE run_id=?", run_id);
    db.exec("DELETE FROM runEvent WHERE run_id=?", run_id);
    db.exec("DELETE FROM run WHERE run_id=?", run_id);
    if (last_for_machine) {
        db.exec("DELETE FROM host_spec   WHERE group_id=?", group_id);
        db.exec("DELETE FROM io_best_fit WHERE group_id=?", group_id);
        db.exec("DELETE FROM current_host WHERE group_id=?", group_id);  // clears the pin only if it named this machine
    }
    jac313::results::rebuild_safeness(db);   // recount the per-(machine x gate x compiler) summary from what's left
    db.exec("VACUUM");
    std::cout << "  deleted run " << run_id << " + " << n_tr << " testRun row(s) + " << n_ev << " runEvent row(s); "
              << (last_for_machine ? ("removed machine " + host + "; ") : "")
              << "safeness rebuilt.\n";
    return 0;
}
