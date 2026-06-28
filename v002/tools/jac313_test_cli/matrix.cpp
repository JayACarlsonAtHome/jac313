#include "matrix.hpp"
#include "build_pipeline.hpp"
#include "format.hpp"
#include "run_identity.hpp"
#include "runner.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

namespace jac313::test_cli {
namespace fs = std::filesystem;

namespace {

std::string persist_log_dir_name(const std::string& persist) {
    if (persist == "jtext") return "jText_logs";
    if (persist == "sql")   return "sql_logs";
    if (persist == "none")  return "inmem_logs";
    if (persist == "unit")  return "unit_logs";
    if (persist == "flags") return "flags_logs";
    return "binary_logs";
}

std::string test_to_subdir_name(const std::string& test_name) {
    if (test_name == "jac313_store_flags") {
        return "JAC313_STORE_TEST_flags";
    }
    constexpr std::string_view prefix = "jac313_store_";
    if (test_name.starts_with(prefix)) {
        return "JAC313_STORE_TEST_" + test_name.substr(prefix.size());
    }
    return test_name;
}

std::string matrix_test_key(const std::string& test_name) {
    if (test_name == "jac313_store_flags") {
        return "flags";
    }
    if (test_name == "jac313_store_metric_view") {
        return "metric_view";
    }
    constexpr std::string_view prefix = "jac313_store_";
    if (test_name.starts_with(prefix)) {
        const std::string_view rest = std::string_view(test_name).substr(prefix.size());
        if (rest.size() >= 3) {
            return std::string(rest.substr(0, 3));
        }
    }
    return {};
}

void apply_scaling(MatrixScenario& scen, const MatrixParams& params) {
    const std::string key = matrix_test_key(scen.entry.name);
    if (key.empty()) {
        return;
    }
    const MatrixScaling scaling = get_matrix_scaling(key, params.size);
    scen.threads = scaling.threads;
    scen.events_per_thread = scaling.events_per_thread;
    scen.runs = scaling.runs;
}

MatrixScenario make_scenario(const TestEntry& entry,
                               const std::string& package,
                               const std::string& category,
                               const std::string& compiler,
                               const std::string& persist,
                               const std::string& output_mode)
{
    MatrixScenario scen;
    scen.entry = entry;
    scen.package = package;
    scen.category = category;
    scen.compiler = compiler;
    scen.persist = persist;
    scen.output_mode = output_mode;
    return scen;
}

bool cmake_cache_bool(const std::string& line, const char* key, bool& value) {
    const std::string prefix = std::string(key) + ":BOOL=";
    if (!line.starts_with(prefix)) {
        return false;
    }
    const std::string v = line.substr(prefix.size());
    value = (v == "ON" || v == "1" || v == "TRUE");
    return true;
}

void expand_store_matrix_scenarios(std::vector<MatrixScenario>& out,
                                   const TestEntry& entry,
                                   const std::string& package,
                                   const std::string& compiler,
                                   const MatrixParams& params,
                                   const BuildFeatures& features)
{
    const std::string key = matrix_test_key(entry.name);

    if (key == "flags") {
        auto scen = make_scenario(entry, package, "matrix", compiler, "unit", "off");
        apply_scaling(scen, params);
        out.push_back(std::move(scen));
        return;
    }

    if (key == "metric_view") {
        // single scenario like flags: read-side summary demo at --test-size scale
        auto scen = make_scenario(entry, package, "matrix", compiler, "unit", "off");
        apply_scaling(scen, params);
        out.push_back(std::move(scen));
        return;
    }

    if (key == "008") {
        auto scen = make_scenario(entry, package, "matrix", compiler, "flags", "off");
        apply_scaling(scen, params);
        out.push_back(std::move(scen));
        return;
    }

    static const std::vector<std::string> persist_types = {"binary", "jtext", "sql", "none"};
    static const std::vector<std::string> output_modes  = {"on", "off"};

    for (const auto& persist : persist_types) {
        if (persist == "jtext" && !features.jtext_persist) {
            continue;
        }
        if (persist == "sql" && !features.sql_persist) {
            continue;
        }
        for (const auto& mode : output_modes) {
            auto scen = make_scenario(entry, package, "matrix", compiler, persist, mode);
            apply_scaling(scen, params);
            out.push_back(std::move(scen));
        }
    }
}

fs::path scenario_log_dir(const fs::path& results_base, const MatrixScenario& scen) {
    if (scen.category != "matrix") {
        return results_base / "logs";
    }
    return results_base
        / persist_log_dir_name(scen.persist)
        / test_to_subdir_name(scen.entry.name);
}

fs::path scenario_log_path(const fs::path& results_base, const MatrixScenario& scen) {
    if (scen.category != "matrix") {
        return scenario_log_dir(results_base, scen) / (scen.entry.name + ".log");
    }
    const fs::path log_dir = scenario_log_dir(results_base, scen);
    const std::string file = scen.compiler + "_" + scen.persist + "_" + scen.output_mode + ".log";
    return log_dir / file;
}

fs::path scenario_persist_base(const fs::path& log_dir) {
    return log_dir / "persist";
}

std::vector<std::string> build_scenario_args(const MatrixScenario& scen,
                                             const MatrixParams& params,
                                             const fs::path& persist_base)
{
    std::vector<std::string> args;
    args.emplace_back("--no-interactive");
    args.push_back(std::string("--color=") + (scen.output_mode == "on" ? "1" : "0"));

    if (!params.size.empty()) {
        args.push_back(std::string("--test-size=") + params.size);
    }

    if (scen.category == "matrix") {
        if (scen.entry.name == "jac313_store_flags" || scen.entry.name == "jac313_store_metric_view") {
            return args;
        }

        const std::string persist_arg =
            (scen.persist == "flags") ? "none" : scen.persist;
        args.push_back(std::string("--persist=") + persist_arg);
        args.push_back(std::string("--base-name=") + persist_base.string());

        if (scen.threads > 0) {
            args.push_back(std::string("--threads=") + std::to_string(scen.threads));
        }
        if (scen.events_per_thread > 0) {
            args.push_back(std::string("--events-per-thread=")
                           + std::to_string(scen.events_per_thread));
        }
        if (scen.runs > 0) {
            args.push_back(std::string("--runs=") + std::to_string(scen.runs));
        }
    } else {
        args.emplace_back("--persist=none");
    }

    return args;
}

void write_log_file(const fs::path& log_path,
                    const MatrixScenario& scen,
                    const std::vector<std::string>& args,
                    const TestResult& result)
{
    std::ofstream log(log_path);
    if (!log) {
        return;
    }
    log << "# jac313 matrix log\n";
    log << "test=" << scen.entry.name << '\n';
    log << "package=" << scen.package << '\n';
    log << "category=" << scen.category << '\n';
    log << "persist=" << scen.persist << '\n';
    log << "output_mode=" << scen.output_mode << '\n';
    log << "command=" << scen.entry.command.string() << '\n';
    log << "args=";
    for (std::size_t i = 0; i < args.size(); ++i) {
        if (i > 0) log << ' ';
        log << args[i];
    }
    log << '\n';
    log << "duration_ms=" << result.duration.count() << '\n';
    log << "exit_code=" << result.exit_code << '\n';
    if (!result.message.empty()) {
        log << "message=" << result.message << '\n';
    }
    log << "\n--- stdout ---\n" << result.stdout_tail;
    log << "\n--- stderr ---\n" << result.stderr_tail;
}

} // namespace

BuildFeatures read_build_features(const fs::path& build_dir) {
    BuildFeatures features;
    std::ifstream cache(build_dir / "CMakeCache.txt");
    if (!cache) {
        return features;
    }

    std::string line;
    while (std::getline(cache, line)) {
        cmake_cache_bool(line, "JAC313_STORE_JTEXT_PERSIST", features.jtext_persist);
        cmake_cache_bool(line, "JAC313_STORE_SQL_PERSIST", features.sql_persist);
        cmake_cache_bool(line, "JAC313_BUILD_MODULES", features.modules);
        if (line.rfind("CMAKE_BUILD_TYPE:", 0) == 0) {
            if (const auto eq = line.find('='); eq != std::string::npos) {
                std::string v = line.substr(eq + 1);
                if (!v.empty()) {
                    features.build_type = v;
                }
            }
        }
    }
    return features;
}

bool scenario_matches_filter(const MatrixScenario& scen, const std::string& filter) {
    if (filter.empty()) {
        return true;
    }
    const std::string key = scen.entry.name + '|' + scen.persist + '|' + scen.output_mode;
    const std::string persist_mode = scen.persist + '|' + scen.output_mode;
    return matches_filter(scen.entry.name, filter)
        || matches_filter(scen.persist, filter)
        || matches_filter(scen.output_mode, filter)
        || matches_filter(persist_mode, filter)
        || matches_filter(key, filter);
}

void print_matrix_scenario_line(const MatrixScenario& scen,
                                const int index,
                                const int total,
                                const bool in_progress,
                                const int global_index,
                                const int global_total,
                                const int name_width)
{
    // Right-justify the numeric counters to the width of their maximum, and
    // left-justify the variable text columns to fixed widths, so NOTHING shifts:
    //   name    -> widest scenario name in the run (name_width)
    //   persist -> width of "binary" (6, the longest sink)
    //   mode    -> width of "off"    (3, the longest mode)
    const auto rjust = [](const std::string& s, std::size_t w) {
        return s.size() < w ? std::string(w - s.size(), ' ') + s : s;
    };
    const auto ljust = [](const std::string& s, std::size_t w) {
        return s.size() < w ? s + std::string(w - s.size(), ' ') : s;
    };
    const std::string total_s = format_count(total);
    std::cout << "[" << rjust(format_count(index), total_s.size()) << "/" << total_s;
    if (global_total > 0) {
        const std::string gtot_s = format_count(global_total);
        std::cout << " · " << rjust(format_count(global_index), gtot_s.size())
                  << " of " << gtot_s;
    }
    std::cout << "] " << ljust(scen.entry.name, static_cast<std::size_t>(name_width));
    if (scen.category == "matrix") {
        std::cout << " | " << ljust(scen.persist, 6) << " | " << ljust(scen.output_mode, 3);
        // workload (threads/events/runs) is constant per run — shown once in the matrix summary, not here.
    } else {
        std::cout << " (" << scen.package << '/' << scen.category << ')';
    }
    if (in_progress) {
        std::cout << " ... ";
        std::cout.flush();
    } else {
        std::cout << '\n';
    }
}

std::string infer_package(const std::string& test_name) {
    if (test_name.find("jac313_qlite_") == 0) return "qlite";
    if (test_name.find("jac313_jtext_") == 0) return "jtext";
    if (test_name.find("jac313_store_") == 0) return "store";
    return "unknown";
}

std::string infer_category(const std::string& test_name) {
    if (test_name == "jac313_store_flags") return "matrix";
    if (test_name == "jac313_store_metric_view") return "matrix";
    if (test_name.find("jac313_store_00") == 0) return "matrix";
    if (test_name.find("_module_test") != std::string::npos) return "module";
    if (test_name.find("_smoke") != std::string::npos) return "smoke";
    return "unit";
}

fs::path compute_results_base(const fs::path& project_root,
                              const MatrixParams& params,
                              const std::string& compiler,
                              const std::string& build_type,
                              const std::string& modules)
{
    // Derived from the single run-identity definition, so the path can't drift from the
    // DB keys: test-results/<os>/<compiler>/<build_type>/<disk>/<size_label>/<modules>.
    const RunIdentity id{sensed_os_key(), compiler, build_type,
                         params.disk_type, size_label(params.size), modules};
    return identity_path(project_root, id);
}

std::vector<MatrixScenario> build_matrix_scenarios(const std::vector<TestEntry>& tests,
                                                   const MatrixParams& params,
                                                   const std::string& compiler,
                                                   const fs::path& build_dir,
                                                   const std::string& scenario_filter)
{
    std::vector<MatrixScenario> scenarios;
    const BuildFeatures features = read_build_features(build_dir);

    for (const auto& test : tests) {
        const std::string package = infer_package(test.name);
        if (!test_selected(params, test.name, package)) {
            continue;
        }

        const std::string category = infer_category(test.name);
        if (category == "matrix") {
            expand_store_matrix_scenarios(scenarios, test, package, compiler, params, features);
        } else {
            scenarios.push_back(make_scenario(test, package, category, compiler, "none", "off"));
        }
    }

    if (!scenario_filter.empty()) {
        std::vector<MatrixScenario> filtered;
        filtered.reserve(scenarios.size());
        for (auto& scen : scenarios) {
            if (scenario_matches_filter(scen, scenario_filter)) {
                filtered.push_back(std::move(scen));
            }
        }
        return filtered;
    }

    return scenarios;
}

bool build_matrix_targets_before_run(const GlobalOptions& global,
                                     const std::vector<MatrixScenario>& scenarios,
                                     const int build_jobs,
                                     const int failsafe_sec)
{
    if (!fs::exists(global.build_dir / "build.ninja")) {
        std::cerr << "Build dir not configured: " << global.build_dir
                  << " (run configure + build first)\n";
        return false;
    }

    std::vector<std::string> targets;
    targets.reserve(scenarios.size());
    for (const auto& scen : scenarios) {
        targets.push_back(scen.entry.name);
    }
    std::sort(targets.begin(), targets.end());
    targets.erase(std::unique(targets.begin(), targets.end()), targets.end());

    std::cout << "=== pre-matrix build (" << format_count(targets.size())
              << " targets) ===\n";
    return timed_build_targets(global.build_dir, targets, build_jobs, false, failsafe_sec) >= 0;
}

std::vector<MatrixRunResult> run_matrix(const std::vector<MatrixScenario>& scenarios,
                                        const MatrixParams& params,
                                        const fs::path& results_base,
                                        const MatrixRunOptions& opts)
{
    std::vector<MatrixRunResult> results;
    results.reserve(scenarios.size());

    const int total = static_cast<int>(scenarios.size());
    int name_width = 0;   // widest scenario name in this run (for column alignment)
    for (const auto& s : scenarios) {
        if (static_cast<int>(s.entry.name.size()) > name_width) {
            name_width = static_cast<int>(s.entry.name.size());
        }
    }
    int index = 0;

    for (const auto& scen : scenarios) {
        ++index;

        const fs::path log_dir = scenario_log_dir(results_base, scen);
        fs::create_directories(log_dir);
        const fs::path log_path = scenario_log_path(results_base, scen);
        const fs::path persist_base = scenario_persist_base(log_dir);
        const auto args = build_scenario_args(scen, params, persist_base);

        print_matrix_scenario_line(scen, index, total, true,
                                   opts.global_done_before + index, opts.global_total, name_width);

        MatrixRunResult run;
        run.scenario = scen;
        run.log_path = log_path;

        run.result = run_test_with_args(scen.entry.command, args, opts.verbose, opts.failsafe_sec);
        run.result.entry = scen.entry;

        switch (run.result.status) {
        case TestStatus::Passed:
            std::cout << "PASS (" << format_count_padded(run.result.duration.count()) << " ms)\n";
            break;
        case TestStatus::Skipped:
            std::cout << "SKIP";
            if (!run.result.message.empty()) {
                std::cout << " — " << run.result.message;
            }
            std::cout << '\n';
            break;
        case TestStatus::Failed:
            std::cout << "FAIL — " << run.result.message
                      << " (" << format_count_padded(run.result.duration.count()) << " ms)\n";
            if (run.result.message.find("failsafe timeout") != std::string::npos) {
                std::cout << "  (killed — exceeded failsafe " << format_count(opts.failsafe_sec)
                          << "s)\n";
            }
            break;
        case TestStatus::Error:
            std::cout << "ERROR — " << run.result.message << '\n';
            break;
        }

        write_log_file(log_path, scen, args, run.result);

        const TestStatus final_status = run.result.status;
        results.push_back(std::move(run));

        if (opts.fail_fast && final_status != TestStatus::Passed
            && final_status != TestStatus::Skipped) {
            break;
        }
    }

    return results;
}

MatrixTally tally_matrix_results(const std::vector<MatrixRunResult>& results) {
    MatrixTally tally;
    for (const auto& r : results) {
        switch (r.result.status) {
        case TestStatus::Passed:  ++tally.passed;  break;
        case TestStatus::Failed:  ++tally.failed;  break;
        case TestStatus::Skipped: ++tally.skipped; break;
        case TestStatus::Error:   ++tally.errors;  break;
        }
    }
    return tally;
}

} // namespace jac313::test_cli