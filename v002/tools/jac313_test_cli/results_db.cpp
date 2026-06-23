#include "results_db.hpp"
#include "format.hpp"
#include "host_hardware.hpp"
#include "matrix_manifest.hpp"
#include "run_identity.hpp"
#include "time_util.hpp"

#include <jac313/Qlite/v002.hpp>

#include <algorithm>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace jac313::test_cli {
namespace fs = std::filesystem;

using Sqlite = jac313::Qlite::v002::Sqlite;

namespace {

constexpr int kResultsDbSchemaVersion = 4;  // 4: modules is a first-class identity dimension

void drop_all_results_tables(Sqlite& db) {
    db.exec("DROP VIEW IF EXISTS v_top_throughput");
    db.exec("DROP VIEW IF EXISTS v_desired_matrix");
    db.exec("DROP VIEW IF EXISTS v_latest_runs");
    db.exec("DROP VIEW IF EXISTS v_scenario_throughput");
    db.exec("DROP TABLE IF EXISTS scenarios");
    db.exec("DROP TABLE IF EXISTS runs");
    db.exec("DROP TABLE IF EXISTS group_compiler_builds");
    db.exec("DROP TABLE IF EXISTS host_hardware");
    db.exec("DROP TABLE IF EXISTS group_specs");
    db.exec("DROP TABLE IF EXISTS schema_meta");
    db.exec("DROP TABLE IF EXISTS package_versions");
}

int read_schema_version(Sqlite& db) {
    try {
        auto stmt = db.prepare("SELECT value FROM schema_meta WHERE key='version'");
        if (!stmt.step()) {
            return 0;
        }
        std::string value;
        stmt.get(value);
        return std::stoi(value);
    } catch (const std::exception&) {
        return 0;
    }
}

void write_schema_version(Sqlite& db) {
    db.exec(R"SQL(
CREATE TABLE IF NOT EXISTS schema_meta (
    key TEXT PRIMARY KEY,
    value TEXT NOT NULL
)
)SQL");
    db.exec("INSERT OR REPLACE INTO schema_meta(key, value) VALUES('version', ?)",
            std::to_string(kResultsDbSchemaVersion));
}

// Per-package source of the version() literal ("major.minor"), with display order. Read at
// run time to record a group's versions; the names match what the hub shows.
struct PkgSource { const char* name; const char* file; int ord; };
const PkgSource kPkgSources[] = {
    {"jac313::Qlite::v002", "Qlite/include/jac313/Qlite/v002/Sqlite.body.hpp", 0},
    {"jac313::jText::v002", "jText/jText.api.inc", 1},
    {"jac313::Store::v002", "Store/include/jac313/Store/v002/headers/ts_store_config.hpp", 2},
};

// Extract the quoted "major.minor" from the package's version() definition line.
std::string read_version_literal(const fs::path& file) {
    std::ifstream in(file);
    std::string line;
    while (std::getline(in, line)) {
        const auto sv = line.find("version(");
        if (sv == std::string::npos) continue;
        const auto ret = line.find("return", sv);
        if (ret == std::string::npos) continue;
        const auto q1 = line.find('"', ret);
        const auto q2 = (q1 == std::string::npos) ? std::string::npos : line.find('"', q1 + 1);
        if (q2 == std::string::npos) continue;
        return line.substr(q1 + 1, q2 - q1 - 1);  // e.g. "v002.001"
    }
    return {};
}

// Per-run version table. Migrate a pre-group_id table (regenerable test data) by dropping it.
// New groups self-record at run time; existing groups were backfilled once by a throwaway
// script — no permanent backfill logic lives here (that would be a one-time migration baked
// into the schema forever).
void ensure_package_versions(Sqlite& db) {
    bool has_group_id = false;
    try {
        db.prepare("SELECT group_id FROM package_versions LIMIT 0");
        has_group_id = true;
    } catch (const std::exception&) {
    }
    if (!has_group_id) {
        db.exec("DROP TABLE IF EXISTS package_versions");
    }
    db.exec(R"SQL(
CREATE TABLE IF NOT EXISTS package_versions (
    group_id INTEGER NOT NULL,
    name  TEXT NOT NULL,
    major TEXT NOT NULL,
    minor TEXT NOT NULL,
    ord   INTEGER NOT NULL DEFAULT 0,
    PRIMARY KEY (group_id, name),
    FOREIGN KEY(group_id) REFERENCES group_specs(group_id) ON DELETE CASCADE
)
)SQL");
}

void exec_schema(Sqlite& db) {
    db.exec(R"SQL(
CREATE TABLE IF NOT EXISTS group_specs (
    group_id INTEGER PRIMARY KEY,
    run_utc TEXT NOT NULL,
    hardware_specs TEXT NOT NULL,
    os_specs TEXT NOT NULL,
    created_utc TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS host_hardware (
    group_id INTEGER PRIMARY KEY REFERENCES group_specs(group_id) ON DELETE CASCADE,
    hostname TEXT,
    cpu_model TEXT,
    cpu_cores INTEGER,
    ram_gb INTEGER,
    cpu_mhz INTEGER,
    storage_model TEXT,
    disk_type_label TEXT,
    os_pretty TEXT,
    kernel_release TEXT,
    os_id TEXT
);

CREATE TABLE IF NOT EXISTS group_compiler_builds (
    group_id INTEGER NOT NULL REFERENCES group_specs(group_id) ON DELETE CASCADE,
    compiler TEXT NOT NULL,
    build_dir TEXT NOT NULL,
    build_dir_no_modules TEXT,
    build_without_modules_ms INTEGER NOT NULL,
    build_modules_only_ms INTEGER NOT NULL,
    build_with_modules_ms INTEGER NOT NULL,
    build_modules_only_importstd_ms INTEGER,
    build_with_modules_importstd_ms INTEGER,
    recorded_utc TEXT NOT NULL,
    PRIMARY KEY (group_id, compiler)
);

CREATE TABLE IF NOT EXISTS runs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    group_id INTEGER REFERENCES group_specs(group_id),
    run_utc TEXT NOT NULL,
    os TEXT NOT NULL DEFAULT 'unknown',
    compiler TEXT NOT NULL,
    build_type TEXT NOT NULL DEFAULT 'Debug',
    disk_type TEXT NOT NULL,
    size TEXT NOT NULL,
    size_label TEXT NOT NULL,
    modules TEXT NOT NULL DEFAULT 'modules',
    os_id TEXT,
    host_hardware TEXT,
    passed INTEGER NOT NULL,
    failed INTEGER NOT NULL,
    skipped INTEGER NOT NULL,
    errors INTEGER NOT NULL,
    duration_ms INTEGER NOT NULL,
    results_base TEXT NOT NULL,
    summary_rel TEXT NOT NULL,
    ingested_utc TEXT NOT NULL,
    UNIQUE(os, compiler, build_type, disk_type, size_label, modules, run_utc)
);

CREATE TABLE IF NOT EXISTS scenarios (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    run_id INTEGER NOT NULL REFERENCES runs(id) ON DELETE CASCADE,
    test TEXT NOT NULL,
    package TEXT NOT NULL,
    category TEXT NOT NULL,
    persist TEXT NOT NULL,
    output_mode TEXT NOT NULL,
    status TEXT NOT NULL,
    duration_ms INTEGER NOT NULL,
    log_rel TEXT NOT NULL,
    peak_ops_sec INTEGER,
    compile_with_modules_ms INTEGER,
    compile_without_modules_ms INTEGER,
    persist_log_bytes INTEGER,
    UNIQUE(run_id, test, persist, output_mode)
);

CREATE VIEW IF NOT EXISTS v_desired_matrix AS
WITH compilers(compiler)     AS (VALUES ('gcc15'),('clang')),
     build_types(build_type) AS (VALUES ('Debug'),('Release')),
     modes(modules)          AS (VALUES ('modules'),('textual')),
     sizes(size_label)       AS (VALUES ('Smoke'),('xFull'))
SELECT compiler, build_type, modules, size_label
FROM compilers, build_types, modes, sizes;

-- v_latest_runs is defined in refresh_latest_runs_view() (built from the RunIdentity
-- column list) so the "latest per identity" grouping lives in exactly one place.

CREATE VIEW IF NOT EXISTS v_scenario_throughput AS
SELECT
    s.id AS scenario_id,
    r.id AS run_id,
    r.group_id,
    r.run_utc,
    r.compiler,
    r.size_label,
    r.disk_type,
    r.results_base,
    g.hardware_specs,
    g.os_specs,
    s.test,
    s.persist,
    s.output_mode,
    s.duration_ms,
    s.peak_ops_sec,
    s.log_rel
FROM scenarios s
JOIN runs r ON r.id = s.run_id
LEFT JOIN group_specs g ON g.group_id = r.group_id
WHERE s.status = 'pass' AND s.peak_ops_sec IS NOT NULL;

CREATE VIEW IF NOT EXISTS v_top_throughput AS
SELECT *
FROM v_scenario_throughput
ORDER BY peak_ops_sec DESC;
)SQL");
}

void ensure_runs_host_hardware_column(Sqlite& db) {
    try {
        db.exec("ALTER TABLE runs ADD COLUMN host_hardware TEXT");
    } catch (const std::exception&) {
    }
}

void ensure_runs_group_id_column(Sqlite& db) {
    try {
        db.exec("ALTER TABLE runs ADD COLUMN group_id INTEGER REFERENCES group_specs(group_id)");
    } catch (const std::exception&) {
    }
}

void ensure_group_compiler_builds_columns(Sqlite& db) {
    try {
        db.exec("ALTER TABLE group_compiler_builds ADD COLUMN build_dir_no_modules TEXT");
    } catch (const std::exception&) {
    }
    try {
        db.exec("ALTER TABLE group_compiler_builds ADD COLUMN build_modules_only_ms INTEGER NOT NULL DEFAULT 0");
    } catch (const std::exception&) {
    }
    // `import std;` timings — nullable (NULL = not measured / unsupported toolchain).
    // Additive, backward-compatible columns: no schema-version bump (which would wipe
    // the committed baseline); existing rows simply read NULL.
    try {
        db.exec("ALTER TABLE group_compiler_builds ADD COLUMN build_modules_only_importstd_ms INTEGER");
    } catch (const std::exception&) {
    }
    try {
        db.exec("ALTER TABLE group_compiler_builds ADD COLUMN build_with_modules_importstd_ms INTEGER");
    } catch (const std::exception&) {
    }
}

void ensure_scenarios_compile_columns(Sqlite& db) {
    try {
        db.exec("ALTER TABLE scenarios ADD COLUMN compile_with_modules_ms INTEGER");
    } catch (const std::exception&) {
    }
    try {
        db.exec("ALTER TABLE scenarios ADD COLUMN compile_without_modules_ms INTEGER");
    } catch (const std::exception&) {
    }
}

// Non-destructive migration: add build_type to existing DBs (defaults to Debug).
void ensure_runs_build_type_column(Sqlite& db) {
    try {
        db.exec("ALTER TABLE runs ADD COLUMN build_type TEXT NOT NULL DEFAULT 'Debug'");
    } catch (const std::exception&) {
    }
}

// Recreate v_latest_runs from the RunIdentity column list, so "latest run per identity"
// (os, compiler, build_type, disk, size) is defined in exactly one place.
void refresh_latest_runs_view(Sqlite& db) {
    db.exec("DROP VIEW IF EXISTS v_latest_runs");
    db.exec(
        "CREATE VIEW v_latest_runs AS\n"
        "SELECT r.*\n"
        "FROM runs r\n"
        "INNER JOIN (\n"
        "    SELECT " + identity_group_by() + ", MAX(id) AS max_id\n"
        "    FROM runs\n"
        "    GROUP BY " + identity_group_by() + "\n"
        ") latest\n"
        "    ON " + identity_join_on("r", "latest") + "\n"
        "   AND r.id = latest.max_id\n"
        "ORDER BY " + identity_select_list("r") + ";");
}

void refresh_throughput_views(Sqlite& db) {
    db.exec("DROP VIEW IF EXISTS v_top_throughput");
    db.exec("DROP VIEW IF EXISTS v_scenario_throughput");
    db.exec(R"SQL(
CREATE VIEW v_scenario_throughput AS
SELECT
    s.id AS scenario_id,
    r.id AS run_id,
    r.group_id,
    r.run_utc,
    r.compiler,
    r.size_label,
    r.disk_type,
    r.results_base,
    g.hardware_specs,
    g.os_specs,
    s.test,
    s.persist,
    s.output_mode,
    s.duration_ms,
    s.peak_ops_sec,
    s.log_rel
FROM scenarios s
JOIN runs r ON r.id = s.run_id
LEFT JOIN group_specs g ON g.group_id = r.group_id
WHERE s.status = 'pass' AND s.peak_ops_sec IS NOT NULL;

CREATE VIEW v_top_throughput AS
SELECT *
FROM v_scenario_throughput
ORDER BY peak_ops_sec DESC;
)SQL");
}

std::string persist_mode_cell(const std::string& persist, const std::string& output_mode) {
    if (persist.empty() || persist == "-") {
        return output_mode.empty() || output_mode == "-" ? "-" : output_mode;
    }
    if (output_mode.empty() || output_mode == "-") {
        return persist;
    }
    return persist + "/" + output_mode;
}

std::string hub_log_href(const fs::path& project_root,
                         const std::string& results_base,
                         const std::string& log_rel)
{
    const fs::path hub_dir = project_root / "test-summary";
    const fs::path log_path = project_root / summary_tree_rel(results_base) / log_rel;
    return fs::relative(log_path, hub_dir).generic_string();
}

std::string hub_log_link(const fs::path& project_root,
                         const std::string& results_base,
                         const std::string& log_rel)
{
    if (log_rel.empty() || log_rel == "-") {
        return "-";
    }
    const std::string label = fs::path(log_rel).filename().string();
    return "[" + label + "](" + hub_log_href(project_root, results_base, log_rel) + ")";
}

std::string hub_log_link_html(const fs::path& project_root,
                              const std::string& results_base,
                              const std::string& log_rel)
{
    if (log_rel.empty() || log_rel == "-") {
        return "-";
    }
    const std::string label = fs::path(log_rel).filename().string();
    const std::string href = hub_log_href(project_root, results_base, log_rel);
    return "<a href=\"" + html_escape(href) + "\">" + html_escape(label) + "</a>";
}

const std::vector<std::string>& log_category_order() {
    static const std::vector<std::string> order = {
        "binary_logs",
        "inmem_logs",
        "sql_logs",
        "jText_logs",
        "flags_logs",
    };
    return order;
}

std::optional<std::int64_t> find_run_id(Sqlite& db,
                                        const RunIdentity& id,
                                        const std::string& run_utc)
{
    auto stmt = db.prepare(
        "SELECT id FROM runs WHERE " + identity_where_eq() + " AND run_utc=?");
    stmt.bind(id.os, id.compiler, id.build_type, id.disk_type, id.size_label, id.modules, run_utc);
    if (!stmt.step()) {
        return std::nullopt;
    }
    std::int64_t out = 0;
    stmt.get(out);
    return out;
}

std::string summary_page_rel(const std::string& results_rel) {
    std::string summary = results_rel;
    constexpr std::string_view prefix = "test-results/";
    if (summary.starts_with(prefix)) {
        summary.replace(0, prefix.size(), "test-summary/");
    }
    return (fs::path(summary) / "RUN.md").generic_string();
}

std::string markdown_log_link(const std::string& log_rel, const std::string& compiler) {
    if (log_rel.empty() || log_rel == "-") {
        return "-";
    }
    const std::string normalized = normalize_log_path(log_rel, compiler);
    const std::string label = fs::path(normalized).filename().string();
    return "[" + label + "](" + normalized + ")";
}

struct RunPageData {
    std::int64_t id{0};
    std::int64_t group_id{0};
    std::string run_utc;
    std::string os;
    std::string compiler;
    std::string build_type;
    std::string disk_type;
    std::string size;
    std::string size_label;
    std::string os_id;
    std::string hardware_specs;
    std::string os_specs;
    std::optional<int> build_without_modules_ms;
    std::optional<int> build_modules_only_ms;
    std::optional<int> build_with_modules_ms;
    std::optional<int> build_modules_only_importstd_ms;
    std::optional<int> build_with_modules_importstd_ms;
    int passed{0};
    int failed{0};
    int skipped{0};
    int errors{0};
    int duration_ms{0};
    std::string results_base;
    std::string page_rel;
};

struct ScenarioPageRow {
    std::string test;
    std::string package;
    std::string category;
    std::string persist;
    std::string output_mode;
    std::string status;
    int duration_ms{0};
    std::string log_rel;
    std::optional<std::int64_t> peak_ops_sec;
    std::optional<std::int64_t> persist_log_bytes;
    std::optional<int> compile_with_modules_ms;
    std::optional<int> compile_without_modules_ms;
};

struct PersistCompareKey {
    std::string test;
    std::string output_mode;

    bool operator<(const PersistCompareKey& other) const
    {
        if (test != other.test) {
            return test < other.test;
        }
        return output_mode < other.output_mode;
    }
};

bool is_persist_backend(const std::string& persist)
{
    return persist == "binary" || persist == "jtext" || persist == "sql";
}

int persist_backend_order(const std::string& persist)
{
    if (persist == "binary") {
        return 0;
    }
    if (persist == "jtext") {
        return 1;
    }
    if (persist == "sql") {
        return 2;
    }
    return 99;
}

bool is_benchmark_matrix_test(const std::string& test)
{
    static constexpr const char* kMarkers[] = {"_005_", "_006_", "_007_", "_008_"};
    for (const char* marker : kMarkers) {
        if (test.find(marker) != std::string::npos) {
            return true;
        }
    }
    return false;
}

// test_006 is a tail-reader concurrency/correctness stress test, not a throughput
// benchmark: it measures writer/reader timing, hit/miss counts and verification, and
// deliberately emits no "Fastest run → … ops/sec" line. It is grouped with the
// benchmark matrix tests for the persist comparison, but has no peak ops/sec to report,
// so its throughput cell renders "N/A" rather than the "-" used for missing data.
bool is_correctness_only_matrix_test(const std::string& test)
{
    return test.find("_006_") != std::string::npos;
}

// Throughput cell shared by every results table: a measured value when present, "N/A"
// for the correctness-only test that produces none by design, "-" otherwise (e.g. a
// non-benchmark test, or a benchmark run that failed before printing its ops/sec line).
std::string format_peak_ops_cell(const ScenarioPageRow& row)
{
    if (row.peak_ops_sec.has_value()) {
        return format_count(*row.peak_ops_sec);
    }
    if (is_correctness_only_matrix_test(row.test)) {
        return "N/A";
    }
    return "-";
}

std::string format_persist_log_size_cell(const ScenarioPageRow& row)
{
    if (row.persist_log_bytes.has_value()) {
        return format_bytes(static_cast<std::uint64_t>(*row.persist_log_bytes));
    }
    return "-";
}

void write_persist_backend_comparison_table(
    std::ostream& out,
    const std::vector<ScenarioPageRow>& scenarios,
    const char* heading,
    const std::function<bool(const ScenarioPageRow&)>& include_test)
{
    std::map<PersistCompareKey, std::vector<const ScenarioPageRow*>> grouped;
    for (const auto& row : scenarios) {
        if (row.status != "pass" || !is_persist_backend(row.persist)) {
            continue;
        }
        if (include_test && !include_test(row)) {
            continue;
        }
        grouped[{row.test, row.output_mode}].push_back(&row);
    }
    if (grouped.empty()) {
        return;
    }

    out << heading << "\n\n";
    out << "Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), "
           "and on-disk **persist artifact size** per backend.\n\n";
    out << "_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness "
           "stress test (writer/reader timing + hit-miss verification), not a throughput "
           "benchmark, so it emits no `Fastest run → … ops/sec` line._\n\n";
    out << "| Test | Output | Persist | run ms | Peak ops/sec | Log size |\n";
    out << "|------|--------|---------|--------|--------------|----------|\n";

    for (const auto& [key, rows] : grouped) {
        std::vector<const ScenarioPageRow*> sorted = rows;
        std::sort(sorted.begin(), sorted.end(),
                  [](const ScenarioPageRow* a, const ScenarioPageRow* b) {
                      return persist_backend_order(a->persist) < persist_backend_order(b->persist);
                  });

        bool first = true;
        for (const ScenarioPageRow* prow : sorted) {
            const ScenarioPageRow& row = *prow;
            const std::string peak = format_peak_ops_cell(row);
            out << "| " << (first ? row.test : "")
                << " | " << (first ? row.output_mode : "")
                << " | " << row.persist
                << " | " << format_count(row.duration_ms)
                << " | " << peak
                << " | " << format_persist_log_size_cell(row)
                << " |\n";
            first = false;
        }
    }
    out << '\n';
}

bool load_run_page_data(Sqlite& db, std::int64_t run_id, RunPageData& run) {
    auto stmt = db.prepare(R"SQL(
SELECT r.id, r.group_id, r.run_utc, r.os, r.compiler, r.build_type, r.disk_type, r.size, r.size_label, r.os_id,
       g.hardware_specs, g.os_specs,
       b.build_without_modules_ms, b.build_modules_only_ms, b.build_with_modules_ms,
       b.build_modules_only_importstd_ms, b.build_with_modules_importstd_ms,
       r.passed, r.failed, r.skipped, r.errors, r.duration_ms, r.results_base, r.summary_rel
FROM runs r
LEFT JOIN group_specs g ON g.group_id = r.group_id
LEFT JOIN group_compiler_builds b
    ON b.group_id = r.group_id AND b.compiler = r.compiler
WHERE r.id=?
)SQL");
    stmt.bind(run_id);
    if (!stmt.step()) {
        return false;
    }

    std::int64_t passed = 0;
    std::int64_t failed = 0;
    std::int64_t skipped = 0;
    std::int64_t errors = 0;
    std::int64_t duration_ms = 0;
    std::optional<std::int64_t> build_without_ms;
    std::optional<std::int64_t> build_modules_only_ms;
    std::optional<std::int64_t> build_with_ms;
    std::optional<std::int64_t> build_modules_only_importstd_ms;
    std::optional<std::int64_t> build_with_importstd_ms;
    stmt.get(run.id,
             run.group_id,
             run.run_utc,
             run.os,
             run.compiler,
             run.build_type,
             run.disk_type,
             run.size,
             run.size_label,
             run.os_id,
             run.hardware_specs,
             run.os_specs,
             build_without_ms,
             build_modules_only_ms,
             build_with_ms,
             build_modules_only_importstd_ms,
             build_with_importstd_ms,
             passed,
             failed,
             skipped,
             errors,
             duration_ms,
             run.results_base,
             run.page_rel);
    if (build_without_ms.has_value()) {
        run.build_without_modules_ms = static_cast<int>(*build_without_ms);
    }
    if (build_modules_only_ms.has_value()) {
        run.build_modules_only_ms = static_cast<int>(*build_modules_only_ms);
    }
    if (build_with_ms.has_value()) {
        run.build_with_modules_ms = static_cast<int>(*build_with_ms);
    }
    if (build_modules_only_importstd_ms.has_value()) {
        run.build_modules_only_importstd_ms = static_cast<int>(*build_modules_only_importstd_ms);
    }
    if (build_with_importstd_ms.has_value()) {
        run.build_with_modules_importstd_ms = static_cast<int>(*build_with_importstd_ms);
    }
    run.passed = static_cast<int>(passed);
    run.failed = static_cast<int>(failed);
    run.skipped = static_cast<int>(skipped);
    run.errors = static_cast<int>(errors);
    run.duration_ms = static_cast<int>(duration_ms);
    return true;
}

bool load_run_scenarios(Sqlite& db, std::int64_t run_id, std::vector<ScenarioPageRow>& rows) {
    rows.clear();
    auto stmt = db.prepare(R"SQL(
SELECT test, package, category, persist, output_mode, status, duration_ms, log_rel, peak_ops_sec,
       persist_log_bytes, compile_with_modules_ms, compile_without_modules_ms
FROM scenarios
WHERE run_id=?
ORDER BY test, persist, output_mode
)SQL");
    stmt.bind(run_id);
    while (stmt.step()) {
        ScenarioPageRow row;
        std::int64_t duration_ms = 0;
        std::optional<std::int64_t> peak_ops;
        std::optional<std::int64_t> persist_bytes;
        std::optional<std::int64_t> compile_with_ms;
        std::optional<std::int64_t> compile_without_ms;
        stmt.get(row.test,
                 row.package,
                 row.category,
                 row.persist,
                 row.output_mode,
                 row.status,
                 duration_ms,
                 row.log_rel,
                 peak_ops,
                 persist_bytes,
                 compile_with_ms,
                 compile_without_ms);
        row.duration_ms = static_cast<int>(duration_ms);
        row.peak_ops_sec = peak_ops;
        row.persist_log_bytes = persist_bytes;
        if (compile_with_ms.has_value()) {
            row.compile_with_modules_ms = static_cast<int>(*compile_with_ms);
        }
        if (compile_without_ms.has_value()) {
            row.compile_without_modules_ms = static_cast<int>(*compile_without_ms);
        }
        rows.push_back(std::move(row));
    }
    return !rows.empty();
}

std::string format_peak_scenario_cell(const std::string& test,
                                      const std::string& persist,
                                      const std::string& output_mode)
{
    if (test.empty()) {
        return {};
    }
    const std::string mode = persist_mode_cell(persist, output_mode);
    if (mode.empty() || mode == "-") {
        return test;
    }
    return test + " (" + mode + ")";
}

std::optional<ScenarioPageRow> peak_scenario_from_rows(const std::vector<ScenarioPageRow>& rows) {
    const ScenarioPageRow* best = nullptr;
    for (const auto& row : rows) {
        if (!row.peak_ops_sec.has_value()) {
            continue;
        }
        if (best == nullptr || *row.peak_ops_sec > *best->peak_ops_sec) {
            best = &row;
        }
    }
    if (best == nullptr) {
        return std::nullopt;
    }
    return *best;
}

bool render_run_results_page(std::ostream& out,
                             const RunPageData& run,
                             const std::vector<ScenarioPageRow>& scenarios,
                             const std::string& db_href,
                             const fs::path& project_root)
{
    out << "# jac313 test run\n\n";
    out << "_Rendered from [`jac313_results.db`](" << db_href << ")._\n\n";
    out << "| Field | Value |\n";
    out << "|-------|-------|\n";
    out << "| Run UTC | " << display_run_utc(run.run_utc) << " |\n";
    out << "| OS | " << (run.os.empty() ? "unknown" : run.os) << " |\n";
    out << "| Compiler | " << run.compiler << " |\n";
    out << "| Build type | " << (run.build_type.empty() ? "Debug" : run.build_type) << " |\n";
    out << "| Disk | " << run.disk_type << " |\n";
    out << "| Size | " << run.size << " (" << run.size_label << ") |\n";
    if (run.group_id > 0) {
        out << "| Group ID | " << format_count(run.group_id) << " |\n";
    }
    if (!run.hardware_specs.empty() || !run.os_specs.empty()) {
        out << "| Environment | "
            << format_group_environment_line(run.hardware_specs, run.os_specs) << " |\n";
    } else if (!run.os_id.empty()) {
        out << "| OS ID | " << run.os_id << " |\n";
    }
    out << "| Passed | " << format_count(run.passed) << " |\n";
    out << "| Failed | " << format_count(run.failed) << " |\n";
    out << "| Skipped | " << format_count(run.skipped) << " |\n";
    out << "| Errors | " << format_count(run.errors) << " |\n";
    out << "| Duration | " << format_duration_human(run.duration_ms) << " |\n";
    if (run.build_without_modules_ms.has_value()) {
        out << "| Build straight (no modules) | " << format_count(*run.build_without_modules_ms)
            << " ms (" << format_duration_human(*run.build_without_modules_ms) << ") |\n";
    }
    if (run.build_modules_only_ms.has_value()) {
        out << "| Build modules only | " << format_count(*run.build_modules_only_ms)
            << " ms (" << format_duration_human(*run.build_modules_only_ms) << ") |\n";
    }
    if (run.build_with_modules_ms.has_value()) {
        out << "| Build with modules (rest) | " << format_count(*run.build_with_modules_ms)
            << " ms (" << format_duration_human(*run.build_with_modules_ms) << ") |\n";
    }
    if (run.build_modules_only_importstd_ms.has_value()) {
        out << "| Build modules only (import std) | "
            << format_count(*run.build_modules_only_importstd_ms)
            << " ms (" << format_duration_human(*run.build_modules_only_importstd_ms) << ") |\n";
    }
    if (run.build_with_modules_importstd_ms.has_value()) {
        out << "| Build with modules (import std) | "
            << format_count(*run.build_with_modules_importstd_ms)
            << " ms (" << format_duration_human(*run.build_with_modules_importstd_ms) << ") |\n";
    }
    // The headline comparison: textual-modules vs import-std, same compiler/targets.
    if (run.build_with_modules_ms.has_value() && run.build_with_modules_importstd_ms.has_value()) {
        const int textual = *run.build_with_modules_ms;
        const int istd = *run.build_with_modules_importstd_ms;
        const int delta = textual - istd;  // positive => import std faster
        out << "| import std vs textual modules | " << (delta >= 0 ? "-" : "+")
            << format_count(delta >= 0 ? delta : -delta) << " ms ("
            << (textual > 0 ? (delta * 100 / textual) : 0) << "% "
            << (delta >= 0 ? "faster" : "slower") << ") |\n";
    }

    if (const auto peak = peak_scenario_from_rows(scenarios); peak.has_value()) {
        out << "| Peak ops/sec | " << format_count(*peak->peak_ops_sec) << " |\n";
        out << "| Peak scenario | " << format_peak_scenario_cell(peak->test, peak->persist, peak->output_mode)
            << " — " << markdown_log_link(peak->log_rel, run.compiler) << " |\n";
    }
    out << '\n';

    write_persist_backend_comparison_table(
        out,
        scenarios,
        "## Persist backend comparison (binary vs jText vs SQL)",
        [](const ScenarioPageRow& row) { return is_benchmark_matrix_test(row.test); });
    write_persist_backend_comparison_table(
        out,
        scenarios,
        "## Persist backend comparison (all matrix tests)",
        [](const ScenarioPageRow& row) { return row.category == "matrix"; });

    out << "## Scenarios\n\n";
    out << "_Per-test compile times are stored in the results database (not log files)._ \n\n";
    out << "| Test | Package | Category | Persist | Output | Status | run ms | Peak ops/sec | Log size | Compile ms | Log |\n";
    out << "|------|---------|----------|---------|--------|--------|--------|--------------|----------|------------|-----|\n";
    for (const auto& row : scenarios) {
        const std::string persist = row.persist == "-" ? "-" : row.persist;
        const std::string output_mode = row.output_mode == "-" ? "-" : row.output_mode;
        std::string compile_ms = "-";
        if (row.compile_with_modules_ms.has_value()) {
            compile_ms = format_count(*row.compile_with_modules_ms) + " (modules)";
        } else if (row.compile_without_modules_ms.has_value()) {
            compile_ms = format_count(*row.compile_without_modules_ms) + " (no modules)";
        }
        const std::string peak = format_peak_ops_cell(row);
        const std::string log_size = is_persist_backend(row.persist)
            ? format_persist_log_size_cell(row)
            : "-";
        out << "| " << row.test
            << " | " << row.package
            << " | " << row.category
            << " | " << persist
            << " | " << output_mode
            << " | " << row.status
            << " | " << format_count(row.duration_ms)
            << " | " << peak
            << " | " << log_size
            << " | " << compile_ms
            << " | " << markdown_log_link(row.log_rel, run.compiler) << " |\n";
    }
    return true;
}

bool group_specs_exists(Sqlite& db, std::int64_t group_id) {
    auto stmt = db.prepare("SELECT 1 FROM group_specs WHERE group_id=?");
    stmt.bind(group_id);
    return stmt.step();
}

std::int64_t next_group_id(Sqlite& db) {
    auto stmt = db.prepare("SELECT COALESCE(MAX(group_id), 0) FROM group_specs");
    stmt.step();
    std::int64_t max_id = 0;
    stmt.get(max_id);
    return max_id + 1;
}

void upsert_host_hardware(Sqlite& db, std::int64_t group_id, const MatrixRunMeta& meta) {
    const HostHardwareRecord hw =
        collect_host_hardware_record(meta.disk_type, meta.os_id);
    db.exec(R"SQL(
INSERT OR REPLACE INTO host_hardware (
    group_id, hostname, cpu_model, cpu_cores, ram_gb, cpu_mhz,
    storage_model, disk_type_label, os_pretty, kernel_release, os_id
) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
)SQL",
            group_id,
            hw.hostname.empty() ? std::optional<std::string>{} : hw.hostname,
            hw.cpu_model.empty() ? std::optional<std::string>{} : hw.cpu_model,
            hw.cpu_cores > 0 ? std::optional<int>{hw.cpu_cores} : std::optional<int>{},
            hw.ram_gb > 0 ? std::optional<int>{hw.ram_gb} : std::optional<int>{},
            hw.cpu_mhz > 0 ? std::optional<int>{hw.cpu_mhz} : std::optional<int>{},
            hw.storage_model.empty() ? std::optional<std::string>{} : hw.storage_model,
            hw.disk_type_label.empty() ? std::optional<std::string>{} : hw.disk_type_label,
            hw.os_pretty.empty() ? std::optional<std::string>{} : hw.os_pretty,
            hw.kernel_release.empty() ? std::optional<std::string>{} : hw.kernel_release,
            hw.os_id.empty() ? std::optional<std::string>{} : hw.os_id);
}

std::int64_t ensure_group_specs(Sqlite& db, const MatrixRunMeta& meta) {
    const HostHardwareRecord hw =
        collect_host_hardware_record(meta.disk_type, meta.os_id);
    const std::string hardware = meta.host_hardware.empty()
        ? hw.hardware_specs
        : meta.host_hardware;
    const std::string os = meta.host_hardware.empty() ? hw.os_specs : std::string{};
    const std::string run_utc = meta.run_utc.empty() ? utc_now_iso() : meta.run_utc;
    const std::int64_t group_id =
        meta.group_id > 0 ? meta.group_id : next_group_id(db);

    db.exec(R"SQL(
INSERT OR REPLACE INTO group_specs (
    group_id, run_utc, hardware_specs, os_specs, created_utc
) VALUES (?, ?, ?, ?, ?)
)SQL",
            group_id,
            run_utc,
            hardware,
            os,
            utc_now_iso());
    upsert_host_hardware(db, group_id, meta);

    return group_id;
}

std::optional<std::int64_t> upsert_run_row(Sqlite& db,
                                           const std::int64_t group_id,
                                           const std::string& compiler,
                                           const MatrixRunMeta& meta,
                                           const std::string& results_rel,
                                           const std::string& page_rel,
                                           const std::string& ingested_utc)
{
    const RunIdentity id{
        meta.os.empty() ? std::string("unknown") : meta.os,
        compiler,
        meta.build_type.empty() ? std::string("Debug") : meta.build_type,
        meta.disk_type,
        meta.size_label,
        meta.modules.empty() ? std::string("modules") : meta.modules};
    std::optional<std::int64_t> run_id = find_run_id(db, id, meta.run_utc);

    if (run_id.has_value()) {
        db.exec(R"SQL(
UPDATE runs SET group_id=?, results_base=?, summary_rel=?
WHERE id=?
)SQL",
                group_id,
                results_rel,
                page_rel,
                *run_id);
    } else {
        db.exec(R"SQL(
INSERT INTO runs (
    group_id, run_utc, os, compiler, build_type, disk_type, size, size_label, modules, os_id, host_hardware,
    passed, failed, skipped, errors, duration_ms,
    results_base, summary_rel, ingested_utc
) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 0, 0, 0, 0, 0, ?, ?, ?)
)SQL",
                group_id,
                meta.run_utc,
                id.os,
                id.compiler,
                id.build_type,
                id.disk_type,
                meta.size,
                id.size_label,
                id.modules,
                meta.os_id.empty() ? std::optional<std::string>{} : meta.os_id,
                std::optional<std::string>{},
                results_rel,
                page_rel,
                ingested_utc);
        run_id = find_run_id(db, id, meta.run_utc);
    }

    return run_id;
}

bool ensure_results_schema_path(const fs::path& db_path) {
    try {
        fs::create_directories(db_path.parent_path());
        Sqlite db(db_path.string());
        if (read_schema_version(db) != kResultsDbSchemaVersion) {
            drop_all_results_tables(db);
        }
        exec_schema(db);
        ensure_runs_build_type_column(db);
        // Additive migrations for tables that already exist at the current schema
        // version (exec_schema's CREATE IF NOT EXISTS won't add columns to them).
        // Idempotent (ALTER guarded by try/catch); needed so the import-std timing
        // columns appear on the committed v3 DB without a destructive version bump.
        ensure_group_compiler_builds_columns(db);
        ensure_package_versions(db);
        refresh_latest_runs_view(db);
        write_schema_version(db);
        refresh_throughput_views(db);
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Results DB schema failed: " << ex.what() << '\n';
        return false;
    }
}

} // namespace

std::string summary_tree_rel(const std::string& results_rel) {
    std::string summary = results_rel;
    constexpr std::string_view prefix = "test-results/";
    if (summary.starts_with(prefix)) {
        summary.replace(0, prefix.size(), "test-summary/");
    }
    return summary;
}

std::filesystem::path ResultsDbContext::db_path() const {
    return project_root / "test-summary" / "jac313_results.db";
}

std::string ResultsDbContext::db_href_from(const fs::path& from_dir) const {
    return fs::relative(db_path(), from_dir).generic_string();
}

bool ensure_results_schema(const ResultsDbContext& ctx) {
    return ensure_results_schema_path(ctx.db_path());
}

void record_package_versions_for_group(const ResultsDbContext& ctx, std::int64_t group_id) {
    try {
        Sqlite db(ctx.db_path().string());
        for (const auto& s : kPkgSources) {
            std::string ver = read_version_literal(ctx.project_root / s.file);
            if (ver.empty()) ver = "v002.001";
            const auto dot = ver.find('.');
            const std::string major = (dot == std::string::npos) ? ver : ver.substr(0, dot);
            const std::string minor = (dot == std::string::npos) ? "" : ver.substr(dot + 1);
            db.exec("INSERT OR REPLACE INTO package_versions(group_id, name, major, minor, ord) "
                    "VALUES(?, ?, ?, ?, ?)",
                    group_id, std::string(s.name), major, minor, static_cast<std::int64_t>(s.ord));
        }
    } catch (const std::exception& ex) {
        std::cerr << "record_package_versions_for_group(" << group_id << ") failed: "
                  << ex.what() << '\n';
    }
}

bool load_package_versions(const ResultsDbContext& ctx, std::vector<PackageVersionRow>& out) {
    try {
        Sqlite db(ctx.db_path().string());
        auto stmt = db.prepare(
            "SELECT group_id, name, major, minor FROM package_versions ORDER BY group_id, ord");
        while (stmt.step()) {
            PackageVersionRow row;
            stmt.get(row.group_id, row.name, row.major, row.minor);
            out.push_back(std::move(row));
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool anonymize_hosts(const ResultsDbContext& ctx, bool dry_run) {
    try {
        Sqlite db(ctx.db_path().string());

        struct HostRow { std::string hostname; std::int64_t cores; };
        std::vector<HostRow> hosts;
        {
            auto stmt = db.prepare(
                "SELECT hostname, COALESCE(MAX(cpu_cores),0) FROM host_hardware "
                "WHERE hostname IS NOT NULL AND hostname<>'' AND hostname NOT LIKE 'jac313-%' "
                "GROUP BY hostname");
            while (stmt.step()) {
                HostRow r;
                stmt.get(r.hostname, r.cores);
                hosts.push_back(std::move(r));
            }
        }
        if (hosts.empty()) {
            std::cout << "anonymize-hosts: no identifying hostnames found (already clean).\n";
            return true;
        }

        // Rank by core count desc, then hostname asc (deterministic).
        std::sort(hosts.begin(), hosts.end(), [](const HostRow& a, const HostRow& b) {
            if (a.cores != b.cores) return a.cores > b.cores;
            return a.hostname < b.hostname;
        });

        // Continue numbering after the highest existing jac313-NNN (prior labels stay stable).
        std::int64_t next = 1;
        {
            auto stmt = db.prepare("SELECT hostname FROM host_hardware WHERE hostname LIKE 'jac313-%'");
            while (stmt.step()) {
                std::string h;
                stmt.get(h);
                const auto p = h.find_last_not_of("0123456789");
                if (p != std::string::npos && p + 1 < h.size()) {
                    try {
                        const std::int64_t n = std::stoll(h.substr(p + 1));
                        if (n >= next) next = n + 1;
                    } catch (...) {}
                }
            }
        }

        std::cout << "anonymize-hosts" << (dry_run ? " (dry-run)" : "") << ":\n";
        for (const auto& h : hosts) {
            std::string num = std::to_string(next++);
            if (num.size() < 3) num.insert(0, 3 - num.size(), '0');
            const std::string label = "jac313-" + num;
            std::cout << "  " << h.hostname << "  (" << h.cores << " cores)  ->  " << label << '\n';
            if (!dry_run) {
                db.exec("UPDATE host_hardware SET hostname=? WHERE hostname=?", label, h.hostname);
                db.exec("UPDATE host_hardware SET storage_model=replace(storage_model, ?, ?)",
                        h.hostname, label);
                db.exec("UPDATE group_specs SET hardware_specs=replace(hardware_specs, ?, ?)",
                        h.hostname, label);
                db.exec("UPDATE group_specs SET os_specs=replace(os_specs, ?, ?)",
                        h.hostname, label);
            }
        }
        if (dry_run) {
            std::cout << "  (dry-run: no changes written)\n";
        } else {
            std::cout << "  done -- run `jac313_test_cli matrix render --all` to refresh pages.\n";
        }
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "anonymize-hosts failed: " << ex.what() << '\n';
        return false;
    }
}

std::vector<MatrixCombo> missing_matrix_combos(const ResultsDbContext& ctx,
                                               const std::string& os,
                                               const std::string& disk,
                                               bool include_full,
                                               bool force) {
    std::vector<MatrixCombo> out;
    try {
        if (!ensure_results_schema(ctx)) {
            return out;
        }
        Sqlite db(ctx.db_path().string());
        auto stmt = db.prepare(
            "SELECT d.compiler, d.build_type, d.modules, d.size_label "
            "FROM v_desired_matrix d "
            "LEFT JOIN runs r "
            "  ON r.os=? AND r.disk_type=? "
            "  AND r.compiler=d.compiler AND r.build_type=d.build_type "
            "  AND r.modules=d.modules AND r.size_label=d.size_label "
            "WHERE (? OR r.id IS NULL) "
            "  AND (? OR d.size_label<>'xFull') "
            "ORDER BY (d.size_label='xFull'), d.compiler, d.build_type, d.modules");
        stmt.bind(os, disk,
                  static_cast<std::int64_t>(force ? 1 : 0),
                  static_cast<std::int64_t>(include_full ? 1 : 0));
        while (stmt.step()) {
            MatrixCombo c;
            stmt.get(c.compiler, c.build_type, c.modules, c.size_label);
            out.push_back(std::move(c));
        }
    } catch (const std::exception& ex) {
        std::cerr << "missing_matrix_combos failed: " << ex.what() << '\n';
    }
    return out;
}

std::int64_t allocate_test_group(const ResultsDbContext& ctx, const MatrixRunMeta& meta)
{
    if (!ensure_results_schema(ctx)) {
        throw std::runtime_error("results DB schema unavailable");
    }

    std::int64_t group_id = 0;
    {
        Sqlite db(ctx.db_path().string());
        group_id = ensure_group_specs(db, meta);
    }
    // Record the versions this group is being run against (renderer reads these back).
    record_package_versions_for_group(ctx, group_id);

    std::cout << "Allocated test group " << format_count(group_id) << '\n';
    return group_id;
}

std::optional<std::int64_t> ensure_matrix_run_record(const ResultsDbContext& ctx,
                                                     const MatrixRunMeta& meta,
                                                     const std::filesystem::path& results_base)
{
    if (!ensure_results_schema(ctx)) {
        return std::nullopt;
    }

    try {
        Sqlite db(ctx.db_path().string());
        MatrixRunMeta group_meta = meta;
        if (group_meta.host_hardware.empty()) {
            group_meta.host_hardware = collect_host_hardware_line(meta.disk_type);
        }

        const std::string results_rel =
            fs::relative(results_base, ctx.project_root).generic_string();
        const std::string page_rel = summary_page_rel(results_rel);

        db.begin();
        const std::int64_t group_id = ensure_group_specs(db, group_meta);
        std::optional<std::int64_t> run_id = upsert_run_row(
            db,
            group_id,
            meta.compiler,
            meta,
            results_rel,
            page_rel,
            utc_now_iso());

        db.commit();
        if (!run_id.has_value()) {
            std::cerr << "Failed to resolve run_id for matrix run\n";
            return std::nullopt;
        }
        return *run_id;
    } catch (const std::exception& ex) {
        std::cerr << "Matrix run record failed: " << ex.what() << '\n';
        return std::nullopt;
    }
}

bool record_scenario_result(const ResultsDbContext& ctx,
                            const std::int64_t run_id,
                            const ScenarioIdentity& scenario,
                            const std::string& status,
                            const int duration_ms,
                            const std::string& log_rel,
                            const std::optional<std::int64_t> peak_ops_sec,
                            const std::optional<std::uint64_t> persist_log_bytes)
{
    if (!ensure_results_schema(ctx)) {
        return false;
    }

    try {
        Sqlite db(ctx.db_path().string());
        db.exec(R"SQL(
INSERT INTO scenarios (
    run_id, test, package, category, persist, output_mode,
    status, duration_ms, log_rel, peak_ops_sec, persist_log_bytes
) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
ON CONFLICT(run_id, test, persist, output_mode) DO UPDATE SET
    package = excluded.package,
    category = excluded.category,
    status = excluded.status,
    duration_ms = excluded.duration_ms,
    log_rel = excluded.log_rel,
    peak_ops_sec = excluded.peak_ops_sec,
    persist_log_bytes = excluded.persist_log_bytes
)SQL",
                run_id,
                scenario.test,
                scenario.package,
                scenario.category,
                scenario_db_field(scenario.persist),
                scenario_db_field(scenario.output_mode),
                status,
                duration_ms,
                log_rel,
                peak_ops_sec,
                persist_log_bytes.has_value()
                    ? std::optional<std::int64_t>{static_cast<std::int64_t>(*persist_log_bytes)}
                    : std::optional<std::int64_t>{});
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Scenario result record failed: " << ex.what() << '\n';
        return false;
    }
}

bool finalize_matrix_run(const ResultsDbContext& ctx,
                         const std::int64_t run_id,
                         const MatrixRunMeta& meta)
{
    if (!ensure_results_schema(ctx)) {
        return false;
    }

    try {
        Sqlite db(ctx.db_path().string());
        // Read this run's identity from the row (authoritative), then keep only it for
        // that identity. WHERE + bind order both come from the RunIdentity definition.
        auto sel = db.prepare("SELECT " + identity_select_list() + " FROM runs WHERE id=?");
        sel.bind(run_id);
        if (!sel.step()) {
            return false;
        }
        RunIdentity id;
        sel.get(id.os, id.compiler, id.build_type, id.disk_type, id.size_label, id.modules);

        db.begin();
        db.exec(R"SQL(
UPDATE runs SET
    passed=?, failed=?, skipped=?, errors=?, duration_ms=?, ingested_utc=?
WHERE id=?
)SQL",
                meta.passed,
                meta.failed,
                meta.skipped,
                meta.errors,
                meta.duration_ms,
                utc_now_iso(),
                run_id);
        db.exec("DELETE FROM runs WHERE " + identity_where_eq() + " AND id != ?",
                id.os, id.compiler, id.build_type, id.disk_type, id.size_label,
                run_id);
        db.commit();
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Matrix run finalize failed: " << ex.what() << '\n';
        return false;
    }
}

bool record_scenario_compile(const ResultsDbContext& ctx,
                             const std::int64_t run_id,
                             const ScenarioIdentity& scenario,
                             const ScenarioCompileTimes& times)
{
    if (!ensure_results_schema(ctx)) {
        return false;
    }

    try {
        Sqlite db(ctx.db_path().string());
        db.exec(R"SQL(
INSERT INTO scenarios (
    run_id, test, package, category, persist, output_mode,
    status, duration_ms, log_rel,
    compile_with_modules_ms, compile_without_modules_ms
) VALUES (?, ?, ?, ?, ?, ?, 'pending', 0, '-', ?, ?)
ON CONFLICT(run_id, test, persist, output_mode) DO UPDATE SET
    compile_with_modules_ms = excluded.compile_with_modules_ms,
    compile_without_modules_ms = excluded.compile_without_modules_ms
)SQL",
                run_id,
                scenario.test,
                scenario.package,
                scenario.category,
                scenario_db_field(scenario.persist),
                scenario_db_field(scenario.output_mode),
                times.compile_with_modules_ms,
                times.compile_without_modules_ms);
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Scenario compile record failed: " << ex.what() << '\n';
        return false;
    }
}

bool record_group_compiler_build(const ResultsDbContext& ctx,
                                 const GroupCompilerBuildRecord& record)
{
    if (!ensure_results_schema(ctx)) {
        return false;
    }

    try {
        Sqlite db(ctx.db_path().string());
        const std::string build_dir_rel =
            fs::relative(record.build_dir, ctx.project_root).generic_string();
        const std::string build_dir_no_modules_rel =
            fs::relative(record.build_dir_no_modules, ctx.project_root).generic_string();
        db.exec(R"SQL(
INSERT OR REPLACE INTO group_compiler_builds (
    group_id, compiler, build_dir, build_dir_no_modules,
    build_without_modules_ms, build_modules_only_ms, build_with_modules_ms,
    build_modules_only_importstd_ms, build_with_modules_importstd_ms, recorded_utc
) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
)SQL",
                record.group_id,
                record.compiler,
                build_dir_rel,
                build_dir_no_modules_rel,
                record.build_without_modules_ms,
                record.build_modules_only_ms,
                record.build_with_modules_ms,
                record.build_modules_only_importstd_ms,
                record.build_with_modules_importstd_ms,
                utc_now_iso());
        std::cout << "Recorded build times for group " << format_count(record.group_id)
                  << " / " << record.compiler << " in " << ctx.db_path().string() << '\n';
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "group_compiler_builds insert failed: " << ex.what() << '\n';
        return false;
    }
}

bool load_hub_runs_from_db(const ResultsDbContext& ctx, std::vector<ResultsHubRun>& rows)
{
    rows.clear();
    const fs::path db_path = ctx.db_path();
    if (!fs::exists(db_path)) {
        return false;
    }

    try {
        Sqlite db(db_path.string());
        auto stmt = db.prepare(
            R"SQL(
SELECT
    r.id,
    r.group_id,
    r.summary_rel AS page_rel,
    r.os,
    r.compiler,
    r.build_type,
    r.size_label,
    r.disk_type,
    r.modules,
    r.run_utc,
    r.duration_ms,
    g.hardware_specs,
    g.os_specs,
    b.build_without_modules_ms,
    b.build_modules_only_ms,
    b.build_with_modules_ms,
    r.passed,
    r.failed,
    (
        SELECT MAX(s.peak_ops_sec)
        FROM scenarios s
        WHERE s.run_id = r.id AND s.peak_ops_sec IS NOT NULL
    ) AS peak_ops,
    (
        SELECT s.test
        FROM scenarios s
        WHERE s.run_id = r.id AND s.peak_ops_sec IS NOT NULL
        ORDER BY s.peak_ops_sec DESC
        LIMIT 1
    ) AS peak_test,
    (
        SELECT s.persist
        FROM scenarios s
        WHERE s.run_id = r.id AND s.peak_ops_sec IS NOT NULL
        ORDER BY s.peak_ops_sec DESC
        LIMIT 1
    ) AS peak_persist,
    (
        SELECT s.output_mode
        FROM scenarios s
        WHERE s.run_id = r.id AND s.peak_ops_sec IS NOT NULL
        ORDER BY s.peak_ops_sec DESC
        LIMIT 1
    ) AS peak_mode
FROM runs r
INNER JOIN (
    SELECT )SQL" + identity_group_by() + R"SQL(, MAX(id) AS max_id
    FROM runs
    GROUP BY )SQL" + identity_group_by() + R"SQL(
) latest
    ON )SQL" + identity_join_on("r", "latest") + R"SQL(
   AND r.id = latest.max_id
LEFT JOIN group_specs g ON g.group_id = r.group_id
LEFT JOIN group_compiler_builds b
    ON b.group_id = r.group_id AND b.compiler = r.compiler
ORDER BY )SQL" + identity_select_list("r") + ";");

        while (stmt.step()) {
            ResultsHubRun row;
            std::optional<std::int64_t> peak_ops;
            std::int64_t duration_ms = 0;
            std::int64_t passed = 0;
            std::int64_t failed = 0;
            std::string peak_test;
            std::string peak_persist;
            std::string peak_mode;
            std::optional<std::int64_t> build_without_ms;
            std::optional<std::int64_t> build_modules_only_ms;
            std::optional<std::int64_t> build_with_ms;

            stmt.get(row.run_id,
                     row.group_id,
                     row.page_rel,
                     row.os,
                     row.compiler,
                     row.build_type,
                     row.size_label,
                     row.disk_type,
                     row.modules,
                     row.run_utc,
                     duration_ms,
                     row.hardware_specs,
                     row.os_specs,
                     build_without_ms,
                     build_modules_only_ms,
                     build_with_ms,
                     passed,
                     failed,
                     peak_ops,
                     peak_test,
                     peak_persist,
                     peak_mode);

            if (build_without_ms.has_value()) {
                row.build_without_modules_ms = static_cast<int>(*build_without_ms);
            }
            if (build_modules_only_ms.has_value()) {
                row.build_modules_only_ms = static_cast<int>(*build_modules_only_ms);
            }
            if (build_with_ms.has_value()) {
                row.build_with_modules_ms = static_cast<int>(*build_with_ms);
            }

            row.duration_ms = static_cast<int>(duration_ms);
            row.passed = static_cast<int>(passed);
            row.failed = static_cast<int>(failed);
            row.peak_ops_sec = peak_ops;
            row.peak_scenario = format_peak_scenario_cell(peak_test, peak_persist, peak_mode);
            rows.push_back(std::move(row));
        }

        return !rows.empty();
    } catch (const std::exception& ex) {
        std::cerr << "Results DB hub query failed: " << ex.what() << '\n';
        return false;
    }
}

bool write_run_results_page(const ResultsDbContext& ctx, const std::int64_t run_id) {
    const fs::path db_path = ctx.db_path();
    if (!fs::exists(db_path)) {
        return false;
    }

    try {
        Sqlite db(db_path.string());
        RunPageData run;
        std::vector<ScenarioPageRow> scenarios;
        if (!load_run_page_data(db, run_id, run)) {
            return false;
        }
        load_run_scenarios(db, run_id, scenarios);

        const fs::path page_path = ctx.project_root / run.page_rel;
        fs::create_directories(page_path.parent_path());
        const std::string db_href = ctx.db_href_from(page_path.parent_path());
        std::ofstream out(page_path);
        if (!out) {
            std::cerr << "Failed to write " << page_path << '\n';
            return false;
        }

        render_run_results_page(out, run, scenarios, db_href, ctx.project_root);
        out.close();
        std::cout << "Wrote " << fs::relative(page_path, ctx.project_root).generic_string()
                  << " from results DB\n" << std::flush;
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Run page render failed: " << ex.what() << '\n';
        return false;
    }
}

bool print_top_fastest(const ResultsDbContext& ctx, int limit) {
    if (limit < 1) {
        limit = 10;
    }

    const fs::path db_path = ctx.db_path();
    if (!fs::exists(db_path)) {
        std::cerr << "No results database at " << db_path << " (run matrix run first)\n";
        return false;
    }

    try {
        Sqlite db(db_path.string());
        auto stmt = db.prepare(R"SQL(
SELECT peak_ops_sec, test, persist, output_mode, compiler, size_label, disk_type,
       run_utc, duration_ms, log_rel, results_base
FROM v_scenario_throughput
ORDER BY peak_ops_sec DESC
LIMIT ?
)SQL");
        stmt.bind(limit);

        std::cout << "Top " << limit << " fastest scenarios (peak ops/sec)\n\n";
        std::cout << "| # | Peak ops/sec | Test | Persist | Compiler | Size | Run UTC | ms | Log |\n";
        std::cout << "|---|--------------|------|---------|----------|------|---------|----|-----|\n";

        int rank = 0;
        while (stmt.step()) {
            ++rank;
            std::int64_t peak_ops = 0;
            std::string test;
            std::string persist;
            std::string output_mode;
            std::string compiler;
            std::string size_label;
            std::string disk_type;
            std::string run_utc;
            std::int64_t duration_ms = 0;
            std::string log_rel;
            std::string results_base;

            stmt.get(peak_ops,
                     test,
                     persist,
                     output_mode,
                     compiler,
                     size_label,
                     disk_type,
                     run_utc,
                     duration_ms,
                     log_rel,
                     results_base);

            const std::string persist_cell = persist + "/" + output_mode;
            std::cout << "| " << rank
                      << " | " << format_count(peak_ops)
                      << " | " << test
                      << " | " << persist_cell
                      << " | " << compiler
                      << " | " << size_label
                      << " | " << display_run_utc(run_utc)
                      << " | " << format_count(duration_ms)
                      << " | " << log_rel << " |\n";
        }

        if (rank == 0) {
            std::cout << "_No throughput data yet (benchmark tests 005–008 report Fastest run)._ \n";
            return false;
        }

        std::cout << "\nDatabase: " << db_path.string() << '\n';
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Results DB query failed: " << ex.what() << '\n';
        return false;
    }
}

struct ThroughputRow {
    int rank{0};
    std::int64_t peak_ops_sec{0};
    std::string test;
    std::string persist;
    std::string output_mode;
    std::int64_t duration_ms{0};
    std::string log_rel;
    std::string results_base;
    std::string disk_type;
    std::string hardware_specs;
    std::string os_specs;
};



void write_throughput_category_table(std::ostream& out,
                                     const ResultsDbContext& ctx,
                                     const std::vector<ThroughputRow>& rows)
{
    constexpr int column_count = 6;

    out << "<table>\n<thead>\n<tr>";
    out << "<th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th>";
    out << "</tr>\n</thead>\n<tbody>\n";

    for (const auto& row : rows) {
        out << "<tr>";
        out << "<td>" << row.rank << "</td>";
        out << "<td style=\"text-align:right\">" << html_escape(format_count(row.peak_ops_sec))
            << "</td>";
        out << "<td>" << html_escape(row.test) << "</td>";
        out << "<td>" << html_escape(persist_mode_cell(row.persist, row.output_mode)) << "</td>";
        out << "<td style=\"text-align:right\">" << html_escape(format_count(row.duration_ms))
            << "</td>";
        out << "<td>" << hub_log_link_html(ctx.project_root, row.results_base, row.log_rel)
            << "</td>";
        out << "</tr>\n";
        out << "<tr><td colspan=\"" << column_count << "\"><strong>Hardware:</strong> "
            << html_escape(hardware_display_line(
                   row.hardware_specs, row.os_specs, row.disk_type))
            << "</td></tr>\n";
    }

    out << "</tbody>\n</table>\n\n";
}

bool write_persist_backend_comparison_hub(std::ostream& out, const ResultsDbContext& ctx)
{
    const fs::path db_path = ctx.db_path();
    if (!fs::exists(db_path)) {
        return false;
    }

    try {
        Sqlite db(db_path.string());
        auto stmt = db.prepare(R"SQL(
SELECT r.id, r.os, r.compiler, r.build_type, r.size_label, r.results_base
FROM runs r
JOIN (
    SELECT os, compiler, build_type, MAX(id) AS run_id
    FROM runs
    WHERE size_label = 'xFull'
    GROUP BY os, compiler, build_type
) latest ON latest.run_id = r.id
ORDER BY r.os, r.compiler, r.build_type
)SQL");

        std::ostringstream body;
        bool any = false;
        while (stmt.step()) {
            std::int64_t run_id = 0;
            std::string os;
            std::string compiler;
            std::string build_type;
            std::string size_label;
            std::string results_rel;
            stmt.get(run_id, os, compiler, build_type, size_label, results_rel);

            std::vector<ScenarioPageRow> scenarios;
            if (!load_run_scenarios(db, run_id, scenarios)) {
                continue;
            }

            const std::string heading = std::string("### ") + os + " · " + compiler + " ("
                                        + size_label + ", "
                                        + (build_type.empty() ? "Debug" : build_type)
                                        + ", run " + format_count(run_id) + ")";
            const std::size_t before = body.tellp();
            write_persist_backend_comparison_table(
                body,
                scenarios,
                heading.c_str(),
                [](const ScenarioPageRow& row) { return is_benchmark_matrix_test(row.test); });
            if (body.tellp() > static_cast<std::streampos>(before)) {
                any = true;
            }
        }

        if (!any) {
            return false;
        }

        out << "## Persist backend comparison (latest xFull per compiler & build type)\n\n";
        out << "Benchmark tests 005–008: **run ms**, **peak ops/sec**, and **persist log size** "
               "for binary vs jText vs SQL.\n\n";
        out << "_test_006 reports peak ops/sec as **N/A** — it is a tail-reader "
               "concurrency/correctness test, not a throughput benchmark, and emits no ops/sec line._\n\n";
        out << body.str();
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Persist backend hub comparison failed: " << ex.what() << '\n';
        return false;
    }
}

bool write_throughput_by_log_category(std::ostream& out,
                                      const ResultsDbContext& ctx,
                                      int limit_per_category)
{
    if (limit_per_category < 1) {
        limit_per_category = 10;
    }

    const fs::path db_path = ctx.db_path();
    if (!fs::exists(db_path)) {
        return false;
    }

    try {
        Sqlite db(db_path.string());
        auto stmt = db.prepare(R"SQL(
SELECT rank_in_cat, peak_ops_sec, test, persist, output_mode, duration_ms, log_rel,
       results_base, log_category, hardware_specs, os_specs, disk_type
FROM (
    SELECT substr(v.log_rel, 1, instr(v.log_rel, '/') - 1) AS log_category,
           ROW_NUMBER() OVER (
               PARTITION BY substr(v.log_rel, 1, instr(v.log_rel, '/') - 1)
               ORDER BY v.peak_ops_sec DESC
           ) AS rank_in_cat,
           v.peak_ops_sec, v.test, v.persist, v.output_mode, v.duration_ms,
           v.log_rel, v.results_base, v.hardware_specs, v.os_specs, v.disk_type
    FROM v_scenario_throughput v
    WHERE v.size_label = 'xFull'
)
WHERE rank_in_cat <= ?
ORDER BY log_category, rank_in_cat
)SQL");
        stmt.bind(limit_per_category);

        std::map<std::string, std::vector<ThroughputRow>> by_category;
        while (stmt.step()) {
            ThroughputRow row;
            std::string log_category;
            std::int64_t rank = 0;
            stmt.get(rank,
                     row.peak_ops_sec,
                     row.test,
                     row.persist,
                     row.output_mode,
                     row.duration_ms,
                     row.log_rel,
                     row.results_base,
                     log_category,
                     row.hardware_specs,
                     row.os_specs,
                     row.disk_type);
            row.rank = static_cast<int>(rank);
            by_category[std::move(log_category)].push_back(std::move(row));
        }

        if (by_category.empty()) {
            return false;
        }

        out << "## Top throughput by log category\n\n";
        out << "Peak ops/sec from benchmark tests 005–008 (`Fastest run` in each log). "
               "xFull matrix only, grouped by persist log directory.\n\n";

        bool any_section = false;
        for (const std::string& category : log_category_order()) {
            const auto it = by_category.find(category);
            if (it == by_category.end() || it->second.empty()) {
                continue;
            }
            any_section = true;

            out << "### " << category << "\n\n";
            write_throughput_category_table(out, ctx, it->second);
        }

        for (const auto& [category, rows] : by_category) {
            if (std::find(log_category_order().begin(), log_category_order().end(), category)
                != log_category_order().end()) {
                continue;
            }
            if (rows.empty()) {
                continue;
            }
            any_section = true;
            out << "### " << category << "\n\n";
            write_throughput_category_table(out, ctx, rows);
        }

        return any_section;
    } catch (const std::exception& ex) {
        std::cerr << "Throughput by log category failed: " << ex.what() << '\n';
        return false;
    }
}

bool print_latest_runs(const ResultsDbContext& ctx) {
    const fs::path db_path = ctx.db_path();
    if (!fs::exists(db_path)) {
        std::cerr << "No results database at " << db_path << " (run matrix run first)\n";
        return false;
    }

    try {
        Sqlite db(db_path.string());
        auto stmt = db.prepare(R"SQL(
SELECT compiler, size_label, disk_type, run_utc, passed, failed, duration_ms, results_base
FROM v_latest_runs
ORDER BY compiler, size_label, disk_type
)SQL");

        std::cout << "Latest runs (v_latest_runs)\n\n";
        std::cout << "| Compiler | Size | Disk | Run UTC | Passed | Failed | Duration | Results |\n";
        std::cout << "|----------|------|------|---------|--------|--------|----------|----------|\n";

        int count = 0;
        while (stmt.step()) {
            ++count;
            std::string compiler;
            std::string size_label;
            std::string disk_type;
            std::string run_utc;
            std::int64_t passed = 0;
            std::int64_t failed = 0;
            std::int64_t duration_ms = 0;
            std::string results_base;

            stmt.get(compiler,
                     size_label,
                     disk_type,
                     run_utc,
                     passed,
                     failed,
                     duration_ms,
                     results_base);

            std::cout << "| " << compiler
                      << " | " << size_label
                      << " | " << disk_type
                      << " | " << display_run_utc(run_utc)
                      << " | " << format_count(passed)
                      << " | " << format_count(failed)
                      << " | " << format_duration_human(static_cast<int>(duration_ms))
                      << " | " << results_base << " |\n";
        }

        if (count == 0) {
            std::cout << "_No runs recorded yet._\n";
            return false;
        }

        std::cout << "\nDatabase: " << db_path.string() << '\n';
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Results DB query failed: " << ex.what() << '\n';
        return false;
    }
}

namespace {

std::deque<std::string> last_lines(const std::string& text, const int line_count)
{
    std::deque<std::string> tail;
    std::istringstream in(text);
    std::string line;
    while (std::getline(in, line)) {
        tail.push_back(std::move(line));
        if (static_cast<int>(tail.size()) > line_count) {
            tail.pop_front();
        }
    }
    return tail;
}

} // namespace

bool publish_scenario_log_tail(const ResultsDbContext& ctx,
                               const std::string& results_rel,
                               const std::string& log_rel,
                               const std::string& stdout_text,
                               const int line_count)
{
    if (log_rel.empty() || log_rel == "-" || stdout_text.empty()) {
        return false;
    }

    const fs::path dest =
        ctx.project_root / summary_tree_rel(results_rel) / log_rel;
    fs::create_directories(dest.parent_path());
    std::ofstream out(dest, std::ios::trunc);
    if (!out) {
        return false;
    }

    out << "# jac313 log tail (last " << line_count << " lines)\n";
    out << "# captured at matrix run\n\n";
    for (const auto& row : last_lines(stdout_text, line_count)) {
        out << row << '\n';
    }
    return true;
}

std::optional<std::int64_t> latest_run_id(const ResultsDbContext& ctx)
{
    const fs::path db_path = ctx.db_path();
    if (!fs::exists(db_path)) {
        return std::nullopt;
    }

    try {
        Sqlite db(db_path.string());
        auto stmt = db.prepare("SELECT MAX(id) FROM runs");
        if (!stmt.step()) {
            return std::nullopt;
        }
        std::int64_t id = 0;
        stmt.get(id);
        return id > 0 ? std::optional<std::int64_t>{id} : std::nullopt;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<fs::path> results_base_for_run(const ResultsDbContext& ctx, const std::int64_t run_id)
{
    const fs::path db_path = ctx.db_path();
    if (!fs::exists(db_path)) {
        return std::nullopt;
    }

    try {
        Sqlite db(db_path.string());
        auto stmt = db.prepare("SELECT results_base FROM runs WHERE id=?");
        stmt.bind(run_id);
        if (!stmt.step()) {
            return std::nullopt;
        }
        std::string results_rel;
        stmt.get(results_rel);
        return ctx.project_root / results_rel;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

} // namespace jac313::test_cli