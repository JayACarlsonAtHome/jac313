#include "jtext_gui/core/comments_table.hpp"

#include <algorithm>
#include <cctype>
#include <format>
#include <tuple>

namespace jtext_gui {

std::string comments_table_of(std::string_view table) {
    std::string out(table);
    if (!out.empty() && out.back() == ']') out.insert(out.size() - 1, "_comments");
    else out += "_comments";
    return out;
}

std::string create_comments_table_sql(std::string_view quoted) {
    return std::format("CREATE TABLE IF NOT EXISTS {} (\n"
                       "    block     TEXT    NOT NULL,\n"
                       "    kind      TEXT    NOT NULL,\n"
                       "    record_id INTEGER NOT NULL,\n"
                       "    position  INTEGER NOT NULL,\n"
                       "    seq       INTEGER NOT NULL,\n"
                       "    comment   TEXT    NOT NULL\n"
                       ")", quoted);
}

std::string fields_table_of(std::string_view table) {
    std::string out(table);
    if (!out.empty() && out.back() == ']') out.insert(out.size() - 1, "_fields");
    else out += "_fields";
    return out;
}

std::string create_fields_table_sql(std::string_view quoted) {
    return std::format("CREATE TABLE IF NOT EXISTS {} (\n"
                       "    position  INTEGER NOT NULL,\n"
                       "    level     INTEGER NOT NULL,\n"
                       "    text      TEXT    NOT NULL\n"
                       ")", quoted);
}

std::vector<std::string> declaration_of(const ColumnDef& col) {
    std::vector<std::string> d{col.name, col.jtext_type.empty() ? std::string("String") : col.jtext_type};
    if (col.length > 0 && d[1] == "String") d.push_back(std::to_string(col.length));
    d.emplace_back(col.not_null ? "Not Null" : "Nullable");
    return d;
}

std::vector<std::vector<std::string>> declarations_for(const std::vector<ColumnDef>& cols,
                                                       const std::map<unsigned, std::vector<std::string>>& stored) {
    const auto same = [](std::string_view a, std::string_view b) {
        return std::ranges::equal(a, b, [](unsigned char x, unsigned char y) { return std::tolower(x) == std::tolower(y); });
    };
    std::vector<std::vector<std::string>> out;
    for (std::size_t i = 0; i < cols.size(); ++i) {
        const auto it = stored.find(static_cast<unsigned>(i + 1));
        const bool use = it != stored.end() && !it->second.empty() && same(it->second[0], cols[i].name);
        out.push_back(use ? it->second : declaration_of(cols[i]));
    }
    return out;
}

std::vector<CommentRow> comment_rows_of(const Record& rec, std::int64_t record_id) {
    std::vector<CommentRow> out;
    for (const auto& [f, texts] : rec.comment_lines) {
        unsigned seq = 0;
        for (const auto& t : texts)
            out.push_back({"comment-line", record_id, static_cast<unsigned>(f + 1), ++seq, t});
    }
    for (const auto& [f, text] : rec.line_comments)
        out.push_back({"line", record_id, static_cast<unsigned>(f + 1), 0, text});
    return out;
}

void place_comment_rows(std::vector<Record>& rows, std::vector<CommentRow> comments,
                        std::vector<std::string>& end) {
    std::ranges::sort(comments, [](const CommentRow& a, const CommentRow& b) {
        return std::tie(a.record_id, a.position, a.seq) < std::tie(b.record_id, b.position, b.seq);
    });
    for (auto& c : comments) {
        if (c.position == 0) {
            end.push_back(std::move(c.text));
            continue;
        }
        if (c.position > kMaxFields) continue;
        const auto rec = std::ranges::find_if(rows, [&](const Record& r) { return record_id_of(r) == c.record_id; });
        if (rec == rows.end()) continue;
        const std::size_t f = c.position - 1;
        if (c.kind == "line") rec->line_comments[f] = std::move(c.text);
        else rec->comment_lines[f].push_back(std::move(c.text));
    }
}

std::expected<std::vector<CommentChange>, std::string> comment_changes(const TableSession& session) {
    JTEXT_PRE(session.changes.in_range(session.rows.size()));
    std::vector<CommentChange> out;
    for (std::size_t idx : session.changes.dirty_rows) {
        const Record& rec = session.rows[idx];
        CommentChange change;
        if (auto before = parse_record_id(rec.original_key)) change.replace_ids.push_back(*before);
        const auto id = record_id_of(rec);
        if (id && std::ranges::find(change.replace_ids, *id) == change.replace_ids.end())
            change.replace_ids.push_back(*id);
        if (!rec.line_comments.empty() || !rec.comment_lines.empty()) {
            if (!id)
                return std::unexpected(std::format(
                    "record {} has comments, but field 1 is not an integer record id: "
                    "<table>_comments finds a record's comments by it", idx + 1));
            change.rows = comment_rows_of(rec, *id);
        }
        if (!change.replace_ids.empty() || !change.rows.empty()) out.push_back(std::move(change));
    }
    return out;
}

bool any_comment_rows(const std::vector<CommentChange>& changes) {
    return std::ranges::any_of(changes, [](const CommentChange& c) { return !c.rows.empty(); });
}

} // namespace jtext_gui
