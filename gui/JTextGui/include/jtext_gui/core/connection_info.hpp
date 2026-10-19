#pragma once

#include "jtext_gui/core/session.hpp"

#include <string>

namespace jtext_gui {

/// Connection parameters for any DB backend (no wx).
struct ConnectionInfo {
    SessionKind kind = SessionKind::Sqlite;  ///< Sqlite | MariaDB | PostgreSQL
    std::string host = "localhost";
    std::string port;          ///< empty → default (5432 / 3306)
    std::string username;
    std::string password;      ///< in-memory only
    std::string database;
    std::string file_path;     ///< SQLite file
    std::string extra;         ///< socket, sslmode, etc.
};

} // namespace jtext_gui
