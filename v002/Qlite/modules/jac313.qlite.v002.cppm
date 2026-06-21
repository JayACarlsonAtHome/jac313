module;

// jac313.qlite.v002 — module interface over the shared implementation body
// (Sqlite.body.hpp). Both build configurations export the SAME public API under
// `jac313::Qlite::v002` (matching the textual header and the jText/store packages);
// the ONLY difference is how the std library is made visible to the body:
//
//   JAC313_QLITE_IMPORT_STD  ON  -> `import std;`            (g++-15 pilot)
//                            OFF -> textual std #includes    (clang / fallback)
//
// <sqlite3.h> (a C library) is always textual in the global module fragment.
// The body is defined in the module purview and exported via `export namespace`,
// which gives the entities module linkage while still exposing them, qualified,
// to importers — `export using` would instead inject the names into the global
// namespace, diverging from the header's `jac313::Qlite::v002::` spelling.

#include <sqlite3.h>

#ifndef JAC313_QLITE_IMPORT_STD
#  include <string>
#  include <stdexcept>
#  include <utility>
#  include <type_traits>
#  include <cstdint>
#  include <optional>
#  include <tuple>
#endif

export module jac313.qlite.v002;

#ifdef JAC313_QLITE_IMPORT_STD
import std;
#endif

export namespace jac313::Qlite::v002 {
#include "jac313/Qlite/v002/Sqlite.body.hpp"
}
