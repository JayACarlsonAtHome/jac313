#include "jtext_gui/core/table_store.hpp"

#include <cctype>
#include <cstdio>
#include <format>
#include <vector>

namespace jtext_gui {

void debug_log(std::string_view message) {
    std::fprintf(stderr, "[jtext] %.*s\n", static_cast<int>(message.size()), message.data());
    std::fflush(stderr);
}

bool is_safe_identifier(std::string_view name) {
    if (name.empty() || name.size() > 128) return false;
    if (!(std::isalpha(static_cast<unsigned char>(name[0])) || name[0] == '_'))
        return false;
    for (char c : name) {
        if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_'))
            return false;
    }
    return true;
}

namespace {

std::string trim_copy(std::string_view in) {
    while (!in.empty() && std::isspace(static_cast<unsigned char>(in.front())))
        in.remove_prefix(1);
    while (!in.empty() && std::isspace(static_cast<unsigned char>(in.back())))
        in.remove_suffix(1);
    return std::string(in);
}

bool control_char(unsigned char c) { return c < 0x20 || c == 0x7f; }

std::string quote_ident(std::string_view name, SessionKind kind) {
    const char q = kind == SessionKind::MariaDB ? '`' : '"';
    std::string out;
    out.push_back(q);
    for (unsigned char c : name) {
        if (c == static_cast<unsigned char>(q)) out.push_back(q);
        out.push_back(static_cast<char>(c));
    }
    out.push_back(q);
    return out;
}

std::expected<std::string, std::string> ident_from_brackets(std::string_view interior, SessionKind kind) {
    std::string name;
    for (std::size_t i = 0; i < interior.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(interior[i]);
        if (control_char(c))
            return std::unexpected("bracketed name contains a control character");
        if (interior[i] == ']' && i + 1 < interior.size() && interior[i + 1] == ']') {
            name.push_back(']');
            ++i;
            continue;
        }
        name.push_back(interior[i]);
    }
    if (name.empty()) return std::unexpected("empty []");
    if (is_safe_identifier(name)) return name;
    return quote_ident(name, kind);
}

std::expected<std::string, std::string> one_name(std::string_view part, SessionKind kind) {
    if (part.empty()) return std::unexpected("empty name");
    if (part.front() == '[') {
        if (part.size() < 2 || part.back() != ']')
            return std::unexpected("unclosed [ in name");
        return ident_from_brackets(part.substr(1, part.size() - 2), kind);
    }
    if (is_safe_identifier(part)) return std::string(part);
    for (unsigned char c : part)
        if (control_char(c)) return std::unexpected("name contains a control character");
    return quote_ident(part, kind);
}

} // namespace

std::expected<std::string, std::string> apply_ident_brackets(std::string_view text, SessionKind kind) {
    std::string out;
    out.reserve(text.size());
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\'') {
            out.push_back('\'');
            for (++i; i < text.size(); ++i) {
                out.push_back(text[i]);
                if (text[i] == '\'') {
                    if (i + 1 < text.size() && text[i + 1] == '\'') {
                        out.push_back(text[++i]);
                        continue;
                    }
                    break;
                }
            }
            continue;
        }
        if (text[i] != '[') {
            out.push_back(text[i]);
            continue;
        }
        const std::size_t start = i + 1;
        std::size_t end = start;
        bool closed = false;
        for (; end < text.size(); ++end) {
            if (text[end] == ']' && end + 1 < text.size() && text[end + 1] == ']') {
                ++end;
                continue;
            }
            if (text[end] == ']') {
                closed = true;
                break;
            }
        }
        if (!closed) return std::unexpected("unclosed [ in the condition");
        auto ident = ident_from_brackets(text.substr(start, end - start), kind);
        if (!ident) return ident;
        out += *ident;
        i = end;
    }
    return out;
}

std::expected<std::string, std::string> quote_table_ref(std::string_view name, SessionKind kind) {
    const std::string text = trim_copy(name);
    if (text.empty() || text.size() > 256)
        return std::unexpected(std::format("unsafe table name: '{}'", text));

    std::vector<std::string> parts;
    std::string cur;
    bool in_brackets = false;
    const bool split_dots = text.find('[') != std::string::npos || text.find(' ') == std::string::npos;
    if (!split_dots) return one_name(text, kind);

    for (std::size_t i = 0; i < text.size(); ++i) {
        const char c = text[i];
        if (c == '[') in_brackets = true;
        if (c == ']' && in_brackets) {
            if (i + 1 < text.size() && text[i + 1] == ']') {
                cur.push_back(']');
                cur.push_back(']');
                ++i;
                continue;
            }
            in_brackets = false;
            cur.push_back(c);
            continue;
        }
        if (c == '.' && !in_brackets) {
            parts.push_back(cur);
            cur.clear();
            continue;
        }
        cur.push_back(c);
    }
    parts.push_back(cur);
    if (parts.empty() || parts.size() > 2)
        return std::unexpected(std::format("unsafe table name: '{}'", text));

    std::string out;
    for (std::size_t i = 0; i < parts.size(); ++i) {
        auto one = one_name(parts[i], kind);
        if (!one) return one;
        if (i) out.push_back('.');
        out += *one;
    }
    return out;
}

std::expected<void, std::string> session_open_jtext(TableSession& session,
                                                    std::string_view path) {
    auto loaded = jtext_load(path);
    if (!loaded) return std::unexpected(loaded.error());

    session.clear();
    session.kind = SessionKind::JTextFile;
    session.jtext_path = std::string(path);
    session.table = loaded->section_name;
    session.column_names = std::move(loaded->field_names);
    session.rows = std::move(loaded->rows);
    session.changes.clear();
    // Lines that gained |# were not in the file. The file is dirty until saved.
    if (loaded->filled_default_markers) {
        for (std::size_t i = 0; i < session.rows.size(); ++i)
            session.changes.mark_dirty(i);
    }
    return {};
}

std::expected<void, std::string> session_save_jtext(TableSession& session,
                                                    std::string_view path) {
    if (session.kind != SessionKind::JTextFile)
        return std::unexpected("session_save_jtext: session is not a jText file");

    const std::string out_path = path.empty() ? session.jtext_path : std::string(path);
    if (out_path.empty())
        return std::unexpected("session_save_jtext: no path");

    auto r = jtext_save(out_path, session.rows, session.table.empty() ? "records" : session.table);
    if (!r) return std::unexpected(r.error());

    session.jtext_path = out_path;
    for (auto& row : session.rows)
        row.original_key = row.is_null[0] ? std::string{} : row.fields[0];
    session.changes.clear();
    return {};
}

std::expected<void, std::string> session_open_db(TableSession& session,
                                                 IDbStore& store,
                                                 std::string_view table,
                                                 std::string_view where,
                                                 int limit) {
    if (!store.is_connected())
        return std::unexpected("session_open_db: store not connected");
    auto table_sql = quote_table_ref(table, store.kind());
    if (!table_sql) return std::unexpected(table_sql.error());
    auto where_sql = apply_ident_brackets(where, store.kind());
    if (!where_sql) return std::unexpected(where_sql.error());
    debug_log(std::format("open_db table={} | where={} | limit={}", *table_sql, *where_sql, limit));

    auto q = store.query(table, where, limit);
    if (!q) {
        debug_log("pull error: " + q.error());
        return std::unexpected(q.error());
    }

    session.clear();
    session.kind = store.kind();
    session.table = std::string(table);
    session.where_clause = std::string(where);
    session.column_names = std::move(q->column_names);
    session.columns = std::move(q->columns);
    session.key_column = q->key_column.empty() ? "id" : q->key_column;
    session.rows = std::move(q->rows);
    session.changes.clear();
    return {};
}

std::expected<std::size_t, std::string> session_save_db(TableSession& session,
                                                        IDbStore& store) {
    if (!is_database_session(session.kind))
        return std::unexpected("session_save_db: not a database session");
    if (session.kind != store.kind())
        return std::unexpected("session_save_db: store kind mismatch");
    if (session.changes.empty())
        return 0; // nothing to do — success
    return store.save_changes(session);
}

} // namespace jtext_gui
