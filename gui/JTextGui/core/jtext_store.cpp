#include "jtext_gui/core/table_store.hpp"

#include "jText.h"

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

std::string marker_of(const jText::v001::JTextEntry& e) {
    // parse_entry clears level_sep on a raw line and leaves the comment mark as ']'.
    if (e.level_sep == 0 && e.delimiter == ']') return "[]";
    if (e.level_sep == 0) return {};
    std::string m;
    m.push_back(e.level_sep);
    m.push_back(e.delimiter == 0 ? '#' : e.delimiter);
    return m;
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
    const char sep = e.level_sep == 0 ? '|' : e.level_sep;
    std::string out;
    for (std::size_t i = 0; i < e.fields.size(); ++i) {
        if (i) out.push_back(sep);
        if (e.fields[i]) out += *e.fields[i];
    }
    return out;
}

void blank_record(Record& cur, const std::string& marker) {
    cur = Record{};
    cur.marker = marker.empty() ? std::string(kDefaultMarker) : marker;
    cur.is_null.fill(true);
}

/// A numbered line is one field, even when its marker splits the text
/// (category "Hand Tools/Hammers/Claw" with /#). A compact row is one line
/// that carries the whole record ("1. |# a|b|c") and is not followed by a
/// higher field number. A [] line is the field text unchanged, including a
/// literal NULL and trailing spaces.
void assign_one_field(Record& cur, const jText::v001::JTextEntry& e, const std::string& marker) {
    const std::size_t i = e.number - 1;
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

void collect_field_names(std::string_view path, std::vector<std::string>& names) {
    std::ifstream in{std::string(path)};
    if (!in) return;
    bool in_fields = false;
    for (std::string line; std::getline(in, line);) {
        const std::string t = trim(line);
        if (t == "=== Fields ===") { in_fields = true; continue; }
        if (t == "=== End Fields ===") { in_fields = false; continue; }
        if (!in_fields) continue;
        const auto dot = t.find('.');
        if (dot == std::string::npos) continue;
        auto rest = trim(t.substr(dot + 1));
        const MarkerRun mark = take_marker(rest);
        if (mark.text != "/#") continue;
        rest = trim(rest.substr(mark.consumed));
        const auto slash = rest.find('/');
        if (slash != std::string::npos) rest = trim(rest.substr(0, slash));
        const auto comment = rest.find('#');
        if (comment != std::string::npos) rest = trim(rest.substr(0, comment));
        if (!rest.empty() && names.size() < kMaxFields) names.push_back(rest);
    }
}

/// One data line. has_marker is false when the line names no marker.
/// The marker stored here is the live two characters. A [] line also keeps
/// the field text, because the library trims the line before parse_entry.
struct SourceMark {
    bool has_marker = false;
    bool raw = false;
    std::string marker;
    std::string raw_value;
};

std::optional<SourceMark> source_mark_of(std::string_view line) {
    while (!line.empty() && (line.front() == ' ' || line.front() == '\t'))
        line.remove_prefix(1);
    if (line.empty()) return std::nullopt;
    std::size_t num = 0;
    const auto [ptr, ec] = std::from_chars(line.data(), line.data() + line.size(), num);
    if (ec != std::errc{} || ptr >= line.data() + line.size() || *ptr != '.')
        return std::nullopt;
    if (ptr + 1 >= line.data() + line.size() || ptr[1] != ' ')
        return std::nullopt;
    SourceMark mark;
    const std::string_view rest(ptr + 2);
    const MarkerRun run = take_marker(rest);
    if (!run.text.empty()) {
        mark.has_marker = true;
        mark.marker = run.text;
        if (marker_is_raw(run.text)) {
            mark.raw = true;
            std::string_view content = rest.substr(run.consumed);
            while (!content.empty() && (content.front() == ' ' || content.front() == '\t'))
                content.remove_prefix(1);
            mark.raw_value.assign(content);
        }
    }
    return mark;
}

bool plain_header(std::string_view line) {
    return line.starts_with("JText File - created ") || line.starts_with("Purpose - ") ||
           line.starts_with("AutoID:") || line.starts_with("Case:") ||
           line.starts_with("SQL Dialect:") || line.starts_with("Table Name:");
}

bool section_header(std::string_view line) {
    return line == "Field List" || line == "Data Section" ||
           (line.starts_with("-- ") && line.ends_with(" --"));
}

std::optional<std::string> spec_interior(std::string_view line) {
    if (line.size() < 8 || !line.starts_with("===") || !line.ends_with("==="))
        return std::nullopt;
    return trim(std::string(line.substr(3, line.size() - 6)));
}

/// Markers parallel to JTextFile sections and their entries.
/// The walk follows parse_stream. Data lines are read untrimmed so a []
/// value keeps its trailing spaces.
std::vector<std::vector<SourceMark>> scan_source_marks(std::string_view path) {
    std::vector<std::vector<SourceMark>> sections;
    std::ifstream in{std::string(path)};
    if (!in) return sections;

    bool in_header = true;
    bool file_has_spec_markers = false;
    bool in_fields_block = false;
    bool in_data_block = false;
    std::string cur_section;
    std::vector<SourceMark>* active = nullptr;

    for (std::string raw; std::getline(in, raw);) {
        if (!raw.empty() && raw.back() == '\r') raw.pop_back();
        const std::string line = trim(raw);
        if (line.empty()) continue;

        if (auto interior = spec_interior(line)) {
            const auto& marker = *interior;
            if (marker == "jText File") continue;
            if (marker == "End File" || marker == "End Section" || marker == "End Data" ||
                marker == "End Fields") {
                if (marker == "End Fields") in_fields_block = false;
                if (marker == "End Data") in_data_block = false;
                continue;
            }
            if (marker.starts_with("Section: ")) {
                file_has_spec_markers = true;
                in_header = false;
                in_fields_block = false;
                in_data_block = false;
                cur_section = trim(marker.substr(std::string_view("Section: ").size()));
                sections.emplace_back();
                active = &sections.back();
                continue;
            }
            if (marker == "Fields") {
                in_fields_block = true;
                in_data_block = false;
                continue;
            }
            if (marker == "Data") {
                file_has_spec_markers = true;
                in_fields_block = false;
                in_data_block = true;
                if (!active && !cur_section.empty()) {
                    sections.emplace_back();
                    active = &sections.back();
                }
                continue;
            }
            continue;
        }

        if (in_header && line.starts_with('#')) continue;
        if (in_header && line.starts_with("//")) continue;
        if (in_header) {
            if (plain_header(line)) continue;
            if (section_header(line)) in_header = false;
        }
        if (in_fields_block) continue;
        if (line.starts_with("# Fields:")) continue;

        if (section_header(line)) {
            cur_section = (line.starts_with("-- "))
                              ? trim(line.substr(3, line.size() - 6))
                              : trim(line);
            sections.emplace_back();
            active = &sections.back();
            continue;
        }
        if (line == "-- EOF --") break;

        if (line.ends_with("*** ^^^ ###")) {
            if (!active) {
                sections.emplace_back();
                active = &sections.back();
            }
            std::string skip;
            std::getline(in, skip);
            std::getline(in, skip);
            std::getline(in, skip);
            active->push_back(SourceMark{});
            continue;
        }

        const bool parse_data_row = active && (!file_has_spec_markers || in_data_block);
        if (!parse_data_row) continue;
        if (auto mark = source_mark_of(raw))
            active->push_back(*mark);
    }
    return sections;
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
    out << "/# " << col.name << '/' << (col.jtext_type.empty() ? "String" : col.jtext_type);
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

void write_template(std::ostream& out, std::string_view title, std::string_view sentinel,
                    const std::string& body) {
    out << "=== Template: " << title << " ===\n";
    out << "<<< " << sentinel << "\n";
    out << body;
    if (body.empty() || body.back() != '\n') out << '\n';
    out << sentinel << "\n\n";
}

void write_marked_value(std::ostream& out, std::size_t number, std::size_t pad,
                        const std::string& marker, const std::string& value) {
    jText::v001::write_jtext_line_number(out, number, pad);
    out << live_marker(marker) << ' ' << value << '\n';
}

void write_data_rows(std::ostream& out, const std::vector<Record>& rows) {
    const std::size_t pad = std::max(std::size_t{2},
                                     jText::v001::jtext_line_pad_width_for_count(kMaxFields));
    for (const auto& row : rows) {
        std::size_t last = 0;
        bool hole = false;
        for (std::size_t f = 0; f < kMaxFields; ++f)
            if (!row.is_null[f]) last = f + 1;
        for (std::size_t f = 0; f < last; ++f)
            if (row.is_null[f]) hole = true;
        if (last == 0) continue;
        const std::string record_mark = live_marker(row.marker);
        // A raw record has no splice, so each present field is its own line.
        if (has_field_markers(row) || hole || marker_is_raw(record_mark)) {
            for (std::size_t f = 0; f < kMaxFields; ++f) {
                if (row.is_null[f]) continue;
                write_marked_value(out, f + 1, pad, field_marker(row, f), row.fields[f]);
            }
        } else {
            const char sep = marker_separator(record_mark);
            jText::v001::write_jtext_line_number(out, 1, 0);
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
                                                  const TableShape& shape) {
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

    out << "=== jText File ===\n";
    jText::v001::write_jtext_line_number(out, 1, pad);
    out << "?# " << (filename.empty() ? std::string(path) : filename) << '\n';
    jText::v001::write_jtext_line_number(out, 2, pad);
    out << "?# " << date << '\n';
    jText::v001::write_jtext_line_number(out, 3, pad);
    out << "?# Saved by jText_GUI\n";
    jText::v001::write_jtext_line_number(out, 4, pad);
    out << "?# 0.1\n";
    jText::v001::write_jtext_line_number(out, 5, pad);
    out << "?# Sensitive\n";
    jText::v001::write_jtext_line_number(out, 6, pad);
    out << "?# " << dialect_label(use.dialect) << '\n';
    jText::v001::write_jtext_line_number(out, 7, pad);
    out << "?# " << use.table << '\n';
    out << '\n';

    out << "=== Section: " << use.table << " ===\n\n";

    std::ostringstream create;
    write_create_table(create, use);
    write_template(out, "Create Table", "!!!CT!!!", create.str());

    std::ostringstream insert;
    write_insert_template(insert, use, false);
    write_template(out, "SQL Insert", "!!!INS!!!", insert.str());

    if (use.columns.size() > 1) {
        std::ostringstream no_id;
        write_insert_template(no_id, use, true);
        write_template(out, "SQL Insert - no id", "!!!INS2!!!", no_id.str());
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

/// Source marker when the scan lines up with the parsed section. Otherwise the
/// parser's marker. A line with no marker gets "|#".
std::string marker_for(const jText::v001::JTextEntry& e,
                       const std::vector<SourceMark>* marks,
                       std::size_t index,
                       bool& filled) {
    if (marks && index < marks->size()) {
        const SourceMark& src = (*marks)[index];
        if (src.has_marker) return src.marker;
        filled = true;
        return kDefaultMarker;
    }
    if (std::string parsed = marker_of(e); !parsed.empty()) return parsed;
    filled = true;
    return kDefaultMarker;
}

} // namespace

std::expected<JTextLoadResult, std::string> jtext_load(std::string_view path) {
    jText::v001::JTextFile file;
    if (auto rd = file.read_full(path); !rd) return std::unexpected(rd.error());

    const jText::v001::JTextSection* sec = nullptr;
    for (const auto& s : file.sections) {
        if (!sec || s.entries.size() > sec->entries.size()) sec = &s;
    }

    JTextLoadResult result;
    result.section_name = (sec && !sec->name.empty()) ? sec->name
                         : (!file.table_name.empty() ? file.table_name : "records");
    collect_field_names(path, result.field_names);

    if (!sec) {
        return std::unexpected("no records found in jText file (unsupported layout or empty)");
    }

    const auto scanned = scan_source_marks(path);
    std::size_t sec_index = 0;
    for (std::size_t i = 0; i < file.sections.size(); ++i) {
        if (&file.sections[i] == sec) sec_index = i;
    }
    const std::vector<SourceMark>* marks = nullptr;
    if (sec_index < scanned.size() && scanned[sec_index].size() == sec->entries.size())
        marks = &scanned[sec_index];
    bool filled = false;

    Record cur;
    bool open = false;
    int last_num = 0;
    auto flush = [&] {
        if (!open) return;
        cur.original_key = cur.is_null[0] ? std::string{} : cur.fields[0];
        result.rows.push_back(std::move(cur));
        cur = Record{};
        open = false;
        last_num = 0;
    };

    const auto& entries = sec->entries;
    for (std::size_t ei = 0; ei < entries.size(); ++ei) {
        const auto& e = entries[ei];
        if (e.is_special_block) continue;
        if (e.number < 1 || e.number > kMaxFields) continue;
        const std::string marker = marker_for(e, marks, ei, filled);

        bool next_continues = false;
        for (std::size_t j = ei + 1; j < entries.size(); ++j) {
            if (entries[j].is_special_block) continue;
            if (entries[j].number < 1) continue;
            next_continues = entries[j].number > e.number;
            break;
        }

        const bool multi = e.fields.size() > 1 && !e.is_null;
        const bool starts_new = !open || static_cast<int>(e.number) <= last_num;
        if (multi && starts_new && !next_continues) {
            if (open) flush();
            blank_record(cur, marker);
            open = true;
            assign_compact(cur, e);
            flush();
            continue;
        }

        if (open && static_cast<int>(e.number) <= last_num) flush();
        if (!open) {
            blank_record(cur, marker);
            open = true;
        }
        assign_one_field(cur, e, marker);
        if (marks && ei < marks->size() && (*marks)[ei].raw) {
            const std::size_t i = static_cast<std::size_t>(e.number - 1);
            if (i < kMaxFields) {
                cur.is_null[i] = false;
                cur.has_line[i] = true;
                cur.fields[i] = (*marks)[ei].raw_value;
            }
        }
        last_num = static_cast<int>(e.number);
    }
    flush();

    if (result.rows.empty())
        return std::unexpected("no records found in jText file (unsupported layout or empty)");
    ensure_record_ids(result.rows);
    result.filled_default_markers = filled;
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
                                                   std::string_view table_label) {
    std::ofstream out{std::string(path), std::ios::trunc};
    if (!out) return std::unexpected("cannot write " + std::string(path));
    const std::string table = table_label.empty() ? std::string("records") : std::string(table_label);
    jText::v001::write_file_comment_header(out, path, "Saved by jText_GUI");
    const auto today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    const std::string date = std::format("{:%Y-%m-%d}", today);
    jText::v001::write_jtext_hash_header(out, date, "Saved by jText_GUI",
                                         jText::v001::CaseMode::Sensitive, table);
    out << '\n';
    jText::v001::write_light_section_banner(out, table);
    write_data_rows(out, rows);
    out << "-- EOF --\n";
    if (!out) return std::unexpected("cannot write " + std::string(path));
    return {};
}

} // namespace

std::expected<void, std::string> jtext_save(std::string_view path,
                                            const std::vector<Record>& rows,
                                            std::string_view table_label,
                                            const TableShape* shape) {
    for (std::size_t i = 0; i < rows.size(); ++i) {
        if (!record_id_of(rows[i]))
            return std::unexpected(std::format(
                "record {} is missing an integer record id in field 1", i + 1));
    }
    if (shape && !shape->columns.empty())
        return write_full_jtext(path, rows, table_label, *shape);
    return write_light_jtext(path, rows, table_label);
}

} // namespace jtext_gui
