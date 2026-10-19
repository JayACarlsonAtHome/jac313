#include "jtext_gui/core/connections_file.hpp"
#include "jtext_gui/core/record.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
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

bool is_null(const std::string& v) {
    return v == "\x1f" || v == "\\x1F" || v == "\\x1f" || v == "\\N" || v == "NULL";
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

    // Enough jText for this file: field lines in "=== Fields ===", compact rows
    // "1. |# v1|v2..." and per-field rows "N. ?# value" (a blank line ends a record).
    // An older three-character marker is read as its right two characters.
    std::vector<std::string> fields;
    std::map<int, std::string> rec;
    std::vector<std::map<int, std::string>> recs;
    enum { none, in_fields, in_data } state = none;
    auto flush = [&] { if (!rec.empty()) { recs.push_back(rec); rec.clear(); } };
    for (std::string line; std::getline(in, line);) {
        const std::string t = trim(line);
        if (t.rfind("===", 0) == 0) {
            flush();
            state = t == "=== Fields ===" ? in_fields : t == "=== Data ===" ? in_data : none;
            continue;
        }
        if (t.empty()) { flush(); continue; }
        if (state == none || t.rfind("//", 0) == 0) continue;
        const auto dot = t.find('.');
        if (dot == std::string::npos || dot == 0) continue;
        int num = 0;
        bool digits = true;
        for (std::size_t i = 0; i < dot; ++i) {
            if (t[i] < '0' || t[i] > '9') { digits = false; break; }
            num = num * 10 + (t[i] - '0');
        }
        if (!digits) continue;
        const std::string rest = trim(t.substr(dot + 1));
        const MarkerRun mark = take_marker(rest);
        if (mark.text.size() != 2) continue;
        const char sep = mark.text[0];
        std::string body = rest.substr(mark.consumed);
        if (!body.empty() && body[0] == ' ') body.erase(0, 1);
        if (state == in_fields) {
            fields.push_back(lower(trim(body.substr(0, body.find_first_of("/#")))));
            continue;
        }
        if (sep == '?' || sep == '/') { rec[num] = trim(body); continue; }
        flush();
        std::size_t start = 0;
        for (int k = 0;; ++k) {
            const auto at = body.find(sep, start);
            rec[num + k] = body.substr(start, at == std::string::npos ? std::string::npos : at - start);
            if (at == std::string::npos) break;
            start = at + 1;
        }
        flush();
    }
    flush();

    auto col = [&](const char* name) {
        for (std::size_t i = 0; i < fields.size(); ++i) if (fields[i] == name) return static_cast<int>(i) + 1;
        return -1;
    };
    const int cb = col("backend"), ch = col("host"), cp = col("port"), cd = col("dbname"),
              cu = col("user"), ct = col("connect_timeout"), cn = col("notes");
    if (cb < 0 || ch < 0) { err = path + ": no backend / host fields"; return false; }
    for (const auto& r : recs) {
        auto get = [&](int c) -> std::string {
            auto it = r.find(c);
            return (it == r.end() || is_null(it->second)) ? std::string{} : it->second;
        };
        SavedConnection c{get(cb), get(ch), get(cp), get(cd), get(cu), get(ct), get(cn), {}};
        if (!c.backend.empty() && !c.host.empty()) rows.push_back(std::move(c));
    }
    if (rows.empty()) { err = path + ": no record with a backend and a host"; return false; }
    return true;
}

std::string connections_text(const std::vector<SavedConnection>& rows) {
    char sep = '|';
    for (char c : {'|', '~', '^', '`'}) {
        bool clash = false;
        for (const auto& r : rows)
            for (const std::string* v : {&r.backend, &r.host, &r.port, &r.dbname, &r.user, &r.connect_timeout, &r.notes})
                if (v->find(c) != std::string::npos) clash = true;
        if (!clash) { sep = c; break; }
    }
    auto val = [](const std::string& v) { return v.empty() ? std::string("\x1f") : v; };
    std::ostringstream t;
    t << "//File:    jtext_gui_connections.jtext\n"
         "//Purpose: Saved database servers for jText_GUI's connect dialog.\n"
         "//         Keep this file OUTSIDE every git project. No passwords here: PostgreSQL reads\n"
         "//         them from ~/.pgpass or $PGPASSFILE; the others are typed in the dialog.\n"
         "//\n"
         "=== jText File ===\n"
         " 1. ?# jtext_gui_connections\n"
         " 3. ?# Database servers for jText_GUI\n\n\n"
         "=== Section: connections ===\n\n\n"
         "=== Fields ===\n"
         " 1. /# id/Identity(1,1)/Not Null     # record id\n"
         " 2. /# backend/String/Not Null       # pg-local | pg-remote | mariadb-local | mariadb-remote | mssql-...\n"
         " 3. /# host/String/Not Null          # host name or IP address\n"
         " 4. /# port/Number\n"
         " 5. /# dbname/String\n"
         " 6. /# user/String\n"
         " 7. /# connect_timeout/Number        # seconds\n"
         " 8. /# notes/String\n"
         "=== End Fields ===\n\n\n"
         "=== Data ===\n";
    for (std::size_t i = 0; i < rows.size(); ++i) {
        const auto& r = rows[i];
        // Number fields need a value: an empty port / timeout is written as NULL too.
        t << "1. " << sep << "# " << (i + 1) << sep << r.backend << sep << r.host << sep << val(r.port) << sep
          << val(r.dbname) << sep << val(r.user) << sep << val(r.connect_timeout) << sep << val(r.notes) << "\n\n";
    }
    t << "=== End Data ===\n=== End Section ===\n=== End File ===\n";
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
