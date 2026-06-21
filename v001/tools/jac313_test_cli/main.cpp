// jac313_test_cli — local test runner for the jac313 monorepo.
// Discovers CTest entries from a build tree, runs them with captured output,
// and prints a summary. No shell scripts required.

#include "build_benchmark.hpp"
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
#include "results_db.hpp"
#include "run_identity.hpp"
#include "host_hardware.hpp"
#include "runner.hpp"
#include "test_summary_render.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using namespace jac313::test_cli;

namespace {

ResultsDbContext make_results_db_context(const fs::path& project_root)
{
    ResultsDbContext ctx;
    ctx.project_root = fs::absolute(project_root);
    return ctx;
}

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
        "  anonymize-hosts  scrub identifying hostnames in the results DB -> jac313-NNN\n"
        "  matrix      run | runner | run-all | render | top | runs | verify | verify-lite\n\n"
        "Common options:\n"
        "  --build-dir <path>   Build directory (default: build)\n"
        "  --source-dir <path>  Source root (default: auto-detect)\n"
        "  -v, --verbose        Verbose output\n"
        "  -h, --help           Show help\n\n"
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
        "  ./jac313_test_cli compilers\n"
        "  ./jac313_test_cli run --filter store\n"
        "  ./jac313_test_cli list --build-dir build\n"
        "  ./jac313_test_cli matrix run-all --dry-run\n"
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

int run_tests_command(const GlobalOptions& global, const RunOptions& opts,
                      const std::optional<fs::path>& report_path)
{
    auto tests = discover_tests(global.build_dir);
    if (tests.empty()) {
        std::cerr << "No tests found under " << global.build_dir << '\n';
        std::cerr << "Hint: jac313_test_cli configure && jac313_test_cli build\n";
        return 1;
    }

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
    namespace setup = jac313::Setup::v001;

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

int run_matrix_run_command(const GlobalOptions& global,
                           MatrixOptions matrix_opts,
                           const RunOptions& run_opts,
                           const int build_jobs = 0)
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
    std::cout << "Scenarios: " << format_count(scenarios.size()) << "\n\n";

    const ResultsDbContext db{global.source_dir};
    MatrixRunMeta meta = matrix_run_meta_for(params, matrix_opts.compiler_label);
    meta.build_type = build_type;
    meta.modules = modules_label;

    if (!matrix_opts.dry_run) {
        prepare_matrix_run_session(meta, matrix_opts, global.source_dir);
        std::cout << "Group ID: " << format_count(meta.group_id) << '\n';
    }

    if (matrix_opts.dry_run) {
        const int total = static_cast<int>(scenarios.size());
        int index = 0;
        for (const auto& scen : scenarios) {
            std::cout << "  ";
            print_matrix_scenario_line(scen, ++index, total);
        }
        return 0;
    }

    if (!build_matrix_targets_before_run(global, scenarios, build_jobs, failsafe_sec)) {
        return 1;
    }
    std::cout << '\n';

    MatrixRunOptions run_opts_matrix;
    run_opts_matrix.verbose = global.verbose;
    run_opts_matrix.fail_fast = run_opts.fail_fast;
    run_opts_matrix.failsafe_sec = failsafe_sec;
    run_opts_matrix.db.db = db;
    std::optional<std::int64_t> run_id;
    if (!matrix_opts.dry_run) {
        run_id = ensure_matrix_run_record(db, meta, results_base);
        if (run_id.has_value()) {
            run_opts_matrix.db.run_id = *run_id;
        }
    }

    const auto results = run_matrix(scenarios, params, results_base, run_opts_matrix);
    accumulate_matrix_results(meta, results);

    std::cout << "\n=== matrix summary ===\n";
    std::cout << "Passed:  " << format_count(meta.passed) << '\n';
    std::cout << "Failed:  " << format_count(meta.failed) << '\n';
    std::cout << "Skipped: " << format_count(meta.skipped) << '\n';
    std::cout << "Errors:  " << format_count(meta.errors) << '\n';
    if (run_id.has_value()) {
        finalize_matrix_run(db, *run_id, meta);
        if (!matrix_opts.no_summary) {
            if (!render_test_summary(db, *run_id)) {
                return 1;
            }
        }
    }

    return (meta.failed > 0 || meta.errors > 0) ? 1 : 0;
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

    auto run_one = [&](const std::string& tool, const VgTask& t) {
        if (!fs::exists(t.bin)) {
            std::cout << "  [" << tool << "] " << t.label << " — MISSING BINARY\n";
            ++testfail;
            failures.push_back(tool + " " + t.label + " [missing]");
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
        const auto r = run_process(cmd, 300, scratch_s);
        const int errs = parse_vg_errors(r.stderr_text + r.stdout_text);
        std::string verdict;
        if (!r.started) {
            verdict = "LAUNCH-FAIL";
            ++testfail;
            failures.push_back(tool + " " + t.label + " [launch]");
        } else if (r.exit_code == 99) {
            verdict = "VALGRIND ERRORS (" + std::to_string(errs) + ")";
            ++vgfail;
            failures.push_back(tool + " " + t.label + " [vg:" + std::to_string(errs) + "]");
        } else if (r.exit_code != 0) {
            verdict = "TEST FAIL (rc=" + std::to_string(r.exit_code) + ")";
            ++testfail;
            failures.push_back(tool + " " + t.label + " [rc:" + std::to_string(r.exit_code) + "]");
        } else {
            verdict = "clean";
            ++clean;
        }
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
    return ok ? 0 : 1;
}

// ---- version-check: enforce a version() bump when a package's shipped code changes ----
// Rule: for each package, if its shipped source changed since the commit that last SET its
// version() literal, a bump is owed. Range-independent (compares HEAD to that commit) and
// git-only (no build), so it runs fast in the pre-push hook ahead of the slow cleancheck.
int run_version_check_command(const GlobalOptions& global) {
    struct Pkg { std::string name; std::string version_file; std::vector<std::string> source; };
    const std::vector<Pkg> pkgs = {
        {"Qlite", "Qlite/include/jac313/Qlite/v001/Sqlite.body.hpp", {"Qlite/include"}},
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
                       MatrixOptions& matrix_opts, RunOptions& run_opts, BuildOptions& build_opts)
{
    if (!apply_compiler_resolution(global, configure_opts, matrix_opts)) {
        return 1;
    }
    if (configure_opts.modules) {
        const MatrixParams params =
            load_matrix_params(global.source_dir / matrix_opts.params_file);
        const ResultsDbContext db{global.source_dir};
        if (!matrix_opts.group_id.has_value()) {
            MatrixRunMeta group_meta;
            group_meta.run_utc = matrix_run_utc_timestamp();
            group_meta.disk_type = params.disk_type;
            group_meta.os_id = params.os_id;
            matrix_opts.group_run_utc = group_meta.run_utc;
            matrix_opts.group_id = allocate_test_group(db, group_meta);
        }
        BuildBenchmarkRequest benchmark;
        benchmark.global = global;
        benchmark.configure = configure_opts;
        benchmark.build = build_opts;
        benchmark.db = db;
        benchmark.group_id = *matrix_opts.group_id;
        benchmark.compiler_label = matrix_opts.compiler_label;
        if (const int rc = run_build_benchmark(benchmark); rc != 0) {
            return rc;
        }
    } else if (discover_tests(global.build_dir).empty()) {
        std::cout << "[matrix] '" << global.build_dir.string()
                  << "' has no built tests — configuring + building first\n";
        if (const int rc = run_configure_command(global, configure_opts); rc != 0) {
            return rc;
        }
        if (const int rc = run_build_command(global, build_opts); rc != 0) {
            return rc;
        }
    }
    return run_matrix_run_command(global, matrix_opts, run_opts, build_opts.jobs);
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
            } else if (arg == "--report" && i + 1 < argc) {
                report_path = argv[++i];
            } else if (arg == "--params" && i + 1 < argc) {
                matrix_opts.params_file = argv[++i];
            } else if (arg == "--dry-run") {
                matrix_opts.dry_run = true;
            } else if (arg == "--no-summary") {
                matrix_opts.no_summary = true;
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
                "  run-all        Run every MISSING combo for this host (gap-filling, idempotent)\n"
                "  verify-lite    valgrind gate: ctest under memcheck + helgrind/DRD\n"
                "  verify         valgrind gate: ctest + smoke matrix (full sink coverage)\n"
                "  render         Render test-summary from results DB (no matrix run)\n"
                "  top            Query top N peak ops/sec scenarios (v_top_throughput)\n"
                "  runs           List latest run per compiler/size/disk (v_latest_runs)\n\n"
                "matrix options:\n"
                "  --params <file>   Params file (default: tests/test_params.txt)\n"
                "  --dry-run         List scenarios only\n"
                "  --no-summary      Skip auto test-summary render after run\n"
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

        if (sub == "top") {
            int limit = 10;
            for (int i = 3; i < argc; ++i) {
                const std::string arg = argv[i];
                if (arg == "--source-dir" && i + 1 < argc) {
                    global.source_dir = argv[++i];
                } else if (arg == "--limit" && i + 1 < argc) {
                    limit = std::stoi(argv[++i]);
                } else if (arg == "--help" || arg == "-h") {
                    std::cout << "matrix top — query v_top_throughput\n\n"
                                 "  --limit <n>       Row count (default: 10)\n"
                                 "  --source-dir <p>  Project root\n";
                    return 0;
                } else {
                    std::cerr << "Unknown matrix top option: " << arg << '\n';
                    return 1;
                }
            }
            return print_top_fastest(make_results_db_context(global.source_dir), limit) ? 0 : 1;
        }

        if (sub == "runs") {
            for (int i = 3; i < argc; ++i) {
                const std::string arg = argv[i];
                if (arg == "--source-dir" && i + 1 < argc) {
                    global.source_dir = argv[++i];
                } else if (arg == "--help" || arg == "-h") {
                    std::cout << "matrix runs — query v_latest_runs\n\n"
                                 "  --source-dir <p>  Project root\n";
                    return 0;
                } else {
                    std::cerr << "Unknown matrix runs option: " << arg << '\n';
                    return 1;
                }
            }
            return print_latest_runs(make_results_db_context(global.source_dir)) ? 0 : 1;
        }

        if (sub == "render") {
            std::optional<std::int64_t> run_id;
            bool render_all = false;
            SummaryRenderStep through = SummaryRenderStep::All;
            for (int i = 3; i < argc; ++i) {
                const std::string arg = argv[i];
                if (arg == "--source-dir" && i + 1 < argc) {
                    global.source_dir = argv[++i];
                } else if (arg == "--run-id" && i + 1 < argc) {
                    run_id = std::stoll(argv[++i]);
                } else if (arg == "--all") {
                    render_all = true;
                } else if (arg == "--step" && i + 1 < argc) {
                    const std::string step = argv[++i];
                    if (step == "run") {
                        through = SummaryRenderStep::RunPage;
                    } else if (step == "hub") {
                        through = SummaryRenderStep::Hub;
                    } else if (step == "all") {
                        through = SummaryRenderStep::All;
                    } else {
                        std::cerr << "Unknown render step: " << step
                                  << " (run|hub|all)\n";
                        return 1;
                    }
                } else if (arg == "--help" || arg == "-h") {
                    std::cout << "matrix render — generate test-summary from results DB\n\n"
                                 "  --source-dir <p>  Project or fixture root\n"
                                 "  --run-id <n>      Run id (default: latest)\n"
                                 "  --all             Re-render every run's page + hub\n"
                                 "  --step <name>     Stop after step: run, hub, all\n";
                    return 0;
                } else {
                    std::cerr << "Unknown matrix render option: " << arg << '\n';
                    return 1;
                }
            }

            const ResultsDbContext db = make_results_db_context(global.source_dir);
            if (render_all) {
                // Re-render every current run's page, then the hub once. Useful after a
                // rendering/format change to refresh all committed pages from the DB.
                std::vector<ResultsHubRun> runs;
                load_hub_runs_from_db(db, runs);
                for (const auto& run : runs) {
                    write_run_results_page(db, run.run_id);
                }
                return write_summary_hub(db, true) ? 0 : 1;
            }
            if (!run_id.has_value()) {
                run_id = latest_run_id(db);
            }
            if (!run_id.has_value()) {
                std::cerr << "No runs in " << db.db_path().string() << '\n';
                return 1;
            }
            return render_test_summary_through(db, *run_id, through) ? 0 : 1;
        }

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
        if (sub == "run-all") {
            // Diff the desired matrix against runs (this host+disk) and run the gap,
            // composing the same execute_matrix_run core as `runner`. Idempotent.
            bool force = false, include_full = true, dry_run = false;
            for (int i = 3; i < argc; ++i) {
                const std::string a = argv[i];
                if (a == "--force") {
                    force = true;
                } else if (a == "--no-full") {
                    include_full = false;
                } else if (a == "--dry-run") {
                    dry_run = true;
                } else if (a == "--source-dir" && i + 1 < argc) {
                    global.source_dir = argv[++i];
                } else if (a == "-v" || a == "--verbose") {
                    global.verbose = true;
                } else if (a == "--help" || a == "-h") {
                    std::cout <<
                        "matrix run-all — run every MISSING combo for this host (gap-filling, idempotent)\n\n"
                        "  Diffs v_desired_matrix {compiler}x{build_type}x{modules}x{size} against runs\n"
                        "  (this host+disk) and runs the difference via the same core as `runner`.\n\n"
                        "  --force        Run all combos, not just the missing ones\n"
                        "  --no-full      Smoke only (skip xFull)\n"
                        "  --dry-run      List the combos that would run, then stop\n";
                    return 0;
                } else {
                    std::cerr << "Unknown matrix run-all option: " << a << '\n';
                    return 2;
                }
            }
            const std::string os = sensed_os_key();
            std::string disk = detect_disk_type(".");
            if (disk.empty()) {
                disk = "ssd";
            }
            const ResultsDbContext db{global.source_dir};
            const auto combos = missing_matrix_combos(db, os, disk, include_full, force);
            std::cout << "run-all: " << combos.size()
                      << (force ? " combo(s) [forced]" : " missing combo(s)")
                      << " for " << os << "/" << disk
                      << (include_full ? "" : " [smoke only]") << ":\n";
            for (const auto& c : combos) {
                std::cout << "  " << c.compiler << ' ' << c.build_type << ' '
                          << c.modules << ' ' << c.size_label << '\n';
            }
            if (combos.empty()) {
                std::cout << "Nothing to do — the matrix is complete for this host.\n";
                return 0;
            }
            if (dry_run) {
                std::cout << "(dry-run: nothing executed)\n";
                return 0;
            }
            int failures = 0, idx = 0;
            for (const auto& c : combos) {
                ++idx;
                std::cout << "\n=== run-all [" << idx << '/' << combos.size() << "]  "
                          << c.compiler << ' ' << c.build_type << ' ' << c.modules
                          << ' ' << c.size_label << " ===\n";
                ConfigureOptions co;
                MatrixOptions mo;
                if (c.compiler == "gcc15") {
                    co.prefer_gcc15 = true;
                    mo.compiler_label = "gcc15";
                } else {
                    co.prefer_clang = true;
                    mo.compiler_label = "clang";
                }
                co.build_type = c.build_type;
                co.modules = (c.modules == "modules");
                mo.params_file = (c.size_label == "xFull")
                                     ? "tests/test_params_full.txt"
                                     : "tests/test_params.txt";
                GlobalOptions go = global;
                const std::string bt = (c.build_type == "Release") ? "release" : "debug";
                const std::string md = co.modules ? "modules" : "textual";
                go.build_dir = "build-" + c.compiler + "-" + bt + "-" + md;
                if (execute_matrix_run(go, co, mo, run_opts, build_opts) != 0) {
                    ++failures;
                    std::cerr << "run-all: FAILED combo " << c.compiler << ' ' << c.build_type
                              << ' ' << c.modules << ' ' << c.size_label << '\n';
                }
            }
            std::cout << "\nrun-all done: " << (static_cast<int>(combos.size()) - failures)
                      << '/' << combos.size() << " combo(s) ok\n";
            return failures == 0 ? 0 : 1;
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

    if (command == "anonymize-hosts") {
        bool dry_run = false;
        for (int i = argi; i < argc; ++i) {
            const std::string a = argv[i];
            if (a == "--dry-run") {
                dry_run = true;
            } else if (a == "--source-dir" && i + 1 < argc) {
                global.source_dir = argv[++i];
            } else if (a == "--help" || a == "-h") {
                std::cout <<
                    "anonymize-hosts — replace identifying hostnames in the results DB with jac313-NNN\n\n"
                    "  Ranks distinct hosts by core count, maps each to jac313-NNN, and scrubs the\n"
                    "  hostname from host_hardware + group_specs. Idempotent. Re-render afterwards.\n\n"
                    "  --dry-run         Show the mapping without writing\n"
                    "  --source-dir <p>  Project root\n";
                return 0;
            } else {
                std::cerr << "Unknown anonymize-hosts option: " << a << '\n';
                return 2;
            }
        }
        return anonymize_hosts(make_results_db_context(global.source_dir), dry_run) ? 0 : 1;
    }

    parse_common(argi);

    if (command == "list") {
        return run_list_command(global);
    }
    if (command == "run") {
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