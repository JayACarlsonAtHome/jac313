// MariaDB in a build without its client library (CMake chose this file over mariadb_store.cpp).
#include "db_stores.hpp"

namespace jtext_gui {

std::unique_ptr<IDbStore> make_mariadb_store() {
    return std::make_unique<UnavailableStore>(
        SessionKind::MariaDB, "mariadb support not built (install MariaDB client dev and reconfigure)");
}

} // namespace jtext_gui
