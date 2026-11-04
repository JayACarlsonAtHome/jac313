// PostgreSQL in a build without libpq (CMake chose this file over postgres_store.cpp).
#include "db_stores.hpp"

namespace jtext_gui {

std::unique_ptr<IDbStore> make_postgres_store() {
    return std::make_unique<UnavailableStore>(SessionKind::PostgreSQL,
                                              "postgresql support not built (install libpq and reconfigure)");
}

} // namespace jtext_gui
