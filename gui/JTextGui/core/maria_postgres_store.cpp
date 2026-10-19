#include "jtext_gui/core/table_store.hpp"

#include <memory>
#include <string>

// MariaDB and PostgreSQL stores: same IDbStore contract as SQLite.
// Linked when libpq / MariaDB client is available; otherwise stub kinds return errors.

#if defined(JTEXT_HAS_POSTGRESQL)
#include <libpq-fe.h>
#endif

#if defined(JTEXT_HAS_MARIADB)
#include <mysql.h>
#endif

#include <cstdio>
#include <format>

namespace jtext_gui {
namespace {

#if defined(JTEXT_HAS_POSTGRESQL)

std::vector<ColumnDef> load_pg_columns(PGconn* conn, Oid relid) {
    if (!conn || relid == 0) return {};
    const std::string sql = std::format(
        "SELECT a.attname, pg_catalog.format_type(a.atttypid, a.atttypmod), "
        "CASE WHEN a.attnotnull THEN 1 ELSE 0 END, "
        "CASE WHEN EXISTS ("
        "SELECT 1 FROM pg_catalog.pg_index i "
        "WHERE i.indrelid = a.attrelid AND i.indisprimary "
        "AND a.attnum = ANY (i.indkey)) THEN 1 ELSE 0 END "
        "FROM pg_catalog.pg_attribute a "
        "WHERE a.attrelid = {} AND a.attnum > 0 AND NOT a.attisdropped "
        "ORDER BY a.attnum",
        static_cast<unsigned long>(relid));
    debug_log("postgres columns: " + sql);
    PGresult* res = PQexec(conn, sql.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        debug_log(std::string("postgres columns error: ") + PQerrorMessage(conn));
        PQclear(res);
        return {};
    }
    std::vector<ColumnDef> cols;
    const int n = PQntuples(res);
    for (int i = 0; i < n; ++i) {
        if (PQgetisnull(res, i, 0)) continue;
        const char* name = PQgetvalue(res, i, 0);
        const char* typ = PQgetisnull(res, i, 1) ? "" : PQgetvalue(res, i, 1);
        const bool notnull = !PQgetisnull(res, i, 2) && PQgetvalue(res, i, 2)[0] == '1';
        const bool pk = !PQgetisnull(res, i, 3) && PQgetvalue(res, i, 3)[0] == '1';
        cols.push_back(column_from_declared(name ? name : "", typ ? typ : "", notnull || pk, pk,
                                             SessionKind::PostgreSQL));
    }
    PQclear(res);
    return cols;
}

class PostgresStore final : public IDbStore {
public:
    ~PostgresStore() override { disconnect(); }
    SessionKind kind() const override { return SessionKind::PostgreSQL; }

    bool connect(const ConnectionInfo& info) override {
        disconnect();
        const std::string port = info.port.empty() ? "5432" : info.port;
        // An empty password= in the string blocks the pgpass file. Omit it.
        std::string conninfo = std::format(
            "host={} port={} dbname={} user={}",
            info.host, port, info.database, info.username);
        if (!info.password.empty())
            conninfo += std::format(" password={}", info.password);
        conn_ = PQconnectdb(conninfo.c_str());
        if (PQstatus(conn_) != CONNECTION_OK) {
            last_error_ = conn_ ? PQerrorMessage(conn_) : "postgres: connect failed";
            if (conn_) PQfinish(conn_);
            conn_ = nullptr;
            return false;
        }
        last_error_.clear();
        return true;
    }

    void disconnect() override {
        if (conn_) {
            PQfinish(conn_);
            conn_ = nullptr;
        }
    }

    bool is_connected() const override {
        return conn_ && PQstatus(conn_) == CONNECTION_OK;
    }
    std::string last_error() const override { return last_error_; }

    std::expected<void, std::string> ping() override {
        if (!is_connected())
            return std::unexpected(last_error_.empty() ? std::string("postgres: not connected") : last_error_);
        debug_log("postgres SQL: SELECT 1");
        PGresult* res = PQexec(conn_, "SELECT 1");
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::string err = PQerrorMessage(conn_);
            PQclear(res);
            return std::unexpected(err);
        }
        PQclear(res);
        return {};
    }

    std::expected<DbQueryResult, std::string> query(std::string_view table,
                                                    std::string_view where,
                                                    int limit) override {
        if (!is_connected()) {
            debug_log("postgres query: not connected");
            return std::unexpected("postgres: not connected");
        }
        auto table_sql = quote_table_ref(table, SessionKind::PostgreSQL);
        if (!table_sql) return std::unexpected(table_sql.error());
        auto where_sql = apply_ident_brackets(where, SessionKind::PostgreSQL);
        if (!where_sql) return std::unexpected(where_sql.error());

        std::string sql = "SELECT * FROM " + *table_sql;
        if (!where_sql->empty()) {
            sql += " WHERE ";
            sql += *where_sql;
        }
        if (limit > 0) sql += std::format(" LIMIT {}", limit);
        debug_log("postgres SQL: " + sql);

        PGresult* res = PQexec(conn_, sql.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::string err = PQerrorMessage(conn_);
            PQclear(res);
            debug_log("postgres error: " + err);
            return std::unexpected(err + "\nSQL: " + sql);
        }

        DbQueryResult out;
        const int ncols = PQnfields(res);
        Oid relid = 0;
        for (int c = 0; c < ncols; ++c) {
            out.column_names.emplace_back(PQfname(res, c));
            if (relid == 0) relid = PQftable(res, c);
        }
        if (!out.column_names.empty()) out.key_column = out.column_names.front();

        const int nrows = PQntuples(res);
        for (int r = 0; r < nrows; ++r) {
            Record rec;
            rec.marker = kDefaultMarker;
            rec.is_null.fill(true);
            for (int c = 0; c < static_cast<int>(kMaxFields) && c < ncols; ++c) {
                const auto col = static_cast<std::size_t>(c);
                if (PQgetisnull(res, r, c)) continue;
                const char* v = PQgetvalue(res, r, c);
                rec.is_null[col] = false;
                rec.has_line[col] = true;
                rec.fields[col] = v ? v : "";
            }
            rec.original_key = rec.is_null[0] ? std::string{} : rec.fields[0];
            out.rows.push_back(std::move(rec));
        }
        PQclear(res);
        adopt_columns(out, load_pg_columns(conn_, relid), SessionKind::PostgreSQL);
        return out;
    }

    std::expected<std::size_t, std::string> save_changes(TableSession& session) override {
        // v1: full mergesert of dirty rows via simple SQL (parameterized values)
        if (!is_connected()) return std::unexpected("postgres: not connected");
        auto table_sql = quote_table_ref(session.table, SessionKind::PostgreSQL);
        if (!table_sql) return std::unexpected(table_sql.error());
        if (session.column_names.empty())
            return std::unexpected("postgres: no columns");

        // Transaction
        PGresult* b = PQexec(conn_, "BEGIN");
        if (PQresultStatus(b) != PGRES_COMMAND_OK) {
            std::string e = PQerrorMessage(conn_);
            PQclear(b);
            return std::unexpected(e);
        }
        PQclear(b);

        std::size_t written = 0;
        std::string err;
        const auto& cols = session.column_names;
        const std::string key = session.key_column.empty() ? cols[0] : session.key_column;

        for (std::size_t idx : session.changes.dirty_rows) {
            if (idx >= session.rows.size()) continue;
            const Record& rec = session.rows[idx];
            const bool is_insert =
                session.changes.inserted.count(idx) || rec.original_key.empty();

            if (!is_insert) {
                std::string sql = "UPDATE " + *table_sql + " SET ";
                for (std::size_t c = 1; c < cols.size() && c < kMaxFields; ++c) {
                    if (c > 1) sql += ", ";
                    // literal escape for v1 — improve with PQexecParams later
                    sql += cols[c];
                    sql += " = ";
                    if (rec.is_null[c]) {
                        sql += "NULL";
                    } else {
                        sql += "'";
                        for (char ch : rec.fields[c]) {
                            if (ch == '\'') sql += "''";
                            else sql += ch;
                        }
                        sql += "'";
                    }
                }
                sql += std::format(" WHERE {} = '", key);
                for (char ch : rec.original_key) {
                    if (ch == '\'') sql += "''";
                    else sql += ch;
                }
                sql += "'";
                PGresult* r = PQexec(conn_, sql.c_str());
                if (PQresultStatus(r) != PGRES_COMMAND_OK) {
                    err = PQerrorMessage(conn_);
                    PQclear(r);
                    break;
                }
                const char* n = PQcmdTuples(r);
                PQclear(r);
                if (n && n[0] != '0') {
                    ++written;
                    continue;
                }
            }

            std::string sql = "INSERT INTO " + *table_sql + " (";
            for (std::size_t c = 0; c < cols.size() && c < kMaxFields; ++c) {
                if (c) sql += ",";
                sql += cols[c];
            }
            sql += ") VALUES (";
            for (std::size_t c = 0; c < cols.size() && c < kMaxFields; ++c) {
                if (c) sql += ",";
                if (rec.is_null[c]) {
                    sql += "NULL";
                } else {
                    sql += "'";
                    for (char ch : rec.fields[c]) {
                        if (ch == '\'') sql += "''";
                        else sql += ch;
                    }
                    sql += "'";
                }
            }
            sql += ")";
            PGresult* r = PQexec(conn_, sql.c_str());
            if (PQresultStatus(r) != PGRES_COMMAND_OK) {
                err = PQerrorMessage(conn_);
                PQclear(r);
                break;
            }
            PQclear(r);
            ++written;
        }

        if (!err.empty()) {
            PGresult* rb = PQexec(conn_, "ROLLBACK");
            PQclear(rb);
            return std::unexpected(err);
        }
        PGresult* c = PQexec(conn_, "COMMIT");
        if (PQresultStatus(c) != PGRES_COMMAND_OK) {
            std::string e = PQerrorMessage(conn_);
            PQclear(c);
            PGresult* rb = PQexec(conn_, "ROLLBACK");
            PQclear(rb);
            return std::unexpected(e);
        }
        PQclear(c);

        for (std::size_t idx : session.changes.dirty_rows)
            if (idx < session.rows.size())
                session.rows[idx].original_key = session.rows[idx].is_null[0]
                                                     ? std::string{}
                                                     : session.rows[idx].fields[0];
        session.changes.clear();
        return written;
    }

private:
    PGconn* conn_ = nullptr;
    std::string last_error_;
};
#endif

#if defined(JTEXT_HAS_MARIADB)

std::string maria_literal(std::string_view text) {
    std::string out = "'";
    for (unsigned char c : text) {
        if (c < 0x20 || c == 0x7f) return {};
        if (c == '\'') out += "''";
        else if (c == '\\') out += "\\\\";
        else out.push_back(static_cast<char>(c));
    }
    out.push_back('\'');
    return out;
}

ColumnDef from_mysql_field(const MYSQL_FIELD& field) {
    const bool pk = (field.flags & PRI_KEY_FLAG) != 0;
    const bool not_null = (field.flags & NOT_NULL_FLAG) != 0 || pk;
    const char* name = field.name ? field.name : "";
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
    case MYSQL_TYPE_DATETIME:   declared = "datetime"; break;
    case MYSQL_TYPE_TIMESTAMP:  declared = "timestamp"; break;
    case MYSQL_TYPE_TIME:       declared = "time"; break;
#ifdef MYSQL_TYPE_DATETIME2
    case MYSQL_TYPE_DATETIME2:  declared = "datetime"; break;
#endif
#ifdef MYSQL_TYPE_TIMESTAMP2
    case MYSQL_TYPE_TIMESTAMP2: declared = "timestamp"; break;
#endif
#ifdef MYSQL_TYPE_TIME2
    case MYSQL_TYPE_TIME2:      declared = "time"; break;
#endif
    case MYSQL_TYPE_YEAR:       declared = "year"; break;
    case MYSQL_TYPE_BIT:        declared = "boolean"; break;
#ifdef MYSQL_TYPE_JSON
    case MYSQL_TYPE_JSON:       declared = "json"; break;
#endif
    case MYSQL_TYPE_BLOB:
    case MYSQL_TYPE_TINY_BLOB:
    case MYSQL_TYPE_MEDIUM_BLOB:
    case MYSQL_TYPE_LONG_BLOB:  declared = "text"; break;
    case MYSQL_TYPE_STRING:
    case MYSQL_TYPE_VAR_STRING:
    case MYSQL_TYPE_VARCHAR:    declared = "varchar"; break;
    default:                    declared = "text"; break;
    }
    return column_from_declared(name, declared, not_null, pk, SessionKind::MariaDB);
}

std::vector<ColumnDef> load_maria_columns(MYSQL* mysql, std::string_view schema, std::string_view table) {
    if (!mysql || table.empty()) return {};
    const std::string table_lit = maria_literal(table);
    if (table_lit.empty()) return {};
    std::string schema_sql = "TABLE_SCHEMA = DATABASE()";
    if (!schema.empty()) {
        const std::string schema_lit = maria_literal(schema);
        if (schema_lit.empty()) return {};
        schema_sql = "TABLE_SCHEMA = " + schema_lit;
    }
    const std::string sql = std::format(
        "SELECT COLUMN_NAME, COLUMN_TYPE, IS_NULLABLE, COLUMN_KEY "
        "FROM information_schema.COLUMNS WHERE {} AND TABLE_NAME = {} "
        "ORDER BY ORDINAL_POSITION",
        schema_sql, table_lit);
    debug_log("mariadb columns: " + sql);
    if (mysql_query(mysql, sql.c_str()) != 0) {
        debug_log(std::string("mariadb columns error: ") + mysql_error(mysql));
        return {};
    }
    MYSQL_RES* res = mysql_store_result(mysql);
    if (!res) return {};
    std::vector<ColumnDef> cols;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr) {
        if (!row[0]) continue;
        const char* typ = row[1] ? row[1] : "";
        const bool nullable = row[2] && (row[2][0] == 'Y' || row[2][0] == 'y');
        const bool pk = row[3] && std::string_view(row[3]) == "PRI";
        cols.push_back(column_from_declared(row[0], typ, !nullable || pk, pk, SessionKind::MariaDB));
    }
    mysql_free_result(res);
    return cols;
}

class MariaDbStore final : public IDbStore {
public:
    ~MariaDbStore() override { disconnect(); }
    SessionKind kind() const override { return SessionKind::MariaDB; }

    bool connect(const ConnectionInfo& info) override {
        disconnect();
        mysql_ = mysql_init(nullptr);
        if (!mysql_) return false;
        const unsigned port = info.port.empty() ? 3306u : static_cast<unsigned>(std::stoul(info.port));
        if (!mysql_real_connect(mysql_, info.host.c_str(), info.username.c_str(),
                                info.password.c_str(), info.database.c_str(), port, nullptr, 0)) {
            last_error_ = mysql_ ? mysql_error(mysql_) : "mariadb: connect failed";
            if (mysql_) mysql_close(mysql_);
            mysql_ = nullptr;
            return false;
        }
        last_error_.clear();
        return true;
    }

    void disconnect() override {
        if (mysql_) {
            mysql_close(mysql_);
            mysql_ = nullptr;
        }
    }

    bool is_connected() const override { return mysql_ != nullptr; }
    std::string last_error() const override { return last_error_; }

    std::expected<void, std::string> ping() override {
        if (!mysql_)
            return std::unexpected(last_error_.empty() ? std::string("mariadb: not connected") : last_error_);
        debug_log("mariadb SQL: SELECT 1");
        if (mysql_query(mysql_, "SELECT 1") != 0)
            return std::unexpected(mysql_error(mysql_));
        MYSQL_RES* res = mysql_store_result(mysql_);
        if (res) mysql_free_result(res);
        return {};
    }

    std::expected<DbQueryResult, std::string> query(std::string_view table,
                                                    std::string_view where,
                                                    int limit) override {
        if (!mysql_) return std::unexpected("mariadb: not connected");
        auto table_sql = quote_table_ref(table, SessionKind::MariaDB);
        if (!table_sql) return std::unexpected(table_sql.error());
        auto where_sql = apply_ident_brackets(where, SessionKind::MariaDB);
        if (!where_sql) return std::unexpected(where_sql.error());

        std::string sql = "SELECT * FROM " + *table_sql;
        if (!where_sql->empty()) {
            sql += " WHERE ";
            sql += *where_sql;
        }
        if (limit > 0) sql += std::format(" LIMIT {}", limit);
        debug_log("mariadb SQL: " + sql);

        if (mysql_query(mysql_, sql.c_str()) != 0)
            return std::unexpected(std::string(mysql_error(mysql_)) + "\nSQL: " + sql);

        MYSQL_RES* res = mysql_store_result(mysql_);
        if (!res) return std::unexpected(mysql_error(mysql_));

        DbQueryResult out;
        const unsigned ncols = mysql_num_fields(res);
        MYSQL_FIELD* fields = mysql_fetch_fields(res);
        std::string schema;
        std::string org_table;
        std::vector<ColumnDef> fallback;
        if (fields && ncols > 0) {
            if (fields[0].db) schema = fields[0].db;
            if (fields[0].org_table && fields[0].org_table[0]) org_table = fields[0].org_table;
            else if (fields[0].table) org_table = fields[0].table;
            for (unsigned c = 0; c < ncols; ++c) {
                out.column_names.emplace_back(fields[c].name ? fields[c].name : "");
                fallback.push_back(from_mysql_field(fields[c]));
            }
        }
        if (!out.column_names.empty()) out.key_column = out.column_names.front();

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr) {
            Record rec;
            rec.marker = kDefaultMarker;
            rec.is_null.fill(true);
            for (unsigned c = 0; c < kMaxFields && c < ncols; ++c) {
                if (!row[c]) continue;
                rec.is_null[c] = false;
                rec.has_line[c] = true;
                rec.fields[c] = row[c];
            }
            rec.original_key = rec.is_null[0] ? std::string{} : rec.fields[0];
            out.rows.push_back(std::move(rec));
        }
        mysql_free_result(res);
        auto catalog = load_maria_columns(mysql_, schema, org_table);
        adopt_columns(out, catalog.empty() ? std::move(fallback) : std::move(catalog),
                      SessionKind::MariaDB);
        return out;
    }

    std::expected<std::size_t, std::string> save_changes(TableSession& session) override {
        // Symmetric to Postgres v1 path — implement fully when MariaDB is in regular use
        if (!mysql_) return std::unexpected("mariadb: not connected");
        return std::unexpected("mariadb: save_changes not fully implemented yet (reader works)");
    }

private:
    MYSQL* mysql_ = nullptr;
    std::string last_error_;
};
#endif

class UnavailableStore final : public IDbStore {
public:
    explicit UnavailableStore(SessionKind k, std::string why)
        : kind_(k), why_(std::move(why)) {}
    SessionKind kind() const override { return kind_; }
    bool connect(const ConnectionInfo&) override { return false; }
    void disconnect() override {}
    bool is_connected() const override { return false; }
    std::string last_error() const override { return why_; }
    std::expected<void, std::string> ping() override { return std::unexpected(why_); }
    std::expected<DbQueryResult, std::string> query(std::string_view, std::string_view,
                                                    int) override {
        return std::unexpected(why_);
    }
    std::expected<std::size_t, std::string> save_changes(TableSession&) override {
        return std::unexpected(why_);
    }

private:
    SessionKind kind_;
    std::string why_;
};

} // namespace

// sqlite_store.cpp
std::unique_ptr<IDbStore> make_sqlite_store();

std::unique_ptr<IDbStore> make_db_store(SessionKind kind) {
    switch (kind) {
    case SessionKind::Sqlite:
        return make_sqlite_store();
    case SessionKind::PostgreSQL:
#if defined(JTEXT_HAS_POSTGRESQL)
        return std::make_unique<PostgresStore>();
#else
        return std::make_unique<UnavailableStore>(
            SessionKind::PostgreSQL,
            "postgresql support not built (install libpq and reconfigure)");
#endif
    case SessionKind::MariaDB:
#if defined(JTEXT_HAS_MARIADB)
        return std::make_unique<MariaDbStore>();
#else
        return std::make_unique<UnavailableStore>(
            SessionKind::MariaDB,
            "mariadb support not built (install MariaDB client dev and reconfigure)");
#endif
    default:
        return nullptr;
    }
}

} // namespace jtext_gui
