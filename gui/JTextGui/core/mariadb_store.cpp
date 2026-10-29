// The MariaDB (and MySQL) store, over Qlite::v001::MariaDB (Connector/C). In a build without
// the client library, CMake compiles mariadb_none.cpp instead.
#include "db_stores.hpp"
#include "qlite_store.hpp"

#include <Qlite/v001/MariaDB.hpp>

#include <format>
#include <optional>

namespace jtext_gui {
namespace {

using Qlite::v001::MariaDB;

constexpr qlite_store::Dialect kMariaDB{SessionKind::MariaDB, false, '`', "START TRANSACTION", "mariadb"};

/// A result column's type as a declared type, for when the catalog has nothing.
ColumnDef from_mysql_field(const MYSQL_FIELD& field) {
    const bool pk = (field.flags & PRI_KEY_FLAG) != 0;
    const bool not_null = (field.flags & NOT_NULL_FLAG) != 0 || pk;
    std::string declared = "text";
    switch (field.type) {
    case MYSQL_TYPE_TINY:       declared = field.length == 1 ? "tinyint(1)" : "tinyint"; break;
    case MYSQL_TYPE_SHORT:      declared = "smallint"; break;
    case MYSQL_TYPE_INT24:
    case MYSQL_TYPE_LONG:       declared = "int"; break;
    case MYSQL_TYPE_LONGLONG:   declared = "bigint"; break;
    case MYSQL_TYPE_FLOAT:      declared = "float"; break;
    case MYSQL_TYPE_DOUBLE:     declared = "double"; break;
    case MYSQL_TYPE_DECIMAL:
    case MYSQL_TYPE_NEWDECIMAL: declared = "decimal"; break;
    case MYSQL_TYPE_DATE:
    case MYSQL_TYPE_NEWDATE:    declared = "date"; break;
    case MYSQL_TYPE_DATETIME:
    case MYSQL_TYPE_DATETIME2:  declared = "datetime"; break;
    case MYSQL_TYPE_TIMESTAMP:
    case MYSQL_TYPE_TIMESTAMP2: declared = "timestamp"; break;
    case MYSQL_TYPE_TIME:
    case MYSQL_TYPE_TIME2:      declared = "time"; break;
    case MYSQL_TYPE_YEAR:       declared = "year"; break;
    case MYSQL_TYPE_BIT:        declared = "boolean"; break;
    case MYSQL_TYPE_JSON:       declared = "json"; break;
    case MYSQL_TYPE_STRING:
    case MYSQL_TYPE_VAR_STRING:
    case MYSQL_TYPE_VARCHAR:    declared = "varchar"; break;
    default:                    declared = "text"; break;
    }
    return column_from_declared(field.name ? field.name : "", declared, not_null, pk, SessionKind::MariaDB);
}

class MariaDbStore final : public IDbStore {
public:
    SessionKind kind() const override { return SessionKind::MariaDB; }

    bool connect(const ConnectionInfo& info) override {
        disconnect();
        MariaDB::Options o;
        o.host = info.host;
        o.user = info.username;
        o.password = info.password;
        o.database = info.database;
        o.port = info.port.empty() ? 3306u : static_cast<unsigned>(std::stoul(info.port));
        // utf8mb4, and an UPDATE counts the rows it matched (CLIENT_FOUND_ROWS), so a saved
        // row that was already up to date is not taken for a missing one: both are Qlite's.
        try {
            db_.emplace(o);
        } catch (const Qlite::v001::Error& e) {
            last_error_ = e.what();
            return false;
        }
        last_error_.clear();
        return true;
    }

    void disconnect() override { db_.reset(); }
    bool is_connected() const override { return db_.has_value(); }
    std::string last_error() const override { return last_error_; }

    std::expected<void, std::string> ping() override {
        if (!db_) return std::unexpected(last_error_.empty() ? std::string("mariadb: not connected") : last_error_);
        try {
            qlite_store::run(*db_, kMariaDB, "SELECT 1");
        } catch (const Qlite::v001::Error& e) {
            return std::unexpected(e.what());
        }
        return {};
    }

    std::expected<DbQueryResult, std::string> query(std::string_view table, std::string_view where,
                                                    int limit) override {
        if (!db_) return std::unexpected("mariadb: not connected");
        auto table_sql = quote_table_ref(table, SessionKind::MariaDB);
        if (!table_sql) return std::unexpected(table_sql.error());
        auto where_sql = apply_ident_brackets(where, SessionKind::MariaDB);
        if (!where_sql) return std::unexpected(where_sql.error());
        auto comments = quote_table_ref(comments_table_of(table), SessionKind::MariaDB);
        if (!comments) return std::unexpected(comments.error());

        DbQueryResult out;
        try {
            std::string sql = "SELECT * FROM " + *table_sql;
            if (!where_sql->empty()) sql += " WHERE " + *where_sql;
            if (limit > 0) sql += std::format(" LIMIT {}", limit);
            debug_log("mariadb SQL: " + sql);
            auto st = db_->prepare(sql);

            // Where the result came from (its schema and table), and each column's type for
            // when the catalog has nothing: the statement's result metadata.
            std::string schema, org_table;
            std::vector<ColumnDef> fallback;
            if (MYSQL_RES* meta = mysql_stmt_result_metadata(st.handle())) {
                const unsigned n = mysql_num_fields(meta);
                const MYSQL_FIELD* f = mysql_fetch_fields(meta);
                if (n > 0) {
                    if (f[0].db) schema = f[0].db;
                    if (f[0].org_table && f[0].org_table[0]) org_table = f[0].org_table;
                    else if (f[0].table) org_table = f[0].table;
                }
                for (unsigned c = 0; c < n; ++c) fallback.push_back(from_mysql_field(f[c]));
                mysql_free_result(meta);
            }
            if (auto r = qlite_store::read_records(st, out); !r) return std::unexpected(r.error());
            auto declared = catalog(schema, org_table);
            adopt_columns(out, declared.empty() ? std::move(fallback) : std::move(declared), SessionKind::MariaDB);
            if (comments_exist(*comments)) qlite_store::read_comments(*db_, kMariaDB, *comments, out);
        } catch (const Qlite::v001::Error& e) {
            return std::unexpected(e.what());
        }
        return out;
    }

    std::expected<std::size_t, std::string> save_changes(TableSession& session) override {
        if (!db_) return std::unexpected("mariadb: not connected");
        return qlite_store::save(*db_, kMariaDB, session, [this](const std::string& t) { return comments_exist(t); });
    }

private:
    /// The table's declared columns (COLUMN_TYPE keeps lengths and UNSIGNED); `schema`
    /// empty: the connection's database.
    std::vector<ColumnDef> catalog(const std::string& schema, const std::string& table) {
        if (table.empty()) return {};
        const std::string sql =
            "SELECT COLUMN_NAME, COLUMN_TYPE, IS_NULLABLE, COLUMN_KEY FROM information_schema.COLUMNS "
            "WHERE TABLE_SCHEMA = COALESCE(NULLIF(?, ''), DATABASE()) AND TABLE_NAME = ? ORDER BY ORDINAL_POSITION";
        debug_log("mariadb columns: " + sql);
        auto st = db_->prepare(sql);
        st.bind(schema, table);
        std::vector<ColumnDef> cols;
        while (st.step()) {
            const bool nullable = st.get_at<std::string>(2) == "YES";
            const bool pk = st.get_at<std::string>(3) == "PRI";
            cols.push_back(column_from_declared(st.get_at<std::string>(0), st.get_at<std::string>(1),
                                                !nullable || pk, pk, SessionKind::MariaDB));
        }
        return cols;
    }

    // ER_NO_SUCH_TABLE (1146): the table is missing; any other error is an error.
    bool comments_exist(const std::string& table) {
        try {
            qlite_store::run(*db_, kMariaDB, "SELECT 1 FROM " + table + " LIMIT 0");
            return true;
        } catch (const Qlite::v001::MariaDBError& e) {
            if (e.code() == 1146) return false;
            throw;
        }
    }

    std::optional<MariaDB> db_;
    std::string last_error_;
};

} // namespace

std::unique_ptr<IDbStore> make_mariadb_store() { return std::make_unique<MariaDbStore>(); }

} // namespace jtext_gui
