#pragma once

// fleet_registry.hpp — jac313-### numbers handed out by the shared (pg-remote) server, so two
// machines never both call themselves jac313-001. The server's table is fleet.host_spec, made by
// schema/fleet_v0_1.sql (run before every registration; it is idempotent). Without a remote server
// (or without libpq in this build) nothing here connects, and numbering stays local.

#include "jac313_results_db.hpp"

#include <cstdint>
#include <filesystem>
#include <string>

namespace jac313::test_cli {

struct FleetResult {
    std::int64_t gid = 0;   // the server's number for this machine (0 = not registered)
    std::string error;      // why not, when gid == 0
};

// The server as named by $JAC313_PG_REMOTE, password-free ("" when none is set). It keys the
// local fleet_sync rows, so a number confirmed by one server is not taken as confirmed by another.
std::string fleet_server_key();

// Look up this machine on the server, or register it. `preferred` (its local number, 0 = none)
// is kept when the server has that number free; otherwise the server's next number is used.
// One locked transaction, so two machines registering at once cannot get the same number.
FleetResult fleet_register(const jac313::results::HostId& h, std::int64_t preferred,
                           const std::filesystem::path& source_dir);

} // namespace jac313::test_cli
