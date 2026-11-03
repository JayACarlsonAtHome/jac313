#pragma once

#include <jac313/Setup/v002.hpp>

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace jac313::test_cli {

// The toolchain-sensing engine now lives in the standalone jac313::Setup package.
// Re-export the names the test CLI uses so existing call sites stay unqualified.
using jac313::Setup::v002::Toolchain;
using jac313::Setup::v002::ResolvedToolchain;
using jac313::Setup::v002::EnvSnapshot;
using jac313::Setup::v002::CaptureResult;
using jac313::Setup::v002::resolve_toolchain;
using jac313::Setup::v002::find_toolchain_by_label;
using jac313::Setup::v002::find_toolchain_by_flag;
using jac313::Setup::v002::parse_version_major;
using jac313::Setup::v002::scrub_exported_shell_functions;
using jac313::Setup::v002::snapshot_env;
using jac313::Setup::v002::apply_env_delta;
using jac313::Setup::v002::restore_env;

// The chosen toolchain for a build, resolved through the registry.
struct CompilerSelection {
    std::string label;       // gcc15  (results label + build-dir source)
    std::string cc_path;     // absolute compiler path -> CMAKE_CXX_COMPILER
    std::string kind;        // gcc | clang
    std::string build_dir;   // build-gcc15
    std::string version_line;
    std::string activation;  // the activation template that matched
    std::vector<std::string> scan_deps;            // clang-scan-deps candidate names
    std::map<std::string, std::string> env_delta;  // activation env to apply to children
    std::string reason;      // why this toolchain was selected
};

struct CompilerResolveRequest {
    std::filesystem::path source_dir;     // where to load the registry from (Setup/compilers.jtext)
    std::optional<std::string> explicit_compiler;  // --compiler value
    bool prefer_gcc{false};
    bool prefer_clang{false};
};

constexpr int kMinClangMajorVersion = 20;

// The CMake version this OS must use (pinned_tools in Setup/recipes.jtext); '' = the system's.
std::string pinned_cmake_version(const std::filesystem::path& source_dir);
// Puts a pinned CMake first on PATH, so this program and everything it starts use it; returns its
// bin folder, cmake_install beside the repository that <version_dir> is in ('' = no pin).
std::filesystem::path use_pinned_cmake(const std::filesystem::path& version_dir);

// The registry for this host's OS family: the toolchains of <source_dir>/Setup/compilers.jtext.
std::vector<Toolchain> load_registry(const std::filesystem::path& source_dir);

// Sense every registry toolchain on this host (for `compilers` + FileCheckList).
std::vector<ResolvedToolchain> probe_compilers(const std::filesystem::path& source_dir);
void print_compiler_probe(const std::vector<ResolvedToolchain>& probes);

// Write <source_dir>/FileCheckList.txt from `probes`, with the recipes of Setup/recipes.jtext
// (their required checks are part of the readiness verdict).
bool write_filecheck_list(const std::filesystem::path& source_dir,
                          const std::vector<ResolvedToolchain>& probes);

// Resolve the requested toolchain (or throw with a helpful message if unavailable).
CompilerSelection resolve_compiler(const CompilerResolveRequest& request);

// First clang-scan-deps candidate that exists on PATH.
std::optional<std::string> find_scan_deps(const std::vector<std::string>& candidates);

// The program NAME as found on PATH (or NAME itself when it holds a '/').
std::optional<std::string> find_on_path(const std::string& name);

bool is_clang_compiler(const std::string& compiler);

} // namespace jac313::test_cli
