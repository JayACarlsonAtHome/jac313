#pragma once

// connections.hpp — the PostgreSQL servers for pg-local / pg-remote, kept in a jText file
// OUTSIDE every git project (default <projects dir>/SQL_Secrets/jac313_connections.jtext,
// i.e. a SQL_Secrets folder beside the jac313 checkout; $JAC313_CONNECTIONS overrides).
// The file never holds a password: libpq reads those from the password files beside it
// (apply_password_files), else ~/.pgpass or $PGPASSFILE.
//
// One section, one record per server:
//   1 id/Identity(1,1)  2 backend (pg-local | pg-remote)  3 host  4 port  5 dbname
//   6 user  7 connect_timeout  8 notes  9 name
// `name` is what every report calls the server (results.db run_target.server); '' = its host.
// It is the last field, so a file written before it existed still reads (every name '').
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
    std::string id;               // the row's record id
    std::string name;             // the server's name on every report ('' = its host)
    std::string conninfo() const; // libpq keyword=value form (no password)
};

// $JAC313_CONNECTIONS, else <project root>/../SQL_Secrets/jac313_connections.jtext, where the
// project root is the parent of the version dir `version_dir` (e.g. jac315/v001 -> jac315).
std::filesystem::path default_connections_path(const std::filesystem::path& version_dir);

// Read with jText's own parser + validator. False (and `err`) when the file cannot be read,
// is not valid jText, or has no usable rows.
bool load_connections(const std::filesystem::path& file, std::vector<PgConnection>& out, std::string& err);

// Set $JAC313_PG_REMOTE / $JAC313_PG_LOCAL from the file's rows, and $JAC313_PG_REMOTE_NAME /
// $JAC313_PG_LOCAL_NAME to the row's name (results.db names the server by it; unset when ''). With
// overwrite=false a value already in the environment wins; `skip_remote` / `skip_local` keep what
// a --pg-* flag set.
void apply_connections(const std::vector<PgConnection>& rows, bool overwrite,
                       bool skip_remote, bool skip_local);

// A --pg-remote / --pg-local conninfo replaces the server: its name ($JAC313_PG_*_NAME) stays only
// when the new conninfo has the same host as the server it replaces (the GUI passes the file's
// server again as --pg-remote), else the name is unset and reports name the server by its host.
void replace_pg_server(bool remote, const std::string& conninfo);

// The host= value of a libpq keyword=value conninfo ("" when none).
std::string conninfo_host(const std::string& conninfo);

// The file's text for `rows`: the `//` comment lines `comments` (each line without its newline),
// then the jText header, the Fields block (name last) and one record per row. A row without an
// id gets its position.
std::string connections_text(const std::vector<PgConnection>& rows, const std::vector<std::string>& comments);

// Write `rows` to `file`, keeping the file's leading `//` comment lines (owner-only modes). A file
// read without the name column is written back with it.
bool save_connections(const std::filesystem::path& file, const std::vector<PgConnection>& rows, std::string& err);

// The password files beside the connections file: <dir>/jac313_pg_local.pgpass and
// <dir>/jac313_pg_remote.pgpass (libpq pgpass lines host:port:dbname:user:password, mode 600;
// bootstrap writes the local one). When one exists, its backend's conninfo gets
// passfile=<file>; pg-local also gets its default server when none is set.
void apply_password_files(const std::filesystem::path& secrets_dir);

// The git work tree that contains `p` ("" when none): the connections file must not be in one.
std::filesystem::path enclosing_git_project(const std::filesystem::path& p);

// A blank file to fill in (`connections --init`).
std::string connections_template();

// `jac313_test_cli connections [--file P] [--init] [--set-name ID NAME]`: show the file, its rows
// (name first) and its check line; --set-name names row ID and writes the file back.
int run_connections_command(int argc, char** argv, int first_arg);

} // namespace jac313::test_cli
