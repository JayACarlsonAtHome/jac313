#pragma once

// summary.hpp — persistence/read-back flag forwarding and the run summary jText that
// Jac313Runner (v001/gui) reads.
//
// Each gate command (run = ctest, matrix run = smoke, store_bench --suite = bench, verify) appends
// one line per result to "<summary>.parts" (key=value pairs, shared/jac313_summary_part.hpp). The
// preset command deletes both files before it starts and, when its script finishes, assembles
// <summary> (runner_last_gcc.jtext) through jText's writer. Its header carries comment lines a
// reader gets as the file's header comments: Host (label and hostname), Run id, OS, CPU, Memory,
// Disk, Compiler, Result. Then four sections:
//   run       one row: contract, run_id, host, hostname, os, kernel, cpu_model, cores,
//             physical_cores, ram_gb, cpu_mhz_max, disk, version, compiler, build_type, contracts,
//             pg_remote, started_utc, finished_utc, command, pass, fail
//   settings  every setting the run was given, in order: id, name, value
//   results   one row per result (see kResultColumns in summary.cpp)
//   failures  the results whose status is not PASS or SKIP: id, host, hostname, gate, test,
//             backend, compiler, status, run_id, error, log (no rows when nothing failed).
//             host is the jac313-### label, hostname the machine's own name (gethostname).

#include "options.hpp"
#include "jac313_summary_part.hpp"

#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace jac313::test_cli {

/// " --persist ... --max-persist-rows N --readback M --pg-cleanup C --summary P --size S"
/// (only the options that were given), for commands generated into run_latest_config.sh.
// Version of the CLI <-> Jac313Runner interface (flags + summary fields), written as "contract"
// in every summary. Must match kRunnerContract in gui/Jac313Runner/Model.h.
inline constexpr int kRunnerContract = 6;   // 2: --pg-remote / --pg-local, summary "pg_remote"; 3: --connections;
                                            // 4: --contracts, compiler-versions lists the contract modes;
                                            // 5: summary "settings" (every setting the run was given);
                                            // 6: the summary is jText (--summary, was --summary-json)

std::string persist_cli_suffix(const GlobalOptions& global);

/// The same options as store_bench arguments (its --suite understands them).
std::string bench_persist_args(const GlobalOptions& global);

/// Test arguments for the matrix tests: --max-persist-rows / --readback / --pg-cleanup.
std::vector<std::string> matrix_test_args(const GlobalOptions& global);

/// Split "a,b,c" (spaces ignored).
std::vector<std::string> split_list(std::string_view s);

/// The parts file next to the summary ("<summary>.parts"); empty when no --summary.
std::filesystem::path summary_parts_path(const GlobalOptions& global);

/// Append one result to the parts file (one key=value line).
void append_summary_part(const GlobalOptions& global, const summary_part::Part& part);

/// A READBACK key/value map as part fields "readback.<key>" (the backend key is left out).
void add_readback_fields(summary_part::Part& part, const std::map<std::string, std::string>& rb);

/// Every setting a run was given, in order: (label, value) pairs ("-" = not set / the default).
using SettingList = std::vector<std::pair<std::string, std::string>>;

/// Assemble <summary> from its parts. `meta` holds the run row's text fields (version, compiler,
/// pg_remote, build_type, contracts, started_utc, finished_utc, command; the machine: os, kernel,
/// cpu_model, cores, physical_cores, ram_gb, cpu_mhz_max, disk), `settings` the run's settings in order.
bool assemble_summary(const GlobalOptions& global, const std::map<std::string, std::string>& meta,
                      const SettingList& settings = {});

} // namespace jac313::test_cli
