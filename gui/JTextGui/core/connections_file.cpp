#include "jtext_gui/core/connections_file.hpp"
#include "jtext_gui/core/record.hpp"

#include "jText.h"
#include "parser/section.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <format>
#include <optional>
#include <sstream>

namespace fs = std::filesystem;

namespace jtext_gui {

namespace {

std::string trim(const std::string& s) {
    const auto b = s.find_first_not_of(" \t\r");
    if (b == std::string::npos) return {};
    const auto e = s.find_last_not_of(" \t\r");
    return s.substr(b, e - b + 1);
}

std::string lower(std::string s) {
    for (char& c : s) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    return s;
}

// {NULL}, or the v0.7 marker U+001F the library reads as it (SPEC 2.6.1): a setting
// with no value. NULL and \N are text.
bool is_null(const std::string& v) {
    return v == jtext::kNullValue || v == "\x1f";
}

} // namespace

std::string default_connections_path(const std::string& start) {
    if (const char* e = std::getenv("JTEXT_GUI_CONNECTIONS"); e && *e) return e;
    const std::string git = enclosing_git_project(start);
    if (git.empty()) return {};
    return (fs::path(git).parent_path() / "SQL_Secrets" / "jtext_gui_connections.jtext").string();
}

std::string enclosing_git_project(const std::string& path) {
    std::error_code ec;
    fs::path d = fs::absolute(path, ec).lexically_normal();
    if (!fs::is_directory(d, ec)) d = d.parent_path();
    for (int guard = 0; guard < 64 && !d.empty(); ++guard) {
        if (fs::exists(d / ".git", ec)) return d.string();
        if (d == d.root_path()) break;
        d = d.parent_path();
    }
    return {};
}

bool load_connections(const std::string& path, std::vector<SavedConnection>& rows, std::string& err) {
    rows.clear();
    std::ifstream in(path);
    if (!in) { err = "cannot read " + path; return false; }
    const auto file = jtext::parse_file_structure(in);
    if (!file) {
        err = std::format("{}: line {}: {}", path, file.error().line_no, file.error().message);
        return false;
    }

    // The library parses. The first section whose Fields name a backend and a host holds
    // the servers: a record is the lines between blank lines, a line's number is its
    // field position.
    for (const auto& sec : file->sections) {
        std::vector<std::string> fields;
        for (const auto& fl : sec.field_lines) {
            if (fl.number == 0 || fl.values.empty()) continue;
            if (fields.size() < fl.number) fields.resize(fl.number);
            fields[fl.number - 1] = lower(trim(fl.values[0]));
        }
        auto col = [&](const char* name) {
            for (std::size_t i = 0; i < fields.size(); ++i) if (fields[i] == name) return static_cast<int>(i);
            return -1;
        };
        const int cb = col("backend"), ch = col("host"), cp = col("port"), cd = col("dbname"),
                  cu = col("user"), ct = col("connect_timeout"), cn = col("notes");
        if (cb < 0 || ch < 0) continue;

        std::vector<std::vector<std::string>> recs;
        std::vector<std::string> rec;
        for (const auto& pl : sec.data_lines) {
            // A blank line ends the record, and a line for field 1 starts the next: rows
            // that each hold a whole record follow one another directly (SPEC 2.6).
            if (pl.number == 0 || (pl.number == 1 && !rec.empty())) {
                if (!rec.empty()) recs.push_back(std::move(rec));
                rec.clear();
                if (pl.number == 0) continue;
            }
            const std::size_t first = pl.number - 1;
            if (rec.size() < first + pl.values.size()) rec.resize(first + pl.values.size(), "\x1f");
            for (std::size_t k = 0; k < pl.values.size(); ++k) rec[first + k] = pl.values[k];
        }
        if (!rec.empty()) recs.push_back(std::move(rec));

        for (const auto& r : recs) {
            auto get = [&](int c) -> std::string {
                return (c < 0 || static_cast<std::size_t>(c) >= r.size() || is_null(r[c])) ? std::string{} : r[c];
            };
            SavedConnection c{get(cb), get(ch), get(cp), get(cd), get(cu), get(ct), get(cn), {}};
            if (!c.backend.empty() && !c.host.empty()) rows.push_back(std::move(c));
        }
        if (rows.empty()) { err = path + ": no record with a backend and a host"; return false; }
        return true;
    }
    err = path + ": no backend / host fields";
    return false;
}

std::string connections_text(const std::vector<SavedConnection>& rows) {
    using namespace jText::v001;
    std::ostringstream t;
    t << "//File:    jtext_gui_connections.jtext\n"
         "//Purpose: Saved database servers for jText_GUI's connect dialog.\n"
         "//         Keep this file OUTSIDE every git project. No passwords here: PostgreSQL reads\n"
         "//         them from ~/.pgpass or $PGPASSFILE; the others are typed in the dialog.\n"
         "//\n";
    JTextWriter w(t);
    w.set_profile(JTextProfile::Full);
    w.set_filename("jtext_gui_connections.jtext");
    w.set_purpose("Database servers for jText_GUI");
    w.write_header();
    w.begin_section("connections");
    w.begin_fields();
    static const char* const fields[][2] = {
        {"id/Identity(1,1)/Not Null", "record id"},
        {"backend/String/Not Null", "pg-local | pg-remote | mariadb-local | mariadb-remote | mssql-..."},
        {"host/String/Not Null", "host name or IP address"},
        {"port/Number", ""},
        {"dbname/String", ""},
        {"user/String", ""},
        {"connect_timeout/Number", "seconds"},
        {"notes/String", ""},
    };
    for (std::size_t i = 0; i < std::size(fields); ++i) {
        std::vector<std::string> levels;
        for (std::string_view def = fields[i][0];;) {
            const auto slash = def.find('/');
            levels.emplace_back(def.substr(0, slash));
            if (slash == std::string_view::npos) break;
            def.remove_prefix(slash + 1);
        }
        w.append_row(i + 1, levels, fields[i][1], U'/');
    }
    w.begin_data();
    // Number fields need a value: an empty port / timeout is written as NULL too.
    auto val = [](const std::string& v) -> std::optional<std::string> {
        if (v.empty()) return std::nullopt;
        return v;
    };
    for (std::size_t i = 0; i < rows.size(); ++i) {
        const auto& r = rows[i];
        JTextEntry e;
        e.number = 1;
        e.fields = {std::to_string(i + 1), r.backend, r.host, val(r.port), val(r.dbname), val(r.user),
                    val(r.connect_timeout), val(r.notes)};
        w.append_entry(e);
        w.end_record();
    }
    w.finalize();
    return t.str();
}

bool save_connections(const std::string& path, const std::vector<SavedConnection>& rows, std::string& err) {
    std::error_code ec;
    const fs::path p(path);
    if (!p.parent_path().empty() && !fs::exists(p.parent_path(), ec)) {
        if (!fs::create_directories(p.parent_path(), ec)) { err = "cannot create " + p.parent_path().string(); return false; }
        fs::permissions(p.parent_path(), fs::perms::owner_all, fs::perm_options::replace, ec);
    }
    const fs::path tmp = p.string() + ".tmp";
    {
        std::ofstream out(tmp, std::ios::trunc);
        if (!out) { err = "cannot write " + tmp.string(); return false; }
        out << connections_text(rows);
        if (!out) { err = "cannot write " + tmp.string(); return false; }
    }
    fs::permissions(tmp, fs::perms::owner_read | fs::perms::owner_write, fs::perm_options::replace, ec);
    fs::rename(tmp, p, ec);
    if (ec) { err = "cannot replace " + path + ": " + ec.message(); return false; }
    return true;
}

bool load_saved_servers(const std::string& path, std::vector<SavedConnection>& rows, std::string& err) {
    rows.clear();
    err.clear();
    std::string primary_err;
    if (!load_connections(path, rows, primary_err)) rows.clear();

    const fs::path dir = fs::path(path).parent_path();
    for (const char* name : {"jac313_connections.jtext", "jac314_connections.jtext"}) {
        if (dir.empty()) break;
        const fs::path sib = dir / name;
        if (sib == fs::path(path)) continue;
        std::vector<SavedConnection> extra;
        std::string ignored;
        if (!load_connections(sib.string(), extra, ignored)) continue;
        for (auto& r : extra) {
            bool dup = false;
            for (const auto& have : rows) {
                if (have.backend == r.backend && have.host == r.host && have.port == r.port &&
                    have.dbname == r.dbname) {
                    dup = true;
                    break;
                }
            }
            if (dup) continue;
            r.origin = name;
            rows.push_back(std::move(r));
        }
    }
    if (rows.empty()) {
        err = primary_err.empty() ? "no saved servers" : primary_err;
        return false;
    }
    return true;
}

void upsert_connection(std::vector<SavedConnection>& rows, const SavedConnection& c) {
    for (auto& r : rows)
        if (r.backend == c.backend) { r = c; return; }
    rows.push_back(c);
}

} // namespace jtext_gui
