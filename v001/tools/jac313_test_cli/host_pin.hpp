#pragma once

#include "host_hardware.hpp"
#include "host_identity.hpp"

#include <jac313/Qlite/v001.hpp>

#include "jac313_results_db.hpp"

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

void print_fleet_table(std::ostream& out, jac313::Qlite::v001::Sqlite& db);

void print_ambiguity_help(std::ostream& out, const jac313::results::HostPinResult& r,
                          const jac313::results::HostId& h);

// Print fleet table + help and return 1.
int exit_ambiguous(std::ostream& err, jac313::Qlite::v001::Sqlite& db,
                   const jac313::results::HostPinResult& r, const jac313::results::HostId& h);

// Resolve + pin; on ambiguity prints table and returns nullopt.
std::optional<std::int64_t> try_resolve_and_pin(jac313::Qlite::v001::Sqlite& db,
                                                const HostHardwareRecord& hw,
                                                const HostPinFlags& flags,
                                                const std::string& version,
                                                std::ostream& err = std::cerr);

// Read-only propose (no write).
jac313::results::HostPinResult propose_host_group(jac313::Qlite::v001::Sqlite& db,
                                                  const HostHardwareRecord& hw);

} // namespace jac313::test_cli