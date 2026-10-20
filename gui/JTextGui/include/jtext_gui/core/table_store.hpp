#pragma once

/// Ultimate backends: SQLite, MariaDB, PostgreSQL readers/writers + jText file store.
/// GUI talks only to TableSession + these store APIs (testable without wx).

#include "jtext_gui/core/connection_info.hpp"
#include "jtext_gui/core/record.hpp"
#include "jtext_gui/core/session.hpp"

#include <expected>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace jtext_gui {

// ---------------------------------------------------------------------------
// jText file store — load jText → save jText
// ---------------------------------------------------------------------------

struct JTextLoadResult {
    std::vector<Record> rows;
    std::vector<std::string> field_names;  ///< if declared; may be empty
    /// The Fields block line by line: name, jtext_type, length, not_null (the editor
    /// shows them for the line being edited). Same order as field_names.
    std::vector<ColumnDef> field_defs;
    std::string section_name;
    /// True when a data line had no marker and was given the default "|#".
    /// An explicit marker is kept as its right two characters. "#|#" loads as "|#".
    bool filled_default_markers = false;
};

/// Load with the canonical jText library (jText::v001). Field values stay as written.
[[nodiscard]] std::expected<JTextLoadResult, std::string> jtext_load(std::string_view path);

/// Map a catalog type string (format_type, COLUMN_TYPE, or a SQLite declared type)
/// onto a ColumnDef. `not_null` and `primary_key` come from the catalog, not the text.
[[nodiscard]] ColumnDef column_from_declared(std::string name,
                                             std::string_view declared_type,
                                             bool not_null,
                                             bool primary_key,
                                             SessionKind kind);

/// Write rows. With no shape, a light file (no templates). With columns, a full
/// jText file: the standard // file header, Create Table, SQL Insert,
/// SQL Insert - no id, the Fields block, then the rows.
[[nodiscard]] std::expected<void, std::string> jtext_save(std::string_view path,
                                                          const std::vector<Record>& rows,
                                                          std::string_view table_label = "records",
                                                          const TableShape* shape = nullptr);

// ---------------------------------------------------------------------------
// Database reader/writer — same shape for SQLite, MariaDB, PostgreSQL
// ---------------------------------------------------------------------------

struct DbQueryResult {
    std::vector<Record> rows;
    std::vector<std::string> column_names;
    std::vector<ColumnDef> columns;
    std::string key_column;  ///< usually column_names[0]
};

/// Attach catalog columns to a pull. An empty catalog becomes TEXT columns
/// named from the result, so Save As still has a Fields block.
void adopt_columns(DbQueryResult& out, std::vector<ColumnDef> catalog, SessionKind kind);

/// Abstract DB store. Implementations: SqliteStore, MariaDbStore, PostgresStore.
class IDbStore {
public:
    virtual ~IDbStore() = default;

    virtual bool connect(const ConnectionInfo& info) = 0;
    virtual void disconnect() = 0;
    [[nodiscard]] virtual bool is_connected() const = 0;
    [[nodiscard]] virtual SessionKind kind() const = 0;
    /// Why the last connect() failed. Empty after a successful connect.
    [[nodiscard]] virtual std::string last_error() const { return {}; }
    /// Confirm the session with SELECT 1. Does not change the caller's data.
    [[nodiscard]] virtual std::expected<void, std::string> ping() {
        return std::unexpected(std::string("SELECT 1 is not available"));
    }

    /// Pull a table slice. `where` is optional raw predicate (v1); prefer empty or simple forms.
    /// Implementations must reject unsafe table names (identifier check).
    virtual std::expected<DbQueryResult, std::string> query(std::string_view table,
                                                            std::string_view where,
                                                            int limit = 500) = 0;

    /// Apply only dirty/inserted rows from session (UPDATE by key, else INSERT).
    /// Returns number of rows written (update+insert).
    virtual std::expected<std::size_t, std::string> save_changes(TableSession& session) = 0;
};

/// Factory: returns nullptr if backend not built / not available.
[[nodiscard]] std::unique_ptr<IDbStore> make_db_store(SessionKind kind);

/// Validate SQL identifier (table/column): [A-Za-z_][A-Za-z0-9_]* only.
[[nodiscard]] bool is_safe_identifier(std::string_view name);

/// Turn [Field Name] into a dialect identifier. A plain name inside the
/// brackets is left bare so PostgreSQL can fold case. A space or other
/// special character is quoted ("..." or MariaDB backticks). ]] is a
/// literal ]. Text inside single quotes is left unchanged.
[[nodiscard]] std::expected<std::string, std::string>
apply_ident_brackets(std::string_view text, SessionKind kind);

/// One table, schema.table, [Name], or a bare name that contains a space.
[[nodiscard]] std::expected<std::string, std::string>
quote_table_ref(std::string_view name, SessionKind kind);

/// stderr line tagged [jtext], flushed. No passwords or connection strings.
void debug_log(std::string_view message);

// ---------------------------------------------------------------------------
// Session helpers — open + save by kind
// ---------------------------------------------------------------------------

/// Open jText file into session (sets kind JTextFile).
[[nodiscard]] std::expected<void, std::string> session_open_jtext(TableSession& session,
                                                                  std::string_view path);

/// Save session to jText (only valid when kind == JTextFile). Clears dirty on success.
[[nodiscard]] std::expected<void, std::string> session_save_jtext(TableSession& session,
                                                                  std::string_view path = {});

/// Open DB table into session via store (sets kind from store).
[[nodiscard]] std::expected<void, std::string> session_open_db(TableSession& session,
                                                               IDbStore& store,
                                                               std::string_view table,
                                                               std::string_view where = {},
                                                               int limit = 500);

/// Save dirty rows via store (only when is_database_session). Clears dirty on success.
[[nodiscard]] std::expected<std::size_t, std::string> session_save_db(TableSession& session,
                                                                      IDbStore& store);

} // namespace jtext_gui
