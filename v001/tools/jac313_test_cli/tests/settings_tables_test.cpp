// The settings tables jac313_test_cli and bsir.sh read (Setup/*.jtext, tests/*.jtext): each one
// reads through settings.cpp, validates with jText's validator without errors, has the expected
// columns, and agrees with the others: every recipe has an install command on every OS family,
// every toolchain has an activation on every OS family, and every toolchain a recipe is `for`
// exists. The pin writer's output reads back the same.

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
    const fs::path pins = kSource / "Setup" / "compilers.pin.jtext";
    const fs::path limits = kSource / "tests" / "test_limits.jtext";
    const std::vector<fs::path> params = {kSource / "tests" / "test_params.jtext",
                                          kSource / "tests" / "test_params_full.jtext",
                                          kSource / "tests" / "test_params_bench.jtext"};
    const fs::path production = kSource / "tests" / "production.jtext";

    for (const auto& p : {compilers, recipes, pins, limits, production}) check_validates(p);
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

    // ---- compilers.pin.jtext ----
    table(pins, "pins", {"id", "label", "cpu_model", "cores", "ram_gb", "os_pretty", "gcc_label", "clang_label"});
    try {
        for (const auto& p : load_compiler_pins(kSource)) {
            check(labels.count(p.gcc_label) == 1, "pin " + p.label + " gcc is a toolchain: " + p.gcc_label);
            check(labels.count(p.clang_label) == 1, "pin " + p.label + " clang is a toolchain: " + p.clang_label);
        }
        // the writer's output reads back as written
        const fs::path tmp = fs::temp_directory_path() / ("jac313_settings_test_" + std::to_string(::getpid()));
        fs::create_directories(tmp / "Setup");
        const std::vector<CompilerPinRow> rows = {
            {"jac313-001", "Intel(R) Core(TM) Ultra 7 265", "Fedora Linux 44 (Workstation Edition)", "gcc16", "clang", 20, 62},
            {"jac313-002", "CPU with | ^ ~ # and ' in it", "Linux Mint 22.3", "gcc15", "clang", 8, 31}};
        write_compiler_pins(tmp, rows);
        check_validates(compiler_pin_path(tmp));
        const auto back = load_compiler_pins(tmp);
        check(back.size() == rows.size(), "pins read back: count");
        for (std::size_t i = 0; i < std::min(back.size(), rows.size()); ++i)
            check(back[i].label == rows[i].label && back[i].cpu == rows[i].cpu && back[i].os == rows[i].os
                      && back[i].gcc_label == rows[i].gcc_label && back[i].clang_label == rows[i].clang_label
                      && back[i].cores == rows[i].cores && back[i].ram_gb == rows[i].ram_gb,
                  "pin row " + std::to_string(i + 1) + " reads back as written");
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
