#pragma once

#include "host_hardware.hpp"
#include "host_identity.hpp"

#include <jac313/Qlite/v002.hpp>

#include "jac313_results_db.hpp"

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

namespace jac313::test_cli {

struct HostPinFlags {
    std::string claim;
    std::int64_t assign_new = 0;
};

// Parse --claim jac313-### and --assign-new-### from argv (mutually exclusive).
HostPinFlags parse_host_pin_flags(int argc, char** argv, int start = 1);

jac313::results::HostPinOptions to_pin_options(const HostPinFlags& flags);

void print_fleet_table(std::ostream& out, jac313::Qlite::v002::Sqlite& db);

void print_ambiguity_help(std::ostream& out, const jac313::results::HostPinResult& r,
                          const jac313::results::HostId& h);

// Print fleet table + help and return 1.
int exit_ambiguous(std::ostream& err, jac313::Qlite::v002::Sqlite& db,
                   const jac313::results::HostPinResult& r, const jac313::results::HostId& h);

// Resolve + pin; on ambiguity prints table and returns nullopt. With a remote PG server set
// ($JAC313_PG_REMOTE) and no --claim / --assign-new, the number comes from the server's fleet
// registry (fleet_registry.hpp) and the local rows are renumbered to it; when the server cannot
// be reached the local number is used and checked again on the next run. source_dir is the
// version dir (it holds schema/fleet_v0_1.sql).
std::optional<std::int64_t> try_resolve_and_pin(jac313::Qlite::v002::Sqlite& db,
                                                const HostHardwareRecord& hw,
                                                const HostPinFlags& flags,
                                                const std::string& version,
                                                std::ostream& err = std::cerr,
                                                const std::filesystem::path& source_dir = {});

// True once this process has renumbered local results to the shared server's numbers (this
// machine's, or another machine's rows moved aside): the report pages then carry stale labels.
bool fleet_renumbered();

// Read-only propose (no write).
jac313::results::HostPinResult propose_host_group(jac313::Qlite::v002::Sqlite& db,
                                                  const HostHardwareRecord& hw);

} // namespace jac313::test_cli