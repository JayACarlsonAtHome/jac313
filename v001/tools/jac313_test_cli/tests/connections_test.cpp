// The connections file (connections.cpp): a file written before the name column existed reads
// with every name '' and is written back with the column; a server's name, when it has one, is
// what results.db records as run_target.server (else its host); --pg-remote to the same host keeps
// the name, to another host drops it.

#include "connections.hpp"
#include "jac313_results_db.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>
#include <unistd.h>

namespace fs = std::filesystem;
using namespace jac313::test_cli;

namespace {

int failures = 0;

void check(bool ok, std::string_view what, std::source_location at = std::source_location::current()) {
    if (ok) return;
    std::cerr << "FAIL: " << what << " at " << at.file_name() << ":" << at.line() << "\n";
    ++failures;
}

std::string slurp(const fs::path& p) {
    std::ifstream in(p);
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

// A file as the Runner wrote it before the name column: eight fields.
const char* const kOldFile =
    "//File:    jac313_connections.jtext\n"
    "//Date:    2026-10-23\n"
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
    "1. #|# 2|pg-remote|192.168.1.26|5432|jac313|jac313|10|\n"
    "\n"
    "=== End Data ===\n"
    "=== End Section ===\n"
    "=== End File ===\n";

} // namespace

int main() {
    const fs::path dir = fs::temp_directory_path() / ("jac313_connections_test_" + std::to_string(::getpid()));
    fs::create_directories(dir);
    const fs::path file = dir / "jac313_connections.jtext";
    std::ofstream(file) << kOldFile;

    // A file without the name column reads, every name ''.
    std::vector<PgConnection> rows;
    std::string err;
    check(load_connections(file, rows, err), "a file without the name column reads: " + err);
    check(rows.size() == 2 && rows[0].name.empty() && rows[1].name.empty(), "every name is '' without the column");
    check(rows.size() == 2 && rows[1].id == "2" && rows[1].host == "192.168.1.26", "the rows keep their ids and hosts");

    // Written back with the column (last), the comment lines kept; it reads again with the names.
    if (rows.size() == 2) { rows[0].name = "local"; rows[1].name = "r430"; }
    check(save_connections(file, rows, err), "save_connections writes: " + err);
    const std::string text = slurp(file);
    check(text.find(" 9. #/# name/String") != std::string::npos, "the name column is written as field 9");
    check(text.find("1. #|# 2|pg-remote|192.168.1.26|5432|jac313|jac313|10||r430\n") != std::string::npos,
          "the name is the last value of its row");
    check(text.rfind("//File:    jac313_connections.jtext\n//Date:    2026-10-23\n//\n=== jText File ===", 0) == 0,
          "the file's own comment lines are kept");
    std::vector<PgConnection> again;
    check(load_connections(file, again, err), "the written file reads (and validates): " + err);
    check(again.size() == 2 && again[0].name == "local" && again[1].name == "r430", "the names read back");

    // Saving again changes nothing.
    check(save_connections(file, again, err) && slurp(file) == text, "a second save writes the same text");

    // The server's name, not its host and never its id, names it in results.db.
    for (const char* v : {"JAC313_PG_REMOTE", "JAC313_PG_LOCAL", "JAC313_PG_REMOTE_NAME", "JAC313_PG_LOCAL_NAME"})
        ::unsetenv(v);
    apply_connections(again, true, false, false);
    check(jac313::results::pg_server_name("pg-remote", "192.168.1.26") == "r430", "the name wins over the host");
    check(jac313::results::pg_server_name("pg-local", "localhost") == "local", "pg-local has its own name");

    // --pg-remote to the same host keeps the name; to another host the host names it.
    replace_pg_server(true, "host=192.168.1.26 port=5432 dbname=jac313 user=jac313 connect_timeout=10");
    check(jac313::results::pg_server_name("pg-remote", "192.168.1.26") == "r430", "the same host keeps the name");
    replace_pg_server(true, "host='db two' dbname=x");
    check(conninfo_host("host='db two' dbname=x") == "db two", "conninfo_host reads a quoted host");
    check(jac313::results::pg_server_name("pg-remote", "db two") == "db two", "another host is named by its host");

    // A row without a name: its host names it.
    again[1].name.clear();
    apply_connections(again, true, false, false);
    check(jac313::results::pg_server_name("pg-remote", "192.168.1.26") == "192.168.1.26", "no name: the host");

    // The --init template validates and names the local server.
    std::ofstream(file, std::ios::trunc) << connections_template();
    check(load_connections(file, rows, err) && rows.size() == 2 && rows[0].name == "local", "the template reads: " + err);

    std::error_code ec;
    fs::remove_all(dir, ec);
    if (failures) { std::cerr << failures << " failure(s)\n"; return 1; }
    std::cout << "connections_test: OK\n";
    return 0;
}
