#pragma once

/**
 * jac313::Qlite::v001 — the jac313 SQLite wrapper.
 *
 * The implementation now lives in-tree (migrated from the legacy jacQlite repo /
 * jac::qlite). This is the public entry point:
 *
 *   #include <jac313/Qlite/v001.hpp>
 *   jac313::Qlite::v001::Sqlite db("app.db");
 *
 * Public symbols: Sqlite, SqliteError, bind_value, column.
 */

#include "jac313/Qlite/v001/Sqlite.hpp"
