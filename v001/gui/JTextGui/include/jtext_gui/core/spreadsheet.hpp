#pragma once

// Excel (.xlsx) files, and moving records in and out of them. (No CSV, by design: it loses
// types and encoding, and spreadsheet programs rewrite its values.)
//
// A Sheet is rows of text cells; row 0 is the header (field names). Export writes a
// Sheet from the records; import reads one and merges it into the records by record id:
// a row whose id exists updates that record, a row with a new id adds a record, nothing
// is deleted, and ids are never changed. An empty cell is an empty string (never NULL);
// an empty cell where the record holds NULL counts as unchanged, so importing an
// unchanged export changes nothing.

#include "jtext_gui/core/session.hpp"

#include <cstddef>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

namespace jtext_gui {

struct Sheet {
    std::vector<std::vector<std::string>> rows;   ///< rows[0] is the header
};

/// How an .xlsx cell is written: text stays text (Excel does not turn "0012" into 12 or
/// "3-4" into a date), numbers and dates become real numbers and dates when they parse.
enum class CellType { Text, Number, Date };

// --- Excel .xlsx --------------------------------------------------------------------------
/// One worksheet: a bold, frozen header row, columns sized to their contents.
/// types[i] is column i's type (missing = Text).
[[nodiscard]] std::expected<void, std::string> write_xlsx_file(const std::string& path, const Sheet& sheet,
                                                               const std::vector<CellType>& types,
                                                               const std::string& sheet_name);
/// The first worksheet, every cell as text: numbers as written, dates as YYYY-MM-DD
/// (YYYY-MM-DD HH:MM:SS with a time), booleans TRUE/FALSE.
[[nodiscard]] std::expected<Sheet, std::string> read_xlsx_file(const std::string& path);

// --- records <-> sheet --------------------------------------------------------------------
/// A field's name for headers: the database column or Fields block name, else "line N".
[[nodiscard]] std::string field_header(const TableSession& s, std::size_t field);
/// The column type for .xlsx, from the field's jText type (Integer / Number / Date).
[[nodiscard]] CellType field_cell_type(const TableSession& s, std::size_t field);

/// rows: indexes into s.rows; fields: field indexes (the id, field 0, is always first).
/// NULL and empty values are both empty cells.
[[nodiscard]] Sheet session_to_sheet(const TableSession& s, const std::vector<std::size_t>& rows,
                                     std::vector<std::size_t> fields);

struct ImportSummary {
    int records_updated = 0;   ///< existing records with at least one changed value
    int values_changed = 0;
    int records_added = 0;
    int rows_skipped = 0;      ///< no usable record id
    int columns_used = 0;      ///< including the id column
    std::vector<std::string> notes;   ///< ignored columns, skipped rows (the first few)
};

/// Merge sheet into s by record id (see the top of this file). apply = false only counts
/// (the preview); apply = true makes the changes through the session, so they are unsaved
/// edits like any other. Fails when there is no header or no id column ("id", or "line 1").
[[nodiscard]] std::expected<ImportSummary, std::string> import_sheet(TableSession& s, const Sheet& sheet,
                                                                     bool apply);

} // namespace jtext_gui
