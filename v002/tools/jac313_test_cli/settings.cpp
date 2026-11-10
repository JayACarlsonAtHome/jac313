#include "settings.hpp"

#include "parser/section.hpp"
#include "validator/validator.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <system_error>

namespace jac313::test_cli {
namespace fs = std::filesystem;
namespace setup = jac313::Setup::v002;

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

std::optional<SettingsTable> find_jtext_table(const fs::path& path, std::string_view section);

// Section `section` of the local file `local`, checked against the tracked table's columns; none when
// the local file leaves that section out (no overrides for it: the tracked rows stand as they are).
std::optional<SettingsTable> read_local_section(const fs::path& local, std::string_view section,
                                                const SettingsTable& tracked, const fs::path& tracked_path) {
    std::optional<SettingsTable> loc = find_jtext_table(local, section);
    if (!loc) return std::nullopt;
    require_columns(*loc, local, section, tracked.columns);
    for (const auto& c : loc->columns)
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
    const std::optional<SettingsTable> local_rows = read_local_section(local, section, table, path);
    if (!local_rows) return table;
    const SettingsTable& loc = *local_rows;
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
        if (const auto loc = read_local_section(local, s.section, read_jtext_table(tracked, s.section), tracked))
            rows += loc->rows.size();
    return "local overrides: " + local.string() + " (" + std::to_string(rows) + (rows == 1 ? " row)" : " rows)");
}

SettingsTable read_jtext_table(const fs::path& path, std::string_view section) {
    if (std::optional<SettingsTable> table = find_jtext_table(path, section)) return std::move(*table);
    throw std::runtime_error(path.string() + ": no section " + std::string(section));
}

namespace {

// Section `section` of `path`, or none when the file has no such section. A file that cannot be read
// or does not validate is an error either way.
std::optional<SettingsTable> find_jtext_table(const fs::path& path, std::string_view section) {
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
    return std::nullopt;
}

} // namespace

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

fs::path compiler_pin_path(const fs::path& version_dir) {
    if (const char* e = std::getenv("COMPILER_PINS"); e != nullptr && *e != '\0') return fs::path(e);
    std::error_code ec;
    fs::path v = fs::weakly_canonical(fs::absolute(version_dir, ec), ec);
    if (v.filename().empty()) v = v.parent_path();
    // <base>/<repository>/<vNNN>: the pins folder sits beside the repository
    return v.parent_path().parent_path() / "Compiler_Pins" / "compilers.pin.jtext";
}

std::vector<CompilerPinRow> load_compiler_pins(const fs::path& file) {
    std::error_code ec;
    if (!fs::exists(file, ec)) return {};
    std::ifstream in(file);
    if (!in) throw std::runtime_error("cannot read " + file.string());
    std::vector<CompilerPinRow> out;
    int line_no = 0;
    for (std::string line; std::getline(in, line);) {
        ++line_no;
        // a data line: " 1. #|# id|host|gcc_version|gcc_path|clang_version|clang_path|pinned"
        const std::size_t start = line.find_first_not_of(' ');
        if (start == std::string::npos || line.compare(start, 7, "1. #|# ") != 0) continue;
        std::vector<std::string> f;
        std::string_view rest = std::string_view(line).substr(start + 7);
        for (std::size_t bar; (bar = rest.find('|')) != std::string_view::npos; rest.remove_prefix(bar + 1))
            f.emplace_back(rest.substr(0, bar));
        f.emplace_back(rest);
        if (f.size() != 7)
            throw std::runtime_error(file.string() + ":" + std::to_string(line_no) + ": a pin row has 7 fields "
                                     "(id|host|gcc_version|gcc_path|clang_version|clang_path|pinned), this one "
                                     + std::to_string(f.size()));
        out.push_back({f[1], f[2], f[3], f[4], f[5], f[6]});
    }
    return out;
}

void write_compiler_pins(const fs::path& file, const std::vector<CompilerPinRow>& rows) {
    std::ostringstream out;
    // Exactly what tools/hooks/compiler-pin.sh writes, so either one can rewrite the file.
    out << "//File:    compilers.pin.jtext\n"
           "//Purpose: The GCC and Clang each machine builds with, pinned. Outside every repository (like\n"
           "//         SQL_Secrets), so removing the git folder starts over. One row per machine (hostname).\n"
           "//         Written by tools/hooks/compiler-pin.sh (Qlite, jText, jText_GUI). Values never contain '|'.\n"
           "//\n"
           "=== jText File ===\n"
           " 1. #?# compilers.pin\n"
           " 3. #?# Pinned compilers per machine\n"
           "\n\n"
           "=== Section: pins ===\n"
           "\n\n"
           "=== Fields ===\n"
           " 1. #/# id/Identity(1,1)/Not Null\n"
           " 2. #/# host/String/Not Null          # uname -n\n"
           " 3. #/# gcc_version/String/Not Null   # when pinned; the major version is what is held\n"
           " 4. #/# gcc_path/String/Not Null\n"
           " 5. #/# clang_version/String/Not Null\n"
           " 6. #/# clang_path/String/Not Null\n"
           " 7. #/# pinned/String/Not Null        # date of the last pin or upgrade\n"
           "=== End Fields ===\n"
           "\n\n"
           "=== Data ===\n";
    int id = 0;
    for (const CompilerPinRow& r : rows) {
        for (const std::string* v : {&r.host, &r.gcc_version, &r.gcc_path, &r.clang_version, &r.clang_path, &r.pinned})
            if (v->find_first_of("|\n") != std::string::npos)
                throw std::runtime_error("a compiler pin value holds '|' or a line break: " + *v);
        out << "1. #|# " << ++id << '|' << r.host << '|' << r.gcc_version << '|' << r.gcc_path << '|'
            << r.clang_version << '|' << r.clang_path << '|' << r.pinned << "\n\n";
    }
    out << "=== End Data ===\n=== End Section ===\n=== End File ===\n";
    std::error_code ec;
    fs::create_directories(file.parent_path(), ec);
    // Write beside it, then rename over it: a reader never sees half a file.
    const fs::path tmp = file.string() + ".tmp";
    {
        std::ofstream f(tmp, std::ios::trunc);
        f << out.str();
        if (!f.flush()) throw std::runtime_error("cannot write " + tmp.string());
    }
    fs::rename(tmp, file);
}

const CompilerPinRow* find_compiler_pin(const std::vector<CompilerPinRow>& rows, const std::string& host) {
    for (const auto& r : rows)
        if (r.host == host) return &r;
    return nullptr;
}

void set_compiler_pin(std::vector<CompilerPinRow>& rows, const CompilerPinRow& row) {
    std::erase_if(rows, [&](const CompilerPinRow& r) { return r.host == row.host; });
    rows.push_back(row);
}

int version_major_of(const std::string& version) {
    int major = 0;
    const auto [end, ec] = std::from_chars(version.data(), version.data() + version.size(), major);
    return ec == std::errc{} ? major : 0;
}

std::string toolchain_label_for(const std::vector<setup::Toolchain>& toolchains, const std::string& kind, int major) {
    for (const auto& tc : toolchains)
        if (tc.kind == kind && !tc.min_version && tc.version_major == major) return tc.label;
    for (const auto& tc : toolchains)
        if (tc.kind == kind && tc.min_version && major >= *tc.min_version) return tc.label;
    return {};
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
