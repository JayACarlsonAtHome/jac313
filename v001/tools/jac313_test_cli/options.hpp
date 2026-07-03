#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "jac313_harness_version.hpp"

namespace jac313::test_cli {

struct GlobalOptions {
    std::filesystem::path build_dir{"build"};
    std::filesystem::path source_dir;
    bool verbose{false};
    std::string host_claim;           // --claim jac313-###
    std::int64_t host_assign_new{0};  // --assign-new-### or --assign-new N
};

inline std::string harness_version(const GlobalOptions& global) {
    // Subcommands invoked from run_latest_config.sh often pass only --build-dir; fall back to cwd.
    if (global.source_dir.empty()) {
        return jac313::harness_version_from_cwd();
    }
    return jac313::harness_version_from_source_dir(global.source_dir);
}

struct ConfigureOptions {
    std::optional<std::string> compiler;  // set by --compiler; empty = auto-detect
    bool prefer_gcc15{false};
    bool prefer_clang{false};
    bool modules{false};
    bool import_std{false};  // modules built against `import std;` (-DJAC313_QLITE_IMPORT_STD=ON)
    bool store_tests{true};
    bool helgrind_annotate{false};  // -DJAC313_STORE_HELGRIND_ANNOTATE=ON (cleancheck tree)
    std::string generator{"Ninja"};
    std::string build_type{"Debug"};      // CMAKE_BUILD_TYPE (Debug default; --release)
    std::string resolved_compiler;        // absolute compiler path -> CMAKE_CXX_COMPILER
    std::string compiler_kind;            // "gcc" | "clang" (drives module scan-deps)
    std::vector<std::string> scan_deps_candidates; // clang-scan-deps names to try
};

struct BuildOptions {
    int jobs{0};  // 0 = ninja default
};

inline constexpr int kSmokeFailsafeSec = 30;
inline constexpr int kFullFailsafeSec = 900;
inline constexpr int kDefaultFailsafeSec = kFullFailsafeSec;

struct RunOptions {
    std::string filter;
    bool fail_fast{false};
    bool list_only{false};
    bool include_missing{false};
    std::optional<int> failsafe_sec; // unset = auto (smoke 30s, full 900s); 0 = off
};

inline int effective_failsafe_sec(const RunOptions& opts, const std::string& matrix_size = {})
{
    if (opts.failsafe_sec.has_value()) {
        return *opts.failsafe_sec;
    }
    if (!matrix_size.empty()) {
        if (matrix_size == "full" || matrix_size == "FULL"
            || matrix_size == "bench" || matrix_size == "BENCH") {
            return kFullFailsafeSec;
        }
        return kSmokeFailsafeSec;
    }
    return kDefaultFailsafeSec;
}

struct MatrixOptions {
    std::filesystem::path params_file{"tests/test_params.txt"};
    std::string compiler_label{"gcc15"};
    bool dry_run{false};
    bool no_summary{false};
    std::optional<std::int64_t> group_id;
    std::optional<std::string> group_run_utc;
    std::optional<std::string> run_utc;
    // run-all sweep position (1-based). combo_count>0 enables the global
    // "G of TOTAL" scenario counter (TOTAL = combo_count * scenarios-per-combo).
    int combo_index{0};
    int combo_count{0};
};

// Per-combo result tally, reported back from a matrix run so run-all can
// aggregate a pass/fail verdict across the whole sweep.
struct MatrixComboTally {
    int scenarios = 0;   // scenarios this combo would run (set even on build failure)
    int passed    = 0;
    int failed    = 0;
    int skipped   = 0;
    int errors    = 0;
};

// Heuristic fallbacks for test timeouts when the value is not (yet) in the results.db
// testControl table. The DB is the runtime store for these control parameter values.
inline int get_test_timeout(const std::string& test_name, const std::string& size = {}) {
    if (test_name.find("binary_to_jtext") != std::string::npos ||
        test_name.find("binary_persist") != std::string::npos ||
        test_name.find("smoke") != std::string::npos) {
        return 30;
    }
    if (size == "full" || size == "bench") return 900;
    return 30;
}

} // namespace jac313::test_cli