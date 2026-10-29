#pragma once

/**
 * jac313::Qlite::v002 — the C++26 SQLite wrapper. Public entry point:
 *
 *   #include <jac313/Qlite/v002.hpp>
 *   jac313::Qlite::v002::Sqlite db("app.db");
 *
 * Public symbols: Sqlite (Statement, Transaction, prepare/prepare_cached, get_top_one_*),
 * SqliteError, bind_value, column, version.
 */

#include "jac313/Qlite/v002/Sqlite.hpp"
