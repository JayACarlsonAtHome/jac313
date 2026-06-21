#include "compiler.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <unistd.h>

namespace jac313::test_cli {
namespace fs = std::filesystem;

namespace {

bool is_executable(const fs::path& path) {
    return fs::exists(path) && ::access(path.c_str(), X_OK) == 0;
}

std::optional<std::string> find_executable(const std::string& name) {
    if (name.find('/') != std::string::npos) {
        const fs::path direct(name);
        return is_executable(direct) ? std::optional<std::string>{direct.string()}
                                     : std::nullopt;
    }
    if (const char* path_env = std::getenv("PATH")) {
        std::string_view rest(path_env);
        while (!rest.empty()) {
            const auto sep = rest.find(':');
            const std::string_view entry = rest.substr(0, sep);
            if (!entry.empty()) {
                const fs::path candidate = fs::path(std::string(entry)) / name;
                if (is_executable(candidate)) {
                    return candidate.string();
                }
            }
            if (sep == std::string_view::npos) {
                break;
            }
            rest.remove_prefix(sep + 1);
        }
    }
    const fs::path fallback = fs::path("/usr/bin") / name;
    return is_executable(fallback) ? std::optional<std::string>{fallback.string()}
                                   : std::nullopt;
}

const ResolvedToolchain* find_resolved(const std::vector<ResolvedToolchain>& probes,
                                       const std::string& label) {
    for (const auto& probe : probes) {
        if (probe.spec.label == label) {
            return &probe;
        }
    }
    return nullptr;
}

CompilerSelection selection_from(const ResolvedToolchain& rt, std::string reason) {
    CompilerSelection sel;
    sel.label = rt.spec.label;
    sel.cc_path = rt.cc_path;
    sel.kind = rt.spec.kind;
    sel.build_dir = rt.spec.build_dir.empty() ? ("build-" + rt.spec.label)
                                              : rt.spec.build_dir;
    sel.version_line = rt.version_line;
    sel.activation = rt.matched_activation;
    sel.scan_deps = rt.spec.scan_deps;
    sel.env_delta = rt.env_delta;
    sel.reason = std::move(reason);
    return sel;
}

// Pick the auto-default toolchain: highest-version available gcc, else first available.
const ResolvedToolchain* pick_auto(const std::vector<ResolvedToolchain>& probes) {
    const ResolvedToolchain* best_gcc = nullptr;
    for (const auto& probe : probes) {
        if (!probe.available || probe.spec.kind != "gcc") {
            continue;
        }
        if (best_gcc == nullptr
            || probe.spec.version_major > best_gcc->spec.version_major) {
            best_gcc = &probe;
        }
    }
    if (best_gcc != nullptr) {
        return best_gcc;
    }
    for (const auto& probe : probes) {
        if (probe.available) {
            return &probe;
        }
    }
    return nullptr;
}

// Resolve an explicit --compiler value: a registry label, a known flag, or an
// ad-hoc name/path (version-agnostic, direct activation).
CompilerSelection resolve_explicit(const std::vector<Toolchain>& registry,
                                   const EnvSnapshot& baseline,
                                   const std::string& value) {
    if (const Toolchain* tc = find_toolchain_by_label(registry, value)) {
        const ResolvedToolchain rt = resolve_toolchain(*tc, baseline);
        if (rt.available) {
            return selection_from(rt, "explicit --compiler");
        }
        throw std::runtime_error("compiler not available: " + value);
    }

    Toolchain adhoc;
    adhoc.label = value;
    adhoc.kind = (value.find("clang") != std::string::npos) ? "clang" : "gcc";
    adhoc.min_version = 0;  // accept whatever version it reports
    adhoc.cc_names = {value};
    adhoc.activations = {"{CMD}"};
    adhoc.build_dir = "build-custom";
    if (adhoc.kind == "clang") {
        adhoc.scan_deps = {"clang-scan-deps-20", "clang-scan-deps"};
    }
    const ResolvedToolchain rt = resolve_toolchain(adhoc, baseline);
    if (rt.available) {
        return selection_from(rt, "explicit --compiler");
    }
    throw std::runtime_error("compiler not available: " + value);
}

} // namespace

bool is_clang_compiler(const std::string& compiler) {
    return compiler.find("clang") != std::string::npos;
}

std::optional<std::string> find_scan_deps(const std::vector<std::string>& candidates) {
    for (const std::string& name : candidates) {
        if (const auto path = find_executable(name)) {
            return path;
        }
    }
    return std::nullopt;
}

std::vector<ResolvedToolchain> probe_compilers(const fs::path& source_dir) {
    return resolve_registry(source_dir);
}

void print_compiler_probe(const std::vector<ResolvedToolchain>& probes) {
    std::cout << "=== compiler probe ===\n";
    for (const auto& probe : probes) {
        std::cout << "  " << probe.spec.label;
        if (probe.available) {
            std::cout << "  " << probe.cc_path;
            if (!probe.version_line.empty()) {
                std::cout << "  " << probe.version_line;
            }
            if (!probe.matched_activation.empty() && probe.matched_activation != "{CMD}") {
                std::cout << "  [via " << probe.matched_activation << "]";
            }
            std::cout << "  [available]";
        } else {
            std::cout << "  [not found]";
        }
        std::cout << '\n';
    }
}

CompilerSelection resolve_compiler(const CompilerResolveRequest& request) {
    const std::vector<Toolchain> registry = load_registry(request.source_dir);
    const EnvSnapshot baseline = snapshot_env();

    if (request.explicit_compiler) {
        return resolve_explicit(registry, baseline, *request.explicit_compiler);
    }

    // Flag-directed selection: map each flag to a registry toolchain.
    const char* flag = nullptr;
    const char* reason = nullptr;
    if (request.prefer_gcc15) {
        flag = "--gcc15";
        reason = "--gcc15";
    } else if (request.prefer_clang) {
        flag = "--clang";
        reason = "--clang";
    }

    if (flag != nullptr) {
        const Toolchain* tc = find_toolchain_by_flag(registry, flag);
        if (tc == nullptr) {
            throw std::runtime_error(std::string("no registry toolchain for ") + flag);
        }
        const ResolvedToolchain rt = resolve_toolchain(*tc, baseline);
        if (!rt.available) {
            throw std::runtime_error(tc->label + " not available (required by " + flag + ")");
        }
        return selection_from(rt, reason);
    }

    // Auto: resolve all, prefer highest-version gcc, else first available.
    std::vector<ResolvedToolchain> probes;
    probes.reserve(registry.size());
    for (const Toolchain& tc : registry) {
        probes.push_back(resolve_toolchain(tc, baseline));
    }
    if (const ResolvedToolchain* chosen = pick_auto(probes)) {
        return selection_from(*chosen, "auto (" + chosen->spec.label + ")");
    }

    throw std::runtime_error("no supported compiler found on this host (see compilers.conf)");
}

} // namespace jac313::test_cli
