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
 * Public symbols: Sqlite (Statement, Transaction, prepare/prepare_cached, get_one_*),
 * SqliteError, bind_value, column, version.
 */

#include "jac313/Qlite/v001/Sqlite.hpp"
