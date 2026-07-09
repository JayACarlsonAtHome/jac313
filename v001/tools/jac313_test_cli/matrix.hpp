#pragma once

#include "manifest.hpp"
#include "options.hpp"
#include "params.hpp"
#include "runner.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace jac313::test_cli {

struct MatrixScenario {
    TestEntry entry;
    std::string package;
    std::string category;
    std::string compiler;
    std::string persist;
    std::string output_mode;
    std::size_t threads{0};
    std::size_t events_per_thread{0};
    std::size_t runs{0};
};

struct MatrixRunOptions {
    bool verbose{false};
    bool fail_fast{false};
    int failsafe_sec{kDefaultFailsafeSec};
    bool keep_logs{false};
    std::filesystem::path version_root;
    std::filesystem::path keep_logs_root;
    // Global scenario progress across a whole run-all sweep (0 total = don't show).
    // Lets each line read "[i/116 · G of TOTAL]" so the expected grand total is
    // always visible and an inflated combo count is impossible to miss.
    int global_done_before{0};
    int global_total{0};
};

struct MatrixRunResult {
    MatrixScenario scenario;
    TestResult result;
    std::filesystem::path log_path;
    std::optional<std::uint64_t> persist_log_bytes;
};

void print_matrix_scenario_line(const MatrixScenario& scen,
                                int index,
                                int total,
                                bool in_progress = false,
                                int global_index = 0,
                                int global_total = 0,
                                int name_width = 0);

std::string infer_package(const std::string& test_name);
std::string infer_category(const std::string& test_name);
std::filesystem::path compute_results_base(const std::filesystem::path& project_root,
                                           const MatrixParams& params,
                                           const std::string& compiler,
                                           const std::string& build_type = "Debug",
                                           const std::string& modules = "modules");
struct BuildFeatures {
    bool jtext_persist{false};
    bool sql_persist{false};
    bool modules{false};              // JAC313_BUILD_MODULES from the tree's CMakeCache
    std::string build_type{"Debug"};  // CMAKE_BUILD_TYPE from the tree's CMakeCache
};

BuildFeatures read_build_features(const std::filesystem::path& build_dir);
bool scenario_matches_filter(const MatrixScenario& scen, const std::string& filter);
std::vector<MatrixScenario> build_matrix_scenarios(const std::vector<TestEntry>& tests,
                                                   const MatrixParams& params,
                                                   const std::string& compiler,
                                                   const std::filesystem::path& build_dir,
                                                   const std::string& scenario_filter = "");
bool build_matrix_targets_before_run(const GlobalOptions& global,
                                     const std::vector<MatrixScenario>& scenarios,
                                     int build_jobs,
                                     int failsafe_sec);
std::vector<MatrixRunResult> run_matrix(const std::vector<MatrixScenario>& scenarios,
                                        const MatrixParams& params,
                                        const std::filesystem::path& results_base,
                                        const MatrixRunOptions& opts = {});

struct MatrixTally {
    std::size_t passed{0};
    std::size_t failed{0};
    std::size_t skipped{0};
    std::size_t errors{0};
};

MatrixTally tally_matrix_results(const std::vector<MatrixRunResult>& results);

void write_test_result_log(const std::filesystem::path& log_path,
                           const TestEntry& entry,
                           const TestResult& result,
                           const std::vector<std::string>& args = {});

} // namespace jac313::test_cli