#pragma once

#include <string>

namespace jac313::test_cli {

struct HostHardwareRecord {
    std::string hostname;
    std::string cpu_model;
    int cpu_cores{0};        // logical/thread count (T.Cores)
    int p_cores{0};          // physical cores (P.Cores)
    int ram_gb{0};
    int cpu_mhz{0};
    int cpu_mhz_min{0};      // min/max clock for the Speed range (MHz)
    int cpu_mhz_max{0};
    std::string storage_model;
    std::string disk_type_label;
    std::string os_pretty;
    std::string kernel_release;
    std::string os_id;
    std::string hardware_specs;
    std::string os_specs;
};

// uname nodename only — used for instance_hash (never host_label.local override).
std::string nodename_for_hash();

// True for localhost / localhost.localdomain — forces operator disambiguation.
bool is_generic_nodename(const std::string& nodename);

HostHardwareRecord collect_host_hardware_record(const std::string& disk_type_label,
                                                const std::string& os_id = {});

std::string collect_hardware_specs(const std::string& disk_type_label);

std::string collect_os_specs(const std::string& os_id = {});

std::string collect_host_hardware_line(const std::string& disk_type_label);

// Auto-detect the disk tier (ssd / x7k / 10k / 15k) from the physical device backing `dir`
// (the repo / test-output disk, NOT the system root). Empty string if undeterminable.
std::string detect_disk_type(const std::string& dir = ".");

std::string format_group_environment_line(const std::string& hardware_specs,
                                          const std::string& os_specs);

std::string hardware_display_line(const std::string& hardware_specs,
                                  const std::string& os_specs,
                                  const std::string& disk_type);

std::string html_escape(const std::string& text);

} // namespace jac313::test_cli