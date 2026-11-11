// The settings tables jac313_test_cli and bsir.sh read (Setup/*.jtext, tests/*.jtext): each one
// reads through settings.cpp, validates with jText's validator without errors, has the expected
// columns, and agrees with the others: every recipe has an install command on every OS family,
// every toolchain has an activation on every OS family, and every toolchain a recipe is `for`
// exists. The compiler pin file (outside the repository, shared with Qlite, jText and jText_GUI)
// is read and written in their helper's exact format; this test writes only a temporary copy. The machine-local override files (the
// Setup/*.local.jtext.example files, used as local files of copies of the tracked tables) replace,
// add and remove rows; a missing one changes nothing and a malformed one is an error naming it.

#include "settings.hpp"

#include "parser/section.hpp"
#include "validator/validator.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <unistd.h>

namespace fs = std::filesystem;
using namespace jac313::test_cli;

namespace {

int failures = 0;

// A failed check says what and where, and counts toward failures.
void check(bool ok, std::string_view what, std::source_location at = std::source_location::current()) {
    if (ok) return;
    std::cerr << "FAIL: " << what << " at " << at.file_name() << ":" << at.line() << "\n";
    ++failures;
}

const fs::path kSource = JAC313_SOURCE_DIR;
const std::vector<std::string> kFamilies = {"rhel", "fedora", "linuxmint"};

// jText's validator on the whole file: no errors.
void check_validates(const fs::path& path) {
    std::ifstream in(path);
    std::stringstream ss;
    ss << in.rdbuf();
    const std::string text = ss.str();
    auto pf = jtext::parse_file_structure(std::string_view(text));
    check(pf.has_value(), path.string() + " parses");
    if (!pf) return;
    const auto vr = jtext::validate(*pf);
    for (const auto& is : vr.report.issues)
        if (is.severity == jtext::issue_severity::error) std::cerr << "  " << path.string() << ": " << is.to_string() << "\n";
    check(vr.report.error_count() == 0, path.string() + " validates without errors");
}

// The section reads through read_jtext_table with exactly these columns, in this order.
SettingsTable table(const fs::path& path, std::string_view section, const std::vector<std::string>& columns) {
    try {
        SettingsTable t = read_jtext_table(path, section);
        check(t.columns == columns, path.string() + " section " + std::string(section) + " has the expected columns");
        return t;
    } catch (const std::exception& e) {
        check(false, std::string("read ") + path.string() + ": " + e.what());
        return {};
    }
}

} // namespace

int main() {
    const fs::path compilers = kSource / "Setup" / "compilers.jtext";
    const fs::path recipes = kSource / "Setup" / "recipes.jtext";
    const fs::path limits = kSource / "tests" / "test_limits.jtext";
    const std::vector<fs::path> params = {kSource / "tests" / "test_params.jtext",
                                          kSource / "tests" / "test_params_full.jtext",
                                          kSource / "tests" / "test_params_bench.jtext"};
    const fs::path production = kSource / "tests" / "production.jtext";

    for (const auto& p : {compilers, recipes, limits, production}) check_validates(p);
    for (const auto& p : params) check_validates(p);

    // ---- compilers.jtext ----
    const auto tcs = table(compilers, "toolchains",
                           {"id", "label", "kind", "version", "min_version", "build_dir", "flag", "cc_names", "scan_deps"});
    const auto tos = table(compilers, "toolchain_os",
                           {"id", "os", "toolchain", "required", "activation_seq", "activation"});
    check(!tcs.rows.empty(), "compilers.jtext has toolchains");
    std::set<std::string> labels;
    for (const auto& r : tcs.rows) labels.insert(r.at("label"));
    for (const auto& label : labels)
        for (const auto& fam : kFamilies)
            check(std::any_of(tos.rows.begin(), tos.rows.end(),
                              [&](const auto& o) { return o.at("toolchain") == label && o.at("os") == fam; }),
                  "toolchain " + label + " has an activation on " + fam);
    for (const auto& o : tos.rows) {
        check(labels.count(o.at("toolchain")) == 1, "toolchain_os names a toolchain: " + o.at("toolchain"));
        check(o.at("activation").find("{CMD}") != std::string::npos, "activation holds {CMD}: " + o.at("activation"));
    }
    // each OS its own limit: toolchain_min_os rows name a toolchain and an OS family
    const auto tmins = table(compilers, "toolchain_min_os", {"id", "os", "toolchain", "min_version"});
    for (const auto& m : tmins.rows) {
        check(labels.count(m.at("toolchain")) == 1, "toolchain_min_os names a toolchain: " + m.at("toolchain"));
        check(std::find(kFamilies.begin(), kFamilies.end(), m.at("os")) != kFamilies.end(),
              "toolchain_min_os names an OS family: " + m.at("os"));
    }
    try {
        check(min_pinnable_major(kSource, "linuxmint", "clang") == 21, "Linux Mint: Clang minimum 21");
        check(min_pinnable_major(kSource, "rhel", "clang") == 20, "RHEL: Clang minimum 20");
        check(min_pinnable_major(kSource, "fedora", "clang") == 20, "Fedora: Clang minimum 20");
        for (const auto& fam : kFamilies)
            check(min_pinnable_major(kSource, fam, "gcc") == 15, "GCC minimum 15 on " + fam);
        check(toolchain_label_for(load_toolchains(kSource, "linuxmint"), "clang", 20).empty(),
              "Linux Mint: Clang 20 is below its minimum");
        check(toolchain_label_for(load_toolchains(kSource, "rhel"), "clang", 20) == "clang",
              "RHEL: Clang 20 is its clang");
    } catch (const std::exception& e) {
        check(false, std::string("per-OS minimums: ") + e.what());
    }
    for (const auto& fam : kFamilies) {
        try {
            const auto loaded = load_toolchains(kSource, fam);
            check(loaded.size() == labels.size(), "load_toolchains(" + fam + ") gives every toolchain");
            for (const auto& tc : loaded) check(!tc.activations.empty() && !tc.cc_names.empty(),
                                                tc.label + " on " + fam + " has activations and cc_names");
        } catch (const std::exception& e) {
            check(false, std::string("load_toolchains: ") + e.what());
        }
    }

    // ---- recipes.jtext ----
    const auto comps = table(recipes, "components", {"id", "component", "desc", "for", "required", "check"});
    const auto ros = table(recipes, "recipe_os", {"id", "os", "component", "command"});
    check(!comps.rows.empty(), "recipes.jtext has components");
    for (const auto& c : comps.rows) {
        const std::string& name = c.at("component");
        for (const auto& fam : kFamilies)
            check(std::count_if(ros.rows.begin(), ros.rows.end(), [&](const auto& o) {
                      return o.at("component") == name && o.at("os") == fam && !o.at("command").empty();
                  }) == 1,
                  "component " + name + " has one command on " + fam);
        if (!c.at("for").empty())
            check(labels.count(c.at("for")) == 1, "component " + name + " is for a toolchain in compilers.jtext: " + c.at("for"));
    }
    try {
        const auto loaded = load_recipes(kSource);
        check(loaded.size() == comps.rows.size(), "load_recipes gives every component");
        for (const auto& r : loaded) check(r.install_by_family.size() == kFamilies.size(), r.component + " has every family");
    } catch (const std::exception& e) {
        check(false, std::string("load_recipes: ") + e.what());
    }

    // ---- machine-local overrides: Setup/compilers.local.jtext, Setup/recipes.local.jtext ----
    {
        const fs::path ex_c = kSource / "Setup" / "compilers.local.jtext.example";
        const fs::path ex_r = kSource / "Setup" / "recipes.local.jtext.example";
        check_validates(ex_c);
        check_validates(ex_r);
        const fs::path tmp = fs::temp_directory_path() / ("jac313_settings_local_test_" + std::to_string(::getpid()));
        fs::create_directories(tmp / "Setup");
        fs::copy_file(compilers, compilers_table_path(tmp));
        fs::copy_file(recipes, recipes_table_path(tmp));
        const auto find_tc = [](const auto& v, const std::string& label) {
            return std::find_if(v.begin(), v.end(), [&](const auto& t) { return t.label == label; });
        };
        const auto find_rec = [](const auto& v, const std::string& name) {
            return std::find_if(v.begin(), v.end(), [&](const auto& r) { return r.component == name; });
        };
        const auto has = [](const std::vector<std::string>& v, const std::string& x) {
            return std::find(v.begin(), v.end(), x) != v.end();
        };
        // the error `load` throws holds `want` (empty: it throws at all)
        const auto throws_naming = [](auto load, const std::string& want) {
            try {
                load();
            } catch (const std::exception& e) {
                return std::string(e.what()).find(want) != std::string::npos;
            }
            return false;
        };
        try {
            // missing: the tracked tables as they are
            check(local_override_path(compilers_table_path(tmp)) == tmp / "Setup" / "compilers.local.jtext",
                  "local_override_path: compilers.jtext -> compilers.local.jtext");
            check(local_override_note(compilers_table_path(tmp)).empty() && local_override_note(recipes_table_path(tmp)).empty(),
                  "no local file: no note");
            const auto base_tc = load_toolchains(tmp, "fedora");
            const auto base_rec = load_recipes(tmp);
            check(base_tc.size() == labels.size() && find_tc(base_tc, "gcc16")->build_dir == "build-gcc16",
                  "no local file: compilers as tracked");
            check(base_rec.size() == comps.rows.size() && find_rec(base_rec, "dbeaver") != base_rec.end(),
                  "no local file: recipes as tracked");

            // the examples: one override, one addition, one removal per table
            fs::copy_file(ex_c, local_override_path(compilers_table_path(tmp)));
            fs::copy_file(ex_r, local_override_path(recipes_table_path(tmp)));
            check(local_override_note(compilers_table_path(tmp))
                      == "local overrides: " + (tmp / "Setup" / "compilers.local.jtext").string() + " (3 rows)",
                  "compilers note: " + local_override_note(compilers_table_path(tmp)));
            check(local_override_note(recipes_table_path(tmp))
                      == "local overrides: " + (tmp / "Setup" / "recipes.local.jtext").string() + " (4 rows)",
                  "recipes note: " + local_override_note(recipes_table_path(tmp)));
            const auto tc = load_toolchains(tmp, "fedora");
            check(tc.size() == base_tc.size() && tc[1].label == "gcc16", "override keeps gcc16 in place");
            const auto g16 = find_tc(tc, "gcc16");
            check(g16->build_dir == "build-gcc16-local", "override: gcc16 build_dir is build-gcc16-local");
            check(g16->activations.size() == find_tc(base_tc, "gcc16")->activations.size() + 1
                      && g16->activations.back() == "bash -c 'source /opt/rh/gcc-toolset-16/enable && {CMD}'",
                  "add: fedora gcc16 has a fourth activation");
            const auto g15 = find_tc(tc, "gcc15");
            check(!has(g15->activations, "scl enable gcc-toolset-15 -- {CMD}")
                      && g15->activations.size() == find_tc(base_tc, "gcc15")->activations.size() - 1,
                  "remove: fedora gcc15 no longer tries scl enable");
            check(has(find_tc(load_toolchains(tmp, "rhel"), "gcc15")->activations, "scl enable gcc-toolset-15 -- {CMD}"),
                  "remove: rhel gcc15 still tries scl enable");
            const auto rec = load_recipes(tmp);
            check(rec.size() == base_rec.size(), "recipes: one added, one removed");
            check(find_rec(rec, "ninja")->install_by_family.at("fedora") == "sudo dnf install -y ninja-build"
                      && find_rec(rec, "ninja")->install_by_family.at("rhel") == find_rec(base_rec, "ninja")->install_by_family.at("rhel"),
                  "override: fedora ninja command");
            check(!rec.empty() && rec.back().component == "tmux" && rec.back().install_by_family.at("fedora") == "sudo dnf install -y tmux",
                  "add: tmux, last, with its fedora command");
            check(find_rec(rec, "dbeaver") == rec.end(), "remove: dbeaver");

            // malformed: errors that name the local file
            const std::string lc = local_override_path(compilers_table_path(tmp)).string();
            const std::string lr = local_override_path(recipes_table_path(tmp)).string();
            const auto write = [](const std::string& path, const std::string& text) { std::ofstream(path, std::ios::trunc) << text; };
            write(lc, "this is not jText\n");
            check(throws_naming([&] { load_toolchains(tmp, "fedora"); }, lc), "unparsable compilers.local.jtext: error names it");
            check(throws_naming([&] { local_override_note(compilers_table_path(tmp)); }, lc), "unparsable: the note names it too");
            std::ifstream in(ex_c);
            std::stringstream ss;
            ss << in.rdbuf();
            std::string text = ss.str();
            text.replace(text.find("/# build_dir/"), 13, "/# build_dirx/");
            write(lc, text);
            check(throws_naming([&] { load_toolchains(tmp, "fedora"); }, lc + " (section toolchains): no column build_dir"),
                  "compilers.local.jtext without build_dir: error names it");
            fs::remove(lc);
            std::ifstream in_r(ex_r);
            std::stringstream ss_r;
            ss_r << in_r.rdbuf();
            text = ss_r.str();
            text.erase(text.find("=== Section: recipe_os ==="), text.find("=== End File ===") - text.find("=== Section: recipe_os ==="));
            write(lr, text);
            try {
                const auto only_components = load_recipes(tmp);
                check(!only_components.empty() && only_components.back().component == "tmux",
                      "recipes.local.jtext without recipe_os: its components rows still apply");
                check(local_override_note(recipes_table_path(tmp)).ends_with(" (2 rows)"),
                      "recipes.local.jtext without recipe_os: the note counts only the rows it has");
            } catch (const std::exception& e) {
                check(false, std::string("recipes.local.jtext without recipe_os must load: ") + e.what());
            }
            fs::remove(lr);
            check(load_toolchains(tmp, "fedora").size() == base_tc.size() && load_recipes(tmp).size() == base_rec.size(),
                  "local files removed: back to the tracked tables");
        } catch (const std::exception& e) {
            check(false, std::string("local overrides: ") + e.what());
        }
        fs::remove_all(tmp);
    }

    // ---- the compiler pin: <base>/Compiler_Pins/compilers.pin.jtext (never the real one here) ----
    try {
        const fs::path tmp = fs::temp_directory_path() / ("jac313_settings_test_" + std::to_string(::getpid()));
        fs::create_directories(tmp);
        // where it is: $COMPILER_PINS, else beside the repository the version folder is in
        const char* saved_env = std::getenv("COMPILER_PINS");
        const std::string saved = saved_env ? saved_env : "";
        ::unsetenv("COMPILER_PINS");
        check(compiler_pin_path(tmp / "repo" / "v001") == tmp / "Compiler_Pins" / "compilers.pin.jtext",
              "pin file: <base>/Compiler_Pins/compilers.pin.jtext");
        const fs::path file = tmp / "pins" / "compilers.pin.jtext";
        ::setenv("COMPILER_PINS", file.c_str(), 1);
        check(compiler_pin_path(tmp / "repo" / "v001") == file, "pin file: $COMPILER_PINS wins");
        if (saved_env) ::setenv("COMPILER_PINS", saved.c_str(), 1); else ::unsetenv("COMPILER_PINS");

        check(load_compiler_pins(file).empty(), "no pin file: no rows");
        // what tools/hooks/compiler-pin.sh writes reads as its rows
        {
            fs::create_directories(file.parent_path());
            std::ofstream(file) << "=== jText File ===\n=== Section: pins ===\n=== Data ===\n"
                                   "1. #|# 1|Other-Box|15.1.0|/opt/rh/gcc-toolset-15/root/usr/bin/g++|21.1.8|/usr/bin/clang++|2026-11-01\n\n"
                                   "1. #|# 2|Fedora-Tower-001|16.2.1|/usr/bin/g++|22.1.8|/usr/bin/clang++|2026-11-10\n\n"
                                   "=== End Data ===\n=== End Section ===\n=== End File ===\n";
            const auto rows = load_compiler_pins(file);
            check(rows.size() == 2, "helper file: two rows");
            const CompilerPinRow* r = find_compiler_pin(rows, "Fedora-Tower-001");
            check(r && r->gcc_version == "16.2.1" && r->gcc_path == "/usr/bin/g++" && r->clang_version == "22.1.8"
                      && r->clang_path == "/usr/bin/clang++" && r->pinned == "2026-11-10",
                  "helper file: this host's row");
            check(find_compiler_pin(rows, "Nobody") == nullptr, "no row for another host");
        }
        // writing: this host's row replaced, the other host's kept, the helper's exact lines
        {
            auto rows = load_compiler_pins(file);
            set_compiler_pin(rows, {"Fedora-Tower-001", "17.0.1", "/usr/bin/g++-17", "23.1.0", "/usr/bin/clang++", "2026-11-11"});
            write_compiler_pins(file, rows);
            check_validates(file);
            std::ifstream in(file);
            std::stringstream ss;
            ss << in.rdbuf();
            const std::string text = ss.str();
            check(text.starts_with("//File:    compilers.pin.jtext\n"), "written: the helper's header");
            check(text.find(" 2. #/# host/String/Not Null          # uname -n\n") != std::string::npos, "written: the helper's fields");
            check(text.find("1. #|# 1|Other-Box|15.1.0|/opt/rh/gcc-toolset-15/root/usr/bin/g++|21.1.8|/usr/bin/clang++|2026-11-01\n\n"
                            "1. #|# 2|Fedora-Tower-001|17.0.1|/usr/bin/g++-17|23.1.0|/usr/bin/clang++|2026-11-11\n\n"
                            "=== End Data ===\n") != std::string::npos,
                  "written: other host kept, this host's row last, ids from 1");
            const auto back = load_compiler_pins(file);
            check(back.size() == 2 && back[1].gcc_version == "17.0.1" && back[0].host == "Other-Box", "written: reads back");
            bool refused = false;
            try { write_compiler_pins(file, {{"bad|host", "1", "p", "1", "p", "d"}}); } catch (const std::exception&) { refused = true; }
            check(refused, "a value with '|' is refused");
        }
        // a malformed row is an error naming the file
        {
            std::ofstream(file) << "1. #|# 1|only|three\n";
            bool threw = false;
            try { (void)load_compiler_pins(file); } catch (const std::exception& e) {
                threw = std::string(e.what()).find(file.string()) != std::string::npos;
            }
            check(threw, "a pin row without 7 fields is an error naming the file");
        }
        // the pinned major version picks the toolchain label
        const auto tcs_fedora = load_toolchains(kSource, "fedora");
        check(version_major_of("16.2.1") == 16 && version_major_of("x") == 0, "version_major_of");
        check(toolchain_label_for(tcs_fedora, "gcc", 16) == "gcc16", "GCC 16 -> gcc16");
        check(toolchain_label_for(tcs_fedora, "gcc", 15) == "gcc15", "GCC 15 -> gcc15");
        check(toolchain_label_for(tcs_fedora, "gcc", 99).empty(), "GCC 99 -> no toolchain");
        check(toolchain_label_for(tcs_fedora, "clang", 22) == "clang", "Clang 22 -> clang (min_version)");
        check(toolchain_label_for(tcs_fedora, "clang", 19).empty(), "Clang 19 -> below min_version");
        fs::remove_all(tmp);
    } catch (const std::exception& e) {
        check(false, std::string("compiler pins: ") + e.what());
    }

    // ---- tests/test_limits.jtext ----
    table(limits, "test_control", {"id", "test", "timeout_sec", "memory_mb"});
    try {
        const auto rows = load_test_limits(limits);
        check(!rows.empty(), "test_limits.jtext has rows");
        for (const auto& t : rows) check(t.timeout_sec >= 0 && t.memory_mb >= 0, t.test + " has limits >= 0");
    } catch (const std::exception& e) {
        check(false, std::string("load_test_limits: ") + e.what());
    }

    // ---- tests/test_params*.jtext ----
    for (const auto& p : params) {
        table(p, "selection", {"id", "name", "include"});
        table(p, "settings", {"id", "key", "value"});
        try {
            const auto t = load_params_table(p);
            check(!t.selection.empty(), p.string() + " selects something");
            check(std::any_of(t.settings.begin(), t.settings.end(), [](const auto& kv) { return kv.first == "SIZE"; }),
                  p.string() + " sets SIZE");
        } catch (const std::exception& e) {
            check(false, std::string("load_params_table: ") + e.what());
        }
        // The production row lives in tests/production.jtext (one row for every profile), not here.
        bool has_production = true;
        try {
            (void)read_jtext_table(p, "production");
        } catch (const std::exception&) {
            has_production = false;
        }
        check(!has_production, p.string() + " has no production section (it is in production.jtext)");
    }

    // ---- tests/production.jtext: the bench's production run, one row for every profile ----
    table(production, "production", {"id", "keeper_sink", "database_sink", "keeper_pct", "database_pct", "binary"});
    try {
        const auto prod = load_production_settings(production);
        check(prod.spec() == "keeper=jtext,database=sql,keeper_pct=10,database_pct=10,binary=on",
              production.string() + " production row holds the defaults (jtext, sql, 10, 10, on)");
    } catch (const std::exception& e) {
        check(false, std::string("load_production_settings: ") + e.what());
    }

    if (failures == 0) std::cout << "settings_tables_test: all passed\n";
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
