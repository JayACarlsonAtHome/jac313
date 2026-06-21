#include "matrix_manifest.hpp"
#include "host_hardware.hpp"
#include "run_identity.hpp"

namespace jac313::test_cli {

std::string matrix_run_utc_timestamp() {
    return utc_now_manifest();
}

std::string scenario_db_field(const std::string& value) {
    return value.empty() ? std::string("-") : value;
}

MatrixRunMeta matrix_run_meta_for(const MatrixParams& params, const std::string& compiler_label)
{
    MatrixRunMeta meta;
    meta.os = sensed_os_key();              // identity dimension, e.g. "rhel-10.2"
    meta.compiler = compiler_label;
    meta.disk_type = params.disk_type;
    meta.size = params.size;
    meta.size_label = size_label(params.size);
    meta.os_id = params.os_id;
    meta.host_hardware = collect_host_hardware_line(params.disk_type);
    return meta;
}

std::string normalize_log_path(std::string log_rel, const std::string& /*compiler*/) {
    // gcc14 was the only compiler that needed log-path rewriting; retired -> no-op now.
    return log_rel;
}

std::string display_run_utc(std::string utc) {
    if (const auto pos = utc.find('T'); pos != std::string::npos) {
        utc[pos] = ' ';
    }
    return utc;
}

} // namespace jac313::test_cli