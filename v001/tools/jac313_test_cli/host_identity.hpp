#pragma once

// Build jac313::results::MachineId from sensed hardware — one place so every gate agrees
// on the machine key (cpu, cores, ram_gb, disk, instance_hash). Hardware only: the OS is
// recorded per run (results os table), never part of the machine.

#include "host_hardware.hpp"

#include "jac313_results_db.hpp"
#include "jac313_sha256.hpp"

namespace jac313::test_cli {

inline jac313::results::MachineId make_machine_id(const HostHardwareRecord& hw) {
    return jac313::results::MachineId{
        hw.cpu_model,
        static_cast<std::int64_t>(hw.cpu_cores),
        static_cast<std::int64_t>(hw.ram_gb),
        hw.disk_type_label,
        jac313::results::sha256_hex_lower(nodename_for_hash()),
        static_cast<std::int64_t>(hw.p_cores),
        static_cast<std::int64_t>(hw.cpu_mhz_min),
        static_cast<std::int64_t>(hw.cpu_mhz_max),
    };
}

} // namespace jac313::test_cli
