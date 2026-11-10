#pragma once

// settings.hpp — jac313_test_cli's settings, read from jText tables (no ad-hoc file formats):
//
//   Setup/compilers.jtext      toolchains + toolchain_os   the compiler registry, by OS family
//   Setup/recipes.jtext        components + recipe_os      provision recipes, by OS family
//   Setup/compilers.pin.jtext  pins                        this machine's pinned gcc / clang
//   Setup/contracts.jtext      contract_modes              contract modes by OS and toolchain
//   tests/test_limits.jtext    test_control                seed of the results DB testControl table
//   tests/test_params*.jtext   selection + settings        what a matrix run selects (--params)
//   tests/production.jtext     production                  the bench's production run (--bench;
//                                                          one row for every profile)
//
//   Setup/compilers.local.jtext, Setup/recipes.local.jtext  (gitignored, optional) this machine's
//                                                          row-level overrides of the two above
//
// One reader for all of them: read_jtext_table parses the file with jText's parser and validator
// (jtext::parse_file_structure + jtext::validate) and returns one section as rows of column ->
// value. The typed loaders below build on it. Setup (toolchain sensing) does not link jText: it
// takes the toolchains and recipes loaded here.

#include <jac313/Setup/v002.hpp>

#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace jac313::test_cli {

// One table (one section of a jText file): its column names in field order, and its rows, each
// a map from column name to the value as written (a missing value is "").
struct SettingsTable {
    std::vector<std::string> columns;
    std::vector<std::map<std::string, std::string>> rows;
};

// Read section `section` of the jText file `path`. Throws std::runtime_error, naming the file,
// when it cannot be read, does not parse, has a validation error, or has no such section.
SettingsTable read_jtext_table(const std::filesystem::path& path, std::string_view section);

// Throws std::runtime_error when `table` (section `section` of `path`) lacks one of `columns`.
void require_columns(const SettingsTable& table, const std::filesystem::path& path,
                     std::string_view section, const std::vector<std::string>& columns);

// jText Bool spellings (SPEC 2.4.1): Yes/No, True/False, Y/N, T/F, 1/0, any case.
bool jtext_bool(std::string_view value);

// ---- machine-local overrides: Setup/compilers.local.jtext, Setup/recipes.local.jtext ----
// Gitignored and optional. A local file holds any of the sections of its tracked table (compilers:
// toolchains, toolchain_os; recipes: components, recipe_os): a section it leaves out has no overrides.
// A section it holds has every column of the tracked one, and optionally a Bool column `delete`. Its rows apply by key (toolchains: label; toolchain_os: os +
// toolchain + activation_seq; components: component; recipe_os: os + component), in file order: a
// row whose key the tracked table has REPLACES that row in place, a new key is ADDED at the end, and
// a row with delete = true REMOVES the row with its key (nothing when there is none). Every loader
// below sees the merged rows. No local file = no change; a local file that cannot be read, does not
// validate, lacks a column in a section it holds, or has a column the tracked table lacks is an error
// that names it.

std::filesystem::path compilers_table_path(const std::filesystem::path& source_dir);   // Setup/compilers.jtext
std::filesystem::path recipes_table_path(const std::filesystem::path& source_dir);     // Setup/recipes.jtext
// The local override file of a tracked table: Setup/compilers.jtext -> Setup/compilers.local.jtext.
std::filesystem::path local_override_path(const std::filesystem::path& tracked);
// "local overrides: <file> (N rows)" when the local file of `tracked` (compilers or recipes) exists,
// else "". Throws as the loaders do when that file is malformed.
std::string local_override_note(const std::filesystem::path& tracked);

// The toolchains of <source_dir>/Setup/compilers.jtext in table order, with the activations and
// required_on of `os_family` (toolchain_os rows, by activation_seq). A family with no rows tries
// every activation listed for the toolchain and requires none.
std::vector<jac313::Setup::v002::Toolchain> load_toolchains(const std::filesystem::path& source_dir,
                                                            const std::string& os_family);

// The recipes of <source_dir>/Setup/recipes.jtext, in table order, each with its install command
// per OS family (recipe_os rows).
std::vector<jac313::Setup::v002::ProvisionRecipe> load_recipes(const std::filesystem::path& source_dir);
// The version of `tool` this OS family must use (pinned_tools in Setup/recipes.jtext); '' = no pin.
std::string pinned_tool_version(const std::filesystem::path& source_dir, const std::string& os_family,
                                const std::string& tool);

// ---- compiler pin (Setup/compilers.pin.jtext): committed, per-machine {gcc, clang} labels ----
// A row is keyed on the machine (cpu, cores, ram_gb, disk); os is information only. A row written
// before the disk column ('' disk) matches the machine on cpu, cores and ram_gb.
struct CompilerPinRow {
    std::string label, cpu, os, gcc_label, clang_label;
    int cores = 0, ram_gb = 0;
    std::string disk;
};
std::filesystem::path compiler_pin_path(const std::filesystem::path& source_dir);
// The pins, in table order; none when the file does not exist.
std::vector<CompilerPinRow> load_compiler_pins(const std::filesystem::path& source_dir);
// Rewrite the file with `rows`, through jText's writer (JTextWriter).
void write_compiler_pins(const std::filesystem::path& source_dir, const std::vector<CompilerPinRow>& rows);

// ---- tests/test_limits.jtext: per-test timeout and memory limit ----
struct TestLimit {
    std::string test;
    int timeout_sec = 0;
    int memory_mb = 0;   // 0 = no limit
};
std::vector<TestLimit> load_test_limits(const std::filesystem::path& path);

// ---- tests/test_params*.jtext: a matrix run's selection and settings ----
struct ParamsTable {
    std::vector<std::pair<std::string, bool>> selection;          // name -> include
    std::vector<std::pair<std::string, std::string>> settings;    // key -> value (DISK_TYPE, SIZE)
};
ParamsTable load_params_table(const std::filesystem::path& path);

// ---- tests/production.jtext, section production: the bench's production run ----
// One row: the keeper sink (jtext | html | json), the database sink (sql | pg-local | pg-remote),
// the % of events flagged KeeperRecord / DatabaseEntry, and the binary replay log on or off.
struct ProductionSettings {
    std::string keeper_sink = "jtext";
    std::string database_sink = "sql";
    int keeper_pct = 10;
    int database_pct = 10;
    bool binary = true;
    // store_bench's --production value: "keeper=jtext,database=sql,keeper_pct=10,database_pct=10,binary=on"
    [[nodiscard]] std::string spec() const;
};
// Throws std::runtime_error (naming the file) when the section is missing, has not exactly one
// row, or holds a value outside the choices above.
ProductionSettings load_production_settings(const std::filesystem::path& path);

} // namespace jac313::test_cli
