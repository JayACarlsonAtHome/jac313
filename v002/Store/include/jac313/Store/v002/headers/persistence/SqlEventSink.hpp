#pragma once

// SqlEventSink.hpp
// Direct-to-SQL persistence sink for ts_store.
// Writes events as INSERTs (prepared statements) to SQLite DB.
// For debug, can also write the equivalent INSERT statements as text to a .sql file
// (so you can inspect/replay the straight SQL).
// Designed to work with DoubleBufferedWriter for asynchronous background draining.

#include "EventSink.hpp"
#include "Sqlite.hpp"
#include "PersistCommon.hpp"

#ifndef JAC313_STORE_IMPORT_STD
#include <memory>
#include <fstream>
#include <string>
#include <string_view>
#include <filesystem>
#endif

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v002 {
#endif

class SqlEventSink : public IEventSink {
public:
    // base_name: used for "base.db" (the SQLite DB) and "base.sql" (debug insert statements file, if enabled)
    // int_count, dbl_count: number of metric columns (determines schema for _ints / _floats tables)
    // mode: All, KeeperOnly (bit 1), or DatabaseOnly (bit 2)
    // write_debug_sql: if true, also emit textual INSERT statements to base.sql for debugging/replay
    SqlEventSink(std::string_view base_name,
                 size_t int_count,
                 size_t dbl_count,
                 PersistMode mode = PersistMode::All,
                 bool write_debug_sql = false);

    ~SqlEventSink() override;

    void write_batch(std::span<const PersistedEvent> batch) override;
    void flush() override;
    void finalize() override;

    std::string_view name() const override { return "SqlEventSink"; }

    [[nodiscard]] size_t main_row_count() const { return main_rows_inserted_; }

private:
    void ensure_tables_and_prepare();
    void insert_event(const PersistedEvent& e);
    void write_debug_insert(const PersistedEvent& e);
    std::string escape_for_sql(const std::string& s) const;

    std::unique_ptr<Sqlite> db_;
    std::ofstream debug_sql_;

    std::string db_path_;
    std::string debug_path_;
    std::string table_base_;   // e.g. "persist" (filename part of base_name)
    size_t int_count_ = 0;
    size_t dbl_count_ = 0;
    PersistMode mode_ = PersistMode::All;

    // Prepared statements for the three tables
    std::unique_ptr<Sqlite::Statement> stmt_main_;
    std::unique_ptr<Sqlite::Statement> stmt_ints_;
    std::unique_ptr<Sqlite::Statement> stmt_dbls_;

    bool finalized_ = false;
    size_t main_rows_inserted_ = 0;
};

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v002
#endif