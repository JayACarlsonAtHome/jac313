#include "db_stores.hpp"

namespace jtext_gui {

std::unique_ptr<IDbStore> make_db_store(SessionKind kind) {
    switch (kind) {
    case SessionKind::Sqlite:     return make_sqlite_store();
    case SessionKind::PostgreSQL: return make_postgres_store();
    case SessionKind::MariaDB:    return make_mariadb_store();
    default:                      return nullptr;
    }
}

} // namespace jtext_gui
