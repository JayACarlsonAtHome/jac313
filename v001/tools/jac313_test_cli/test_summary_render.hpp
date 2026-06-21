#pragma once

#include "results_db.hpp"

#include <cstdint>
#include <filesystem>

namespace jac313::test_cli {

inline constexpr int kLogTailLineCount = 50;

enum class SummaryRenderStep {
    RunPage,
    Hub,
    All,
};

bool render_test_summary(const ResultsDbContext& ctx, std::int64_t run_id);

bool write_summary_hub(const ResultsDbContext& ctx, bool include_throughput = true);

/// Incremental render: runs steps up to and including `through`.
bool render_test_summary_through(const ResultsDbContext& ctx,
                                 std::int64_t run_id,
                                 SummaryRenderStep through);

} // namespace jac313::test_cli