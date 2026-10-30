// jac313_test_cli — local test runner for the jac313 monorepo.
// Discovers CTest entries from a build tree, runs them with captured output,
// and prints a summary. No shell scripts required.

#include "contract_modes.hpp"
#include "build_pipeline.hpp"
#include "compiler.hpp"
#include "format.hpp"
#include "manifest.hpp"
#include "matrix.hpp"
#include "options.hpp"
#include "params.hpp"
#include "platform.hpp"
#include "process.hpp"
#include "report.hpp"
#include "run_identity.hpp"
#include "settings.hpp"
#include "host_hardware.hpp"
#include "host_identity.hpp"
#include "host_pin.hpp"
#include "kept_logs.hpp"
#include "runner.hpp"
#include "test_logs.hpp"
#include "summary.hpp"
#include "connections.hpp"
#include "vendor.hpp"

#include <jac313/Qlite/v001.hpp>   // shared testList catalog in the bench results DB
#include "jac313_results_db.hpp"   // jac313::results — shared schema + dimension helpers

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <string>
#include <vector>
#include <chrono>
#include <locale>
#include <sstream>
#include <map>
#include <set>
#include <tuple>
#include <unordered_map>
#include <iomanip>

#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>

namespace fs = std::filesystem;
using namespace jac313::test_cli;

namespace {

void print_usage() {
    std::cout <<
        "jac313_test_cli — local jac313 test runner\n\n"
        "Usage:\n"
        "  jac313_test_cli <command> [options]\n\n"
        "Commands:\n"
        "  list        List discovered tests from the build tree, and its contract mode\n"
        "  run         Run tests (default if command omitted)\n"
        "  configure   CMake configure (Ninja; auto-detect compiler)\n"
        "  build       Build the configured tree\n"
        "  build-times compile-time matrix (smoke+bench x front-end x compiler), gap-filled per host\n"
        "  pin         find-or-create this machine's committed compiler pin (Setup/compilers.pin.jtext)\n"
        "  compilers   Sense toolchains from the registry (writes FileCheckList.txt)\n"
        "  setup       Sense readiness; name anything ./bsir.sh still has to install\n"
        "  version-check  check each package owes a version() bump (git, no build)\n"
        "  vendor-check   drift of the vendored jText copy (and the Qlite it brings in) (warnings only)\n"
        "  vendor-sync    forward-port canonical into a vendored copy (dry run; --write)\n"
        "  vendor-baseline  a starting-line VENDORED.jtext for an existing copy\n"
        "  note        note RUN_ID [\"TEXT\"]: add a note to a run in results.db, or list its notes\n"
        "  connections show / --init the PostgreSQL connections file (jText, outside git)\n"
        "  gui-build   build the U++ GUI (<root>/gui/Jac313Runner) with umk -> <root>/gui/out/\n"
        "  matrix      run | runner | verify | verify-lite\n\n"
        "Common options:\n"
        "  --build-dir <path>   Build directory (default: build)\n"
        "  --source-dir <path>  Source root (default: auto-detect)\n"
        "  -v, --verbose        Verbose output\n"
        "  -h, --help           Show help\n"
        "  --allow-root         run as root in a tree a regular user owns (refused by default)\n\n"
        "Preset gates (no subcommand — compose freely; writes & runs ./run_latest_config.sh):\n"
        "  --ctest              ctest unit suite\n"
        "  --smoke              smoke matrix: persist (binary/jtext/html/json/sql/none) × output on/off\n"
        "  --bench              throughput benchmark — the production run first (keeper + database\n"
        "                       sinks + binary replay log, flags routed), then each durable sink\n"
        "                       alone, then the non-durable flag sweep (auto-records to results.db)\n"
        "  --production <spec>  the production run's mix, e.g. keeper=jtext,database=sql,keeper_pct=10,\n"
        "                       database_pct=10,binary=on (default: the one row of tests/production.jtext,\n"
        "                       which serves the smoke, full and bench profiles)\n"
        "  --report             render-only: (re)build the .md report pages from results.db\n"
        "                       (alone; every gate run renders them at its end anyway)\n"
        "  --verify-lite        valgrind: ctest + matrix 001 (binary; 001 across jtext/html/json/sql)\n"
        "  --verify             valgrind: full smoke matrix (all sinks incl. html/json) + helgrind/DRD\n"
        "  --machine-id         read-only precheck: which jac313-### (machine) this machine records under\n"
        "  --claim <jac313-###> bind this machine to an existing fleet slot (same OS+hardware)\n"
        "  --assign-new-###     create a new slot at explicit ### (e.g. --assign-new-003)\n"
        "  --run-everything     the FULL battery: every gate on both compilers + build-times + report\n"
        "                       (does NOT keep durable captures — add --keep-logs for that)\n"
        "  --keep-logs          opt-in: retain durable captures under kept-logs/ (see kept-logs/README.md)\n"
        "                       layout: bench/<compiler>/<option>/capture (e.g. json_10M, html_1M),\n"
        "                                smoke/<compiler>/<test>/<option>/ (e.g. jtext_output_on)\n"
        "  --keep-logs-dir <p>  root for kept output (default: kept-logs; needs ~12+ GB for full run)\n"
        "  --wipe-logs          delete test-results/ + kept-logs/ (keeps README.md); alone or before a run\n"
        "  --threads N, --events-per-thread N, --runs N, --batch N\n"
        "                       bench: replace the suite's 50 threads / per-config events and runs /\n"
        "                       the calibrated durable batch for every config\n"
        "  Every flag given to a preset is forwarded to each step that takes it; a step that cannot\n"
        "  take one says so in a comment line of run_latest_config.sh. A recorder that could not write\n"
        "  results.db prints \"NOT RECORDED: ...\" and the command exits non-zero.\n"
        "  (every gate auto-records + renders the report at the end. everyday: --ctest --smoke ;\n"
        "   bench: --bench ; render: --report ; full battery: --run-everything ;\n"
        "   full battery + retained captures: --run-everything --keep-logs)\n\n"
        "Run options:\n"
        "  --filter <regex>     Run only matching tests (matrix: test, persist backend,\n"
        "                       or name|persist|mode — binary|jtext|html|json|sql|none)\n"
        "  --fail-fast          Stop on first failure\n"
        "  --failsafe <sec>     Kill stuck test/compile after N seconds (smoke: "
        + std::to_string(kSmokeFailsafeSec) + ", full: " + std::to_string(kFullFailsafeSec)
        + ", 0=off)\n"
        "                       tests/test_limits.jtext (testControl) timeouts win over N; 0 turns\n"
        "                       every timeout off, testControl included\n"
        "  -j N                 parallel build jobs\n"
        "  --report <path>      Write summary file (tab-separated)\n\n"
        "Configure options:\n"
        "  --compiler <cxx>     Explicit compiler: registry label, name, or path\n"
        "  --gcc                The newest gcc on this machine (default)\n"
        "  --clang              Select the clang toolchain (min Clang 20)\n"
        "  --modules            Enable JAC313_BUILD_MODULES=ON\n"
        "  --release            CMAKE_BUILD_TYPE=Release (default: Debug)\n"
        "  --pg-remote \"host=H [port=P] dbname=D user=U\"   the remote PostgreSQL server for pg-remote\n"
        "                       (no default; or $JAC313_PG_REMOTE). Password: ../SQL_Secrets/jac313_pg_remote.pgpass\n"
        "  --pg-local \"...\"     the local server for pg-local (default host=localhost dbname=jac313 user=jac313)\n"
        "  --connections FILE   both servers from a jText connections file kept outside git (default:\n"
        "                       <projects>/SQL_Secrets/jac313_connections.jtext; see `connections`)\n"
        "  --strict             JAC313_STRICT=ON: -Wall -Wextra -Wpedantic and friends (ctest/smoke, bench, verify)\n"
        "                       (format checks, -Wformat=2 -Werror=format, are on in every build)\n"
        "  --hardened           JAC313_HARDENED=ON: libstdc++ assertions, zero-init locals, stack/CF\n"
        "                       protection (ctest/smoke, bench and verify builds)\n"
        "                       Either one builds in build-<cc>-strict / -hardened and records the\n"
        "                       build type as Debug-strict / Debug-hardened / Debug-strict-hardened\n"
        "  --contracts MODE     JAC313_CONTRACTS=MODE: how contracts run (ctest/smoke, bench, verify):\n"
        "                       enforce (default; a violation prints and ends the program),\n"
        "                       observe (prints, goes on), quick_enforce (ends it at once, silently),\n"
        "                       ignore (not checked). The modes each toolchain has on each OS:\n"
        "                       Setup/contracts.jtext (native C++26 contracts, GCC 16 on v002, have\n"
        "                       all four; elsewhere enforce or ignore). Not enforce: builds in\n"
        "                       build-<cc>-contracts-MODE; results.db records the mode per run\n"
        "  --no-store-tests     Disable JAC313_BUILD_STORE_TESTS\n\n"
        "Toolchains are data-driven: see Setup/compilers.jtext\n"
        "(activation-aware — handles gcc-toolset via scl / env-launcher / source-script).\n"
        "A gitignored Setup/compilers.local.jtext / Setup/recipes.local.jtext overrides rows of\n"
        "those tables on this machine (see the .local.jtext.example files).\n"
        "Auto-detect prefers the highest-version gcc that resolves; build dir from the\n"
        "toolchain's build_dir when --build-dir is omitted.\n\n"
        "Examples:\n"
        "  ./jac313_test_cli --ctest --smoke\n"
        "  ./jac313_test_cli --bench --report\n"
        "  ./jac313_test_cli compilers\n"
        "  ./jac313_test_cli run --filter store\n"
        "  ./jac313_test_cli list --build-dir build\n"
        "  ./jac313_test_cli matrix runner --compiler gcc15 --build-type Debug --modules on --size smoke\n"
        "  ./jac313_test_cli matrix verify-lite\n";
}

fs::path detect_source_dir(const fs::path& start) {
    for (fs::path dir = fs::absolute(start);; dir = dir.parent_path()) {
        if (fs::exists(dir / "CMakeLists.txt") && fs::exists(dir / "Store") && fs::exists(dir / "jText")) {
            return dir;
        }
        if (dir == dir.parent_path()) {
            break;
        }
    }
    return start;
}

// The shared-DB path (JAC313_RESULTS_DB) + its one-time resolver live in jac313_results_db.hpp so the
// wipe tools + store_bench share ONE definition. Surface them unqualified here.
using jac313::results::JAC313_RESULTS_DB;
using jac313::results::resolve_results_db;

// A recorder that could not write its rows (no test-summary/, no host pin, a DB error) says so here;
// main() then prints one "NOT RECORDED: ..." line per reason on stdout and exits non-zero, so a run
// that recorded nothing can never look successful.
std::vector<std::string> g_not_recorded;
void not_recorded(const std::string& gate, const std::string& why) {
    std::cerr << "[results] " << gate << " not recorded: " << why << '\n';
    g_not_recorded.push_back(gate + ": " + why);
}

int run_list_command(const GlobalOptions& global) {
    const auto tests = discover_tests(global.build_dir);
    if (tests.empty()) {
        std::cerr << "No tests found under " << global.build_dir
                  << " (run configure + build first?)\n";
        return 1;
    }

    std::cout << "Discovered " << tests.size() << " test(s) (contracts: "
              << read_build_features(global.build_dir).contracts << "):\n";
    for (const auto& t : tests) {
        std::cout << "  " << t.name << '\n';
        if (global.verbose) {
            std::cout << "    " << t.command.string();
            for (const auto& arg : t.args) {
                std::cout << ' ' << arg;
            }
            std::cout << '\n';
        }
    }
    return 0;
}

// Seed the discovered test names into the shared testList catalog in results.db. Best-effort:
// the catalog is a convenience, never block a run on it. ensure_schema + INSERT OR IGNORE,
// so it merges with store_bench's bench-config entries and a re-run writes nothing (no DB churn).
void seed_test_catalog(const fs::path& /*source_dir*/, const std::vector<TestEntry>& tests) {
    const fs::path db_path = JAC313_RESULTS_DB;
    std::error_code ec;
    if (!fs::exists(db_path.parent_path(), ec)) {
        return;   // no test-summary/ -> nothing to catalog into
    }
    try {
        jac313::Qlite::v001::Sqlite db(db_path.string());
        jac313::results::ensure_schema(db);
        for (const auto& t : tests) {
            db.exec("INSERT OR IGNORE INTO testList(name) VALUES(?)", t.name);
        }
    } catch (const std::exception&) {
        // best-effort: a catalog write failure never fails the test run
    }
}

// Defined further down (next to the other results.db recorders); declared here for run_tests_command.
std::int64_t record_ctest_results(const GlobalOptions& global, const fs::path& build_dir,
                                  const std::vector<TestResult>& results);
// A recorded run's machine ("jac313-004") and OS ("Fedora Linux 44 · 7.2.13-..."), "" when unknown.
struct RunWhere { std::string host, os; };
RunWhere run_where(std::int64_t run_id);
std::string short_test_name(const std::string& n);
const char* matrix_status_str(TestStatus s);
// Status as the summary JSON spells it (PASS / FAIL / SKIP / ERROR).
inline const char* summary_status(TestStatus s) {
    switch (s) {
        case TestStatus::Passed:  return "PASS";
        case TestStatus::Failed:  return "FAIL";
        case TestStatus::Skipped: return "SKIP";
        default:                  return "ERROR";
    }
}

HostPinFlags host_pin_flags_from(const GlobalOptions& global) {
    return HostPinFlags{global.host_claim, global.host_assign_new};
}

// Pin this machine; returns nullopt on ambiguity or error (message on err).
// Read the "vNNN.NNN" literal from a package's version() source file (the same file version-check
// treats as authoritative). Empty if the file/line can't be read.
std::string read_version_literal(const fs::path& file) {
    std::ifstream in(file);
    std::string line;
    while (std::getline(in, line)) {
        if (line.find("version()") == std::string::npos) continue;   // the constexpr version() line
        const auto q1 = line.find('"'); if (q1 == std::string::npos) continue;
        const auto q2 = line.find('"', q1 + 1); if (q2 == std::string::npos) continue;
        return line.substr(q1 + 1, q2 - q1 - 1);
    }
    return {};
}

// Pin each package's code version() into results.db for THIS machine, sourced from the
// version() literals — so every tool (store_bench included, which can't reach jText's header from
// its TU) reads current versions straight from the DB. Paths match version-check's version files.
void write_package_versions(jac313::Qlite::v001::Sqlite& db, std::int64_t mid,
                            const fs::path& src, const std::string& version) {
    struct Pkg { const char* name; const char* file; };
    static const Pkg pkgs[] = {
        {"Store", "Store/include/jac313/Store/v001/headers/ts_store_config.hpp"},
        {"Qlite", "jText/Qlite/include/jac313/Qlite/v001/Database.body.hpp"},
        {"jText", "jText/jText.api.inc"},
    };
    for (const auto& p : pkgs) {
        const std::string v = read_version_literal(src / p.file);
        if (!v.empty()) jac313::results::upsert_package_version(db, mid, version, p.name, v);
    }
}

int run_report_command(const fs::path& source_dir);

std::optional<std::int64_t> pin_current_host(jac313::Qlite::v001::Sqlite& db, const GlobalOptions& global,
                                               std::ostream& err = std::cerr) {
    const auto hw = collect_host_hardware_record(machine_disk_tier("."));
    const auto mid = try_resolve_and_pin(db, hw, host_pin_flags_from(global), harness_version(global), err,
                                         global.source_dir);
    if (mid) write_package_versions(db, *mid, global.source_dir, harness_version(global));   // refresh code versions (per machine+world)
    // a renumber (the shared server's jac313-###) relabels results.db at once; re-render this
    // version's report pages now so none keeps the old label (the other version's pages follow
    // on its next run). Once per process: the flag stays set after the first renumber.
    static bool reported = false;
    if (mid && fleet_renumbered() && !reported) {
        reported = true;
        err << "[fleet] re-rendering the report pages with the new jac313-### labels\n";
        run_report_command(global.source_dir);
    }
    return mid;
}

// "jac313-### · compiler · disk" — the host/compiler/disk line shown above each summary's pass/fail.
// compiler comes from the build dir (build-gcc15 -> gcc15); jac313-### is resolved against the machine table.
inline std::string summary_context_line(const GlobalOptions& global) {
    const auto hw = collect_host_hardware_record(machine_disk_tier("."));
    std::string cc = global.build_dir.filename().string();
    if (cc.rfind("build-", 0) == 0) cc = cc.substr(6);
    std::string label = "jac313-???";
    try {
        jac313::Qlite::v001::Sqlite db(JAC313_RESULTS_DB);
        jac313::results::ensure_schema(db);
        const auto propose = propose_host_machine(db, hw);
        if (propose.error == jac313::results::HostPinError::None && propose.mid > 0) {
            label = jac313::results::host_label(propose.mid);
        }
    } catch (...) {}
    return label + "  ·  " + cc + "  ·  " + (hw.disk_type_label.empty() ? "?" : hw.disk_type_label);
}

int run_tests_command(const GlobalOptions& global, const RunOptions& opts,
                      const std::optional<fs::path>& report_path)
{
    auto tests = discover_tests(global.build_dir);
    if (tests.empty()) {
        std::cerr << "No tests found under " << global.build_dir << '\n';
        std::cerr << "Hint: jac313_test_cli configure && jac313_test_cli build\n";
        return 1;
    }
    seed_test_catalog(global.source_dir, tests);   // record the discovered names in testList

    if (opts.list_only) {
        return run_list_command(global);
    }

    std::cout << "=== jac313 test run ===\n";
    std::cout << "Build dir: " << global.build_dir << "\n";
    if (!opts.filter.empty()) {
        std::cout << "Filter:    " << opts.filter << "\n";
    }
    std::cout << '\n';

    RunOptions run_opts = opts;   // the runner seeds testControl from <source>/tests/test_limits.jtext
    run_opts.source_dir = global.source_dir;
    const auto results = run_tests(tests, run_opts, global.verbose);
    if (results.empty()) {
        return 1;
    }
    const auto summary = summarize(results);
    print_summary(summary, results, summary_context_line(global));

    const std::int64_t ctest_run = record_ctest_results(global, global.build_dir, results);   // capture into results.db
    if (!global.summary_json.empty()) {
        const RunWhere where = run_where(ctest_run);
        const std::string cc = compiler_label_from_build(global.build_dir);
        for (const auto& r : results) {
            append_summary_part(global, std::format(
                R"({{"gate":"ctest","test":"{}","backend":"-","status":"{}","ms":{},"compiler":"{}","run_id":{},"host":"{}","os":"{}"}})",
                json_escape(short_test_name(r.entry.name)), summary_status(r.status),
                r.duration.count(), json_escape(cc), ctest_run, json_escape(where.host), json_escape(where.os)));
        }
    }
    write_ctest_logs(global, compiler_label_from_build(global.build_dir), results);

    if (report_path) {
        if (write_summary_file(*report_path, summary, results)) {
            std::cout << "\nWrote report: " << report_path->string() << '\n';
        } else {
            std::cerr << "Failed to write report: " << report_path->string() << '\n';
            return 1;
        }
    }

    if (summary.failed > 0 || summary.errors > 0) {
        return 1;
    }
    return 0;
}

bool apply_compiler_resolution(GlobalOptions& global,
                               ConfigureOptions& configure_opts,
                               MatrixOptions& matrix_opts)
{
    try {
        const auto probes = probe_compilers(global.source_dir);
        print_compiler_probe(probes);
        write_filecheck_list(global.source_dir, probes);

        CompilerResolveRequest request;
        request.source_dir = global.source_dir;
        request.explicit_compiler = configure_opts.compiler;
        request.prefer_gcc = configure_opts.prefer_gcc;
        request.prefer_clang = configure_opts.prefer_clang;

        const CompilerSelection selection = resolve_compiler(request);
        // --contracts: a mode this toolchain has on this OS (Setup/contracts.jtext)
        const std::string os_family = jac313::Setup::v001::sense_host().os_family;
        const auto modes = contract_modes(global.source_dir, os_family, selection.label);
        if (std::find(modes.begin(), modes.end(), configure_opts.contracts) == modes.end()) {
            std::string list;
            for (const auto& m : modes) list += (list.empty() ? "" : ", ") + m;
            std::cerr << "--contracts " << configure_opts.contracts << ": " << selection.label << " on "
                      << os_family << " has " << list << " (Setup/contracts.jtext)\n";
            return false;
        }
        configure_opts.resolved_compiler = selection.cc_path;
        configure_opts.compiler_kind = selection.kind;
        configure_opts.scan_deps_candidates = selection.scan_deps;
        matrix_opts.compiler_label = selection.label;

        // Activate the toolchain env (gcc-toolset / scl / source-script) for every
        // child process: configure, build, ctest, and each matrix test binary.
        apply_env_delta(selection.env_delta);

        std::cout << "Selected: " << selection.label << " (" << selection.reason << ")\n";
        std::cout << "Compiler: " << selection.cc_path;
        if (!selection.activation.empty() && selection.activation != "{CMD}") {
            std::cout << "  [via " << selection.activation << "]";
        }
        std::cout << '\n';

        if (global.build_dir == "build") {
            global.build_dir = selection.build_dir;
            std::cout << "Build dir:  " << global.build_dir.string() << " (auto)\n";
        }
        std::cout << '\n';
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Compiler resolution failed: " << ex.what() << '\n';
        return false;
    }
}

// "local overrides: <file> (N rows)" for each of `tracked` whose machine-local file applies.
void print_local_overrides(const std::vector<fs::path>& tracked) {
    for (const auto& t : tracked)
        if (const std::string note = local_override_note(t); !note.empty()) std::cout << note << '\n';
}

int run_compilers_command(const GlobalOptions& global, const ConfigureOptions& configure_opts) {
    print_local_overrides({compilers_table_path(global.source_dir)});
    const auto probes = probe_compilers(global.source_dir);
    print_compiler_probe(probes);
    write_filecheck_list(global.source_dir, probes);
    std::cout << '\n';

    const auto demo = [&](const char* heading, CompilerResolveRequest req) {
        req.source_dir = global.source_dir;
        try {
            const CompilerSelection sel = resolve_compiler(req);
            std::cout << heading << sel.label << " (" << sel.reason << ")\n";
        } catch (const std::exception& ex) {
            std::cout << heading << "(none) — " << ex.what() << '\n';
        }
    };

    demo("Auto-select:       ", {});
    CompilerResolveRequest clang_req;
    clang_req.prefer_clang = true;
    demo("With --clang:       ", clang_req);

    if (configure_opts.compiler) {
        CompilerResolveRequest explicit_req;
        explicit_req.source_dir = global.source_dir;
        explicit_req.explicit_compiler = configure_opts.compiler;
        try {
            const CompilerSelection explicit_sel = resolve_compiler(explicit_req);
            std::cout << "With --compiler:    " << explicit_sel.label
                      << " (" << explicit_sel.reason << ")\n";
        } catch (const std::exception& ex) {
            std::cout << "With --compiler:    (none) — " << ex.what() << '\n';
            return 1;
        }
    }

    return 0;
}

int run_setup_command(const GlobalOptions& global) {
    namespace setup = jac313::Setup::v001;

    print_local_overrides({compilers_table_path(global.source_dir), recipes_table_path(global.source_dir)});
    const auto resolved = probe_compilers(global.source_dir);
    print_compiler_probe(resolved);
    write_filecheck_list(global.source_dir, resolved);

    const setup::HostSnapshot host = setup::sense_host();
    const auto recipes = load_recipes(global.source_dir);
    const setup::ReadinessReport rd = setup::assess_readiness(resolved, host.os_family, recipes);
    const setup::ProvisionPlan plan = setup::plan_provision(resolved, host.os_family, recipes);

    std::cout << "\nPlatform: " << host.os_family << "\n";
    std::cout << "Readiness: " << (rd.ready ? "READY" : "NOT READY");
    if (!rd.ready) {
        std::cout << " — missing required: ";
        for (std::size_t i = 0; i < rd.missing.size(); ++i) {
            std::cout << (i ? ", " : "") << rd.missing[i];
        }
    }
    std::cout << "\n";

    if (plan.steps.empty() && plan.unsupported.empty()) {
        std::cout << "Nothing to provision — prerequisites satisfied.\n";
        return 0;
    }

    std::cout << "Not installed yet:";
    for (const auto& step : plan.steps) {
        std::cout << " " << step.component;
    }
    std::cout << " (install commands: Setup/recipes.jtext)\n";
    for (const std::string& comp : plan.unsupported) {
        std::cout << "  note: '" << comp << "' is needed but has no recipe for family '"
                  << host.os_family << "'.\n";
    }
    return 0;
}

// Distinct exit code for a configure/build-phase failure (vs 1 = a test failure,
// 0 = ok). A build failure means the combo's tests were never run — run-all uses
// this to label the failure and suggest a retry (build crashes are often a
// transient toolchain issue, e.g. clang-scan-deps).
constexpr int kBuildPhaseFailed = 2;

// ---- results.db (CLI side): capture functional smoke results as testRun rows ----
// Schema + lookup live in jac313_results_db.hpp (shared with store_bench, single source of truth).
std::int64_t cli_results_id(jac313::Qlite::v001::Sqlite& db, const char* sql, const std::string& name) {
    return jac313::results::lookup_id(db, sql, name);
}

// How a tree was built, as results.db parameter records it: CMAKE_BUILD_TYPE alone, the front end,
// and JAC313_CONTRACTS / JAC313_STRICT / JAC313_HARDENED, all from the tree's CMakeCache.
struct BuildAxes { std::string build_type, modules, import_std, contracts, strict, hardened; };
BuildAxes build_axes(const BuildFeatures& bf, const std::string& modules_override = "",
                     const std::string& import_std_override = "") {
    return {bf.cmake_build_type, modules_override.empty() ? (bf.modules ? "on" : "off") : modules_override,
            import_std_override.empty() ? "off" : import_std_override, bf.contracts,
            bf.strict ? "on" : "off", bf.hardened ? "on" : "off"};
}

// Insert-or-find a parameter row: the build axes plus the test's own (size, persist, output_mode,
// threads, events, runs, valgrind tool, persistence batch; '' / 0 where they do not apply). flag_count
// is the bench's (0 here). SELECT-then-INSERT.
std::int64_t cli_parameter_id(jac313::Qlite::v001::Sqlite& db, std::int64_t compiler_id, const BuildAxes& b,
        const std::string& size = "", const std::string& persist = "", const std::string& output_mode = "",
        std::int64_t threads = 0, std::int64_t events = 0, std::int64_t runs = 0, const std::string& tool = "",
        std::int64_t batch = 0) {
    const char* where =
        "SELECT id FROM parameter WHERE compiler_id=? AND build_type=? AND modules=? AND import_std=? AND size=? "
        "AND persist=? AND output_mode=? AND threads=? AND events_per_thread=? AND runs=? "
        "AND batch=? AND flag_count=0 AND valgrind_tool=? AND contracts=? AND strict=? AND hardened=?";
    auto find = [&]() -> std::int64_t {
        std::int64_t id = 0; auto st = db.prepare(where);
        st.bind(compiler_id, b.build_type, b.modules, b.import_std, size, persist, output_mode, threads, events, runs,
                batch, tool, b.contracts, b.strict, b.hardened);
        if (st.step()) st.get(id);
        return id;
    };
    std::int64_t id = find();
    if (id == 0) {
        db.exec("INSERT INTO parameter(compiler_id,build_type,modules,import_std,size,persist,output_mode,threads,"
                "events_per_thread,runs,batch,flag_count,valgrind_tool,contracts,strict,hardened) "
                "VALUES(?,?,?,?,?,?,?,?,?,?,?,0,?,?,?,?)",
                compiler_id, b.build_type, b.modules, b.import_std, size, persist, output_mode, threads, events, runs,
                batch, tool, b.contracts, b.strict, b.hardened);
        id = find();
    }
    return id;
}

const char* matrix_status_str(TestStatus s) {
    switch (s) {
        case TestStatus::Passed:  return "pass";
        case TestStatus::Failed:  return "fail";
        case TestStatus::Skipped: return "skip";
        default:                  return "error";
    }
}

// Harness version (v001/v002/...) is derived from global.source_dir — see harness_version() in options.hpp.

// Ensure schema, resolve this machine's number (the hardware, matching store_bench) and the OS it runs
// under (sensed now), create a fresh run row with both, and return its run_id. Shared by the smoke/ctest/verify recorders. Uses the shared helpers.
std::int64_t cli_begin_run(jac313::Qlite::v001::Sqlite& db, const GlobalOptions& global) {
    jac313::results::ensure_schema(db);
    const auto mid = pin_current_host(db, global);
    if (!mid) {
        throw std::runtime_error("host pin required before recording (run `host` or use --claim / --assign-new-###)");
    }
    // Do NOT delete the prior run here. Each gate (ctest/smoke/verify/build-times) records its OWN run
    // and safeness SUMS across all of a machine's runs, so "delete the prior run" would drop the sibling
    // gate that ran just before this one — which is what emptied the run-everything verdict. The
    // explicit fresh-start (machine wipe) belongs to run-everything's top-of-run cleanup and the
    // jac313_wipe_* tools, NOT to every per-gate recorder.
    const std::int64_t run_id   = jac313::results::next_run_id(db);
    jac313::results::insert_run(db, run_id, jac313::results::current_os_id(db), *mid, harness_version(global));
    // The fleet registry server, when pinning reached it (run_target role registry).
    if (const auto reg = fleet_registry_target()) jac313::results::insert_run_targets(db, run_id, {*reg});
    return run_id;
}

// Read the tree's CMAKE_CXX_COMPILER and run it for its EXACT version (name + X.Y.Z + major), so a
// functional result ties to the precise toolchain — same granularity store_bench records.
jac313::results::CompilerInfo read_compiler_info(const fs::path& build_dir) {
    jac313::results::CompilerInfo c{"gcc", "0", 0};
    std::string cxx;
    { std::ifstream in(build_dir / "CMakeCache.txt"); std::string line;
      while (std::getline(in, line))
          if (line.rfind("CMAKE_CXX_COMPILER:", 0) == 0) { cxx = line.substr(line.find('=') + 1); break; } }
    if (cxx.empty()) return c;
    const auto r = run_process({cxx, "--version"}, 0, "");
    const std::string out = r.stdout_text + r.stderr_text;
    c.name = (out.find("clang") != std::string::npos) ? "clang" : "gcc";
    for (std::size_t i = 0; i < out.size(); ++i) {     // first X.Y[.Z] token = the version
        if (out[i] >= '0' && out[i] <= '9' && (i == 0 || out[i - 1] < '0' || out[i - 1] > '9')) {
            std::size_t j = i;
            while (j < out.size() && ((out[j] >= '0' && out[j] <= '9') || out[j] == '.')) ++j;
            const std::string tok = out.substr(i, j - i);
            if (tok.find('.') != std::string::npos) { c.version = tok; break; }
        }
    }
    c.major = std::atoi(c.version.c_str());
    return c;
}

std::int64_t cli_compiler_id(jac313::Qlite::v001::Sqlite& db, const jac313::results::CompilerInfo& c) {
    return jac313::results::compiler_id(db, c);
}

// ============================================================================
// results.db report: a compiler inventory page + per-test-type summary + per-run detail pages,
// under test-summary/{compiler,ctest,smoke,verify-lite,verify}/. Generated by `jac313_test_cli --report`.
// ============================================================================
std::string comp_label(const std::string& name, std::int64_t major) { return name + std::to_string(major); }
std::string frontend_label(const std::string& modules, const std::string& import_std) {
    (void)import_std;   // the column stays in results.db; every build is headers or modules
    return (modules == "on") ? "modules" : "headers";
}
std::string run_label_md(std::int64_t run_id) {
    char b[24]; std::snprintf(b, sizeof b, "Run_%03lld", static_cast<long long>(run_id)); return b;
}
std::string dash(const std::string& s) { return s.empty() ? "-" : s; }

// "jac313-002 — Intel(R) Core(TM) i7 · 8 cores · 31 GB · xssd": a machine section heading (the
// hardware; the OS is per run, see each run's header and the machines page).
std::string machine_heading(jac313::Qlite::v001::Sqlite& db, std::int64_t mid) {
    std::string cpu, disk; std::int64_t cores = 0, ram = 0;
    { auto st = db.prepare("SELECT cpu, cores, ram_gb, disk FROM machine WHERE id=? LIMIT 1");
      st.bind(mid); if (st.step()) st.get(cpu, cores, ram, disk); }
    return jac313::results::host_label(mid) + " — " + dash(cpu) + " · " + std::to_string(cores) + " cores · "
         + std::to_string(ram) + " GB" + (disk.empty() ? std::string() : " · " + disk);
}

// SQL for an OS release as a column suffix ("fedora 44"), over the os alias o.
constexpr const char* kOsReleaseSql = "o.family || ' ' || o.version";

// " · fedora 44" when a machine's runs of one kind span more than one OS release, else "": the
// compiler columns of a page then show which OS each ran under.
std::string os_suffix(bool multi_os, const std::string& release) {
    return multi_os ? " · " + release : std::string();
}
// " [observe]" after a row key or compiler when a run's contract mode is not enforce (the default).
std::string contracts_tag(const std::string& mode) { return mode == "enforce" ? std::string() : " [" + mode + "]"; }
// "Release", "Debug · strict · hardened": the build type and the strict / hardened options that were on.
std::string build_label(const std::string& build_type, const std::string& strict, const std::string& hardened) {
    return dash(build_type) + (strict == "on" ? " · strict" : "") + (hardened == "on" ? " · hardened" : "");
}

// The database backend of a persist value ("pg-remote", "pg-remote/cap=10", "jtext+pg-remote/nobinary"
// -> "pg-remote"); "" when it is not a database.
std::string database_backend(const std::string& persist) {
    std::string b = persist.substr(0, persist.find('/'));
    if (const auto plus = b.find('+'); plus != std::string::npos) b = b.substr(plus + 1);
    return (b == "sql" || b == "pg-local" || b == "pg-remote") ? b : std::string();
}

// " (r430)" after a database backend: the server run `run_id` wrote it to (run_target), so runs against
// different servers are different rows. "" for a file, SQLite (no server) or an unrecorded run.
std::string server_tag(jac313::Qlite::v001::Sqlite& db, std::int64_t run_id, const std::string& persist) {
    const std::string backend = database_backend(persist);
    if (backend.empty()) return {};
    // IFNULL kept: GROUP_CONCAT over no rows is NULL.
    auto st = db.prepare("SELECT IFNULL(GROUP_CONCAT(DISTINCT server), '') FROM run_target "
                         "WHERE run_id=? AND backend=? AND role<>'registry' AND server<>''");
    st.bind(run_id, backend);
    std::string servers;
    if (st.step()) st.get(servers);
    return servers.empty() ? std::string() : " (" + servers + ")";
}

// "| role | backend | server | address | database | table / file |": what run `run_id` wrote where.
void write_targets_section(jac313::Qlite::v001::Sqlite& db, std::ostream& md, std::int64_t run_id) {
    auto st = db.prepare("SELECT role, backend, server, address, port, dbname, table_name FROM run_target "
                         "WHERE run_id=? ORDER BY role='registry', backend, dbname, table_name");
    st.bind(run_id);
    bool any = false;
    while (st.step()) {
        std::string role, backend, server, address, dbname, table; std::int64_t port = 0;
        st.get(role, backend, server, address, port, dbname, table);
        if (!any) {
            md << "\n## Written to\n\n_What this run wrote where (`run_target`); the servers page lists every "
                  "server: [servers](../servers/README.md)._\n\n"
                  "| role | backend | server | address | database | table / file |\n|---|---|---|---|---|---|\n";
            any = true;
        }
        md << "| " << role << " | " << backend << " | " << dash(server) << " | "
           << (address.empty() ? std::string("-") : address + (port ? ":" + std::to_string(port) : std::string()))
           << " | " << dash(dbname) << " | " << dash(table) << " |\n";
    }
}

// Page navigation. `back` = relative path to the parent index ("../README.md" for an area page,
// "README.md" for a run-detail page). nav_top sets the #top anchor + a back / jump-to-bottom line;
// nav_bottom sets the #bottom anchor + a back / jump-to-top line. Anchors are GitHub-compatible.
void nav_top(std::ostream& md, const std::string& back) {
    md << "<a id=\"top\"></a>\n[← back](" << back << ") · [↓ bottom](#bottom)\n\n";
}
void nav_bottom(std::ostream& md, const std::string& back) {
    md << "\n---\n[← back](" << back << ") · [↑ top](#top)\n<a id=\"bottom\"></a>\n";
}

// Locale-aware integer formatting — thousands separators per the locale of whoever runs --report
// ("6,030,096" en_US, "6.030.096" de_DE). Falls back to plain digits if the locale is unavailable.
std::string fmt_num(std::int64_t n) {
    try {
        std::ostringstream ss;
        ss.imbue(std::locale(""));
        ss << n;
        return ss.str();
    } catch (...) { return std::to_string(n); }
}

// Human-readable count, 2 decimals: 6,251,809 -> "6.25M", 5,515 -> "5.52K", small stays as-is.
std::string human_ops(std::int64_t n) {
    if (n <= 0) return "0";
    double v = static_cast<double>(n);
    const char* suf = "";
    if (n >= 1000000000)   { v = n / 1e9; suf = "G"; }
    else if (n >= 1000000) { v = n / 1e6; suf = "M"; }
    else if (n >= 1000)    { v = n / 1e3; suf = "K"; }
    else return std::to_string(n);
    char b[32]; std::snprintf(b, sizeof b, "%.2f%s", v, suf);
    return b;
}

// Human-readable byte size: "0 B", "1.23 KB", "12.35 MB", "1.50 GB" (1024-based, 2 decimals).
std::string human_bytes(std::int64_t b) {
    if (b <= 0) return "0 B";
    static const char* unit[] = {"B", "KB", "MB", "GB", "TB"};
    double v = static_cast<double>(b);
    int i = 0;
    while (v >= 1024.0 && i < 4) { v /= 1024.0; ++i; }
    char buf[32];
    if (i == 0) std::snprintf(buf, sizeof buf, "%lld B", static_cast<long long>(b));
    else        std::snprintf(buf, sizeof buf, "%.2f %s", v, unit[i]);
    return buf;
}

// Build/compile wall-clock as locale-grouped seconds, 2 decimals — same locale mechanism as
// fmt_num, so build times group like every other number: 10576 ms -> "10.58s",
// 1234567 ms -> "1,234.57s". Falls back to ungrouped digits if the locale is unavailable.
std::string fmt_build_seconds(std::int64_t ms) {
    const double secs = static_cast<double>(ms) / 1000.0;
    try {
        std::ostringstream ss;
        ss.imbue(std::locale(""));
        ss << std::fixed << std::setprecision(2) << secs;
        return ss.str() + "s";
    } catch (...) {
        char b[32]; std::snprintf(b, sizeof b, "%.2fs", secs); return b;
    }
}

void write_compiler_page(jac313::Qlite::v001::Sqlite& db, const fs::path& out) {
    std::error_code ec; fs::create_directories(out / "compiler", ec);
    std::ofstream md(out / "compiler" / "README.md");
    nav_top(md, "../README.md");
    md << "# Compilers\n\n_Generated from `results.db`._\n\n"
          "| label | name | version | major | builds | contracts | runs |\n|---|---|---|--:|---|---|--:|\n";
    // IFNULL: a compiler with no parameter row yet (LEFT JOIN, no match) has no builds or contract modes.
    // builds: each build type as built, with the strict / hardened options that were on.
    auto st = db.prepare(
        "SELECT c.name, c.version, c.major, "
        "IFNULL(GROUP_CONCAT(DISTINCT CASE WHEN p.build_type = '' THEN NULL ELSE p.build_type "
        "|| CASE WHEN p.strict = 'on' THEN '·strict' ELSE '' END "
        "|| CASE WHEN p.hardened = 'on' THEN '·hardened' ELSE '' END END), ''), "
        "IFNULL(GROUP_CONCAT(DISTINCT p.contracts), ''), "
        "COUNT(DISTINCT tr.run_id) "
        "FROM compiler c LEFT JOIN parameter p ON p.compiler_id=c.id "
        "LEFT JOIN testRun tr ON tr.parameter_id=p.id GROUP BY c.id ORDER BY c.name, c.major, c.version");
    while (st.step()) {
        std::string name, version, builds, modes; std::int64_t major = 0, runs = 0;
        st.get(name, version, major, builds, modes, runs);
        md << "| " << comp_label(name, major) << " | " << name << " | " << version << " | "
           << major << " | " << dash(builds) << " | " << dash(modes) << " | " << runs << " |\n";
    }
    nav_bottom(md, "../README.md");
}

// A run's configuration: what a result column is keyed by. The build (compiler, build type, front
// end, strict, hardened, contracts), the OS release, and the run-wide test settings (size, valgrind
// tools, the persisted-row cap, a read-back mode other than the default 'sample'). Two runs that
// differ in any of these are two columns; a column shows the latest run of its configuration.
struct RunConfig {
    std::int64_t compiler_id = 0;
    std::string comp, release, build_type, modules, strict, hardened, contracts, size;
    std::set<std::string> tools, rb_modes;
    std::int64_t max_rows = 0;
};

// "gcc16 · Debug · hdr · enforce · smoke", then the tools, strict / hardened, "max 2 rows" and a
// read-back mode other than sample only when they apply; " · fedora 44" when the machine ran under
// more than one OS release.
std::string config_label(const RunConfig& c, bool multi_os) {
    std::string s = c.comp + " · " + dash(c.build_type) + " · " + (c.modules == "on" ? "mod" : "hdr") + " · "
                  + dash(c.contracts);
    if (!c.size.empty()) s += " · " + c.size;
    if (!c.tools.empty()) {
        std::string t;
        for (const auto& x : c.tools) t += (t.empty() ? "" : "+") + x;
        s += " · " + t;
    }
    if (c.strict == "on") s += " · strict";
    if (c.hardened == "on") s += " · hardened";
    if (c.max_rows > 0) s += " · max " + fmt_num(c.max_rows) + " rows";
    for (const auto& m : c.rb_modes) if (m != "sample") s += " · read-back " + m;
    return s + os_suffix(multi_os, c.release);
}

// The configuration of every `type` run on machine `mid`, newest run first, and whether those runs
// span more than one OS release.
std::vector<std::pair<std::int64_t, RunConfig>> run_configs(jac313::Qlite::v001::Sqlite& db, const std::string& type,
                                                            std::int64_t mid, bool& multi_os) {
    std::vector<std::pair<std::int64_t, RunConfig>> runs;
    std::set<std::string> releases;
    auto st = db.prepare(std::string("SELECT tr.run_id, c.id, c.name, c.major, ") + kOsReleaseSql + ", "
        "p.build_type, p.modules, p.strict, p.hardened, p.contracts, p.size, p.valgrind_tool, tr.max_persist_rows, "
        "tr.readback_mode FROM testRun tr JOIN run r ON r.run_id=tr.run_id JOIN os o ON o.id=r.os_id "
        "JOIN parameter p ON p.id=tr.parameter_id JOIN compiler c ON c.id=p.compiler_id "
        "JOIN testType tt ON tt.id=tr.test_type_id WHERE tt.name=? AND r.machine_id=? ORDER BY tr.run_id DESC");
    st.bind(type, mid);
    while (st.step()) {
        std::int64_t run = 0, cid = 0, major = 0, max_rows = 0;
        std::string name, release, bt, mod, strict, hardened, contracts, size, tool, rb;
        st.get(run, cid, name, major, release, bt, mod, strict, hardened, contracts, size, tool, max_rows, rb);
        if (runs.empty() || runs.back().first != run) {
            RunConfig c;
            c.compiler_id = cid; c.comp = comp_label(name, major); c.release = release; c.build_type = bt;
            c.modules = mod; c.strict = strict; c.hardened = hardened; c.contracts = contracts;
            runs.push_back({run, c});
        }
        RunConfig& c = runs.back().second;
        if (!size.empty()) c.size = size;
        if (!tool.empty()) c.tools.insert(tool);
        if (!rb.empty()) c.rb_modes.insert(rb);
        c.max_rows = std::max(c.max_rows, max_rows);
        releases.insert(release);
    }
    multi_os = releases.size() > 1;
    return runs;
}

// The configuration columns of a page: one per distinct configuration (label + compiler row),
// linked to the latest run of it, in label order. `col_of` maps every run to its column.
struct ConfigColumns {
    std::vector<std::pair<std::string, std::int64_t>> cols;   // label -> latest run
    std::map<std::int64_t, int> col_of;                      // run -> column
};
ConfigColumns config_columns(const std::vector<std::pair<std::int64_t, RunConfig>>& runs, bool multi_os) {
    std::map<std::pair<std::string, std::int64_t>, std::int64_t> latest;   // (label, compiler id) -> newest run
    for (const auto& [run, c] : runs) {
        const auto key = std::make_pair(config_label(c, multi_os), c.compiler_id);
        if (!latest.count(key)) latest[key] = run;   // runs come newest first
    }
    ConfigColumns cc;
    std::map<std::pair<std::string, std::int64_t>, int> idx;
    for (const auto& [key, run] : latest) { idx[key] = static_cast<int>(cc.cols.size()); cc.cols.push_back({key.first, run}); }
    for (const auto& [run, c] : runs) cc.col_of[run] = idx[std::make_pair(config_label(c, multi_os), c.compiler_id)];
    return cc;
}

// "sql, pg-remote (r430), jtext": what run `run_id` wrote to (run_target, the registry excepted); ""
// when it wrote nothing.
std::string written_to(jac313::Qlite::v001::Sqlite& db, std::int64_t run_id) {
    auto st = db.prepare("SELECT DISTINCT backend, server FROM run_target WHERE run_id=? AND role<>'registry' "
                         "ORDER BY backend, server");
    st.bind(run_id);
    std::string s;
    while (st.step()) {
        std::string backend, server; st.get(backend, server);
        s += (s.empty() ? "" : ", ") + backend + (server.empty() ? std::string() : " (" + server + ")");
    }
    return s;
}

// "## Settings": every value recorded for run `run_id` — each parameter column and the run-wide
// testRun settings, as the distinct values over the run's rows ("-" when none applies) — then a
// per-test table of the testRun extras (persisted rows, read-back, PostgreSQL timings, production
// hot path and drains, batch per sink, bytes) for the rows that have any.
void write_settings_section(jac313::Qlite::v001::Sqlite& db, std::ostream& md, std::int64_t run_id) {
    struct Setting { const char* label; const char* expr; };
    static const Setting settings[] = {
        {"compiler", "c.name || ' ' || c.version"}, {"build type", "p.build_type"}, {"front end", "p.modules"},
        {"import std", "p.import_std"}, {"strict", "p.strict"}, {"hardened", "p.hardened"},
        {"contracts", "p.contracts"}, {"size", "p.size"}, {"persist", "p.persist"}, {"output mode", "p.output_mode"},
        {"threads", "p.threads"}, {"events per thread", "p.events_per_thread"}, {"runs", "p.runs"},
        {"batch", "p.batch"}, {"flag count", "p.flag_count"}, {"valgrind tool", "p.valgrind_tool"},
        {"max persisted rows", "tr.max_persist_rows"}, {"read-back mode", "tr.readback_mode"},
        {"batch keeper", "tr.batch_keeper"}, {"batch database", "tr.batch_database"},
        {"batch binary", "tr.batch_binary"}};
    md << "\n## Settings\n\n_Every value recorded for this run (several values: they differ by test)._\n\n"
          "| setting | value |\n|---|---|\n";
    for (const Setting& s : settings) {
        auto st = db.prepare(std::string("SELECT CAST(") + s.expr + " AS TEXT) FROM testRun tr "
            "JOIN parameter p ON p.id=tr.parameter_id JOIN compiler c ON c.id=p.compiler_id "
            "WHERE tr.run_id=? GROUP BY " + s.expr + " ORDER BY " + s.expr);
        st.bind(run_id);
        std::string v;
        while (st.step()) {
            std::string x; st.get(x);
            if (x.empty() || x == "0") continue;
            if (std::string(s.label) == "front end") x = frontend_label(x, "");
            v += (v.empty() ? "" : ", ") + x;
        }
        md << "| " << s.label << " | " << dash(v) << " |\n";
    }
    // IFNULL kept: tl.name is NULL when the LEFT JOIN finds no testList row.
    auto st = db.prepare(
        "SELECT IFNULL(tl.name, ''), p.persist || CASE WHEN p.output_mode <> '' THEN ' · ' || p.output_mode ELSE '' END, "
        "tr.max_persist_rows, tr.rows_persisted, tr.readback_mode, tr.readback_status, "
        "tr.rows_checked, printf('%.1f', tr.read_ms), printf('%.1f', tr.pg_connect_ms), printf('%.2f', tr.pg_ping_ms), "
        "printf('%.1f', tr.pg_write_ms), tr.hot_ops, tr.drain_binary_ms, tr.drain_keeper_ms, tr.drain_database_ms, "
        "tr.batch_keeper, tr.batch_database, tr.batch_binary, tr.bytes, "
        "tr.pg_connect_ms + tr.pg_ping_ms + tr.pg_write_ms > 0 "
        "FROM testRun tr LEFT JOIN testList tl ON tl.id=tr.test_list_id JOIN parameter p ON p.id=tr.parameter_id "
        "WHERE tr.run_id=? AND (tr.max_persist_rows + tr.rows_persisted + tr.rows_checked + tr.hot_ops "
        "+ tr.batch_keeper + tr.batch_database + tr.batch_binary + tr.bytes > 0 OR tr.readback_mode <> '' "
        "OR tr.read_ms + tr.pg_connect_ms + tr.pg_ping_ms + tr.pg_write_ms > 0) ORDER BY 1, 2");
    st.bind(run_id);
    bool any = false;
    auto n0 = [](std::int64_t n) { return n ? fmt_num(n) : std::string("-"); };
    while (st.step()) {
        std::string test, per, rb_mode, rb_status, read_ms, pg_c, pg_p, pg_w;
        std::int64_t cap = 0, persisted = 0, checked = 0, hot = 0, dbin = 0, dkeep = 0, ddb = 0, bk = 0, bd = 0, bb = 0,
                     bytes = 0, pg = 0;
        st.get(test, per, cap, persisted, rb_mode, rb_status, checked, read_ms, pg_c, pg_p, pg_w, hot, dbin, dkeep, ddb,
               bk, bd, bb, bytes, pg);
        if (!any) {
            md << "\n**Per test** (rows = persisted / cap; read-back = mode status · rows checked · ms; pg = "
                  "connect / ping / write ms; drain = binary / keeper / database ms; batch = keeper / database / "
                  "binary):\n\n| test | persist · mode | rows | read-back | pg | hot ops/sec | drain | batch | bytes |\n"
                  "|---|---|--:|---|---|--:|---|---|--:|\n";
            any = true;
        }
        const std::string rows = (persisted || cap) ? n0(persisted) + " / " + n0(cap) : std::string("-");
        const std::string rb = rb_mode.empty() ? std::string("-")
                             : rb_mode + " " + rb_status + " · " + fmt_num(checked) + " · " + read_ms;
        const bool prod = hot || dbin || dkeep || ddb;
        md << "| " << dash(test) << " | " << dash(per) << " | " << rows << " | " << rb << " | "
           << (pg ? pg_c + " / " + pg_p + " / " + pg_w : std::string("-")) << " | " << n0(hot) << " | "
           << (prod ? n0(dbin) + " / " + n0(dkeep) + " / " + n0(ddb) : std::string("-")) << " | "
           << ((bk || bd || bb) ? n0(bk) + " / " + n0(bd) + " / " + n0(bb) : std::string("-")) << " | "
           << (bytes ? human_bytes(bytes) : std::string("-")) << " |\n";
    }
}

// The run's header line: machine · OS · kernel · compiler version · configuration · recorded.
void write_run_header(jac313::Qlite::v001::Sqlite& db, std::ostream& det, std::int64_t run_id) {
    auto st = db.prepare(
        "SELECT c.name, c.version, p.build_type, p.strict, p.hardened, p.modules, p.import_std, p.size, "
        "p.contracts, r.ts_utc "
        "FROM testRun tr JOIN run r ON r.run_id=tr.run_id JOIN parameter p ON p.id=tr.parameter_id "
        "JOIN compiler c ON c.id=p.compiler_id WHERE tr.run_id=? ORDER BY p.size DESC LIMIT 1");
    st.bind(run_id);
    if (st.step()) { std::string name, version, bt, strict, hardened, mod, imp, size, contracts, ts;
        st.get(name, version, bt, strict, hardened, mod, imp, size, contracts, ts);
        det << "_" << jac313::results::run_header(db, run_id) << " · " << name << " " << version << " · "
            << build_label(bt, strict, hardened) << " · " << frontend_label(mod, imp) << " · contracts " << contracts
            << (size.empty() ? "" : (" · " + size)) << " · " << ts << "_\n\n"; }
}

// Per-run detail page (ctest / smoke / verify): every test with persist/mode/tool + status + ms,
// then the run's settings and what it wrote where.
void write_run_detail(jac313::Qlite::v001::Sqlite& db, const fs::path& dir, const std::string& type, std::int64_t run_id) {
    const std::string rl = run_label_md(run_id);
    std::ofstream det(dir / (rl + ".md"));
    nav_top(det, "README.md");
    det << "# " << type << " — " << rl << "\n\n";
    write_run_header(db, det, run_id);
    det << "| test | persist | mode | tool | status | ms |\n|---|---|---|---|---|--:|\n";
    auto st = db.prepare(
        "SELECT tl.name, p.persist, p.output_mode, p.valgrind_tool, "
        "tr.status, tr.duration_ms FROM testRun tr JOIN testList tl ON tl.id=tr.test_list_id "
        "JOIN parameter p ON p.id=tr.parameter_id JOIN testType tt ON tt.id=tr.test_type_id "
        "WHERE tr.run_id=? AND tt.name=? ORDER BY tl.name");
    st.bind(run_id, type);
    while (st.step()) {
        std::string t, per, mode, tool, status; std::int64_t ms = 0;
        st.get(t, per, mode, tool, status, ms);
        det << "| " << t << " | " << dash(per) << server_tag(db, run_id, per) << " | " << dash(mode) << " | "
            << dash(tool) << " | " << status << " | " << fmt_num(ms) << " |\n";
    }
    write_settings_section(db, det, run_id);
    write_targets_section(db, det, run_id);
    nav_bottom(det, "README.md");
}

// The run history of one machine's section: every run, newest first, each linked to its detail
// page, labelled with its configuration; a "written to" column only when some run wrote anywhere.
void write_run_history(jac313::Qlite::v001::Sqlite& db, std::ostream& md,
                       const std::vector<std::pair<std::int64_t, RunConfig>>& runs, bool multi_os) {
    std::vector<std::string> wrote;
    bool any = false;
    for (const auto& r : runs) { wrote.push_back(written_to(db, r.first)); any = any || !wrote.back().empty(); }
    md << "\n**Runs** (newest first):\n\n| run | configuration · machine · OS · kernel · recorded |"
       << (any ? " written to |\n|---|---|---|\n" : "\n|---|---|\n");
    for (std::size_t i = 0; i < runs.size(); ++i) {
        const std::int64_t run = runs[i].first;
        std::string ts;
        { auto st = db.prepare("SELECT ts_utc FROM run WHERE run_id=?"); st.bind(run); if (st.step()) st.get(ts); }
        md << "| [" << run_label_md(run) << "](" << run_label_md(run) << ".md) | " << config_label(runs[i].second, multi_os)
           << " · " << jac313::results::run_header(db, run) << (ts.empty() ? std::string() : " · " + ts) << " |"
           << (any ? " " + dash(wrote[i]) + " |" : std::string()) << "\n";
    }
}

// Per-type page (ctest / smoke / verify): each machine (jac313-###) is its own section with a
// hardware header and its OWN columns, one per configuration (config_label), each the latest run of
// that configuration. Scenario rows × configuration columns; cell = ms (pass) or status (verify:
// pass · ms), plus "read ok" / "read FAIL" where the test read its data back.
void write_type_pages(jac313::Qlite::v001::Sqlite& db, const fs::path& out, const std::string& type) {
    std::vector<std::int64_t> machines;
    { auto st = db.prepare("SELECT DISTINCT r.machine_id FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
        "JOIN testType tt ON tt.id=tr.test_type_id WHERE tt.name=? ORDER BY r.machine_id");
      st.bind(type); while (st.step()) { std::int64_t m = 0; st.get(m); machines.push_back(m); } }
    if (machines.empty()) return;
    std::error_code ec; fs::create_directories(out / type, ec);
    std::ofstream md(out / type / "README.md");
    nav_top(md, "../README.md");
    md << "# " << type << " — by machine\n\n_Generated from `results.db`. Each machine (`jac313-###`) is its "
          "own section. A column is one configuration (compiler · build type · hdr/mod · contracts · size, then "
          "tools, strict, hardened, a row cap or read-back mode when set) and shows its latest run; older runs "
          "are in the run list. Cell = ms (pass) or status; \"read ok\" / \"read FAIL\" = the test read its "
          "data back. Each run's page lists every setting it recorded._\n";

    const bool is_verify = (type == "verify" || type == "verify-lite");
    for (const std::int64_t g : machines) {
        bool multi_os = false;
        const auto runs = run_configs(db, type, g, multi_os);
        if (runs.empty()) continue;
        const ConfigColumns cc = config_columns(runs, multi_os);
        const auto& comps = cc.cols;

        md << "\n## " << machine_heading(db, g) << "\n\n";

        std::string inlist;
        for (const auto& c : comps) { if (!inlist.empty()) inlist += ","; inlist += std::to_string(c.second); }

        struct Row { std::string key; std::vector<std::string> cells; };
        std::vector<Row> rows;
        { auto st = db.prepare(
            "SELECT tr.run_id, tl.name, p.persist, p.output_mode, p.valgrind_tool, "
            "tr.status, tr.duration_ms, tr.readback_status FROM testRun tr JOIN testList tl ON tl.id=tr.test_list_id "
            "JOIN parameter p ON p.id=tr.parameter_id JOIN testType tt ON tt.id=tr.test_type_id "
            "WHERE tt.name=? AND tr.run_id IN (" + inlist + ") ORDER BY tl.name, p.persist, p.output_mode, p.valgrind_tool");
          st.bind(type);
          while (st.step()) {
              std::int64_t run = 0, ms = 0; std::string t, per, mode, tool, status, rb;
              st.get(run, t, per, mode, tool, status, ms, rb);
              std::string key = t;
              if (!per.empty())  key += " · " + per + server_tag(db, run, per);
              if (!mode.empty()) key += " · " + mode;
              if (!tool.empty()) key += " · " + tool;
              Row* r = nullptr;
              for (auto& rr : rows) if (rr.key == key) { r = &rr; break; }
              if (!r) { rows.push_back({key, std::vector<std::string>(comps.size(), "-")}); r = &rows.back(); }
              const auto it = cc.col_of.find(run);
              if (it == cc.col_of.end()) continue;
              std::string cell = (status == "pass")
                  ? (is_verify ? ("pass · " + fmt_num(ms)) : (ms ? fmt_num(ms) : std::string("pass")))
                  : status;
              if (!rb.empty()) cell += rb == "PASS" ? " · read ok" : " · read " + rb;
              r->cells[static_cast<std::size_t>(it->second)] = cell; } }

        md << "| scenario";
        for (const auto& c : comps) md << " | [" << c.first << "](" << run_label_md(c.second) << ".md)";
        md << " |\n|---";
        for (std::size_t i = 0; i < comps.size(); ++i) md << "|--:";
        md << "|\n";
        for (const auto& r : rows) {
            md << "| " << r.key;
            for (const auto& cell : r.cells) md << " | " << cell;
            md << " |\n";
        }
        write_run_history(db, md, runs, multi_os);
        for (const auto& r : runs) write_run_detail(db, out / type, type, r.first);
    }
    nav_bottom(md, "../README.md");
}

// Bench pages: bench rows carry ops stats (median/band/bytes), not status/duration — its own format.
// Four groups, in this order: the production run (persist = its sink pair, "jtext+sql"), the
// databases alone (sql, pg-local, pg-remote), the files alone (binary, jtext, html, json), and the
// non-durable flag sweep. A capped run's persist carries "/cap=N"; it groups like its backend.
int bench_group(const std::string& persist) {
    const std::string b = persist.substr(0, persist.find('/'));
    if (b.find('+') != std::string::npos) return 0;
    if (b == "sql" || b == "pg-local" || b == "pg-remote") return 1;
    if (b.empty() || b == "none") return 3;
    return 2;
}
const char* const kBenchGroupTitle[] = {"Production", "Databases", "Files", "Non-durable (flag sweep)"};

// The config name when the testList row is missing (LEFT JOIN) or blank: rebuilt from the parameters.
// IFNULL kept: tl.name is NULL when the LEFT JOIN finds no testList row.
const char* const kBenchNameSql =
    "CASE WHEN IFNULL(tl.name, '') <> '' THEN tl.name WHEN p.persist IN ('','none') "
    "THEN p.flag_count || ' flags, non-durable' "
    "WHEN p.persist LIKE '%+%' THEN 'production ' || p.persist ELSE 'durable ' || p.persist END";

// "26 / 26 / 27" — the production run's drain times (binary / keeper / database, ms); binary "off"
// when the run had no replay log (its persist carries "/nobinary").
std::string drain_cell(const std::string& persist, std::int64_t bin, std::int64_t keeper, std::int64_t database) {
    const bool nobin = persist.find("/nobinary") != std::string::npos;
    return (nobin ? std::string("off") : fmt_num(bin)) + " / " + fmt_num(keeper) + " / " + fmt_num(database);
}

// Bench per-run detail (ops format): each config with persist/events + median/band/bytes, grouped
// like the bench page; production rows also show the hot path and the three drain times. Then the
// run's settings (batch, caps, read-back, PostgreSQL timings per config) and what it wrote where.
void write_bench_detail(jac313::Qlite::v001::Sqlite& db, const fs::path& dir, std::int64_t run_id) {
    const std::string rl = run_label_md(run_id);
    std::ofstream det(dir / (rl + ".md"));
    nav_top(det, "README.md");
    det << "# bench — " << rl << "\n\n";
    write_run_header(db, det, run_id);
    det << "_Production rows: hot path = producers until they join (before finalize); drain = attach → "
           "that sink's finalize done, binary / keeper / database; durable total = the whole process "
           "(median ops/sec, as for every row)._\n\n";
    struct Line { int group; std::string text; };
    std::vector<Line> lines;
    auto st = db.prepare(
        std::string("SELECT ") + kBenchNameSql + ", p.persist, p.threads, p.events_per_thread, p.runs, "
        "tr.median_ops, tr.low_ops, tr.high_ops, tr.bytes, tr.hot_ops, tr.drain_binary_ms, tr.drain_keeper_ms, "
        "tr.drain_database_ms "
        "FROM testRun tr LEFT JOIN testList tl ON tl.id=tr.test_list_id JOIN parameter p ON p.id=tr.parameter_id "
        "JOIN testType tt ON tt.id=tr.test_type_id WHERE tr.run_id=? AND tt.name='bench' ORDER BY tr.median_ops DESC");
    st.bind(run_id);
    while (st.step()) {
        std::string name, per; std::int64_t threads = 0, epe = 0, runs = 0, med = 0, low = 0, high = 0, bytes = 0;
        std::int64_t hot = 0, dbin = 0, dkeep = 0, ddb = 0;
        st.get(name, per, threads, epe, runs, med, low, high, bytes, hot, dbin, dkeep, ddb);
        const int g = bench_group(per);
        std::ostringstream l;
        l << "| " << kBenchGroupTitle[g] << " | " << name << " | " << dash(per) << server_tag(db, run_id, per) << " | "
          << fmt_num(threads) << " × " << fmt_num(epe) << " | " << fmt_num(runs) << " | "
          << (g == 0 ? fmt_num(hot) : std::string("-")) << " | "
          << (g == 0 ? drain_cell(per, dbin, dkeep, ddb) : std::string("-")) << " | "
          << fmt_num(med) << " | " << human_ops(low) << "–" << human_ops(high) << " | " << human_bytes(bytes) << " |\n";
        lines.push_back({g, l.str()});
    }
    std::stable_sort(lines.begin(), lines.end(), [](const Line& a, const Line& b) { return a.group < b.group; });
    det << "| group | config | persist | threads × events | runs | hot ops/sec | drain ms<br>binary / keeper / database | "
           "durable median ops/sec | band (low–high) | bytes |\n"
           "|---|---|---|--:|--:|--:|--:|--:|---|--:|\n";
    for (const auto& l : lines) det << l.text;
    write_settings_section(db, det, run_id);
    write_targets_section(db, det, run_id);
    nav_bottom(det, "README.md");
}

// A bench row's name on the bench page: the config name, the server it wrote to, the record cap
// ("[capped]" in the name becomes "[capped 10]", the events the run kept), a size the name does not
// carry ("[10 events]"), and the persisted-row cap ("[max 2 rows]"), so a capped run never shares a
// row with the full-size one.
std::string bench_row_name(std::string name, const std::string& server, std::int64_t events, std::int64_t max_rows) {
    name += server;
    const std::string capped = " [capped]";
    if (const auto at = name.find(capped); at != std::string::npos) {
        name.replace(at, capped.size(), " [capped " + fmt_num(events) + "]");
    } else if (events != 1000000 && events != 10000000 && name.find('@') == std::string::npos
               && name.find("events/thread=") == std::string::npos) {
        name += " [" + fmt_num(events) + " events]";
    }
    if (max_rows > 0) name += " [max " + fmt_num(max_rows) + " rows]";
    return name;
}

// Bench page = a HOST-SCOPED comparison: throughput is hardware-specific, so each machine
// (jac313-###) is its own section with a hardware header (cpu/cores/ram/disk) and its OWN columns,
// one per build configuration (config_label). Groups in order: Production, Databases, Files,
// Non-durable, each split @1M / @10M. Config rows × configuration columns; cell = the latest result
// of that row in that configuration: median ops/sec · low–high band · footprint; production rows
// add the hot path and the three drain times.
void write_bench_pages(jac313::Qlite::v001::Sqlite& db, const fs::path& out) {
    std::vector<std::int64_t> machines;
    { auto st = db.prepare("SELECT DISTINCT r.machine_id FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
        "JOIN testType tt ON tt.id=tr.test_type_id WHERE tt.name='bench' ORDER BY r.machine_id");
      while (st.step()) { std::int64_t m = 0; st.get(m); machines.push_back(m); } }
    if (machines.empty()) return;
    std::error_code ec; fs::create_directories(out / "bench", ec);
    std::ofstream md(out / "bench" / "README.md");
    nav_top(md, "../README.md");
    md << "# bench — throughput by machine\n\n_Generated from `results.db`. Throughput is hardware-specific, "
          "so each machine (`jac313-###`) is its own section. A column is one build configuration (compiler · "
          "build type · hdr/mod · contracts, then strict / hardened when on); a cell is the latest result of "
          "that row in it. A row name carries its caps: `[capped N]` = the run kept N events, `[N events]` = a "
          "size the name does not say, `[max N rows]` = at most N rows persisted; rows that would still look "
          "alike add their threads × events, runs and batch. Groups: **Production** "
          "(the keeper file + the database + the binary replay log at once, events routed by their "
          "KeeperRecord / DatabaseEntry flags — the run that looks like a real program), then each "
          "**database** alone, each **file** alone, and the **non-durable** flag sweep; each split @1M / "
          "@10M events and sorted by median ops/sec (descending). Production cells: hot path ops/sec "
          "(producers until they join) · drain ms (binary / keeper / database: attach → that sink's "
          "finalize done) · durable total median ops/sec (the whole process) · band · size. Each run's page "
          "lists every setting it recorded. See docs/Benchmarks.md._\n";

    struct Cell { bool set = false; std::int64_t med = 0; std::string median = "-", band = "-", size = "-", hot = "-", drain = "-"; };
    struct Row { std::string key; int group; int scale; std::vector<Cell> cells; };   // scale 1 = @1M, 2 = @10M
    for (const std::int64_t g : machines) {
        bool multi_os = false;
        auto runs = run_configs(db, "bench", g, multi_os);
        if (runs.empty()) continue;
        for (auto& r : runs) r.second.max_rows = 0;   // a bench row cap is in the row name, not the column
        const ConfigColumns cc = config_columns(runs, multi_os);
        const auto& comps = cc.cols;

        md << "\n## " << machine_heading(db, g) << "\n\n";

        // Every bench row of this machine, newest run first: the first row seen for a (row, column)
        // fills that cell.
        struct Fact { std::int64_t run, events, threads, epe, runs, batch, med, low, high, bytes, hot, dbin, dkeep, ddb;
                      std::string base, persist; int group, scale; };
        std::vector<Fact> facts;
        { auto st = db.prepare(
            std::string("SELECT tr.run_id, ") + kBenchNameSql + ", p.persist, p.threads, p.events_per_thread, "
            "p.runs, p.batch, tr.max_persist_rows, tr.median_ops, tr.low_ops, tr.high_ops, tr.bytes, tr.hot_ops, "
            "tr.drain_binary_ms, tr.drain_keeper_ms, tr.drain_database_ms "
            "FROM testRun tr JOIN run r ON r.run_id=tr.run_id LEFT JOIN testList tl ON tl.id=tr.test_list_id "
            "JOIN parameter p ON p.id=tr.parameter_id JOIN testType tt ON tt.id=tr.test_type_id "
            "WHERE tt.name='bench' AND r.machine_id=? ORDER BY tr.run_id DESC, tr.id");
          st.bind(g);
          while (st.step()) {
              Fact f{}; std::string config; std::int64_t max_rows = 0;
              st.get(f.run, config, f.persist, f.threads, f.epe, f.runs, f.batch, max_rows, f.med, f.low, f.high,
                     f.bytes, f.hot, f.dbin, f.dkeep, f.ddb);
              f.events = f.threads * f.epe;
              f.base = bench_row_name(config, server_tag(db, f.run, f.persist), f.events, max_rows);
              f.group = bench_group(f.persist);
              f.scale = f.group == 3 ? 0 : (f.events <= 1000000 ? 1 : 2);
              facts.push_back(std::move(f)); } }
        // Rows with one name but a different shape (threads × events, runs, batch) get it appended.
        std::map<std::string, std::set<std::tuple<std::int64_t, std::int64_t, std::int64_t, std::int64_t>>> shapes;
        for (const auto& f : facts) shapes[f.base].insert({f.threads, f.epe, f.runs, f.batch});

        std::vector<Row> rows;
        for (const auto& f : facts) {
            std::string key = f.base;
            if (shapes[f.base].size() > 1)
                key += " [" + fmt_num(f.threads) + " × " + fmt_num(f.epe) + ", " + fmt_num(f.runs) + " runs"
                     + (f.batch ? ", batch " + fmt_num(f.batch) : std::string()) + "]";
            Row* r = nullptr;
            for (auto& rr : rows) if (rr.key == key && rr.group == f.group && rr.scale == f.scale) { r = &rr; break; }
            if (!r) { rows.push_back({key, f.group, f.scale, std::vector<Cell>(comps.size())}); r = &rows.back(); }
            const auto it = cc.col_of.find(f.run);
            if (it == cc.col_of.end()) continue;
            Cell& cell = r->cells[static_cast<std::size_t>(it->second)];
            if (cell.set) continue;   // a newer run already filled it
            cell.set    = true;
            cell.med    = f.med;
            cell.median = fmt_num(f.med);
            cell.band   = human_ops(f.low) + "–" + human_ops(f.high);
            cell.size   = human_bytes(f.bytes);
            cell.hot    = fmt_num(f.hot);
            cell.drain  = drain_cell(f.persist, f.dbin, f.dkeep, f.ddb);
        }
        auto best = [](const Row& r) { std::int64_t b = 0; for (const auto& c : r.cells) b = std::max(b, c.med); return b; };
        std::stable_sort(rows.begin(), rows.end(), [&](const Row& a, const Row& b) { return best(a) > best(b); });

        // One table per group and scale, each in median-desc order. The configuration label heads a
        // column's first cell; its other cells follow it.
        for (int grp = 0; grp < 4; ++grp) {
            for (int scale = 0; scale < 3; ++scale) {
                bool any = false; for (const auto& r : rows) if (r.group == grp && r.scale == scale) { any = true; break; }
                if (!any) continue;
                md << "\n### " << kBenchGroupTitle[grp]
                   << (scale == 1 ? " @ 1M events" : scale == 2 ? " @ 10M events" : "")
                   << "  _(median ops/sec, descending)_\n\n| config";
                for (const auto& c : comps) {
                    md << " | [" << c.first << "](" << run_label_md(c.second) << ".md)<br>";
                    if (grp == 0) md << "hot ops/sec | drain ms<br>bin / keep / db | durable median ops/sec | band | size";
                    else          md << "median ops/sec | band | size";
                }
                md << " |\n|---";
                for (std::size_t i = 0; i < comps.size(); ++i) md << (grp == 0 ? "|--:|--:|--:|:--:|--:" : "|--:|:--:|--:");
                md << "|\n";
                for (const auto& r : rows) if (r.group == grp && r.scale == scale) {
                    md << "| " << r.key;
                    for (const auto& cell : r.cells) {
                        if (grp == 0) md << " | " << cell.hot << " | " << cell.drain;
                        md << " | " << cell.median << " | " << cell.band << " | " << cell.size;
                    }
                    md << " |\n";
                }
            }
        }
        // Run history (every bench run for this host, each linked to its detail page) + a page per run.
        write_run_history(db, md, runs, multi_os);
        for (const auto& r : runs) write_bench_detail(db, out / "bench", r.first);
    }
    nav_bottom(md, "../README.md");
}

// build = a HOST-SCOPED compile-time matrix: compile time is hardware-specific, so each machine
// (jac313-###) is its own section with a hardware header (cpu/cores/ram/disk) and its OWN config
// columns (different boxes run different compilers). Rows = tests, columns = config (compiler ×
// front-end: hdr/mod). Cell = compile+link seconds · status · binary size (latest build).
void write_build_pages(jac313::Qlite::v001::Sqlite& db, const fs::path& out) {
    std::vector<std::int64_t> machines;
    { auto st = db.prepare("SELECT DISTINCT r.machine_id FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
        "JOIN testType tt ON tt.id=tr.test_type_id WHERE tt.name='build' ORDER BY r.machine_id");
      while (st.step()) { std::int64_t m = 0; st.get(m); machines.push_back(m); } }
    if (machines.empty()) return;
    // Output dir is compiler-build-times/ (NOT build/) so the **/build/ ignore rule for CMake trees
    // doesn't swallow this report page. testType name in the DB stays 'build'.
    std::error_code ec; fs::create_directories(out / "compiler-build-times", ec);
    std::ofstream md(out / "compiler-build-times" / "README.md");
    nav_top(md, "../README.md");
    md << "# Compiler build times\n\n_Generated from `results.db`. Compile time is hardware-specific, so "
          "each machine (`jac313-###`) is its own section.\n\n"
          "Column headers are `compiler·fe` and the build:\n"
          "- `hdr` — code uses headers only\n"
          "- `mod` — code uses modules\n"
          "- the build type (Debug / Release), then `strict` / `hardened` when those options were on\n\n"
          "Cell = compile+link seconds · status · binary size (latest build); per test × front-end × compiler._\n";

    struct Config { std::string label, cname, bt, mod, imp, contracts, strict, hardened; std::int64_t cmajor; };
    for (const std::int64_t g : machines) {
        md << "\n## " << machine_heading(db, g) << "\n\n";

        std::vector<Config> configs;
        { auto st = db.prepare("SELECT DISTINCT c.name, c.major, p.build_type, p.modules, p.import_std, p.contracts, "
            "p.strict, p.hardened "
            "FROM testRun tr JOIN run r ON r.run_id=tr.run_id JOIN parameter p ON p.id=tr.parameter_id "
            "JOIN compiler c ON c.id=p.compiler_id JOIN testType tt ON tt.id=tr.test_type_id "
            "WHERE tt.name='build' AND r.machine_id=? "
            "ORDER BY c.name, c.major, p.modules, p.import_std, p.build_type, p.strict, p.hardened, p.contracts");
          st.bind(g);
          while (st.step()) { std::string cn, bt, mod, imp, ct, strict, hardened; std::int64_t cm = 0;
              st.get(cn, cm, bt, mod, imp, ct, strict, hardened);
              const std::string fe = (mod != "on") ? "hdr" : "mod";
              configs.push_back({comp_label(cn, cm) + "·" + fe + "<br>" + build_label(bt, strict, hardened)
                                 + contracts_tag(ct), cn, bt, mod, imp, ct, strict, hardened, cm}); } }
        if (configs.empty()) continue;

        std::vector<std::string> tests;
        { auto st = db.prepare("SELECT DISTINCT tl.name FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
            "JOIN testList tl ON tl.id=tr.test_list_id JOIN testType tt ON tt.id=tr.test_type_id "
            "WHERE tt.name='build' AND r.machine_id=? ORDER BY tl.name");
          st.bind(g); while (st.step()) { std::string n; st.get(n); tests.push_back(n); } }

        md << "| test";
        for (const auto& cf : configs) md << " | " << cf.label;
        md << " |\n|---";
        for (std::size_t i = 0; i < configs.size(); ++i) md << "|:--:";
        md << "|\n";
        for (const auto& t : tests) {
            md << "| " << t;
            for (const auto& cf : configs) {
                std::string cell = "-";
                auto st = db.prepare("SELECT tr.duration_ms, tr.status, tr.bytes "
                    "FROM testRun tr JOIN run r ON r.run_id=tr.run_id JOIN parameter p ON p.id=tr.parameter_id "
                    "JOIN compiler c ON c.id=p.compiler_id JOIN testList tl ON tl.id=tr.test_list_id "
                    "JOIN testType tt ON tt.id=tr.test_type_id "
                    "WHERE tt.name='build' AND r.machine_id=? AND tl.name=? AND c.name=? AND c.major=? "
                    "AND p.build_type=? AND p.modules=? AND p.import_std=? AND p.contracts=? AND p.strict=? "
                    "AND p.hardened=? ORDER BY tr.run_id DESC LIMIT 1");
                st.bind(g, t, cf.cname, cf.cmajor, cf.bt, cf.mod, cf.imp, cf.contracts, cf.strict, cf.hardened);
                if (st.step()) { std::int64_t ms = 0, bytes = 0; std::string status; st.get(ms, status, bytes);
                    if (status == "NA") { cell = "NA"; }   // config not built by design
                    else if (ms > 0) { cell = fmt_build_seconds(ms) + " · " + (status.empty() ? "-" : status)
                                  + (bytes ? (" · " + human_bytes(bytes)) : std::string()); } }
                md << " | " << cell;
            }
            md << " |\n";
        }
    }
    nav_bottom(md, "../README.md");
}

// "[Run_012](../smoke/Run_012.md)": a run, linked to its detail page from a page one level below
// test-summary/ (a build-times run has no detail page: plain text with its gate).
std::string run_link(jac313::Qlite::v001::Sqlite& db, std::int64_t run_id) {
    std::string type;
    { auto st = db.prepare("SELECT tt.name FROM testRun tr JOIN testType tt ON tt.id=tr.test_type_id "
                           "WHERE tr.run_id=? ORDER BY tr.id LIMIT 1");
      st.bind(run_id); if (st.step()) st.get(type); }
    const std::string rl = run_label_md(run_id);
    if (type.empty() || type == "build") return rl + (type.empty() ? "" : " (" + type + ")");
    return "[" + rl + "](../" + type + "/" + rl + ".md)";
}

// test-summary/servers/README.md: every database server a run wrote to or reached (run_target), with
// its databases and tables and the runs that used it; then this machine's SQLite files the same way.
void write_servers_page(jac313::Qlite::v001::Sqlite& db, const fs::path& out) {
    std::error_code ec; fs::create_directories(out / "servers", ec);
    std::ofstream md(out / "servers" / "README.md");
    nav_top(md, "../README.md");
    md << "# Servers\n\n_Generated from `results.db` (`run_target`): every database server a run wrote to, "
          "or reached as the fleet registry, with its databases and tables and the runs that used it. A server "
          "is named by its name in the connections file (`r430`), else by its host; one heading per name and address. Files "
          "(jtext, html, json, binary) are listed on each run's page._\n";
    // One table per (server, address, port): database · table · roles · runs.
    auto table = [&](const std::string& where_sql, auto&&... binds) {
        struct Item { std::set<std::string> roles; std::vector<std::int64_t> runs; };
        std::map<std::pair<std::string, std::string>, Item> items;   // (dbname, table) -> roles, runs
        auto st = db.prepare("SELECT dbname, table_name, role, run_id FROM run_target WHERE " + where_sql
                             + " ORDER BY dbname, table_name, run_id DESC");
        st.bind(binds...);
        while (st.step()) {
            std::string dbname, tbl, role; std::int64_t run = 0;
            st.get(dbname, tbl, role, run);
            Item& it = items[{dbname, tbl}];
            it.roles.insert(role);
            if (std::find(it.runs.begin(), it.runs.end(), run) == it.runs.end()) it.runs.push_back(run);
        }
        md << "| database | table | role | runs |\n|---|---|---|---|\n";
        for (const auto& [key, it] : items) {
            std::string roles, runs;
            for (const auto& r : it.roles) roles += (roles.empty() ? "" : ", ") + r;
            for (const auto run : it.runs) runs += (runs.empty() ? "" : ", ") + run_link(db, run);
            md << "| " << dash(key.first) << " | " << dash(key.second) << " | " << roles << " | " << runs << " |\n";
        }
    };
    bool any = false;
    { auto st = db.prepare("SELECT DISTINCT server, address, port FROM run_target "
                           "WHERE server<>'' OR address<>'' ORDER BY server, address, port");
      std::vector<std::tuple<std::string, std::string, std::int64_t>> servers;
      while (st.step()) { std::string s, a; std::int64_t p = 0; st.get(s, a, p); servers.emplace_back(s, a, p); }
      for (const auto& [server, address, port] : servers) {
          any = true;
          md << "\n## " << dash(server) << " — " << dash(address) << (port ? ":" + std::to_string(port) : std::string())
             << "\n\n";
          table("server=? AND address=? AND port=?", server, address, port);
      } }
    if (!any) md << "\n_No run has written to a database server yet._\n";
    // SQLite: a file on this machine, no server; its database is the .db path.
    std::int64_t sqlite_rows = 0;
    { auto st = db.prepare("SELECT COUNT(*) FROM run_target WHERE backend='sql'"); if (st.step()) st.get(sqlite_rows); }
    if (sqlite_rows > 0) {
        md << "\n## SQLite (files on the machine that ran)\n\n";
        table("backend=?", std::string("sql"));
    }
    nav_bottom(md, "../README.md");
}

// Why a machine is NOT SAFE, in words the reader can act on. Each gate gives its name, whether it
// has any recorded result, and its fail count. Empty when every gate ran clean (PRESUMED SAFE).
struct GateState { const char* name; bool ran; std::int64_t fail; };
std::string not_safe_reason(std::initializer_list<GateState> gates) {
    std::string missing, failed;
    for (const GateState& g : gates) {
        if (!g.ran)      missing += (missing.empty() ? "" : ", ") + std::string(g.name);
        else if (g.fail) failed += (failed.empty() ? "" : ", ") + std::string(g.name) + " " + std::to_string(g.fail);
    }
    std::string why;
    if (!missing.empty())
        why = "not every test has run (no results for: " + missing + "). PRESUMED SAFE needs the full test: "
              "ctest, smoke, bench and valgrind (memcheck, helgrind, drd) on both gcc and clang. Run "
              "`jac313_test_cli --run-everything`, or in Jac313Runner tick every gate with Compiler: both";
    if (!failed.empty())
        why += (why.empty() ? "" : "; ") + std::string("failures: ") + failed;
    return why;
}

// test-summary/machines/README.md: one section per machine (jac313-###): its hardware, then every OS
// it ran under (os, from its runs) with the run count and the latest run, linked.
void write_machines_page(jac313::Qlite::v001::Sqlite& db, const fs::path& out) {
    std::error_code ec; fs::create_directories(out / "machines", ec);
    std::ofstream md(out / "machines" / "README.md");
    nav_top(md, "../README.md");
    md << "# Machines\n\n_Generated from `results.db` (`machine`, `os`, `run`). A machine is its hardware "
          "(`jac313-###`); the OS is recorded per run, so a machine that boots two OSes is one machine "
          "with two OS rows. \\* P.Cores = Physical Cores; T.Cores = Threading Cores._\n";
    struct M { std::int64_t id = 0, mhz_lo = 0, mhz_hi = 0, p_cores = 0, t_cores = 0, ram = 0;
               std::string cpu, disk, hash; };
    std::vector<M> machines;
    { auto st = db.prepare("SELECT id, cpu, cpu_mhz_min, cpu_mhz_max, p_cores, cores, ram_gb, disk, instance_hash "
                           "FROM machine ORDER BY id");
      while (st.step()) { M m; st.get(m.id, m.cpu, m.mhz_lo, m.mhz_hi, m.p_cores, m.t_cores, m.ram, m.disk, m.hash);
                          machines.push_back(m); } }
    if (machines.empty()) md << "\n_No machine is recorded yet._\n";
    for (const M& m : machines) {
        std::string speed = "-";
        if (m.mhz_hi > 0) { char b[40];
            if (m.mhz_lo == m.mhz_hi) std::snprintf(b, sizeof b, "%.1f GHz", static_cast<double>(m.mhz_hi) / 1000.0);
            else std::snprintf(b, sizeof b, "%.1f–%.1f GHz",
                               static_cast<double>(m.mhz_lo) / 1000.0, static_cast<double>(m.mhz_hi) / 1000.0);
            speed = b; }
        md << "\n## " << jac313::results::host_label(m.id) << "\n\n"
              "| CPU | Speed | P.Cores | T.Cores | RAM | Disk | instance |\n|---|--:|--:|--:|--:|---|---|\n"
           << "| " << dash(m.cpu) << " | " << speed << " | " << m.p_cores << " | " << m.t_cores << " | "
           << m.ram << " GB | " << dash(m.disk) << " | " << dash(jac313::results::hash_prefix(m.hash)) << " |\n\n";
        // The OSes this machine ran under: one row per os row its runs reference.
        auto st = db.prepare("SELECT o.name, o.version, o.kernel, o.arch, COUNT(*), MAX(r.run_id) FROM run r "
                             "INNER JOIN os o ON o.id = r.os_id WHERE r.machine_id=? "
                             "GROUP BY o.id ORDER BY MAX(r.run_id) DESC");
        st.bind(m.id);
        bool any = false;
        while (st.step()) {
            if (!any) { md << "| OS | kernel | arch | runs | latest run |\n|---|---|---|--:|---|\n"; any = true; }
            std::string name, version, kernel, arch; std::int64_t runs = 0, latest = 0;
            st.get(name, version, kernel, arch, runs, latest);
            md << "| " << dash(jac313::results::os_display(name, version)) << " | " << dash(kernel) << " | "
               << dash(arch) << " | " << runs << " | " << run_link(db, latest) << " |\n";
        }
        if (!any) md << "_No run recorded on this machine yet._\n";
    }
    nav_bottom(md, "../README.md");
}

// Top-level landing index: test-summary/README.md, linking to the compiler page and every test
// type that has data (with its run/row counts).
void write_index_page(jac313::Qlite::v001::Sqlite& db, const fs::path& out) {
    std::ofstream md(out / "README.md");
    nav_top(md, "../README.md");
    md << "# Test results\n\n_Generated from `results.db` by `jac313_test_cli --report`._\n\n";
    // Version isolation (v001 vs v002 trees) using JAC313_VERSION
    {
        std::string ver;
        auto st = db.prepare("SELECT version FROM run WHERE version <> '' ORDER BY run_id DESC LIMIT 1");
        if (st.step()) st.get(ver);
        if (!ver.empty()) md << "**jac313 version:** " << ver << " (runs, deletes, and safeness are scoped by this)\n\n";
    }

    // Safeness — summary-of-summary verdict per machine and OS (the same machine under two OSes is two
    // results), read from the `safeness` table (never recounts).
    {
        struct GatePF { std::int64_t pass = 0, fail = 0, ran = 0; };
        struct MachineOs { std::int64_t machine = 0, os = 0; };
        auto gate_pf = [&](const MachineOs& g, const char* gate) {
            GatePF x;
            // Prefer data for the latest recorded JAC313_VERSION on this machine (v001 vs v002 isolation)
            std::string ver;
            { auto vst = db.prepare("SELECT version FROM run WHERE machine_id=? AND version <> '' ORDER BY run_id DESC LIMIT 1");
              vst.bind(g.machine); if (vst.step()) vst.get(ver); }
            // COALESCE kept: SUM over no matching rows is NULL.
            std::string sql = "SELECT COALESCE(SUM(pass),0), COALESCE(SUM(fail),0), COUNT(*) FROM safeness "
                              "WHERE machine_id=? AND os_id=? AND gate=?";
            if (!ver.empty()) sql += " AND (version=? OR version='')";
            auto st = db.prepare(sql);
            // Bind ALL params in ONE call: Sqlite::bind() does sqlite3_clear_bindings() first, so a
            // second bind() would wipe the earlier values and rebind ver into param 1 — the
            // version-scoping regression that zeroed the verdict. Branch on ver instead of re-binding.
            if (ver.empty()) st.bind(g.machine, g.os, std::string(gate));
            else             st.bind(g.machine, g.os, std::string(gate), ver);
            if (st.step()) st.get(x.pass, x.fail, x.ran);
            return x; };
        // "jac313-002 · Fedora Linux 44 (Workstation Edition) · 7.2.13-200.fc44.x86_64"
        auto label = [&](const MachineOs& g) {
            std::string name, version, kernel;
            { auto st = db.prepare("SELECT name, version, kernel FROM os WHERE id=?");
              st.bind(g.os); if (st.step()) st.get(name, version, kernel); }
            const std::string os = jac313::results::os_display(name, version);
            return jac313::results::host_label(g.machine) + (os.empty() ? std::string() : " · " + os)
                 + (kernel.empty() ? std::string() : " · " + kernel); };
        auto pf_cell = [](const GatePF& x) { return x.ran ? (std::to_string(x.pass) + "/" + std::to_string(x.fail)) : std::string("-"); };
        auto vg_cell = [](const GatePF& x) { return x.ran ? std::string(x.fail == 0 ? "✅" : "❌") : std::string("n/a"); };
        std::vector<MachineOs> groups;
        { auto st = db.prepare("SELECT DISTINCT machine_id, os_id FROM safeness ORDER BY machine_id, os_id");
          while (st.step()) { MachineOs g; st.get(g.machine, g.os); groups.push_back(g); } }
        if (!groups.empty()) {
            md << "## Safeness\n\n| machine · OS · kernel | verdict | ctest | smoke | bench | mem | hel | drd |\n"
                  "|---|---|--:|--:|--:|:--:|:--:|:--:|\n";
            for (const MachineOs& g : groups) {
                const GatePF ct = gate_pf(g, "ctest"), sm = gate_pf(g, "smoke"), bn = gate_pf(g, "bench");
                const GatePF me = gate_pf(g, "memcheck"), he = gate_pf(g, "helgrind"), dr = gate_pf(g, "drd");
                const bool ran  = ct.ran && sm.ran && bn.ran && me.ran && he.ran && dr.ran;
                const bool clean = (ct.fail + sm.fail + bn.fail + me.fail + he.fail + dr.fail) == 0;
                md << "| " << label(g) << " | "
                   << ((ran && clean) ? "✅ PRESUMED SAFE" : !ran ? "❌ NOT SAFE: tests missing" : "❌ NOT SAFE: failures")
                   << " | " << pf_cell(ct) << " | " << pf_cell(sm) << " | " << pf_cell(bn)
                   << " | " << vg_cell(me) << " | " << vg_cell(he) << " | " << vg_cell(dr) << " |\n";
            }
            md << "\n\\* ✅ PRESUMED SAFE = builds + ctest/smoke/bench 0-fail + memcheck/helgrind/drd clean, "
                  "both compilers. Presumed, not proven.\n\n";
            for (const MachineOs& g : groups) {
                const GatePF ct = gate_pf(g, "ctest"), sm = gate_pf(g, "smoke"), bn = gate_pf(g, "bench");
                const GatePF me = gate_pf(g, "memcheck"), he = gate_pf(g, "helgrind"), dr = gate_pf(g, "drd");
                const std::string why = not_safe_reason({ {"ctest", ct.ran != 0, ct.fail}, {"smoke", sm.ran != 0, sm.fail},
                    {"bench", bn.ran != 0, bn.fail}, {"memcheck", me.ran != 0, me.fail},
                    {"helgrind", he.ran != 0, he.fail}, {"drd", dr.ran != 0, dr.fail} });
                if (!why.empty()) md << "❌ **" << label(g) << "** is NOT SAFE: " << why << ".\n\n";
            }
            for (const MachineOs& g : groups) {
                const GatePF ct = gate_pf(g, "ctest"), sm = gate_pf(g, "smoke"), bn = gate_pf(g, "bench");
                const GatePF me = gate_pf(g, "memcheck"), he = gate_pf(g, "helgrind"), dr = gate_pf(g, "drd");
                md << "### " << label(g) << " — gate detail\n\n"
                      "| gate | pass | fail | memcheck | helgrind | drd |\n|---|--:|--:|:--:|:--:|:--:|\n"
                   << "| ctest | " << ct.pass << " | " << ct.fail << " | " << vg_cell(me) << " | " << vg_cell(he) << " | " << vg_cell(dr) << " |\n"
                   << "| smoke | " << sm.pass << " | " << sm.fail << " | n/a | n/a | n/a |\n"
                   << "| bench | " << bn.pass << " | " << bn.fail << " | n/a | n/a | n/a |\n\n";
            }
        }
    }

    md << "| area | runs | rows |\n|---|--:|--:|\n"
          "| [machines](machines/README.md) (each machine's hardware and the OSes it ran under) | — | — |\n"
          "| [compilers](compiler/README.md) | — | — |\n"
          "| [servers](servers/README.md) (what each run wrote where) | — | — |\n";
    // testType name (DB) vs. output dir/link name (GitHub-facing): they match except for
    // 'build', whose page lives in compiler-build-times/ to avoid the **/build/ ignore rule.
    struct Area { const char* type; const char* dir; };
    for (const Area a : {Area{"ctest", "ctest"}, {"smoke", "smoke"}, {"bench", "bench"},
                         {"verify-lite", "verify-lite"}, {"verify", "verify"},
                         {"build", "compiler-build-times"}}) {
        std::int64_t runs = 0, rows = 0;
        { auto st = db.prepare("SELECT COUNT(DISTINCT tr.run_id), COUNT(*) FROM testRun tr "
                               "JOIN testType tt ON tt.id=tr.test_type_id WHERE tt.name=?");
          st.bind(std::string(a.type)); if (st.step()) st.get(runs, rows); }
        if (rows == 0) continue;
        md << "| [" << a.dir << "](" << a.dir << "/README.md) | " << runs << " | " << rows << " |\n";
    }
    md << "\n**Settings shown:** result columns (ctest, smoke, verify, bench, build times) = the build "
          "configuration and run-wide settings; bench row names = caps and size; each run's page (`Run_###`) = "
          "every recorded setting, read-back, timings and batch; servers = what each run wrote where; "
          "machines = hardware and OS.\n";
    nav_bottom(md, "../README.md");
}

int run_report_command(const fs::path& source_dir) {
    const fs::path db_path = JAC313_RESULTS_DB;
    const std::string ver = jac313::harness_version_from_source_dir(source_dir);
    std::error_code ec;
    if (!fs::exists(db_path, ec)) { std::cerr << "no results.db at " << db_path.string() << "\n"; return 1; }
    try {
        // Render-only: open READ-ONLY and skip ensure_schema, so --report only writes the .md pages
        // and never modifies results.db. The DB exists (checked above) and was schema'd by the recorders.
        jac313::Qlite::v001::Sqlite db(db_path.string(), SQLITE_OPEN_READONLY);
        // Version scoping for the SHARED DB: shadow run/testRun/safeness with TEMP VIEWs that keep ONLY
        // this tree's version, permissive on version='' rows. Every report query below then filters
        // transparently, and the "latest version in DB" banner/safeness heuristics auto-correct because
        // their underlying `run` is now this-version-only. TEMP objects are allowed on a read-only main
        // DB (they live in the writable temp schema) and are per-connection.
        db.exec("CREATE TEMP VIEW run AS SELECT * FROM main.run "
                "WHERE version='" + ver + "' OR version=''");
        db.exec("CREATE TEMP VIEW testRun AS SELECT tr.* FROM main.testRun tr "
                "JOIN main.run r ON r.run_id=tr.run_id "
                "WHERE r.version='" + ver + "' OR r.version=''");
        db.exec("CREATE TEMP VIEW safeness AS SELECT * FROM main.safeness "
                "WHERE version='" + ver + "' OR version=''");
        db.exec("CREATE TEMP VIEW run_target AS SELECT t.* FROM main.run_target t "
                "JOIN main.run r ON r.run_id=t.run_id "
                "WHERE r.version='" + ver + "' OR r.version=''");
        const fs::path out = source_dir / "test-summary";
        write_compiler_page(db, out);
        for (const char* t : {"ctest", "smoke", "verify-lite", "verify"}) write_type_pages(db, out, t);
        write_bench_pages(db, out);
        write_build_pages(db, out);
        write_servers_page(db, out);
        write_machines_page(db, out);
        write_index_page(db, out);
        // Self-clean: a Run_<id>.md belongs in area <X>'s dir only if run <id> actually recorded rows of
        // type <X>. Drop any page whose run is gone OR is a different gate's run (stale pages left behind
        // when a re-record/merge/wipe renumbered runs) — so nothing orphaned or mislabeled survives.
        int swept = 0;
        for (const char* area : {"ctest", "smoke", "verify", "verify-lite", "bench"}) {
            const fs::path d = out / area; std::error_code dec;
            if (!fs::is_directory(d, dec)) continue;
            auto has_type = [&](std::int64_t id) {
                auto st = db.prepare("SELECT 1 FROM testRun tr JOIN testType tt ON tt.id=tr.test_type_id "
                                     "WHERE tr.run_id=? AND tt.name=? LIMIT 1");
                st.bind(id, std::string(area)); return st.step(); };
            for (const auto& e : fs::directory_iterator(d, dec)) {
                const std::string fn = e.path().filename().string();
                if (fn.rfind("Run_", 0) == 0 && e.path().extension() == ".md"
                    && !has_type(std::strtoll(fn.c_str() + 4, nullptr, 10))) {
                    std::error_code rec; if (fs::remove(e.path(), rec)) ++swept;
                }
            }
        }
        std::cout << "[report] wrote test-summary/README.md + {compiler,ctest,smoke,verify-lite,verify,bench,compiler-build-times,servers,machines}/ from results.db"
                  << (swept ? (" (swept " + std::to_string(swept) + " orphan run page(s))") : std::string()) << "\n";
        return 0;
    } catch (const std::exception& e) { std::cerr << "report failed: " << e.what() << "\n"; return 1; }
}

// Parse ninja's build log (.ninja_log lines: "start_ms<TAB>end_ms<TAB>mtime<TAB>output<TAB>hash")
// and aggregate compile+link wall-clock per target: a target's time = sum of its object edges
// (output paths containing "<target>.dir/") + its link edge (output whose filename == target).
// The LAST entry per output wins (ninja appends on rebuild). Single-source targets — which is
// every report test binary — sum exactly to wall-clock; multi-source targets over-count (their
// objects compile in parallel). A target with no edges in the log maps to 0 (not built this round).
std::map<std::string, std::int64_t> parse_ninja_build_times(const fs::path& build_dir,
                                                            const std::vector<std::string>& targets) {
    std::map<std::string, std::int64_t> out;
    for (const auto& t : targets) out[t] = 0;
    std::ifstream in(build_dir / ".ninja_log");
    if (!in) return out;
    std::unordered_map<std::string, std::int64_t> edge_ms;   // output path -> ms (last entry wins)
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        const std::size_t p1 = line.find('\t');
        const std::size_t p2 = p1 == std::string::npos ? p1 : line.find('\t', p1 + 1);
        const std::size_t p3 = p2 == std::string::npos ? p2 : line.find('\t', p2 + 1);
        const std::size_t p4 = p3 == std::string::npos ? p3 : line.find('\t', p3 + 1);
        if (p4 == std::string::npos) continue;
        const std::int64_t start = std::strtoll(line.c_str(), nullptr, 10);
        const std::int64_t end   = std::strtoll(line.c_str() + p1 + 1, nullptr, 10);
        edge_ms[line.substr(p3 + 1, p4 - p3 - 1)] = end - start;
    }
    for (auto& [t, ms] : out) {
        const std::string dirtag = t + ".dir/";
        for (const auto& [output, d] : edge_ms)
            if (output.find(dirtag) != std::string::npos || fs::path(output).filename() == t) ms += d;
    }
    return out;
}

// Size (bytes) of the built executable named `target` under build_dir (0 if not found). Mirrors
// build_matrix.sh's `find -name <target>` — robust to CMake's per-target output layout.
std::int64_t exe_size_in_tree(const fs::path& build_dir, const std::string& target) {
    std::error_code ec;
    for (fs::recursive_directory_iterator it(build_dir, ec), end; it != end; it.increment(ec)) {
        if (ec) { ec.clear(); continue; }
        if (it->is_regular_file(ec) && it->path().filename() == target)
            return static_cast<std::int64_t>(fs::file_size(it->path(), ec));
    }
    return 0;
}

// Capture compile+link times from a freshly-built tree's .ninja_log and UPSERT them as 'build'
// rows for THIS machine. Banks every target that actually built, so a normal ctest/smoke
// build records its times for free. Front-end (headers/modules) is read from the tree unless
// overridden. Machine-scoped upsert: a prior row for the same (this machine, config, test) is replaced;
// other machines' rows (same config, different machine_id) are left intact.
void capture_build_times(const GlobalOptions& global, const fs::path& build_dir,
                         const std::string& modules_override = "",
                         const std::string& import_std_override = "") {
    const fs::path db_path = JAC313_RESULTS_DB;
    std::error_code ec;
    if (!fs::exists(db_path.parent_path(), ec)) { not_recorded("build-times", "no " + db_path.parent_path().string()); return; }
    std::vector<std::string> names;
    for (const auto& t : discover_tests(build_dir)) names.push_back(t.name);
    // jac313_store_bench is a real build target but NOT a ctest test, so discover_tests misses it.
    // Add it explicitly so its compile time is banked whenever the tree actually built it.
    if (std::find(names.begin(), names.end(), "jac313_store_bench") == names.end())
        names.push_back("jac313_store_bench");
    if (names.empty()) return;
    const auto times = parse_ninja_build_times(build_dir, names);
    try {
        jac313::Qlite::v001::Sqlite db(db_path.string());
        const std::int64_t run_id  = cli_begin_run(db, global);   // one run for the whole capture batch
        std::int64_t mid = 0;
        { auto st = db.prepare("SELECT machine_id FROM run WHERE run_id=?"); st.bind(run_id); if (st.step()) st.get(mid); }
        const std::int64_t type_id = cli_results_id(db, "SELECT id FROM testType WHERE name=?", std::string("build"));
        const std::int64_t comp_id = cli_compiler_id(db, read_compiler_info(build_dir));
        const BuildAxes axes = build_axes(read_build_features(build_dir), modules_override, import_std_override);
        const std::string& modules = axes.modules;
        const std::int64_t param_id = cli_parameter_id(db, comp_id, axes);
        int banked = 0;
        auto tx = db.transaction();   // every banked row lands together (or none on an error)
        for (const auto& [t, ms] : times) {
            if (ms <= 0) continue;   // not built in this tree — leave any prior row intact
            db.exec("INSERT OR IGNORE INTO testList(name) VALUES(?)", t);
            const std::int64_t list_id = cli_results_id(db, "SELECT id FROM testList WHERE name=?", t);
            const std::int64_t bytes = exe_size_in_tree(build_dir, t);
            db.exec("DELETE FROM testRun WHERE test_type_id=? AND test_list_id=? AND parameter_id=? "
                    "AND run_id IN (SELECT run_id FROM run WHERE machine_id=?)", type_id, list_id, param_id, mid);
            db.exec("INSERT INTO testRun(run_id, test_type_id, test_list_id, parameter_id, status, duration_ms, bytes) "
                    "VALUES(?,?,?,?,?,?,?)", run_id, type_id, list_id, param_id, std::string("built"), ms, bytes);
            ++banked;
        }
        tx.commit();
        const std::string fe = (modules == "on") ? "mod" : "hdr";
        std::cout << "[results] build-times: banked " << banked << " target(s) under "
                  << jac313::results::host_label(mid) << " " << read_compiler_info(build_dir).name
                  << "·" << fe << " (run " << run_id << ")\n";
    } catch (const std::exception& e) {
        not_recorded("build-times", e.what());
    }
}

// Build, then on success bank this tree's compile times (capture-on-build). Use in place of
// run_build_command wherever the CLI builds the test tree, so a normal run records times for free.
int build_and_capture(const GlobalOptions& global, const BuildOptions& build_opts) {
    const int rc = run_build_command(global, build_opts);
    if (rc == 0) capture_build_times(global, global.build_dir);
    return rc;
}

// ---- build-times gate (the folded-in build_matrix.sh) ----------------------------------------
// The compile-time matrix: smoke + bench targets × {hdr, mod} front-ends × {gcc, clang},
// gap-filled per host. A target is in scope if it's a store matrix (smoke) test or the bench bin.
bool is_smoke_or_bench_target(const std::string& name) {
    if (name == "jac313_store_bench") return true;                 // bench
    if (name == "jac313_store_flags" || name == "jac313_store_metric_view") return true;  // smoke matrix
    constexpr std::string_view pre = "jac313_store_";              // jac313_store_<NNN>_<TS|XS>
    if (!name.starts_with(pre)) return false;
    const std::string rest = name.substr(pre.size());
    return rest.size() == 6 && std::isdigit(static_cast<unsigned char>(rest[0]))
           && (rest.ends_with("_TS") || rest.ends_with("_XS"));
}

// Refuse to delete anything not under <source>/tmp_build (ported from build_matrix.sh safe_rmdir).
void safe_rmdir(const fs::path& source_dir, const fs::path& target) {
    std::error_code ec;
    const fs::path root = fs::weakly_canonical(source_dir / "tmp_build", ec);
    const fs::path t    = fs::weakly_canonical(target, ec);
    if (t != root && t.string().rfind(root.string() + "/", 0) != 0) {
        std::cerr << "REFUSING to delete '" << target.string() << "' (not under tmp_build)\n";
        return;
    }
    fs::remove_all(t, ec);
}

// Resolve this machine's pinned gcc/clang to a compiler path — DEFINED below the pin helpers
// (find_pin_for_host lives further down in this file).
std::string resolve_pinned_cc(const fs::path& src, bool want_gcc);

// Does a 'build' row already exist for (this machine, compiler, front-end, target)?
bool build_row_exists(jac313::Qlite::v001::Sqlite& db, std::int64_t mid, std::int64_t comp_id,
                      const std::string& modules, const std::string& istd, const std::string& target) {
    auto st = db.prepare(
        "SELECT 1 FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
        "JOIN testType tt ON tt.id=tr.test_type_id JOIN testList tl ON tl.id=tr.test_list_id "
        "JOIN parameter p ON p.id=tr.parameter_id "
        "WHERE tt.name='build' AND r.machine_id=? AND p.compiler_id=? AND p.modules=? AND p.import_std=? "
        "AND tl.name=? LIMIT 1");
    st.bind(mid, comp_id, modules, istd, target);
    return st.step();
}

struct BuildTimesCfg { std::string label, cc; std::vector<std::string> feflags; std::string mod, istd; };

int run_build_times_gate(const fs::path& source_dir, bool dry_run, bool force = false) {
    const std::string gcc_cc = resolve_pinned_cc(source_dir, true);
    const std::string clang_cc = resolve_pinned_cc(source_dir, false);
    if (gcc_cc.empty() && clang_cc.empty()) {
        std::cerr << "build-times: no pinned compiler resolved — run ./bsir.sh first.\n";
        return 1;
    }
    std::vector<BuildTimesCfg> cfgs;
    for (const auto& [name, cc] : {std::pair<std::string,std::string>{"gcc", gcc_cc}, {"clang", clang_cc}}) {
        if (cc.empty()) continue;
        cfgs.push_back({name + "-hdr",  cc, {}, "off", "off"});
        cfgs.push_back({name + "-mod",  cc, {"-DJAC313_BUILD_MODULES=ON"}, "on", "off"});
    }

    const fs::path db_path = JAC313_RESULTS_DB;
    std::error_code ec;
    if (!fs::exists(db_path.parent_path(), ec)) { std::cerr << "build-times: no test-summary/.\n"; return 1; }
    jac313::Qlite::v001::Sqlite db(db_path.string());
    jac313::results::ensure_schema(db);
    const HostHardwareRecord hw = collect_host_hardware_record(machine_disk_tier("."));
    const jac313::results::MachineId h = make_machine_id(hw);
    const bool new_setup = jac313::results::find_exact_machine(db, h) == 0
        && jac313::results::propose_machine_id(db, h, is_generic_nodename(nodename_for_hash())).mid
               == jac313::results::next_auto_machine_id(db);
    GlobalOptions pin_global;
    const auto mid_opt = pin_current_host(db, pin_global);
    if (!mid_opt) return 1;
    const std::int64_t mid = *mid_opt;
    const std::string host = jac313::results::host_label(mid);
    std::cout << "=== build-times gate — " << host << (new_setup ? "  *** NEW SETUP (full collection) ***" : "")
              << " ===\n";
    if (dry_run) {
        std::cout << cfgs.size() << " configs (smoke+bench × {hdr,mod} × {gcc,clang}); per config:\n";
        for (const auto& c : cfgs) {
            std::string flags; for (const auto& f : c.feflags) flags += " " + f;
            std::cout << "  cmake -G Ninja -S . -B tmp_build/" << c.label << " -DCMAKE_CXX_COMPILER=" << c.cc
                      << " -DCMAKE_BUILD_TYPE=Debug -DJAC313_BUILD_STORE_TESTS=ON" << flags << "\n";
        }
        std::cout << "(builds only the gap targets per config; configs already complete for " << host << " are skipped)\n";
        return 0;
    }

    // Fast-skip (keeps the pre-push hook cheap): if every expected config already has build rows
    // for this host AND they all carry the same target count, the machine is complete — return
    // without configuring anything. (A newly-added test, absent from all configs alike, won't be
    // detected here; a normal run's capture-on-build or a forced re-run fills it.)
    if (!new_setup && !force) {
        const int expected = (gcc_cc.empty() ? 0 : 2) + (clang_cc.empty() ? 0 : 2);
        int combos = 0; std::int64_t lo = -1, hi = 0;
        auto st = db.prepare(
            "SELECT COUNT(DISTINCT tr.test_list_id) FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
            "JOIN testType tt ON tt.id=tr.test_type_id JOIN parameter p ON p.id=tr.parameter_id "
            "JOIN compiler c ON c.id=p.compiler_id WHERE tt.name='build' AND r.machine_id=? "
            "GROUP BY c.name, c.major, p.modules, p.import_std");
        st.bind(mid);
        while (st.step()) { std::int64_t n = 0; st.get(n); ++combos; if (lo < 0 || n < lo) lo = n; if (n > hi) hi = n; }
        if (combos >= expected && lo == hi && lo > 0) {
            std::cout << "  complete: " << combos << " configs × " << lo << " targets already recorded for "
                      << host << " — nothing to build.\n";
            return 0;
        }
    }

    std::vector<std::string> targets;   // smoke+bench target names, discovered once
    int built_total = 0, skipped = 0, failed = 0;
    for (const auto& c : cfgs) {
        const fs::path dir = source_dir / "tmp_build" / c.label;
        safe_rmdir(source_dir, dir);
        std::vector<std::string> conf = {"cmake", "-G", "Ninja", "-S", ".", "-B", "tmp_build/" + c.label,
            "-DCMAKE_CXX_COMPILER=" + c.cc, "-DCMAKE_BUILD_TYPE=Debug", "-DJAC313_BUILD_STORE_TESTS=ON"};
        conf.insert(conf.end(), c.feflags.begin(), c.feflags.end());
        if (run_process(conf, 0, source_dir.string(), 0).exit_code != 0) {
            std::cout << "  " << c.label << ": CONFIGURE FAILED\n";
            ++failed;
            safe_rmdir(source_dir, dir); continue;
        }
        if (targets.empty()) {
            for (const auto& t : discover_tests(dir)) if (is_smoke_or_bench_target(t.name)) targets.push_back(t.name);
            if (std::find(targets.begin(), targets.end(), "jac313_store_bench") == targets.end())
                targets.push_back("jac313_store_bench");
        }
        const std::int64_t comp_id = cli_compiler_id(db, read_compiler_info(dir));
        std::vector<std::string> missing;
        for (const auto& t : targets)
            if (force || !build_row_exists(db, mid, comp_id, c.mod, c.istd, t)) missing.push_back(t);
        if (missing.empty()) {
            std::cout << "  " << c.label << ": complete (" << targets.size() << " targets) — skip\n";
            safe_rmdir(source_dir, dir); ++skipped; continue;
        }
        std::cout << "  " << c.label << ": building " << missing.size() << " of " << targets.size() << " targets...\n";
        std::vector<std::string> build = {"cmake", "--build", "tmp_build/" + c.label};
        for (const auto& m : missing) { build.push_back("--target"); build.push_back(m); }
        if (run_process(build, 0, source_dir.string(), 0).exit_code != 0) {
            std::cout << "  " << c.label << ": BUILD FAILED (what did build is recorded)\n";
            ++failed;
        }
        { GlobalOptions rec; rec.source_dir = source_dir; capture_build_times(rec, dir, c.mod, c.istd); }
        built_total += static_cast<int>(missing.size());
        safe_rmdir(source_dir, dir);
    }
    safe_rmdir(source_dir, source_dir / "tmp_build");
    std::cout << "build-times gate: " << built_total << " target-builds recorded, " << skipped
              << " config(s) already complete for " << host << (failed ? ", " + std::to_string(failed) + " FAILED" : "")
              << ".\n";
    return failed ? 1 : 0;   // the pre-push hook stops on a broken compiler config
}

// Record one testRun row per ctest unit test into results.db.
std::int64_t record_ctest_results(const GlobalOptions& global, const fs::path& build_dir,
                                  const std::vector<TestResult>& results) {
    if (results.empty()) return 0;
    const fs::path db_path = JAC313_RESULTS_DB;
    std::error_code ec;
    if (!fs::exists(db_path.parent_path(), ec)) { not_recorded("ctest", "no " + db_path.parent_path().string()); return 0; }
    try {
        jac313::Qlite::v001::Sqlite db(db_path.string());
        const std::int64_t run_id = cli_begin_run(db, global);   // with --claim / --assign-new when given
        for (const auto& r : results) {
            db.exec("INSERT OR IGNORE INTO testList(name) VALUES(?)", r.entry.name);
        }
        const std::int64_t comp_id = cli_compiler_id(db, read_compiler_info(build_dir));
        const std::int64_t type_id = cli_results_id(db, "SELECT id FROM testType WHERE name=?", std::string("ctest"));
        const std::int64_t param_id = cli_parameter_id(db, comp_id, build_axes(read_build_features(build_dir)));
        auto tx = db.transaction();   // the rows + the safeness refresh commit together
        for (const auto& r : results) {
            const std::int64_t list_id = cli_results_id(db, "SELECT id FROM testList WHERE name=?", r.entry.name);
            db.exec("INSERT INTO testRun(run_id, test_type_id, test_list_id, parameter_id, status, duration_ms) "
                    "VALUES(?,?,?,?,?,?)",
                    run_id, type_id, list_id, param_id, std::string(matrix_status_str(r.status)),
                    static_cast<std::int64_t>(r.duration.count()));
            // a test that persisted says what it wrote where (TARGET lines)
            jac313::results::insert_run_targets(db, run_id, jac313::results::parse_target_lines(r.stdout_tail));
        }
        jac313::results::rebuild_safeness(db);   // refresh the safeness summary (read by report + verdict)
        tx.commit();
        std::cout << "[results] recorded " << results.size() << " ctest results (run " << run_id << ") -> "
                  << db_path.string() << '\n';
        return run_id;
    } catch (const std::exception& e) {
        not_recorded("ctest", e.what());
    }
    return 0;
}

// Record one testRun row per scenario of a matrix (smoke) run into results.db.
std::int64_t record_matrix_results(const GlobalOptions& global, const fs::path& build_dir,
                                   const std::vector<MatrixRunResult>& results, const std::string& size,
                                   std::int64_t max_persist_rows = 0) {
    if (results.empty()) return 0;
    const fs::path db_path = JAC313_RESULTS_DB;
    std::error_code ec;
    if (!fs::exists(db_path.parent_path(), ec)) { not_recorded("smoke", "no " + db_path.parent_path().string()); return 0; }
    try {
        jac313::Qlite::v001::Sqlite db(db_path.string());
        const std::int64_t run_id = cli_begin_run(db, global);   // with --claim / --assign-new when given
        for (const auto& r : results) {
            db.exec("INSERT OR IGNORE INTO testList(name) VALUES(?)", r.scenario.entry.name);
        }
        const std::int64_t type_id = cli_results_id(db, "SELECT id FROM testType WHERE name=?", std::string("smoke"));
        const std::int64_t comp_id = cli_compiler_id(db, read_compiler_info(build_dir));
        const BuildAxes axes = build_axes(read_build_features(build_dir));
        auto tx = db.transaction();   // the rows + the safeness refresh commit together
        for (const auto& r : results) {
            const std::int64_t list_id = cli_results_id(db, "SELECT id FROM testList WHERE name=?", r.scenario.entry.name);
            const std::int64_t param_id = cli_parameter_id(db, comp_id, axes, size,
                r.scenario.persist, r.scenario.output_mode,
                static_cast<std::int64_t>(r.scenario.threads), static_cast<std::int64_t>(r.scenario.events_per_thread),
                static_cast<std::int64_t>(r.scenario.runs), "", r.batch);
            // the persist row cap applies to a scenario that persists (0 for the rest)
            const bool persisting = r.scenario.category == "matrix" && r.scenario.persist != "none"
                                    && r.scenario.persist != "flags" && r.scenario.persist != "unit";
            // the batch the test reported (its BATCH line), filed under its sink: a database (sql |
            // pg-*), the binary log, or a keeper-style file (jtext | html | json)
            const std::string& per = r.scenario.persist;
            const bool is_db = per == "sql" || per == "pg-local" || per == "pg-remote";
            const std::int64_t b_binary = per == "binary" ? r.batch : 0;
            const std::int64_t b_database = is_db ? r.batch : 0;
            const std::int64_t b_keeper = (per != "binary" && !is_db) ? r.batch : 0;
            db.exec("INSERT INTO testRun(run_id, test_type_id, test_list_id, parameter_id, status, duration_ms, "
                    "max_persist_rows, batch_keeper, batch_database, batch_binary) VALUES(?,?,?,?,?,?,?,?,?,?)",
                    run_id, type_id, list_id, param_id, std::string(matrix_status_str(r.result.status)),
                    static_cast<std::int64_t>(r.result.duration.count()), persisting ? max_persist_rows : std::int64_t{0},
                    b_keeper, b_database, b_binary);
            const std::int64_t test_run_id = sqlite3_last_insert_rowid(db.handle());
            // what the scenario wrote where (its TARGET lines)
            jac313::results::insert_run_targets(db, run_id, jac313::results::parse_target_lines(r.result.stdout_tail));
            if (!r.readback.empty()) {
                // read-back outcome (tests/matrix/readback.hpp) on the row just inserted
                auto num = [&](const char* k) -> double {   // a missing/unparsable figure is 0, never NULL
                    auto it = r.readback.find(k);
                    if (it == r.readback.end()) return 0.0;
                    try { return std::stod(it->second); } catch (...) { return 0.0; }
                };
                auto get = [&](const char* k) { auto it = r.readback.find(k); return it == r.readback.end() ? std::string{} : it->second; };
                db.exec("UPDATE testRun SET rows_persisted=?, readback_mode=?, readback_status=?, "
                        "rows_checked=?, read_ms=?, pg_connect_ms=?, pg_ping_ms=?, pg_write_ms=? "
                        "WHERE id = ?",
                        static_cast<std::int64_t>(num("expected")), get("mode"), get("status"),
                        static_cast<std::int64_t>(num("checked")), num("read_ms"),
                        num("connect_ms"), num("ping_ms"), num("write_ms"), test_run_id);
            }
        }
        jac313::results::rebuild_safeness(db);   // refresh the safeness summary (read by report + verdict)
        tx.commit();
        std::cout << "[results] recorded " << results.size() << " smoke results (run " << run_id << ") -> "
                  << db_path.string() << '\n';
        return run_id;
    } catch (const std::exception& e) {
        not_recorded("smoke", e.what());
    }
    return 0;
}

RunWhere run_where(std::int64_t run_id) {
    RunWhere w;
    if (run_id <= 0) return w;
    try {
        jac313::Qlite::v001::Sqlite db(JAC313_RESULTS_DB);
        auto st = db.prepare("SELECT r.machine_id, o.name, o.version, o.kernel FROM run r "
                             "INNER JOIN os o ON o.id = r.os_id WHERE r.run_id=?");
        st.bind(run_id);
        if (st.step()) {
            std::int64_t mid = 0; std::string name, version, kernel;
            st.get(mid, name, version, kernel);
            w.host = jac313::results::host_label(mid);
            w.os = jac313::results::os_display(name, version) + (kernel.empty() ? std::string() : " · " + kernel);
        }
    } catch (...) {}
    return w;
}

// Short test name for the summary: "jac313_store_006_TS" -> "006_TS".
std::string short_test_name(const std::string& n) {
    for (const char* p : {"jac313_store_", "jac313_"}) {
        if (n.rfind(p, 0) == 0) return n.substr(std::string(p).size());
    }
    return n;
}

int run_matrix_run_command(const GlobalOptions& global,
                           MatrixOptions matrix_opts,
                           const RunOptions& run_opts,
                           const int build_jobs = 0,
                           MatrixComboTally* out_tally = nullptr)
{
    const auto tests = discover_tests(global.build_dir);
    if (tests.empty()) {
        std::cerr << "No tests found under " << global.build_dir << '\n';
        return 1;
    }

    const fs::path params_path = global.source_dir / matrix_opts.params_file;
    MatrixParams params = load_matrix_params(params_path);
    // --size / --persist / --max-persist-rows / --readback / --pg-cleanup (Jac313Runner options)
    if (!global.size_override.empty()) params.size = global.size_override;
    if (!global.persist_list.empty()) params.persist_list = split_list(global.persist_list);
    params.extra_test_args = matrix_test_args(global);
    auto scenarios = build_matrix_scenarios(
        tests, params, matrix_opts.compiler_label, global.build_dir, run_opts.filter);
    // --max-runs / --max-records: lower each test's runs and events per run (never raise them).
    // A zero scaling means "the test's own default" (test_options.hpp: smoke 5 x 20 x 1, full 250 x 4000 x 50).
    if (global.max_runs > 0 || global.max_records > 0) {
        const bool full = params.size == "full" || params.size == "FULL";
        for (auto& sc : scenarios) {
            if (sc.category != "matrix" || sc.persist == "unit") continue;
            std::size_t t = sc.threads ? sc.threads : (full ? 250 : 5);
            std::size_t e = sc.events_per_thread ? sc.events_per_thread : (full ? 4000 : 20);
            std::size_t r = sc.runs ? sc.runs : (full ? 50 : 1);
            if (global.max_runs > 0 && r > global.max_runs) { r = global.max_runs; sc.runs = r; }
            if (global.max_records > 0 && t * e > global.max_records) {
                if (t > global.max_records) { t = global.max_records; e = 1; }
                else e = std::max<std::size_t>(1, global.max_records / t);
                sc.threads = t;
                sc.events_per_thread = e;
            }
        }
        std::cout << "Limits: max runs " << (global.max_runs ? format_count(global.max_runs) : std::string("-"))
                  << ", max records/run " << (global.max_records ? format_count(global.max_records) : std::string("-"))
                  << '\n';
    }

    if (scenarios.empty()) {
        std::cerr << "No matrix scenarios selected (check " << params_path;
        if (!run_opts.filter.empty()) {
            std::cerr << " and --filter \"" << run_opts.filter << '"';
        }
        std::cerr << ")\n";
        return 1;
    }

    // Ground truth for Debug vs Release: read CMAKE_BUILD_TYPE from the tree under test.
    const BuildFeatures features = read_build_features(global.build_dir);
    const std::string build_type = features.build_type.empty() ? "Debug" : features.build_type;
    const std::string modules_label = features.modules ? "modules" : "textual";

    const fs::path results_base = compute_results_base(
        global.source_dir, params, matrix_opts.compiler_label, features.dir_label(), modules_label);

    std::cout << "=== jac313 test matrix ===\n";
    std::cout << "Params:  " << params_path.string() << '\n';
    std::cout << "SIZE=" << params.size << " DISK=" << params.disk_type;
    std::cout << " COMPILER=" << matrix_opts.compiler_label
              << " BUILD=" << build_type << " CONTRACTS=" << features.contracts << '\n';
    if (!run_opts.filter.empty()) {
        std::cout << "Filter:  " << run_opts.filter << '\n';
    }
    const int failsafe_sec = effective_failsafe_sec(run_opts, params.size);
    const bool timeouts_off = run_opts.failsafe_sec.has_value() && *run_opts.failsafe_sec == 0;
    if (failsafe_sec > 0) {
        std::cout << "Failsafe: " << format_count(failsafe_sec) << "s per scenario without a testControl timeout\n";
    } else {
        std::cout << "Failsafe: off (--failsafe 0: no scenario timeout, testControl included)\n";
    }
    // Per-test limits (testControl, seeded from tests/test_limits.jtext) for the tests in this run.
    const TestControls controls = load_test_controls(global.source_dir);
    {
        std::set<std::string> shown;
        for (const auto& sc : scenarios) {
            const auto& n = sc.entry.name;
            const auto t = controls.timeout_sec.find(n);
            const auto m = controls.memory_mb.find(n);
            if ((t == controls.timeout_sec.end() && m == controls.memory_mb.end()) || !shown.insert(n).second) continue;
            std::cout << "Limit:   " << n << "  timeout "
                      << (timeouts_off ? std::string("off")
                                       : t == controls.timeout_sec.end() ? std::to_string(failsafe_sec) + "s (failsafe)"
                                                                         : std::to_string(t->second) + "s")
                      << "  memory " << (m == controls.memory_mb.end() ? std::string("none") : std::to_string(m->second) + " MB")
                      << "  (testControl)\n";
        }
    }
    std::cout << "Results: " << results_base.string() << '\n';
    std::cout << "Logs:    " << results_base.string() << "/…/*.log  (text only — safe to commit; see test-results/README.md)\n";
    print_keep_logs_status(global);

    // Global scenario accounting for a run-all sweep: TOTAL = combo_count * this
    // combo's scenario count (constant across combos). Surfaced so every line and
    // header shows ".. of TOTAL" and an inflated combo count is obvious at a glance.
    const int per_combo     = static_cast<int>(scenarios.size());
    const int grand_total   = (matrix_opts.combo_count > 0) ? matrix_opts.combo_count * per_combo : 0;
    const int global_before = (matrix_opts.combo_count > 0 && matrix_opts.combo_index > 0)
                                  ? (matrix_opts.combo_index - 1) * per_combo : 0;
    if (out_tally != nullptr) {
        out_tally->scenarios = per_combo;
    }
    std::cout << "Scenarios: " << format_count(per_combo);
    if (grand_total > 0) {
        std::cout << "  (combo " << matrix_opts.combo_index << '/' << matrix_opts.combo_count
                  << "  -  " << format_count(global_before + 1) << ".."
                  << format_count(global_before + per_combo) << " of "
                  << format_count(grand_total) << " scenarios)";
    }
    std::cout << "\n\n";

    if (matrix_opts.dry_run) {
        const int total = static_cast<int>(scenarios.size());
        int name_width = 0;
        for (const auto& s : scenarios) {
            if (static_cast<int>(s.entry.name.size()) > name_width) {
                name_width = static_cast<int>(s.entry.name.size());
            }
        }
        int index = 0;
        for (const auto& scen : scenarios) {
            std::cout << "  ";
            print_matrix_scenario_line(scen, ++index, total, false, 0, 0, name_width);
        }
        return 0;
    }

    if (!build_matrix_targets_before_run(global, scenarios, build_jobs, failsafe_sec)) {
        return kBuildPhaseFailed;   // build failed -> skip the tests for this combo
    }
    std::cout << '\n';

    MatrixRunOptions run_opts_matrix;
    run_opts_matrix.verbose = global.verbose;
    run_opts_matrix.fail_fast = run_opts.fail_fast;
    run_opts_matrix.failsafe_sec = failsafe_sec;
    run_opts_matrix.timeouts_off = timeouts_off;
    run_opts_matrix.controls = controls;
    run_opts_matrix.global_done_before = global_before;
    run_opts_matrix.global_total = grand_total;
    run_opts_matrix.keep_logs = global.keep_logs;
    run_opts_matrix.version_root = global.source_dir;
    run_opts_matrix.keep_logs_root = global.keep_logs_dir;

    const auto results = run_matrix(scenarios, params, results_base, run_opts_matrix);
    MatrixTally tally = tally_matrix_results(results);
    if (out_tally != nullptr) {
        out_tally->passed  = static_cast<int>(tally.passed);
        out_tally->failed  = static_cast<int>(tally.failed);
        out_tally->skipped = static_cast<int>(tally.skipped);
        out_tally->errors  = static_cast<int>(tally.errors);
    }

    std::cout << "\n=== matrix summary ===\n";
    std::cout << summary_context_line(global) << '\n';   // jac313-### · compiler · disk
    // Workload (constant per run) — shown once here, spelled out, instead of on every scenario line.
    for (const auto& r : results) if (r.scenario.threads > 0) {
        std::cout << "Workload: threads=" << format_count(r.scenario.threads)
                  << "  events/thread=" << format_count(r.scenario.events_per_thread)
                  << "  runs=" << format_count(r.scenario.runs) << '\n';
        break;
    }
    std::cout << "Passed:  " << format_count_padded(tally.passed) << '\n';
    std::cout << "Failed:  " << format_count_padded(tally.failed) << '\n';
    std::cout << "Skipped: " << format_count_padded(tally.skipped) << '\n';
    std::cout << "Errors:  " << format_count_padded(tally.errors) << '\n';
    std::cout << "\n\n";   // two blank lines after the smoke summary

    // Capture this smoke run into results.db (one testRun row per scenario).
    const std::int64_t smoke_run = record_matrix_results(global, global.build_dir, results, params.size,
                          static_cast<std::int64_t>(global.max_persist_rows.value_or(0)));
    if (!global.summary_json.empty()) {
        const RunWhere where = run_where(smoke_run);
        for (const auto& r : results) {
            const bool persisting = r.scenario.category == "matrix" && r.scenario.persist != "none"
                                    && r.scenario.persist != "flags" && r.scenario.persist != "unit";
            std::string obj = std::format(
                R"({{"gate":"smoke","test":"{}","backend":"{}","output_mode":"{}","status":"{}","ms":{},"compiler":"{}","run_id":{},"host":"{}","os":"{}","log":"{}")",
                json_escape(short_test_name(r.scenario.entry.name)), json_escape(r.scenario.persist),
                r.scenario.output_mode, summary_status(r.result.status), r.result.duration.count(),
                json_escape(matrix_opts.compiler_label), smoke_run, json_escape(where.host), json_escape(where.os),
                json_escape(r.log_path.string()));
            if (!r.readback.empty()) {
                obj += ",\"rows_written\":" + (r.readback.count("expected") ? r.readback.at("expected") : "0");
                obj += ",\"readback\":" + readback_json(r.readback);
            }
            if (persisting && !r.persist_base.empty()) {
                obj += std::format(R"(,"dataset":{{"backend":"{}","base":"{}"}})",
                                   json_escape(r.scenario.persist), json_escape(r.persist_base.string()));
            }
            append_summary_part(global, obj + "}");
        }
    }

    return (tally.failed > 0 || tally.errors > 0) ? 1 : 0;
}

// ---- cleancheck: valgrind memcheck + helgrind/DRD over the representative set ----
// A pre-push gate (smoke scale): memcheck the whole memory surface, helgrind+DRD the
// multithreaded surface (on an annotated tree). Verdict-only — no DB, no committed
// artifact; the point is PASS/FAIL + a non-zero exit so a dirty push is blocked.

struct VgTask {
    std::string label;
    fs::path bin;
    std::vector<std::string> args;
};

// Parse the integer from the last "ERROR SUMMARY: N errors ..." line. -1 if absent.
int parse_vg_errors(const std::string& text) {
    const std::string key = "ERROR SUMMARY:";
    const std::size_t at = text.rfind(key);
    if (at == std::string::npos) {
        return -1;
    }
    std::size_t pos = at + key.size();
    while (pos < text.size() && text[pos] == ' ') {
        ++pos;
    }
    int n = 0;
    bool any = false;
    while (pos < text.size() && text[pos] >= '0' && text[pos] <= '9') {
        n = n * 10 + (text[pos] - '0');
        ++pos;
        any = true;
    }
    return any ? n : -1;
}

int run_verify_command(GlobalOptions global, ConfigureOptions configure_opts,
                       BuildOptions build_opts, bool full, const std::string& name)
{
    std::cout << "=== " << name << " — valgrind memcheck + helgrind/DRD"
              << (full ? " (ctest + smoke matrix — full sink coverage)" : " (ctest representative set)")
              << " ===\n\n";

    // 1. valgrind must be present (it's the whole point of the gate).
    const auto vg_probe = run_process({"valgrind", "--version"});
    if (!vg_probe.started || vg_probe.exit_code != 0) {
        std::cerr << "valgrind not found on PATH — " << name << " requires it.\n"
                     "  Debian/Mint:  sudo apt-get install -y valgrind\n"
                     "  RHEL/Fedora:  sudo dnf install -y valgrind\n";
        return 1;
    }
    std::string vg_version = vg_probe.stdout_text;
    while (!vg_version.empty() && (vg_version.back() == '\n' || vg_version.back() == '\r')) {
        vg_version.pop_back();
    }
    std::cout << "valgrind: " << vg_version << "\n\n";

    // 2. Configure + build a dedicated annotated Debug tree (so helgrind/DRD see the
    //    lock-free handoff and the annotation flag never pollutes the normal build dir).
    configure_opts.build_type = "Debug";
    configure_opts.modules = false;
    configure_opts.store_tests = true;
    configure_opts.helgrind_annotate = true;
    MatrixOptions matrix_opts;  // compiler_label sink only
    if (!apply_compiler_resolution(global, configure_opts, matrix_opts)) {
        return 1;
    }
    global.build_dir = global.build_dir.string()
                     + build_flavor_suffix(configure_opts.strict, configure_opts.hardened, configure_opts.contracts)
                     + "-" + name;
    std::cout << "Build dir:  " << global.build_dir.string()
              << " (annotated: JAC313_STORE_HELGRIND_ANNOTATE=ON)\n\n";
    if (const int rc = run_configure_command(global, configure_opts); rc != 0) {
        return rc;
    }
    if (const int rc = run_build_command(global, build_opts); rc != 0) {
        return rc;
    }

    // 3. Scratch working dir so persist_* output never lands in the repo.
    // One scratch dir per process (two verifies at once, gcc and clang or two users, must not
    // delete each other's files), removed when this function returns.
    const fs::path scratch = fs::temp_directory_path() / ("jac313-cleancheck-" + std::to_string(::getpid()));
    std::error_code ec;
    fs::remove_all(scratch, ec);
    fs::create_directories(scratch, ec);
    struct ScratchCleanup {
        fs::path dir;
        ~ScratchCleanup() { std::error_code e; fs::remove_all(dir, e); }
    } scratch_cleanup{scratch};
    const std::string scratch_s = scratch.string();

    const fs::path build_abs = fs::absolute(global.build_dir);
    const fs::path mxdir = build_abs / "Store" / "tests" / "matrix";
    const fs::path exdir = build_abs / "Store" / "examples";
    const fs::path utdir = build_abs / "Store" / "tests";
    const std::vector<std::string> SMOKE = {"--no-interactive", "--color=0", "--test-size=smoke"};

    auto mx_args = [&](const std::string& persist, const std::string& tag) {
        std::vector<std::string> a = SMOKE;
        a.push_back("--persist=" + persist);
        a.push_back("--base-name=" + (scratch / tag).string());
        return a;
    };

    // Create run early so live status events go to runEvent (watch in DataGrip while the
    // long valgrind run is in progress). Reuse the same run_id for the final testRun inserts.
    std::int64_t run_id = 0;
    const fs::path db_path = JAC313_RESULTS_DB;
    try {
        jac313::Qlite::v001::Sqlite db(db_path.string());
        run_id = cli_begin_run(db, global);
        jac313::results::log_run_event(db, run_id, "", "run_started", name + " starting");
    } catch (...) {}

    // memcheck: the full memory surface.
    std::vector<VgTask> mem;
    mem.push_back({"ut:v001_test", utdir / "jac313_store_v001_test", {}});
    mem.push_back({"ut:binary_reader_test", utdir / "jac313_store_binary_reader_test", {}});
    mem.push_back({"ex:binary_persist", exdir / "jac313_store_binary_persist_smoke", {}});
    mem.push_back({"ex:binary_to_jtext", exdir / "jac313_store_binary_to_jtext_smoke", {}});
    mem.push_back({"ex:jtext_persist", exdir / "jac313_store_jtext_persist_smoke", {}});
    mem.push_back({"ex:sql_persist", exdir / "jac313_store_sql_persist_smoke", {}});
    mem.push_back({"ex:flag_routing", exdir / "jac313_store_flag_routing_smoke", {}});
    mem.push_back({"ex:v001_example", exdir / "jac313_store_v001_example", {}});
    // matrix memcheck surface: lite = binary (+ 001 across sinks); full = every sink (full coverage).
    const std::vector<std::string> sinks =
        full ? std::vector<std::string>{"binary", "jtext", "html", "json", "sql", "none", "inmem"}
             : std::vector<std::string>{"binary"};
    for (const auto* n : {"001", "002", "003", "004", "005", "006", "007", "008"}) {
        for (const std::string& p : sinks) {
            mem.push_back({std::string("mx:") + n + "_TS:" + p,
                           mxdir / (std::string("jac313_store_") + n + "_TS"),
                           mx_args(p, std::string("m") + n + p.substr(0, 1))});
        }
    }
    if (!full) {
        mem.push_back({"mx:001_TS:jtext", mxdir / "jac313_store_001_TS", mx_args("jtext", "m001j")});
        mem.push_back({"mx:001_TS:sql", mxdir / "jac313_store_001_TS", mx_args("sql", "m001s")});
        mem.push_back({"mx:001_TS:none", mxdir / "jac313_store_001_TS", mx_args("none", "m001n")});
    }
    mem.push_back({"mx:flags", mxdir / "jac313_store_flags", SMOKE});
    mem.push_back({"mx:metric_view", mxdir / "jac313_store_metric_view", SMOKE});

    // helgrind + DRD: the multithreaded surface (lock-free path + persistence path).
    std::vector<VgTask> thr;
    thr.push_back({"mx:003_TS:none", mxdir / "jac313_store_003_TS", mx_args("none", "t003")});
    thr.push_back({"mx:006_TS:none", mxdir / "jac313_store_006_TS", mx_args("none", "t006")});
    thr.push_back({"mx:001_TS:binary", mxdir / "jac313_store_001_TS", mx_args("binary", "t001b")});
    thr.push_back({"mx:001_TS:jtext", mxdir / "jac313_store_001_TS", mx_args("jtext", "t001j")});
    thr.push_back({"mx:001_TS:sql", mxdir / "jac313_store_001_TS", mx_args("sql", "t001s")});

    int clean = 0, vgfail = 0, testfail = 0;
    std::vector<std::string> failures;
    struct VerifyRec { std::string tool, test, persist, status; std::int64_t dur_ms; };
    std::vector<VerifyRec> recs;   // one per (tool x task) — captured into results.db below
    std::vector<jac313::results::RunTarget> targets;   // what the tasks wrote where (their TARGET lines)

    // Aligned console output (match the ctest/smoke runners): pad the tool and the task
    // label to fixed columns — label to the widest across ALL tasks (mem ∪ thr), so every
    // section lines up in the same column — and right-pad the index + duration so numbers stack.
    std::size_t label_w = 0;
    for (const auto& t : mem) label_w = std::max(label_w, t.label.size());
    for (const auto& t : thr) label_w = std::max(label_w, t.label.size());
    const std::size_t tool_w = 8;  // widest of memcheck / helgrind / drd
    const auto ljust = [](const std::string& s, std::size_t w) {
        return s.size() < w ? s + std::string(w - s.size(), ' ') : s;
    };
    // One CONTINUOUS index across all three sections (memcheck + helgrind + drd) with a single
    // total + width, so the [i/N] bracket is identical width everywhere and the columns line up
    // ACROSS sections — helgrind/drd align under memcheck — like the smoke/ctest runners.
    const std::size_t vg_total = mem.size() + thr.size() + thr.size();
    const std::size_t iw = format_count(static_cast<std::int64_t>(vg_total)).size();
    // Pad "[tool] label" as ONE segment so a compact tag (e.g. [drd], not [drd     ]) keeps the
    // '...' column aligned — the spaces a short tag would need land after the label instead.
    const std::size_t seg_w = 1 + tool_w + 2 + label_w;
    std::size_t vg_idx = 0;

    auto run_one = [&](const std::string& tool, const VgTask& t) {
        ++vg_idx;
        std::string persist;
        for (const auto& a : t.args) if (a.rfind("--persist=", 0) == 0) persist = a.substr(10);
        const std::string test = t.bin.filename().string();

        // Live per-task status (for DataGrip / runEvent while valgrinding)
        if (run_id > 0) {
            try {
                jac313::Qlite::v001::Sqlite db(db_path.string());
                jac313::results::log_run_event(db, run_id, t.label, "started");
            } catch (...) {}
        }

        if (!fs::exists(t.bin)) {
            std::cout << "  [" << format_count_padded(static_cast<std::int64_t>(vg_idx), iw) << "/" << vg_total
                      << "] " << ljust("[" + tool + "] " + t.label, seg_w)
                      << " ... MISSING BINARY\n";
            ++testfail;
            failures.push_back(tool + " " + t.label + " [missing]");
            recs.push_back({tool, test, persist, "error", 0});
            if (run_id > 0) {
                try {
                    jac313::Qlite::v001::Sqlite db(db_path.string());
                    jac313::results::log_run_event(db, run_id, t.label, "error", "MISSING BINARY");
                } catch (...) {}
            }
            return;
        }
        std::vector<std::string> cmd = {"valgrind", "--tool=" + tool, "--error-exitcode=99"};
        // Scrub known false positives (std::atomic load/store — helgrind/DRD can't model C++11
        // atomics) BEFORE counting/recording, so results.db only ever holds real errors.
        { const fs::path supp = global.source_dir / "suppressions" / "jac313.supp";
          std::error_code sec;
          if (std::filesystem::exists(supp, sec)) cmd.push_back("--suppressions=" + supp.string()); }
        if (tool == "memcheck") {
            cmd.push_back("--leak-check=full");
            cmd.push_back("--errors-for-leak-kinds=definite,indirect");
        }
        cmd.push_back(t.bin.string());
        for (const auto& a : t.args) {
            cmd.push_back(a);
        }
        const auto t0 = std::chrono::steady_clock::now();
        const auto r = run_process(cmd, 300, scratch_s);
        const auto dur_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::steady_clock::now() - t0).count();
        const int errs = parse_vg_errors(r.stderr_text + r.stdout_text);
        for (auto& tg : jac313::results::parse_target_lines(r.stdout_text)) targets.push_back(std::move(tg));
        std::string verdict, status;
        if (!r.started) {
            verdict = "LAUNCH-FAIL"; status = "error";
            ++testfail;
            failures.push_back(tool + " " + t.label + " [launch]");
        } else if (r.exit_code == 99) {
            verdict = "VALGRIND ERRORS (" + std::to_string(errs) + ")"; status = "fail";
            ++vgfail;
            failures.push_back(tool + " " + t.label + " [vg:" + std::to_string(errs) + "]");
        } else if (r.exit_code != 0) {
            verdict = "TEST FAIL (rc=" + std::to_string(r.exit_code) + ")"; status = "fail";
            // rc 126 + "Permission denied": valgrind refused to start the binary (it checks the
            // mode bits itself; as root it uses the "other" bits). Say so instead of a bare rc.
            if (r.exit_code == 126 &&
                (r.stderr_text + r.stdout_text).find("Permission denied") != std::string::npos)
                verdict += ::geteuid() == 0
                    ? " valgrind could not start it: running as root, and the binary has no"
                      " \"other\" execute bit"
                    : " valgrind could not start it: Permission denied (check the binary's mode)";
            ++testfail;
            failures.push_back(tool + " " + t.label + " [rc:" + std::to_string(r.exit_code) + "]");
        } else {
            verdict = "clean"; status = "pass";
            ++clean;
        }
        recs.push_back({tool, test, persist, status, static_cast<std::int64_t>(dur_ms)});

        if (run_id > 0) {
            try {
                jac313::Qlite::v001::Sqlite db(db_path.string());
                std::string ev = (status == "pass") ? "ended" : "error";
                std::string msg = verdict;
                const std::string combined = r.stderr_text + r.stdout_text;
                if (!combined.empty()) {
                    std::string tail = combined.substr(0, 400);
                    if (combined.size() > 400) tail += "...";
                    msg += " | " + tail;
                }
                jac313::results::log_run_event(db, run_id, t.label, ev, msg, static_cast<std::int64_t>(dur_ms));
            } catch (...) {}
        }

        std::cout << "  [" << format_count_padded(static_cast<std::int64_t>(vg_idx), iw) << "/" << vg_total
                  << "] " << ljust("[" + tool + "] " + t.label, seg_w)
                  << " ... " << verdict
                  << " (" << format_count_padded(static_cast<std::int64_t>(dur_ms)) << " ms)\n";
    };

    std::cout << "--- memcheck (" << mem.size() << ") ---\n";
    for (const auto& t : mem) run_one("memcheck", t);
    std::cout << "\n--- helgrind (" << thr.size() << ") ---\n";
    for (const auto& t : thr) run_one("helgrind", t);
    std::cout << "\n--- drd (" << thr.size() << ") ---\n";
    for (const auto& t : thr) run_one("drd", t);

    const int total = clean + vgfail + testfail;
    std::cout << "\n=== " << name << " summary ===\n"
              << "valgrind:  " << vg_version << '\n'
              << "total=" << total << "  clean=" << clean
              << "  valgrind-errors=" << vgfail << "  test-failures=" << testfail << '\n';
    if (!failures.empty()) {
        std::cout << "FAILURES:\n";
        for (const auto& f : failures) {
            std::cout << "  - " << f << '\n';
        }
    }
    const bool ok = (vgfail == 0 && testfail == 0);
    std::cout << (ok ? (name + " PASS\n") : (name + " FAIL\n"));

    if (run_id > 0) {
        try {
            jac313::Qlite::v001::Sqlite db(db_path.string());
            jac313::results::log_run_event(db, run_id, "", "run_ended", name + (ok ? " complete" : " failed"));
        } catch (...) {}
    }

    // Capture each (tool x task) into results.db — test_type = this gate's name (verify-lite|verify),
    // valgrind tool as a parameter, status pass/fail/error + duration. Best-effort.
    {
        const fs::path rpath = JAC313_RESULTS_DB;
        std::error_code rec;
        if (!recs.empty() && !fs::exists(rpath.parent_path(), rec)) not_recorded(name, "no " + rpath.parent_path().string());
        if (!recs.empty() && fs::exists(rpath.parent_path(), rec)) {
            try {
                jac313::Qlite::v001::Sqlite rdb(rpath.string());
                std::int64_t use_run = run_id;
                if (use_run == 0) {
                    use_run = cli_begin_run(rdb, global);
                }
                const std::int64_t comp_id = cli_compiler_id(rdb, read_compiler_info(global.build_dir));
                const std::int64_t type_id = cli_results_id(rdb, "SELECT id FROM testType WHERE name=?", name);
                const BuildAxes axes = build_axes(read_build_features(global.build_dir));
                auto tx = rdb.transaction();   // the rows + the safeness refresh commit together
                for (const auto& v : recs) rdb.exec("INSERT OR IGNORE INTO testList(name) VALUES(?)", v.test);
                for (const auto& v : recs) {
                    const std::int64_t list_id  = cli_results_id(rdb, "SELECT id FROM testList WHERE name=?", v.test);
                    const std::int64_t param_id = cli_parameter_id(rdb, comp_id, axes, "", v.persist, "", 0, 0, 0,
                                                                   v.tool);
                    rdb.exec("INSERT INTO testRun(run_id, test_type_id, test_list_id, parameter_id, status, duration_ms) "
                             "VALUES(?,?,?,?,?,?)", use_run, type_id, list_id, param_id, v.status, v.dur_ms);
                }
                jac313::results::insert_run_targets(rdb, use_run, targets);
                jac313::results::rebuild_safeness(rdb);   // refresh the safeness summary (read by report + verdict)
                tx.commit();
                std::cout << "[results] recorded " << recs.size() << " " << name << " results (run " << use_run
                          << ") -> " << rpath.string() << '\n';
            } catch (const std::exception& e) {
                not_recorded(name, e.what());
            }
        }
    }
    return ok ? 0 : 1;
}

// ---- version-check: enforce a version() bump when a package's shipped code changes ----
// Rule: for each package, if its shipped source changed since the commit that last SET its
// version() literal, a bump is owed. Range-independent (compares HEAD to that commit) and
// git-only (no build), so it runs fast in the pre-push hook ahead of the slow cleancheck.
int run_version_check_command(const GlobalOptions& global) {
    struct Pkg { std::string name; std::string version_file; std::vector<std::string> source; };
    const std::vector<Pkg> pkgs = {
        {"Qlite", "jText/Qlite/include/jac313/Qlite/v001/Database.body.hpp", {"jText/Qlite/include"}},
        {"jText", "jText/jText.api.inc",
            {"jText/include", "jText/src", "jText/jText.h", "jText/jText.cpp",
             "jText/jText.api.inc", "jText/jText.impl.inc"}},
        {"Store", "Store/include/jac313/Store/v001/headers/ts_store_config.hpp", {"Store/include"}},
    };
    const std::string dir = global.source_dir.string();
    std::cout << "=== version-check — version() bump owed? ===\n";
    int owed = 0;
    for (const auto& p : pkgs) {
        // Last commit that touched the version() literal ("vNNN.NNN") in this file. The regex
        // "v[0-9] is basic/extended-regex safe (no parens) and survives a future v002 bump (here v001).
        auto log = run_process({"git", "-C", dir, "log", "-1", "--format=%H",
                                "-G\"v[0-9]", "--", p.version_file});
        std::string c = log.stdout_text;
        while (!c.empty() && (c.back() == '\n' || c.back() == '\r')) c.pop_back();
        if (c.empty()) {
            std::cout << "  " << p.name << ": version() not yet committed — skipped\n";
            continue;
        }
        std::vector<std::string> diff = {"git", "-C", dir, "diff", "--quiet", c, "HEAD", "--"};
        for (const auto& s : p.source) diff.push_back(s);
        const auto d = run_process(diff);
        if (d.exit_code == 1) {
            std::cout << "  " << p.name << ": NEEDS BUMP — shipped code changed since version set ("
                      << c.substr(0, 9) << ")\n";
            ++owed;
        } else if (d.exit_code == 0) {
            std::cout << "  " << p.name << ": ok\n";
        } else {
            std::cout << "  " << p.name << ": git error (rc=" << d.exit_code << ") — skipped\n";
        }
    }
    if (owed) {
        std::cout << "\n" << owed << " package(s) owe a bump. Bump jac313::<Pkg>::v001::version()\n"
                     "  (the \"vNNN.NNN\" literal in the package's version file), commit, and re-push.\n";
        return 1;
    }
    std::cout << "\nversion-check OK.\n";
    return 0;
}

// Shared run core for `matrix run` and `matrix runner`: resolve the compiler, build the
// tree (benchmarked module path when modules are on, plain configure+build otherwise),
// then run the matrix. The two front-ends differ only in how they fill the options.
int execute_matrix_run(GlobalOptions& global, ConfigureOptions& configure_opts,
                       MatrixOptions& matrix_opts, RunOptions& run_opts, BuildOptions& build_opts,
                       MatrixComboTally* out_tally = nullptr)
{
    if (!apply_compiler_resolution(global, configure_opts, matrix_opts)) {
        return 1;
    }
    if (configure_opts.modules || discover_tests(global.build_dir).empty()) {
        std::cout << "[matrix] configuring + building '" << global.build_dir.string()
                  << "' before run\n";
        if (const int rc = run_configure_command(global, configure_opts); rc != 0) {
            return kBuildPhaseFailed;   // configure failed -> tests skipped
        }
        if (const int rc = build_and_capture(global, build_opts); rc != 0) {
            return kBuildPhaseFailed;   // build failed -> tests skipped
        }
    }
    return run_matrix_run_command(global, matrix_opts, run_opts, build_opts.jobs, out_tally);
}

// Forward — defined with the compiler-pin helpers below; presets need it early.
std::pair<std::string, std::string> pinned_or_highest_labels(const fs::path& source_dir);

// ---- preset gates: generate ./run_latest_config.sh, run it, leave it re-runnable ----
// The composable flags (--ctest/--smoke/--bench/--report/--verify[-lite]) expand into a
// concrete, ordered command list written to ./run_latest_config.sh at the source root. The
// CLI then runs that script and leaves it in place so the user can re-run the exact same
// commands by hand; the next preset invocation overwrites it.
struct PresetOptions {
    bool ctest{false};
    bool smoke{false};
    bool bench{false};
    bool report{false};        // modifies --bench: also record the DB + render the report
    bool verify_lite{false};
    bool verify{false};
    bool machine_id{false};    // read-only precheck: which jac313-### (machine) this machine records under
    bool run_everything{false}; // the full battery on both compilers + matrix + report (C++ orchestrated)
};

// Single-quote a value for the generated shell lines (run_latest_config.sh, run-everything steps).
std::string shell_quote(const std::string& s) {
    std::string q = "'";
    for (char c : s) q += (c == '\'') ? std::string("'\\''") : std::string(1, c);
    return q + "'";
}

// Every flag the user gave a preset (--ctest/--smoke/--bench/--verify[-lite]) or --run-everything,
// re-spelled for each generated step that takes it. A flag a step cannot take is never dropped
// silently: `notes` holds one "# ..." comment line saying where it does not apply and why.
struct StepFlags {
    std::vector<std::string> labels;   // the compilers to run (pinned/highest; --compiler / --gcc / --clang narrow)
    std::string flavor;                // build-dir suffix of a strict / hardened / contracts tree
    std::string host;                  // "host --claim X" / "--assign-new N" step args ('' = none)
    std::string configure;             // the ctest/smoke tree's configure flags
    std::string build;                 // build flags (-j N)
    std::string ctest;                 // `run` flags
    std::string smoke;                 // `matrix run` flags (without --compiler: added per label)
    std::string verify;                // `matrix verify[-lite]` flags (without --compiler)
    std::string bench_configure;       // the Release bench tree's configure flags (without --compiler)
    std::string bench;                 // store_bench flags
    std::vector<std::string> notes;
};

StepFlags step_flags(const GlobalOptions& g, const ConfigureOptions& c, const BuildOptions& b,
                     const RunOptions& r, const MatrixOptions& m, const std::string& gcc_label,
                     const std::string& clang_label, bool want_ctest_smoke, bool want_verify, bool want_bench) {
    StepFlags f;
    if (c.compiler) f.labels = {*c.compiler};
    else if (c.prefer_gcc && !c.prefer_clang) f.labels = {gcc_label};
    else if (c.prefer_clang && !c.prefer_gcc) f.labels = {clang_label};
    else f.labels = {gcc_label, clang_label};
    f.flavor = build_flavor_suffix(c.strict, c.hardened, c.contracts);

    if (!g.host_claim.empty()) f.host = " --claim " + shell_quote(g.host_claim);
    else if (g.host_assign_new > 0) f.host = " --assign-new " + std::to_string(g.host_assign_new);

    std::string flavor_flags;   // strict / hardened / contracts: every tree that is built takes them
    if (c.strict) flavor_flags += " --strict";
    if (c.hardened) flavor_flags += " --hardened";
    if (c.contracts != "enforce") flavor_flags += " --contracts " + c.contracts;

    if (c.build_type == "Release") f.configure += " --release";
    if (c.modules) f.configure += " --modules";
    f.configure += flavor_flags;
    if (!c.store_tests) f.configure += " --no-store-tests";
    if (b.jobs > 0) f.build = " -j " + std::to_string(b.jobs);

    std::string pg;   // the servers (also exported at the top of the script / in the environment)
    if (!g.connections.empty()) pg += " --connections " + shell_quote(fs::absolute(g.connections).string());
    if (!g.pg_remote.empty()) pg += " --pg-remote " + shell_quote(g.pg_remote);
    if (!g.pg_local.empty()) pg += " --pg-local " + shell_quote(g.pg_local);

    std::string run_sel;   // test selection and time limits (ctest and smoke)
    if (!r.filter.empty()) run_sel += " --filter " + shell_quote(r.filter);
    if (r.fail_fast) run_sel += " --fail-fast";
    if (r.failsafe_sec) run_sel += " --failsafe " + std::to_string(*r.failsafe_sec);

    const std::string psfx = persist_cli_suffix(g);
    f.ctest = run_sel + pg + psfx;
    const bool params_given = m.params_file != MatrixOptions{}.params_file;
    f.smoke = (params_given ? " --params " + shell_quote(m.params_file.string()) : std::string())
            + run_sel + pg + keep_logs_cli_suffix(g) + psfx;
    f.verify = flavor_flags + f.build;
    f.bench_configure = flavor_flags;
    f.bench = bench_persist_args(g);
    if (g.bench_threads) f.bench += " --threads " + std::to_string(g.bench_threads);
    if (g.bench_events_per_thread) f.bench += " --events-per-thread " + std::to_string(g.bench_events_per_thread);
    if (g.bench_runs) f.bench += " --runs " + std::to_string(g.bench_runs);
    if (g.bench_batch) f.bench += " --batch " + std::to_string(g.bench_batch);

    // Where a given flag does not apply, and why (one comment line each).
    auto note = [&](bool given, const std::string& flag, std::vector<std::pair<bool, std::string>> gates,
                    const std::string& why) {
        if (!given) return;
        std::string where;
        for (const auto& [active, name] : gates) if (active) where += (where.empty() ? "" : ", ") + name;
        if (!where.empty()) f.notes.push_back("# " + flag + ": not applied to " + where + " (" + why + ")");
    };
    const bool vg = want_verify, bn = want_bench, cs = want_ctest_smoke;
    note(c.build_type == "Release", "--release", {{vg, "verify"}}, "the valgrind gate builds Debug");
    note(c.modules, "--modules", {{vg, "verify"}, {bn, "bench"}}, "verify and the bench build the textual front end");
    note(!c.store_tests, "--no-store-tests", {{vg, "verify"}, {bn, "bench"}}, "they run the store tests / store_bench");
    note(!r.filter.empty(), "--filter", {{vg, "verify"}, {bn, "bench"}}, "fixed test sets");
    note(r.fail_fast, "--fail-fast", {{vg, "verify"}, {bn, "bench"}}, "a bench or valgrind gate runs every config");
    note(r.failsafe_sec.has_value(), "--failsafe", {{vg, "verify"}, {bn, "bench"}}, "no per-test timeout there");
    note(params_given, "--params", {{cs, "ctest"}, {vg, "verify"}, {bn, "bench"}}, "only the smoke matrix reads a params file");
    note(!g.size_override.empty(), "--size", {{vg, "verify"}, {bn, "bench"}},
         "verify runs the smoke scale; the bench scale is --max-records / --threads / --events-per-thread");
    note(!g.persist_list.empty() || g.max_persist_rows || g.max_runs || g.max_records || !g.readback.empty()
             || !g.pg_cleanup.empty(), "--persist/--max-*/--readback/--pg-cleanup", {{vg, "verify"}},
         "the valgrind gate runs its fixed sink set");
    note(g.bench_threads || g.bench_events_per_thread || g.bench_runs || g.bench_batch,
         "--threads/--events-per-thread/--runs/--batch", {{cs, "ctest, smoke"}, {vg, "verify"}}, "store_bench axes");
    note(!pg.empty(), "--pg-remote/--pg-local/--connections", {{vg, "verify"}, {bn, "bench"}},
         "as flags; they reach every step through the JAC313_PG_* environment");
    if (bn) f.notes.push_back("# bench: always a Release tree (a throughput number from a Debug tree means nothing)");
    if (vg) f.notes.push_back("# verify: always a Debug, textual tree (the valgrind gate)");
    return f;
}

// The bench's production run (store_bench --production): --production SPEC as given, else the
// one row of tests/production.jtext (it serves the smoke, full and bench profiles). A file
// without a valid row falls back to store_bench's defaults (jtext, sql, 10, 10, binary on), with
// a note.
std::string resolve_production_spec(const GlobalOptions& global, const fs::path& root) {
    if (!global.production.empty()) return global.production;
    const fs::path file = root / "tests" / "production.jtext";
    try {
        return load_production_settings(file).spec();
    } catch (const std::exception& e) {
        std::cerr << "note: " << e.what() << " — the production run uses its defaults\n";
        return ProductionSettings{}.spec();
    }
}

int run_preset_command(const GlobalOptions& global_in, const ConfigureOptions& configure_opts,
                       const BuildOptions& build_opts, const RunOptions& run_opts,
                       const MatrixOptions& matrix_opts, const PresetOptions& preset)
{
    // Max persisted in a run can never exceed Max records in a run (same rule as Jac313Runner).
    GlobalOptions global = global_in;
    if (preset.bench) global.production = resolve_production_spec(global, fs::absolute(global.source_dir));
    // pg-remote has no default server: stop before building anything if none was named.
    {
        const auto chosen = split_list(global.persist_list);
        const char* remote = std::getenv("JAC313_PG_REMOTE");
        const bool prod_remote = global.production.find("database=pg-remote") != std::string::npos;
        if ((prod_remote || std::find(chosen.begin(), chosen.end(), "pg-remote") != chosen.end()) && !(remote && *remote)) {
            std::cerr << "pg-remote chosen but no remote PostgreSQL server set: pass --pg-remote "
                         "\"host=... dbname=... user=...\" (or set $JAC313_PG_REMOTE)\n";
            return 2;
        }
    }
    if (global.max_records && global.max_persist_rows && *global.max_persist_rows > global.max_records) {
        std::cout << "note: --max-persist-rows " << *global.max_persist_rows << " limited to --max-records "
                  << global.max_records << "\n";
        global.max_persist_rows = global.max_records;
    }
    const fs::path root = fs::absolute(global.source_dir);

    // The script re-invokes THIS binary by its real absolute path, so it works whether or not
    // the convenience symlink (./jac313_test_cli) is present.
    fs::path self = root / "jac313_test_cli";
    {
        std::error_code ec;
        const fs::path exe = fs::read_symlink("/proc/self/exe", ec);
        if (!ec) {
            self = exe;
        }
    }

    // Preset gates run BOTH compilers (gcc first, then clang), using THIS machine's pinned
    // labels from Setup/compilers.pin.jtext — or highest-available from compilers.jtext when unpinned
    // (gcc16 on Fedora, gcc15 on RHEL/Mint, etc.). --gcc / --clang run just that one; --compiler L
    // runs exactly L.
    const auto [gcc_label, clang_label] = pinned_or_highest_labels(root);
    if (!configure_opts.compiler && (gcc_label.empty() || clang_label.empty())) {
        std::cerr << "preset: could not resolve gcc/clang for this machine "
                     "(run `jac313_test_cli pin --ensure`)\n";
        return 1;
    }
    const StepFlags sf = step_flags(global, configure_opts, build_opts, run_opts, matrix_opts, gcc_label, clang_label,
                                    preset.ctest || preset.smoke, preset.verify || preset.verify_lite, preset.bench);
    const std::vector<std::string>& preset_labels = sf.labels;
    std::cout << "preset compilers:";
    for (const auto& l : preset_labels) std::cout << ' ' << l;
    std::cout << "\n";
    const std::string started_utc = jac313::results::utc_now_iso();
    if (!global.summary_json.empty()) {   // a stale summary must never pass for this run
        std::error_code rm_ec;
        fs::remove(global.summary_json, rm_ec);
        fs::remove(summary_parts_path(global), rm_ec);
    }
    // A strict / hardened / contracts tree is its own build dir, so the everyday trees never
    // rebuild for it.
    const std::string& flavor = sf.flavor;

    std::vector<std::string> lines;
    const auto emit = [&](std::string s) { lines.push_back(std::move(s)); };
    emit("#!/usr/bin/env bash");
    emit("# Generated by jac313_test_cli — the exact commands for this run.");
    emit("# Re-run any time with:  bash run_latest_config.sh");
    emit("# Overwritten on the next preset invocation; copy it elsewhere to keep a config.");
    emit("set -euo pipefail");
    emit("cd \"" + root.string() + "\"");
    // PostgreSQL servers chosen for this run (no passwords: libpq reads ~/.pgpass / $PGPASSFILE)
    // (and each one's name from the connections file, which names the server in results.db)
    for (const char* var : {"JAC313_PG_REMOTE", "JAC313_PG_LOCAL", "JAC313_PG_REMOTE_NAME", "JAC313_PG_LOCAL_NAME"})
        if (const char* val = std::getenv(var); val && *val) emit(std::string("export ") + var + "=" + shell_quote(val));
    emit("CLI=\"" + self.string() + "\"");
    // Every gate records into results.db; render the report pages ONCE at the end, also when a
    // gate failed (set -e), keeping the run's exit status.
    emit("# report pages — rendered from results.db when the run ends (pass or fail)");
    emit("trap 'rc=$?; \"$CLI\" --report || echo \"report pages: not rendered\"; exit $rc' EXIT");
    for (const auto& n : sf.notes) emit(n);
    emit("");
    if (!sf.host.empty()) {
        emit("# pin this machine as asked (every gate below records under it)");
        emit("\"$CLI\" host" + sf.host);
        emit("");
    }

    if (preset.ctest || preset.smoke) {
        for (const std::string& label : preset_labels) {
            const std::string dir = "build-" + label + flavor;
            emit("# functional gate — " + dir + " (" + configure_opts.build_type + ", built once then run)");
            emit("\"$CLI\" configure --build-dir " + dir + " --compiler " + label + sf.configure);
            emit("\"$CLI\" build --build-dir " + dir + sf.build);
            if (preset.ctest) emit("\"$CLI\" run --build-dir " + dir + sf.ctest);         // ctest unit suite
            if (preset.smoke) emit("\"$CLI\" matrix run --build-dir " + dir + " --compiler " + label + sf.smoke);
        }
        emit("");
    }

    if (preset.verify_lite) {
        emit("# memory/thread gate — valgrind memcheck (ctest + smoke surface)");
        for (const std::string& label : preset_labels)
            emit("\"$CLI\" matrix verify-lite --compiler " + label + sf.verify);
        emit("");
    }
    if (preset.verify) {
        emit("# memory/thread gate — valgrind memcheck + helgrind + DRD");
        for (const std::string& label : preset_labels)
            emit("\"$CLI\" matrix verify --compiler " + label + sf.verify);
        emit("");
    }

    if (preset.bench) {
        emit("# throughput benchmark — Release, store_bench target");
        for (const std::string& label : preset_labels) {
            const std::string dir = "build-bench-" + label + flavor;
            emit("\"$CLI\" configure --release --build-dir " + dir + " --compiler " + label + sf.bench_configure);
            emit("cmake --build " + dir + " --target jac313_store_bench" + sf.build);
            emit("BENCH=\"" + dir + "/Store/tests/matrix/jac313_store_bench\"");
            // bench auto-records like every other gate (no --report needed); --report only renders.
            emit("\"$BENCH\" --suite --db \"" + JAC313_RESULTS_DB + "\"" + bench_keep_logs_args(global, label) + sf.bench);
        }
        emit("");
    }

    const fs::path script = root / "run_latest_config.sh";
    {
        std::ofstream out(script, std::ios::trunc);
        if (!out) {
            std::cerr << "Failed to write " << script.string() << '\n';
            return 1;
        }
        for (const auto& l : lines) {
            out << l << '\n';
        }
    }
    std::error_code ec;
    fs::permissions(script,
                    fs::perms::owner_all | fs::perms::group_read | fs::perms::group_exec
                        | fs::perms::others_read | fs::perms::others_exec,
                    ec);

    std::cout << "Wrote " << script.string() << "\n"
              << "Running it now — re-run any time with:  bash run_latest_config.sh\n\n";

    std::cout.flush();   // flush buffered cout before the child writes to the inherited fd
    std::cerr.flush();
    const std::string cmd = "bash \"" + script.string() + "\"";
    const int rc = std::system(cmd.c_str());
    if (rc == -1) {
        std::cerr << "Failed to launch bash for " << script.string() << '\n';
        return 1;
    }
    if (!global.summary_json.empty()) {
        std::string compilers;
        for (const auto& l : preset_labels) compilers += (compilers.empty() ? "" : ",") + l;
        assemble_summary(global, std::map<std::string, std::string>{
            {"version", harness_version(global)},
            {"compiler", compilers},
            {"pg_remote", [] {   // which remote server this run used (never a password)
                const char* r = std::getenv("JAC313_PG_REMOTE");
                const std::string s = (r && *r) ? r : "";
                return s.find("password") == std::string::npos ? s : std::string("(set; contains a password, not shown)");
            }()},
            {"build_type", configure_opts.build_type   // the tree's label: the Runner finds its build dir by it
                           + build_flavor_suffix(configure_opts.strict, configure_opts.hardened,
                                                 configure_opts.contracts)},
            {"contracts", configure_opts.contracts},
            {"started_utc", started_utc},
            {"finished_utc", jac313::results::utc_now_iso()},
            {"command", script.string()},
        });
    }
    return WIFEXITED(rc) ? WEXITSTATUS(rc) : 1;
}

// --machine-id precheck (read-only): list the machines already in results.db, then resolve and show
// which jac313-### THIS machine would record under (matched on cpu+cores+ram+disk+instance_hash, the
// hardware only; hostname never used) and whether that REUSES an existing machine or creates a new
// one. Writes nothing. It does NOT write run_latest_config.sh, so it won't clobber your last preset run.
int run_machine_id_command(const GlobalOptions& global) {
    const fs::path db_path = JAC313_RESULTS_DB;
    try {
        jac313::Qlite::v001::Sqlite db(db_path.string());
        jac313::results::ensure_schema(db);
        const auto hw = collect_host_hardware_record(machine_disk_tier("."));
        const jac313::results::MachineId h = make_machine_id(hw);
        std::cout << "=== machine-id precheck (read-only — test-summary/results.db) ===\n\n"
                     "Existing machines (from machine):\n";
        print_fleet_table(std::cout, db);
        const auto propose = propose_host_machine(db, hw);
        const bool stale_pin = jac313::results::current_host_is_stale(db, h, harness_version(global));
        std::cout << "\nThis machine:\n  cpu:   " << h.cpu << "\n  cores: " << h.cores
                  << "    ram_gb: " << h.ram_gb << "\n  disk:  " << h.disk
                  << "\n  os:    " << hw.os_pretty << "  (recorded per run, not part of the machine)\n"
                  << "  hash:  " << jac313::results::hash_prefix(h.instance_hash) << "\n"
                  << (stale_pin ? "\n  NOTE: current_host pin in results.db names another machine — "
                                  "run `host` or bootstrap to re-pin.\n" : "");
        if (propose.error == jac313::results::HostPinError::Ambiguous) {
            std::cout << "\nProposed machine: (ambiguous — pin required before recording)\n";
            print_ambiguity_help(std::cout, propose, h);
            return 1;
        }
        const std::string label = jac313::results::host_label(propose.mid);
        std::cout << "\nProposed machine: " << propose.mid << "   (" << label << ")\n"
                  << "   (matched on cpu+cores+ram+disk+instance_hash; pin is set only after this compare)\n"
                  << (jac313::results::find_exact_machine(db, h) != 0
                        ? "  -> REUSES an existing machine: recording ADDS rows under " + label + ".\n"
                        : "  -> NEW machine: recording creates a fresh one; nothing existing is touched.\n");
        (void)global;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "machine-id precheck failed: " << e.what() << "\n"; return 1;
    }
}

// ---- compiler pin (Setup/compilers.pin.jtext, settings.cpp): committed, per-machine {gcc,clang}
// toolchain labels so the choice is DETERMINISTIC — no "highest changes when a newer compiler lands"
// drift. Keyed on the machine: cpu+cores+ram+disk (Setup/compilers.pin.jtext rows; os_pretty is
// information only), labeled jac313-###. bootstrap does find-or-create; you change a row by editing it
// or `pin --gcc <label> --clang <label>`. No pin -> highest-available. ----
// This machine's row: cpu, cores and ram_gb equal, and the same disk tier (a row from before the disk
// column, disk '', matches on the other three; an exact disk match wins over it).
const CompilerPinRow* find_pin_for_host(const std::vector<CompilerPinRow>& rows, const HostHardwareRecord& hw) {
    const CompilerPinRow* legacy = nullptr;
    for (const auto& r : rows) {
        if (r.cpu != hw.cpu_model || r.cores != hw.cpu_cores || r.ram_gb != hw.ram_gb) continue;
        if (r.disk == hw.disk_type_label) return &r;
        if (r.disk.empty() && legacy == nullptr) legacy = &r;
    }
    return legacy;
}

// "sense the highest": the highest-version available toolchain label of a kind from the live probe.
std::string highest_toolchain_label(const std::vector<ResolvedToolchain>& probes, const std::string& kind) {
    const ResolvedToolchain* best = nullptr;
    for (const auto& p : probes) {
        if (!p.available || p.spec.kind != kind) continue;
        if (best == nullptr || p.detected_major > best->detected_major) best = &p;
    }
    return best ? best->spec.label : std::string{};
}

// Resolve this machine's pinned gcc/clang to a compiler path (highest-available if unpinned).
// Forward-declared up by the build-times gate; defined here, after the pin helpers it needs.
std::string resolve_pinned_cc(const fs::path& src, bool want_gcc) {
    CompilerResolveRequest request; request.source_dir = src;
    const HostHardwareRecord hw = collect_host_hardware_record(machine_disk_tier("."));
    const auto rows = load_compiler_pins(src);
    const CompilerPinRow* row = find_pin_for_host(rows, hw);
    std::string label = row ? (want_gcc ? row->gcc_label : row->clang_label)
                            : highest_toolchain_label(probe_compilers(src), want_gcc ? "gcc" : "clang");
    if (label.empty()) return {};
    request.explicit_compiler = label;
    try { return resolve_compiler(request).cc_path; } catch (...) { return {}; }
}

// {gcc_label, clang_label} for this machine: the committed pin if present, else highest-available.
std::pair<std::string, std::string> pinned_or_highest_labels(const fs::path& source_dir) {
    const HostHardwareRecord hw = collect_host_hardware_record(machine_disk_tier("."));
    const auto rows = load_compiler_pins(source_dir);
    if (const CompilerPinRow* row = find_pin_for_host(rows, hw))
        return {row->gcc_label, row->clang_label};
    const auto probes = probe_compilers(source_dir);
    return {highest_toolchain_label(probes, "gcc"), highest_toolchain_label(probes, "clang")};
}

// --run-everything: the full battery, orchestrated IN CODE (per-step exit codes, continue-on-error,
// a failure summary). Re-invokes this CLI's own subcommands per step. Uses THIS machine's PINNED
// gcc/clang (Setup/compilers.pin.jtext) so the run is deterministic. Run from v001/.
int run_everything_command(const GlobalOptions& global_in, const ConfigureOptions& configure_opts,
                           const BuildOptions& build_opts, const RunOptions& run_opts,
                           const MatrixOptions& matrix_opts) {
    // the steps record and print their own results; no Runner summary is assembled here
    GlobalOptions global = global_in;
    global.summary_json.clear();
    namespace fs = std::filesystem;
    std::error_code ec;
    if (!global.source_dir.empty()) {
        fs::current_path(global.source_dir, ec);
        if (ec) {
            std::cerr << "run-everything: cannot enter " << global.source_dir << ": " << ec.message() << '\n';
            return 1;
        }
    }
    // This very program, by absolute path: the steps run through the shell from the version
    // dir, which may be anywhere (and so may hold a space).
    const std::string CLI = "\"" + fs::read_symlink("/proc/self/exe", ec).string() + "\"";
    if (ec) { std::cerr << "run-everything: cannot find this program (/proc/self/exe)\n"; return 1; }
    const auto [gcc_label, clang_label] = pinned_or_highest_labels(".");
    if (!configure_opts.compiler && (gcc_label.empty() || clang_label.empty())) {
        std::cerr << "run-everything: could not resolve this machine's pinned/highest compilers "
                     "(run `jac313_test_cli pin --ensure`)\n";
        return 1;
    }
    // Every flag given is forwarded to the steps that take it (the rest are named, not dropped).
    global.production = resolve_production_spec(global, fs::current_path());
    const StepFlags sf = step_flags(global, configure_opts, build_opts, run_opts, matrix_opts, gcc_label, clang_label,
                                    true, true, true);
    std::cout << "######## run-everything: full battery —";
    for (const auto& l : sf.labels) std::cout << ' ' << l;
    std::cout << "  (pinned)"
              << "  keep-logs=" << (global.keep_logs ? "on" : "off (default)") << " ########\n";
    for (const auto& n : sf.notes) std::cout << n << "\n";
    print_keep_logs_status(global);
    // Clear ONLY this machine's prior results (the pinned machine), NOT the whole fleet DB — every
    // other machine's committed results survive. The machine row + the current_host pin are kept (we
    // re-record the SAME machine, reusing its number); the gates below repopulate it.
    const std::string& SHARED_DB = JAC313_RESULTS_DB;
    if (std::error_code dec; fs::exists(SHARED_DB, dec)) {
        try {
            jac313::results::Sqlite db(SHARED_DB);
            jac313::results::ensure_schema(db);
            const auto hw = collect_host_hardware_record(machine_disk_tier("."));
            const auto g_opt = pin_current_host(db, global);
            if (!g_opt) return 1;
            const std::int64_t g = *g_opt;
            if (g != 0) {
                // Version-scoped on the SHARED DB: clear only THIS version's prior runs for this
                // machine — never the OTHER version's data for the same machine.
                const std::string ver = harness_version(global);
                db.exec("DELETE FROM testRun WHERE run_id IN (SELECT run_id FROM run WHERE machine_id=? AND version=?)", g, ver);
                db.exec("DELETE FROM run_target WHERE run_id IN (SELECT run_id FROM run WHERE machine_id=? AND version=?)", g, ver);
                db.exec("DELETE FROM run WHERE machine_id=? AND version=?", g, ver);
                db.exec("DELETE FROM io_best_fit WHERE machine_id=?", g);
                jac313::results::rebuild_safeness(db);
                std::cout << "Cleared prior results for this machine (" << jac313::results::host_label(g)
                          << ", version " << ver << "); other machines/versions untouched.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "warning: could not clear this machine's prior results: " << e.what()
                      << " — continuing.\n";
        }
    }

    std::vector<std::string> failures;
    auto step = [&](const std::string& label, const std::string& cmd) {
        std::cout << "\n===== " << label << " =====\n" << std::flush;
        const int raw = std::system(cmd.c_str());
        const int rc = WIFEXITED(raw) ? WEXITSTATUS(raw) : 1;
        std::cout << "[exit " << rc << "] " << label << "\n";
        if (rc != 0) failures.push_back(label + " (exit " + std::to_string(rc) + ")");
    };

    for (const std::string& cc : sf.labels) {
        const std::string dir = "build-" + cc + sf.flavor;
        step("configure " + cc, CLI + " configure --build-dir " + dir + " --compiler " + cc + sf.configure);
        step("build " + cc,     CLI + " build --build-dir " + dir + sf.build);
        step("ctest " + cc,     CLI + " run --build-dir " + dir + sf.ctest);
        step("smoke " + cc,     CLI + " matrix run --build-dir " + dir + " --compiler " + cc + sf.smoke);
    }
    // Build the Release bench for each compiler.
    for (const std::string& cc : sf.labels) {
        const std::string dir = "build-bench-" + cc + sf.flavor;
        step("configure bench " + cc, CLI + " configure --release --build-dir " + dir + " --compiler " + cc + sf.bench_configure);
        step("build bench " + cc,     "cmake --build " + dir + " --target jac313_store_bench" + sf.build);
    }
    // Calibrate the per-machine durable persistence batch sizes (io_best_fit) AFTER the smoke gate (the host
    // is pinned by the ctest/smoke recorders by now) and BEFORE the recorded suite, so each --suite below
    // runs at the calibrated useThis. I/O batch is hardware-bound (not compiler-bound), so calibrate ONCE
    // with the first bench binary. --calibrate ensure_schema's the io_best_fit table itself (this machine's
    // rows were cleared above), and falls back gracefully (default batch) if it ever fails. A --batch given
    // replaces the calibrated batch, so the calibration is skipped then.
    if (global.bench_batch == 0)
        step("calibrate", "build-bench-" + sf.labels.front() + sf.flavor + "/Store/tests/matrix/jac313_store_bench "
                          "--calibrate --db \"" + SHARED_DB + "\"");
    // Recorded throughput suite per compiler (reads io_best_fit.useThis).
    for (const std::string& cc : sf.labels) {
        const std::string dir = "build-bench-" + cc + sf.flavor;
        step("bench " + cc, dir + "/Store/tests/matrix/jac313_store_bench --suite "
                            "--db \"" + SHARED_DB + "\"" + bench_keep_logs_args(global, cc) + sf.bench);
    }
    for (const std::string& cc : sf.labels)
        step("verify " + cc, CLI + " matrix verify --compiler " + cc + sf.verify);
    step("build-times", CLI + " build-times");
    step("report",       CLI + " --report");

    // PRESUMED-SAFE verdict block — read straight from the safeness summary (no recount).
    try {
        jac313::Qlite::v001::Sqlite db(SHARED_DB);
        const std::int64_t g = jac313::results::current_host(db, harness_version(global));
        if (g != 0) {
            // This machine under the OS it runs now (safeness is per machine and OS).
            const jac313::results::OsInfo os_now = jac313::results::sense_os();
            const std::int64_t os_id = jac313::results::os_id(db, os_now);
            std::string disk;
            { auto st = db.prepare("SELECT disk FROM machine WHERE id=?");
              st.bind(g); if (st.step()) st.get(disk); }
            const std::string os = jac313::results::os_display(os_now.name, os_now.version);
            struct PF { std::int64_t pass = 0, fail = 0, ran = 0; };
            auto pf = [&](const char* gate) { PF x;
                std::string ver;
                { auto vst = db.prepare("SELECT version FROM run WHERE machine_id=? AND version <> '' ORDER BY run_id DESC LIMIT 1");
                  vst.bind(g); if (vst.step()) vst.get(ver); }
                // COALESCE kept: SUM over no matching rows is NULL.
                std::string sql = "SELECT COALESCE(SUM(pass),0), COALESCE(SUM(fail),0), COUNT(*) FROM safeness "
                                  "WHERE machine_id=? AND os_id=? AND gate=?";
                if (!ver.empty()) sql += " AND (version=? OR version='')";
                auto st = db.prepare(sql);
                // Bind ALL params in ONE call: Sqlite::bind() does sqlite3_clear_bindings() first, so
                // a second bind() would wipe the earlier values and rebind ver into param 1 —
                // the version-scoping regression that zeroed this verdict. Branch instead of re-binding.
                if (ver.empty()) st.bind(g, os_id, std::string(gate));
                else             st.bind(g, os_id, std::string(gate), ver);
                if (st.step()) st.get(x.pass, x.fail, x.ran);
                return x; };
            const PF ct = pf("ctest"), sm = pf("smoke"), bn = pf("bench"),
                     me = pf("memcheck"), he = pf("helgrind"), dr = pf("drd");
            const std::int64_t vgfail = me.fail + he.fail + dr.fail;
            const bool ran   = ct.ran && sm.ran && bn.ran && me.ran && he.ran && dr.ran;
            const bool clean = (ct.fail + sm.fail + bn.fail + vgfail) == 0;
            std::cout << "\n══════════ " << jac313::results::host_label(g) << " · " << gcc_label << "+" << clang_label
                      << (os.empty() ? "" : " · " + os) << (disk.empty() ? "" : " · " + disk) << " ══════════\n"
                      << ((ran && clean) ? "  ✅ PRESUMED SAFE\n" : "  ❌ NOT SAFE\n")
                      << ((ran && clean) ? std::string() : "    why: " + not_safe_reason({ {"ctest", ct.ran != 0, ct.fail},
                             {"smoke", sm.ran != 0, sm.fail}, {"bench", bn.ran != 0, bn.fail}, {"memcheck", me.ran != 0, me.fail},
                             {"helgrind", he.ran != 0, he.fail}, {"drd", dr.ran != 0, dr.fail} }) + ".\n")
                      << "    ctest " << ct.pass << "/" << ct.fail << " · smoke " << sm.pass << "/" << sm.fail
                      << " · bench " << bn.pass << "/" << bn.fail << " · valgrind " << vgfail << " errors\n"
                      << "════════════════════════════════════════════════════════\n";
        }
    } catch (...) {}

    std::cout << "\n######## run-everything DONE — open test-summary/README.md ########\n";
    if (failures.empty()) { std::cout << "ALL GREEN.\n"; return 0; }
    std::cout << failures.size() << " step(s) had a non-zero exit (results recorded for what ran):\n";
    for (const auto& f : failures) std::cout << "  - " << f << "\n";
    return 1;
}

// ---- root guard: root must not write into a tree a regular user owns ----
// Root running in a user-owned checkout leaves root-owned build trees, logs and /tmp files the
// user's later runs cannot overwrite; and valgrind checks a binary's mode bits itself, so as root
// it gets the "other" bits and refuses a 770 binary ("Permission denied", rc=126). A tree owned
// by root (a container, CI) is fine and runs as usual. --allow-root (or JAC313_ALLOW_ROOT=1,
// which child runs inherit) overrides. Strips the flag from argv.
int refuse_root_unless_allowed(int& argc, char** argv) {
    bool allow = false;
    int out = 1;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--allow-root") { allow = true; continue; }
        argv[out++] = argv[i];
    }
    argc = out;
    argv[argc] = nullptr;
    if (const char* e = std::getenv("JAC313_ALLOW_ROOT"); e && std::string(e) == "1") allow = true;
    if (::geteuid() != 0) return 0;
    const fs::path tree = detect_source_dir(fs::current_path());
    struct stat st{};
    if (::stat(tree.c_str(), &st) != 0 || st.st_uid == 0) return 0;   // root's own tree: fine
    if (!allow) {
        const passwd* pw = ::getpwuid(st.st_uid);
        const std::string owner = pw ? pw->pw_name : "uid " + std::to_string(st.st_uid);
        std::cerr << "jac313_test_cli: refusing to run as root in " << tree.string() << ",\n"
                     "  which " << owner << " owns. Run it as " << owner << " (no sudo).\n"
                     "  A root run here leaves root-owned build trees, logs and /tmp files that\n"
                     "  " << owner << "'s later runs cannot overwrite, and valgrind will not start\n"
                     "  binaries that have no \"other\" execute bit. Override with --allow-root.\n";
        return 2;
    }
    ::setenv("JAC313_ALLOW_ROOT", "1", 1);
    std::cerr << "note: running as root in a tree owned by uid " << st.st_uid
              << " (--allow-root): files written now will be owned by root.\n";
    return 0;
}
} // namespace

// `compiler-versions`: the gcc and the clang a run uses on this machine (pinned in
// Setup/compilers.pin.jtext, else the newest installed), one line each: "gcc gcc16 16.2.1",
// "clang clang 22.1.8" ("-" when missing), then the contract modes it has on this OS
// (Setup/contracts.jtext, e.g. "enforce,observe,quick_enforce,ignore"). Jac313Runner shows the versions
// beside its choices and enables only those contract modes.
int run_compiler_versions_command(const GlobalOptions& global) {
    const auto [gcc_label, clang_label] = pinned_or_highest_labels(global.source_dir);
    const auto probes = probe_compilers(global.source_dir);
    const std::regex number(R"((\d+\.\d+(\.\d+)?))");
    const std::string os_family = jac313::Setup::v001::sense_host().os_family;
    const std::pair<std::string, std::string> kinds[] = {{"gcc", gcc_label}, {"clang", clang_label}};
    for (const auto& [kind, label] : kinds) {
        std::string version;
        for (const auto& p : probes) {
            std::smatch m;
            if (p.available && p.spec.label == label && std::regex_search(p.version_line, m, number))
                version = m[1];
        }
        std::string modes;
        for (const auto& m : contract_modes(global.source_dir, os_family, label))
            modes += (modes.empty() ? "" : ",") + m;
        std::cout << kind << ' ' << (label.empty() ? "-" : label) << ' '
                  << (version.empty() ? "-" : version) << ' ' << modes << '\n';
    }
    return 0;
}

static int run_main(int argc, char** argv) {
    // flush every line at once: through a pipe (Jac313Runner, tee) std::cout is block-buffered,
    // so its lines arrived late and out of order with the build tools' output
    std::cout << std::unitbuf;
    if (const int rc = refuse_root_unless_allowed(argc, argv); rc != 0) return rc;
    if (argc < 2) {
        print_usage();
        return 1;
    }

    // Drop inherited exported bash functions so activation probes via /bin/sh -c
    // don't emit "error importing function definition" noise.
    scrub_exported_shell_functions();

    GlobalOptions global;
    global.source_dir = detect_source_dir(fs::current_path());
    // The default connections file (a SQL_Secrets folder beside the project) fills in the
    // PostgreSQL servers the environment has not set; --connections / --pg-* flags win over it.
    {
        const fs::path cf = default_connections_path(fs::current_path());
        std::error_code cec;
        if (fs::exists(cf, cec)) {
            std::vector<PgConnection> rows;
            std::string cerr_text;
            if (load_connections(cf, rows, cerr_text)) apply_connections(rows, false, false, false);
            else std::cerr << "note: ignoring the connections file: " << cerr_text << '\n';
        }
        // and its password files now, for the commands that take their own arguments (host,
        // run-everything: they pin this machine through the fleet registry). The flag parser
        // applies them again after --connections / --pg-* replace a server.
        apply_password_files(cf.parent_path());
    }
    // Resolve the ONE shared results DB once (absolute, at the monorepo root); read as JAC313_RESULTS_DB
    // everywhere after this — in this process and, independently, in each spawned gate/tool.
    resolve_results_db(global.source_dir);

    std::string command = argv[1];
    int argi = 2;

    if (command == "--help" || command == "-h") {
        print_usage();
        return 0;
    }
    if (command == "connections") {   // show or create the PostgreSQL connections file
        return run_connections_command(argc, argv, 2);
    }
    if (command == "gui-build") {   // build a U++ GUI package under <root>/gui with umk (no shell script)
        // umk and its uppsrc come as a pair from one U++ install (jac314 gui_build rule):
        // $UPP_DIR, else $HOME/upp_basefix (upp_basefix.sh install: the pinned, patched U++),
        // else $HOME/u++/{umk,uppsrc}. No fixed drive paths: the repository can be anywhere.
        fs::path src = fs::current_path();
        std::vector<std::string> packages;
        std::string method = "GCC";   // a U++ build method (GCC uses the g++ on PATH; CLANG uses clang++)
        bool clean = false;            // rebuild every package (umk -a): U++ shares one object cache
        for (int i = 2; i < argc; ++i) {
            const std::string a = argv[i];
            if (a == "--source-dir" && i + 1 < argc) src = argv[++i];
            else if (a == "--method" && i + 1 < argc) method = argv[++i];
            else if (a == "--clean") clean = true;
            else if (a == "--help" || a == "-h") {
                std::cout << "gui-build [PACKAGE...] [--source-dir DIR] [--method GCC|CLANG] [--clean]\n"
                             "  Builds U++ packages in <root>/gui (default: Jac313Runner) into <root>/gui/out/<PACKAGE>.\n"
                             "  The GUI is version-independent: it sits beside the vNNN folders, not inside one.\n"
                             "  --method picks the U++ build method (default GCC). An upp_basefix install has\n"
                             "  GCC.bm and CLANG.bm next to its umk. U++ is taken from $UPP_DIR, else\n"
                             "  $HOME/upp_basefix, else $HOME/u++.\n"
                             "  --clean rebuilds everything (umk -a). Use it when the link fails with undefined\n"
                             "  references: U++ shares one object cache per method, and stale objects break links.\n";
                return 0;
            } else packages.push_back(a);
        }
        if (packages.empty()) packages.push_back("Jac313Runner");
        // <root>/gui, one level above the version dir (<root>/vNNN)
        fs::path vdir = fs::absolute(src).lexically_normal();
        if (vdir.filename().empty()) vdir = vdir.parent_path();   // "…/v002/" -> "…/v002"
        const fs::path gui = vdir.parent_path() / "gui";
        const char* home = std::getenv("HOME");
        std::vector<std::pair<fs::path, fs::path>> pairs;   // (umk, uppsrc) in preference order
        if (const char* d = std::getenv("UPP_DIR"); d && *d) pairs.emplace_back(fs::path(d) / "umk", fs::path(d) / "uppsrc");
        if (home) pairs.emplace_back(fs::path(home) / "upp_basefix/umk", fs::path(home) / "upp_basefix/uppsrc");
        if (home) pairs.emplace_back(fs::path(home) / "u++/umk", fs::path(home) / "u++/uppsrc");
        fs::path umk, uppsrc;
        for (const auto& [u, s] : pairs) {
            std::error_code ec;
            if (fs::is_regular_file(u, ec) && fs::is_directory(s, ec)) { umk = u; uppsrc = s; break; }
        }
        if (umk.empty()) {
            std::cerr << "gui-build: no umk + uppsrc pair found (looked in $UPP_DIR, $HOME/upp_basefix and $HOME/u++);\n"
                         "  ./bsir.sh installs U++ (upp_basefix)\n";
            return 1;
        }
        int rc = 0;
        for (const auto& pkg : packages) {
            const fs::path out = gui / "out" / pkg;
            std::error_code ec;
            fs::create_directories(out.parent_path(), ec);
            std::cout << "gui-build: " << pkg << "  (umk " << umk.string() << ", " << method << ")\n";
            const auto r = run_process({umk.string(), gui.string() + "," + uppsrc.string(), pkg, method, clean ? "-abrsH" : "-brsH",
                                        out.string()}, 1800);
            std::cout << r.stdout_text;
            std::cerr << r.stderr_text;
            const bool ok = r.exit_code == 0 && fs::exists(out, ec);
            std::cout << (ok ? "gui-build: OK  -> " : "gui-build: FAILED  ") << out.string() << "\n";
            if (!ok) rc = 1;
        }
        return rc;
    }

    if (command == "pin") {   // committed per-machine compiler pin (Setup/compilers.pin.jtext); find-or-create
        fs::path src = "."; bool ensure = false; std::string set_gcc, set_clang;
        for (int i = 2; i < argc; ++i) {
            const std::string a = argv[i];
            if (a == "--ensure") ensure = true;
            else if (a == "--gcc" && i + 1 < argc) set_gcc = argv[++i];
            else if (a == "--clang" && i + 1 < argc) set_clang = argv[++i];
            else if (a == "--source-dir" && i + 1 < argc) src = argv[++i];
        }
        const HostHardwareRecord hw = collect_host_hardware_record(machine_disk_tier("."));
        auto rows = load_compiler_pins(src);
        const CompilerPinRow* existing = find_pin_for_host(rows, hw);
        const std::string machine_text = hw.cpu_model + " · " + std::to_string(hw.cpu_cores) + " cores · "
                                       + std::to_string(hw.ram_gb) + " GB · " + (hw.disk_type_label.empty() ? "-" : hw.disk_type_label);
        if (!ensure && set_gcc.empty() && set_clang.empty()) {   // default: show this machine's pin
            if (existing) std::cout << "pinned " << existing->label << ": gcc=" << existing->gcc_label
                                    << " clang=" << existing->clang_label << "  (" << machine_text << ")\n";
            else std::cout << "no pin for this machine (" << machine_text
                           << ") — run: jac313_test_cli pin --ensure\n";
            return 0;
        }
        if (ensure && existing && set_gcc.empty() && set_clang.empty() && !existing->disk.empty()) {   // find-or-create: never overwrite
            std::cout << "pin exists " << existing->label << " (gcc=" << existing->gcc_label
                      << " clang=" << existing->clang_label << ") — unchanged  (" << machine_text << ")\n";
            return 0;
        }
        const auto probes = probe_compilers(src);
        const std::string gcc_label = !set_gcc.empty() ? set_gcc
            : (existing ? existing->gcc_label : highest_toolchain_label(probes, "gcc"));
        const std::string clang_label = !set_clang.empty() ? set_clang
            : (existing ? existing->clang_label : highest_toolchain_label(probes, "clang"));
        if (gcc_label.empty() && clang_label.empty()) {
            std::cerr << "pin: no gcc or clang toolchain available to pin (see Setup/compilers.jtext)\n"; return 1;
        }
        std::string row_label = existing ? existing->label : std::string{};
        if (row_label.empty()) {
            int maxn = 0;
            for (const auto& r : rows)
                if (r.label.rfind("jac313-", 0) == 0)
                    try { maxn = std::max(maxn, std::stoi(r.label.substr(7))); } catch (...) {}
            char buf[24]; std::snprintf(buf, sizeof buf, "jac313-%03d", maxn + 1); row_label = buf;
        }
        // The machine's row (a row from before the disk column gains this machine's disk tier).
        const bool updated = existing != nullptr;
        if (updated) {
            auto& r = rows[static_cast<std::size_t>(existing - rows.data())];
            r.gcc_label = gcc_label; r.clang_label = clang_label;
            r.disk = hw.disk_type_label; r.os = hw.os_pretty;
        } else {
            rows.push_back({row_label, hw.cpu_model, hw.os_pretty, gcc_label, clang_label, hw.cpu_cores, hw.ram_gb,
                            hw.disk_type_label});
        }
        write_compiler_pins(src, rows);
        std::cout << (updated ? "updated " : "pinned ") << row_label << ": gcc=" << gcc_label
                  << " clang=" << clang_label << "  (" << machine_text << ")\n";
        return 0;
    }

    if (command == "build-times") {   // the folded-in build_matrix.sh: gap-filled compile-time matrix
        fs::path src = ".";
        bool dry = false, force = false;
        for (int i = 2; i < argc; ++i) { const std::string a = argv[i];
            if (a == "--dry-run") dry = true;
            else if (a == "--force") force = true;   // rebuild everything (ignore fast-skip + gap-query)
            else if (a == "--source-dir" && i + 1 < argc) src = argv[++i]; }
        return run_build_times_gate(src, dry, force);
    }
    if (command == "host") {   // sense + pin THIS machine (machine + current_host) and show the grid
        GlobalOptions host_global = global;
        const HostPinFlags pin_flags = parse_host_pin_flags(argc, argv, 2);   // --claim / --assign-new[-###]
        host_global.host_claim = pin_flags.claim;
        host_global.host_assign_new = pin_flags.assign_new;
        const fs::path db_path = JAC313_RESULTS_DB;
        std::error_code ec;
        if (!fs::exists(db_path.parent_path(), ec)) { std::cerr << "host: no test-summary/.\n"; return 1; }
        jac313::Qlite::v001::Sqlite db(db_path.string());
        jac313::results::ensure_schema(db);
        const auto hw = collect_host_hardware_record(machine_disk_tier("."));
        const auto mid_opt = pin_current_host(db, host_global);
        if (!mid_opt) return 1;
        const jac313::results::MachineId h = make_machine_id(hw);
        const std::int64_t mid = *mid_opt;
        const jac313::results::OsInfo os_now = jac313::results::sense_os();
        const std::int64_t os_row = jac313::results::os_id(db, os_now);
        char speed[40];
        if (h.cpu_mhz_min == h.cpu_mhz_max) std::snprintf(speed, sizeof speed, "%.1f GHz", static_cast<double>(h.cpu_mhz_max) / 1000.0);
        else std::snprintf(speed, sizeof speed, "%.1f-%.1f GHz",
            static_cast<double>(h.cpu_mhz_min) / 1000.0, static_cast<double>(h.cpu_mhz_max) / 1000.0);
        std::cout << "=== host — " << jac313::results::host_label(mid) << " (pinned as current_host) ===\n"
                  << "  CPU      " << h.cpu << "\n"
                  << "  Speed    " << (h.cpu_mhz_max > 0 ? speed : "-") << "\n"
                  << "  P.Cores  " << h.p_cores << "      T.Cores  " << h.cores << "\n"
                  << "  RAM      " << h.ram_gb << " GB\n"
                  << "  Disk     " << dash(h.disk) << "\n"
                  << "  OS       " << jac313::results::os_display(os_now.name, os_now.version) << " · " << dash(os_now.kernel)
                  << " · " << dash(os_now.arch) << "  (os row " << os_row << ", recorded per run)\n"
                  << "  Hash     " << jac313::results::hash_prefix(h.instance_hash) << "  (SHA256 nodename)\n"
                  << "  * P.Cores = Physical Cores; T.Cores = Threading Cores\n";
        return 0;
    }
    if (command.rfind('-', 0) == 0) {
        command = "run";
        argi = 1;
    }

    ConfigureOptions configure_opts;
    BuildOptions build_opts;
    RunOptions run_opts;
    MatrixOptions matrix_opts;
    PresetOptions preset;
    std::optional<fs::path> report_path;

    auto parse_common = [&](int start) {
        for (int i = start; i < argc; ++i) {
            const std::string arg = argv[i];
            if (arg == "--build-dir" && i + 1 < argc) {
                global.build_dir = argv[++i];
            } else if (arg == "--source-dir" && i + 1 < argc) {
                global.source_dir = argv[++i];
            } else if (arg == "-v" || arg == "--verbose") {
                global.verbose = true;
            } else if (arg == "--filter" && i + 1 < argc) {
                run_opts.filter = argv[++i];
            } else if (arg == "--fail-fast") {
                run_opts.fail_fast = true;
            } else if (arg == "--failsafe" && i + 1 < argc) {
                run_opts.failsafe_sec = std::stoi(argv[++i]);
            } else if (arg == "--report") {
                // Disambiguate the legacy `run --report <path>` (TSV summary) from the bench
                // `--report` modifier (no argument): a following non-flag token is the path.
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    report_path = argv[++i];
                } else {
                    preset.report = true;
                }
            } else if (arg == "--ctest") {
                preset.ctest = true;
            } else if (arg == "--smoke") {
                preset.smoke = true;
            } else if (arg == "--bench") {
                preset.bench = true;
            } else if (arg == "--verify") {
                preset.verify = true;
            } else if (arg == "--verify-lite") {
                preset.verify_lite = true;
            } else if (arg == "--machine-id") {
                preset.machine_id = true;
            } else if (arg == "--claim" && i + 1 < argc) {
                global.host_claim = argv[++i];
            } else if (arg.rfind("--assign-new-", 0) == 0 || (arg == "--assign-new" && i + 1 < argc)) {
                const std::string num = arg == "--assign-new" ? std::string(argv[++i])
                                                              : arg.substr(std::string("--assign-new-").size());
                try { global.host_assign_new = std::stoll(num); }
                catch (const std::exception&) {
                    std::cerr << arg << ": a slot number is needed (e.g. --assign-new-003)\n";
                    std::exit(2);
                }
            } else if (arg == "--claim" && i + 1 < argc) {
                global.host_claim = argv[++i];
            } else if (arg.rfind("--assign-new-", 0) == 0) {
                global.host_assign_new = std::stoll(arg.substr(15));
            } else if (arg == "--assign-new" && i + 1 < argc) {
                global.host_assign_new = std::stoll(argv[++i]);
            } else if (arg == "--run-everything") {
                preset.run_everything = true;
            } else if (arg == "--keep-logs") {
                global.keep_logs = true;
            } else if (arg == "--keep-logs-dir" && i + 1 < argc) {
                global.keep_logs_dir = argv[++i];
            } else if (arg == "--wipe-logs") {
                global.wipe_logs = true;
            } else if (arg == "--params" && i + 1 < argc) {
                matrix_opts.params_file = argv[++i];
            } else if (arg == "--dry-run") {
                matrix_opts.dry_run = true;
            } else if (arg == "--persist" && i + 1 < argc) {
                global.persist_list = argv[++i];
            } else if (arg == "--max-persist-rows" && i + 1 < argc) {
                global.max_persist_rows = std::stoull(argv[++i]);
            } else if (arg == "--max-runs" && i + 1 < argc) {
                global.max_runs = std::stoull(argv[++i]);
            } else if (arg == "--max-records" && i + 1 < argc) {
                global.max_records = std::stoull(argv[++i]);
            } else if (arg == "--readback" && i + 1 < argc) {
                global.readback = argv[++i];
            } else if (arg == "--pg-cleanup" && i + 1 < argc) {
                global.pg_cleanup = argv[++i];
            } else if (arg == "--summary-json" && i + 1 < argc) {
                global.summary_json = fs::absolute(argv[++i]);
            } else if (arg == "--size" && i + 1 < argc) {
                global.size_override = argv[++i];
            } else if (arg == "--production" && i + 1 < argc) {
                global.production = argv[++i];
            } else if (arg == "--connections" && i + 1 < argc) {
                // The PostgreSQL servers from a jText connections file (keep it outside git).
                global.connections = argv[++i];
                std::vector<PgConnection> rows;
                std::string cerr_text;
                if (!load_connections(global.connections, rows, cerr_text)) {
                    std::cerr << "--connections: " << cerr_text << '\n';
                    std::exit(2);
                }
                if (const auto g = enclosing_git_project(global.connections); !g.empty())
                    std::cerr << "WARNING: " << global.connections.string() << " is inside the git project "
                              << g.string() << " - move it out, so it is never committed\n";
                apply_connections(rows, true, !global.pg_remote.empty(), !global.pg_local.empty());
            } else if ((arg == "--pg-remote" || arg == "--pg-local") && i + 1 < argc) {
                // The PostgreSQL server for pg-remote / pg-local, as a libpq conninfo. Passwords
                // never go on a command line (it lands in run_latest_config.sh, summaries and
                // AI reports): libpq reads them from ~/.pgpass or $PGPASSFILE.
                const std::string ci = argv[++i];
                if (ci.find("password") != std::string::npos) {
                    std::cerr << arg << ": leave the password out; put it in ../SQL_Secrets/jac313_pg_remote.pgpass (or _local)\n";
                    std::exit(2);   // parse_common returns nothing (like --help's exit)
                }
                (arg == "--pg-remote" ? global.pg_remote : global.pg_local) = ci;
                replace_pg_server(arg == "--pg-remote", ci);   // keeps the file's name for the same host
            } else if (arg == "--compiler" && i + 1 < argc) {
                configure_opts.compiler = argv[++i];
            } else if (arg == "--gcc") {
                configure_opts.prefer_gcc = true;
            } else if (arg == "--clang") {
                configure_opts.prefer_clang = true;
            } else if (arg == "--modules") {
                configure_opts.modules = true;
            } else if (arg == "--strict") {
                configure_opts.strict = true;
            } else if (arg == "--hardened") {
                configure_opts.hardened = true;
            } else if (arg == "--contracts" && i + 1 < argc) {
                configure_opts.contracts = argv[++i];
                if (!is_contracts_mode(configure_opts.contracts)) {
                    std::cerr << "--contracts " << configure_opts.contracts
                              << ": use enforce, observe, quick_enforce or ignore\n";
                    std::exit(2);
                }
            } else if (arg == "--release") {
                configure_opts.build_type = "Release";
            } else if (arg == "--debug") {
                configure_opts.build_type = "Debug";
            } else if (arg == "--no-store-tests") {
                configure_opts.store_tests = false;
            } else if (arg == "-j" && i + 1 < argc) {
                build_opts.jobs = std::stoi(argv[++i]);
            } else if (arg == "--threads" && i + 1 < argc) {
                global.bench_threads = std::stoull(argv[++i]);
            } else if (arg == "--events-per-thread" && i + 1 < argc) {
                global.bench_events_per_thread = std::stoull(argv[++i]);
            } else if (arg == "--runs" && i + 1 < argc) {
                global.bench_runs = std::stoull(argv[++i]);
            } else if (arg == "--batch" && i + 1 < argc) {
                global.bench_batch = std::stoull(argv[++i]);
            } else if (arg == "--help" || arg == "-h") {
                print_usage();
                std::exit(0);
            } else {
                std::cerr << "Unknown option: " << arg << '\n';
                std::exit(2);
            }
        }
        // the password files beside the connections file (after the flags: they may name the servers)
        apply_password_files((global.connections.empty() ? default_connections_path(fs::current_path())
                                                         : fs::absolute(global.connections)).parent_path());
    };

    if (command == "matrix") {
        if (argc < 3 || std::string(argv[2]) == "--help" || std::string(argv[2]) == "-h") {
            std::cout <<
                "matrix commands:\n"
                "  run            Run selected package scenarios\n"
                "  runner         Run ONE explicit config: --compiler --build-type --modules --size\n"
                "  verify-lite    valgrind gate: ctest + matrix 001 (binary; representative sinks)\n"
                "  verify         valgrind gate: ctest + smoke matrix (all sinks incl. html/json)\n\n"
                "Smoke matrix persist backends: binary, jtext, html, json, sql, none — each × output on/off.\n"
                "A matrix run is a correctness gate only. Throughput is the `--bench --report` gate,\n"
                "and `jac313_test_cli --report` renders every result from test-summary/results.db.\n\n"
                "matrix options:\n"
                "  --params <file>   Params file (default: tests/test_params.jtext)\n"
                "  --dry-run         List scenarios only\n"
                "  --modules         Configure+build with modules before run\n"
                "  --filter <regex>  Match test, persist (binary|jtext|html|json|sql|none),\n"
                "                    or name|persist|mode\n"
                "  --failsafe <sec>  Kill stuck scenario after N seconds (smoke: "
                + std::to_string(kSmokeFailsafeSec) + ", full: " + std::to_string(kFullFailsafeSec)
                + ", 0=off)\n"
                "  --clang           Force the clang toolchain (registry-resolved) for results path / build dir\n"
                "  --compiler <cxx>  Explicit compiler (skips auto-detect; any Setup/compilers.jtext label)\n";
            return argc < 3 ? 1 : 0;
        }
        const std::string sub = argv[2];

        if (sub == "run") {
            parse_common(3);
            return execute_matrix_run(global, configure_opts, matrix_opts, run_opts, build_opts);
        }

        if (sub == "runner") {
            // Explicit-options primitive: every dimension named, no ad-hoc flags.
            // This is the unit `run-all` composes. Accepts any Setup/compilers.jtext label.
            std::string compiler, build_type, size, modules = "on";
            std::optional<std::string> build_dir_override;
            for (int i = 3; i < argc; ++i) {
                const std::string a = argv[i];
                if (a == "--compiler" && i + 1 < argc) {
                    compiler = argv[++i];
                } else if (a == "--build-type" && i + 1 < argc) {
                    build_type = argv[++i];
                } else if (a == "--modules" && i + 1 < argc) {
                    modules = argv[++i];
                } else if (a == "--size" && i + 1 < argc) {
                    size = argv[++i];
                } else if (a == "--build-dir" && i + 1 < argc) {
                    build_dir_override = argv[++i];
                } else if (a == "--source-dir" && i + 1 < argc) {
                    global.source_dir = argv[++i];
                } else if (a == "-v" || a == "--verbose") {
                    global.verbose = true;
                } else if (a == "--help" || a == "-h") {
                    std::cout <<
                        "matrix runner — run ONE configuration (the primitive run-all composes)\n\n"
                        "  --compiler gcc15|gcc16|clang  (required; any Setup/compilers.jtext label)\n"
                        "  --build-type Debug|Release (required)\n"
                        "  --modules on|off           (default on)\n"
                        "  --size smoke|full          (required)\n"
                        "  --build-dir <path>         (default: build-<compiler>-<buildtype>-<modules>)\n"
                        "  --source-dir <path>\n";
                    return 0;
                } else {
                    std::cerr << "Unknown matrix runner option: " << a << '\n';
                    return 2;
                }
            }
            if (compiler == "clang") {
                configure_opts.prefer_clang = true;
                matrix_opts.compiler_label = "clang";
            } else if (!compiler.empty()) {
                // Any gcc label (gcc15, gcc16, future toolchains): resolved by label
                // via Setup/compilers.jtext; the run is recorded under that exact label.
                configure_opts.compiler = compiler;
                matrix_opts.compiler_label = compiler;
            } else {
                std::cerr << "matrix runner: --compiler is required (gcc15 | gcc16 | clang | registry label)\n";
                return 2;
            }
            if (build_type == "Debug" || build_type == "Release") {
                configure_opts.build_type = build_type;
            } else {
                std::cerr << "matrix runner: --build-type must be Debug|Release\n";
                return 2;
            }
            if (modules == "on") {
                configure_opts.modules = true;
            } else if (modules == "off") {
                configure_opts.modules = false;
            } else {
                std::cerr << "matrix runner: --modules must be on|off\n";
                return 2;
            }
            if (size == "smoke") {
                matrix_opts.params_file = "tests/test_params.jtext";
            } else if (size == "full") {
                matrix_opts.params_file = "tests/test_params_full.jtext";
            } else {
                std::cerr << "matrix runner: --size must be smoke|full\n";
                return 2;
            }
            if (build_dir_override) {
                global.build_dir = *build_dir_override;
            } else {
                const std::string bt = (build_type == "Release") ? "release" : "debug";
                const std::string md = configure_opts.modules ? "modules" : "textual";
                global.build_dir = "build-" + compiler + "-" + bt + "-" + md;
            }
            return execute_matrix_run(global, configure_opts, matrix_opts, run_opts, build_opts);
        }

        if (sub == "verify" || sub == "verify-lite") {
            const bool full = (sub == "verify");
            ConfigureOptions co;
            for (int i = 3; i < argc; ++i) {
                const std::string a = argv[i];
                if (a == "--gcc") {
                    co.prefer_gcc = true;
                } else if (a == "--clang") {
                    co.prefer_clang = true;
                } else if (a == "--compiler" && i + 1 < argc) {
                    co.compiler = argv[++i];
                } else if (a == "--strict") {
                    co.strict = true;
                } else if (a == "--hardened") {
                    co.hardened = true;
                } else if (a == "--contracts" && i + 1 < argc) {
                    co.contracts = argv[++i];
                    if (!is_contracts_mode(co.contracts)) {
                        std::cerr << "--contracts " << co.contracts << ": use enforce, observe, quick_enforce or ignore\n";
                        return 2;
                    }
                } else if (a == "-j" && i + 1 < argc) {
                    build_opts.jobs = std::stoi(argv[++i]);
                } else if (a == "--source-dir" && i + 1 < argc) {
                    global.source_dir = argv[++i];
                } else if (a == "-v" || a == "--verbose") {
                    global.verbose = true;
                } else if (a == "--help" || a == "-h") {
                    std::cout << "matrix " << sub << " — valgrind gate ("
                              << (full ? "ctest + smoke matrix — all sinks (binary/jtext/html/json/sql/none)"
                                      : "ctest + matrix 001 — representative sinks (binary; 001 across jtext/html/json/sql)")
                              << "; memcheck + helgrind/DRD)\n\n"
                                 "  --gcc | --clang | --compiler <cxx>   (default: auto-detect)\n"
                                 "  --strict | --hardened | --contracts M (a tree of its own: build-<label>[-flavor]-" << sub << ")\n"
                                 "  -j N                                 parallel build jobs\n"
                                 "  --source-dir <path>\n";
                    return 0;
                } else {
                    std::cerr << "Unknown matrix " << sub << " option: " << a << '\n';
                    return 2;
                }
            }
            return run_verify_command(global, co, build_opts, full, sub);
        }

        std::cerr << "Unknown matrix subcommand: " << sub << '\n';
        return 1;
    }

    if (command == "vendor-check") return jac313::test_cli::vendor::run_vendor_check(argc, argv, 2);
    if (command == "vendor-sync") return jac313::test_cli::vendor::run_vendor_sync(argc, argv, 2);
    if (command == "vendor-baseline") return jac313::test_cli::vendor::run_vendor_baseline(argc, argv, 2);
    if (command == "note") {   // note RUN_ID "TEXT" adds a note to a run; note RUN_ID lists its notes
        // A note is a runEvent row with event 'note': it explains a run (say, a failure caused by
        // the environment, not the code) without changing or deleting any recorded result.
        if (argc < 3 || std::string(argv[2]) == "--help" || std::string(argv[2]) == "-h") {
            std::cout << "note RUN_ID [\"TEXT\"]   add TEXT as a note on RUN_ID, or list its notes\n";
            return argc < 3 ? 2 : 0;
        }
        std::int64_t run_id = 0;
        try { run_id = std::stoll(argv[2]); } catch (...) {}
        try {
            jac313::Qlite::v001::Sqlite db(JAC313_RESULTS_DB);
            std::int64_t found = 0;
            { auto st = db.prepare("SELECT COUNT(*) FROM run WHERE run_id=?"); st.bind(run_id);
              if (st.step()) st.get(found); }
            if (run_id <= 0 || found == 0) {
                std::cerr << "note: no run " << argv[2] << " in " << JAC313_RESULTS_DB << '\n';
                return 1;
            }
            if (argc >= 4) {
                jac313::results::log_run_event(db, run_id, "", "note", argv[3]);
                std::cout << "note added to run " << run_id << '\n';
            }
            auto st = db.prepare("SELECT ts_utc, message FROM runEvent WHERE run_id=? AND event='note' ORDER BY id");
            st.bind(run_id);
            while (st.step()) {
                std::string ts, msg;
                st.get(ts, msg);
                std::cout << "  run " << run_id << "  " << ts << "  " << msg << '\n';
            }
        } catch (const std::exception& e) {
            std::cerr << "note: " << e.what() << '\n';
            return 1;
        }
        return 0;
    }
    if (command == "version-check") {
        for (int i = argi; i < argc; ++i) {
            const std::string a = argv[i];
            if (a == "--source-dir" && i + 1 < argc) {
                global.source_dir = argv[++i];
            }
        }
        return run_version_check_command(global);
    }

    parse_common(argi);

    if (command == "list") {
        return run_list_command(global);
    }
    if (command == "run") {
        if (preset.machine_id) {
            return run_machine_id_command(global);
        }
        if (global.wipe_logs) {
            if (run_wipe_logs_command(global) != 0) return 1;
        }
        if (preset.run_everything) {
            return run_everything_command(global, configure_opts, build_opts, run_opts, matrix_opts);
        }
        const bool any_preset = preset.ctest || preset.smoke || preset.bench
                             || preset.report || preset.verify_lite || preset.verify;
        if (any_preset) {
            // --report alone renders only; with gates it is implied (the script renders at its end)
            if (preset.report && !(preset.ctest || preset.smoke || preset.bench
                                   || preset.verify_lite || preset.verify)) {
                return run_report_command(global.source_dir);   // generate the report pages from results.db
            }
            return run_preset_command(global, configure_opts, build_opts, run_opts, matrix_opts, preset);
        }
        if (global.wipe_logs) {
            return 0;
        }
        return run_tests_command(global, run_opts, report_path);
    }
    if (command == "compilers") {
        return run_compilers_command(global, configure_opts);
    }
    if (command == "compiler-versions") {
        return run_compiler_versions_command(global);
    }
    if (command == "setup") {
        return run_setup_command(global);
    }
    if (command == "configure") {
        if (!apply_compiler_resolution(global, configure_opts, matrix_opts)) {
            return 1;
        }
        return run_configure_command(global, configure_opts);
    }
    if (command == "build") {
        return build_and_capture(global, build_opts);
    }
    print_usage();
    return 1;
}

// A settings table that does not read (Setup/*.jtext, tests/*.jtext) ends the command with its
// reason, not with an uncaught exception.
int main(int argc, char** argv) {
    int rc = 1;
    try {
        rc = run_main(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "jac313_test_cli: " << e.what() << '\n';
        rc = 1;
    }
    for (const auto& why : g_not_recorded) std::cout << "NOT RECORDED: " << why << '\n';
    return (rc == 0 && !g_not_recorded.empty()) ? 1 : rc;
}
