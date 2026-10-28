// Store's jText log keeps any category and payload as written. Text holding the main rows'
// formatter characters (U+25AE, U+258C), a pipe, a comment marker or a newline is written
// with a formatter that cannot clash with it, and reads back through the jText parser.

#include <jac313/Store/v001/headers/persistence/JTextSplitEventLog.hpp>

#include "parser/section.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using jac313::Store::v001::JTextSplitEventLog;
using jac313::Store::v001::PersistMode;

#define CHECK(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
            return EXIT_FAILURE; \
        } \
    } while (0)

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
    CHECK(file.has_value());
    CHECK(file->sections.size() == 1);
    CHECK(file->sections[0].templates.size() == 2);

    // Records are separated by blank lines; each line carries the values from its position on.
    std::vector<std::vector<std::string>> records(1);
    for (const auto& pl : file->sections[0].data_lines) {
        if (pl.number == 0) {
            if (!records.back().empty()) records.emplace_back();
            continue;
        }
        auto& r = records.back();
        if (r.size() < pl.number - 1 + pl.hierarchy.size()) r.resize(pl.number - 1 + pl.hierarchy.size());
        for (std::size_t k = 0; k < pl.hierarchy.size(); ++k) r[pl.number - 1 + k] = pl.hierarchy[k];
    }
    if (records.back().empty()) records.pop_back();
    CHECK(records.size() == events.size());
    for (std::size_t n = 0; n < events.size(); ++n) {
        CHECK(records[n].size() == 7);
        CHECK(records[n][4] == events[n].first);
        CHECK(records[n][5] == events[n].second);
    }
    CHECK(text.find(" 1. \xE2\x96\xAE\xE2\x96\x8C 0\xE2\x96\xAE") != std::string::npos);   // the usual formatter
    CHECK(text.find("//File:    text.jtext\n") == 0);                                          // the name only

    fs::remove_all(dir);
    std::cout << "Store jText text: every category and payload read back as written\n";
    return EXIT_SUCCESS;
}
