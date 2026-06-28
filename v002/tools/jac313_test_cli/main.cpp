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
#include <chrono>
#include <locale>
#include <sstream>
#include <map>
#include <unordered_map>
#include <iomanip>

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
        "  build-times compile-time matrix (smoke+bench x front-end x compiler), gap-filled per host\n"
        "  pin         find-or-create this machine's committed compiler pin (Setup/compilers.pin)\n"
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
        "  --group-id           read-only precheck: which jac313-<group_id> this machine records under\n"
        "  --run-everything     the FULL battery: every gate on both compilers + build-times + report\n"
        "  (everyday: --ctest --smoke ; recorded bench: --bench --report ; all of it: --run-everything)\n\n"
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

// "jac313-### · compiler · disk" — the host/compiler/disk line shown above each summary's pass/fail.
// compiler comes from the build dir (build-gcc15 -> gcc15); jac313-### is resolved against host_spec.
inline std::string summary_context_line(const GlobalOptions& global) {
    const auto hw = collect_host_hardware_record(detect_disk_type("."));
    std::string cc = global.build_dir.filename().string();
    if (cc.rfind("build-", 0) == 0) cc = cc.substr(6);
    std::string label = "jac313-???";
    try {
        jac313::Qlite::v002::Sqlite db((global.source_dir / "test-summary" / "results.db").string());
        jac313::results::ensure_schema(db);
        const jac313::results::HostId h{hw.cpu_model, static_cast<std::int64_t>(hw.cpu_cores),
            static_cast<std::int64_t>(hw.ram_gb), hw.os_pretty, hw.disk_type_label,
            static_cast<std::int64_t>(hw.p_cores), static_cast<std::int64_t>(hw.cpu_mhz_min),
            static_cast<std::int64_t>(hw.cpu_mhz_max)};
        label = jac313::results::host_label(jac313::results::group_id(db, h));
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

    const auto results = run_tests(tests, opts, global.verbose);
    if (results.empty()) {
        return 1;
    }
    const auto summary = summarize(results);
    print_summary(summary, results, summary_context_line(global));

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
// Schema + lookup live in jac313_results_db.hpp (shared with store_bench, single source of truth).
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
    const auto hw = collect_host_hardware_record(detect_disk_type("."));
    const jac313::results::HostId h{hw.cpu_model, static_cast<std::int64_t>(hw.cpu_cores),
                                    static_cast<std::int64_t>(hw.ram_gb), hw.os_pretty, hw.disk_type_label, static_cast<std::int64_t>(hw.p_cores), static_cast<std::int64_t>(hw.cpu_mhz_min), static_cast<std::int64_t>(hw.cpu_mhz_max)};
    const std::int64_t group_id = jac313::results::pin_host(db, h);   // find-or-assign + write host_spec + current_host
    const std::int64_t run_id   = jac313::results::next_run_id(db);
    jac313::results::insert_run(db, run_id, group_id, jac313::results::host_label(group_id));
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
            << status << " | " << fmt_num(ms) << " |\n";
    }
}

// Per-type page = a HOST-SCOPED compiler comparison: each machine (jac313-###) is its own section
// with a hardware header (cpu/cores/ram/os) and its OWN compiler columns. Scenario rows × compiler
// columns; cell = ms (pass) or status. (ctest/smoke = ms; verify/verify-lite = pass·ms.)
void write_type_pages(jac313::Qlite::v002::Sqlite& db, const fs::path& out, const std::string& type) {
    std::vector<std::int64_t> groups;
    { auto st = db.prepare("SELECT DISTINCT r.group_id FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
        "JOIN testType tt ON tt.id=tr.test_type_id WHERE tt.name=? ORDER BY r.group_id");
      st.bind(type); while (st.step()) { std::int64_t g = 0; st.get(g); groups.push_back(g); } }
    if (groups.empty()) return;
    std::error_code ec; fs::create_directories(out / type, ec);
    std::ofstream md(out / type / "README.md");
    md << "# " << type << " — by machine\n\n_Generated from `results.db`. Each machine (`jac313-###`) is its "
          "own section. Columns are that host's compilers (latest run each); cell = ms (pass) or status._\n";

    const bool is_verify = (type == "verify" || type == "verify-lite");
    for (const std::int64_t g : groups) {
        std::vector<std::pair<std::string, std::int64_t>> comps;   // this host's compilers (latest run each)
        { auto st = db.prepare(
            "SELECT c.name, c.major, MAX(tr.run_id) FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
            "JOIN parameter p ON p.id=tr.parameter_id JOIN compiler c ON c.id=p.compiler_id "
            "JOIN testType tt ON tt.id=tr.test_type_id WHERE tt.name=? AND r.group_id=? "
            "GROUP BY c.id ORDER BY c.name, c.major");
          st.bind(type, g);
          while (st.step()) { std::string name; std::int64_t major = 0, run = 0; st.get(name, major, run);
              comps.push_back({comp_label(name, major), run}); } }
        if (comps.empty()) continue;

        std::string cpu, os, disk; std::int64_t cores = 0, ram = 0;
        { auto st = db.prepare("SELECT cpu, cores, ram_gb, os, disk FROM host_spec WHERE group_id=? LIMIT 1");
          st.bind(g); if (st.step()) st.get(cpu, cores, ram, os, disk); }
        md << "\n## " << jac313::results::host_label(g) << " — " << dash(cpu) << " · " << cores
           << " cores · " << ram << " GB · " << dash(os) << (disk.empty() ? std::string() : " · " + disk) << "\n\n";

        std::string inlist;
        for (const auto& c : comps) { if (!inlist.empty()) inlist += ","; inlist += std::to_string(c.second); }
        auto col_for = [&](std::int64_t run) -> int {
            for (std::size_t i = 0; i < comps.size(); ++i) if (comps[i].second == run) return static_cast<int>(i);
            return -1; };

        struct Row { std::string key; std::vector<std::string> cells; };
        std::vector<Row> rows;
        { auto st = db.prepare(
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
              if (col >= 0)
                  r->cells[static_cast<std::size_t>(col)] =
                      (status == "pass")
                          ? (is_verify ? ("pass · " + fmt_num(ms)) : (ms ? fmt_num(ms) : std::string("pass")))
                          : status; } }

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
        for (const auto& c : comps) write_run_detail(db, out / type, type, c.second);
    }
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
        det << "| " << name << " | " << dash(per) << " | " << fmt_num(events) << " | " << fmt_num(med) << " | "
            << human_ops(low) << "–" << human_ops(high) << " | " << human_bytes(bytes) << " |\n";
    }
}

// Bench page = a HOST-SCOPED compiler comparison: throughput is hardware-specific, so each machine
// (jac313-###) is its own section with a hardware header (cpu/cores/ram/os) and its OWN compiler
// columns. Config rows (config @scale) × compiler columns; cell = median ops/sec · low–high band ·
// footprint (latest run per compiler, for that host).
void write_bench_pages(jac313::Qlite::v002::Sqlite& db, const fs::path& out) {
    std::vector<std::int64_t> groups;
    { auto st = db.prepare("SELECT DISTINCT r.group_id FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
        "JOIN testType tt ON tt.id=tr.test_type_id WHERE tt.name='bench' ORDER BY r.group_id");
      while (st.step()) { std::int64_t g = 0; st.get(g); groups.push_back(g); } }
    if (groups.empty()) return;
    std::error_code ec; fs::create_directories(out / "bench", ec);
    std::ofstream md(out / "bench" / "README.md");
    md << "# bench — throughput by machine\n\n_Generated from `results.db`. Throughput is hardware-specific, "
          "so each machine (`jac313-###`) is its own section. Per compiler: median ops/sec, the low–high band, "
          "and footprint; latest run per compiler._\n";

    struct Cell { std::string median = "-", band = "-", size = "-"; };
    struct Row { std::string key; std::vector<Cell> cells; };
    for (const std::int64_t g : groups) {
        std::vector<std::pair<std::string, std::int64_t>> comps;   // this host's compilers (latest run each)
        { auto st = db.prepare(
            "SELECT c.name, c.major, MAX(tr.run_id) FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
            "JOIN parameter p ON p.id=tr.parameter_id JOIN compiler c ON c.id=p.compiler_id "
            "JOIN testType tt ON tt.id=tr.test_type_id WHERE tt.name='bench' AND r.group_id=? "
            "GROUP BY c.id ORDER BY c.name, c.major");
          st.bind(g);
          while (st.step()) { std::string name; std::int64_t major = 0, run = 0; st.get(name, major, run);
              comps.push_back({comp_label(name, major), run}); } }
        if (comps.empty()) continue;

        std::string cpu, os, disk; std::int64_t cores = 0, ram = 0;
        { auto st = db.prepare("SELECT cpu, cores, ram_gb, os, disk FROM host_spec WHERE group_id=? LIMIT 1");
          st.bind(g); if (st.step()) st.get(cpu, cores, ram, os, disk); }
        md << "\n## " << jac313::results::host_label(g) << " — " << dash(cpu) << " · " << cores
           << " cores · " << ram << " GB · " << dash(os) << (disk.empty() ? std::string() : " · " + disk) << "\n\n";

        std::string inlist;
        for (const auto& c : comps) { if (!inlist.empty()) inlist += ","; inlist += std::to_string(c.second); }
        auto col_for = [&](std::int64_t run) -> int {
            for (std::size_t i = 0; i < comps.size(); ++i) if (comps[i].second == run) return static_cast<int>(i);
            return -1; };

        std::vector<Row> rows;
        { auto st = db.prepare(
            "SELECT tr.run_id, tl.name, IFNULL(p.threads*p.events_per_thread,0), IFNULL(tr.median_ops,0), "
            "IFNULL(tr.low_ops,0), IFNULL(tr.high_ops,0), IFNULL(tr.bytes,0) "
            "FROM testRun tr JOIN testList tl ON tl.id=tr.test_list_id JOIN parameter p ON p.id=tr.parameter_id "
            "JOIN testType tt ON tt.id=tr.test_type_id WHERE tt.name='bench' AND tr.run_id IN (" + inlist + ") "
            "ORDER BY tl.name, p.threads*p.events_per_thread");
          while (st.step()) {
              std::int64_t run = 0, events = 0, med = 0, low = 0, high = 0, bytes = 0; std::string name;
              st.get(run, name, events, med, low, high, bytes);
              const std::string key = events ? (name + " @" + std::to_string(events / 1000000) + "M") : name;
              Row* r = nullptr;
              for (auto& rr : rows) if (rr.key == key) { r = &rr; break; }
              if (!r) { rows.push_back({key, std::vector<Cell>(comps.size())}); r = &rows.back(); }
              const int col = col_for(run);
              if (col >= 0) { Cell& cell = r->cells[static_cast<std::size_t>(col)];
                              cell.median = fmt_num(med);
                              cell.band   = human_ops(low) + "–" + human_ops(high);
                              cell.size   = human_bytes(bytes); } } }

        md << "| config";
        for (const auto& c : comps) {
            const std::string rl = run_label_md(c.second);
            md << " | [" << c.first << "](" << rl << ".md)<br>median ops/sec | " << c.first << "<br>band | "
               << c.first << "<br>size";
        }
        md << " |\n|---";
        for (std::size_t i = 0; i < comps.size(); ++i) md << "|--:|:--:|--:";
        md << "|\n";
        for (const auto& r : rows) {
            md << "| " << r.key;
            for (const auto& cell : r.cells) md << " | " << cell.median << " | " << cell.band << " | " << cell.size;
            md << " |\n";
        }
        for (const auto& c : comps) write_bench_detail(db, out / "bench", c.second);
    }
}

// build = a HOST-SCOPED compile-time matrix: compile time is hardware-specific, so each machine
// (jac313-###) is its own section with a hardware header (cpu/cores/ram/os) and its OWN config
// columns (different boxes run different compilers). Rows = tests, columns = config (compiler ×
// front-end: hdr/mod/istd). Cell = compile+link seconds · status · binary size (latest build).
void write_build_pages(jac313::Qlite::v002::Sqlite& db, const fs::path& out) {
    std::vector<std::int64_t> groups;
    { auto st = db.prepare("SELECT DISTINCT r.group_id FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
        "JOIN testType tt ON tt.id=tr.test_type_id WHERE tt.name='build' ORDER BY r.group_id");
      while (st.step()) { std::int64_t g = 0; st.get(g); groups.push_back(g); } }
    if (groups.empty()) return;
    // Output dir is compiler-build-times/ (NOT build/) so the **/build/ ignore rule for CMake trees
    // doesn't swallow this report page. testType name in the DB stays 'build'.
    std::error_code ec; fs::create_directories(out / "compiler-build-times", ec);
    std::ofstream md(out / "compiler-build-times" / "README.md");
    md << "# Compiler build times\n\n_Generated from `results.db`. Compile time is hardware-specific, so "
          "each machine (`jac313-###`) is its own section. Cell = compile+link seconds · status · binary "
          "size (latest build); per test × front-end × compiler._\n";

    struct Config { std::string label, cname, bt, mod, imp; std::int64_t cmajor; };
    for (const std::int64_t g : groups) {
        std::string cpu, os, disk; std::int64_t cores = 0, ram = 0;
        { auto st = db.prepare("SELECT cpu, cores, ram_gb, os, disk FROM host_spec WHERE group_id=? LIMIT 1");
          st.bind(g); if (st.step()) st.get(cpu, cores, ram, os, disk); }
        md << "\n## " << jac313::results::host_label(g) << " — " << dash(cpu) << " · " << cores
           << " cores · " << ram << " GB · " << dash(os) << (disk.empty() ? std::string() : " · " + disk) << "\n\n";

        std::vector<Config> configs;
        { auto st = db.prepare("SELECT DISTINCT c.name, c.major, p.build_type, p.modules, p.import_std "
            "FROM testRun tr JOIN run r ON r.run_id=tr.run_id JOIN parameter p ON p.id=tr.parameter_id "
            "JOIN compiler c ON c.id=p.compiler_id JOIN testType tt ON tt.id=tr.test_type_id "
            "WHERE tt.name='build' AND r.group_id=? ORDER BY c.name, c.major, p.modules, p.import_std");
          st.bind(g);
          while (st.step()) { std::string cn, bt, mod, imp; std::int64_t cm = 0; st.get(cn, cm, bt, mod, imp);
              const std::string fe = (mod != "on") ? "hdr" : (imp == "on" ? "istd" : "mod");
              configs.push_back({comp_label(cn, cm) + "·" + fe, cn, bt, mod, imp, cm}); } }
        if (configs.empty()) continue;

        std::vector<std::string> tests;
        { auto st = db.prepare("SELECT DISTINCT tl.name FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
            "JOIN testList tl ON tl.id=tr.test_list_id JOIN testType tt ON tt.id=tr.test_type_id "
            "WHERE tt.name='build' AND r.group_id=? ORDER BY tl.name");
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
                auto st = db.prepare("SELECT IFNULL(tr.duration_ms,0), IFNULL(tr.status,''), IFNULL(tr.bytes,0) "
                    "FROM testRun tr JOIN run r ON r.run_id=tr.run_id JOIN parameter p ON p.id=tr.parameter_id "
                    "JOIN compiler c ON c.id=p.compiler_id JOIN testList tl ON tl.id=tr.test_list_id "
                    "JOIN testType tt ON tt.id=tr.test_type_id "
                    "WHERE tt.name='build' AND r.group_id=? AND tl.name=? AND c.name=? AND c.major=? "
                    "AND p.build_type=? AND p.modules=? AND p.import_std=? ORDER BY tr.run_id DESC LIMIT 1");
                st.bind(g, t, cf.cname, cf.cmajor, cf.bt, cf.mod, cf.imp);
                if (st.step()) { std::int64_t ms = 0, bytes = 0; std::string status; st.get(ms, status, bytes);
                    if (status == "NA") { cell = "NA"; }   // config not built by design
                    else if (ms > 0) { cell = fmt_build_seconds(ms) + " · " + (status.empty() ? "-" : status)
                                  + (bytes ? (" · " + human_bytes(bytes)) : std::string()); } }
                md << " | " << cell;
            }
            md << " |\n";
        }
    }
}

// Top-level landing index: test-summary/README.md, linking to the compiler page and every test
// type that has data (with its run/row counts).
void write_index_page(jac313::Qlite::v002::Sqlite& db, const fs::path& out) {
    std::ofstream md(out / "README.md");
    md << "# Test results\n\n_Generated from `results.db` by `jac313_test_cli --report`._\n\n";

    // Machines table — decodes each jac313-### (latest run per group) with its hardware spec.
    {
        bool any = false;
        auto st = db.prepare(
            "SELECT group_id, cpu, cpu_mhz_min, cpu_mhz_max, p_cores, cores, ram_gb, disk, os "
            "FROM host_spec ORDER BY group_id");
        while (st.step()) {
            if (!any) { md << "## Machines\n\n| machine | CPU | Speed | P.Cores | T.Cores | RAM | Disk | OS |\n"
                              "|---|---|--:|--:|--:|--:|---|---|\n"; any = true; }
            std::int64_t gid = 0, mhz_lo = 0, mhz_hi = 0, p_cores = 0, t_cores = 0, ram = 0;
            std::string cpu, disk, os;
            st.get(gid, cpu, mhz_lo, mhz_hi, p_cores, t_cores, ram, disk, os);
            std::string speed = "-";
            if (mhz_hi > 0) { char b[40]; std::snprintf(b, sizeof b, "%.1f–%.1f GHz",
                              static_cast<double>(mhz_lo) / 1000.0, static_cast<double>(mhz_hi) / 1000.0); speed = b; }
            md << "| " << jac313::results::host_label(gid) << " | " << dash(cpu) << " | " << speed
               << " | " << p_cores << " | " << t_cores << " | " << ram << " GB | " << dash(disk)
               << " | " << dash(os) << " |\n";
        }
        if (any) md << "\n\\* P.Cores = Physical Cores; T.Cores = Threading Cores\n\n";
    }

    md << "| area | runs | rows |\n|---|--:|--:|\n"
          "| [compilers](compiler/README.md) | — | — |\n";
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
        write_build_pages(db, out);
        write_index_page(db, out);
        std::cout << "[report] wrote test-summary/README.md + {compiler,ctest,smoke,verify-lite,verify,bench,compiler-build-times}/ from results.db\n";
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
// rows for THIS host (group). Banks every target that actually built, so a normal ctest/smoke
// build records its times for free. Front-end (modules/import-std) is read from the tree unless
// overridden. Group-scoped upsert: a prior row for the same (this host, config, test) is replaced;
// other hosts' rows (same config, different group_id) are left intact.
void capture_build_times(const fs::path& source_dir, const fs::path& build_dir,
                         const std::string& modules_override = "",
                         const std::string& import_std_override = "") {
    const fs::path db_path = source_dir / "test-summary" / "results.db";
    std::error_code ec;
    if (!fs::exists(db_path.parent_path(), ec)) return;
    std::vector<std::string> names;
    for (const auto& t : discover_tests(build_dir)) names.push_back(t.name);
    // jac313_store_bench is a real build target but NOT a ctest test, so discover_tests misses it.
    // Add it explicitly so its compile time is banked whenever the tree actually built it.
    if (std::find(names.begin(), names.end(), "jac313_store_bench") == names.end())
        names.push_back("jac313_store_bench");
    if (names.empty()) return;
    const auto times = parse_ninja_build_times(build_dir, names);
    try {
        jac313::Qlite::v002::Sqlite db(db_path.string());
        const std::int64_t run_id  = cli_begin_run(db);   // one run for the whole capture batch
        std::int64_t gid = 0;
        { auto st = db.prepare("SELECT group_id FROM run WHERE run_id=?"); st.bind(run_id); if (st.step()) st.get(gid); }
        const std::int64_t type_id = cli_results_id(db, "SELECT id FROM testType WHERE name=?", std::string("build"));
        const std::int64_t comp_id = cli_compiler_id(db, read_compiler_info(build_dir));
        const BuildFeatures bf = read_build_features(build_dir);
        const std::string modules = modules_override.empty() ? (bf.modules ? "on" : "off") : modules_override;
        const std::string istd    = import_std_override.empty() ? read_import_std(build_dir) : import_std_override;
        const std::int64_t param_id = cli_parameter_id_ctest(db, comp_id, bf.build_type, modules, istd);
        int banked = 0;
        for (const auto& [t, ms] : times) {
            if (ms <= 0) continue;   // not built in this tree — leave any prior row intact
            db.exec("INSERT OR IGNORE INTO testList(name) VALUES(?)", t);
            const std::int64_t list_id = cli_results_id(db, "SELECT id FROM testList WHERE name=?", t);
            const std::int64_t bytes = exe_size_in_tree(build_dir, t);
            db.exec("DELETE FROM testRun WHERE test_type_id=? AND test_list_id=? AND parameter_id=? "
                    "AND run_id IN (SELECT run_id FROM run WHERE group_id=?)", type_id, list_id, param_id, gid);
            db.exec("INSERT INTO testRun(run_id, test_type_id, test_list_id, parameter_id, status, duration_ms, bytes) "
                    "VALUES(?,?,?,?,?,?,?)", run_id, type_id, list_id, param_id, std::string("built"), ms, bytes);
            ++banked;
        }
        const std::string fe = (modules == "on") ? (istd == "on" ? "istd" : "mod") : "hdr";
        std::cout << "[results] build-times: banked " << banked << " target(s) under "
                  << jac313::results::host_label(gid) << " " << read_compiler_info(build_dir).name
                  << "·" << fe << " (run " << run_id << ")\n";
    } catch (const std::exception& e) {
        std::cerr << "[results] build-times capture failed: " << e.what() << "\n";
    }
}

// Build, then on success bank this tree's compile times (capture-on-build). Use in place of
// run_build_command wherever the CLI builds the test tree, so a normal run records times for free.
int build_and_capture(const GlobalOptions& global, const BuildOptions& build_opts) {
    const int rc = run_build_command(global, build_opts);
    if (rc == 0) capture_build_times(global.source_dir, global.build_dir);
    return rc;
}

// ---- build-times gate (the folded-in build_matrix.sh) ----------------------------------------
// The compile-time matrix: smoke + bench targets × {hdr, mod, istd} front-ends × {gcc, clang},
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
// (read_compiler_pins / CompilerPinRow live further down in this file).
std::string resolve_pinned_cc(const fs::path& src, bool want_gcc);

// Does a 'build' row already exist for (this host group, compiler, front-end, target)?
bool build_row_exists(jac313::Qlite::v002::Sqlite& db, std::int64_t gid, std::int64_t comp_id,
                      const std::string& modules, const std::string& istd, const std::string& target) {
    auto st = db.prepare(
        "SELECT 1 FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
        "JOIN testType tt ON tt.id=tr.test_type_id JOIN testList tl ON tl.id=tr.test_list_id "
        "JOIN parameter p ON p.id=tr.parameter_id "
        "WHERE tt.name='build' AND r.group_id=? AND p.compiler_id=? AND p.modules=? AND p.import_std=? "
        "AND tl.name=? LIMIT 1");
    st.bind(gid, comp_id, modules, istd, target);
    return st.step();
}

struct BuildTimesCfg { std::string label, cc; std::vector<std::string> feflags; std::string mod, istd; };

int run_build_times_gate(const fs::path& source_dir, bool dry_run, bool force = false) {
    const std::string gcc_cc = resolve_pinned_cc(source_dir, true);
    const std::string clang_cc = resolve_pinned_cc(source_dir, false);
    if (gcc_cc.empty() && clang_cc.empty()) {
        std::cerr << "build-times: no pinned compiler resolved — run ./bootstrap.sh first.\n";
        return 1;
    }
    std::vector<BuildTimesCfg> cfgs;
    const std::vector<std::string> istd_flags = {"-DJAC313_BUILD_MODULES=ON", "-DJAC313_QLITE_IMPORT_STD=ON",
                                                 "-DJAC313_JTEXT_IMPORT_STD=ON", "-DJAC313_STORE_IMPORT_STD=ON"};
    for (const auto& [name, cc] : {std::pair<std::string,std::string>{"gcc", gcc_cc}, {"clang", clang_cc}}) {
        if (cc.empty()) continue;
        cfgs.push_back({name + "-hdr",  cc, {}, "off", "off"});
        cfgs.push_back({name + "-mod",  cc, {"-DJAC313_BUILD_MODULES=ON"}, "on", "off"});
        cfgs.push_back({name + "-istd", cc, istd_flags, "on", "on"});
    }

    const fs::path db_path = source_dir / "test-summary" / "results.db";
    std::error_code ec;
    if (!fs::exists(db_path.parent_path(), ec)) { std::cerr << "build-times: no test-summary/.\n"; return 1; }
    jac313::Qlite::v002::Sqlite db(db_path.string());
    jac313::results::ensure_schema(db);
    const HostHardwareRecord hw = collect_host_hardware_record(detect_disk_type("."));
    const jac313::results::HostId h{hw.cpu_model, static_cast<std::int64_t>(hw.cpu_cores),
                                    static_cast<std::int64_t>(hw.ram_gb), hw.os_pretty, hw.disk_type_label, static_cast<std::int64_t>(hw.p_cores), static_cast<std::int64_t>(hw.cpu_mhz_min), static_cast<std::int64_t>(hw.cpu_mhz_max)};
    bool new_setup = false;
    { auto st = db.prepare("SELECT 1 FROM host_spec WHERE cpu=? AND cores=? AND ram_gb=? AND os=? AND disk=? LIMIT 1");
      st.bind(h.cpu, h.cores, h.ram_gb, h.os, h.disk); new_setup = !st.step(); }
    const std::int64_t gid = jac313::results::pin_host(db, h);   // record host_spec + current_host
    const std::string host = jac313::results::host_label(gid);
    std::cout << "=== build-times gate — " << host << (new_setup ? "  *** NEW SETUP (full collection) ***" : "")
              << " ===\n";
    if (dry_run) {
        std::cout << cfgs.size() << " configs (smoke+bench × {hdr,mod,istd} × {gcc,clang}); per config:\n";
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
        const int expected = (gcc_cc.empty() ? 0 : 3) + (clang_cc.empty() ? 0 : 3);
        int combos = 0; std::int64_t lo = -1, hi = 0;
        auto st = db.prepare(
            "SELECT COUNT(DISTINCT tr.test_list_id) FROM testRun tr JOIN run r ON r.run_id=tr.run_id "
            "JOIN testType tt ON tt.id=tr.test_type_id JOIN parameter p ON p.id=tr.parameter_id "
            "JOIN compiler c ON c.id=p.compiler_id WHERE tt.name='build' AND r.group_id=? "
            "GROUP BY c.name, c.major, p.modules, p.import_std");
        st.bind(gid);
        while (st.step()) { std::int64_t n = 0; st.get(n); ++combos; if (lo < 0 || n < lo) lo = n; if (n > hi) hi = n; }
        if (combos >= expected && lo == hi && lo > 0) {
            std::cout << "  complete: " << combos << " configs × " << lo << " targets already recorded for "
                      << host << " — nothing to build.\n";
            return 0;
        }
    }

    std::vector<std::string> targets;   // smoke+bench target names, discovered once
    int built_total = 0, skipped = 0;
    for (const auto& c : cfgs) {
        const fs::path dir = source_dir / "tmp_build" / c.label;
        safe_rmdir(source_dir, dir);
        std::vector<std::string> conf = {"cmake", "-G", "Ninja", "-S", ".", "-B", "tmp_build/" + c.label,
            "-DCMAKE_CXX_COMPILER=" + c.cc, "-DCMAKE_BUILD_TYPE=Debug", "-DJAC313_BUILD_STORE_TESTS=ON"};
        conf.insert(conf.end(), c.feflags.begin(), c.feflags.end());
        if (run_process(conf, 0, source_dir.string()).exit_code != 0) {
            std::cout << "  " << c.label << ": CONFIGURE FAILED — skipped\n";
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
            if (force || !build_row_exists(db, gid, comp_id, c.mod, c.istd, t)) missing.push_back(t);
        if (missing.empty()) {
            std::cout << "  " << c.label << ": complete (" << targets.size() << " targets) — skip\n";
            safe_rmdir(source_dir, dir); ++skipped; continue;
        }
        std::cout << "  " << c.label << ": building " << missing.size() << " of " << targets.size() << " targets...\n";
        std::vector<std::string> build = {"cmake", "--build", "tmp_build/" + c.label};
        for (const auto& m : missing) { build.push_back("--target"); build.push_back(m); }
        run_process(build, 0, source_dir.string());   // ignore rc: capture records whatever built
        capture_build_times(source_dir, dir, c.mod, c.istd);
        built_total += static_cast<int>(missing.size());
        safe_rmdir(source_dir, dir);
    }
    safe_rmdir(source_dir, source_dir / "tmp_build");
    std::cout << "build-times gate: " << built_total << " target-builds recorded, " << skipped
              << " config(s) already complete for " << host << ".\n";
    return 0;
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
        if (const int rc = build_and_capture(global, build_opts); rc != 0) {
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
    bool group_id{false};      // read-only precheck: which jac313-<group_id> this machine records under
    bool run_everything{false}; // the full battery on both compilers + matrix + report (C++ orchestrated)
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

    // Preset gates run BOTH compilers, gcc first then clang, sequentially — each in its own build
    // dir. Compiler-steering (--clang/--gcc15/--compiler) is IGNORED for presets (always both);
    // --release/--modules still steer the functional tree.
    std::string func_extra;
    if (configure_opts.build_type == "Release") func_extra += " --release";
    if (configure_opts.modules) func_extra += " --modules";
    struct PresetCc { const char* flag; const char* label; };
    const PresetCc preset_ccs[] = {{"--gcc15", "gcc15"}, {"--clang", "clang20"}};

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
        for (const auto& cc : preset_ccs) {
            const std::string dir = std::string("build-") + cc.label;
            emit("# functional gate — " + dir + " (Debug, built once then run)");
            emit("\"$CLI\" configure --build-dir " + dir + " " + cc.flag + func_extra);
            emit("\"$CLI\" build --build-dir " + dir);
            if (preset.ctest) emit("\"$CLI\" run --build-dir " + dir);         // ctest unit suite
            if (preset.smoke) emit("\"$CLI\" matrix run --build-dir " + dir);  // persist x output smoke grid
        }
        emit("");
    }

    if (preset.verify_lite) {
        emit("# memory/thread gate — valgrind memcheck (ctest + smoke surface), both compilers");
        for (const auto& cc : preset_ccs) emit(std::string("\"$CLI\" matrix verify-lite ") + cc.flag);
        emit("");
    }
    if (preset.verify) {
        emit("# memory/thread gate — valgrind memcheck + helgrind + DRD, both compilers");
        for (const auto& cc : preset_ccs) emit(std::string("\"$CLI\" matrix verify ") + cc.flag);
        emit("");
    }

    if (preset.bench) {
        emit("# throughput benchmark — Release, store_bench target, both compilers");
        for (const auto& cc : preset_ccs) {
            const std::string dir = std::string("build-bench-") + cc.label;
            emit("\"$CLI\" configure --release --build-dir " + dir + " " + cc.flag);
            emit("cmake --build " + dir + " --target jac313_store_bench");
            emit("BENCH=\"" + dir + "/Store/tests/matrix/jac313_store_bench\"");
            if (preset.report) {
                emit("# record numbers to results.db (rendered once after both compilers, below)");
                emit("\"$BENCH\" --suite --db test-summary/results.db --jtext-ver v002.002");
            } else {
                emit("\"$BENCH\" --suite");                      // numbers to stdout only
            }
        }
        if (preset.report) emit("\"$CLI\" --report");            // render ONCE after both compilers
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
// --group-id precheck (read-only): list the machine groups already in results.db, then resolve and
// show which group_id / jac313-<id> label THIS machine would record under (matched on cpu+cores+ram+os,
// hostname never used) and whether that REUSES an existing group or creates a new one. Writes nothing.
int run_group_id_command(const GlobalOptions& global) {
    const fs::path db_path = global.source_dir / "test-summary" / "results.db";
    try {
        jac313::Qlite::v002::Sqlite db(db_path.string());
        jac313::results::ensure_schema(db);
        const auto hw = collect_host_hardware_record(detect_disk_type("."));
        const jac313::results::HostId h{hw.cpu_model, static_cast<std::int64_t>(hw.cpu_cores),
                                        static_cast<std::int64_t>(hw.ram_gb), hw.os_pretty, hw.disk_type_label, static_cast<std::int64_t>(hw.p_cores), static_cast<std::int64_t>(hw.cpu_mhz_min), static_cast<std::int64_t>(hw.cpu_mhz_max)};
        std::cout << "=== group-id precheck (read-only — test-summary/results.db) ===\n\n"
                     "Existing machine groups (from host_spec):\n"
                     "  gid | host       | hardware                          | os\n"
                     "  ----+------------+-----------------------------------+----------------------\n";
        std::int64_t db_max = 0; bool any = false;
        { auto st = db.prepare("SELECT group_id, cpu, cores, ram_gb, os FROM host_spec ORDER BY group_id");
          while (st.step()) {
              std::int64_t gid = 0, cores = 0, ram = 0; std::string cpu, os;
              st.get(gid, cpu, cores, ram, os);
              const std::string host = jac313::results::host_label(gid);
              if (gid > db_max) db_max = gid; any = true;
              const std::string hwd = cpu + " (" + std::to_string(cores) + "c/" + std::to_string(ram) + "G)";
              char line[480];
              std::snprintf(line, sizeof line, "  %3lld | %-10.10s | %-33.33s | %-.21s\n",
                            static_cast<long long>(gid), host.c_str(), hwd.c_str(), os.c_str());
              std::cout << line;
          } }
        if (!any) std::cout << "  (none yet)\n";
        const std::int64_t proposed = jac313::results::group_id(db, h);   // read-only resolve (SELECT, or MAX+1)
        const std::string label = jac313::results::host_label(proposed);
        std::cout << "\nThis machine:\n  cpu:   " << h.cpu << "\n  cores: " << h.cores
                  << "    ram_gb: " << h.ram_gb << "\n  os:    " << h.os
                  << "\n  recorded as: " << label << "   (matched on cpu+cores+ram+os; hostname not used)\n"
                  << "\nProposed group_id: " << proposed << "   (" << label << ")\n"
                  << (proposed <= db_max
                        ? "  -> REUSES an existing group: recording ADDS rows under " + label + ".\n"
                        : "  -> NEW group: recording creates a fresh group; nothing existing is touched.\n");
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "group-id precheck failed: " << e.what() << "\n"; return 1;
    }
}

// ---- compiler pin (Setup/compilers.pin): committed, per-machine {gcc,clang} toolchain labels so the
// choice is DETERMINISTIC — no "highest changes when a newer compiler lands" drift. Keyed on the
// hardware identity (the group_id tuple), labeled jac313-###. bootstrap does find-or-create; you
// change a row by editing it or `pin --gcc <label> --clang <label>`. No pin -> highest-available. ----
struct CompilerPinRow {
    std::string label, cpu, os, gcc_label, clang_label;
    int cores = 0, ram_gb = 0;
};
fs::path compiler_pin_path(const fs::path& source_dir) { return source_dir / "Setup" / "compilers.pin"; }

std::vector<CompilerPinRow> read_compiler_pins(const fs::path& source_dir) {
    std::vector<CompilerPinRow> rows;
    std::ifstream in(compiler_pin_path(source_dir));
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::vector<std::string> f; std::string cur;
        for (char c : line) { if (c == '\t') { f.push_back(cur); cur.clear(); } else cur += c; }
        f.push_back(cur);
        if (f.size() < 7) continue;
        CompilerPinRow r; r.label = f[0]; r.cpu = f[1];
        try { r.cores = std::stoi(f[2]); r.ram_gb = std::stoi(f[3]); } catch (...) {}
        r.os = f[4]; r.gcc_label = f[5]; r.clang_label = f[6];
        rows.push_back(r);
    }
    return rows;
}

const CompilerPinRow* find_pin_for_host(const std::vector<CompilerPinRow>& rows, const HostHardwareRecord& hw) {
    for (const auto& r : rows)
        if (r.cpu == hw.cpu_model && r.cores == hw.cpu_cores && r.ram_gb == hw.ram_gb && r.os == hw.os_pretty)
            return &r;
    return nullptr;
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
    const HostHardwareRecord hw = collect_host_hardware_record(detect_disk_type("."));
    const auto rows = read_compiler_pins(src);
    const CompilerPinRow* row = find_pin_for_host(rows, hw);
    std::string label = row ? (want_gcc ? row->gcc_label : row->clang_label)
                            : highest_toolchain_label(probe_compilers(src), want_gcc ? "gcc" : "clang");
    if (label.empty()) return {};
    request.explicit_compiler = label;
    try { return resolve_compiler(request).cc_path; } catch (...) { return {}; }
}

void write_compiler_pins(const fs::path& source_dir, const std::vector<CompilerPinRow>& rows) {
    std::ofstream out(compiler_pin_path(source_dir));
    out << "# Committed per-machine compiler pin. One row per machine, keyed on the hardware identity\n"
           "# (cpu / cores / ram_gb / os), labeled jac313-###. bootstrap find-or-creates a row; the\n"
           "# chosen gcc/clang are then DETERMINISTIC across runs. Change by editing a row, or:\n"
           "#   jac313_test_cli pin --gcc <label> --clang <label>   (labels are from Setup/compilers.conf)\n"
           "# TAB-separated: label  cpu_model  cores  ram_gb  os_pretty  gcc_label  clang_label\n";
    for (const auto& r : rows)
        out << r.label << '\t' << r.cpu << '\t' << r.cores << '\t' << r.ram_gb << '\t'
            << r.os << '\t' << r.gcc_label << '\t' << r.clang_label << '\n';
}

// {gcc_label, clang_label} for this machine: the committed pin if present, else highest-available.
std::pair<std::string, std::string> pinned_or_highest_labels(const fs::path& source_dir) {
    const HostHardwareRecord hw = collect_host_hardware_record(detect_disk_type("."));
    const auto rows = read_compiler_pins(source_dir);
    if (const CompilerPinRow* row = find_pin_for_host(rows, hw))
        return {row->gcc_label, row->clang_label};
    const auto probes = probe_compilers(source_dir);
    return {highest_toolchain_label(probes, "gcc"), highest_toolchain_label(probes, "clang")};
}

// --run-everything: the full battery, orchestrated IN CODE (per-step exit codes, continue-on-error,
// a failure summary). Re-invokes this CLI's own subcommands per step. Uses THIS machine's PINNED
// gcc/clang (Setup/compilers.pin) so the run is deterministic. Run from v002/.
int run_everything_command(const GlobalOptions& global) {
    namespace fs = std::filesystem;
    std::error_code ec;
    if (!global.source_dir.empty()) fs::current_path(global.source_dir, ec);
    const std::string CLI = "./jac313_test_cli";
    const auto [gcc_label, clang_label] = pinned_or_highest_labels(".");
    if (gcc_label.empty() || clang_label.empty()) {
        std::cerr << "run-everything: could not resolve this machine's pinned/highest compilers "
                     "(run `jac313_test_cli pin --ensure`)\n";
        return 1;
    }
    std::cout << "######## run-everything: full battery — gcc=" << gcc_label
              << "  clang=" << clang_label << "  (pinned) ########\n";
    fs::remove("test-summary/results.db", ec);   // fresh dataset (regenerable)

    std::vector<std::string> failures;
    auto step = [&](const std::string& label, const std::string& cmd) {
        std::cout << "\n===== " << label << " =====\n" << std::flush;
        const int raw = std::system(cmd.c_str());
        const int rc = WIFEXITED(raw) ? WEXITSTATUS(raw) : 1;
        std::cout << "[exit " << rc << "] " << label << "\n";
        if (rc != 0) failures.push_back(label + " (exit " + std::to_string(rc) + ")");
    };

    for (const std::string& cc : {gcc_label, clang_label}) {
        const std::string dir = "build-" + cc;
        step("configure " + cc, CLI + " configure --build-dir " + dir + " --compiler " + cc);
        step("build " + cc,     CLI + " build --build-dir " + dir);
        step("ctest " + cc,     CLI + " run --build-dir " + dir);
        step("smoke " + cc,     CLI + " matrix run --build-dir " + dir);
    }
    for (const std::string& cc : {gcc_label, clang_label}) {
        const std::string dir = "build-bench-" + cc;
        step("configure bench " + cc, CLI + " configure --release --build-dir " + dir + " --compiler " + cc);
        step("build bench " + cc,     "cmake --build " + dir + " --target jac313_store_bench");
        step("bench " + cc, dir + "/Store/tests/matrix/jac313_store_bench --suite "
                            "--db test-summary/results.db --jtext-ver v002.002");
    }
    step("verify gcc",   CLI + " matrix verify --compiler " + gcc_label);
    step("verify clang", CLI + " matrix verify --compiler " + clang_label);
    step("build-times", CLI + " build-times");
    step("report",       CLI + " --report");

    std::cout << "\n######## run-everything DONE — open test-summary/README.md ########\n";
    if (failures.empty()) { std::cout << "ALL GREEN.\n"; return 0; }
    std::cout << failures.size() << " step(s) had a non-zero exit (results recorded for what ran):\n";
    for (const auto& f : failures) std::cout << "  - " << f << "\n";
    return 1;
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
    if (command == "pin") {   // committed per-machine compiler pin (Setup/compilers.pin); find-or-create
        fs::path src = "."; bool ensure = false; std::string set_gcc, set_clang;
        for (int i = 2; i < argc; ++i) {
            const std::string a = argv[i];
            if (a == "--ensure") ensure = true;
            else if (a == "--gcc" && i + 1 < argc) set_gcc = argv[++i];
            else if (a == "--clang" && i + 1 < argc) set_clang = argv[++i];
            else if (a == "--source-dir" && i + 1 < argc) src = argv[++i];
        }
        const HostHardwareRecord hw = collect_host_hardware_record(detect_disk_type("."));
        auto rows = read_compiler_pins(src);
        const CompilerPinRow* existing = find_pin_for_host(rows, hw);
        if (!ensure && set_gcc.empty() && set_clang.empty()) {   // default: show this machine's pin
            if (existing) std::cout << "pinned " << existing->label << ": gcc=" << existing->gcc_label
                                    << " clang=" << existing->clang_label << "  (" << hw.cpu_model << ")\n";
            else std::cout << "no pin for this machine (" << hw.cpu_model
                           << ") — run: jac313_test_cli pin --ensure\n";
            return 0;
        }
        if (ensure && existing && set_gcc.empty() && set_clang.empty()) {   // find-or-create: never overwrite
            std::cout << "pin exists " << existing->label << " (gcc=" << existing->gcc_label
                      << " clang=" << existing->clang_label << ") — unchanged\n";
            return 0;
        }
        const auto probes = probe_compilers(src);
        const std::string gcc_label = !set_gcc.empty() ? set_gcc
            : (existing ? existing->gcc_label : highest_toolchain_label(probes, "gcc"));
        const std::string clang_label = !set_clang.empty() ? set_clang
            : (existing ? existing->clang_label : highest_toolchain_label(probes, "clang"));
        if (gcc_label.empty() && clang_label.empty()) {
            std::cerr << "pin: no gcc or clang toolchain available to pin (see Setup/compilers.conf)\n"; return 1;
        }
        std::string row_label = existing ? existing->label : std::string{};
        if (row_label.empty()) {
            int maxn = 0;
            for (const auto& r : rows)
                if (r.label.rfind("jac313-", 0) == 0)
                    try { maxn = std::max(maxn, std::stoi(r.label.substr(7))); } catch (...) {}
            char buf[24]; std::snprintf(buf, sizeof buf, "jac313-%03d", maxn + 1); row_label = buf;
        }
        bool updated = false;
        for (auto& r : rows)
            if (r.cpu == hw.cpu_model && r.cores == hw.cpu_cores && r.ram_gb == hw.ram_gb && r.os == hw.os_pretty) {
                r.gcc_label = gcc_label; r.clang_label = clang_label; updated = true; break;
            }
        if (!updated)
            rows.push_back({row_label, hw.cpu_model, hw.os_pretty, gcc_label, clang_label, hw.cpu_cores, hw.ram_gb});
        write_compiler_pins(src, rows);
        std::cout << (updated ? "updated " : "pinned ") << row_label << ": gcc=" << gcc_label
                  << " clang=" << clang_label << "  (" << hw.cpu_model << ")\n";
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
    if (command == "host") {   // sense + pin THIS machine (host_spec + current_host) and show the grid
        fs::path src = ".";
        for (int i = 2; i < argc; ++i) { const std::string a = argv[i]; if (a == "--source-dir" && i + 1 < argc) src = argv[++i]; }
        const fs::path db_path = src / "test-summary" / "results.db";
        std::error_code ec;
        if (!fs::exists(db_path.parent_path(), ec)) { std::cerr << "host: no test-summary/.\n"; return 1; }
        jac313::Qlite::v002::Sqlite db(db_path.string());
        jac313::results::ensure_schema(db);
        const auto hw = collect_host_hardware_record(detect_disk_type("."));
        const jac313::results::HostId h{hw.cpu_model, static_cast<std::int64_t>(hw.cpu_cores),
            static_cast<std::int64_t>(hw.ram_gb), hw.os_pretty, hw.disk_type_label,
            static_cast<std::int64_t>(hw.p_cores), static_cast<std::int64_t>(hw.cpu_mhz_min),
            static_cast<std::int64_t>(hw.cpu_mhz_max)};
        const std::int64_t gid = jac313::results::pin_host(db, h);
        char speed[40]; std::snprintf(speed, sizeof speed, "%.1f-%.1f GHz",
            static_cast<double>(h.cpu_mhz_min) / 1000.0, static_cast<double>(h.cpu_mhz_max) / 1000.0);
        std::cout << "=== host — " << jac313::results::host_label(gid) << " (pinned as current_host) ===\n"
                  << "  CPU      " << h.cpu << "\n"
                  << "  Speed    " << (h.cpu_mhz_max > 0 ? speed : "-") << "\n"
                  << "  P.Cores  " << h.p_cores << "      T.Cores  " << h.cores << "\n"
                  << "  RAM      " << h.ram_gb << " GB\n"
                  << "  Disk     " << dash(h.disk) << "\n"
                  << "  OS       " << h.os << "\n"
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
            } else if (arg == "--group-id") {
                preset.group_id = true;
            } else if (arg == "--run-everything") {
                preset.run_everything = true;
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
                "A matrix run is a correctness gate only. Throughput is the `--bench --report` gate,\n"
                "and `jac313_test_cli --report` renders every result from test-summary/results.db.\n\n"
                "matrix options:\n"
                "  --params <file>   Params file (default: tests/test_params.txt)\n"
                "  --dry-run         List scenarios only\n"
                "  --modules         Configure+build with modules before run\n"
                "  --filter <regex>  Match test name, persist, or name|persist|mode\n"
                "  --failsafe <sec>  Kill stuck scenario after N seconds (smoke: "
                + std::to_string(kSmokeFailsafeSec) + ", full: " + std::to_string(kFullFailsafeSec)
                + ", 0=off)\n"
                "  --clang           Force the clang toolchain (registry-resolved) for results path / build dir\n"
                "  --compiler <cxx>  Explicit compiler (skips auto-detect; any compilers.conf label)\n";
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
            return run_group_id_command(global);
        }
        if (preset.run_everything) {
            return run_everything_command(global);
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
        return build_and_capture(global, build_opts);
    }
    print_usage();
    return 1;
}