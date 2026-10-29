// The PostgreSQL store, over Qlite::v001::Postgres (libpq). In a build without libpq, CMake
// compiles postgres_none.cpp instead.
#include "db_stores.hpp"
#include "qlite_store.hpp"

#include <Qlite/v001/Postgres.hpp>

#include <format>
#include <optional>

namespace jtext_gui {
namespace {

using Qlite::v001::Postgres;

constexpr qlite_store::Dialect kPostgres{SessionKind::PostgreSQL, true, '"', "BEGIN", "postgres"};

/// One libpq connection setting, quoted ('' around it, \ and ' escaped).
std::string conn_value(std::string_view v) {
    std::string out = "'";
    for (const char c : v) {
        if (c == '\\' || c == '\'') out += '\\';
        out += c;
    }
    return out + "'";
}

class PostgresStore final : public IDbStore {
public:
    SessionKind kind() const override { return SessionKind::PostgreSQL; }

    bool connect(const ConnectionInfo& info) override {
        disconnect();
        // An empty password= blocks the pgpass file, so it is left out.
        std::string conninfo = std::format("host={} port={} dbname={} user={}", conn_value(info.host),
                                           conn_value(info.port.empty() ? "5432" : info.port),
                                           conn_value(info.database), conn_value(info.username));
        if (!info.password.empty()) conninfo += " password=" + conn_value(info.password);
        try {
            db_.emplace(conninfo);
        } catch (const Qlite::v001::Error& e) {
            last_error_ = e.what();
            return false;
        }
        last_error_.clear();
        return true;
    }

    void disconnect() override { db_.reset(); }
    bool is_connected() const override { return db_ && PQstatus(db_->handle()) == CONNECTION_OK; }
    std::string last_error() const override { return last_error_; }

    std::expected<void, std::string> ping() override {
        if (!is_connected())
            return std::unexpected(last_error_.empty() ? std::string("postgres: not connected") : last_error_);
        try {
            qlite_store::run(*db_, kPostgres, "SELECT 1");
        } catch (const Qlite::v001::Error& e) {
            return std::unexpected(e.what());
        }
        return {};
    }

    std::expected<DbQueryResult, std::string> query(std::string_view table, std::string_view where,
                                                    int limit) override {
        if (!is_connected()) return std::unexpected("postgres: not connected");
        auto table_sql = quote_table_ref(table, SessionKind::PostgreSQL);
        if (!table_sql) return std::unexpected(table_sql.error());
        auto where_sql = apply_ident_brackets(where, SessionKind::PostgreSQL);
        if (!where_sql) return std::unexpected(where_sql.error());
        auto comments = quote_table_ref(comments_table_of(table), SessionKind::PostgreSQL);
        if (!comments) return std::unexpected(comments.error());

        DbQueryResult out;
        try {
            std::string sql = "SELECT * FROM " + *table_sql;
            if (!where_sql->empty()) sql += " WHERE " + *where_sql;
            if (limit > 0) sql += std::format(" LIMIT {}", limit);
            debug_log("postgres SQL: " + sql);
            auto st = db_->prepare(sql);
            if (auto r = qlite_store::read_records(st, out); !r) return std::unexpected(r.error());
            adopt_columns(out, catalog(*table_sql), SessionKind::PostgreSQL);
            if (comments_exist(*comments)) qlite_store::read_comments(*db_, kPostgres, *comments, out);
            auto fields = quote_table_ref(fields_table_of(table), SessionKind::PostgreSQL);
            if (!fields) return std::unexpected(fields.error());
            qlite_store::read_declarations(*db_, kPostgres, comments_exist(*fields) ? *fields : std::string{}, out);
        } catch (const Qlite::v001::Error& e) {
            return std::unexpected(e.what());
        }
        return out;
    }

    std::expected<std::size_t, std::string> save_changes(TableSession& session) override {
        if (!is_connected()) return std::unexpected("postgres: not connected");
        return qlite_store::save(*db_, kPostgres, session, [this](const std::string& t) { return comments_exist(t); });
    }

private:
    /// The table's declared columns: name, type (format_type), Not Null, primary key.
    std::vector<ColumnDef> catalog(const std::string& table) {
        const std::string sql =
            "SELECT a.attname, pg_catalog.format_type(a.atttypid, a.atttypmod), a.attnotnull, "
            "EXISTS (SELECT 1 FROM pg_catalog.pg_index i WHERE i.indrelid = a.attrelid "
            "AND i.indisprimary AND a.attnum = ANY (i.indkey)) "
            "FROM pg_catalog.pg_attribute a "
            "WHERE a.attrelid = to_regclass($1) AND a.attnum > 0 AND NOT a.attisdropped ORDER BY a.attnum";
        debug_log("postgres columns: " + sql);
        auto st = db_->prepare(sql);
        st.bind(table);
        std::vector<ColumnDef> cols;
        while (st.step()) {
            const bool pk = st.get_at<std::string>(3) == "t";
            cols.push_back(column_from_declared(st.get_at<std::string>(0), st.get_at<std::string>(1),
                                                st.get_at<std::string>(2) == "t" || pk, pk, SessionKind::PostgreSQL));
        }
        return cols;
    }

    bool comments_exist(const std::string& table) {
        return Qlite::v001::get_top_one_long(*db_, "SELECT CASE WHEN to_regclass($1) IS NULL THEN 0 ELSE 1 END",
                                             table) == 1;
    }

    std::optional<Postgres> db_;
    std::string last_error_;
};

} // namespace

std::unique_ptr<IDbStore> make_postgres_store() { return std::make_unique<PostgresStore>(); }

} // namespace jtext_gui
