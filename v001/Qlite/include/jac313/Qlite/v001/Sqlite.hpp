#pragma once

/**
 * jacQLite - A modern C++ SQLite wrapper emphasizing clean variadic
 * "peel first, recurse on ellipses" patterns for binding and querying.
 *
 * This is the standalone evolution of the SQLite integration prototype
 * originally developed within ts_store for direct SQL persistence and
 * jText slurping use cases.
 *
 * Key features:
 * - Header-only friendly
 * - Variadic bind/get using recursive pack peeling
 * - Type-safe column extractors and binders via specializations
 * - RAII for connections and statements
 * - Transaction helpers
 */

#include <sqlite3.h>

#include <string>
#include <string_view>
#include <stdexcept>
#include <utility>
#include <type_traits>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <tuple>
#include <vector>
#include <span>
#include <unordered_map>
#if __has_include(<expected>)
#  include <expected>
#endif
#include <iostream>

// Migrated in-tree from the legacy jacQlite repo (was namespace jac::qlite).
// This is now the canonical home of the SQLite wrapper for the jac313 family.
//
// The implementation lives in Sqlite.body.hpp (an include-free fragment). This
// textual header makes the std + <sqlite3.h> names visible via the textual
// includes above, then pastes the shared body inside the namespace. The
// import-std module (jac313.qlite.v001.cppm) includes the SAME body, differing
// only in how std/sqlite3 are made visible. Keep declarations in the body file.
namespace jac313::Qlite::v001 {

#include "jac313/Qlite/v001/Sqlite.body.hpp"

} // namespace jac313::Qlite::v001
