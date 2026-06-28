#include "report.hpp"
#include "format.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

namespace jac313::test_cli {

RunSummary summarize(const std::vector<TestResult>& results) {
    RunSummary summary;
    for (const auto& r : results) {
        summary.total_duration += r.duration;
        switch (r.status) {
        case TestStatus::Passed:  ++summary.passed;  break;
        case TestStatus::Failed:  ++summary.failed;  break;
        case TestStatus::Skipped: ++summary.skipped; break;
        case TestStatus::Error:   ++summary.errors;  break;
        }
    }
    return summary;
}

void print_summary(const RunSummary& summary, const std::vector<TestResult>& results,
                   const std::string& context) {
    std::cout << "\n=== jac313 test summary ===\n";
    if (!context.empty()) std::cout << context << '\n';   // jac313-### · compiler · disk
    std::cout << "Passed:  " << format_count_padded(summary.passed) << '\n';
    std::cout << "Failed:  " << format_count_padded(summary.failed) << '\n';
    std::cout << "Skipped: " << format_count_padded(summary.skipped) << '\n';
    std::cout << "Errors:  " << format_count_padded(summary.errors) << '\n';
    std::cout << "Time:    " << format_count_padded(summary.total_duration.count()) << " ms\n";

    if (summary.failed > 0 || summary.errors > 0) {
        std::cout << "\nFailures:\n";
        for (const auto& r : results) {
            if (r.status == TestStatus::Failed || r.status == TestStatus::Error) {
                std::cout << "  - " << r.entry.name;
                if (!r.message.empty()) {
                    std::cout << " (" << r.message << ')';
                }
                std::cout << '\n';
            }
        }
    }
    std::cout << "\n\n";   // two blank lines after the summary, for visual separation
}

bool write_summary_file(const std::filesystem::path& path,
                        const RunSummary& summary,
                        const std::vector<TestResult>& results)
{
    std::ofstream out(path);
    if (!out) {
        return false;
    }

    out << "# jac313 test summary\n";
    out << "passed=" << summary.passed << '\n';
    out << "failed=" << summary.failed << '\n';
    out << "skipped=" << summary.skipped << '\n';
    out << "errors=" << summary.errors << '\n';
    out << "duration_ms=" << summary.total_duration.count() << "\n\n";

    for (const auto& r : results) {
        const char* status = "error";
        switch (r.status) {
        case TestStatus::Passed:  status = "pass"; break;
        case TestStatus::Failed:  status = "fail"; break;
        case TestStatus::Skipped: status = "skip"; break;
        case TestStatus::Error:   status = "error"; break;
        }
        out << r.entry.name << '\t' << status << '\t' << r.duration.count() << "ms";
        if (!r.message.empty()) {
            out << '\t' << r.message;
        }
        out << '\n';
    }
    return true;
}

} // namespace jac313::test_cli