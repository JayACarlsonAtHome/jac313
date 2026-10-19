#pragma once

// connections_file.hpp — saved database servers for the connect dialog, kept in a jText file
// OUTSIDE every git project (the Captain, 2026-10-16: the same layout in every jac project).
// Default: <projects>/SQL_Secrets/jtext_gui_connections.jtext, i.e. a SQL_Secrets folder
// beside the jText_GUI checkout; $JTEXT_GUI_CONNECTIONS overrides. Never a password:
// PostgreSQL reads it from ~/.pgpass or $PGPASSFILE, the others are typed in the dialog.
//
// One section, one record per server:
//   1 id/Identity(1,1)  2 backend  3 host  4 port  5 dbname  6 user  7 connect_timeout  8 notes
// backend is <engine>-<where>: pg-local, pg-remote, mariadb-local, mariadb-remote,
// mssql-local, mssql-remote.

#include <string>
#include <vector>

namespace jtext_gui {

struct SavedConnection {
    std::string backend;          // e.g. "pg-remote"
    std::string host;
    std::string port;
    std::string dbname;
    std::string user;
    std::string connect_timeout;  // seconds ("" = driver default)
    std::string notes;
    /// Set when the row was read from a sibling catalog (jac313 / jac314). Not written.
    std::string origin;
};

// $JTEXT_GUI_CONNECTIONS, else <dir above the git checkout that holds `start`>/SQL_Secrets/
// jtext_gui_connections.jtext ("" when no checkout is found above `start`).
std::string default_connections_path(const std::string& start);

// The git work tree that contains `path` ("" when none): the file must not be in one.
std::string enclosing_git_project(const std::string& path);

// Read every record that has a backend and a host. False (and `err`) when the file cannot be
// read or has no such record.
bool load_connections(const std::string& path, std::vector<SavedConnection>& rows, std::string& err);

// The GUI file at `path`, plus jac313_connections.jtext and jac314_connections.jtext in the
// same folder. A GUI row wins when backend, host, port and dbname match. Sibling rows keep
// `origin` set to the file name. False only when no file yields a row.
bool load_saved_servers(const std::string& path, std::vector<SavedConnection>& rows, std::string& err);

// The jText text for `rows` (compact rows; NULL = the Unit Separator byte 0x1F).
std::string connections_text(const std::vector<SavedConnection>& rows);

// Write `rows`, creating the folder (0700) and the file (0600) when missing.
bool save_connections(const std::string& path, const std::vector<SavedConnection>& rows, std::string& err);

// Replace the row with the same backend, or append it.
void upsert_connection(std::vector<SavedConnection>& rows, const SavedConnection& c);

} // namespace jtext_gui
