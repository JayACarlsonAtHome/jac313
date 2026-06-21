#include "test_summary_render.hpp"
#include "format.hpp"
#include "host_hardware.hpp"
#include "matrix_manifest.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace jac313::test_cli {
namespace fs = std::filesystem;

namespace {

struct HubRow {
    fs::path page_path;
    std::string os;
    std::string compiler;
    std::string build_type;
    std::string size_label;
    std::string disk_type;
    std::string modules;
    std::string run_utc;
    std::string duration;
    std::string peak_ops;
    std::string peak_scenario;
    std::string passed;
    std::string failed;
    std::string hardware_specs;
    std::string os_specs;
    std::int64_t group_id{0};
    std::string build_with_modules;
};

std::string hub_cell_or_dash(const std::string& value) {
    return value.empty() ? "-" : value;
}

struct HubColumn {
    const char* header;
    bool right_align;
    std::function<std::string(const HubRow&, const fs::path& project_root)> cell;
    bool raw_html{false};
};

std::vector<HubColumn> hub_columns() {
    return {
        {"OS", false, [](const HubRow& row, const fs::path&) {
            return hub_cell_or_dash(row.os);
        }},
        {"Compiler", false, [](const HubRow& row, const fs::path&) {
            return hub_cell_or_dash(row.compiler);
        }},
        {"Build", false, [](const HubRow& row, const fs::path&) {
            return hub_cell_or_dash(row.build_type);
        }},
        {"Modules", false, [](const HubRow& row, const fs::path&) {
            return hub_cell_or_dash(row.modules);
        }},
        {"Size", false, [](const HubRow& row, const fs::path&) {
            return hub_cell_or_dash(row.size_label);
        }},
        {"Disk", false, [](const HubRow& row, const fs::path&) {
            return hub_cell_or_dash(row.disk_type);
        }},
        {"Group", true, [](const HubRow& row, const fs::path&) {
            return row.group_id > 0 ? format_count(row.group_id) : "-";
        }},
        {"Run UTC", false, [](const HubRow& row, const fs::path&) {
            return hub_cell_or_dash(row.run_utc);
        }},
        {"Duration", true, [](const HubRow& row, const fs::path&) {
            return hub_cell_or_dash(row.duration);
        }},
        {"Build (modules)", true, [](const HubRow& row, const fs::path&) {
            return hub_cell_or_dash(row.build_with_modules);
        }},
        {"Peak ops/sec", true, [](const HubRow& row, const fs::path&) {
            return hub_cell_or_dash(row.peak_ops);
        }},
        {"Peak scenario", false, [](const HubRow& row, const fs::path&) {
            return hub_cell_or_dash(row.peak_scenario);
        }},
        {"Passed", true, [](const HubRow& row, const fs::path&) {
            return row.passed;
        }},
        {"Failed", true, [](const HubRow& row, const fs::path&) {
            return row.failed;
        }},
        {"Run",
         false,
         [](const HubRow& row, const fs::path& project_root) {
             const std::string rel =
                 fs::relative(row.page_path, project_root).generic_string();
             return "<a href=\"" + html_escape(rel) + "\">RUN.md</a>";
         },
         true},
    };
}

// "jac313::Qlite::v001" -> "Qlite" for the per-run Versions sub-row.
static std::string short_pkg_name(const std::string& qualified) {
    const auto a = qualified.find("::");
    if (a == std::string::npos) return qualified;
    const auto b = qualified.find("::", a + 2);
    if (b == std::string::npos) return qualified.substr(a + 2);
    return qualified.substr(a + 2, b - (a + 2));
}

void write_padded_hub_table(
    std::ofstream& hub,
    const std::vector<HubRow>& hub_rows,
    const fs::path& project_root,
    const std::unordered_map<std::int64_t, std::vector<PackageVersionRow>>& versions_by_group)
{
    const std::vector<HubColumn> columns = hub_columns();
    const std::size_t column_count = columns.size();

    // Emit one <table> per chunk of N runs (header + N runs), blank line between. Separate
    // tables each get their OWN horizontal scrollbar in the web view, so you can scroll the
    // wide columns right where you're reading — instead of one scrollbar stranded at the
    // bottom of a giant list — and the column header is never far above the row.
    constexpr std::size_t kRunsPerTable = 6;

    auto emit_run = [&](const HubRow& row) {
        hub << "<tr>";
        for (const auto& column : columns) {
            const std::string cell = column.cell(row, project_root);
            const char* align = column.right_align ? " style=\"text-align:right\"" : "";
            hub << "<td" << align << ">";
            hub << (column.raw_html ? cell : html_escape(cell));
            hub << "</td>";
        }
        hub << "</tr>\n";

        // Versions sub-row ABOVE Hardware: version sits before the taller hardware line.
        hub << "<tr><td colspan=\"" << column_count << "\"><strong>Versions:</strong> ";
        if (const auto it = versions_by_group.find(row.group_id);
            it != versions_by_group.end() && !it->second.empty()) {
            bool first = true;
            for (const auto& v : it->second) {
                if (!first) hub << " · ";
                first = false;
                hub << html_escape(short_pkg_name(v.name)) << ' ' << v.major << '.' << v.minor;
            }
        } else {
            hub << "—";
        }
        hub << "</td></tr>\n";

        hub << "<tr><td colspan=\"" << column_count << "\"><strong>Hardware:</strong> "
            << html_escape(hardware_display_line(
                   row.hardware_specs, row.os_specs, row.disk_type))
            << "</td></tr>\n";
    };

    for (std::size_t start = 0; start < hub_rows.size(); start += kRunsPerTable) {
        if (start > 0) {
            hub << '\n';  // blank line separates the tables
        }
        hub << "<table>\n<thead>\n<tr>";
        for (const auto& column : columns) {
            hub << "<th>" << html_escape(column.header) << "</th>";
        }
        hub << "</tr>\n</thead>\n<tbody>\n";
        const std::size_t end = std::min(start + kRunsPerTable, hub_rows.size());
        for (std::size_t i = start; i < end; ++i) {
            emit_run(hub_rows[i]);
        }
        hub << "</tbody>\n</table>\n";
    }
}

bool hub_row_from_db_run(const fs::path& project_root,
                         const ResultsHubRun& src,
                         HubRow& row)
{
    row.page_path = project_root / src.page_rel;
    row.os = src.os;
    row.compiler = src.compiler;
    row.build_type = src.build_type;
    row.size_label = src.size_label;
    row.disk_type = src.disk_type;
    row.modules = src.modules;
    row.run_utc = display_run_utc(src.run_utc);
    row.duration = format_duration_human(src.duration_ms);
    row.passed = format_count(src.passed);
    row.failed = format_count(src.failed);
    row.hardware_specs = src.hardware_specs;
    row.os_specs = src.os_specs;
    row.group_id = src.group_id;
    if (src.build_with_modules_ms.has_value()) {
        row.build_with_modules = format_duration_human(*src.build_with_modules_ms);
    }
    if (src.peak_ops_sec.has_value()) {
        row.peak_ops = format_count(*src.peak_ops_sec);
    }
    row.peak_scenario = src.peak_scenario;
    return true;
}

} // namespace

bool write_summary_hub(const ResultsDbContext& ctx, const bool include_throughput) {
    const fs::path hub_dir = ctx.project_root / "test-summary";
    fs::create_directories(hub_dir);

    // Make render self-sufficient: ensure the schema (creates package_versions and runs the
    // additive migrations) so a render on a DB last written by an older build still works.
    ensure_results_schema(ctx);

    std::vector<HubRow> hub_rows;
    std::vector<ResultsHubRun> db_runs;
    if (load_hub_runs_from_db(ctx, db_runs)) {
        hub_rows.reserve(db_runs.size());
        for (const auto& db_run : db_runs) {
            HubRow row;
            if (hub_row_from_db_run(ctx.project_root, db_run, row)) {
                hub_rows.push_back(std::move(row));
            }
        }
    }

    std::sort(hub_rows.begin(), hub_rows.end(),
              [](const HubRow& a, const HubRow& b) {
                  if (a.os != b.os) return a.os < b.os;
                  if (a.compiler != b.compiler) return a.compiler < b.compiler;
                  if (a.build_type != b.build_type) return a.build_type < b.build_type;
                  if (a.size_label != b.size_label) return a.size_label < b.size_label;
                  return a.disk_type < b.disk_type;
              });

    std::ofstream hub(hub_dir / "README.md");
    if (!hub) {
        return false;
    }

    hub << "# jac313 test-summary hub\n\n";
    const std::string db_href = ctx.db_href_from(hub_dir);
    hub << "Run index and per-run pages from [`jac313_results.db`](" << db_href << "); "
           "log tails (last " << kLogTailLineCount
        << " lines) published at matrix run from captured stdout.\n";
    hub << "Pipeline docs: [SETUP → Testing](../docs/Setup.md#4-testing) "
           "(`release-check`, matrix runs, `--filter`).\n\n";

    // Per-run versions: load all (recorded at run time; old groups backfilled) and index by
    // group_id so the runs table can show each group's versions as a sub-row. Pure DB read.
    std::unordered_map<std::int64_t, std::vector<PackageVersionRow>> versions_by_group;
    {
        std::vector<PackageVersionRow> all;
        load_package_versions(ctx, all);
        for (auto& v : all) {
            versions_by_group[v.group_id].push_back(std::move(v));
        }
    }

    if (hub_rows.empty()) {
        hub << "_No recorded runs yet. Run `jac313_test_cli matrix run`._\n";
    } else {
        write_padded_hub_table(hub, hub_rows, ctx.project_root, versions_by_group);
        if (include_throughput) {
            hub << '\n';
            write_persist_backend_comparison_hub(hub, ctx);
            hub << '\n';
            write_throughput_by_log_category(hub, ctx, 10);
        }
    }

    hub.flush();
    hub.close();
    std::cout << "Wrote " << (hub_dir / "README.md").string() << '\n';
    return true;
}

bool render_test_summary_through(const ResultsDbContext& ctx,
                                 const std::int64_t run_id,
                                 const SummaryRenderStep through)
{
    std::cout << "\n=== rendering test-summary ===\n" << std::flush;

    // Ensure schema before ANY query: runs the additive migrations (import-std build
    // columns, package_versions table) so a render on a DB written by an older build
    // doesn't fail at the first query.
    ensure_results_schema(ctx);

    if (through >= SummaryRenderStep::RunPage) {
        std::cout << "[render] step 1: run page\n" << std::flush;
        if (!write_run_results_page(ctx, run_id)) {
            return false;
        }
    }

    if (through >= SummaryRenderStep::Hub) {
        std::cout << "[render] step 2: hub README\n" << std::flush;
        if (!write_summary_hub(ctx, through >= SummaryRenderStep::Hub)) {
            return false;
        }
    }

    std::cout << "[render] done\n" << std::flush;
    return true;
}

bool render_test_summary(const ResultsDbContext& ctx, const std::int64_t run_id)
{
    return render_test_summary_through(ctx, run_id, SummaryRenderStep::All);
}

} // namespace jac313::test_cli