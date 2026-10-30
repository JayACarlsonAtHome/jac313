#pragma once

// settings.hpp — jac313_test_cli's settings, read from jText tables (no ad-hoc file formats):
//
//   Setup/compilers.jtext      toolchains + toolchain_os   the compiler registry, by OS family
//   Setup/recipes.jtext        components + recipe_os      provision recipes, by OS family
//   Setup/compilers.pin.jtext  pins                        this machine's pinned gcc / clang
//   Setup/contracts.jtext      contract_modes              contract modes by OS and toolchain
//   tests/test_limits.jtext    test_control                seed of the results DB testControl table
//   tests/test_params*.jtext   selection + settings        what a matrix run selects (--params)
//
// One reader for all of them: read_jtext_table parses the file with jText's parser and validator
// (jtext::parse_file_structure + jtext::validate) and returns one section as rows of column ->
// value. The typed loaders below build on it. Setup (toolchain sensing) does not link jText: it
// takes the toolchains and recipes loaded here.

#include <jac313/Setup/v001.hpp>

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

// The toolchains of <source_dir>/Setup/compilers.jtext in table order, with the activations and
// required_on of `os_family` (toolchain_os rows, by activation_seq). A family with no rows tries
// every activation listed for the toolchain and requires none.
std::vector<jac313::Setup::v001::Toolchain> load_toolchains(const std::filesystem::path& source_dir,
                                                            const std::string& os_family);

// The recipes of <source_dir>/Setup/recipes.jtext, in table order, each with its install command
// per OS family (recipe_os rows).
std::vector<jac313::Setup::v001::ProvisionRecipe> load_recipes(const std::filesystem::path& source_dir);

// ---- compiler pin (Setup/compilers.pin.jtext): committed, per-machine {gcc, clang} labels ----
struct CompilerPinRow {
    std::string label, cpu, os, gcc_label, clang_label;
    int cores = 0, ram_gb = 0;
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
    std::vector<std::pair<std::string, std::string>> settings;    // key -> value (DISK_TYPE, SIZE, OS_ID)
};
ParamsTable load_params_table(const std::filesystem::path& path);

} // namespace jac313::test_cli
