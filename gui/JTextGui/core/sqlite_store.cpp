#include "jtext_gui/core/table_store.hpp"

#include <sqlite3.h>

#include <format>
#include <memory>

namespace jtext_gui {
namespace {

class SqliteStore final : public IDbStore {
public:
    ~SqliteStore() override { disconnect(); }

    SessionKind kind() const override { return SessionKind::Sqlite; }

    bool connect(const ConnectionInfo& info) override {
        disconnect();
        if (info.file_path.empty()) {
            last_error_ = "sqlite: no file";
            return false;
        }
        if (sqlite3_open(info.file_path.c_str(), &db_) != SQLITE_OK) {
            last_error_ = db_ ? sqlite3_errmsg(db_) : "sqlite: open failed";
            if (db_) {
                sqlite3_close(db_);
                db_ = nullptr;
            }
            return false;
        }
        last_error_.clear();
        path_ = info.file_path;
        return true;
    }

    void disconnect() override {
        if (db_) {
            sqlite3_close(db_);
            db_ = nullptr;
        }
        path_.clear();
    }

    bool is_connected() const override { return db_ != nullptr; }
    std::string last_error() const override { return last_error_; }

    std::expected<void, std::string> ping() override {
        if (!db_)
            return std::unexpected(last_error_.empty() ? std::string("sqlite: not connected") : last_error_);
        debug_log("sqlite SQL: SELECT 1");
        char* err = nullptr;
        if (sqlite3_exec(db_, "SELECT 1", nullptr, nullptr, &err) != SQLITE_OK) {
            std::string msg = err ? err : sqlite3_errmsg(db_);
            sqlite3_free(err);
            return std::unexpected(msg);
        }
        return {};
    }

    std::expected<DbQueryResult, std::string> query(std::string_view table,
                                                    std::string_view where,
                                                    int limit) override {
        if (!db_) return std::unexpected("sqlite: not connected");
        auto table_sql = quote_table_ref(table, SessionKind::Sqlite);
        if (!table_sql) return std::unexpected(table_sql.error());
        auto where_sql = apply_ident_brackets(where, SessionKind::Sqlite);
        if (!where_sql) return std::unexpected(where_sql.error());

        DbQueryResult out;
        // PRAGMA table_info
        {
            std::string pragma = std::format("PRAGMA table_info({});", *table_sql);
            sqlite3_stmt* st = nullptr;
            if (sqlite3_prepare_v2(db_, pragma.c_str(), -1, &st, nullptr) != SQLITE_OK)
                return std::unexpected(sqlite3_errmsg(db_));
            while (sqlite3_step(st) == SQLITE_ROW) {
                const char* name = reinterpret_cast<const char*>(sqlite3_column_text(st, 1));
                const char* type = reinterpret_cast<const char*>(sqlite3_column_text(st, 2));
                const bool notnull = sqlite3_column_int(st, 3) != 0;
                const bool pk = sqlite3_column_int(st, 5) != 0;
                if (!name) continue;
                out.columns.push_back(column_from_declared(name, type ? type : "", notnull || pk, pk,
                                                           SessionKind::Sqlite));
                out.column_names.emplace_back(name);
            }
            sqlite3_finalize(st);
        }
        if (out.column_names.empty())
            return std::unexpected(std::string("sqlite: no columns for table ") + std::string(table));
        out.key_column = out.column_names.front();

        std::string sql = "SELECT * FROM " + *table_sql;
        if (!where_sql->empty()) {
            // v1: append WHERE as-is (documented risk); empty preferred for tests
            sql += " WHERE ";
            sql += *where_sql;
        }
        if (limit > 0) sql += std::format(" LIMIT {}", limit);
        debug_log("sqlite SQL: " + sql);

        sqlite3_stmt* st = nullptr;
        if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &st, nullptr) != SQLITE_OK)
            return std::unexpected(std::string(sqlite3_errmsg(db_)) + "\nSQL: " + sql);

        const int ncols = sqlite3_column_count(st);
        while (sqlite3_step(st) == SQLITE_ROW) {
            Record rec;
            rec.marker = kDefaultMarker;
            rec.ensure_size(static_cast<std::size_t>(ncols));
            rec.mark_all_absent();
            for (int i = 0; i < static_cast<int>(kMaxFields) && i < ncols; ++i) {
                const auto col = static_cast<std::size_t>(i);
                if (sqlite3_column_type(st, i) == SQLITE_NULL) continue;
                const unsigned char* t = sqlite3_column_text(st, i);
                rec.is_null[col] = false;
                rec.has_line[col] = true;
                rec.fields[col] = t ? reinterpret_cast<const char*>(t) : "";
            }
            rec.original_key = rec.is_null[0] ? std::string{} : rec.fields[0];
            out.rows.push_back(std::move(rec));
        }
        sqlite3_finalize(st);
        return out;
    }

    std::expected<std::size_t, std::string> save_changes(TableSession& session) override {
        if (!db_) return std::unexpected("sqlite: not connected");
        if (session.table.empty() || !is_safe_identifier(session.table))
            return std::unexpected("sqlite: bad table name");
        if (session.column_names.empty())
            return std::unexpected("sqlite: no column metadata (re-open table)");

        const auto& cols = session.column_names;
        const std::string& key_col = session.key_column.empty() ? cols[0] : session.key_column;
        if (!is_safe_identifier(key_col))
            return std::unexpected("sqlite: bad key column");

        if (sqlite3_exec(db_, "BEGIN IMMEDIATE;", nullptr, nullptr, nullptr) != SQLITE_OK)
            return std::unexpected(sqlite3_errmsg(db_));

        std::size_t written = 0;
        std::string err;

        auto rollback = [&] {
            sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        };

        for (std::size_t idx : session.changes.dirty_rows) {
            if (idx >= session.rows.size()) continue;
            const Record& rec = session.rows[idx];
            const bool is_insert =
                session.changes.inserted.count(idx) > 0 || rec.original_key.empty();

            if (!is_insert) {
                // UPDATE non-key columns WHERE key = original_key
                std::string sql = std::format("UPDATE {} SET ", session.table);
                bool first = true;
                for (std::size_t c = 1; c < cols.size() && c < kMaxFields; ++c) {
                    if (!is_safe_identifier(cols[c])) {
                        err = "unsafe column: " + cols[c];
                        break;
                    }
                    if (!first) sql += ", ";
                    sql += cols[c];
                    sql += "=?";
                    first = false;
                }
                if (!err.empty()) break;
                sql += std::format(" WHERE {}=?", key_col);

                sqlite3_stmt* st = nullptr;
                if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &st, nullptr) != SQLITE_OK) {
                    err = sqlite3_errmsg(db_);
                    break;
                }
                int p = 1;
                for (std::size_t c = 1; c < cols.size() && c < kMaxFields; ++c) {
                    if (field_is_null(rec, c)) sqlite3_bind_null(st, p++);
                    else sqlite3_bind_text(st, p++, field_text(rec, c).c_str(), -1, SQLITE_TRANSIENT);
                }
                sqlite3_bind_text(st, p, rec.original_key.c_str(), -1, SQLITE_TRANSIENT);

                if (sqlite3_step(st) != SQLITE_DONE) {
                    err = sqlite3_errmsg(db_);
                    sqlite3_finalize(st);
                    break;
                }
                const int ch = sqlite3_changes(db_);
                sqlite3_finalize(st);
                if (ch > 0) {
                    ++written;
                    continue;
                }
                // fall through to insert if no row matched
            }

            // INSERT
            std::string sql = std::format("INSERT INTO {} (", session.table);
            for (std::size_t c = 0; c < cols.size() && c < kMaxFields; ++c) {
                if (c) sql += ",";
                sql += cols[c];
            }
            sql += ") VALUES (";
            for (std::size_t c = 0; c < cols.size() && c < kMaxFields; ++c) {
                if (c) sql += ",";
                sql += "?";
            }
            sql += ")";

            sqlite3_stmt* st = nullptr;
            if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &st, nullptr) != SQLITE_OK) {
                err = sqlite3_errmsg(db_);
                break;
            }
            for (std::size_t c = 0; c < cols.size() && c < kMaxFields; ++c) {
                if (field_is_null(rec, c))
                    sqlite3_bind_null(st, static_cast<int>(c + 1));
                else
                    sqlite3_bind_text(st, static_cast<int>(c + 1), field_text(rec, c).c_str(), -1,
                                      SQLITE_TRANSIENT);
            }
            if (sqlite3_step(st) != SQLITE_DONE) {
                err = sqlite3_errmsg(db_);
                sqlite3_finalize(st);
                break;
            }
            sqlite3_finalize(st);
            ++written;
        }

        if (!err.empty()) {
            rollback();
            return std::unexpected(err);
        }
        if (sqlite3_exec(db_, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK) {
            rollback();
            return std::unexpected(sqlite3_errmsg(db_));
        }

        // Refresh original keys after successful write
        for (std::size_t idx : session.changes.dirty_rows) {
            if (idx < session.rows.size())
                session.rows[idx].original_key = session.rows[idx].is_null[0]
                                                     ? std::string{}
                                                     : session.rows[idx].fields[0];
        }
        session.changes.clear();
        return written;
    }

private:
    sqlite3* db_ = nullptr;
    std::string path_;
    std::string last_error_;
};

} // namespace

std::unique_ptr<IDbStore> make_sqlite_store() {
    return std::make_unique<SqliteStore>();
}

} // namespace jtext_gui
