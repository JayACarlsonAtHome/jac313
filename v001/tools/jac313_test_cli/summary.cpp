#include "summary.hpp"

#include <cstdio>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace jac313::test_cli {

namespace {

std::string quote(std::string_view s) { return "\"" + json_escape(s) + "\""; }

bool is_number(std::string_view s) {
    if (s.empty()) return false;
    std::size_t i = (s[0] == '-') ? 1 : 0;
    bool digit = false, dot = false;
    for (; i < s.size(); ++i) {
        if (s[i] >= '0' && s[i] <= '9') digit = true;
        else if (s[i] == '.' && !dot) dot = true;
        else return false;
    }
    return digit;
}

} // namespace

std::vector<std::string> split_list(std::string_view s) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == ',') { if (!cur.empty()) out.push_back(cur); cur.clear(); }
        else if (c != ' ') cur += c;
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

std::string json_escape(std::string_view s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (unsigned char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if (c < 0x20) { char b[8]; std::snprintf(b, sizeof b, "\\u%04x", c); out += b; }
                else out += static_cast<char>(c);
        }
    }
    return out;
}

std::string persist_cli_suffix(const GlobalOptions& g) {
    std::string s;
    if (!g.persist_list.empty())  s += " --persist " + g.persist_list;
    if (g.max_persist_rows)       s += " --max-persist-rows " + std::to_string(*g.max_persist_rows);
    if (g.max_runs)               s += " --max-runs " + std::to_string(g.max_runs);
    if (g.max_records)            s += " --max-records " + std::to_string(g.max_records);
    if (!g.readback.empty())      s += " --readback " + g.readback;
    if (!g.pg_cleanup.empty())    s += " --pg-cleanup " + g.pg_cleanup;
    if (!g.summary_json.empty())  s += " --summary-json \"" + g.summary_json.string() + "\"";
    if (!g.size_override.empty()) s += " --size " + g.size_override;
    return s;
}

std::string bench_persist_args(const GlobalOptions& g) {
    std::string s;
    if (!g.persist_list.empty())  s += " --persist-list " + g.persist_list;
    if (g.max_persist_rows)       s += " --max-persist-rows " + std::to_string(*g.max_persist_rows);
    if (g.max_runs)               s += " --max-runs " + std::to_string(g.max_runs);
    if (g.max_records)            s += " --max-records " + std::to_string(g.max_records);
    if (!g.readback.empty())      s += " --readback " + g.readback;
    if (!g.pg_cleanup.empty())    s += " --pg-cleanup " + g.pg_cleanup;
    if (!g.summary_json.empty())  s += " --summary-parts \"" + summary_parts_path(g).string() + "\"";
    return s;
}

std::vector<std::string> matrix_test_args(const GlobalOptions& g) {
    std::vector<std::string> a;
    if (g.max_persist_rows) a.push_back("--max-persist-rows=" + std::to_string(*g.max_persist_rows));
    if (!g.readback.empty())   a.push_back("--readback=" + g.readback);
    if (!g.pg_cleanup.empty()) a.push_back("--pg-cleanup=" + g.pg_cleanup);
    return a;
}

fs::path summary_parts_path(const GlobalOptions& g) {
    if (g.summary_json.empty()) return {};
    return fs::path(g.summary_json.string() + ".parts");
}

void append_summary_part(const GlobalOptions& g, const std::string& json_object) {
    const auto p = summary_parts_path(g);
    if (p.empty()) return;
    std::error_code ec;
    fs::create_directories(p.parent_path(), ec);
    std::ofstream out(p, std::ios::app);
    if (out) out << json_object << '\n';
}

std::string readback_json(const std::map<std::string, std::string>& rb) {
    if (rb.empty()) return {};
    std::string s = "{";
    bool first = true;
    for (const auto& [k, v] : rb) {
        if (k == "backend") continue;
        s += (first ? "" : ",") + quote(k) + ":" + (is_number(v) ? v : quote(v));
        first = false;
    }
    return s + "}";
}

bool assemble_summary(const GlobalOptions& g, const std::map<std::string, std::string>& meta) {
    if (g.summary_json.empty()) return true;
    std::vector<std::string> parts;
    {
        std::ifstream in(summary_parts_path(g));
        for (std::string line; std::getline(in, line);) if (!line.empty()) parts.push_back(line);
    }
    long pass = 0, fail = 0;
    std::string run_id, host;
    for (const auto& p : parts) {
        // the result's own status is the FIRST "status" key (a nested readback status follows it)
        const auto st = p.find("\"status\":\"");
        const std::string status = st == std::string::npos ? "" : p.substr(st + 10, p.find('"', st + 10) - st - 10);
        if (status == "PASS") ++pass;
        else if (status != "SKIP") ++fail;
        // the last recorded run id / host win (the summary header shows the latest)
        auto field = [&](std::string_view key) -> std::string {
            const std::string k = "\"" + std::string(key) + "\":";
            const auto at = p.find(k);
            if (at == std::string::npos) return {};
            auto v = p.substr(at + k.size());
            if (!v.empty() && v[0] == '"') return v.substr(1, v.find('"', 1) - 1);
            return v.substr(0, v.find_first_of(",}"));
        };
        if (auto r = field("run_id"); !r.empty()) run_id = r;
        if (auto h = field("host"); !h.empty()) host = h;
    }
    std::error_code ec;
    fs::create_directories(g.summary_json.parent_path(), ec);
    std::ofstream out(g.summary_json, std::ios::trunc);
    if (!out) {
        std::cerr << "summary: cannot write " << g.summary_json.string() << '\n';
        return false;
    }
    out << "{\n";
    // Jac313Runner refuses a summary whose contract is newer than it understands
    // (gui/Jac313Runner/Model.h kRunnerContract). Bump both together when the flags or fields change.
    out << "  \"contract\": " << kRunnerContract << ",\n";
    out << "  \"run_id\": " << (run_id.empty() ? "null" : run_id) << ",\n";
    out << "  \"host\": " << quote(host) << ",\n";
    for (const auto& [k, v] : meta) out << "  " << quote(k) << ": " << quote(v) << ",\n";
    out << "  \"totals\": {\"pass\": " << pass << ", \"fail\": " << fail << "},\n";
    out << "  \"results\": [\n";
    for (std::size_t i = 0; i < parts.size(); ++i) {
        out << "    " << parts[i] << (i + 1 < parts.size() ? ",\n" : "\n");
    }
    out << "  ]\n}\n";
    std::cout << "[summary] wrote " << g.summary_json.string() << " (" << parts.size() << " results: "
              << pass << " pass, " << fail << " fail)\n";
    return true;
}

} // namespace jac313::test_cli
