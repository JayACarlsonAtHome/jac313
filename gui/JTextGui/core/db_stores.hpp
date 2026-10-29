#pragma once

// The database stores the factory (make_db_store) chooses from. Each lives in its own file;
// a database whose client library was not found at build time is given its *_none.cpp,
// whose store says so (CMake picks the file, so no code is switched off with macros).

#include "jtext_gui/core/table_store.hpp"

#include <memory>
#include <string>

namespace jtext_gui {

std::unique_ptr<IDbStore> make_sqlite_store();     // sqlite_store.cpp
std::unique_ptr<IDbStore> make_postgres_store();   // postgres_store.cpp, or postgres_none.cpp
std::unique_ptr<IDbStore> make_mariadb_store();    // mariadb_store.cpp, or mariadb_none.cpp

/// A database this build has no client for: every call fails with `why`.
class UnavailableStore final : public IDbStore {
public:
    UnavailableStore(SessionKind k, std::string why) : kind_(k), why_(std::move(why)) {}
    SessionKind kind() const override { return kind_; }
    bool connect(const ConnectionInfo&) override { return false; }
    void disconnect() override {}
    bool is_connected() const override { return false; }
    std::string last_error() const override { return why_; }
    std::expected<void, std::string> ping() override { return std::unexpected(why_); }
    std::expected<DbQueryResult, std::string> query(std::string_view, std::string_view, int) override {
        return std::unexpected(why_);
    }
    std::expected<std::size_t, std::string> save_changes(TableSession&) override {
        return std::unexpected(why_);
    }

private:
    SessionKind kind_;
    std::string why_;
};

} // namespace jtext_gui
