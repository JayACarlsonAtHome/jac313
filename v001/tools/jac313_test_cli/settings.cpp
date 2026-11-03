#include "settings.hpp"

#include "parser/section.hpp"
#include "validator/validator.hpp"

#include <jac313/jText/v001.hpp>

#include <algorithm>
#include <cctype>
#include <charconv>
#include <chrono>
#include <format>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <system_error>

namespace jac313::test_cli {
namespace fs = std::filesystem;
namespace setup = jac313::Setup::v001;

namespace {

std::string where(const fs::path& path, std::string_view section) {
    return path.string() + " (section " + std::string(section) + ")";
}

std::vector<std::string> split_words(const std::string& value) {
    std::vector<std::string> out;
    std::istringstream in(value);
    for (std::string w; in >> w;) out.push_back(w);
    return out;
}

int to_int(const std::map<std::string, std::string>& row, const std::string& column,
           const fs::path& path, std::string_view section) {
    const std::string& v = row.at(column);
    int out = 0;
    const auto [end, ec] = std::from_chars(v.data(), v.data() + v.size(), out);
    if (ec != std::errc{} || end != v.data() + v.size())
        throw std::runtime_error(where(path, section) + ": " + column + " '" + v + "' is not an integer");
    return out;
}

// The sections a local override file holds, each with its key columns, by tracked file name.
struct OverrideSection {
    std::string_view section;
    std::vector<std::string> key;
};
const std::vector<OverrideSection>& override_sections(const fs::path& tracked) {
    static const std::vector<OverrideSection> compilers = {{"toolchains", {"label"}},
                                                           {"toolchain_os", {"os", "toolchain", "activation_seq"}}};
    static const std::vector<OverrideSection> recipes = {{"components", {"component"}},
                                                         {"recipe_os", {"os", "component"}}};
    if (tracked.filename() == "compilers.jtext") return compilers;
    if (tracked.filename() == "recipes.jtext") return recipes;
    throw std::runtime_error(tracked.string() + " has no local override file");
}

// Section `section` of the local file `local`, checked against the tracked table's columns.
SettingsTable read_local_section(const fs::path& local, std::string_view section, const SettingsTable& tracked,
                                 const fs::path& tracked_path) {
    SettingsTable loc = read_jtext_table(local, section);
    require_columns(loc, local, section, tracked.columns);
    for (const auto& c : loc.columns)
        if (c != "delete" && std::find(tracked.columns.begin(), tracked.columns.end(), c) == tracked.columns.end())
            throw std::runtime_error(where(local, section) + ": column " + c + " is not in " + tracked_path.string());
    return loc;
}

// Section `section` of the tracked table `path`, with the rows of its local override file applied by
// `key` when that file exists (replace in place, add at the end, delete = true removes).
SettingsTable read_with_local(const fs::path& path, std::string_view section) {
    SettingsTable table = read_jtext_table(path, section);
    const fs::path local = local_override_path(path);
    std::error_code ec;
    if (!fs::exists(local, ec)) return table;
    const auto& sections = override_sections(path);
    const auto sec = std::find_if(sections.begin(), sections.end(), [&](const auto& s) { return s.section == section; });
    if (sec == sections.end()) throw std::runtime_error(where(path, section) + ": no local override key");
    const SettingsTable loc = read_local_section(local, section, table, path);
    const bool has_delete = std::find(loc.columns.begin(), loc.columns.end(), "delete") != loc.columns.end();
    const auto key_of = [&](const std::map<std::string, std::string>& row) {
        std::string k;
        for (const auto& c : sec->key) k += row.at(c) + '\x1f';
        return k;
    };
    for (const auto& lr : loc.rows) {
        std::map<std::string, std::string> row;
        for (const auto& c : table.columns) row[c] = lr.at(c);
        const std::string k = key_of(row);
        const auto it = std::find_if(table.rows.begin(), table.rows.end(), [&](const auto& r) { return key_of(r) == k; });
        if (has_delete && jtext_bool(lr.at("delete"))) {
            if (it != table.rows.end()) table.rows.erase(it);
        } else if (it != table.rows.end()) {
            *it = std::move(row);
        } else {
            table.rows.push_back(std::move(row));
        }
    }
    return table;
}

} // namespace

fs::path compilers_table_path(const fs::path& source_dir) { return source_dir / "Setup" / "compilers.jtext"; }
fs::path recipes_table_path(const fs::path& source_dir) { return source_dir / "Setup" / "recipes.jtext"; }

std::string pinned_tool_version(const fs::path& source_dir, const std::string& os_family, const std::string& tool) {
    for (const auto& row : read_jtext_table(recipes_table_path(source_dir), "pinned_tools").rows)
        if (row.at("os") == os_family && row.at("tool") == tool) return row.at("version");
    return {};
}

fs::path local_override_path(const fs::path& tracked) {
    fs::path local = tracked;
    local.replace_extension(".local.jtext");
    return local;
}

std::string local_override_note(const fs::path& tracked) {
    const fs::path local = local_override_path(tracked);
    std::error_code ec;
    if (!fs::exists(local, ec)) return {};
    std::size_t rows = 0;
    for (const auto& s : override_sections(tracked))
        rows += read_local_section(local, s.section, read_jtext_table(tracked, s.section), tracked).rows.size();
    return "local overrides: " + local.string() + " (" + std::to_string(rows) + (rows == 1 ? " row)" : " rows)");
}

SettingsTable read_jtext_table(const fs::path& path, std::string_view section) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("cannot read " + path.string());
    std::stringstream ss;
    ss << in.rdbuf();
    const std::string text = ss.str();
    auto pf = jtext::parse_file_structure(std::string_view(text));
    if (!pf) throw std::runtime_error(path.string() + ": " + pf.error().to_string());
    const auto vr = jtext::validate(*pf);
    if (vr.report.error_count() > 0) {
        std::string err = path.string() + ": " + std::to_string(vr.report.error_count()) + " jText error(s)";
        for (const auto& is : vr.report.issues)
            if (is.severity == jtext::issue_severity::error) err += "\n  " + is.to_string();
        throw std::runtime_error(err);
    }
    for (const auto& sec : vr.file.sections) {
        if (sec.name != section) continue;
        SettingsTable table;
        for (const auto& f : sec.fields) table.columns.push_back(f.name);
        for (const auto& rec : sec.records) {
            std::map<std::string, std::string> row;
            for (std::size_t i = 0; i < table.columns.size(); ++i)
                row[table.columns[i]] = i < rec.values.size() ? rec.values[i] : std::string{};
            table.rows.push_back(std::move(row));
        }
        return table;
    }
    throw std::runtime_error(path.string() + ": no section " + std::string(section));
}

void require_columns(const SettingsTable& table, const fs::path& path, std::string_view section,
                     const std::vector<std::string>& columns) {
    for (const auto& c : columns)
        if (std::find(table.columns.begin(), table.columns.end(), c) == table.columns.end())
            throw std::runtime_error(where(path, section) + ": no column " + c);
}

bool jtext_bool(std::string_view value) {
    std::string v(value);
    std::transform(v.begin(), v.end(), v.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return v == "true" || v == "yes" || v == "y" || v == "t" || v == "1";
}

std::vector<setup::Toolchain> load_toolchains(const fs::path& source_dir, const std::string& os_family) {
    const fs::path path = compilers_table_path(source_dir);
    const SettingsTable tcs = read_with_local(path, "toolchains");
    require_columns(tcs, path, "toolchains",
                    {"label", "kind", "version", "min_version", "build_dir", "flag", "cc_names", "scan_deps"});
    const SettingsTable os = read_with_local(path, "toolchain_os");
    require_columns(os, path, "toolchain_os", {"os", "toolchain", "required", "activation_seq", "activation"});

    const bool family_listed = std::any_of(os.rows.begin(), os.rows.end(),
                                           [&](const auto& r) { return r.at("os") == os_family; });
    std::vector<setup::Toolchain> out;
    for (const auto& r : tcs.rows) {
        setup::Toolchain tc;
        tc.label = r.at("label");
        tc.kind = r.at("kind");
        tc.version_major = to_int(r, "version", path, "toolchains");
        if (const int min = to_int(r, "min_version", path, "toolchains"); min > 0) tc.min_version = min;
        tc.build_dir = r.at("build_dir");
        tc.flag = r.at("flag");
        tc.cc_names = split_words(r.at("cc_names"));
        tc.scan_deps = split_words(r.at("scan_deps"));

        std::vector<std::pair<int, std::string>> acts;
        for (const auto& o : os.rows) {
            if (o.at("toolchain") != tc.label) continue;
            if (jtext_bool(o.at("required"))
                && std::find(tc.required_on.begin(), tc.required_on.end(), o.at("os")) == tc.required_on.end())
                tc.required_on.push_back(o.at("os"));
            if (family_listed && o.at("os") != os_family) continue;
            acts.emplace_back(to_int(o, "activation_seq", path, "toolchain_os"), o.at("activation"));
        }
        std::stable_sort(acts.begin(), acts.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
        for (auto& [seq, act] : acts)
            if (std::find(tc.activations.begin(), tc.activations.end(), act) == tc.activations.end())
                tc.activations.push_back(std::move(act));
        if (tc.activations.empty())
            throw std::runtime_error(where(path, "toolchain_os") + ": no activation for " + tc.label);
        out.push_back(std::move(tc));
    }
    return out;
}

std::vector<setup::ProvisionRecipe> load_recipes(const fs::path& source_dir) {
    const fs::path path = recipes_table_path(source_dir);
    const SettingsTable comps = read_with_local(path, "components");
    require_columns(comps, path, "components", {"component", "desc", "for", "required", "check"});
    const SettingsTable os = read_with_local(path, "recipe_os");
    require_columns(os, path, "recipe_os", {"os", "component", "command"});

    std::vector<setup::ProvisionRecipe> out;
    for (const auto& r : comps.rows) {
        setup::ProvisionRecipe rec;
        rec.component = r.at("component");
        rec.desc = r.at("desc");
        rec.for_toolchain = r.at("for");
        rec.required = jtext_bool(r.at("required"));
        rec.check = r.at("check");
        for (const auto& o : os.rows)
            if (o.at("component") == rec.component) rec.install_by_family.emplace(o.at("os"), o.at("command"));
        out.push_back(std::move(rec));
    }
    return out;
}

fs::path compiler_pin_path(const fs::path& source_dir) { return source_dir / "Setup" / "compilers.pin.jtext"; }

std::vector<CompilerPinRow> load_compiler_pins(const fs::path& source_dir) {
    const fs::path path = compiler_pin_path(source_dir);
    std::error_code ec;
    if (!fs::exists(path, ec)) return {};
    const SettingsTable pins = read_jtext_table(path, "pins");
    require_columns(pins, path, "pins",
                    {"label", "cpu_model", "cores", "ram_gb", "os_pretty", "gcc_label", "clang_label"});
    std::vector<CompilerPinRow> out;
    for (const auto& r : pins.rows) {
        CompilerPinRow p;
        p.label = r.at("label");
        p.cpu = r.at("cpu_model");
        p.cores = to_int(r, "cores", path, "pins");
        p.ram_gb = to_int(r, "ram_gb", path, "pins");
        p.os = r.at("os_pretty");
        if (const auto d = r.find("disk"); d != r.end()) p.disk = d->second;   // '' in a row from before the column
        p.gcc_label = r.at("gcc_label");
        p.clang_label = r.at("clang_label");
        out.push_back(std::move(p));
    }
    return out;
}

void write_compiler_pins(const fs::path& source_dir, const std::vector<CompilerPinRow>& rows) {
    using namespace jac313::jText::v001;
    const fs::path path = compiler_pin_path(source_dir);
    std::ostringstream out;
    out << "//File:    compilers.pin.jtext\n"
        << "//Date:    " << std::format("{:%Y-%m-%d}", std::chrono::floor<std::chrono::days>(
                                                         std::chrono::system_clock::now())) << "\n"
        << "//Purpose: The committed per-machine compiler pin (v001). One row per machine, keyed on the\n"
        << "//         hardware identity (cpu_model / cores / ram_gb / disk), labeled jac313-###; os_pretty is\n"
        << "//         information only (the OS the row was written under). bootstrap find-or-creates a\n"
        << "//         row; the chosen gcc / clang are then DETERMINISTIC across runs.\n"
        << "//\n"
        << "// Change a row by editing it, or: jac313_test_cli pin --gcc <label> --clang <label>\n"
        << "// (labels are from the toolchains table of Setup/compilers.jtext).\n"
        << "// Written by jac313_test_cli pin through jText's writer.\n"
        << "//\n";
    {
        JTextWriter w(out);
        w.set_profile(JTextProfile::Full);
        w.set_filename("compilers.pin.jtext");
        w.set_purpose("Per-machine compiler pin (v001)");
        w.write_header();
        w.begin_section("pins");
        w.begin_fields();
        const std::vector<std::vector<std::string>> fields = {
            {"id", "Integer", "Not Null"},          {"label", "String", "Not Null", "Not Empty"},
            {"cpu_model", "String", "Not Null"},    {"cores", "Integer", "Not Null"},
            {"ram_gb", "Integer", "Not Null"},      {"disk", "String", "Not Null"},
            {"os_pretty", "String", "Not Null"},
            {"gcc_label", "String", "Not Null"},    {"clang_label", "String", "Not Null"}};
        for (std::size_t f = 0; f < fields.size(); ++f) {
            JTextEntry e;
            e.number = f + 1;
            e.delimiter = U'#';
            e.level_sep = U'/';
            for (const auto& level : fields[f]) e.fields.emplace_back(level);
            w.append_entry(e);
        }
        w.begin_data();
        for (std::size_t n = 0; n < rows.size(); ++n) {
            const CompilerPinRow& r = rows[n];
            w.append_row(1, {std::to_string(n + 1), r.label, r.cpu, std::to_string(r.cores),
                             std::to_string(r.ram_gb), r.disk, r.os, r.gcc_label, r.clang_label});
            w.end_record();
        }
        w.finalize();
    }
    // Write beside it, then rename over it: a reader never sees half a file.
    const fs::path tmp = path.string() + ".tmp";
    {
        std::ofstream f(tmp, std::ios::trunc);
        f << out.str();
        if (!f.flush()) throw std::runtime_error("cannot write " + tmp.string());
    }
    fs::rename(tmp, path);
}

std::vector<TestLimit> load_test_limits(const fs::path& path) {
    const SettingsTable t = read_jtext_table(path, "test_control");
    require_columns(t, path, "test_control", {"test", "timeout_sec", "memory_mb"});
    std::vector<TestLimit> out;
    for (const auto& r : t.rows)
        out.push_back({r.at("test"), to_int(r, "timeout_sec", path, "test_control"),
                       to_int(r, "memory_mb", path, "test_control")});
    return out;
}

ParamsTable load_params_table(const fs::path& path) {
    const SettingsTable sel = read_jtext_table(path, "selection");
    require_columns(sel, path, "selection", {"name", "include"});
    const SettingsTable set = read_jtext_table(path, "settings");
    require_columns(set, path, "settings", {"key", "value"});
    ParamsTable out;
    for (const auto& r : sel.rows) out.selection.emplace_back(r.at("name"), jtext_bool(r.at("include")));
    for (const auto& r : set.rows) out.settings.emplace_back(r.at("key"), r.at("value"));
    return out;
}

std::string ProductionSettings::spec() const {
    return "keeper=" + keeper_sink + ",database=" + database_sink + ",keeper_pct=" + std::to_string(keeper_pct)
         + ",database_pct=" + std::to_string(database_pct) + ",binary=" + (binary ? "on" : "off");
}

ProductionSettings load_production_settings(const fs::path& path) {
    const SettingsTable t = read_jtext_table(path, "production");
    require_columns(t, path, "production", {"keeper_sink", "database_sink", "keeper_pct", "database_pct", "binary"});
    if (t.rows.size() != 1)
        throw std::runtime_error(path.string() + ": section production must have exactly one row, has "
                                 + std::to_string(t.rows.size()));
    const auto& r = t.rows.front();
    ProductionSettings out;
    out.keeper_sink = r.at("keeper_sink");
    out.database_sink = r.at("database_sink");
    out.keeper_pct = to_int(r, "keeper_pct", path, "production");
    out.database_pct = to_int(r, "database_pct", path, "production");
    out.binary = jtext_bool(r.at("binary"));
    auto bad = [&](const std::string& what) {
        throw std::runtime_error(path.string() + ": section production: " + what);
    };
    if (out.keeper_sink != "jtext" && out.keeper_sink != "html" && out.keeper_sink != "json")
        bad("keeper_sink must be jtext | html | json, not '" + out.keeper_sink + "'");
    if (out.database_sink != "sql" && out.database_sink != "pg-local" && out.database_sink != "pg-remote")
        bad("database_sink must be sql | pg-local | pg-remote, not '" + out.database_sink + "'");
    if (out.keeper_pct < 0 || out.keeper_pct > 100) bad("keeper_pct must be 0..100");
    if (out.database_pct < 0 || out.database_pct > 100) bad("database_pct must be 0..100");
    return out;
}

} // namespace jac313::test_cli
