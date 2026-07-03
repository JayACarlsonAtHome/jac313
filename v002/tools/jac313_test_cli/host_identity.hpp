#pragma once

// Build jac313::results::HostId from sensed hardware — one place so every gate agrees
// on the group_id key (cpu, cores, ram_gb, os_key, disk).

#include "host_hardware.hpp"
#include "run_identity.hpp"

#include "jac313_results_db.hpp"
#include "jac313_sha256.hpp"

namespace jac313::test_cli {

inline jac313::results::HostId make_host_id(const HostHardwareRecord& hw) {
    return jac313::results::HostId{
        hw.cpu_model,
        static_cast<std::int64_t>(hw.cpu_cores),
        static_cast<std::int64_t>(hw.ram_gb),
        sensed_os_key(),
        hw.disk_type_label,
        jac313::results::sha256_hex_lower(nodename_for_hash()),
        static_cast<std::int64_t>(hw.p_cores),
        static_cast<std::int64_t>(hw.cpu_mhz_min),
        static_cast<std::int64_t>(hw.cpu_mhz_max),
    };
}

} // namespace jac313::test_cli