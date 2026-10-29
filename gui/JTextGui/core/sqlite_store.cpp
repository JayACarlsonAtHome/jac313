// The SQLite store, over Qlite::v001::Sqlite.
#include "db_stores.hpp"
#include "qlite_store.hpp"

#include <Qlite/v001/Sqlite.hpp>

#include <format>
#include <optional>

namespace jtext_gui {
namespace {

using Qlite::v001::Sqlite;

constexpr qlite_store::Dialect kSqlite{SessionKind::Sqlite, false, '"', "BEGIN IMMEDIATE", "sqlite"};

class SqliteStore final : public IDbStore {
public:
    SessionKind kind() const override { return SessionKind::Sqlite; }

    bool connect(const ConnectionInfo& info) override {
        disconnect();
        if (info.file_path.empty()) {
            last_error_ = "sqlite: no file";
            return false;
        }
        try {
            db_.emplace(info.file_path);
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
        if (!db_) return std::unexpected(last_error_.empty() ? std::string("sqlite: not connected") : last_error_);
        try {
            qlite_store::run(*db_, kSqlite, "SELECT 1");
        } catch (const Qlite::v001::Error& e) {
            return std::unexpected(e.what());
        }
        return {};
    }

    std::expected<DbQueryResult, std::string> query(std::string_view table, std::string_view where,
                                                    int limit) override {
        if (!db_) return std::unexpected("sqlite: not connected");
        auto table_sql = quote_table_ref(table, SessionKind::Sqlite);
        if (!table_sql) return std::unexpected(table_sql.error());
        auto where_sql = apply_ident_brackets(where, SessionKind::Sqlite);
        if (!where_sql) return std::unexpected(where_sql.error());
        auto comments = quote_table_ref(comments_table_of(table), SessionKind::Sqlite);
        if (!comments) return std::unexpected(comments.error());

        DbQueryResult out;
        try {
            // The declared columns: name, type, Not Null, primary key.
            std::vector<ColumnDef> catalog;
            auto info = db_->prepare(std::format("PRAGMA table_info({})", *table_sql));
            while (info.step()) {
                auto name = info.get_at<std::optional<std::string>>(1);
                if (!name) continue;
                const bool pk = info.get_at<std::int64_t>(5) != 0;
                catalog.push_back(column_from_declared(*name, info.get_at<std::string>(2),
                                                       info.get_at<std::int64_t>(3) != 0 || pk, pk,
                                                       SessionKind::Sqlite));
            }
            if (catalog.empty()) return std::unexpected("sqlite: no columns for table " + std::string(table));

            std::string sql = "SELECT * FROM " + *table_sql;
            if (!where_sql->empty()) sql += " WHERE " + *where_sql;   // v1: as written (documented risk)
            if (limit > 0) sql += std::format(" LIMIT {}", limit);
            debug_log("sqlite SQL: " + sql);
            auto st = db_->prepare(sql);
            if (auto r = qlite_store::read_records(st, out); !r) return std::unexpected(r.error());
            out.columns = std::move(catalog);
            if (comments_exist(*comments)) qlite_store::read_comments(*db_, kSqlite, *comments, out);
            auto fields = quote_table_ref(fields_table_of(table), SessionKind::Sqlite);
            if (!fields) return std::unexpected(fields.error());
            qlite_store::read_declarations(*db_, kSqlite, comments_exist(*fields) ? *fields : std::string{}, out);
        } catch (const Qlite::v001::Error& e) {
            return std::unexpected(e.what());
        }
        return out;
    }

    std::expected<std::size_t, std::string> save_changes(TableSession& session) override {
        if (!db_) return std::unexpected("sqlite: not connected");
        return qlite_store::save(*db_, kSqlite, session, [this](const std::string& t) { return comments_exist(t); });
    }

private:
    // A table that cannot be read is taken for a missing one.
    bool comments_exist(const std::string& table) {
        try {
            db_->prepare("SELECT 1 FROM " + table + " LIMIT 0");
            return true;
        } catch (const Qlite::v001::Error&) {
            return false;
        }
    }

    std::optional<Sqlite> db_;
    std::string last_error_;
};

} // namespace

std::unique_ptr<IDbStore> make_sqlite_store() { return std::make_unique<SqliteStore>(); }

} // namespace jtext_gui
