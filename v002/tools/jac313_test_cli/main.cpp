// jac313_test_cli — local test runner for the jac313 monorepo.
// Discovers CTest entries from a build tree, runs them with captured output,
// and prints a summary. No shell scripts required.

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
#include "host_hardware.hpp"
#include "runner.hpp"

#include <jac313/Qlite/v002.hpp>   // shared testList catalog in the bench results DB
#include "jac313_results_db.hpp"   // jac313::results — shared schema + dimension helpers

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <sys/wait.h>

namespace fs = std::filesystem;
using namespace jac313::test_cli;

namespace {

void print_usage() {
    std::cout <<
        "jac313_test_cli — local jac313 test runner\n\n"
        "Usage:\n"
        "  jac313_test_cli <command> [options]\n\n"
        "Commands:\n"
        "  list        List discovered tests from the build tree\n"
        "  run         Run tests (default if command omitted)\n"
        "  configure   CMake configure (Ninja; auto-detect compiler)\n"
        "  build       Build the configured tree\n"
        "  compilers   Sense toolchains from the registry (writes FileCheckList.txt)\n"
        "  setup       Sense readiness; generate Setup.sh for missing prerequisites\n"
        "  version-check  check each package owes a version() bump (git, no build)\n"
        "  matrix      run | runner | verify | verify-lite\n\n"
        "Common options:\n"
        "  --build-dir <path>   Build directory (default: build)\n"
        "  --source-dir <path>  Source root (default: auto-detect)\n"
        "  -v, --verbose        Verbose output\n"
        "  -h, --help           Show help\n\n"
        "Preset gates (no subcommand — compose freely; writes & runs ./run_latest_config.sh):\n"
        "  --ctest              ctest unit suite\n"
        "  --smoke              persist x output smoke matrix\n"
        "  --bench              throughput benchmark (numbers to stdout)\n"
        "  --report             with --bench: also record the DB + render the report\n"
        "  --verify-lite        valgrind memcheck over the ctest + smoke surface\n"
        "  --verify             valgrind memcheck + helgrind + DRD\n"
        "  --group-id           precheck: list bench groups + this machine's proposed group_id\n"
        "  (everyday: --ctest --smoke ; recorded bench: --bench --report)\n\n"
        "Run options:\n"
        "  --filter <regex>     Run only matching tests (matrix: scenario filter)\n"
        "  --fail-fast          Stop on first failure\n"
        "  --failsafe <sec>     Kill stuck test/compile after N seconds (smoke: "
        + std::to_string(kSmokeFailsafeSec) + ", full: " + std::to_string(kFullFailsafeSec)
        + ", 0=off)\n"
        "  --report <path>      Write summary file (tab-separated)\n\n"
        "Configure options:\n"
        "  --compiler <cxx>     Explicit compiler: registry label, name, or path\n"
        "  --gcc15              Select the gcc15 toolchain (default)\n"
        "  --clang              Select the clang toolchain (min Clang 20)\n"
        "  --modules            Enable JAC313_BUILD_MODULES=ON\n"
        "  --release            CMAKE_BUILD_TYPE=Release (default: Debug)\n"
        "  --no-store-tests     Disable JAC313_BUILD_STORE_TESTS\n\n"
        "Toolchains are data-driven: see tools/jac313_test_cli/compilers.conf\n"
        "(activation-aware — handles gcc-toolset via scl / env-launcher / source-script).\n"
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
        if (fs::exists(dir / "CMakeLists.txt") && fs::exists(dir / "Store") && fs::exists(dir / "Qlite")) {
            return dir;
        }
        if (dir == dir.parent_path()) {
            break;
        }
    }
    return start;
}

int run_list_command(const GlobalOptions& global) {
    const auto tests = discover_tests(global.build_dir);
    if (tests.empty()) {
        std::cerr << "No tests found under " << global.build_dir
                  << " (run configure + build first?)\n";
        return 1;
    }

    std::cout << "Discovered " << tests.size() << " test(s):\n";
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
// the catalog is a convenience, never block a run on it. CREATE IF NOT EXISTS + INSERT OR IGNORE,
// so it merges with store_bench's bench-config entries and a re-run writes nothing (no DB churn).
void seed_test_catalog(const fs::path& source_dir, const std::vector<TestEntry>& tests) {
    const fs::path db_path = source_dir / "test-summary" / "results.db";
    std::error_code ec;
    if (!fs::exists(db_path.parent_path(), ec)) {
        return;   // no test-summary/ -> nothing to catalog into
    }
    try {
        jac313::Qlite::v002::Sqlite db(db_path.string());
        db.exec("CREATE TABLE IF NOT EXISTS testList (id INTEGER PRIMARY KEY, name TEXT UNIQUE NOT NULL)");
        for (const auto& t : tests) {
            db.exec("INSERT OR IGNORE INTO testList(name) VALUES(?)", t.name);
        }
    } catch (const std::exception&) {
        // best-effort: a catalog write failure never fails the test run
    }
}

// Defined further down (next to the other results.db recorders); declared here for run_tests_command.
void record_ctest_results(const fs::path& source_dir, const fs::path& build_dir,
                          const std::vector<TestResult>& results);

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

    const auto results = run_tests(tests, opts, global.verbose);
    if (results.empty()) {
        return 1;
    }
    const auto summary = summarize(results);
    print_summary(summary, results);

    record_ctest_results(global.source_dir, global.build_dir, results);   // capture into results.db

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
        request.prefer_gcc15 = configure_opts.prefer_gcc15;
        request.prefer_clang = configure_opts.prefer_clang;

        const CompilerSelection selection = resolve_compiler(request);
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

int run_compilers_command(const GlobalOptions& global, const ConfigureOptions& configure_opts) {
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
    namespace setup = jac313::Setup::v002;

    const auto resolved = probe_compilers(global.source_dir);
    print_compiler_probe(resolved);
    write_filecheck_list(global.source_dir, resolved);

    const setup::HostSnapshot host = setup::sense_host();
    const setup::ReadinessReport rd = setup::assess_readiness(resolved, host.os_family, global.source_dir);
    const setup::ProvisionPlan plan = setup::plan_provision(global.source_dir, resolved, host.os_family);

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

    if (setup::write_setup_script(global.source_dir, plan)) {
        std::cout << "\nWrote Setup.sh (" << plan.steps.size() << " step(s)). "
                  << "It modifies your system and may need sudo — review it, then run:\n";
        std::cout << "  bash Setup.sh\n";
    }
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
// NOTE: this schema MUST match store_bench's ensure_results_schema (two writers, one DB).
// TODO: factor the shared schema/resolution into a common header to remove the duplication.
// Schema + lookup live in jac313_results_db.hpp (shared with store_bench, single source of truth).
void cli_ensure_results_schema(jac313::Qlite::v002::Sqlite& db) { jac313::results::ensure_schema(db); }

std::int64_t cli_results_id(jac313::Qlite::v002::Sqlite& db, const char* sql, const std::string& name) {
    return jac313::results::lookup_id(db, sql, name);
}

// insert-or-find a functional parameter combo (batch/flag_count N/A => NULL); SELECT-then-INSERT.
std::int64_t cli_parameter_id(jac313::Qlite::v002::Sqlite& db,
        std::int64_t compiler_id, const std::string& build_type, const std::string& modules,
        const std::string& import_std, const std::string& size, const std::string& persist,
        const std::string& output_mode, std::int64_t threads, std::int64_t events, std::int64_t runs) {
    const char* where =
        "SELECT id FROM parameter WHERE compiler_id=? AND build_type=? AND modules=? AND import_std=? AND size=? "
        "AND persist=? AND output_mode=? AND threads=? AND events_per_thread=? AND runs=? "
        "AND batch IS NULL AND flag_count IS NULL AND valgrind_tool IS NULL";
    auto find = [&]() -> std::int64_t {
        std::int64_t id = 0; auto st = db.prepare(where);
        st.bind(compiler_id, build_type, modules, import_std, size, persist, output_mode, threads, events, runs);
        if (st.step()) st.get(id); return id;
    };
    std::int64_t id = find();
    if (id == 0) {
        db.exec("INSERT INTO parameter(compiler_id,build_type,modules,import_std,size,persist,output_mode,threads,"
                "events_per_thread,runs,batch,flag_count) VALUES(?,?,?,?,?,?,?,?,?,?,NULL,NULL)",
                compiler_id, build_type, modules, import_std, size, persist, output_mode, threads, events, runs);
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

// Ensure schema, resolve this machine's group_id (hardware+os, matching store_bench), create a fresh
// run row, and return its run_id. Shared by the smoke/ctest/verify recorders. Uses the shared helpers.
std::int64_t cli_begin_run(jac313::Qlite::v002::Sqlite& db) {
    jac313::results::ensure_schema(db);
    const auto hw = collect_host_hardware_record("");
    const jac313::results::HostId h{hw.cpu_model, static_cast<std::int64_t>(hw.cpu_cores),
                                    static_cast<std::int64_t>(hw.ram_gb), hw.os_pretty};
    const std::int64_t group_id = jac313::results::group_id(db, h);
    const std::int64_t run_id   = jac313::results::next_run_id(db);
    jac313::results::insert_run(db, run_id, group_id, jac313::results::host_label(group_id), h);
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
    const auto r = run_process({cxx, "--version"});
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

std::int64_t cli_compiler_id(jac313::Qlite::v002::Sqlite& db, const jac313::results::CompilerInfo& c) {
    return jac313::results::compiler_id(db, c);
}

// "on"/"off" — whether the tree was configured with `import std` (JAC313_*_IMPORT_STD in the cache).
std::string read_import_std(const fs::path& build_dir) {
    std::ifstream in(build_dir / "CMakeCache.txt");
    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("JAC313_QLITE_IMPORT_STD:", 0) == 0 || line.rfind("JAC313_STORE_IMPORT_STD:", 0) == 0) {
            return (line.find("=ON") != std::string::npos) ? "on" : "off";
        }
    }
    return "off";
}

// ============================================================================
// results.db report: a compiler inventory page + per-test-type summary + per-run detail pages,
// under test-summary/{compiler,ctest,smoke,verify-lite,verify}/. Generated by `jac313_test_cli --report`.
// ============================================================================
std::string comp_label(const std::string& name, std::int64_t major) { return name + std::to_string(major); }
std::string frontend_label(const std::string& modules, const std::string& import_std) {
    if (modules != "on") return "headers";
    return (import_std == "on") ? "import-std" : "modules";
}
std::string run_label_md(std::int64_t run_id) {
    char b[24]; std::snprintf(b, sizeof b, "Run_%03lld", static_cast<long long>(run_id)); return b;
}
std::string dash(const std::string& s) { return s.empty() ? "-" : s; }

void write_compiler_page(jac313::Qlite::v002::Sqlite& db, const fs::path& out) {
    std::error_code ec; fs::create_directories(out / "compiler", ec);
    std::ofstream md(out / "compiler" / "README.md");
    md << "# Compilers\n\n_Generated from `results.db`._\n\n"
          "| label | name | version | major | runs |\n|---|---|---|--:|--:|\n";
    auto st = db.prepare(
        "SELECT c.name, c.version, c.major, COUNT(DISTINCT tr.run_id) "
        "FROM compiler c LEFT JOIN parameter p ON p.compiler_id=c.id "
        "LEFT JOIN testRun tr ON tr.parameter_id=p.id GROUP BY c.id ORDER BY c.name, c.major, c.version");
    while (st.step()) {
        std::string name, version; std::int64_t major = 0, runs = 0;
        st.get(name, version, major, runs);
        md << "| " << comp_label(name, major) << " | " << name << " | " << version << " | "
           << major << " | " << runs << " |\n";
    }
}

// Compilers present for a type, each with its LATEST run_id; ordered by name so clang sorts left, gcc right.
std::vector<std::pair<std::string, std::int64_t>> compilers_latest(jac313::Qlite::v002::Sqlite& db, const std::string& type) {
    std::vector<std::pair<std::string, std::int64_t>> v;
    auto st = db.prepare(
        "SELECT c.name, c.major, MAX(tr.run_id) FROM testRun tr JOIN parameter p ON p.id=tr.parameter_id "
        "JOIN compiler c ON c.id=p.compiler_id JOIN testType tt ON tt.id=tr.test_type_id "
        "WHERE tt.name=? GROUP BY c.id ORDER BY c.name, c.major");
    st.bind(type);
    while (st.step()) { std::string name; std::int64_t major = 0, run = 0; st.get(name, major, run);
        v.push_back({comp_label(name, major), run}); }
    return v;
}

// Per-run detail page (one compiler's run): every scenario with persist/mode/tool + status + ms.
void write_run_detail(jac313::Qlite::v002::Sqlite& db, const fs::path& dir, const std::string& type, std::int64_t run_id) {
    const std::string rl = run_label_md(run_id);
    std::ofstream det(dir / (rl + ".md"));
    det << "# " << type << " — " << rl << "\n\n[← back](README.md)\n\n";
    { auto st = db.prepare(
        "SELECT r.host, c.name, c.version, p.build_type, p.modules, p.import_std, IFNULL(p.size,''), r.ts_utc "
        "FROM testRun tr JOIN run r ON r.run_id=tr.run_id JOIN parameter p ON p.id=tr.parameter_id "
        "JOIN compiler c ON c.id=p.compiler_id WHERE tr.run_id=? LIMIT 1");
      st.bind(run_id);
      if (st.step()) { std::string host, name, version, bt, mod, imp, size, ts;
          st.get(host, name, version, bt, mod, imp, size, ts);
          det << "_" << host << " · " << name << " " << version << " · " << bt << " · "
              << frontend_label(mod, imp) << (size.empty() ? "" : (" · " + size)) << " · " << ts << "_\n\n"; } }
    det << "| test | persist | mode | tool | status | ms |\n|---|---|---|---|---|--:|\n";
    auto st = db.prepare(
        "SELECT tl.name, IFNULL(p.persist,''), IFNULL(p.output_mode,''), IFNULL(p.valgrind_tool,''), "
        "tr.status, IFNULL(tr.duration_ms,0) FROM testRun tr JOIN testList tl ON tl.id=tr.test_list_id "
        "JOIN parameter p ON p.id=tr.parameter_id JOIN testType tt ON tt.id=tr.test_type_id "
        "WHERE tr.run_id=? AND tt.name=? ORDER BY tl.name");
    st.bind(run_id, type);
    while (st.step()) {
        std::string t, per, mode, tool, status; std::int64_t ms = 0;
        st.get(t, per, mode, tool, status, ms);
        det << "| " << t << " | " << dash(per) << " | " << dash(mode) << " | " << dash(tool) << " | "
            << status << " | " << ms << " |\n";
    }
}

// Per-type page = a COMPILER COMPARISON pivot: scenario rows x compiler columns (clang left -> gcc right),
// cell = ms (pass) or the status. Each column header links to that compiler's per-run detail page.
void write_type_pages(jac313::Qlite::v002::Sqlite& db, const fs::path& out, const std::string& type) {
    const auto comps = compilers_latest(db, type);
    if (comps.empty()) return;
    std::error_code ec; fs::create_directories(out / type, ec);

    std::string inlist;
    for (const auto& c : comps) { if (!inlist.empty()) inlist += ","; inlist += std::to_string(c.second); }
    auto col_for = [&](std::int64_t run) -> int {
        for (std::size_t i = 0; i < comps.size(); ++i) if (comps[i].second == run) return static_cast<int>(i);
        return -1; };

    struct Row { std::string key; std::vector<std::string> cells; };
    std::vector<Row> rows;
    {
        auto st = db.prepare(
            "SELECT tr.run_id, tl.name, IFNULL(p.persist,''), IFNULL(p.output_mode,''), IFNULL(p.valgrind_tool,''), "
            "tr.status, IFNULL(tr.duration_ms,0) FROM testRun tr JOIN testList tl ON tl.id=tr.test_list_id "
            "JOIN parameter p ON p.id=tr.parameter_id JOIN testType tt ON tt.id=tr.test_type_id "
            "WHERE tt.name=? AND tr.run_id IN (" + inlist + ") ORDER BY tl.name, p.persist, p.output_mode, p.valgrind_tool");
        st.bind(type);
        while (st.step()) {
            std::int64_t run = 0, ms = 0; std::string t, per, mode, tool, status;
            st.get(run, t, per, mode, tool, status, ms);
            std::string key = t;
            if (!per.empty())  key += " · " + per;
            if (!mode.empty()) key += " · " + mode;
            if (!tool.empty()) key += " · " + tool;
            Row* r = nullptr;
            for (auto& rr : rows) if (rr.key == key) { r = &rr; break; }
            if (!r) { rows.push_back({key, std::vector<std::string>(comps.size(), "-")}); r = &rows.back(); }
            const int col = col_for(run);
            if (col >= 0) r->cells[static_cast<std::size_t>(col)] =
                (status == "pass") ? (ms ? std::to_string(ms) : std::string("pass")) : status;
        }
    }

    {
        std::ofstream md(out / type / "README.md");
        md << "# " << type << " — compiler comparison\n\n_Generated from `results.db`. Columns are compilers "
              "(latest run each); cell = ms (pass) or status._\n\n| scenario";
        for (const auto& c : comps) md << " | [" << c.first << "](" << run_label_md(c.second) << ".md)";
        md << " |\n|---";
        for (std::size_t i = 0; i < comps.size(); ++i) md << "|--:";
        md << "|\n";
        for (const auto& r : rows) {
            md << "| " << r.key;
            for (const auto& cell : r.cells) md << " | " << cell;
            md << " |\n";
        }
    }

    for (const auto& c : comps) write_run_detail(db, out / type, type, c.second);
}

// Bench pages: bench rows carry ops stats (median/band/bytes), not status/duration — its own format.
// Bench per-run detail (ops format): each config with persist/events + median/band/bytes.
void write_bench_detail(jac313::Qlite::v002::Sqlite& db, const fs::path& dir, std::int64_t run_id) {
    const std::string rl = run_label_md(run_id);
    std::ofstream det(dir / (rl + ".md"));
    det << "# bench — " << rl << "\n\n[← back](README.md)\n\n";
    { auto st = db.prepare("SELECT r.host, c.name, c.version, p.build_type, r.ts_utc FROM testRun tr "
        "JOIN run r ON r.run_id=tr.run_id JOIN parameter p ON p.id=tr.parameter_id JOIN compiler c ON c.id=p.compiler_id "
        "WHERE tr.run_id=? LIMIT 1");
      st.bind(run_id);
      if (st.step()) { std::string host, name, ver, bt, ts; st.get(host, name, ver, bt, ts);
          det << "_" << host << " · " << name << " " << ver << " · " << bt << " · " << ts << "_\n\n"; } }
    det << "| config | persist | events | median ops/sec | band (low–high) | bytes |\n|---|---|--:|--:|---|--:|\n";
    auto st = db.prepare(
        "SELECT tl.name, IFNULL(p.persist,''), IFNULL(p.threads*p.events_per_thread,0), "
        "IFNULL(tr.median_ops,0), IFNULL(tr.low_ops,0), IFNULL(tr.high_ops,0), IFNULL(tr.bytes,0) "
        "FROM testRun tr JOIN testList tl ON tl.id=tr.test_list_id JOIN parameter p ON p.id=tr.parameter_id "
        "JOIN testType tt ON tt.id=tr.test_type_id WHERE tr.run_id=? AND tt.name='bench' ORDER BY IFNULL(tr.median_ops,0) DESC");
    st.bind(run_id);
    while (st.step()) {
        std::string name, per; std::int64_t events = 0, med = 0, low = 0, high = 0, bytes = 0;
        st.get(name, per, events, med, low, high, bytes);
        det << "| " << name << " | " << dash(per) << " | " << events << " | " << med << " | "
            << low << "–" << high << " | " << bytes << " |\n";
    }
}

// Bench page = a COMPILER COMPARISON pivot: config rows (config @scale) x compiler columns, cell = median ops/sec.
void write_bench_pages(jac313::Qlite::v002::Sqlite& db, const fs::path& out) {
    const auto comps = compilers_latest(db, "bench");
    if (comps.empty()) return;
    std::error_code ec; fs::create_directories(out / "bench", ec);

    std::string inlist;
    for (const auto& c : comps) { if (!inlist.empty()) inlist += ","; inlist += std::to_string(c.second); }
    auto col_for = [&](std::int64_t run) -> int {
        for (std::size_t i = 0; i < comps.size(); ++i) if (comps[i].second == run) return static_cast<int>(i);
        return -1; };

    struct Row { std::string key; std::vector<std::string> cells; };
    std::vector<Row> rows;
    {
        auto st = db.prepare(
            "SELECT tr.run_id, tl.name, IFNULL(p.threads*p.events_per_thread,0), IFNULL(tr.median_ops,0) "
            "FROM testRun tr JOIN testList tl ON tl.id=tr.test_list_id JOIN parameter p ON p.id=tr.parameter_id "
            "JOIN testType tt ON tt.id=tr.test_type_id WHERE tt.name='bench' AND tr.run_id IN (" + inlist + ") "
            "ORDER BY tl.name, p.threads*p.events_per_thread");
        while (st.step()) {
            std::int64_t run = 0, events = 0, med = 0; std::string name;
            st.get(run, name, events, med);
            const std::string key = events ? (name + " @" + std::to_string(events / 1000000) + "M") : name;
            Row* r = nullptr;
            for (auto& rr : rows) if (rr.key == key) { r = &rr; break; }
            if (!r) { rows.push_back({key, std::vector<std::string>(comps.size(), "-")}); r = &rows.back(); }
            const int col = col_for(run);
            if (col >= 0) r->cells[static_cast<std::size_t>(col)] = std::to_string(med);
        }
    }

    {
        std::ofstream md(out / "bench" / "README.md");
        md << "# bench — compiler comparison\n\n_Generated from `results.db`. Cell = median ops/sec; "
              "latest run per compiler._\n\n| config";
        for (const auto& c : comps) md << " | [" << c.first << "](" << run_label_md(c.second) << ".md)";
        md << " |\n|---";
        for (std::size_t i = 0; i < comps.size(); ++i) md << "|--:";
        md << "|\n";
        for (const auto& r : rows) {
            md << "| " << r.key;
            for (const auto& cell : r.cells) md << " | " << cell;
            md << " |\n";
        }
    }

    for (const auto& c : comps) write_bench_detail(db, out / "bench", c.second);
}

// Top-level landing index: test-summary/README.md, linking to the compiler page and every test
// type that has data (with its run/row counts).
void write_index_page(jac313::Qlite::v002::Sqlite& db, const fs::path& out) {
    std::ofstream md(out / "README.md");
    md << "# Test results\n\n_Generated from `results.db` by `jac313_test_cli --report`._\n\n"
          "| area | runs | rows |\n|---|--:|--:|\n"
          "| [compilers](compiler/README.md) | — | — |\n";
    for (const char* t : {"ctest", "smoke", "bench", "verify-lite", "verify"}) {
        std::int64_t runs = 0, rows = 0;
        { auto st = db.prepare("SELECT COUNT(DISTINCT tr.run_id), COUNT(*) FROM testRun tr "
                               "JOIN testType tt ON tt.id=tr.test_type_id WHERE tt.name=?");
          st.bind(std::string(t)); if (st.step()) st.get(runs, rows); }
        if (rows == 0) continue;
        md << "| [" << t << "](" << t << "/README.md) | " << runs << " | " << rows << " |\n";
    }
}

int run_report_command(const fs::path& source_dir) {
    const fs::path db_path = source_dir / "test-summary" / "results.db";
    std::error_code ec;
    if (!fs::exists(db_path, ec)) { std::cerr << "no results.db at " << db_path.string() << "\n"; return 1; }
    try {
        jac313::Qlite::v002::Sqlite db(db_path.string());
        jac313::results::ensure_schema(db);
        const fs::path out = source_dir / "test-summary";
        write_compiler_page(db, out);
        for (const char* t : {"ctest", "smoke", "verify-lite", "verify"}) write_type_pages(db, out, t);
        write_bench_pages(db, out);
        write_index_page(db, out);
        std::cout << "[report] wrote test-summary/README.md + {compiler,ctest,smoke,verify-lite,verify,bench}/ from results.db\n";
        return 0;
    } catch (const std::exception& e) { std::cerr << "report failed: " << e.what() << "\n"; return 1; }
}

// ctest parameter combo: just the build axes; persist/output_mode/scaling/batch/flag_count are N/A.
std::int64_t cli_parameter_id_ctest(jac313::Qlite::v002::Sqlite& db, std::int64_t compiler_id,
                                    const std::string& build_type, const std::string& modules,
                                    const std::string& import_std) {
    const char* where =
        "SELECT id FROM parameter WHERE compiler_id=? AND build_type=? AND modules=? AND import_std=? AND size IS NULL "
        "AND persist IS NULL AND output_mode IS NULL AND threads IS NULL AND events_per_thread IS NULL "
        "AND runs IS NULL AND batch IS NULL AND flag_count IS NULL AND valgrind_tool IS NULL";
    auto find = [&]() -> std::int64_t {
        std::int64_t id = 0; auto st = db.prepare(where);
        st.bind(compiler_id, build_type, modules, import_std); if (st.step()) st.get(id); return id;
    };
    std::int64_t id = find();
    if (id == 0) {
        db.exec("INSERT INTO parameter(compiler_id,build_type,modules,import_std,size,persist,output_mode,threads,"
                "events_per_thread,runs,batch,flag_count) VALUES(?,?,?,?,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)",
                compiler_id, build_type, modules, import_std);
        id = find();
    }
    return id;
}

// Verify parameter combo: build axes + the underlying test's persist (may be empty) + the valgrind
// tool (memcheck/helgrind/drd). SELECT-then-INSERT, persist NULL when the task has none (unit tests).
std::int64_t cli_parameter_id_verify(jac313::Qlite::v002::Sqlite& db, std::int64_t compiler_id,
                                     const std::string& build_type, const std::string& modules,
                                     const std::string& import_std, const std::string& persist,
                                     const std::string& tool) {
    const bool has_p = !persist.empty();
    auto find = [&]() -> std::int64_t {
        std::int64_t id = 0;
        const std::string where = std::string(
            "SELECT id FROM parameter WHERE compiler_id=? AND build_type=? AND modules=? AND import_std=? "
            "AND size IS NULL AND persist ") + (has_p ? "=?" : "IS NULL") +
            " AND output_mode IS NULL AND threads IS NULL AND events_per_thread IS NULL AND runs IS NULL "
            "AND batch IS NULL AND flag_count IS NULL AND valgrind_tool=?";
        auto st = db.prepare(where.c_str());
        if (has_p) st.bind(compiler_id, build_type, modules, import_std, persist, tool);
        else       st.bind(compiler_id, build_type, modules, import_std, tool);
        if (st.step()) st.get(id); return id;
    };
    std::int64_t id = find();
    if (id == 0) {
        if (has_p)
            db.exec("INSERT INTO parameter(compiler_id,build_type,modules,import_std,size,persist,output_mode,threads,"
                    "events_per_thread,runs,batch,flag_count,valgrind_tool) VALUES(?,?,?,?,NULL,?,NULL,NULL,NULL,NULL,NULL,NULL,?)",
                    compiler_id, build_type, modules, import_std, persist, tool);
        else
            db.exec("INSERT INTO parameter(compiler_id,build_type,modules,import_std,size,persist,output_mode,threads,"
                    "events_per_thread,runs,batch,flag_count,valgrind_tool) VALUES(?,?,?,?,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,?)",
                    compiler_id, build_type, modules, import_std, tool);
        id = find();
    }
    return id;
}

// Record one testRun row per ctest unit test into results.db.
void record_ctest_results(const fs::path& source_dir, const fs::path& build_dir,
                          const std::vector<TestResult>& results) {
    if (results.empty()) return;
    const fs::path db_path = source_dir / "test-summary" / "results.db";
    std::error_code ec;
    if (!fs::exists(db_path.parent_path(), ec)) return;
    try {
        jac313::Qlite::v002::Sqlite db(db_path.string());
        const std::int64_t run_id = cli_begin_run(db);
        for (const auto& r : results) {
            db.exec("INSERT OR IGNORE INTO testList(name) VALUES(?)", r.entry.name);
        }
        const BuildFeatures bf = read_build_features(build_dir);
        const std::int64_t comp_id = cli_compiler_id(db, read_compiler_info(build_dir));
        const std::int64_t type_id = cli_results_id(db, "SELECT id FROM testType WHERE name=?", std::string("ctest"));
        const std::int64_t param_id = cli_parameter_id_ctest(db, comp_id, bf.build_type,
                                                             bf.modules ? "on" : "off", read_import_std(build_dir));
        for (const auto& r : results) {
            const std::int64_t list_id = cli_results_id(db, "SELECT id FROM testList WHERE name=?", r.entry.name);
            db.exec("INSERT INTO testRun(run_id, test_type_id, test_list_id, parameter_id, status, duration_ms) "
                    "VALUES(?,?,?,?,?,?)",
                    run_id, type_id, list_id, param_id, std::string(matrix_status_str(r.status)),
                    static_cast<std::int64_t>(r.duration.count()));
        }
        std::cout << "[results] recorded " << results.size() << " ctest results (run " << run_id << ") -> "
                  << db_path.string() << '\n';
    } catch (const std::exception& e) {
        std::cerr << "[results] ctest record failed: " << e.what() << '\n';
    }
}

// Record one testRun row per scenario of a matrix (smoke) run into results.db.
void record_matrix_results(const fs::path& source_dir, const fs::path& build_dir,
                           const std::vector<MatrixRunResult>& results, const std::string& build_type,
                           const std::string& modules, const std::string& size) {
    if (results.empty()) return;
    const fs::path db_path = source_dir / "test-summary" / "results.db";
    std::error_code ec;
    if (!fs::exists(db_path.parent_path(), ec)) return;
    try {
        jac313::Qlite::v002::Sqlite db(db_path.string());
        const std::int64_t run_id = cli_begin_run(db);
        for (const auto& r : results) {
            db.exec("INSERT OR IGNORE INTO testList(name) VALUES(?)", r.scenario.entry.name);
        }
        const std::int64_t type_id = cli_results_id(db, "SELECT id FROM testType WHERE name=?", std::string("smoke"));
        const std::int64_t comp_id = cli_compiler_id(db, read_compiler_info(build_dir));
        const std::string import_std = read_import_std(build_dir);
        for (const auto& r : results) {
            const std::int64_t list_id = cli_results_id(db, "SELECT id FROM testList WHERE name=?", r.scenario.entry.name);
            const std::int64_t param_id = cli_parameter_id(db, comp_id, build_type, modules, import_std, size,
                r.scenario.persist, r.scenario.output_mode,
                static_cast<std::int64_t>(r.scenario.threads), static_cast<std::int64_t>(r.scenario.events_per_thread),
                static_cast<std::int64_t>(r.scenario.runs));
            db.exec("INSERT INTO testRun(run_id, test_type_id, test_list_id, parameter_id, status, duration_ms) "
                    "VALUES(?,?,?,?,?,?)",
                    run_id, type_id, list_id, param_id, std::string(matrix_status_str(r.result.status)),
                    static_cast<std::int64_t>(r.result.duration.count()));
        }
        std::cout << "[results] recorded " << results.size() << " smoke results (run " << run_id << ") -> "
                  << db_path.string() << '\n';
    } catch (const std::exception& e) {
        std::cerr << "[results] matrix record failed: " << e.what() << '\n';
    }
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
    const MatrixParams params = load_matrix_params(params_path);
    const auto scenarios = build_matrix_scenarios(
        tests, params, matrix_opts.compiler_label, global.build_dir, run_opts.filter);

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
        global.source_dir, params, matrix_opts.compiler_label, build_type, modules_label);

    std::cout << "=== jac313 test matrix ===\n";
    std::cout << "Params:  " << params_path.string() << '\n';
    std::cout << "SIZE=" << params.size << " DISK=" << params.disk_type;
    if (!params.os_id.empty()) {
        std::cout << " OS_ID=" << params.os_id;
    }
    std::cout << " COMPILER=" << matrix_opts.compiler_label
              << " BUILD=" << build_type << '\n';
    if (!run_opts.filter.empty()) {
        std::cout << "Filter:  " << run_opts.filter << '\n';
    }
    const int failsafe_sec = effective_failsafe_sec(run_opts, params.size);
    if (failsafe_sec > 0) {
        std::cout << "Failsafe: " << format_count(failsafe_sec) << "s per scenario\n";
    }
    std::cout << "Results: " << results_base.string() << '\n';

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
    run_opts_matrix.global_done_before = global_before;
    run_opts_matrix.global_total = grand_total;

    const auto results = run_matrix(scenarios, params, results_base, run_opts_matrix);
    MatrixTally tally = tally_matrix_results(results);
    if (out_tally != nullptr) {
        out_tally->passed  = static_cast<int>(tally.passed);
        out_tally->failed  = static_cast<int>(tally.failed);
        out_tally->skipped = static_cast<int>(tally.skipped);
        out_tally->errors  = static_cast<int>(tally.errors);
    }

    std::cout << "\n=== matrix summary ===\n";
    std::cout << "Passed:  " << format_count(tally.passed) << '\n';
    std::cout << "Failed:  " << format_count(tally.failed) << '\n';
    std::cout << "Skipped: " << format_count(tally.skipped) << '\n';
    std::cout << "Errors:  " << format_count(tally.errors) << '\n';

    // Capture this smoke run into results.db (one testRun row per scenario).
    record_matrix_results(global.source_dir, global.build_dir, results, build_type,
                          features.modules ? "on" : "off", params.size);

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
    global.build_dir = global.build_dir.string() + "-" + name;
    std::cout << "Build dir:  " << global.build_dir.string()
              << " (annotated: JAC313_STORE_HELGRIND_ANNOTATE=ON)\n\n";
    if (const int rc = run_configure_command(global, configure_opts); rc != 0) {
        return rc;
    }
    if (const int rc = run_build_command(global, build_opts); rc != 0) {
        return rc;
    }

    // 3. Scratch working dir so persist_* output never lands in the repo.
    const fs::path scratch = fs::temp_directory_path() / "jac313-cleancheck";
    std::error_code ec;
    fs::remove_all(scratch, ec);
    fs::create_directories(scratch, ec);
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

    // memcheck: the full memory surface.
    std::vector<VgTask> mem;
    mem.push_back({"ut:v002_test", utdir / "jac313_store_v002_test", {}});
    mem.push_back({"ut:binary_reader_test", utdir / "jac313_store_binary_reader_test", {}});
    mem.push_back({"ex:binary_persist", exdir / "jac313_store_binary_persist_smoke", {}});
    mem.push_back({"ex:binary_to_jtext", exdir / "jac313_store_binary_to_jtext_smoke", {}});
    mem.push_back({"ex:jtext_persist", exdir / "jac313_store_jtext_persist_smoke", {}});
    mem.push_back({"ex:sql_persist", exdir / "jac313_store_sql_persist_smoke", {}});
    mem.push_back({"ex:flag_routing", exdir / "jac313_store_flag_routing_smoke", {}});
    mem.push_back({"ex:v002_example", exdir / "jac313_store_v002_example", {}});
    // matrix memcheck surface: lite = binary (+ 001 across sinks); full = every sink (full coverage).
    const std::vector<std::string> sinks =
        full ? std::vector<std::string>{"binary", "jtext", "sql", "none", "inmem"}
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

    auto run_one = [&](const std::string& tool, const VgTask& t) {
        std::string persist;
        for (const auto& a : t.args) if (a.rfind("--persist=", 0) == 0) persist = a.substr(10);
        const std::string test = t.bin.filename().string();
        if (!fs::exists(t.bin)) {
            std::cout << "  [" << tool << "] " << t.label << " — MISSING BINARY\n";
            ++testfail;
            failures.push_back(tool + " " + t.label + " [missing]");
            recs.push_back({tool, test, persist, "error", 0});
            return;
        }
        std::vector<std::string> cmd = {"valgrind", "--tool=" + tool, "--error-exitcode=99"};
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
            ++testfail;
            failures.push_back(tool + " " + t.label + " [rc:" + std::to_string(r.exit_code) + "]");
        } else {
            verdict = "clean"; status = "pass";
            ++clean;
        }
        recs.push_back({tool, test, persist, status, static_cast<std::int64_t>(dur_ms)});
        std::cout << "  [" << tool << "] " << t.label << " — " << verdict << '\n';
    };

    std::cout << "--- memcheck (" << mem.size() << ") ---\n";
    for (const auto& t : mem) {
        run_one("memcheck", t);
    }
    std::cout << "\n--- helgrind (" << thr.size() << ") ---\n";
    for (const auto& t : thr) {
        run_one("helgrind", t);
    }
    std::cout << "\n--- drd (" << thr.size() << ") ---\n";
    for (const auto& t : thr) {
        run_one("drd", t);
    }

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

    // Capture each (tool x task) into results.db — test_type = this gate's name (verify-lite|verify),
    // valgrind tool as a parameter, status pass/fail/error + duration. Best-effort.
    {
        const fs::path rpath = global.source_dir / "test-summary" / "results.db";
        std::error_code rec;
        if (!recs.empty() && fs::exists(rpath.parent_path(), rec)) {
            try {
                jac313::Qlite::v002::Sqlite rdb(rpath.string());
                const std::int64_t run_id  = cli_begin_run(rdb);
                const std::int64_t comp_id = cli_compiler_id(rdb, read_compiler_info(global.build_dir));
                const std::int64_t type_id = cli_results_id(rdb, "SELECT id FROM testType WHERE name=?", name);
                for (const auto& v : recs) rdb.exec("INSERT OR IGNORE INTO testList(name) VALUES(?)", v.test);
                for (const auto& v : recs) {
                    const std::int64_t list_id  = cli_results_id(rdb, "SELECT id FROM testList WHERE name=?", v.test);
                    const std::int64_t param_id = cli_parameter_id_verify(rdb, comp_id, "Debug", "off", "off", v.persist, v.tool);
                    rdb.exec("INSERT INTO testRun(run_id, test_type_id, test_list_id, parameter_id, status, duration_ms) "
                             "VALUES(?,?,?,?,?,?)", run_id, type_id, list_id, param_id, v.status, v.dur_ms);
                }
                std::cout << "[results] recorded " << recs.size() << " " << name << " results (run " << run_id
                          << ") -> " << rpath.string() << '\n';
            } catch (const std::exception& e) {
                std::cerr << "[results] verify record failed: " << e.what() << '\n';
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
        {"Qlite", "Qlite/include/jac313/Qlite/v002/Sqlite.body.hpp", {"Qlite/include"}},
        {"jText", "jText/jText.api.inc",
            {"jText/include", "jText/src", "jText/jText.h", "jText/jText.cpp",
             "jText/jText.api.inc", "jText/jText.impl.inc"}},
        {"Store", "Store/include/jac313/Store/v002/headers/ts_store_config.hpp", {"Store/include"}},
    };
    const std::string dir = global.source_dir.string();
    std::cout << "=== version-check — version() bump owed? ===\n";
    int owed = 0;
    for (const auto& p : pkgs) {
        // Last commit that touched the version() literal ("vNNN.NNN") in this file. The regex
        // "v[0-9] is basic/extended-regex safe (no parens) and survives a future v002 bump.
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
        std::cout << "\n" << owed << " package(s) owe a bump. Bump jac313::<Pkg>::v002::version()\n"
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
        if (const int rc = run_build_command(global, build_opts); rc != 0) {
            return kBuildPhaseFailed;   // build failed -> tests skipped
        }
    }
    return run_matrix_run_command(global, matrix_opts, run_opts, build_opts.jobs, out_tally);
}

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
    bool group_id{false};      // read-only bench-group precheck (standalone; no script written)
};

int run_preset_command(const GlobalOptions& global, const ConfigureOptions& configure_opts,
                       const PresetOptions& preset)
{
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

    // Forward compiler steering to the generated `configure` calls. The functional tree also
    // honors --release / --modules; the bench tree is always Release.
    std::string cc_flags;
    if (configure_opts.compiler) {
        cc_flags = " --compiler " + *configure_opts.compiler;
    } else if (configure_opts.prefer_clang) {
        cc_flags = " --clang";
    } else if (configure_opts.prefer_gcc15) {
        cc_flags = " --gcc15";
    }
    std::string func_flags = cc_flags;
    if (configure_opts.build_type == "Release") {
        func_flags += " --release";
    }
    if (configure_opts.modules) {
        func_flags += " --modules";
    }

    std::vector<std::string> lines;
    const auto emit = [&](std::string s) { lines.push_back(std::move(s)); };
    emit("#!/usr/bin/env bash");
    emit("# Generated by jac313_test_cli — the exact commands for this run.");
    emit("# Re-run any time with:  bash run_latest_config.sh");
    emit("# Overwritten on the next preset invocation; copy it elsewhere to keep a config.");
    emit("set -euo pipefail");
    emit("cd \"" + root.string() + "\"");
    emit("CLI=\"" + self.string() + "\"");
    emit("");

    if (preset.ctest || preset.smoke) {
        emit("# functional gate(s) — Debug tree build-gcc15, built once then run");
        emit("\"$CLI\" configure --build-dir build-gcc15" + func_flags);
        emit("\"$CLI\" build --build-dir build-gcc15");
        if (preset.ctest) {
            emit("\"$CLI\" run --build-dir build-gcc15");        // ctest unit suite
        }
        if (preset.smoke) {
            emit("\"$CLI\" matrix run --build-dir build-gcc15"); // persist x output smoke grid
        }
        emit("");
    }

    if (preset.verify_lite) {
        emit("# memory/thread gate — valgrind memcheck (ctest + smoke surface)");
        emit("\"$CLI\" matrix verify-lite");
        emit("");
    }
    if (preset.verify) {
        emit("# memory/thread gate — valgrind memcheck + helgrind + DRD");
        emit("\"$CLI\" matrix verify");
        emit("");
    }

    if (preset.bench) {
        emit("# throughput benchmark — Release tree build-bench, store_bench target only");
        emit("\"$CLI\" configure --release --build-dir build-bench" + cc_flags);
        emit("cmake --build build-bench --target jac313_store_bench");
        emit("BENCH=\"build-bench/Store/tests/matrix/jac313_store_bench\"");
        if (preset.report) {
            emit("# record numbers to results.db, then render the report from it (host label auto-resolved)");
            emit("\"$BENCH\" --suite --db test-summary/results.db --jtext-ver v002.002");
            emit("./jac313_test_cli --report");
        } else {
            emit("\"$BENCH\" --suite");                          // numbers to stdout only
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
    return WIFEXITED(rc) ? WEXITSTATUS(rc) : 1;
}

// Read-only bench-group precheck: ensure a Release store_bench is built (incremental), then run its
// `--group-id` mode against the tracked DB — list existing groups + this machine's proposed
// group_id, so you can see whether recording reuses a group or makes a new one. Inspection only:
// it does NOT write run_latest_config.sh, so it won't clobber your last preset run.
int run_group_id_command(GlobalOptions global, ConfigureOptions configure_opts, BuildOptions build_opts)
{
    const fs::path root = fs::absolute(global.source_dir);
    configure_opts.build_type = "Release";
    configure_opts.store_tests = true;
    MatrixOptions matrix_opts;
    if (!apply_compiler_resolution(global, configure_opts, matrix_opts)) {
        return 1;
    }
    global.build_dir = root / "build-bench";
    if (const int rc = run_configure_command(global, configure_opts); rc != 0) {
        return rc;
    }
    if (const int rc = run_build_targets_command(global, build_opts, {"jac313_store_bench"}); rc != 0) {
        return rc;
    }
    const fs::path bench = root / "build-bench" / "Store" / "tests" / "matrix" / "jac313_store_bench";
    std::cout.flush();   // flush buffered cout before the child writes to the inherited fd
    std::cerr.flush();
    const std::string cmd = "cd \"" + root.string() + "\" && \"" + bench.string()
                          + "\" --group-id --db test-summary/results.db";
    const int rc = std::system(cmd.c_str());
    return WIFEXITED(rc) ? WEXITSTATUS(rc) : 1;
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    // Drop inherited exported bash functions so activation probes via /bin/sh -c
    // don't emit "error importing function definition" noise.
    scrub_exported_shell_functions();

    GlobalOptions global;
    global.source_dir = detect_source_dir(fs::current_path());

    std::string command = argv[1];
    int argi = 2;

    if (command == "--help" || command == "-h") {
        print_usage();
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
            } else if (arg == "--group-id") {
                preset.group_id = true;
            } else if (arg == "--params" && i + 1 < argc) {
                matrix_opts.params_file = argv[++i];
            } else if (arg == "--dry-run") {
                matrix_opts.dry_run = true;
            } else if (arg == "--compiler" && i + 1 < argc) {
                configure_opts.compiler = argv[++i];
            } else if (arg == "--gcc15") {
                configure_opts.prefer_gcc15 = true;
            } else if (arg == "--clang") {
                configure_opts.prefer_clang = true;
            } else if (arg == "--modules") {
                configure_opts.modules = true;
            } else if (arg == "--release") {
                configure_opts.build_type = "Release";
            } else if (arg == "--debug") {
                configure_opts.build_type = "Debug";
            } else if (arg == "--no-store-tests") {
                configure_opts.store_tests = false;
            } else if (arg == "-j" && i + 1 < argc) {
                build_opts.jobs = std::stoi(argv[++i]);
            } else if (arg == "--help" || arg == "-h") {
                print_usage();
                std::exit(0);
            } else {
                std::cerr << "Unknown option: " << arg << '\n';
                std::exit(2);
            }
        }
    };

    if (command == "matrix") {
        if (argc < 3 || std::string(argv[2]) == "--help" || std::string(argv[2]) == "-h") {
            std::cout <<
                "matrix commands:\n"
                "  run            Run selected package scenarios\n"
                "  runner         Run ONE explicit config: --compiler --build-type --modules --size\n"
                "  verify-lite    valgrind gate: ctest under memcheck + helgrind/DRD\n"
                "  verify         valgrind gate: ctest + smoke matrix (full sink coverage)\n\n"
                "A matrix run is a correctness gate only; throughput + the committed report come\n"
                "from store_bench (--db bench_results.db / --report).\n\n"
                "matrix options:\n"
                "  --params <file>   Params file (default: tests/test_params.txt)\n"
                "  --dry-run         List scenarios only\n"
                "  --modules         Configure+build with modules before run\n"
                "  --filter <regex>  Match test name, persist, or name|persist|mode\n"
                "  --failsafe <sec>  Kill stuck scenario after N seconds (smoke: "
                + std::to_string(kSmokeFailsafeSec) + ", full: " + std::to_string(kFullFailsafeSec)
                + ", 0=off)\n"
                "  --clang           Force clang++-20 for results path / auto build dir\n"
                "  --compiler <cxx>  Explicit compiler (skips auto-detect; clang min 20)\n";
            return argc < 3 ? 1 : 0;
        }
        const std::string sub = argv[2];

        if (sub == "run") {
            parse_common(3);
            return execute_matrix_run(global, configure_opts, matrix_opts, run_opts, build_opts);
        }

        if (sub == "runner") {
            // Explicit-options primitive: every dimension named, no ad-hoc flags.
            // This is the unit `run-all` composes. Accepts any compilers.conf label.
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
                        "  --compiler gcc15|gcc16|clang  (required; any compilers.conf label)\n"
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
            if (compiler == "gcc15") {
                configure_opts.prefer_gcc15 = true;
                matrix_opts.compiler_label = "gcc15";
            } else if (compiler == "clang") {
                configure_opts.prefer_clang = true;
                matrix_opts.compiler_label = "clang";
            } else if (!compiler.empty()) {
                // Any other registry label (gcc16, future toolchains): resolved by label
                // via compilers.conf; the run is recorded under that exact label.
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
                matrix_opts.params_file = "tests/test_params.txt";
            } else if (size == "full") {
                matrix_opts.params_file = "tests/test_params_full.txt";
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
                if (a == "--gcc15") {
                    co.prefer_gcc15 = true;
                } else if (a == "--clang") {
                    co.prefer_clang = true;
                } else if (a == "--compiler" && i + 1 < argc) {
                    co.compiler = argv[++i];
                } else if (a == "--source-dir" && i + 1 < argc) {
                    global.source_dir = argv[++i];
                } else if (a == "-v" || a == "--verbose") {
                    global.verbose = true;
                } else if (a == "--help" || a == "-h") {
                    std::cout << "matrix " << sub << " — valgrind gate ("
                              << (full ? "ctest + smoke matrix" : "ctest representative")
                              << ", memcheck + helgrind/DRD)\n\n"
                                 "  --gcc15 | --clang | --compiler <cxx>   (default: auto-detect)\n"
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
        if (preset.group_id) {
            return run_group_id_command(global, configure_opts, build_opts);
        }
        const bool any_preset = preset.ctest || preset.smoke || preset.bench
                             || preset.report || preset.verify_lite || preset.verify;
        if (any_preset) {
            if (preset.report && !preset.bench) {
                return run_report_command(global.source_dir);   // generate the report pages from results.db
            }
            return run_preset_command(global, configure_opts, preset);
        }
        return run_tests_command(global, run_opts, report_path);
    }
    if (command == "compilers") {
        return run_compilers_command(global, configure_opts);
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
        return run_build_command(global, build_opts);
    }
    print_usage();
    return 1;
}