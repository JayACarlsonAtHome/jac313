#pragma once

#include "params.hpp"
#include "time_util.hpp"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace jac313::test_cli {

struct MatrixRunMeta {
    std::int64_t group_id{0};
    std::string run_utc;
    std::string os;                    // sensed OS identity key, e.g. "rhel-10.2"
    std::string compiler;
    std::string build_type{"Debug"};   // CMAKE_BUILD_TYPE of the tree under test
    std::string disk_type;
    std::string size;
    std::string size_label;
    std::string modules{"modules"};    // "modules" | "textual" (identity dimension)
    std::string os_id;                 // legacy manual OS label (params.os_id)
    std::string host_hardware;
    int passed{0};
    int failed{0};
    int skipped{0};
    int errors{0};
    int duration_ms{0};
};

struct ScenarioIdentity {
    std::string test;
    std::string package;
    std::string category;
    std::string persist;
    std::string output_mode;
};

std::string scenario_db_field(const std::string& value);

MatrixRunMeta matrix_run_meta_for(const MatrixParams& params, const std::string& compiler_label);

std::string matrix_run_utc_timestamp();

std::string display_run_utc(std::string utc);

std::string normalize_log_path(std::string log_rel, const std::string& compiler);

} // namespace jac313::test_cli