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
    /// The Fields block as declared (parts per column); empty: from `columns`.
    std::vector<std::vector<std::string>> declarations;
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
    bool end_comments = false;          ///< the comment lines at the end of the data changed

    void clear() {
        dirty_rows.clear();
        inserted.clear();
        deleted.clear();
        end_comments = false;
    }

    [[nodiscard]] bool empty() const {
        return dirty_rows.empty() && inserted.empty() && deleted.empty() && !end_comments;
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
    /// The file's sections when kind == JTextFile; `table` is the one open.
    std::vector<std::string> jtext_sections;
    /// What opening the jText file warned about (e.g. an older jText version).
    std::vector<std::string> jtext_warnings;
    /// The comment lines at the end of the data (records keep their own): a jText
    /// section's, or a table's, from its <table>_comments.
    std::vector<std::string> end_comments;

    /// DB identity when kind is a database session
    std::string db_path_or_dsn;  ///< SQLite file path, or host label for network DBs
    std::string table;
    std::string where_clause;
    std::string key_column = "id";  ///< first column used as PK for mergesert
    std::vector<std::string> column_names;
    std::vector<ColumnDef> columns;  ///< empty for a jText file; filled on a database pull
    /// A database session's field declarations (parts), one per column: from <table>_fields
    /// (Not Empty, NullDate(...), auto designations), else from the column.
    std::vector<std::vector<std::string>> declarations;
    /// A jText file's Fields block (name, type, length, Not Null), for display only;
    /// a database session describes its lines with `columns` instead.
    std::vector<ColumnDef> field_defs;

    std::vector<Record> rows;
    ChangeSet changes;
    /// How values compare (the Filter, lookups), like a database collation; values are
    /// always kept as written. A jText file: its Case line. A database: the server's
    /// default (SQLite, PostgreSQL compare with case; MariaDB without). New: Sensitive.
    bool case_sensitive = true;

    void clear() {
        kind = SessionKind::None;
        jtext_path.clear();
        jtext_sections.clear();
        jtext_warnings.clear();
        end_comments.clear();
        db_path_or_dsn.clear();
        table.clear();
        where_clause.clear();
        key_column = "id";
        column_names.clear();
        columns.clear();
        declarations.clear();
        field_defs.clear();
        rows.clear();
        changes.clear();
        case_sensitive = true;
    }

    [[nodiscard]] bool is_open() const { return kind != SessionKind::None; }
    [[nodiscard]] bool is_dirty() const { return !changes.empty(); }

    /// Update field and mark row dirty; the record grows to reach it. Returns false if
    /// out of range (row, or field >= kMaxFields).
    bool set_field(std::size_t row, std::size_t field, std::string value) {
        if (row >= rows.size() || field >= kMaxFields) return false;
        auto& rec = rows[row];
        rec.ensure_size(field + 1);
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
        if (field >= rec.size()) return true;   // past the end is already absent
        if (rec.is_null[field] && !rec.has_line[field] && rec.fields[field].empty()) return true;
        rec.is_null[field] = true;
        rec.has_line[field] = false;
        rec.fields[field].clear();
        rec.field_markers[field].clear();
        changes.mark_dirty(row);
        return true;
    }

    /// The comment lines standing before the line that starts at field (SPEC 3.2.1), in
    /// order; none removes them.
    bool set_comment_lines(std::size_t row, std::size_t field, std::vector<std::string> lines) {
        if (row >= rows.size() || field >= kMaxFields) return false;
        auto& all = rows[row].comment_lines;
        const auto it = all.find(field);
        if (lines.empty()) {
            if (it == all.end()) return true;
            all.erase(it);
        } else {
            if (it != all.end() && it->second == lines) return true;
            all[field] = std::move(lines);
        }
        changes.mark_dirty(row);
        return true;
    }

    /// The comment lines at the end of the data.
    void set_end_comments(std::vector<std::string> lines) {
        if (lines == end_comments) return;
        end_comments = std::move(lines);
        changes.end_comments = true;
    }

    /// A {NULL} line: the field holds SQL NULL (SPEC 2.6.1; the file writes {NULL}). Not
    /// the same as a missing line (set_null), which is an empty value.
    bool set_null_value(std::size_t row, std::size_t field) {
        if (row >= rows.size() || field >= kMaxFields) return false;
        auto& rec = rows[row];
        rec.ensure_size(field + 1);
        if (rec.is_null[field] && rec.has_line[field]) return true;
        rec.is_null[field] = true;
        rec.has_line[field] = true;
        rec.fields[field].clear();
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
        if (field >= rows[row].size() && marker.empty()) return true;   // nothing to record
        rows[row].ensure_size(field + 1);
        if (rows[row].field_markers[field] == marker) return true;
        rows[row].field_markers[field] = std::move(marker);
        changes.mark_dirty(row);
        return true;
    }

    /// The comment on the line that starts at field (SPEC 3.2.1); "" removes it. The text
    /// only: the file's comment character is added when it is written.
    bool set_line_comment(std::size_t row, std::size_t field, std::string text) {
        if (row >= rows.size() || field >= kMaxFields) return false;
        auto& comments = rows[row].line_comments;
        const auto it = comments.find(field);
        if (text.empty()) {
            if (it == comments.end()) return true;
            comments.erase(it);
        } else {
            if (it != comments.end() && it->second == text) return true;
            comments[field] = std::move(text);
        }
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
