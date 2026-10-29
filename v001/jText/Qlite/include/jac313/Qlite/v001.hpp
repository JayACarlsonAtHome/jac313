#pragma once

/**
 * jac313::Qlite::v001 — the C++23 SQLite wrapper. Public entry point:
 *
 *   #include <jac313/Qlite/v001.hpp>
 *   jac313::Qlite::v001::Sqlite db("app.db");
 *
 * Public symbols: Sqlite (Statement, Transaction, prepare/prepare_cached, get_top_one_*),
 * SqliteError, bind_value, column, version.
 */

#include "jac313/Qlite/v001/Sqlite.hpp"
