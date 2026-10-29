#pragma once

// A table's comments in a database (SPEC 4.3): a column holds values, so the comments of
// a table's records live beside it, in <table>_comments. A line is named by its record
// (field 1, the record id) and the field it starts at, so each comment goes back on its
// line, and each comment line back where it stood. It holds no nulls. Rows of the Fields
// block (block 'fields') are kept as they are: the editor writes only the data's.

#include "jtext_gui/core/record.hpp"
#include "jtext_gui/core/session.hpp"

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

namespace jtext_gui {

/// One row of <table>_comments with block 'data'.
struct CommentRow {
    std::string kind;            ///< "line" (the comment on a line) or "comment-line"
    std::int64_t record_id = 0;  ///< field 1 of the record
    unsigned position = 0;       ///< the field the line starts at (1-based); 0: the end of the data
    unsigned seq = 0;            ///< comment lines standing together: 1, 2, ...; a line's comment: 0
    std::string text;
};

/// The comments table of `table`: "items" -> "items_comments", "app.items" ->
/// "app.items_comments", "[My Items]" -> "[My Items_comments]".
[[nodiscard]] std::string comments_table_of(std::string_view table);

/// CREATE TABLE IF NOT EXISTS for it; `quoted` is the name as the dialect writes it.
[[nodiscard]] std::string create_comments_table_sql(std::string_view quoted);

/// The rows of one record's comments, by its record id.
[[nodiscard]] std::vector<CommentRow> comment_rows_of(const Record& rec, std::int64_t record_id);

/// Each row back on its record (by record id; rows of records not in `rows` are left
/// out). Rows at position 0 are the comment lines at the end of the data: `end`. Comment
/// lines stand in `seq` order.
void place_comment_rows(std::vector<Record>& rows, std::vector<CommentRow> comments,
                        std::vector<std::string>& end);

/// What saving one changed record does to <table>_comments: delete the rows of
/// `replace_ids` (its record id before the edit, and now), then insert `rows`.
struct CommentChange {
    std::vector<std::int64_t> replace_ids;
    std::vector<CommentRow> rows;
};

/// The comment changes of a save: one per changed record. A record with comments needs an
/// integer record id (field 1), which names it in the table; without one the save is refused.
[[nodiscard]] std::expected<std::vector<CommentChange>, std::string> comment_changes(const TableSession& session);

/// True when some change has a row to insert (the table is then made if it is missing).
[[nodiscard]] bool any_comment_rows(const std::vector<CommentChange>& changes);

} // namespace jtext_gui
