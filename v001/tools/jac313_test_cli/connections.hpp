#pragma once

// connections.hpp — the PostgreSQL servers for pg-local / pg-remote, kept in a jText file
// OUTSIDE every git project (default <projects dir>/SQL_Secrets/jac313_connections.jtext,
// i.e. a SQL_Secrets folder beside the jac313 checkout; $JAC313_CONNECTIONS overrides).
// The file never holds a password: libpq reads those from ~/.pgpass or $PGPASSFILE.
//
// One section, one record per server:
//   1 id/Identity(1,1)  2 backend (pg-local | pg-remote)  3 host  4 port  5 dbname
//   6 user  7 connect_timeout  8 notes
//
// Precedence for a backend's server: --pg-remote / --pg-local, then --connections FILE,
// then $JAC313_PG_REMOTE / $JAC313_PG_LOCAL, then the default file.

#include <filesystem>
#include <string>
#include <vector>

namespace jac313::test_cli {

struct PgConnection {
    std::string backend;          // "pg-local" | "pg-remote"
    std::string host;
    std::string port;             // "" = 5432
    std::string dbname;
    std::string user;
    std::string connect_timeout;  // seconds ("" = 10)
    std::string notes;
    std::string conninfo() const; // libpq keyword=value form (no password)
};

// $JAC313_CONNECTIONS, else <project root>/../SQL_Secrets/jac313_connections.jtext, where the
// project root is the parent of the version dir `version_dir` (e.g. jac315/v001 -> jac315).
std::filesystem::path default_connections_path(const std::filesystem::path& version_dir);

// Read with jText's own parser + validator. False (and `err`) when the file cannot be read,
// is not valid jText, or has no usable rows.
bool load_connections(const std::filesystem::path& file, std::vector<PgConnection>& out, std::string& err);

// Set $JAC313_PG_REMOTE / $JAC313_PG_LOCAL from the file's rows. With overwrite=false a value
// already in the environment wins; `skip_remote` / `skip_local` keep what a --pg-* flag set.
void apply_connections(const std::vector<PgConnection>& rows, bool overwrite,
                       bool skip_remote, bool skip_local);

// The git work tree that contains `p` ("" when none): the connections file must not be in one.
std::filesystem::path enclosing_git_project(const std::filesystem::path& p);

// A blank file to fill in (`connections --init`).
std::string connections_template();

// `jac313_test_cli connections [--file P] [--init]`: show the file, its rows and its check line.
int run_connections_command(int argc, char** argv, int first_arg);

} // namespace jac313::test_cli
