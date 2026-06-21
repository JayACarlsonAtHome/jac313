#pragma once

#include "matrix_manifest.hpp"

#include <cstdint>
#include <filesystem>
#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

namespace jac313::test_cli {

struct ResultsDbContext {
    std::filesystem::path project_root;

    [[nodiscard]] std::filesystem::path db_path() const;
    [[nodiscard]] std::string db_href_from(const std::filesystem::path& from_dir) const;
};

std::optional<std::int64_t> latest_run_id(const ResultsDbContext& ctx);

std::optional<std::filesystem::path> results_base_for_run(const ResultsDbContext& ctx,
                                                          std::int64_t run_id);

struct MatrixDbSession {
    ResultsDbContext db;
    std::int64_t run_id{0};

    [[nodiscard]] bool active() const { return run_id > 0; }
};

struct ResultsHubRun {
    std::int64_t run_id{0};
    std::int64_t group_id{0};
    std::string page_rel;
    std::string os;
    std::string compiler;
    std::string build_type;
    std::string size_label;
    std::string disk_type;
    std::string modules;
    std::string run_utc;
    int duration_ms{0};
    std::string hardware_specs;
    std::string os_specs;
    int passed{0};
    int failed{0};
    std::optional<std::int64_t> peak_ops_sec;
    std::string peak_scenario;
    std::optional<int> build_without_modules_ms;
    std::optional<int> build_modules_only_ms;
    std::optional<int> build_with_modules_ms;
};

struct GroupCompilerBuildRecord {
    std::int64_t group_id{0};
    std::string compiler;
    std::filesystem::path build_dir;
    std::filesystem::path build_dir_no_modules;
    int build_without_modules_ms{0};
    int build_modules_only_ms{0};
    int build_with_modules_ms{0};
    // `import std;` variant (gcc + the exact pinned CMake — the gate UUID is version-specific,
    // so >= 3.30 is the floor but not sufficient). Null when the toolchain can't build it yet
    // (e.g. clang has no std module here) — the column stays empty until it can.
    std::optional<int> build_modules_only_importstd_ms;
    std::optional<int> build_with_modules_importstd_ms;
};

struct ScenarioCompileTimes {
    std::optional<int> compile_with_modules_ms;
    std::optional<int> compile_without_modules_ms;
};

std::string summary_tree_rel(const std::string& results_rel);

bool ensure_results_schema(const ResultsDbContext& ctx);

// Package version registry (Qlite/jText/Store): major.minor split from each package's
// version() literal. Source of truth stays in the headers; these rows are a DB projection
// the hub renders from (so the renderer never touches package source).
struct PackageVersionRow {
    std::int64_t group_id{0};
    std::string name;
    std::string major;
    std::string minor;
};
// Record the current package versions (read from the version() literals) for a run group.
// Called at group-allocation time, so the renderer never reads source — it just queries.
void record_package_versions_for_group(const ResultsDbContext& ctx, std::int64_t group_id);
bool load_package_versions(const ResultsDbContext& ctx, std::vector<PackageVersionRow>& out);

// Anonymize identifying hostnames in the results DB -> jac313-NNN (ranked by core count;
// numbering continues after any existing jac313-* so prior labels stay stable). Idempotent.
// Scrubs host_hardware (hostname, storage_model) + group_specs (hardware_specs, os_specs).
// dry_run reports the mapping without writing. Re-render afterwards to refresh pages.
bool anonymize_hosts(const ResultsDbContext& ctx, bool dry_run);

// One cell of the desired test matrix (a v_desired_matrix row).
struct MatrixCombo {
    std::string compiler;
    std::string build_type;
    std::string modules;
    std::string size_label;
};
// Combos in v_desired_matrix not yet present in `runs` for (os, disk) — the gap that
// `matrix run-all` fills. force=true returns every desired combo (not just missing);
// include_full=false restricts to Smoke. Smoke combos are ordered before xFull.
std::vector<MatrixCombo> missing_matrix_combos(const ResultsDbContext& ctx,
                                               const std::string& os,
                                               const std::string& disk,
                                               bool include_full,
                                               bool force);

std::int64_t allocate_test_group(const ResultsDbContext& ctx, const MatrixRunMeta& meta);

bool record_group_compiler_build(const ResultsDbContext& ctx,
                                 const GroupCompilerBuildRecord& record);

std::optional<std::int64_t> ensure_matrix_run_record(const ResultsDbContext& ctx,
                                                     const MatrixRunMeta& meta,
                                                     const std::filesystem::path& results_base);

bool record_scenario_compile(const ResultsDbContext& ctx,
                             std::int64_t run_id,
                             const ScenarioIdentity& scenario,
                             const ScenarioCompileTimes& times);

bool record_scenario_result(const ResultsDbContext& ctx,
                            std::int64_t run_id,
                            const ScenarioIdentity& scenario,
                            const std::string& status,
                            int duration_ms,
                            const std::string& log_rel,
                            std::optional<std::int64_t> peak_ops_sec = std::nullopt,
                            std::optional<std::uint64_t> persist_log_bytes = std::nullopt);

bool publish_scenario_log_tail(const ResultsDbContext& ctx,
                               const std::string& results_rel,
                               const std::string& log_rel,
                               const std::string& stdout_text,
                               int line_count = 50);

bool finalize_matrix_run(const ResultsDbContext& ctx,
                         std::int64_t run_id,
                         const MatrixRunMeta& meta);

bool print_top_fastest(const ResultsDbContext& ctx, int limit);

bool print_latest_runs(const ResultsDbContext& ctx);

bool load_hub_runs_from_db(const ResultsDbContext& ctx, std::vector<ResultsHubRun>& rows);

bool write_run_results_page(const ResultsDbContext& ctx, std::int64_t run_id);

bool write_persist_backend_comparison_hub(std::ostream& out, const ResultsDbContext& ctx);

bool write_throughput_by_log_category(std::ostream& out,
                                      const ResultsDbContext& ctx,
                                      int limit_per_category = 10);

} // namespace jac313::test_cli