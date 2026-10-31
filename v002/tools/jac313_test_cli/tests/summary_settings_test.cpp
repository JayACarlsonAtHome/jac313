// The run summary JSON (summary.cpp, read by Jac313Runner): "contract" is kRunnerContract and
// "settings" lists every setting the run was given, in order, as [label, value] pairs, escaped.

#include "summary.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <source_location>
#include <string>
#include <string_view>
#include <unistd.h>

namespace fs = std::filesystem;
namespace T = jac313::test_cli;

namespace {

int failures = 0;

void check(bool ok, std::string_view what, std::source_location at = std::source_location::current()) {
    if (ok) return;
    std::cerr << "FAIL: " << what << " at " << at.file_name() << ":" << at.line() << "\n";
    ++failures;
}

std::string slurp(const fs::path& p) {
    std::ifstream in(p);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

} // namespace

int main() {
    const fs::path dir = fs::temp_directory_path() / ("jac313_summary_settings_test_" + std::to_string(::getpid()));
    fs::create_directories(dir);
    T::GlobalOptions g;
    g.summary_json = dir / "summary.json";
    T::append_summary_part(g, R"({"gate":"ctest","test":"x","status":"PASS","run_id":7,"host":"jac313-001","os":"f"})");
    check(T::assemble_summary(g, {{"version", "v002"}},
                              {{"filter", "a|b \"q\""}, {"parallel jobs (-j)", "4"}, {"keep logs", "off"}}),
          "assemble_summary writes the file");
    const std::string j = slurp(g.summary_json);
    check(j.find("\"contract\": " + std::to_string(T::kRunnerContract)) != std::string::npos, "contract written");
    check(T::kRunnerContract == 5, "kRunnerContract == 5 (summary settings)");
    const auto at = j.find("\"settings\": [");
    check(at != std::string::npos, "a settings array");
    const auto f = j.find("[\"filter\", \"a|b \\\"q\\\"\"]");
    const auto jobs = j.find("[\"parallel jobs (-j)\", \"4\"]");
    const auto keep = j.find("[\"keep logs\", \"off\"]");
    check(f != std::string::npos && jobs != std::string::npos && keep != std::string::npos, "every setting, escaped");
    check(at < f && f < jobs && jobs < keep, "settings in the order given");
    check(j.find("\"totals\": {\"pass\": 1, \"fail\": 0}") != std::string::npos, "totals still written");

    T::GlobalOptions g2;   // no settings: an empty array, still valid JSON
    g2.summary_json = dir / "empty.json";
    T::assemble_summary(g2, {});
    check(slurp(g2.summary_json).find("\"settings\": [],") != std::string::npos, "no settings: an empty array");

    fs::remove_all(dir);
    if (failures) { std::cerr << failures << " check(s) failed\n"; return 1; }
    std::cout << "summary_settings_test: OK\n";
    return 0;
}
