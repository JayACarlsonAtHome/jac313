#pragma once

#include "options.hpp"
#include "results_db.hpp"

#include <cstdint>
#include <filesystem>
#include <string>

namespace jac313::test_cli {

struct BuildBenchmarkRequest {
    GlobalOptions global;
    ConfigureOptions configure;
    BuildOptions build;
    ResultsDbContext db;
    std::int64_t group_id{0};
    std::string compiler_label;
};

int run_build_benchmark(const BuildBenchmarkRequest& request);

} // namespace jac313::test_cli