#pragma once

// summary.hpp — persistence/read-back flag forwarding and the run summary JSON that
// Jac313Runner (v001/gui) reads.
//
// Each gate command (run = ctest, matrix run = smoke, store_bench --suite = bench) appends one
// JSON object per result to "<summary-json>.parts" (one per line). The preset command deletes
// both files before it starts and, when its script finishes, assembles <summary-json>:
//   { run_id, host, version, compiler, build_type, started_utc, finished_utc, command,
//     totals: {pass, fail}, results: [ ...parts... ] }

#include "options.hpp"

#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace jac313::test_cli {

/// " --persist ... --max-persist-rows N --readback M --pg-cleanup C --summary-json P --size S"
/// (only the options that were given), for commands generated into run_latest_config.sh.
// Version of the CLI <-> Jac313Runner interface (flags + summary JSON fields), written as
// "contract" in every summary. Must match kRunnerContract in <root>/gui/Jac313Runner/Model.h.
inline constexpr int kRunnerContract = 4;   // 2: --pg-remote / --pg-local, summary "pg_remote"; 3: --connections;
                                            // 4: --contracts, compiler-versions lists the contract modes

std::string persist_cli_suffix(const GlobalOptions& global);

/// The same options as store_bench arguments (its --suite understands them).
std::string bench_persist_args(const GlobalOptions& global);

/// Test arguments for the matrix tests: --max-persist-rows / --readback / --pg-cleanup.
std::vector<std::string> matrix_test_args(const GlobalOptions& global);

/// Split "a,b,c" (spaces ignored).
std::vector<std::string> split_list(std::string_view s);

std::string json_escape(std::string_view s);

/// The parts file next to the summary ("<summary>.parts"); empty when no --summary-json.
std::filesystem::path summary_parts_path(const GlobalOptions& global);

/// Append one already-formatted JSON object (single line) to the parts file.
void append_summary_part(const GlobalOptions& global, const std::string& json_object);

/// JSON object text for a READBACK key/value map ("" when empty).
std::string readback_json(const std::map<std::string, std::string>& rb);

/// Assemble <summary-json> from its parts. `meta` holds the top-level string fields.
bool assemble_summary(const GlobalOptions& global, const std::map<std::string, std::string>& meta);

} // namespace jac313::test_cli
