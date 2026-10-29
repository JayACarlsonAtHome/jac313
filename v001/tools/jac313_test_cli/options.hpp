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
    bool keep_logs{false};                       // retain durable capture files (bench + smoke)
    std::filesystem::path keep_logs_dir;       // optional root; bench uses <dir>/bench/<compiler>
    bool wipe_logs{false};                       // delete test-results/ + kept-logs/ (keep README.md)
    std::string host_claim;           // --claim jac313-###
    std::int64_t host_assign_new{0};  // --assign-new-### or --assign-new N

    // Persistence / read-back controls (forwarded to the matrix tests and store_bench):
    std::string persist_list;            // --persist a,b,c: matrix backends (empty = default set)
    std::optional<std::size_t> max_persist_rows;  // --max-persist-rows N (0 = no limit)
    std::size_t max_runs{0};             // --max-runs N: cap each test's runs (0 = test default)
    std::size_t max_records{0};          // --max-records N: cap events per run (threads x events/thread)
    std::string readback;                // --readback off|count|sample|all (empty = test default)
    std::string pg_cleanup;              // --pg-cleanup keep-last|delete (empty = keep-last)
    std::filesystem::path summary_json;  // --summary-json PATH: JSON summary for Jac313Runner
    std::string size_override;           // --size smoke|full (overrides tests/test_params.txt)
    std::string pg_remote;               // --pg-remote "host=.. dbname=.. user=..": sets $JAC313_PG_REMOTE
    std::string pg_local;                // --pg-local  "...": sets $JAC313_PG_LOCAL (default localhost)
    std::filesystem::path connections;   // --connections FILE (jText; see connections.hpp)
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
    bool prefer_gcc{false};
    bool prefer_clang{false};
    bool modules{false};
    bool store_tests{true};
    bool helgrind_annotate{false};  // -DJAC313_STORE_HELGRIND_ANNOTATE=ON (cleancheck tree)
    bool strict{false};    // --strict:   -DJAC313_STRICT=ON (extra warnings; ctest/smoke builds)
    bool hardened{false};  // --hardened: -DJAC313_HARDENED=ON (runtime hardening; ctest/smoke builds)
    std::string contracts{"enforce"};  // --contracts: -DJAC313_CONTRACTS=<mode> (ctest/smoke builds)
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