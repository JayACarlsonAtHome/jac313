#pragma once

#include "jtext_gui/core/record.hpp"

#include <cstddef>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace jtext_gui {

/// What we opened — determines how Save works.
enum class SessionKind {
    None,       ///< empty / not loaded
    JTextFile,  ///< load jText → save jText
    Sqlite,     ///< load table → SQL updates
    MariaDB,    ///< load table → SQL updates
    PostgreSQL  ///< load table → SQL updates
};

inline bool is_database_session(SessionKind k) {
    return k == SessionKind::Sqlite || k == SessionKind::MariaDB || k == SessionKind::PostgreSQL;
}

/// One live column, used to render a jText Fields block and the SQL templates.
struct ColumnDef {
    std::string name;
    std::string sql_type;    ///< dialect CREATE type, e.g. VARCHAR(80), INTEGER, DATE
    std::string jtext_type;  ///< String, Integer, Number, Date, DateTime, Bool
    int length = 0;          ///< character length; 0 means none
    bool not_null = false;
    bool primary_key = false;
};

/// Table structure captured on Pull. Save As uses it to write a full jText file.
struct TableShape {
    std::string table;
    SessionKind dialect = SessionKind::None;
    std::vector<ColumnDef> columns;
};

inline const char* session_kind_name(SessionKind k) {
    switch (k) {
    case SessionKind::None:       return "none";
    case SessionKind::JTextFile:  return "jtext";
    case SessionKind::Sqlite:     return "sqlite";
    case SessionKind::MariaDB:    return "mariadb";
    case SessionKind::PostgreSQL: return "postgresql";
    }
    return "unknown";
}

/// Tracks edits so DB saves can write only dirty rows; jText may still rewrite file (v1).
struct ChangeSet {
    std::set<std::size_t> dirty_rows;   ///< indices into session.rows
    std::set<std::size_t> inserted;     ///< new rows (also in dirty)
    std::set<std::size_t> deleted;      ///< indices removed (optional; v1 may clear on reload)

    void clear() {
        dirty_rows.clear();
        inserted.clear();
        deleted.clear();
    }

    [[nodiscard]] bool empty() const {
        return dirty_rows.empty() && inserted.empty() && deleted.empty();
    }

    void mark_dirty(std::size_t row) { dirty_rows.insert(row); }
    void mark_inserted(std::size_t row) {
        inserted.insert(row);
        dirty_rows.insert(row);
    }
};

/// In-memory table slice + persistence context. No UI toolkit.
struct TableSession {
    SessionKind kind = SessionKind::None;

    /// jText path when kind == JTextFile
    std::string jtext_path;

    /// DB identity when kind is a database session
    std::string db_path_or_dsn;  ///< SQLite file path, or host label for network DBs
    std::string table;
    std::string where_clause;
    std::string key_column = "id";  ///< first column used as PK for mergesert
    std::vector<std::string> column_names;
    std::vector<ColumnDef> columns;  ///< empty for a jText file; filled on a database pull

    std::vector<Record> rows;
    ChangeSet changes;

    void clear() {
        kind = SessionKind::None;
        jtext_path.clear();
        db_path_or_dsn.clear();
        table.clear();
        where_clause.clear();
        key_column = "id";
        column_names.clear();
        columns.clear();
        rows.clear();
        changes.clear();
    }

    [[nodiscard]] bool is_open() const { return kind != SessionKind::None; }
    [[nodiscard]] bool is_dirty() const { return !changes.empty(); }

    /// Update field and mark row dirty. Returns false if out of range.
    bool set_field(std::size_t row, std::size_t field, std::string value) {
        if (row >= rows.size() || field >= kMaxFields) return false;
        auto& rec = rows[row];
        if (!rec.is_null[field] && rec.has_line[field] && rec.fields[field] == value) return true;
        rec.is_null[field] = false;
        rec.has_line[field] = true;
        rec.fields[field] = std::move(value);
        changes.mark_dirty(row);
        return true;
    }

    /// Missing line. Distinct from an empty string, which is set_field(row, field, "").
    bool set_null(std::size_t row, std::size_t field) {
        if (row >= rows.size() || field >= kMaxFields) return false;
        auto& rec = rows[row];
        if (rec.is_null[field] && !rec.has_line[field] && rec.fields[field].empty()) return true;
        rec.is_null[field] = true;
        rec.has_line[field] = false;
        rec.fields[field].clear();
        rec.field_markers[field].clear();
        changes.mark_dirty(row);
        return true;
    }

    bool set_marker(std::size_t row, std::string marker) {
        if (row >= rows.size()) return false;
        marker = marker.empty() ? std::string(kDefaultMarker) : live_marker(marker);
        if (rows[row].marker == marker) return true;
        rows[row].marker = std::move(marker);
        changes.mark_dirty(row);
        return true;
    }

    /// Set field's own marker ("" or the record's marker = none of its own).
    /// A longer opener is stored as its right two characters.
    bool set_field_marker(std::size_t row, std::size_t field, std::string marker) {
        if (row >= rows.size() || field >= kMaxFields) return false;
        if (!marker.empty()) marker = live_marker(marker);
        const std::string record_mark = rows[row].marker.empty()
                                            ? std::string(kDefaultMarker)
                                            : live_marker(rows[row].marker);
        if (!marker.empty() && marker == record_mark) marker.clear();
        if (rows[row].field_markers[field] == marker) return true;
        rows[row].field_markers[field] = std::move(marker);
        changes.mark_dirty(row);
        return true;
    }

    std::size_t append_row(Record rec = {}) {
        rec.marker = rec.marker.empty() ? std::string(kDefaultMarker) : live_marker(rec.marker);
        rows.push_back(std::move(rec));
        const std::size_t idx = rows.size() - 1;
        changes.mark_inserted(idx);
        return idx;
    }
};

} // namespace jtext_gui
