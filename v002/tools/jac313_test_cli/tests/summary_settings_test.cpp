// The run summary jText (summary.cpp, read by Jac313Runner), read back with jText's own parser:
// the run row carries kRunnerContract and the totals, "settings" lists every setting the run was
// given in order, "results" holds every result, "failures" only those not PASS or SKIP, with why.
// Also the parts line format (shared/jac313_summary_part.hpp) the gates append while a run goes.

#include "summary.hpp"
#include "parser/section.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <source_location>
#include <string>
#include <string_view>
#include <vector>
#include <unistd.h>

namespace fs = std::filesystem;
namespace T = jac313::test_cli;
namespace SP = jac313::summary_part;

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

using Row = std::map<std::string, std::string>;

// The rows of one section, by column name, through jText's parser ("" file = parse error).
std::vector<Row> table(const std::string& text, std::string_view section, bool* parsed = nullptr) {
    const auto file = jtext::parse_file_structure(std::string_view(text));
    if (parsed) *parsed = file.has_value();
    std::vector<Row> rows;
    if (!file) return rows;
    for (const auto& s : file->sections) {
        if (s.name != section) continue;
        std::vector<std::string> cols;
        for (const auto& f : s.field_lines)
            if (f.number && !f.values.empty()) {
                if (cols.size() < f.number) cols.resize(f.number);
                cols[f.number - 1] = f.values[0];
            }
        for (const auto& d : s.data_lines) {
            if (d.number != 1) continue;   // the writer puts a whole record on its "1." line
            Row r;
            for (std::size_t i = 0; i < d.values.size() && i < cols.size(); ++i) r[cols[i]] = d.values[i];
            rows.push_back(std::move(r));
        }
    }
    return rows;
}

} // namespace

int main() {
    const fs::path dir = fs::temp_directory_path() / ("jac313_summary_settings_test_" + std::to_string(::getpid()));
    fs::create_directories(dir);

    // The parts line: escapes survive a round trip, dotted keys, the status key a shell can grep.
    {
        const SP::Part p{{"gate", "smoke"}, {"status", "FAIL"}, {"error", "a\tb\nc\\d"}, {"readback.status", "PASS"}};
        const std::string line = SP::format(p);
        check(line.find('\n') == std::string::npos, "a parts line is one line");
        check(line.find("\tstatus=FAIL\t") != std::string::npos, "status is its own key=value");
        check(SP::parse(line) == p, "a parts line reads back as written");
        check(SP::get(SP::parse(line), "readback.status") == "PASS", "a dotted key reads back");
    }

    T::GlobalOptions g;
    g.summary_path = dir / "summary.jtext";
    T::append_summary_part(g, {{"gate", "ctest"}, {"test", "x"}, {"status", "PASS"}, {"run_id", "7"},
                               {"host", "jac313-001"}, {"os", "f"}});
    check(T::assemble_summary(g, {{"version", "v002"}, {"compiler", "gcc15"}},
                              {{"filter", "a|b \"q\""}, {"parallel jobs (-j)", "4"}, {"keep logs", "off"}}),
          "assemble_summary writes the file");
    check(!fs::exists(g.summary_path.string() + ".tmp"), "no temp file left");
    const std::string j = slurp(g.summary_path);
    bool parsed = false;
    const auto run = table(j, "run", &parsed);
    check(parsed, "the summary is jText the parser reads");
    check(run.size() == 1, "one run row");
    check(T::kRunnerContract == 6, "kRunnerContract == 6 (jText summary)");
    if (run.size() == 1) {
        check(run[0].at("contract") == std::to_string(T::kRunnerContract), "contract written");
        check(run[0].at("host") == "jac313-001" && run[0].at("run_id") == "7", "run row: host and run id");
        check(run[0].at("version") == "v002" && run[0].at("compiler") == "gcc15", "run row: meta fields");
        check(run[0].at("pass") == "1" && run[0].at("fail") == "0", "run row: totals");
        check(!run[0].at("hostname").empty(), "run row: this machine's hostname");
    }
    const auto set = table(j, "settings");
    check(set.size() == 3, "every setting");
    if (set.size() == 3) {
        check(set[0].at("name") == "filter" && set[0].at("value") == "a|b \"q\"", "a setting kept exactly");
        check(set[1].at("name") == "parallel jobs (-j)" && set[2].at("name") == "keep logs", "settings in the order given");
    }
    check(table(j, "results").size() == 1, "one result row");
    check(table(j, "failures").empty(), "no failure rows when all passed");

    T::GlobalOptions g3;   // failures: the reason on one line, a separator in it kept
    g3.summary_path = dir / "fail.jtext";
    T::append_summary_part(g3, {{"gate", "ctest"}, {"test", "ok_test"}, {"status", "PASS"}, {"run_id", "9"}});
    T::append_summary_part(g3, {{"gate", "ctest"}, {"test", "bad_test"}, {"status", "FAIL"}, {"run_id", "9"},
                                {"error", "FAIL  a|b\nFAIL  \"c\""}});
    T::GlobalOptions g3p = g3;   // a smoke result with read-back fields and bench-style extras
    SP::Part smoke{{"gate", "smoke"}, {"test", "s1"}, {"backend", "sql"}, {"status", "ERROR"}, {"run_id", "9"},
                   {"log", "/tmp/s1.log"}};
    T::add_readback_fields(smoke, {{"backend", "sql"}, {"mode", "all"}, {"status", "FAIL"},
                                   {"error", "bad dbl1"}, {"rows_checked", "12"}});
    smoke.emplace_back("median_ops", "1000");
    T::append_summary_part(g3p, smoke);
    // a row the run script adds itself (a failed build) carries no run id: the run's own is used
    T::append_summary_part(g3p, {{"gate", "build"}, {"test", "build gcc15"}, {"status", "FAIL"},
                                 {"error", "compile failed (rc=1)"}});
    check(T::assemble_summary(g3, {{"compiler", "gcc15"}, {"os", "RHEL 9.8 · 5.14"}, {"cpu_model", "Xeon X"},
                                   {"cores", "8"}, {"ram_gb", "32"}, {"disk", "xssd"}}),
          "assemble_summary with failures");
    const std::string fl = slurp(g3.summary_path);
    const auto fails = table(fl, "failures");
    check(fails.size() == 3, "three failure rows");
    if (fails.size() == 3) {
        check(fails[0].at("test") == "bad_test" && fails[0].at("status") == "FAIL", "the failed test is listed");
        check(fails[0].at("error") == "FAIL a|b FAIL \"c\"", "the reason on one line (spaces closed up)");
        check(fails[1].at("test") == "s1" && fails[1].at("status") == "ERROR", "an ERROR result is listed");
        check(fails[1].at("log") == "/tmp/s1.log", "with its log");
        check(fails[1].at("error") == "bad dbl1", "a smoke result's read-back error");
        check(fails[1].at("backend") == "sql" && fails[0].at("backend") == "-", "backend, '-' when none");
        check(!fails[0].at("hostname").empty(), "a failure names this machine's hostname");
        check(fails[2].at("run_id") == "9", "a failure without its own run id gets the run's");
        check(!fails[0].count("cpu_model"), "the specs are in the header and run row, not every failure");
    }
    const auto res = table(fl, "results");
    check(res.size() == 4, "every result in results");
    if (res.size() == 4) {
        check(res[2].at("rb_mode") == "all" && res[2].at("rb_status") == "FAIL", "read-back columns");
        check(res[2].at("rb_checked") == "12", "rb_checked from rows_checked");
        check(res[2].at("extra") == "median_ops=1000", "other values kept in extra");
    }
    const auto run3 = table(fl, "run");
    check(run3.size() == 1 && run3[0].at("pass") == "1" && run3[0].at("fail") == "3", "totals with failures");
    check(run3.size() == 1 && run3[0].at("cpu_model") == "Xeon X" && run3[0].at("os") == "RHEL 9.8 · 5.14",
          "the run row carries the machine's specs");
    {   // the header comments, as jText's parser gives them
        const auto file = jtext::parse_file_structure(std::string_view(fl));
        std::string hc;
        if (file) for (const auto& c : file->header_comments) hc += c.text + "\n";
        check(hc.find("Host:") != std::string::npos && hc.find("Run id:   9") != std::string::npos,
              "header comments: host and run id");
        check(hc.find("OS:       RHEL 9.8 · 5.14") != std::string::npos && hc.find("CPU:      Xeon X, 8 threads") != std::string::npos
              && hc.find("Memory:   32 GB") != std::string::npos && hc.find("Disk:     xssd") != std::string::npos,
              "header comments: OS and machine specs");
        check(hc.find("Result:   1 pass, 3 fail") != std::string::npos, "header comments: the result");
    }

    T::GlobalOptions g2;   // no parts and no settings: still a whole file
    g2.summary_path = dir / "empty.jtext";
    check(T::assemble_summary(g2, {}), "an empty run writes a summary");
    const std::string e = slurp(g2.summary_path);
    check(table(e, "run").size() == 1 && table(e, "settings").empty() && table(e, "results").empty(),
          "an empty run: the run row only");

    if (failures) { std::cerr << failures << " check(s) failed (files kept in " << dir.string() << ")\n"; return 1; }
    fs::remove_all(dir);
    std::cout << "summary_settings_test: OK\n";
    return 0;
}
