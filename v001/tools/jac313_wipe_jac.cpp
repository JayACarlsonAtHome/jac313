// jac313_wipe_jac — wipe a WHOLE machine (a jac313-### / machine_id) from results.db: ALL of that
// machine's runs and their testRun rows, PLUS every table that names the machine (kMachineTables):
// machine, io_best_fit, package_version, fleet_sync, and the current_host pin (if it named this
// machine). Shared dimension tables (testList/compiler/parameter/os) are kept; safeness is recomputed
// from what's left. A dedicated tool so the machine-scoped delete is
// reliable without re-deriving the SQL. The machine is named as --<G>: the machine_id, i.e. the ### in
// jac313-### (e.g. --1 = jac313-001, --8 = jac313-008). --dry-run / --yes are mutually exclusive.
#include "jac313_results_db.hpp"   // jac313::results — shared schema + helpers (pulls in Qlite + Sqlite)

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    fs::path src = ".";
    bool dry = false, yes = false;
    std::int64_t gid = -1;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if      (a == "--dry-run")                    dry = true;
        else if (a == "--yes" || a == "-y")           yes = true;
        else if (a == "--source-dir" && i + 1 < argc) src = argv[++i];
        else if (a == "-h" || a == "--help") {
            std::cout << "jac313_wipe_jac — wipe a WHOLE machine (jac313-### / machine_id) from results.db.\n"
                         "  --<G>              the machine to wipe, by machine_id / the ### (e.g. --1 = jac313-001)\n"
                         "  --dry-run          preview what would be deleted; change nothing\n"
                         "  --yes              delete all the machine's runs + testRuns + specs, then rebuild safeness\n"
                         "  --source-dir DIR   version root (default: .)\n"
                         "  Removes the machine's runs, machine row, io_best_fit, package_version,\n"
                         "  fleet_sync and the current_host pin.\n"
                         "  Shared dimensions (testList/compiler/parameter/os) are kept.\n"
                         "  (--dry-run and --yes are mutually exclusive)\n";
            return 0;
        }
        // --<digits> selects the machine_id (the only positional-ish flag, per the tool's contract).
        else if (a.size() > 2 && a[0] == '-' && a[1] == '-'
                 && a.find_first_not_of("0123456789", 2) == std::string::npos) {
            gid = std::stoll(a.substr(2));
        }
        else { std::cerr << "jac313_wipe_jac: unknown arg '" << a << "' (try --help)\n"; return 2; }
    }
    if (dry && yes)   { std::cerr << "jac313_wipe_jac: --dry-run and --yes can't be used together.\n"; return 2; }
    if (!dry && !yes) { std::cerr << "jac313_wipe_jac: pass --dry-run (preview) or --yes (do it).\n"; return 2; }
    if (gid < 0)      { std::cerr << "jac313_wipe_jac: name the machine to wipe as --<G> (e.g. --1). See --help.\n"; return 2; }

    const fs::path db_path = jac313::results::resolve_results_db(src);
    std::error_code ec;
    if (!fs::exists(db_path, ec)) { std::cout << "wipe-jac: no results.db at " << db_path << " (nothing to delete).\n"; return 0; }

    jac313::results::Sqlite db(db_path.string());
    jac313::results::ensure_schema(db);

    auto one_long = [&](const char* sql) {
        std::int64_t n = 0; auto st = db.prepare(sql); st.bind(gid); if (st.step()) st.get(n); return n;
    };
    const std::string label   = jac313::results::host_label(gid);
    const std::int64_t n_runs = one_long("SELECT COUNT(*) FROM run WHERE machine_id=?");
    const std::int64_t n_spec = one_long("SELECT COUNT(*) FROM machine WHERE id=?");
    const std::int64_t n_iobf = one_long("SELECT COUNT(*) FROM io_best_fit WHERE machine_id=?");
    const std::int64_t n_pin  = one_long("SELECT COUNT(*) FROM current_host WHERE machine_id=?");
    const std::int64_t n_tr   = one_long("SELECT COUNT(*) FROM testRun WHERE run_id IN "
                                         "(SELECT run_id FROM run WHERE machine_id=?)");
    const std::int64_t n_ev   = one_long("SELECT COUNT(*) FROM runEvent WHERE run_id IN "
                                         "(SELECT run_id FROM run WHERE machine_id=?)");
    if (!jac313::results::machine_in_use(db, gid)) {
        std::cout << "wipe-jac: no machine " << label << " in " << db_path << " (nothing to delete).\n";
        return 0;
    }

    std::cout << "target: " << db_path << "\n  wipe machine " << label << ": "
              << "runs=" << n_runs << " testRun=" << n_tr << " runEvent=" << n_ev << " machine=" << n_spec
              << " io_best_fit=" << n_iobf << " current_host_pin=" << n_pin
              << "\n  shared dimensions (testList/compiler/parameter) kept.\n";
    if (dry) { std::cout << "  dry-run — nothing changed; re-run with --yes to delete.\n"; return 0; }

    db.exec("DELETE FROM testRun WHERE run_id IN (SELECT run_id FROM run WHERE machine_id=?)", gid);
    db.exec("DELETE FROM runEvent WHERE run_id IN (SELECT run_id FROM run WHERE machine_id=?)", gid);
    db.exec("DELETE FROM run_target WHERE run_id IN (SELECT run_id FROM run WHERE machine_id=?)", gid);
    // run, machine, io_best_fit, package_version, fleet_sync, and the current_host pin (only if it
    // named this machine)
    for (const auto& t : jac313::results::kMachineTables)
        db.exec(std::string("DELETE FROM ") + t.table + " WHERE " + t.column + "=?", gid);
    jac313::results::rebuild_safeness(db);   // recount the per-(machine x OS x gate x compiler) summary from what's left
    db.exec("VACUUM");
    std::cout << "  wiped machine " << label << ": " << n_runs << " run(s) + " << n_tr
              << " testRun row(s) + specs; safeness rebuilt.\n";
    return 0;
}
