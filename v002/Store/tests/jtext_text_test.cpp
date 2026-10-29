// Store's jText log keeps any category and payload as written. Text holding the main rows'
// formatter characters (U+25AE, U+258C), a pipe, a comment marker or a newline is written
// with a formatter that cannot clash with it, and reads back through the jText parser.

#include <jac313/Store/v002/headers/persistence/JTextSplitEventLog.hpp>

#include "parser/section.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <source_location>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;
using jac313::Store::v002::JTextSplitEventLog;
using jac313::Store::v002::PersistMode;

// A failed check says what and where, and ends the test.
void check(bool ok, std::string_view what, std::source_location at = std::source_location::current()) {
    if (ok) return;
    std::cerr << "FAIL: " << what << " at " << at.file_name() << ":" << at.line() << "\n";
    std::exit(EXIT_FAILURE);
}

int main() {
    const fs::path dir = fs::temp_directory_path() / "jac313_store_jtext_text_test";
    fs::remove_all(dir);
    fs::create_directories(dir);
    const std::string base = (dir / "text").string();

    const std::vector<std::pair<std::string, std::string>> events = {
        {"plain", "roll=0.01 pitch=0.02"},
        {"pipes|and #hash", "cmd | grep x # not a comment"},
        {"split \xE2\x96\xAE char", "and the comment \xE2\x96\x8C char"},
        {"both", "\xE2\x96\xAE and \xE2\x96\x8C | # all at once"},
        {"lines", "Traceback:\n  File \"x\" | 3\nValueError"},
    };
    {
        JTextSplitEventLog log(base, 0, 0, PersistMode::All);
        for (std::size_t n = 0; n < events.size(); ++n)
            log.append_event(n, 0, n, 0, events[n].first, events[n].second, 1000 + n, {}, {});
        log.finalize();
    }

    std::ifstream in(base + ".jtext");
    const std::string text{std::istreambuf_iterator<char>(in), {}};
    const auto file = jtext::parse_file_structure(text);
    if (!file) std::cerr << "parse: line " << file.error().line_no << ": " << file.error().message << "\n";
    check(file.has_value(), "the log parses");
    check(file->sections.size() == 1, "one section");
    check(file->sections[0].templates.size() == 2, "two templates");

    // A blank line ends a record and a line for field 1 starts one (SPEC 2.6); each line
    // carries the values from its position on.
    std::vector<std::vector<std::string>> records(1);
    for (const auto& pl : file->sections[0].data_lines) {
        if (pl.number == 0 || (pl.number == 1 && !records.back().empty())) {
            if (!records.back().empty()) records.emplace_back();
            if (pl.number == 0) continue;
        }
        auto& r = records.back();
        if (r.size() < pl.number - 1 + pl.values.size()) r.resize(pl.number - 1 + pl.values.size());
        for (std::size_t k = 0; k < pl.values.size(); ++k) r[pl.number - 1 + k] = pl.values[k];
    }
    if (records.back().empty()) records.pop_back();
    check(records.size() == events.size(), "one record per event");
    for (std::size_t n = 0; n < events.size(); ++n) {
        check(records[n].size() == 7, "seven fields");
        check(records[n][4] == events[n].first, "the category as written");
        check(records[n][5] == events[n].second, "the payload as written");
    }
    check(text.find(" 1. \xE2\x96\xAE\xE2\x96\x8C 0\xE2\x96\xAE") != std::string::npos, "the usual formatter");
    check(text.find("//File:    text.jtext\n") == 0, "//File: holds the name only");
    check(text.find(" 1. \xE2\x96\xAE\xE2\x96\x8C text.jtext\n") != std::string::npos, "header line 1 is the file name");

    fs::remove_all(dir);
    std::cout << "Store jText text: every category and payload read back as written\n";
    return EXIT_SUCCESS;
}
