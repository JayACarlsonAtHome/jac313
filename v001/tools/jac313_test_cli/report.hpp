#pragma once

#include "runner.hpp"

#include <filesystem>
#include <vector>

namespace jac313::test_cli {

struct RunSummary {
    int passed{0};
    int failed{0};
    int skipped{0};
    int errors{0};
    std::chrono::milliseconds total_duration{0};
};

RunSummary summarize(const std::vector<TestResult>& results);
void print_summary(const RunSummary& summary, const std::vector<TestResult>& results);
bool write_summary_file(const std::filesystem::path& path,
                        const RunSummary& summary,
                        const std::vector<TestResult>& results);

} // namespace jac313::test_cli