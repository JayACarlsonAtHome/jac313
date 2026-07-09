#pragma once

#include "matrix.hpp"

#include <filesystem>
#include <string>
#include <string_view>

namespace jac313::test_cli {

inline std::filesystem::path default_kept_logs_root(const std::filesystem::path& version_root) {
    return version_root / "kept-logs";
}

inline std::filesystem::path resolve_kept_logs_root(const std::filesystem::path& version_root,
                                                    const std::filesystem::path& override_root)
{
    return override_root.empty() ? default_kept_logs_root(version_root) : override_root;
}

// Matrix test directory name under test-results/ or kept-logs/smoke/.
inline std::string store_test_subdir_name(const std::string& test_name) {
    if (test_name == "jac313_store_flags") return "JAC313_STORE_TEST_flags";
    if (test_name == "jac313_store_metric_view") return "JAC313_STORE_TEST_metric_view";
    constexpr std::string_view prefix = "jac313_store_";
    if (test_name.starts_with(prefix)) {
        return "JAC313_STORE_TEST_" + test_name.substr(prefix.size());
    }
    return test_name;
}

inline std::string bench_option_label(const std::string& persist, const std::string& suite_label) {
    const bool scale_10m = suite_label.find("@10M") != std::string::npos;
    return persist + "_" + (scale_10m ? "10M" : "1M");
}

inline std::string smoke_option_label(const MatrixScenario& scen) {
    return scen.persist + "_output_" + scen.output_mode;
}

inline std::filesystem::path smoke_kept_capture_base(const std::filesystem::path& kept_root,
                                                     const MatrixScenario& scen)
{
    return kept_root / "smoke" / scen.compiler / store_test_subdir_name(scen.entry.name)
           / smoke_option_label(scen) / "capture";
}

inline std::filesystem::path smoke_kept_scenario_log(const std::filesystem::path& kept_root,
                                                     const MatrixScenario& scen)
{
    return smoke_kept_capture_base(kept_root, scen).parent_path() / "scenario.log";
}

// CLI / preset argument suffixes (--keep-logs is opt-in; default runs omit these).
std::string keep_logs_cli_suffix(const GlobalOptions& global);
std::string bench_keep_logs_args(const GlobalOptions& global, const std::string& compiler);

void print_keep_logs_status(const GlobalOptions& global);

} // namespace jac313::test_cli