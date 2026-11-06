#include "summary.hpp"

#include <jac313/jText/v001.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

#include <unistd.h>   // gethostname

namespace fs = std::filesystem;

namespace jac313::test_cli {

namespace SP = jac313::summary_part;

namespace {

// One line for a jText cell: line breaks and tabs become spaces (the summary is read line by line).
std::string flat(std::string_view s) {
    std::string out(s);
    for (char& c : out)
        if (c == '\n' || c == '\r' || c == '\t') c = ' ';
    return out;
}

// A failure reason for a cell: flat, runs of spaces closed up, and a very long reason cut (the
// full text is in the log the row points to).
std::string reason(std::string_view s) {
    constexpr std::size_t kMax = 2000;
    std::string out;
    for (char c : flat(s)) {
        if (c == ' ' && (out.empty() || out.back() == ' ')) continue;
        out += c;
    }
    while (!out.empty() && out.back() == ' ') out.pop_back();
    if (out.size() > kMax) out = out.substr(0, kMax) + " ...";
    return out;
}

// This machine's hostname when the caller gave none (the CLI passes the hardware record's, which
// prefers host_label.local / $JAC313_HOST_LABEL over the real name); "" when unknown.
std::string machine_hostname() {
    char buf[256] = {};
    if (::gethostname(buf, sizeof buf - 1) != 0) return {};
    return buf;
}

std::string meta_value(const std::map<std::string, std::string>& meta, const std::string& key) {
    const auto it = meta.find(key);
    return it == meta.end() ? std::string{} : it->second;
}

// A result's reason: its own "error", else its read-back's.
std::string result_error(const SP::Part& p) {
    std::string e = SP::get(p, "error");
    if (e.empty()) e = SP::get(p, "readback.error");
    return reason(e);
}

// The results section: column name and the part key it is read from ("" = written by hand).
struct Column { const char* name; const char* key; };
constexpr Column kResultColumns[] = {
    {"id", ""},                         {"gate", "gate"},
    {"test", "test"},                   {"backend", "backend"},
    {"output_mode", "output_mode"},     {"status", "status"},
    {"ms", "ms"},                       {"rows_written", "rows_written"},
    {"compiler", "compiler"},           {"run_id", "run_id"},
    {"host", "host"},                   {"os", "os"},
    {"error", ""},                      {"log", "log"},
    {"rb_mode", "readback.mode"},       {"rb_status", "readback.status"},
    {"rb_count", "readback.count"},     {"rb_checksum", "readback.checksum"},
    {"rb_sample", "readback.sample"},   {"rb_checked", ""},
    {"rb_ping_ms", "readback.ping_ms"}, {"rb_write_ms", "readback.write_ms"},
    {"dataset_backend", "dataset.backend"}, {"dataset_base", "dataset.base"},
    {"extra", ""},
};

// Keys a results column already holds, so "extra" leaves them out.
std::set<std::string> column_keys() {
    std::set<std::string> k{"error", "readback.error", "readback.checked", "readback.rows_checked"};
    for (const Column& c : kResultColumns)
        if (*c.key) k.insert(c.key);
    return k;
}

void write_fields(jac313::jText::v001::JTextWriter& w, const std::vector<std::vector<std::string>>& f) {
    using namespace jac313::jText::v001;
    w.begin_fields();
    for (std::size_t i = 0; i < f.size(); ++i) {
        JTextEntry e;
        e.number = i + 1;
        e.delimiter = U'#';
        e.level_sep = U'/';
        for (const auto& level : f[i]) e.fields.emplace_back(level);
        w.append_entry(e);
    }
    w.begin_data();
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

std::string persist_cli_suffix(const GlobalOptions& g) {
    std::string s;
    if (!g.persist_list.empty())  s += " --persist " + g.persist_list;
    if (g.max_persist_rows)       s += " --max-persist-rows " + std::to_string(*g.max_persist_rows);
    if (g.max_runs)               s += " --max-runs " + std::to_string(g.max_runs);
    if (g.max_records)            s += " --max-records " + std::to_string(g.max_records);
    if (!g.readback.empty())      s += " --readback " + g.readback;
    if (!g.pg_cleanup.empty())    s += " --pg-cleanup " + g.pg_cleanup;
    if (!g.summary_path.empty())  s += " --summary \"" + g.summary_path.string() + "\"";
    if (!g.size_override.empty()) s += " --size " + g.size_override;
    return s;
}

std::string bench_persist_args(const GlobalOptions& g) {
    std::string s;
    if (!g.production.empty())    s += " --production " + g.production;
    if (!g.persist_list.empty())  s += " --persist-list " + g.persist_list;
    if (g.max_persist_rows)       s += " --max-persist-rows " + std::to_string(*g.max_persist_rows);
    if (g.max_runs)               s += " --max-runs " + std::to_string(g.max_runs);
    if (g.max_records)            s += " --max-records " + std::to_string(g.max_records);
    if (!g.readback.empty())      s += " --readback " + g.readback;
    if (!g.pg_cleanup.empty())    s += " --pg-cleanup " + g.pg_cleanup;
    if (!g.summary_path.empty())  s += " --summary-parts \"" + summary_parts_path(g).string() + "\"";
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
    if (g.summary_path.empty()) return {};
    return fs::path(g.summary_path.string() + ".parts");
}

void append_summary_part(const GlobalOptions& g, const SP::Part& part) {
    const auto p = summary_parts_path(g);
    if (p.empty()) return;
    std::error_code ec;
    fs::create_directories(p.parent_path(), ec);
    std::ofstream out(p, std::ios::app);
    if (out) out << SP::format(part) << '\n';
}

void add_readback_fields(SP::Part& part, const std::map<std::string, std::string>& rb) {
    for (const auto& [k, v] : rb)
        if (k != "backend") part.emplace_back("readback." + k, v);
}

bool assemble_summary(const GlobalOptions& g, const std::map<std::string, std::string>& meta,
                      const SettingList& settings) {
    using namespace jac313::jText::v001;
    if (g.summary_path.empty()) return true;
    std::vector<SP::Part> parts;
    {
        std::ifstream in(summary_parts_path(g));
        for (std::string line; std::getline(in, line);)
            if (!line.empty()) parts.push_back(SP::parse(line));
    }
    long pass = 0, fail = 0;
    std::string run_id, host, os;
    for (const auto& p : parts) {
        const std::string status = SP::get(p, "status");
        if (status == "PASS") ++pass;
        else if (status != "SKIP") ++fail;
        // the last recorded run id / host / os win (the summary's run row shows the latest)
        if (auto r = SP::get(p, "run_id"); !r.empty() && r != "0") run_id = r;
        if (auto h = SP::get(p, "host"); !h.empty()) host = h;
        if (auto o = SP::get(p, "os"); !o.empty()) os = o;
    }
    if (os.empty()) os = meta_value(meta, "os");   // nothing recorded: the OS the CLI sees

    std::string hostname = meta_value(meta, "hostname");
    if (hostname.empty()) hostname = machine_hostname();
    const fs::path& path = g.summary_path;
    const std::string name = path.filename().string();
    std::ostringstream out;
    out << "//File:    " << name << "\n"
        << "//Purpose: The last Jac313Runner / preset run on this machine, written by jac313_test_cli\n"
        << "//         when the run ends (Jac313Runner reads it; so can a tool that collects results).\n"
        << "//         run: one row (contract " << kRunnerContract << "). settings: what the run was given.\n"
        << "//         results: one row per result. failures: the results whose status is not PASS or\n"
        << "//         SKIP, with why (no rows when nothing failed).\n"
        << "//         extra: the result's other values, \"key=value; key=value\".\n"
        << "//\n";
    {
        JTextWriter w(out);
        w.set_bad_row_policy(BadRowPolicy::Skip);   // a strange message must never stop the CLI
        w.set_profile(JTextProfile::Full);
        w.set_filename(name);
        w.set_purpose("Run summary (jac313_test_cli, read by Jac313Runner)");
        // Where and what ran, as comment lines in the header (a reader gets them as the file's
        // header comments without reading a section; the run section holds the same as data).
        auto spec = [&](const char* key) { const std::string v = flat(meta_value(meta, key)); return v.empty() ? std::string("-") : v; };
        w.add_header_comment_line(0, "Host:     " + (host.empty() ? std::string("-") : flat(host))
                                     + "  (hostname " + (hostname.empty() ? std::string("-") : flat(hostname)) + ")");
        w.add_header_comment_line(0, "Run id:   " + (run_id.empty() ? std::string("-") : run_id));
        w.add_header_comment_line(0, "OS:       " + (os.empty() ? std::string("-") : flat(os)));
        w.add_header_comment_line(0, "CPU:      " + spec("cpu_model") + ", " + spec("cores") + " threads, "
                                     + spec("physical_cores") + " cores, max " + spec("cpu_mhz_max") + " MHz");
        w.add_header_comment_line(0, "Memory:   " + spec("ram_gb") + " GB");
        w.add_header_comment_line(0, "Disk:     " + spec("disk"));
        w.add_header_comment_line(0, "Compiler: " + spec("compiler") + "  (" + spec("build_type") + ", " + spec("version") + ")");
        w.add_header_comment_line(0, "Result:   " + std::to_string(pass) + " pass, " + std::to_string(fail) + " fail");
        w.write_header();

        w.begin_section("run");
        write_fields(w, {{"id", "Integer", "Not Null"},          {"contract", "Integer", "Not Null"},
                         {"run_id", "String", "Not Null"},       {"host", "String", "Not Null"},
                         {"hostname", "String", "Not Null"},     {"os", "String", "Not Null"},
                         {"kernel", "String", "Not Null"},       {"cpu_model", "String", "Not Null"},
                         {"cores", "String", "Not Null"},        {"physical_cores", "String", "Not Null"},
                         {"ram_gb", "String", "Not Null"},       {"cpu_mhz_max", "String", "Not Null"},
                         {"disk", "String", "Not Null"},           {"version", "String", "Not Null"},
                         {"compiler", "String", "Not Null"},     {"build_type", "String", "Not Null"},
                         {"contracts", "String", "Not Null"},    {"pg_remote", "String", "Not Null"},
                         {"started_utc", "String", "Not Null"},  {"finished_utc", "String", "Not Null"},
                         {"command", "String", "Not Null"},      {"pass", "Integer", "Not Null"},
                         {"fail", "Integer", "Not Null"}});
        w.append_row(1, {"1", std::to_string(kRunnerContract), run_id, flat(host), flat(hostname), flat(os),
                         flat(meta_value(meta, "kernel")), flat(meta_value(meta, "cpu_model")),
                         flat(meta_value(meta, "cores")), flat(meta_value(meta, "physical_cores")),
                         flat(meta_value(meta, "ram_gb")), flat(meta_value(meta, "cpu_mhz_max")),
                         flat(meta_value(meta, "disk")),
                         flat(meta_value(meta, "version")), flat(meta_value(meta, "compiler")),
                         flat(meta_value(meta, "build_type")), flat(meta_value(meta, "contracts")),
                         flat(meta_value(meta, "pg_remote")), flat(meta_value(meta, "started_utc")),
                         flat(meta_value(meta, "finished_utc")), flat(meta_value(meta, "command")),
                         std::to_string(pass), std::to_string(fail)});
        w.end_record();

        w.begin_section("settings");
        write_fields(w, {{"id", "Integer", "Not Null"}, {"name", "String", "Not Null"},
                         {"value", "String", "Not Null"}});
        for (std::size_t i = 0; i < settings.size(); ++i) {
            w.append_row(1, {std::to_string(i + 1), flat(settings[i].first), flat(settings[i].second)});
            w.end_record();
        }

        w.begin_section("results");
        {
            std::vector<std::vector<std::string>> f;
            for (const Column& c : kResultColumns)
                f.push_back({c.name, std::string_view(c.name) == "id" ? "Integer" : "String", "Not Null"});
            write_fields(w, f);
        }
        const std::set<std::string> in_columns = column_keys();
        for (std::size_t n = 0; n < parts.size(); ++n) {
            const SP::Part& p = parts[n];
            std::vector<std::string> row;
            for (const Column& c : kResultColumns) {
                const std::string_view col = c.name;
                if (col == "id") row.push_back(std::to_string(n + 1));
                else if (col == "error") row.push_back(result_error(p));
                else if (col == "rb_checked")
                    row.push_back(flat(SP::get(p, "readback.checked", SP::get(p, "readback.rows_checked"))));
                else if (col == "extra") {
                    std::string extra;
                    for (const auto& [k, v] : p) {
                        if (in_columns.count(k)) continue;
                        extra += (extra.empty() ? "" : "; ") + k + "=" + flat(v);
                    }
                    row.push_back(extra);
                } else if (col == "backend") row.push_back(flat(SP::get(p, c.key, "-")));
                else row.push_back(flat(SP::get(p, c.key)));
            }
            w.append_row(1, row);
            w.end_record();
        }

        w.begin_section("failures");
        write_fields(w, {{"id", "Integer", "Not Null"},       {"host", "String", "Not Null"},
                         {"hostname", "String", "Not Null"}, {"gate", "String", "Not Null"},
                         {"test", "String", "Not Null"},     {"backend", "String", "Not Null"},
                         {"compiler", "String", "Not Null"}, {"status", "String", "Not Null"},
                         {"run_id", "String", "Not Null"},   {"error", "String", "Not Null"},
                         {"log", "String", "Not Null"}});
        std::size_t nf = 0;
        for (const auto& p : parts) {
            const std::string status = SP::get(p, "status");
            if (status == "PASS" || status == "SKIP") continue;
            w.append_row(1, {std::to_string(++nf), flat(SP::get(p, "host", host)), flat(hostname),
                             flat(SP::get(p, "gate")), flat(SP::get(p, "test")),
                             flat(SP::get(p, "backend", "-")), flat(SP::get(p, "compiler", meta_value(meta, "compiler"))),
                             status.empty() ? std::string("ERROR") : flat(status),
                             flat(SP::get(p, "run_id", run_id)), result_error(p), flat(SP::get(p, "log"))});
            w.end_record();
        }
        w.finalize();
    }

    // Write beside it, then rename over it: a reader never sees half a file.
    std::error_code ec;
    fs::create_directories(path.parent_path(), ec);
    const fs::path tmp = path.string() + ".tmp";
    {
        std::ofstream f(tmp, std::ios::trunc);
        f << out.str();
        if (!f.flush()) {
            std::cerr << "summary: cannot write " << tmp.string() << '\n';
            return false;
        }
    }
    fs::rename(tmp, path, ec);
    if (ec) {
        std::cerr << "summary: cannot write " << path.string() << ": " << ec.message() << '\n';
        return false;
    }
    std::cout << "[summary] wrote " << path.string() << " (" << parts.size() << " results: "
              << pass << " pass, " << fail << " fail)\n";
    return true;
}

} // namespace jac313::test_cli
