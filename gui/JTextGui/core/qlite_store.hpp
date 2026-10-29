#pragma once

// What the editor's database stores share, written once over Qlite (one SQL API over
// SQLite, PostgreSQL and MariaDB: the Database concept). A store gives its connection and
// its dialect; reading a pull, the comments table (SPEC 4.3) and the Mergesert save are
// here. Every value is a bound parameter: none is ever part of the SQL text.

#include "jtext_gui/core/comments_table.hpp"
#include "jtext_gui/core/table_store.hpp"

#include <Qlite/v001/Database.hpp>

#include <algorithm>
#include <cstdint>
#include <expected>
#include <format>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace jtext_gui::qlite_store {

/// How one database writes what differs: its placeholders, its quoted names, how it starts
/// the save's transaction, and its name in the debug log.
struct Dialect {
    SessionKind kind;
    bool numbered;        ///< $1, $2, ... (PostgreSQL); otherwise ?
    char quote;           ///< '"', or '`' for MariaDB
    const char* begin;    ///< the statement that opens the save's transaction
    const char* log;      ///< "sqlite", "postgres", "mariadb"

    [[nodiscard]] std::string placeholder(std::size_t n) const {
        return numbered ? std::format("${}", n) : std::string("?");
    }
    /// A column name as the database gave it, always quoted (a quote in it doubled), so a
    /// PostgreSQL name keeps its case.
    [[nodiscard]] std::string ident(std::string_view name) const {
        std::string out(1, quote);
        for (const char c : name) {
            if (c == quote) out += quote;
            out += c;
        }
        return out + quote;
    }
};

/// One statement, its values bound in order (nullopt: NULL); the rows it changed (an
/// UPDATE: the rows it matched).
template<Qlite::v001::Database D>
std::int64_t run(D& db, const Dialect& d, const std::string& sql,
                 const std::vector<std::optional<std::string>>& values = {}) {
    debug_log(std::format("{} SQL: {}", d.log, sql));
    auto st = db.prepare(sql);
    for (std::size_t i = 0; i < values.size(); ++i) {
        const int idx = static_cast<int>(i + 1);
        if (values[i]) st.bind_text(idx, *values[i]);
        else st.bind_null(idx);
    }
    while (st.step()) {}
    return db.changes();
}

/// The rows of a SELECT as records, each column a line (SQL NULL: a {NULL} line).
template<class Statement>
std::expected<void, std::string> read_records(Statement& st, DbQueryResult& out) {
    const int ncols = st.column_count();
    if (static_cast<std::size_t>(ncols) > kMaxFields) return std::unexpected(too_many_fields(static_cast<std::size_t>(ncols)));
    for (int c = 0; c < ncols; ++c) out.column_names.push_back(st.column_name(c));
    if (!out.column_names.empty()) out.key_column = out.column_names.front();
    while (st.step()) {
        Record rec;
        rec.marker = kDefaultMarker;
        rec.ensure_size(static_cast<std::size_t>(ncols));
        rec.mark_all_absent();
        for (int c = 0; c < ncols; ++c) {
            auto v = st.template get_at<std::optional<std::string>>(c);
            const auto col = static_cast<std::size_t>(c);
            rec.has_line[col] = true;
            if (!v) continue;   // NULL: is_null stays set
            rec.is_null[col] = false;
            rec.fields[col] = std::move(*v);
        }
        rec.original_key = rec.is_null[0] ? std::string{} : rec.fields[0];
        out.rows.push_back(std::move(rec));
    }
    return {};
}

/// The pulled records' comments from the comments table `table` (which exists), and the
/// comment lines at the end of the data.
template<Qlite::v001::Database D>
void read_comments(D& db, const Dialect& d, const std::string& table, DbQueryResult& out) {
    const std::string sql = "SELECT kind, record_id, position, seq, comment FROM " + table +
                            " WHERE block = 'data'";
    debug_log(std::format("{} SQL: {}", d.log, sql));
    auto st = db.prepare(sql);
    std::vector<CommentRow> rows;
    for (auto& [kind, id, position, seq, text] : st.template get_all<std::string, std::int64_t, std::int64_t,
                                                                     std::int64_t, std::string>())
        rows.push_back({std::move(kind), id, static_cast<unsigned>(position), static_cast<unsigned>(seq),
                        std::move(text)});
    place_comment_rows(out.rows, std::move(rows), out.end_comments);
}

/// The column declarations: those <table>_fields (`table`, which exists) stores, and the
/// rest from the columns. With no such table, `table` is empty: all from the columns.
template<Qlite::v001::Database D>
void read_declarations(D& db, const Dialect& d, const std::string& table, DbQueryResult& out) {
    std::map<unsigned, std::vector<std::string>> stored;
    if (!table.empty()) {
        const std::string sql = "SELECT position, level, text FROM " + table + " ORDER BY position, level";
        debug_log(std::format("{} SQL: {}", d.log, sql));
        auto st = db.prepare(sql);
        for (auto& [position, level, text] : st.template get_all<std::int64_t, std::int64_t, std::string>())
            stored[static_cast<unsigned>(position)].push_back(std::move(text));
    }
    out.declarations = declarations_for(out.columns, stored);
}

/// Mergesert, in one transaction: each changed record is an UPDATE of every column by its
/// original key (so a changed key is saved too), else (a new record, or no row had that
/// key) an INSERT. Then each changed record's rows in the comments table are replaced; the
/// table is made, before the transaction, when the first comment is saved. A table with no
/// <table>_fields gets one, with the session's declarations (SPEC 4.3).
/// `comments_exist(table)` tells whether the comments table is there.
template<Qlite::v001::Database D, class Exists>
std::expected<std::size_t, std::string> save(D& db, const Dialect& d, TableSession& session,
                                             Exists comments_exist) {
    JTEXT_PRE(session.changes.in_range(session.rows.size()));
    auto table = quote_table_ref(session.table, d.kind);
    if (!table) return std::unexpected(table.error());
    auto comments = quote_table_ref(comments_table_of(session.table), d.kind);
    if (!comments) return std::unexpected(comments.error());
    auto fields = quote_table_ref(fields_table_of(session.table), d.kind);
    if (!fields) return std::unexpected(fields.error());
    const auto& cols = session.column_names;
    if (cols.empty()) return std::unexpected(std::string(d.log) + ": no columns (open the table again)");
    const std::string key = d.ident(session.key_column.empty() ? cols[0] : session.key_column);
    const std::size_t width = std::min(cols.size(), kMaxFields);
    const auto changes = comment_changes(session);
    if (!changes) return std::unexpected(changes.error());
    const bool end_changed = session.changes.end_comments;

    std::string update = "UPDATE " + *table + " SET ";
    std::string insert = "INSERT INTO " + *table + " (";
    std::string marks;
    for (std::size_t c = 0; c < width; ++c) {
        update += std::format("{}{} = {}", c ? ", " : "", d.ident(cols[c]), d.placeholder(c + 1));
        insert += (c ? ", " : "") + d.ident(cols[c]);
        marks += (c ? ", " : "") + d.placeholder(c + 1);
    }
    update += std::format(" WHERE {} = {}", key, d.placeholder(width + 1));
    insert += ") VALUES (" + marks + ")";
    const std::string del = std::format("DELETE FROM {} WHERE block = 'data' AND record_id = {}", *comments,
                                        d.placeholder(1));
    const std::string ins = std::format(
        "INSERT INTO {} (block, kind, record_id, position, seq, comment) VALUES ('data', {}, {}, {}, {}, {})",
        *comments, d.placeholder(1), d.placeholder(2), d.placeholder(3), d.placeholder(4), d.placeholder(5));

    std::size_t written = 0;
    bool open = false;
    try {
        const bool write_fields = !session.declarations.empty() && !comments_exist(*fields);
        if (write_fields) run(db, d, create_fields_table_sql(*fields));
        bool have_comments = comments_exist(*comments);
        if (!have_comments && (any_comment_rows(*changes) || (end_changed && !session.end_comments.empty()))) {
            run(db, d, create_comments_table_sql(*comments));
            have_comments = true;
        }
        run(db, d, d.begin);
        open = true;
        if (write_fields) {
            const std::string add = std::format("INSERT INTO {} (position, level, text) VALUES ({}, {}, {})", *fields,
                                                d.placeholder(1), d.placeholder(2), d.placeholder(3));
            for (std::size_t p = 0; p < session.declarations.size(); ++p)
                for (std::size_t level = 0; level < session.declarations[p].size(); ++level)
                    run(db, d, add, {std::to_string(p + 1), std::to_string(level + 1), session.declarations[p][level]});
        }
        for (std::size_t idx : session.changes.dirty_rows) {
            const Record& rec = session.rows[idx];
            std::vector<std::optional<std::string>> values;
            for (std::size_t c = 0; c < width; ++c)
                values.push_back(field_is_null(rec, c) ? std::nullopt
                                                       : std::optional<std::string>(field_text(rec, c)));
            if (!session.changes.inserted.count(idx) && !rec.original_key.empty()) {
                auto by_key = values;
                by_key.emplace_back(rec.original_key);
                if (run(db, d, update, by_key) > 0) {
                    ++written;
                    continue;
                }
            }
            run(db, d, insert, values);
            ++written;
        }
        if (have_comments) {
            for (const auto& change : *changes) {
                for (const auto id : change.replace_ids) run(db, d, del, {std::to_string(id)});
                for (const auto& c : change.rows)
                    run(db, d, ins, {c.kind, std::to_string(c.record_id), std::to_string(c.position),
                                     std::to_string(c.seq), c.text});
            }
            if (end_changed) {   // the comment lines at the end of the data: position 0
                run(db, d, std::format("DELETE FROM {} WHERE block = 'data' AND position = 0", *comments));
                std::size_t seq = 0;
                for (const auto& text : session.end_comments)
                    run(db, d, ins, {"comment-line", "0", "0", std::to_string(++seq), text});
            }
        }
        run(db, d, "COMMIT");
        open = false;
    } catch (const Qlite::v001::Error& e) {
        if (open) {
            try { run(db, d, "ROLLBACK"); } catch (const Qlite::v001::Error&) {}
        }
        return std::unexpected(e.what());
    }

    for (std::size_t idx : session.changes.dirty_rows) {
        Record& rec = session.rows[idx];
        rec.original_key = field_is_null(rec, 0) ? std::string{} : field_text(rec, 0);
    }
    session.changes.clear();
    return written;
}

} // namespace jtext_gui::qlite_store
