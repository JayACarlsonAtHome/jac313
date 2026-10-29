#include "connections.hpp"

#ifdef JAC313_TEST_CLI_HAS_JTEXT
#include "parser/section.hpp"
#include "validator/validator.hpp"
#endif

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

namespace jac313::test_cli {

namespace {

// libpq conninfo value: quoted when it holds a space, quote or backslash.
std::string conn_word(const std::string& v) {
    if (v.find_first_of(" '\\") == std::string::npos) return v;
    std::string q = "'";
    for (char c : v) {
        if (c == '\'' || c == '\\') q += '\\';
        q += c;
    }
    return q + "'";
}

std::string lower(std::string s) {
    for (char& c : s) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    return s;
}

} // namespace

std::string PgConnection::conninfo() const {
    std::string c = "host=" + conn_word(host);
    if (!port.empty()) c += " port=" + conn_word(port);
    if (!dbname.empty()) c += " dbname=" + conn_word(dbname);
    if (!user.empty()) c += " user=" + conn_word(user);
    c += " connect_timeout=" + (connect_timeout.empty() ? std::string("10") : connect_timeout);
    return c;
}

fs::path default_connections_path(const fs::path& version_dir) {
    if (const char* e = std::getenv("JAC313_CONNECTIONS"); e && *e) return fs::path(e);
    std::error_code ec;
    fs::path v = fs::absolute(version_dir, ec).lexically_normal();
    if (v.filename().empty()) v = v.parent_path();
    // <projects>/<project root>/<vNNN>: the secrets folder sits beside the project root
    const fs::path root = v.parent_path();
    return root.parent_path() / "SQL_Secrets" / "jac313_connections.jtext";
}

fs::path enclosing_git_project(const fs::path& p) {
    std::error_code ec;
    fs::path d = fs::absolute(p, ec).lexically_normal().parent_path();
    for (int guard = 0; guard < 64 && !d.empty(); ++guard) {
        if (fs::exists(d / ".git", ec)) return d;
        if (d == d.root_path()) break;
        d = d.parent_path();
    }
    return {};
}

bool load_connections(const fs::path& file, std::vector<PgConnection>& out, std::string& err) {
    out.clear();
    std::ifstream in(file);
    if (!in) { err = "cannot read " + file.string(); return false; }
#ifndef JAC313_TEST_CLI_HAS_JTEXT
    err = "this jac313_test_cli was built without jText";
    return false;
#else
    std::stringstream ss;
    ss << in.rdbuf();
    const std::string text = ss.str();
    auto pf = jtext::parse_file_structure(std::string_view(text));
    if (!pf) { err = file.string() + ": " + pf.error().to_string(); return false; }
    const auto vr = jtext::validate(*pf);
    if (vr.report.error_count() > 0) {
        err = file.string() + ": " + std::to_string(vr.report.error_count()) + " jText error(s)";
        for (const auto& is : vr.report.issues) { err += "\n  " + is.to_string(); }
        return false;
    }
    for (const auto& sec : vr.file.sections) {
        auto col = [&](std::string_view name) -> int {
            for (std::size_t i = 0; i < sec.fields.size(); ++i)
                if (lower(sec.fields[i].name) == name) return static_cast<int>(i);
            return -1;
        };
        const int cb = col("backend"), ch = col("host"), cp = col("port"), cd = col("dbname"),
                  cu = col("user"), ct = col("connect_timeout"), cn = col("notes");
        if (cb < 0 || ch < 0) continue;   // not a connections section
        for (const auto& rec : sec.records) {
            auto get = [&](int c) -> std::string {
                if (c < 0 || c >= static_cast<int>(rec.values.size())) return {};
                const std::string& v = rec.values[c];
                return v == jtext::kNullValue ? std::string{} : v;   // {NULL}: no value (SPEC 2.6.1)
            };
            PgConnection pc{get(cb), get(ch), get(cp), get(cd), get(cu), get(ct), get(cn)};
            if (pc.backend.empty() || pc.host.empty()) continue;
            out.push_back(std::move(pc));
        }
    }
    if (out.empty()) { err = file.string() + ": no rows with a backend and a host"; return false; }
    return true;
#endif
}

void apply_connections(const std::vector<PgConnection>& rows, bool overwrite,
                       bool skip_remote, bool skip_local) {
    for (const auto& r : rows) {
        const bool remote = r.backend == "pg-remote";
        if (!remote && r.backend != "pg-local") continue;
        if ((remote && skip_remote) || (!remote && skip_local)) continue;
        const char* var = remote ? "JAC313_PG_REMOTE" : "JAC313_PG_LOCAL";
        ::setenv(var, r.conninfo().c_str(), overwrite ? 1 : 0);
    }
}

void apply_password_files(const fs::path& secrets_dir) {
    for (const bool remote : {false, true}) {
        const fs::path pf = secrets_dir / (remote ? "jac313_pg_remote.pgpass" : "jac313_pg_local.pgpass");
        std::error_code ec;
        if (!fs::is_regular_file(pf, ec)) continue;
        const char* var = remote ? "JAC313_PG_REMOTE" : "JAC313_PG_LOCAL";
        const char* cur = std::getenv(var);
        std::string ci = (cur && *cur) ? cur : "";
        if (ci.empty()) {
            if (remote) continue;   // pg-remote has no default server
            ci = "host=localhost dbname=jac313 user=jac313 connect_timeout=10";
        }
        if (ci.find("passfile=") != std::string::npos) continue;
        if (ci.rfind("postgresql://", 0) == 0 || ci.rfind("postgres://", 0) == 0) continue;   // a URI takes no key=value
        ci += " passfile=" + conn_word(pf.string());
        ::setenv(var, ci.c_str(), 1);
    }
}

std::string connections_template() {
    return
        "//File:    jac313_connections.jtext\n"
        "//Purpose: PostgreSQL servers for jac313 test runs (pg-local / pg-remote).\n"
        "//         Keep this file OUTSIDE every git project. No passwords here: they go in\n"
        "//         jac313_pg_local.pgpass / jac313_pg_remote.pgpass beside it. Jac313Runner writes it; safe to edit by hand.\n"
        "//         Fill in pg-remote's host (and dbname / user if they differ), or delete that row.\n"
        "//\n"
        "=== jText File ===\n"
        " 1. #?# jac313_connections\n"
        " 3. #?# PostgreSQL connections for jac313 / Jac313Runner\n"
        "\n\n"
        "=== Section: connections ===\n"
        "\n\n"
        "=== Fields ===\n"
        " 1. #/# id/Identity(1,1)/Not Null     # record id\n"
        " 2. #/# backend/String/Not Null       # pg-local | pg-remote\n"
        " 3. #/# host/String/Not Null          # host name or IP address\n"
        " 4. #/# port/Number                   # 5432 is PostgreSQL's usual port\n"
        " 5. #/# dbname/String\n"
        " 6. #/# user/String\n"
        " 7. #/# connect_timeout/Number        # seconds\n"
        " 8. #/# notes/String\n"
        "=== End Fields ===\n"
        "\n\n"
        "=== Data ===\n"
        "1. #|# 1|pg-local|localhost|5432|jac313|jac313|10|this machine\n"
        "\n"
        "1. #|# 2|pg-remote|your-server.example|5432|jac313|jac313|10|your own server\n"
        "\n"
        "=== End Data ===\n"
        "=== End Section ===\n"
        "=== End File ===\n";
}

int run_connections_command(int argc, char** argv, int first_arg) {
    fs::path file;
    bool init = false;
    for (int i = first_arg; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--file" && i + 1 < argc) file = argv[++i];
        else if (a == "--init") init = true;
        else if (a == "--help" || a == "-h") {
            std::cout << "connections [--file PATH] [--init]\n"
                         "  Show the PostgreSQL connections file (default: a SQL_Secrets folder beside\n"
                         "  the project, or $JAC313_CONNECTIONS), its rows and its jText check line.\n"
                         "  --init writes a blank file to fill in when none exists.\n";
            return 0;
        } else {
            std::cerr << "connections: unknown option " << a << '\n';
            return 2;
        }
    }
    if (file.empty()) file = default_connections_path(fs::current_path());
    std::cout << "connections file: " << file.string() << '\n';
    if (const auto g = enclosing_git_project(file); !g.empty())
        std::cout << "WARNING: this file is inside the git project " << g.string()
                  << " - move it out, so it is never committed\n";
    std::error_code ec;
    if (!fs::exists(file, ec)) {
        if (!init) {
            std::cout << "not found (create it with: jac313_test_cli connections --init)\n";
            return 1;
        }
        fs::create_directories(file.parent_path(), ec);
        fs::permissions(file.parent_path(), fs::perms::owner_all, fs::perm_options::replace, ec);
        std::ofstream(file) << connections_template();
        fs::permissions(file, fs::perms::owner_read | fs::perms::owner_write, fs::perm_options::replace, ec);
        std::cout << "created a blank file: fill in pg-remote's host\n";
    }
    std::vector<PgConnection> rows;
    std::string err;
    if (!load_connections(file, rows, err)) {
        std::cout << err << "\nVALIDATION errors=1\n";
        return 1;
    }
    for (const auto& r : rows) std::cout << "  " << r.backend << "  " << r.conninfo()
                                         << (r.notes.empty() ? "" : "   # " + r.notes) << '\n';
    std::cout << "VALIDATION errors=0\n";
    return 0;
}

} // namespace jac313::test_cli
