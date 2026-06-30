// File:    Setup/setup_main.cpp
// Purpose: main() for the prebuilt, committed `jac313_setup` provisioner.
//
// This binary is COMMITTED to the repo (built static so it runs across the fleet's
// distros/glibc versions) and arrives with a clone. bootstrap.sh — the thin shell entry
// point — senses the host and writes a small HANDOFF FILE; this exe reads it and runs
// the install plan with real per-step error handling (captured exit codes, a
// continue-on-failure summary), which a `set -e` shell Setup.sh cannot do.
//
// Handoff file format (written by bootstrap.sh, one directive per line):
//     family=rhel                       # OS family token (matches recipes.conf)
//     activation=scl enable ... --      # informational; how bootstrap reaches the compiler
//     component=ninja                   # a prerequisite to provision (repeatable)
//     component=sqlite
// '#' comments and blank lines are ignored.
//
// Usage:
//   jac313_setup [--source-dir DIR] [--dry-run] [--yes] [--version] [HANDOFF_FILE]
//     --source-dir DIR : repo/version root that holds Setup/recipes.conf (default: sensed)
//     --dry-run        : print the resolved plan and exit; touch nothing
//     --yes            : skip the [y/N] confirmation
//     HANDOFF_FILE     : path to the handoff (default: <source-dir>/Setup/.setup_handoff)
//
// Rebuild + re-commit this binary with Setup/build_setup_exe.sh whenever the Setup
// library or its recipe handling changes.

#include "jac313/Setup/v002.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace setup = jac313::Setup::v002;
namespace fs = std::filesystem;

namespace {

constexpr std::string_view kVersion = "jac313_setup v002 (provisioner)";

std::string trim(std::string s) {
    const auto ws = " \t\r\n";
    const auto b = s.find_first_not_of(ws);
    if (b == std::string::npos) {
        return "";
    }
    const auto e = s.find_last_not_of(ws);
    return s.substr(b, e - b + 1);
}

// Walk up from `start` looking for a directory that contains Setup/recipes.conf.
fs::path detect_source_dir(const fs::path& start) {
    std::error_code ec;
    fs::path dir = fs::absolute(start, ec);
    if (ec) {
        dir = start;
    }
    for (;;) {
        if (fs::exists(dir / "Setup" / "recipes.conf")) {
            return dir;
        }
        const fs::path parent = dir.parent_path();
        if (parent == dir) {
            break; // reached filesystem root
        }
        dir = parent;
    }
    return start;
}

struct Handoff {
    std::string family;
    std::vector<std::string> components;
};

Handoff read_handoff(const fs::path& path, bool& ok) {
    Handoff h;
    std::ifstream in(path);
    ok = static_cast<bool>(in);
    if (!ok) {
        return h;
    }
    std::string line;
    while (std::getline(in, line)) {
        if (const auto hash = line.find('#'); hash != std::string::npos) {
            line = line.substr(0, hash);
        }
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        const auto eq = line.find('=');
        if (eq == std::string::npos) {
            continue;
        }
        const std::string key = trim(line.substr(0, eq));
        const std::string val = trim(line.substr(eq + 1));
        if (key == "family") {
            h.family = val;
        } else if (key == "component") {
            if (!val.empty()) {
                h.components.push_back(val);
            }
        }
        // 'activation' and any other keys are informational — ignored here.
    }
    return h;
}

} // namespace

int main(int argc, char** argv) {
    setup::ExecOptions opts;
    fs::path source_dir;
    fs::path handoff_path;
    bool have_source = false;

    for (int i = 1; i < argc; ++i) {
        const std::string_view a = argv[i];
        if (a == "--version" || a == "-V") {
            std::cout << kVersion << "\n";
            return 0;
        } else if (a == "--help" || a == "-h") {
            std::cout << kVersion << "\n"
                      << "usage: jac313_setup [--source-dir DIR] [--dry-run] [--yes] "
                         "[--version] [HANDOFF_FILE]\n";
            return 0;
        } else if (a == "--dry-run") {
            opts.dry_run = true;
        } else if (a == "--yes" || a == "-y") {
            opts.assume_yes = true;
        } else if (a == "--source-dir") {
            if (i + 1 >= argc) {
                std::cerr << "jac313_setup: --source-dir needs an argument\n";
                return 2;
            }
            source_dir = argv[++i];
            have_source = true;
        } else if (!a.empty() && a.front() == '-') {
            std::cerr << "jac313_setup: unknown option '" << a << "'\n";
            return 2;
        } else {
            handoff_path = std::string(a); // positional: the handoff file
        }
    }

    if (!have_source) {
        source_dir = handoff_path.empty()
                         ? detect_source_dir(fs::current_path())
                         : detect_source_dir(handoff_path.parent_path());
    }
    if (handoff_path.empty()) {
        handoff_path = source_dir / "Setup" / ".setup_handoff";
    }

    bool ok = false;
    const Handoff h = read_handoff(handoff_path, ok);
    if (!ok) {
        std::cerr << "jac313_setup: cannot read handoff file: " << handoff_path << "\n";
        return 2;
    }
    if (h.components.empty()) {
        std::cout << "jac313_setup: handoff lists no components — nothing to do.\n";
        return 0;
    }

    const setup::ProvisionPlan plan =
        setup::plan_for_components(source_dir, h.family, h.components);
    const setup::ExecReport report = setup::execute_plan(plan, opts);

    if (report.declined) {
        return 0; // user chose not to proceed; not an error
    }
    // Unsupported components (no recipe for this family) are a real gap to surface.
    if (report.failed > 0 || !report.unsupported.empty()) {
        return 1;
    }
    return 0;
}
