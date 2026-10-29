#include "jtext_gui/core/table_store.hpp"

#include "jText.h"
#include "parser/section.hpp"
#include "parser/utf8.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace jtext_gui {
namespace {

std::string trim(std::string s) {
    auto not_space = [](unsigned char c) { return !std::isspace(c); };
    while (!s.empty() && !not_space(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
    while (!s.empty() && !not_space(static_cast<unsigned char>(s.back()))) s.pop_back();
    return s;
}

/// A multiline value ("[*" ... "*]") arrives as one field holding its lines; the
/// library leaves it no separator and no comment character (0).
bool is_multiline_entry(const jText::v001::JTextEntry& e) {
    return e.level_sep == 0 && e.delimiter == 0;
}

std::string marker_of(const jText::v001::JTextEntry& e) {
    // A raw line has no separator and keeps ']' as its comment mark.
    if (e.level_sep == 0 && e.delimiter == U']') return "[]";
    if (e.level_sep == 0) return {};
    return jtext::encode_utf8(e.level_sep) + jtext::encode_utf8(e.delimiter == 0 ? U'\u258C' : e.delimiter);
}

std::string field_value(const jText::v001::JTextEntry& e, std::size_t i) {
    if (i >= e.fields.size() || !e.fields[i]) return {};
    return *e.fields[i];
}

bool null_token(const std::optional<std::string>& v) {
    return v && (*v == "NULL" || *v == "\\N" || *v == "\x1F");
}

std::string joined_value(const jText::v001::JTextEntry& e) {
    if (e.fields.empty()) return {};
    const std::string sep = jtext::encode_utf8(e.level_sep == 0 ? U'|' : e.level_sep);
    std::string out;
    for (std::size_t i = 0; i < e.fields.size(); ++i) {
        if (i) out += sep;
        if (e.fields[i]) out += *e.fields[i];
    }
    return out;
}

void blank_record(Record& cur, const std::string& marker) {
    cur = Record{};
    cur.marker = marker.empty() ? std::string(kDefaultMarker) : marker;
    cur.mark_all_absent();
}

/// A numbered line is one field, even when its marker splits the text
/// (category "Hand Tools/Hammers/Claw" with /#). A compact row is one line
/// that carries the whole record ("1. |# a|b|c") and is not followed by a
/// higher field number. A [] line is the field text unchanged, including a
/// literal NULL and trailing spaces.
void assign_one_field(Record& cur, const jText::v001::JTextEntry& e, const std::string& marker) {
    const std::size_t i = e.number - 1;
    cur.ensure_size(i + 1);
    if (marker != cur.marker) cur.field_markers[i] = marker;
    cur.has_line[i] = true;
    if (marker_is_raw(marker)) {
        cur.is_null[i] = false;
        cur.fields[i].clear();
        if (!e.fields.empty() && e.fields[0]) cur.fields[i] = *e.fields[0];
        return;
    }
    const bool missing = e.is_null || (e.fields.size() == 1 && null_token(e.fields[0]));
    if (missing) {
        cur.is_null[i] = true;
        cur.fields[i].clear();
        return;
    }
    cur.is_null[i] = false;
    cur.fields[i] = e.fields.size() > 1 ? joined_value(e) : field_value(e, 0);
}

void assign_compact(Record& cur, const jText::v001::JTextEntry& e) {
    const std::size_t first = e.number - 1;
    cur.ensure_size(std::min(first + e.fields.size(), kMaxFields));
    for (std::size_t fi = 0; fi < e.fields.size() && first + fi < kMaxFields; ++fi) {
        const std::size_t dest = first + fi;
        cur.has_line[dest] = true;
        if (null_token(e.fields[fi]) || !e.fields[fi]) {
            cur.is_null[dest] = true;
            cur.fields[dest].clear();
        } else {
            cur.is_null[dest] = false;
            cur.fields[dest] = *e.fields[fi];
        }
    }
}

/// One Fields line after its marker, e.g. "name/String/80/Not Null   # comment":
/// the name, then a type, an optional length (digits) and Nullable / Not Null.
ColumnDef parse_field_def(std::string rest) {
    const auto comment = rest.find('#');
    if (comment != std::string::npos) rest = rest.substr(0, comment);
    ColumnDef def;
    std::size_t start = 0;
    for (int part = 0; start <= rest.size(); ++part) {
        const auto slash = rest.find('/', start);
        const std::string word = trim(rest.substr(start, slash == std::string::npos ? std::string::npos : slash - start));
        if (part == 0) def.name = word;
        else if (word == "Not Null") def.not_null = true;
        else if (word == "Nullable") def.not_null = false;
        else if (!word.empty() && word.find_first_not_of("0123456789") == std::string::npos)
            def.length = std::stoi(word.substr(0, 6));
        else if (!word.empty() && def.jtext_type.empty()) def.jtext_type = word;
        if (slash == std::string::npos) break;
        start = slash + 1;
    }
    return def;
}

/// The Fields declarations of one section ("name/String/80/Not Null"), read by the library.
/// Only that section's: a file of dev, test and prod sections has three Fields blocks.
void collect_field_names(std::string_view path, std::string_view section,
                         std::vector<std::string>& names, std::vector<ColumnDef>* defs = nullptr) {
    std::ifstream in{std::string(path)};
    if (!in) return;
    const auto parsed = jtext::parse_file_structure(in);
    if (!parsed) return;
    for (const auto& sec : parsed->sections) {
        if (sec.name != section) continue;
        for (const auto& fl : sec.field_lines) {
            if (fl.hierarchy.empty() || fl.hierarchy[0].empty() || names.size() >= kMaxFields) continue;
            names.push_back(fl.hierarchy[0]);
            if (!defs) continue;
            std::string whole;
            for (std::size_t i = 0; i < fl.hierarchy.size(); ++i) whole += (i ? "/" : "") + fl.hierarchy[i];
            defs->push_back(parse_field_def(whole));   // same order as names
        }
        return;
    }
}

const char* dialect_label(SessionKind kind) {
    switch (kind) {
    case SessionKind::Sqlite:     return "SQLite";
    case SessionKind::MariaDB:    return "MariaDB";
    case SessionKind::PostgreSQL: return "PostgreSQL";
    case SessionKind::JTextFile:  return "jText";
    case SessionKind::None:       return "SQL";
    }
    return "SQL";
}

std::string quote_sql_ident(std::string_view name, SessionKind kind) {
    if (is_safe_identifier(name)) return std::string(name);
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

std::string table_sql_name(const TableShape& shape) {
    const std::string table = shape.table.empty() ? std::string("records") : shape.table;
    if (auto q = quote_table_ref(table, shape.dialect)) return *q;
    return quote_sql_ident(table, shape.dialect);
}

std::string constraint_word(bool not_null) { return not_null ? "Not Null" : "Nullable"; }

void write_field_line(std::ostream& out, std::size_t number, std::size_t pad, const ColumnDef& col,
                      bool force_not_null) {
    jText::v001::write_jtext_line_number(out, number, pad);
    out << "/\xE2\x96\x8C " << col.name << '/' << (col.jtext_type.empty() ? "String" : col.jtext_type);
    if (col.length > 0 && col.jtext_type == "String") out << '/' << col.length;
    out << '/' << constraint_word(force_not_null || col.not_null) << '\n';
}

void write_create_table(std::ostream& out, const TableShape& shape) {
    const auto& cols = shape.columns;
    std::size_t pk_count = 0;
    std::size_t name_width = 0;
    for (const auto& col : cols) {
        if (col.primary_key) ++pk_count;
        name_width = std::max(name_width, quote_sql_ident(col.name, shape.dialect).size());
    }
    out << "CREATE TABLE IF NOT EXISTS " << table_sql_name(shape) << " (\n";
    for (std::size_t i = 0; i < cols.size(); ++i) {
        const auto& col = cols[i];
        const std::string name = quote_sql_ident(col.name, shape.dialect);
        const std::string sql = col.sql_type.empty() ? std::string("TEXT") : col.sql_type;
        out << "    " << name << std::string(name_width - name.size(), ' ') << ' ' << sql;
        if (pk_count == 1 && col.primary_key) out << " PRIMARY KEY";
        else if (col.not_null) out << " NOT NULL";
        if (i + 1 < cols.size() || pk_count > 1) out << ',';
        out << '\n';
    }
    if (pk_count > 1) {
        out << "    PRIMARY KEY (";
        bool first = true;
        for (const auto& col : cols) {
            if (!col.primary_key) continue;
            if (!first) out << ", ";
            first = false;
            out << quote_sql_ident(col.name, shape.dialect);
        }
        out << ")\n";
    }
    out << ");\n";
}

void write_insert_template(std::ostream& out, const TableShape& shape, bool skip_id) {
    const std::string table = table_sql_name(shape);
    out << "INSERT INTO " << table << " (\n";
    bool first = true;
    for (std::size_t i = 0; i < shape.columns.size(); ++i) {
        if (skip_id && i == 0) continue;
        if (!first) out << ",\n";
        first = false;
        out << "    " << quote_sql_ident(shape.columns[i].name, shape.dialect);
    }
    out << "\n) VALUES (\n";
    first = true;
    for (std::size_t i = 0; i < shape.columns.size(); ++i) {
        if (skip_id && i == 0) continue;
        if (!first) out << ",\n";
        first = false;
        out << "    {" << (i + 1) << '}';
    }
    out << "\n);\n";
}

/// A multiline value's lines (SPEC v0.8 Part C): "** " and the line ("**" alone for an
/// empty one), then a bare "*]", each indented by `indent`.
void write_value_lines(std::ostream& out, std::string_view indent, std::string_view text) {
    for (;;) {
        const auto nl = text.find('\n');
        const auto line = text.substr(0, nl);
        out << indent << "**";
        if (!line.empty()) out << ' ' << line;
        out << '\n';
        if (nl == std::string_view::npos) break;
        text.remove_prefix(nl + 1);
    }
    out << indent << "*]\n";
}

/// A template's body is a multiline value with a bare "[*" opener.
void write_template(std::ostream& out, std::string_view title, std::string body) {
    if (!body.empty() && body.back() == '\n') body.pop_back();
    out << "=== Template: " << title << " ===\n[*\n";
    write_value_lines(out, "", body);
    out << '\n';
}

/// One field's line. A value holding a newline is a multiline value: "[*" on the
/// numbered line, its lines aligned under the "[*".
void write_marked_value(std::ostream& out, std::size_t number, std::size_t pad,
                        const std::string& marker, const std::string& value) {
    jText::v001::write_jtext_line_number(out, number, pad);
    if (value.find('\n') != std::string::npos) {
        out << "[*\n";
        write_value_lines(out, std::string(std::max(pad, std::to_string(number).size()) + 2, ' '), value);
        return;
    }
    out << live_marker(marker) << ' ' << value << '\n';
}

void write_data_rows(std::ostream& out, const std::vector<Record>& rows) {
    // Line numbers are padded to the widest record (at least kDefaultFields wide, so a
    // table of up to 10 fields keeps " 1." to "10.", as before).
    std::size_t widest = kDefaultFields;
    for (const auto& row : rows) widest = std::max(widest, row.size());
    const std::size_t pad = std::max(std::size_t{2},
                                     jText::v001::jtext_line_pad_width_for_count(widest));
    for (const auto& row : rows) {
        std::size_t last = 0;
        bool hole = false;
        for (std::size_t f = 0; f < row.size(); ++f)
            if (!row.is_null[f]) last = f + 1;
        for (std::size_t f = 0; f < last; ++f)
            if (row.is_null[f]) hole = true;
        if (last == 0) continue;
        const std::string record_mark = live_marker(row.marker);
        // A raw record has no splice, and a multiline value has its own lines, so
        // then each present field is its own line.
        const bool multiline = std::any_of(row.fields.begin(), row.fields.end(),
            [](const std::string& v) { return v.find('\n') != std::string::npos; });
        if (has_field_markers(row) || hole || marker_is_raw(record_mark) || multiline) {
            for (std::size_t f = 0; f < row.size(); ++f) {
                if (row.is_null[f]) continue;
                write_marked_value(out, f + 1, pad, field_marker(row, f), row.fields[f]);
            }
        } else {
            const std::string sep = marker_separator(record_mark);
            jText::v001::write_jtext_line_number(out, 1, pad);   // two columns, like every line (SPEC 3.7)
            out << record_mark << ' ';
            for (std::size_t f = 0; f < last; ++f) {
                if (f) out << sep;
                out << row.fields[f];
            }
            out << '\n';
        }
        out << '\n';
    }
}

/// Full profile. The // header is write_file_comment_header, not text pasted here.
std::expected<void, std::string> write_full_jtext(std::string_view path,
                                                  const std::vector<Record>& rows,
                                                  std::string_view table_label,
                                                  const TableShape& shape,
                                                  bool case_sensitive) {
    TableShape use = shape;
    if (use.table.empty()) use.table = std::string(table_label.empty() ? "records" : table_label);
    debug_log(std::format("save jtext {} columns={} dialect={}", path, use.columns.size(),
                          dialect_label(use.dialect)));

    std::ofstream out{std::string(path), std::ios::trunc};
    if (!out) return std::unexpected("cannot write " + std::string(path));

    const std::string related = std::format("type={} table={}", dialect_label(use.dialect), use.table);
    jText::v001::write_file_comment_header(out, path, "jText Data File", related);

    const auto today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    const std::string date = std::format("{:%Y-%m-%d}", today);
    const std::string filename = std::filesystem::path(std::string(path)).filename().string();
    const std::size_t pad = std::max(std::size_t{2},
                                     jText::v001::jtext_line_pad_width_for_count(
                                         std::max(use.columns.size(), std::size_t{1})));

    // Header lines (SPEC 2.3): filename, date, purpose, jtext_version, case, dialect, table.
    out << "=== jText File ===\n";
    const std::string header[] = {filename.empty() ? std::string(path) : filename, date,
                                  "Saved by jText_GUI", std::string(jtext::kFormatVersion),
                                  case_sensitive ? "Sensitive" : "Insensitive",
                                  std::string(dialect_label(use.dialect)), use.table};
    for (std::size_t i = 0; i < std::size(header); ++i) {
        jText::v001::write_jtext_line_number(out, i + 1, pad);
        out << kDefaultMarker << ' ' << header[i] << '\n';
    }
    out << '\n';

    out << "=== Section: " << use.table << " ===\n\n";

    std::ostringstream create;
    write_create_table(create, use);
    write_template(out, "Create Table", create.str());

    std::ostringstream insert;
    write_insert_template(insert, use, false);
    write_template(out, "SQL Insert", insert.str());

    if (use.columns.size() > 1) {
        std::ostringstream no_id;
        write_insert_template(no_id, use, true);
        write_template(out, "SQL Insert - no id", no_id.str());
    }

    out << "=== Fields ===\n";
    for (std::size_t i = 0; i < use.columns.size(); ++i) {
        const bool record_id = i == 0 && use.columns[i].jtext_type == "Integer";
        write_field_line(out, i + 1, pad, use.columns[i], record_id);
    }
    out << "=== End Fields ===\n\n";

    out << "=== Data ===\n";
    write_data_rows(out, rows);
    out << "=== End Data ===\n=== End Section ===\n=== End File ===\n";
    if (!out) return std::unexpected("cannot write " + std::string(path));
    return {};
}

/// The line's marker, as its two live characters ("#|#" loads as "|#"). Every line has one
/// (the library refuses a line without); a multiline value has none of its own and returns
/// empty: it belongs to its record.
std::string marker_for(const jText::v001::JTextEntry& e) {
    if (is_multiline_entry(e)) return {};
    return marker_of(e);
}

} // namespace

std::expected<JTextLoadResult, std::string> jtext_load(std::string_view path, std::string_view section) {
    jText::v001::JTextFile file;
    if (auto rd = file.read_full(path); !rd) return std::unexpected(rd.error());

    // The named section, or the one with the most records.
    const jText::v001::JTextSection* sec = nullptr;
    for (const auto& s : file.sections) {
        if (section.empty() ? (!sec || s.entries.size() > sec->entries.size()) : s.name == section) {
            sec = &s;
            if (!section.empty()) break;
        }
    }
    if (!sec && !section.empty())
        return std::unexpected(std::format("no section '{}' in {}", section, path));

    JTextLoadResult result;
    for (const auto& s : file.sections) result.sections.push_back(s.name);
    result.case_sensitive = file.case_mode == jText::v001::CaseMode::Sensitive;
    result.warnings = file.warnings;
    result.section_name = (sec && !sec->name.empty()) ? sec->name
                         : (!file.table_name.empty() ? file.table_name : "records");
    if (sec) collect_field_names(path, sec->name, result.field_names, &result.field_defs);

    if (!sec) {
        return std::unexpected("no records found in jText file (unsupported layout or empty)");
    }


    Record cur;
    bool open = false;
    int last_num = 0;
    bool after_multiline = false;   // the last line was a multiline value
    auto flush = [&] {
        if (!open) return;
        cur.original_key = cur.is_null[0] ? std::string{} : cur.fields[0];
        result.rows.push_back(std::move(cur));
        cur = Record{};
        open = false;
        last_num = 0;
        after_multiline = false;
    };

    const auto& entries = sec->entries;
    for (std::size_t ei = 0; ei < entries.size(); ++ei) {
        const auto& e = entries[ei];
        if (e.number < 1 || e.number > kMaxFields) continue;
        std::string marker = marker_for(e);
        if (marker.empty()) marker = open ? cur.marker : std::string(kDefaultMarker);

        std::size_t next_num = 0;
        for (std::size_t j = ei + 1; j < entries.size(); ++j) {
            if (entries[j].number < 1) continue;
            next_num = entries[j].number;
            break;
        }
        const bool next_continues = next_num > e.number;

        // A line with several values is a compact row (SPEC 2.6.3) when it is a whole
        // record, when the next line is the field after its last value, or when it
        // continues a record after a multiline field. Otherwise the values are the
        // levels of one field ("Hand Tools/Hammers/Claw").
        const bool multi = e.fields.size() > 1 && !e.is_null && !marker_is_raw(marker);
        const bool starts_new = !open || static_cast<int>(e.number) <= last_num;
        const bool compact = multi &&
            ((starts_new && !next_continues) ||
             (next_continues && next_num == e.number + e.fields.size()) ||
             (!starts_new && after_multiline));
        if (compact) {
            if (starts_new) {
                if (open) flush();
                blank_record(cur, marker);
                open = true;
            }
            assign_compact(cur, e);
            last_num = static_cast<int>(e.number + e.fields.size() - 1);
            after_multiline = false;
            continue;
        }

        if (open && static_cast<int>(e.number) <= last_num) flush();
        if (!open) {
            blank_record(cur, marker);
            open = true;
        }
        assign_one_field(cur, e, marker);
        last_num = static_cast<int>(e.number);
        after_multiline = is_multiline_entry(e);
    }
    flush();

    if (result.rows.empty())
        return std::unexpected("no records found in jText file (unsupported layout or empty)");
    ensure_record_ids(result.rows);
    return result;
}

namespace {

std::string lower_type(std::string_view in) {
    std::string out;
    bool space = false;
    for (unsigned char c : in) {
        if (std::isspace(c)) {
            if (!out.empty()) space = true;
            continue;
        }
        if (space) {
            out.push_back(' ');
            space = false;
        }
        out.push_back(static_cast<char>(std::tolower(c)));
    }
    return out;
}

bool strip_tail(std::string& text, std::string_view tail) {
    if (text.size() < tail.size() || !text.ends_with(tail)) return false;
    text.resize(text.size() - tail.size());
    while (!text.empty() && text.back() == ' ') text.pop_back();
    return true;
}

int first_int(std::string_view args) {
    long n = 0;
    bool any = false;
    for (unsigned char c : args) {
        if (std::isdigit(c)) {
            any = true;
            n = n * 10 + (c - '0');
            if (n > 10000000) return 0;
        } else if (any) {
            break;
        }
    }
    return any ? static_cast<int>(n) : 0;
}

bool integer_head(std::string_view head) {
    return head == "int" || head == "integer" || head == "int2" || head == "int4" || head == "int8" ||
           head == "smallint" || head == "bigint" || head == "tinyint" || head == "mediumint" ||
           head == "serial" || head == "bigserial" || head == "smallserial" || head == "year";
}

std::string integer_sql(std::string_view head, SessionKind kind) {
    if (head == "bigint" || head == "int8" || head == "bigserial") return "BIGINT";
    if (head == "smallint" || head == "int2" || head == "smallserial") return "SMALLINT";
    if (head == "mediumint") return kind == SessionKind::MariaDB ? "MEDIUMINT" : "INTEGER";
    if (head == "tinyint") return kind == SessionKind::MariaDB ? "TINYINT" : "INTEGER";
    if (head == "year") return "YEAR";
    if (kind == SessionKind::MariaDB &&
        (head == "int" || head == "integer" || head == "int4" || head == "serial"))
        return "INT";
    return "INTEGER";
}

} // namespace

ColumnDef column_from_declared(std::string name, std::string_view declared_type, bool not_null,
                               bool primary_key, SessionKind kind) {
    ColumnDef col;
    col.name = std::move(name);
    col.not_null = not_null;
    col.primary_key = primary_key;

    std::string text = lower_type(declared_type);
    const bool unsign = strip_tail(text, "unsigned");
    strip_tail(text, "zerofill");

    std::string head = text;
    std::string args;
    const auto paren = text.find('(');
    if (paren != std::string::npos) {
        head = text.substr(0, paren);
        while (!head.empty() && head.back() == ' ') head.pop_back();
        const auto end = text.find(')', paren);
        args = text.substr(paren, end == std::string::npos ? std::string::npos : end - paren + 1);
    }

    auto finish_integer = [&](std::string sql) {
        if (unsign && kind == SessionKind::MariaDB) sql += " UNSIGNED";
        col.sql_type = std::move(sql);
        col.jtext_type = "Integer";
    };

    if (head == "tinyint" && args == "(1)") {
        col.sql_type = "BOOLEAN";
        col.jtext_type = "Bool";
        return col;
    }
    if (head == "bool" || head == "boolean") {
        col.sql_type = "BOOLEAN";
        col.jtext_type = "Bool";
        return col;
    }
    if (integer_head(head)) {
        finish_integer(integer_sql(head, kind));
        return col;
    }
    if (head == "numeric" || head == "decimal" || head == "number") {
        col.sql_type = (head == "decimal" ? "DECIMAL" : "NUMERIC") + args;
        col.jtext_type = "Number";
        return col;
    }
    if (head == "real" || head == "float4") {
        col.sql_type = "REAL";
        col.jtext_type = "Number";
        return col;
    }
    if (head == "float") {
        col.sql_type = "FLOAT";
        col.jtext_type = "Number";
        return col;
    }
    if (head == "double" || head == "double precision" || head == "float8") {
        col.sql_type = "DOUBLE PRECISION";
        col.jtext_type = "Number";
        return col;
    }
    if (head == "date") {
        col.sql_type = "DATE";
        col.jtext_type = "Date";
        return col;
    }
    if (head == "datetime" || head.starts_with("datetime") || head == "timestamptz" ||
        head.starts_with("timestamp")) {
        if (head.find("without time zone") != std::string::npos) col.sql_type = "TIMESTAMP";
        else if (head.find("with time zone") != std::string::npos || head == "timestamptz")
            col.sql_type = "TIMESTAMPTZ";
        else if (head == "datetime" || head.starts_with("datetime"))
            col.sql_type = kind == SessionKind::MariaDB && !args.empty() ? "DATETIME" + args
                                                                         : "DATETIME";
        else col.sql_type = "TIMESTAMP";
        col.jtext_type = "DateTime";
        return col;
    }
    if (head == "time" || head.starts_with("time ")) {
        col.sql_type = "TIME";
        col.jtext_type = "String";
        return col;
    }

    const int length = first_int(args);
    if (head == "character varying" || head == "varchar" || head == "nvarchar" || head == "varchar2") {
        col.sql_type = length > 0 ? std::format("VARCHAR({})", length) : "VARCHAR";
        col.jtext_type = "String";
        col.length = length;
        return col;
    }
    if (head == "character" || head == "char" || head == "nchar" || head == "bpchar") {
        col.sql_type = length > 0 ? std::format("CHAR({})", length) : "CHAR";
        col.jtext_type = "String";
        col.length = length;
        return col;
    }
    if (head == "text" || head == "tinytext" || head == "mediumtext" || head == "longtext" ||
        head == "clob" || head == "string" || head.empty()) {
        col.sql_type = "TEXT";
        col.jtext_type = "String";
        return col;
    }
    if (head == "json" || head == "jsonb") {
        col.sql_type = head == "jsonb" ? "JSONB" : "JSON";
        col.jtext_type = "String";
        return col;
    }
    if (head == "uuid") {
        col.sql_type = "UUID";
        col.jtext_type = "String";
        return col;
    }
    if (head == "bytea") {
        col.sql_type = "BYTEA";
        col.jtext_type = "String";
        return col;
    }
    if (head == "blob" || head == "tinyblob" || head == "mediumblob" || head == "longblob") {
        col.sql_type = "BLOB";
        col.jtext_type = "String";
        return col;
    }

    std::string sql;
    sql.reserve(head.size());
    for (unsigned char c : head) sql.push_back(static_cast<char>(std::toupper(c)));
    sql += args;
    if (sql.empty()) sql = "TEXT";
    col.sql_type = std::move(sql);
    col.jtext_type = "String";
    return col;
}

void adopt_columns(DbQueryResult& out, std::vector<ColumnDef> catalog, SessionKind kind) {
    if (catalog.empty()) {
        for (const auto& name : out.column_names)
            out.columns.push_back(column_from_declared(name, "text", false, false, kind));
        return;
    }
    if (catalog.size() == out.column_names.size()) {
        bool same = true;
        for (std::size_t i = 0; i < catalog.size(); ++i)
            if (catalog[i].name != out.column_names[i]) same = false;
        if (same) {
            out.columns = std::move(catalog);
            return;
        }
    }
    std::vector<ColumnDef> aligned;
    aligned.reserve(std::max(catalog.size(), out.column_names.size()));
    for (const auto& name : out.column_names) {
        const auto it = std::find_if(catalog.begin(), catalog.end(),
                                     [&](const ColumnDef& col) { return col.name == name; });
        if (it != catalog.end()) aligned.push_back(*it);
        else aligned.push_back(column_from_declared(name, "text", false, false, kind));
    }
    for (const auto& col : catalog) {
        bool have = false;
        for (const auto& got : aligned)
            if (got.name == col.name) have = true;
        if (!have) aligned.push_back(col);
    }
    out.columns = std::move(aligned);
    debug_log("columns: catalog names differed from the result; templates follow the result order");
}

namespace {

/// Light profile. Data lines use the live two-character marker.
std::expected<void, std::string> write_light_jtext(std::string_view path,
                                                   const std::vector<Record>& rows,
                                                   std::string_view table_label,
                                                   bool case_sensitive) {
    std::ofstream out{std::string(path), std::ios::trunc};
    if (!out) return std::unexpected("cannot write " + std::string(path));
    const std::string table = table_label.empty() ? std::string("records") : std::string(table_label);
    jText::v001::write_file_comment_header(out, path, "Saved by jText_GUI");
    const auto today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    const std::string date = std::format("{:%Y-%m-%d}", today);
    jText::v001::write_jtext_hash_header(out, date, "Saved by jText_GUI",
                                         case_sensitive ? jText::v001::CaseMode::Sensitive
                                                        : jText::v001::CaseMode::Insensitive,
                                         table);
    out << '\n';
    jText::v001::write_light_section_banner(out, table);
    write_data_rows(out, rows);
    out << "-- End " << table << " --\n";   // every block closes, like XML
    out << "-- EOF --\n";
    if (!out) return std::unexpected("cannot write " + std::string(path));
    return {};
}

} // namespace

std::expected<void, std::string> jtext_save_section(std::string_view source_path,
                                                    std::string_view out_path,
                                                    std::string_view section,
                                                    const std::vector<Record>& rows) {
    for (std::size_t i = 0; i < rows.size(); ++i) {
        if (!record_id_of(rows[i]))
            return std::unexpected(std::format(
                "record {} is missing an integer record id in field 1", i + 1));
    }
    std::ifstream in{std::string(source_path), std::ios::binary};
    if (!in) return std::unexpected("cannot read " + std::string(source_path));
    const std::string text{std::istreambuf_iterator<char>(in), {}};
    in.close();

    std::ostringstream data;
    write_data_rows(data, rows);
    auto replaced = jtext::replace_section_data(text, section, data.str());
    if (!replaced)
        return std::unexpected(std::format("cannot save section '{}' of {}: {}", section, source_path,
                                           replaced.error().message));

    // Written beside the file, then renamed over it: a failed write leaves the file as it was.
    const std::string tmp = std::string(out_path) + ".saving";
    {
        std::ofstream out{tmp, std::ios::binary | std::ios::trunc};
        out << *replaced;
        if (!out) return std::unexpected("cannot write " + tmp);
    }
    std::error_code ec;
    std::filesystem::rename(tmp, std::string(out_path), ec);
    if (ec) return std::unexpected("cannot replace " + std::string(out_path) + ": " + ec.message());
    return {};
}

std::expected<void, std::string> jtext_save(std::string_view path,
                                            const std::vector<Record>& rows,
                                            std::string_view table_label,
                                            const TableShape* shape,
                                            bool case_sensitive) {
    for (std::size_t i = 0; i < rows.size(); ++i) {
        if (!record_id_of(rows[i]))
            return std::unexpected(std::format(
                "record {} is missing an integer record id in field 1", i + 1));
    }
    if (shape && !shape->columns.empty())
        return write_full_jtext(path, rows, table_label, *shape, case_sensitive);
    return write_light_jtext(path, rows, table_label, case_sensitive);
}

} // namespace jtext_gui
