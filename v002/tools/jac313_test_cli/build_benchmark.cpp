#include "build_benchmark.hpp"
#include "build_pipeline.hpp"
#include "format.hpp"
#include "results_db.hpp"

#include <chrono>
#include <iostream>
#include <optional>

namespace jac313::test_cli {
namespace fs = std::filesystem;

namespace {

int timed_configure_and_build(const GlobalOptions& global,
                              const ConfigureOptions& configure_opts,
                              const BuildOptions& build_opts)
{
    const auto start = std::chrono::steady_clock::now();
    if (run_configure_command(global, configure_opts) != 0) {
        return -1;
    }
    if (run_build_command(global, build_opts) != 0) {
        return -1;
    }
    const auto end = std::chrono::steady_clock::now();
    return static_cast<int>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}

int timed_build_only(const GlobalOptions& global,
                     const BuildOptions& build_opts,
                     const std::vector<std::string>& targets)
{
    const auto start = std::chrono::steady_clock::now();
    if (run_build_targets_command(global, build_opts, targets, false) != 0) {
        return -1;
    }
    const auto end = std::chrono::steady_clock::now();
    return static_cast<int>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}

} // namespace

int run_build_benchmark(const BuildBenchmarkRequest& request)
{
    GlobalOptions global = request.global;
    ConfigureOptions configure_opts = request.configure;
    const fs::path modules_build_dir = global.build_dir;
    const fs::path no_modules_build_dir = nomodules_build_dir(modules_build_dir);

    std::cout << "=== build benchmark ===\n";
    if (request.group_id > 0) {
        std::cout << "Group:    " << format_count(request.group_id) << '\n';
    }
    std::cout << "Compiler: " << request.compiler_label << '\n';
    std::cout << "Modules build dir:    " << modules_build_dir.string() << '\n';
    std::cout << "No-modules build dir: " << no_modules_build_dir.string() << "\n\n";

    wipe_build_tree(modules_build_dir);
    wipe_build_tree(no_modules_build_dir);

    GlobalOptions no_modules_global = global;
    no_modules_global.build_dir = no_modules_build_dir;
    configure_opts.modules = false;
    const int without_modules_ms =
        timed_configure_and_build(no_modules_global, configure_opts, request.build);
    if (without_modules_ms < 0) {
        return 1;
    }

    wipe_build_tree(modules_build_dir);
    global.build_dir = modules_build_dir;
    configure_opts.modules = true;
    if (run_configure_command(global, configure_opts) != 0) {
        return 1;
    }

    const int modules_only_ms =
        timed_build_only(global, request.build, module_library_targets());
    if (modules_only_ms < 0) {
        return 1;
    }

    const int with_modules_ms = timed_build_only(global, request.build, {});
    if (with_modules_ms < 0) {
        return 1;
    }

    // 4. import std variant — in its OWN tree (never reuse the textual-modules cache;
    //    that is the Debug/Release cross-cache trap). gcc only, AND only on the exact pinned
    //    CMake: the native std-module gate is an experimental, version-specific UUID, so
    //    >= 3.30 is the floor but not sufficient — any other version fails to configure and is
    //    recorded as NULL ("skipped"). Clang has no std module here yet, so it is likewise NULL
    //    and the column auto-populates the day clang gains one — no further code change.
    std::optional<int> importstd_modules_only_ms;
    std::optional<int> importstd_with_modules_ms;
    const fs::path importstd_dir = importstd_build_dir(modules_build_dir);
    if (request.configure.compiler_kind == "gcc") {
        std::cout << "\n=== timing import std variant (" << importstd_dir.string() << ") ===\n";
        wipe_build_tree(importstd_dir);
        GlobalOptions istd_global = global;
        istd_global.build_dir = importstd_dir;
        ConfigureOptions istd_cfg = request.configure;  // clean base (configure_opts was mutated above)
        istd_cfg.modules = true;
        istd_cfg.import_std = true;
        if (run_configure_command(istd_global, istd_cfg) == 0) {
            const int mo = timed_build_only(istd_global, request.build, module_library_targets());
            const int wm = (mo >= 0) ? timed_build_only(istd_global, request.build, {}) : -1;
            if (mo >= 0 && wm >= 0) {
                importstd_modules_only_ms = mo;
                importstd_with_modules_ms = wm;
            } else {
                std::cout << "import std build failed; recording no import-std timing.\n";
            }
        } else {
            std::cout << "import std configure failed — the experimental gate UUID requires the "
                         "exact pinned CMake (see CMakeLists.txt; >= 3.30 is necessary but not "
                         "sufficient). Recording no timing.\n";
        }
    } else {
        std::cout << "\nimport std skipped for " << request.compiler_label
                  << " (no std module for this compiler yet).\n";
    }

    std::cout << "\n=== build benchmark summary ===\n";
    std::cout << "1. Straight (no modules):     " << format_count(without_modules_ms) << " ms ("
              << format_duration_human(without_modules_ms) << ")\n";
    std::cout << "2. Modules only:              " << format_count(modules_only_ms) << " ms ("
              << format_duration_human(modules_only_ms) << ")\n";
    std::cout << "3. With modules (rest):       " << format_count(with_modules_ms) << " ms ("
              << format_duration_human(with_modules_ms) << ")\n";
    if (importstd_modules_only_ms && importstd_with_modules_ms) {
        std::cout << "4. Modules only (import std): " << format_count(*importstd_modules_only_ms) << " ms ("
                  << format_duration_human(*importstd_modules_only_ms) << ")\n";
        std::cout << "5. With modules (import std): " << format_count(*importstd_with_modules_ms) << " ms ("
                  << format_duration_human(*importstd_with_modules_ms) << ")\n";
        const int delta = with_modules_ms - *importstd_with_modules_ms;  // + => import std faster
        std::cout << "   import std vs textual:     " << (delta >= 0 ? "-" : "+")
                  << format_count(delta >= 0 ? delta : -delta) << " ms ("
                  << (with_modules_ms > 0 ? (delta * 100 / with_modules_ms) : 0) << "% "
                  << (delta >= 0 ? "faster" : "slower") << ")\n";
    }
    std::cout << '\n';

    if (request.group_id > 0 && !request.db.project_root.empty()) {
        GroupCompilerBuildRecord record;
        record.group_id = request.group_id;
        record.compiler = request.compiler_label;
        record.build_dir = modules_build_dir;
        record.build_dir_no_modules = no_modules_build_dir;
        record.build_without_modules_ms = without_modules_ms;
        record.build_modules_only_ms = modules_only_ms;
        record.build_with_modules_ms = with_modules_ms;
        record.build_modules_only_importstd_ms = importstd_modules_only_ms;
        record.build_with_modules_importstd_ms = importstd_with_modules_ms;
        record_group_compiler_build(request.db, record);
    }

    return 0;
}

} // namespace jac313::test_cli