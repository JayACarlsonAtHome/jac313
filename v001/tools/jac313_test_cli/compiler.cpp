#include "compiler.hpp"
#include "settings.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string_view>
#include <sys/utsname.h>
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


// The PATH entries, in order.
std::vector<fs::path> path_dirs() {
    std::vector<fs::path> dirs;
    if (const char* path_env = std::getenv("PATH")) {
        std::string_view rest(path_env);
        while (true) {
            const auto sep = rest.find(':');
            if (const std::string_view entry = rest.substr(0, sep); !entry.empty()) dirs.emplace_back(std::string(entry));
            if (sep == std::string_view::npos) break;
            rest.remove_prefix(sep + 1);
        }
    }
    return dirs;
}

// "16.2.1" -> {16, 2, 1}: the parts compare as numbers.
std::vector<int> version_parts(const std::string& version) {
    std::vector<int> out;
    std::size_t pos = 0;
    while (pos < version.size()) {
        const std::size_t dot = version.find('.', pos);
        out.push_back(version_major_of(version.substr(pos, dot - pos)));
        if (dot == std::string::npos) break;
        pos = dot + 1;
    }
    return out;
}

// `dir`/<base> and `dir`/<base>-<digit>... (sorted), as the shell globs base and base-[0-9]*.
void add_named(std::vector<fs::path>& out, const fs::path& dir, const std::string& base) {
    if (is_executable(dir / base)) out.push_back(dir / base);
    std::vector<fs::path> numbered;
    std::error_code ec;
    for (fs::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec)) {
        const std::string name = it->path().filename().string();
        if (name.size() > base.size() + 1 && name.starts_with(base + "-")
            && name[base.size() + 1] >= '0' && name[base.size() + 1] <= '9' && is_executable(it->path()))
            numbered.push_back(it->path());
    }
    std::ranges::sort(numbered);
    out.insert(out.end(), numbered.begin(), numbered.end());
}

std::string today_local() {
    const std::time_t now = std::time(nullptr);
    std::tm tm{};
    ::localtime_r(&now, &tm);
    char buf[16];
    return std::strftime(buf, sizeof buf, "%Y-%m-%d", &tm) == 0 ? std::string{} : std::string(buf);
}

// Yes from the terminal (/dev/tty); false when there is none.
bool ask_on_terminal(const std::string& question) {
    std::FILE* tty = std::fopen("/dev/tty", "r+");
    if (tty == nullptr) return false;
    std::fprintf(tty, "%s [y/N] ", question.c_str());
    std::fflush(tty);
    char line[64] = {};
    const bool read = std::fgets(line, sizeof line, tty) != nullptr;
    std::fclose(tty);
    if (!read) return false;
    std::string ans(line);
    while (!ans.empty() && (ans.back() == '\n' || ans.back() == '\r')) ans.pop_back();
    return ans == "y" || ans == "Y" || ans == "yes" || ans == "Yes" || ans == "YES";
}

// The pinned compiler of `kind` at the pinned major version, made the selection's compiler when the
// selection is that toolchain: the pinned PATH wins over the toolchain's cc_names. Quiet: no file,
// no row or a pinned compiler that is gone leaves the selection as it is (ensure_compiler_pin says so).
void use_pinned_path(CompilerSelection& sel, const std::vector<Toolchain>& registry, const fs::path& version_dir) {
    try {
        const auto rows = load_compiler_pins(compiler_pin_path(version_dir));
        const CompilerPinRow* row = find_compiler_pin(rows, pin_host_name());
        if (row == nullptr) return;
        const bool gcc = sel.kind == "gcc";
        const std::string& version = gcc ? row->gcc_version : row->clang_version;
        const std::string& path = gcc ? row->gcc_path : row->clang_path;
        const int major = version_major_of(version);
        if (toolchain_label_for(registry, sel.kind, major) != sel.label || !is_executable(path)) return;
        if (version_major_of(compiler_full_version(path, sel.kind)) != major) return;
        if (sel.cc_path != path) {
            sel.cc_path = path;
            sel.reason += " (pinned " + path + ")";
        }
    } catch (const std::exception&) {
        // an unreadable pin file: ensure_compiler_pin reports it; the toolchain's own find stands
    }
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

std::optional<std::string> find_on_path(const std::string& name) {
    return find_executable(name);
}

NinjaChoice find_ninja() {
    // Every PATH entry, then ~/.local/bin: a window started by systemd-run --user gets the
    // user manager's PATH, which has no ~/.local/bin, and would otherwise see only the
    // RHEL 9 system ninja 1.10.
    std::vector<fs::path> dirs;
    if (const char* path_env = std::getenv("PATH")) {
        std::string_view rest(path_env);
        while (true) {
            const auto sep = rest.find(':');
            if (const std::string_view entry = rest.substr(0, sep); !entry.empty()) {
                dirs.emplace_back(std::string(entry));
            }
            if (sep == std::string_view::npos) {
                break;
            }
            rest.remove_prefix(sep + 1);
        }
    }
    if (const char* home = std::getenv("HOME"); home != nullptr && *home != '\0') {
        dirs.push_back(fs::path(home) / ".local" / "bin");
    }
    dirs.emplace_back("/usr/bin");

    NinjaChoice first;
    for (const fs::path& dir : dirs) {
        for (const char* name : {"ninja", "ninja-build"}) {
            const fs::path candidate = dir / name;
            if (!is_executable(candidate)) {
                continue;
            }
            const CaptureResult r = run_capture("'" + candidate.string() + "' --version", 10);
            std::string version = r.out.substr(0, r.out.find_first_of("\r\n"));
            int major = 0;
            int minor = 0;
            const bool parsed =
                r.exit_code == 0 && std::sscanf(version.c_str(), "%d.%d", &major, &minor) == 2;
            const bool ok = parsed && (major > kMinNinjaMajor ||
                                       (major == kMinNinjaMajor && minor >= kMinNinjaMinor));
            if (ok) {
                return NinjaChoice{candidate.string(), version, true};
            }
            if (first.path.empty()) {
                first = NinjaChoice{candidate.string(), version, false};
            }
        }
    }
    return first;
}

std::vector<Toolchain> load_registry(const fs::path& source_dir) {
    return load_toolchains(source_dir, jac313::Setup::v001::sense_host().os_family);
}

std::vector<ResolvedToolchain> probe_compilers(const fs::path& source_dir) {
    return jac313::Setup::v001::resolve_registry(load_registry(source_dir));
}

bool write_filecheck_list(const fs::path& source_dir, const std::vector<ResolvedToolchain>& probes) {
    return jac313::Setup::v001::write_filecheck_list(source_dir, probes, load_recipes(source_dir));
}

void print_compiler_probe(const std::vector<ResolvedToolchain>& probes) {
    std::cout << "=== compiler probe ===\n";
    for (const auto& probe : probes) {
        std::cout << "  " << probe.spec.label << "  " << probe.spec.build_dir;
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
        CompilerSelection sel = resolve_explicit(registry, baseline, *request.explicit_compiler);
        use_pinned_path(sel, registry, request.source_dir);
        return sel;
    }

    // Flag-directed selection: map each flag to a registry toolchain.
    const char* flag = nullptr;
    const char* reason = nullptr;
    if (request.prefer_gcc) {
        // --gcc: the newest gcc this machine has
        std::vector<ResolvedToolchain> gccs;
        for (const Toolchain& tc : registry) {
            if (tc.kind == "gcc") gccs.push_back(resolve_toolchain(tc, baseline));
        }
        const ResolvedToolchain* best = pick_auto(gccs);
        if (best == nullptr || !best->available) {
            throw std::runtime_error("no gcc found on this host (required by --gcc)");
        }
        return selection_from(*best, "--gcc (" + best->spec.label + ")");
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

    throw std::runtime_error("no supported compiler found on this host (see Setup/compilers.jtext)");
}

std::string pinned_cmake_version(const std::filesystem::path& source_dir) {
    return pinned_tool_version(source_dir, jac313::Setup::v001::sense_host().os_family, "cmake");
}

std::filesystem::path use_pinned_cmake(const std::filesystem::path& version_dir) {
    if (pinned_cmake_version(version_dir).empty()) return {};
    const auto bin = std::filesystem::weakly_canonical(std::filesystem::absolute(version_dir)).parent_path().parent_path()
                     / "cmake_install" / "bin";
    const char* path = std::getenv("PATH");
    const std::string current = path ? path : "";
    if (current != bin.string() && current.rfind(bin.string() + ":", 0) != 0)
        ::setenv("PATH", (bin.string() + ":" + current).c_str(), 1);
    return bin;
}

std::string compiler_full_version(const std::string& path, const std::string& kind) {
    const CaptureResult r = run_capture("'" + path + "' " + (kind == "gcc" ? "-dumpfullversion -dumpversion" : "-dumpversion")
                                        + " 2>/dev/null", 10);
    if (r.exit_code != 0) return {};
    std::string v = r.out.substr(0, r.out.find_first_of("\r\n"));
    return version_major_of(v) > 0 ? v : std::string{};
}

std::vector<InstalledCompiler> installed_compilers(const std::string& kind) {
    std::vector<fs::path> found;
    for (const fs::path& dir : path_dirs()) add_named(found, dir, kind == "gcc" ? "g++" : "clang++");
    if (kind == "gcc") {
        std::vector<fs::path> toolsets;
        std::error_code ec;
        for (fs::directory_iterator it("/opt/rh", ec), end; !ec && it != end; it.increment(ec))
            if (it->path().filename().string().starts_with("gcc-toolset-")
                && is_executable(it->path() / "root/usr/bin/g++"))
                toolsets.push_back(it->path() / "root/usr/bin/g++");
        std::ranges::sort(toolsets);
        found.insert(found.end(), toolsets.begin(), toolsets.end());
    }
    std::vector<InstalledCompiler> out;
    std::set<fs::path> seen;   // the resolved file only tells duplicates apart; the name is kept as found
    for (const fs::path& c : found) {
        std::error_code ec;
        const fs::path real = fs::canonical(c, ec);
        if (ec || !seen.insert(real).second) continue;
        const std::string v = compiler_full_version(c.string(), kind);
        if (!v.empty()) out.push_back({version_major_of(v), v, c.string()});
    }
    std::ranges::stable_sort(out, [](const InstalledCompiler& a, const InstalledCompiler& b) {
        if (a.major != b.major) return a.major > b.major;
        if (const auto pa = version_parts(a.version), pb = version_parts(b.version); pa != pb) return pa > pb;
        return a.path < b.path;
    });
    return out;
}

std::string pin_host_name() {
    struct utsname u{};
    return ::uname(&u) == 0 ? std::string(u.nodename) : std::string{};
}

std::optional<InstalledCompiler> newest_pinnable(const std::string& kind) {
    const int min = kind == "gcc" ? kMinPinnedGccMajor : kMinPinnedClangMajor;
    for (const auto& c : installed_compilers(kind))
        if (c.major >= min) return c;
    return std::nullopt;
}

namespace {

CompilerPinRow pin_row(const std::string& host, const InstalledCompiler& gcc, const InstalledCompiler& clang) {
    return {host, gcc.version, gcc.path, clang.version, clang.path, today_local()};
}

void write_pin_row(const fs::path& file, const CompilerPinRow& row) {
    auto rows = load_compiler_pins(file);
    set_compiler_pin(rows, row);
    write_compiler_pins(file, rows);
}

std::optional<PinnedCompilers> ensure_compiler_pin_uncached(const fs::path& version_dir) {
    constexpr const char* tag = "[compiler-pin] ";
    PinnedCompilers p;
    p.file = compiler_pin_path(version_dir);
    const std::string host = pin_host_name();
    const auto ng = newest_pinnable("gcc");
    const auto nc = newest_pinnable("clang");
    const auto rows = load_compiler_pins(p.file);
    if (const CompilerPinRow* row = find_compiler_pin(rows, host)) {
        p.row = *row;
    } else {
        if (!ng || !nc) {
            std::cerr << tag << "no GCC " << kMinPinnedGccMajor << "+ and Clang " << kMinPinnedClangMajor
                      << "+ found to pin on " << host << " (bsir.sh installs them)\n";
            return std::nullopt;
        }
        p.row = pin_row(host, *ng, *nc);
        write_pin_row(p.file, p.row);
        std::cerr << tag << host << " pinned: GCC " << ng->version << " (" << ng->path << ") and Clang "
                  << nc->version << " (" << nc->path << ")  (" << p.file.string() << ")\n";
    }
    // a newer major version: ask to upgrade the pin
    const int gmaj = version_major_of(p.row.gcc_version);
    const int cmaj = version_major_of(p.row.clang_version);
    const bool up_g = ng && ng->major > gmaj;
    const bool up_c = nc && nc->major > cmaj;
    if (up_g || up_c) {
        std::string what;
        if (up_g) what = "GCC " + ng->version + " (pinned: GCC " + p.row.gcc_version + ")";
        if (up_c) what += (what.empty() ? "" : ", ") + ("Clang " + nc->version + " (pinned: Clang " + p.row.clang_version + ")");
        // One question per run: the steps this program starts inherit the answer.
        const bool asked_before = std::getenv("JAC313_COMPILER_PIN_ASKED") != nullptr;
        const bool may_ask = !asked_before && ::isatty(STDIN_FILENO) == 1;
        if (may_ask && ask_on_terminal(std::string(tag) + "Newer on " + host + ": " + what + ". Upgrade the pin?")) {
            if (up_g) { p.row.gcc_version = ng->version; p.row.gcc_path = ng->path; }
            if (up_c) { p.row.clang_version = nc->version; p.row.clang_path = nc->path; }
            p.row.pinned = today_local();
            write_pin_row(p.file, p.row);
            std::cerr << tag << "pin upgraded: GCC " << p.row.gcc_version << ", Clang " << p.row.clang_version << "\n";
        } else if (!asked_before) {
            std::cerr << tag << "keeping the pin (GCC " << p.row.gcc_version << ", Clang " << p.row.clang_version
                      << "); newer: " << what << ". Upgrade: jac313_test_cli pin --upgrade\n";
        }
        ::setenv("JAC313_COMPILER_PIN_ASKED", "1", 1);
    }
    // the pinned ones must still be there, at the pinned major version
    for (const bool gcc : {true, false}) {
        const std::string& version = gcc ? p.row.gcc_version : p.row.clang_version;
        const std::string& path = gcc ? p.row.gcc_path : p.row.clang_path;
        const std::string have = compiler_full_version(path, gcc ? "gcc" : "clang");
        if (have.empty() || version_major_of(have) != version_major_of(version)) {
            std::cerr << tag << "pinned " << (gcc ? "GCC " : "Clang ") << version << " (" << path << ") is gone on "
                      << host << (have.empty() ? "" : " (it is " + have + " now)")
                      << ": jac313_test_cli pin --repin  (" << p.file.string() << ")\n";
            return std::nullopt;
        }
    }
    // their toolchains (build dirs, flags) in Setup/compilers.jtext
    const auto registry = load_registry(version_dir);
    p.gcc_label = toolchain_label_for(registry, "gcc", version_major_of(p.row.gcc_version));
    p.clang_label = toolchain_label_for(registry, "clang", version_major_of(p.row.clang_version));
    if (p.gcc_label.empty() || p.clang_label.empty()) {
        std::cerr << tag << "no toolchain in Setup/compilers.jtext for the pinned "
                  << (p.gcc_label.empty() ? "GCC " + p.row.gcc_version : "Clang " + p.row.clang_version)
                  << " (add a toolchains row, or pin another one in " << p.file.string() << ")\n";
        return std::nullopt;
    }
    return p;
}

} // namespace

std::optional<PinnedCompilers> ensure_compiler_pin(const fs::path& version_dir) {
    static std::optional<std::optional<PinnedCompilers>> once;
    if (!once) {
        try {
            once = ensure_compiler_pin_uncached(version_dir);
        } catch (const std::exception& e) {
            std::cerr << "[compiler-pin] " << e.what() << "\n";
            once = std::optional<PinnedCompilers>{};
        }
    }
    return *once;
}

bool repin_newest(const fs::path& version_dir) {
    const auto ng = newest_pinnable("gcc");
    const auto nc = newest_pinnable("clang");
    if (!ng || !nc) {
        std::cerr << "no GCC " << kMinPinnedGccMajor << "+ and Clang " << kMinPinnedClangMajor << "+ found\n";
        return false;
    }
    const fs::path file = compiler_pin_path(version_dir);
    const std::string host = pin_host_name();
    write_pin_row(file, pin_row(host, *ng, *nc));
    std::cout << host << " pinned: GCC " << ng->version << " (" << ng->path << ") and Clang " << nc->version
              << " (" << nc->path << ")  (" << file.string() << ")\n";
    return true;
}

} // namespace jac313::test_cli
