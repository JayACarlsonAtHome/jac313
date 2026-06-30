#include "build_pipeline.hpp"
#include "compiler.hpp"
#include "process.hpp"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <vector>

namespace jac313::test_cli {
namespace fs = std::filesystem;

void wipe_build_tree(const fs::path& build_dir) {
    if (fs::exists(build_dir)) {
        fs::remove_all(build_dir);
    }
    fs::create_directories(build_dir);
}

fs::path nomodules_build_dir(const fs::path& modules_build_dir) {
    const std::string suffix = "-nomodules";
    const std::string name = modules_build_dir.filename().string();
    if (name.ends_with(suffix)) {
        return modules_build_dir;
    }
    return modules_build_dir.parent_path() / (name + suffix);
}

fs::path importstd_build_dir(const fs::path& modules_build_dir) {
    const std::string suffix = "-importstd";
    const std::string name = modules_build_dir.filename().string();
    if (name.ends_with(suffix)) {
        return modules_build_dir;
    }
    return modules_build_dir.parent_path() / (name + suffix);
}

const std::vector<std::string>& module_library_targets() {
    static const std::vector<std::string> targets = {
        "jac313_qlite_module",
        "jac313_jtext_module",
        "jac313_store_module",
    };
    return targets;
}

int run_configure_command(const GlobalOptions& global, const ConfigureOptions& opts) {
    if (!fs::exists(global.source_dir / "CMakeLists.txt")) {
        std::cerr << "Source dir does not look like jac313: " << global.source_dir << '\n';
        return 1;
    }

    fs::create_directories(global.build_dir);

    std::vector<std::string> args = {
        "cmake",
        "-G", opts.generator,
        "-S", global.source_dir.string(),
        "-B", global.build_dir.string(),
        "-DCMAKE_CXX_COMPILER=" + opts.resolved_compiler,
        "-DCMAKE_BUILD_TYPE=" + opts.build_type,
    };
    if (opts.modules) {
        args.push_back("-DJAC313_BUILD_MODULES=ON");
        if (opts.import_std) {
            // Flip every module-native (import-std) library so the benchmark measures
            // the full converted surface, not just one. Add new ones here as they convert.
            args.push_back("-DJAC313_QLITE_IMPORT_STD=ON");
            args.push_back("-DJAC313_JTEXT_IMPORT_STD=ON");
            args.push_back("-DJAC313_STORE_IMPORT_STD=ON");
        }
        if (opts.compiler_kind == "clang") {
            const auto scan_deps = find_scan_deps(opts.scan_deps_candidates);
            if (!scan_deps) {
                std::cerr << "modules require clang-scan-deps (need clang-scan-deps-"
                          << kMinClangMajorVersion << " on PATH for "
                          << opts.resolved_compiler << ")\n";
                return 1;
            }
            args.push_back("-DCMAKE_CXX_COMPILER_CLANG_SCAN_DEPS=" + *scan_deps);
        }
    }
    if (opts.store_tests) {
        args.push_back("-DJAC313_BUILD_STORE_TESTS=ON");
    }
    if (opts.helgrind_annotate) {
        args.push_back("-DJAC313_STORE_HELGRIND_ANNOTATE=ON");
    }

    std::cout << "=== configure"
              << (opts.modules ? " (modules)" : " (no modules)") << " ===\n";
    for (std::size_t i = 0; i < args.size(); ++i) {
        if (i > 0) {
            std::cout << ' ';
        }
        std::cout << args[i];
    }
    std::cout << "\n\n";

    const auto result = run_process(args, 0, "");
    if (!result.stdout_text.empty()) {
        std::cout << result.stdout_text;
    }
    if (!result.stderr_text.empty()) {
        std::cerr << result.stderr_text;
    }

    if (!result.started || result.exit_code != 0) {
        std::cerr << "configure failed";
        if (result.started) {
            std::cerr << " (exit " << result.exit_code << ')';
        }
        std::cerr << '\n';
        return result.exit_code == -1 ? 1 : result.exit_code;
    }
    return 0;
}

int run_build_targets_command(const GlobalOptions& global,
                              const BuildOptions& opts,
                              const std::vector<std::string>& targets,
                              const bool clean_first,
                              const int failsafe_sec)
{
    if (!fs::exists(global.build_dir / "build.ninja")
        && !fs::exists(global.build_dir / "Makefile")) {
        std::cerr << "Build dir not configured: " << global.build_dir << '\n';
        std::cerr << "Hint: jac313_test_cli configure\n";
        return 1;
    }

    std::vector<std::string> args = {"cmake", "--build", global.build_dir.string()};
    if (opts.jobs > 0) {
        args.push_back("-j");
        args.push_back(std::to_string(opts.jobs));
    }
    if (clean_first) {
        args.emplace_back("--clean-first");
    }
    for (const auto& target : targets) {
        args.emplace_back("--target");
        args.push_back(target);
    }

    std::cout << "=== build";
    if (!targets.empty()) {
        std::cout << " (targets:";
        for (const auto& target : targets) {
            std::cout << ' ' << target;
        }
        std::cout << ')';
    }
    std::cout << " ===\n";

    const auto result = run_process(args, failsafe_sec, "");
    if (!result.stdout_text.empty()) {
        std::cout << result.stdout_text;
    }
    if (!result.stderr_text.empty()) {
        std::cerr << result.stderr_text;
    }

    if (result.failsafe_killed) {
        std::cerr << "build failsafe timeout after " << failsafe_sec << "s\n";
        return 1;
    }

    if (!result.started || result.exit_code != 0) {
        std::cerr << "build failed";
        if (result.started) {
            std::cerr << " (exit " << result.exit_code << ')';
        }
        std::cerr << '\n';
        return result.exit_code == -1 ? 1 : result.exit_code;
    }
    return 0;
}

int run_build_command(const GlobalOptions& global, const BuildOptions& opts) {
    return run_build_targets_command(global, opts, {}, false);
}

int timed_build_targets(const fs::path& build_dir,
                        const std::vector<std::string>& targets,
                        const int jobs,
                        const bool clean_first,
                        const int failsafe_sec)
{
    GlobalOptions global;
    global.build_dir = build_dir;
    BuildOptions build_opts;
    build_opts.jobs = jobs;

    const auto start = std::chrono::steady_clock::now();
    if (run_build_targets_command(global, build_opts, targets, clean_first, failsafe_sec) != 0) {
        return -1;
    }
    const auto end = std::chrono::steady_clock::now();
    return static_cast<int>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}

} // namespace jac313::test_cli