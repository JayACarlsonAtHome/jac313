// jac313_summary_part.hpp — one result line of a run's summary parts file.
//
// While a run is going, every gate appends one line per result to "<summary>.parts":
// jac313_test_cli (ctest, smoke, verify), the bench (store_bench --summary-parts) and the run
// script's own gate rows (printf in the generated script). When the run ends, jac313_test_cli
// assembles the parts into the summary jText (summary.cpp). The parts file is that hand-off only.
//
// A line is key=value pairs separated by tabs, in the order written:
//     gate=ctest<TAB>test=jtext_v002_test<TAB>status=PASS<TAB>ms=352
// In a value a backslash, tab, newline and carriage return are written \\ \t \n \r; a key never
// holds '=', a tab or a line break. A nested value is a dotted key: readback.mode=all,
// dataset.base=/path. The result's own status is the key "status" exactly, so a shell can tell a
// failed result with grep -vE '(^|\t)status=(PASS|SKIP)(\t|$)'.
#pragma once

#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace jac313::summary_part {

using Part = std::vector<std::pair<std::string, std::string>>;

inline std::string escape(std::string_view v) {
    std::string o;
    o.reserve(v.size());
    for (const char c : v) {
        switch (c) {
            case '\\': o += "\\\\"; break;
            case '\t': o += "\\t";  break;
            case '\n': o += "\\n";  break;
            case '\r': o += "\\r";  break;
            default:   o += c;
        }
    }
    return o;
}

inline std::string unescape(std::string_view v) {
    std::string o;
    o.reserve(v.size());
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (v[i] != '\\' || i + 1 >= v.size()) { o += v[i]; continue; }
        switch (const char e = v[++i]) {
            case 't': o += '\t'; break;
            case 'n': o += '\n'; break;
            case 'r': o += '\r'; break;
            default:  o += e;
        }
    }
    return o;
}

// One line, without the trailing newline.
inline std::string format(const Part& p) {
    std::string line;
    for (const auto& [k, v] : p) {
        if (!line.empty()) line += '\t';
        line += k;
        line += '=';
        line += escape(v);
    }
    return line;
}

inline Part parse(std::string_view line) {
    Part p;
    while (!line.empty() && (line.back() == '\n' || line.back() == '\r')) line.remove_suffix(1);
    std::size_t i = 0;
    while (i <= line.size()) {
        std::size_t tab = line.find('\t', i);
        if (tab == std::string_view::npos) tab = line.size();
        const std::string_view kv = line.substr(i, tab - i);
        if (const auto eq = kv.find('='); eq != std::string_view::npos && eq > 0)
            p.emplace_back(std::string(kv.substr(0, eq)), unescape(kv.substr(eq + 1)));
        i = tab + 1;
    }
    return p;
}

// The value of `key`, or `fallback` when the line has none.
inline std::string get(const Part& p, std::string_view key, std::string_view fallback = {}) {
    for (const auto& [k, v] : p)
        if (k == key) return v;
    return std::string(fallback);
}

inline bool has(const Part& p, std::string_view key) {
    for (const auto& kv : p)
        if (kv.first == key) return true;
    return false;
}

} // namespace jac313::summary_part
