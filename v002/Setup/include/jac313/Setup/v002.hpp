#pragma once

// jac313::Setup::v002 — standalone, data-driven, activation-aware toolchain sensing.
//
// A "toolchain" (gcc15, gcc14, clang, ...) is described entirely by data: how to
// activate it and how to invoke its compiler. Activation is a shell command template
// with a {CMD} placeholder. Every activation mechanism — a plain PATH binary, a
// gcc-toolset env launcher (RHEL 10), SCL (RHEL 9), a sourced enable-script, a future
// Fedora module — reduces to "run something so the compiler becomes reachable in the
// resulting environment." Adding a platform is a few table rows, not code.
//
// This package is self-contained (its own command-runner, no external deps beyond the
// C++23 standard library) so it can be reused by any build tooling, including via the
// C++23 module `import jac313.setup.v002;`.

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace jac313::Setup::v002 {

// ---- command runner (self-contained: run a shell line, capture stdout + exit) ----

struct CaptureResult {
    bool started{false};
    int exit_code{-1};
    std::string out;   // captured stdout (stderr is discarded)
};

// Run `command_line` via /bin/sh -c, capturing stdout. Killed after timeout_sec.
CaptureResult run_capture(const std::string& command_line, int timeout_sec = 30);

// ---- toolchain model (data) ----

struct Toolchain {
    std::string label;                       // gcc15  (drives build-dir + results label)
    std::string kind;                        // gcc | clang
    int version_major{0};                    // --version major to match
    std::optional<int> min_version;          // if set, accept major >= min (else exact)
    std::string build_dir;                   // build-gcc15
    std::string flag;                        // CLI selector, e.g. --gcc15
    std::vector<std::string> cc_names;       // payload names tried as <ccname>
    std::vector<std::string> activations;    // command templates with {CMD}
    std::vector<std::string> scan_deps;      // clang-scan-deps candidate names (clang only)
    std::vector<std::string> required_on;    // OS families where this toolchain is REQUIRED
};

struct ResolvedToolchain {
    Toolchain spec;
    bool available{false};
    std::string cc_path;                     // absolute path -> CMAKE_CXX_COMPILER
    std::string version_line;                // first line of --version
    int detected_major{0};
    std::string matched_activation;          // the activation template that worked
    std::string matched_cc;                  // the cc_name that worked
    std::map<std::string, std::string> env_delta; // env vars activation added/changed
};

struct EnvSnapshot {
    std::map<std::string, std::string> vars;
};

// The toolchain table itself is data (Setup/compilers.jtext). This package does not read it:
// it links nothing but the standard library, so the caller (jac313_test_cli) loads the table
// and hands the toolchains in.

// Sense one toolchain: walk activations x cc_names, version-sniff, capture cc_path
// and the activation env delta. First viable (activation, cc) wins.
ResolvedToolchain resolve_toolchain(const Toolchain& tc, const EnvSnapshot& baseline);

// Convenience: resolve every toolchain, in registry order.
std::vector<ResolvedToolchain> resolve_registry(const std::vector<Toolchain>& toolchains);

const Toolchain* find_toolchain_by_label(const std::vector<Toolchain>& tcs,
                                         const std::string& label);
const Toolchain* find_toolchain_by_flag(const std::vector<Toolchain>& tcs,
                                        const std::string& flag);

// Version parsing: first "N.N" token in --version text -> N. Works for gcc and clang.
std::optional<int> parse_version_major(const std::string& version_text);

// ---- host sensing + readiness manifest (FileCheckList.txt) ----

// A read-only snapshot of the host, for the informational manifest.
struct HostSnapshot {
    std::string os_id;        // rhel, linuxmint, ...
    std::string os_pretty;    // "Red Hat Enterprise Linux 9.8 (Plow)"
    std::string os_family;    // rhel | linuxmint | unknown (normalized)
    std::string kernel;       // uname release
    std::string cpu_model;
    int cpu_cores{0};
    int cpu_mhz{0};
    int ram_gb{0};
    std::string storage_model; // root-device label, best effort
};

HostSnapshot sense_host();

struct ProvisionRecipe;

// Readiness is EARNED by sensing, not claimed: a toolchain is required when its
// `required_on` lists the host's OS family, and a recipe marked `required` when its `check`
// fails. READY iff every required toolchain resolved and every required check passed.
struct ReadinessReport {
    bool ready{false};
    std::vector<std::string> required; // required toolchain labels for this host
    std::vector<std::string> missing;  // required labels that did not resolve
};

ReadinessReport assess_readiness(const std::vector<ResolvedToolchain>& resolved,
                                 const std::string& os_family,
                                 const std::vector<ProvisionRecipe>& recipes);

// Write <source_dir>/FileCheckList.txt — the merged visibility + readiness manifest:
// a STATUS verdict, host block, and one [x]/[ ] line per sensed toolchain. Controls
// nothing; the bootstrap flow consults it. disk_type is an optional informational label.
bool write_filecheck_list(const std::filesystem::path& source_dir,
                          const std::vector<ResolvedToolchain>& resolved,
                          const std::vector<ProvisionRecipe>& recipes,
                          const std::string& disk_type = "");

// ---- provisioning: data-driven per-platform install recipes ----

// One provisionable prerequisite: a row of Setup/recipes.jtext (components), with its install
// command per OS family (recipe_os). The caller loads the table.
struct ProvisionRecipe {
    std::string component;                              // gcc15, ninja, ...
    std::string desc;                                  // human description
    std::string for_toolchain;                         // toolchain label it provides (opt)
    std::string check;                                 // satisfied-test; exit 0 = present (opt)
    bool required{false};                              // true -> part of the readiness gate
    std::map<std::string, std::string> install_by_family; // family -> install command
};

// A concrete provision plan for this host: the steps to run, and any needed
// components that have no recipe for this OS family.
struct ProvisionStep {
    std::string component;
    std::string desc;
    std::string command;
};
struct ProvisionPlan {
    std::string family;
    std::vector<ProvisionStep> steps;
    std::vector<std::string> unsupported; // needed but no recipe for this family
};

// Decide what to provision: a recipe applies when its `for` toolchain is a missing
// required toolchain, or its `check` command fails (non-zero / absent).
ProvisionPlan plan_provision(const std::vector<ResolvedToolchain>& resolved,
                             const std::string& os_family,
                             const std::vector<ProvisionRecipe>& recipes);

// Drop inherited exported bash functions (BASH_FUNC_*) so activation probes via
// /bin/sh -c don't emit "error importing function definition" noise.
void scrub_exported_shell_functions();

// Environment helpers (apply activation deltas so build + test-run children inherit).
EnvSnapshot snapshot_env();
void apply_env_delta(const std::map<std::string, std::string>& delta);
void restore_env(const EnvSnapshot& snapshot);

} // namespace jac313::Setup::v002
