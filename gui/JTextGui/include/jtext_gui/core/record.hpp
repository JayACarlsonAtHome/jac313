#pragma once

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <format>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "parser/utf8.hpp"

namespace jtext_gui {

/// Most fields (lines) a record can have: field positions 1-99 (SPEC 3.7). Line numbers
/// are two columns wide; a 100th field would make them three on every line of the file,
/// one more character per line that adds up in long tables, and a record that wide is no
/// longer something a person reads. A record grows to what its file or table needs, up to this.
inline constexpr std::size_t kMaxFields = 99;

/// Why a query or sheet with more columns than a record can hold is refused, rather
/// than cut short.
inline std::string too_many_fields(std::size_t columns) {
    return std::format("the result has {} columns; a jText record holds at most {} fields", columns,
                       kMaxFields);
}

/// Fields a new record starts with (the editor shows at least this many lines).
inline constexpr std::size_t kDefaultFields = 10;

/// Marker written when a line has none: the library writer's default (SPEC v0.8 Part E).
/// Splice U+25AE, comment U+258C.
inline constexpr const char* kDefaultMarker = "\xE2\x96\xAE\xE2\x96\x8C";

/// A marker run at the start of text. The live marker is the right two characters
/// (code points, not bytes). A longer opener (`#|#`, `^|#`) drops what is in front.
/// Equal right-two characters are not a marker. A marker character is one code point
/// that is Punctuation or Symbol, the jText parser's own test (SPEC 3.1).
struct MarkerRun {
    std::string text;
    std::size_t consumed = 0;
};

inline MarkerRun take_marker(std::string_view text) {
    std::size_t end = 0, prev = 0, last = 0, count = 0;   // last: start of the last character
    while (end < text.size()) {
        const auto cp = jtext::decode_utf8(text.substr(end));
        if (!cp || !jtext::is_punctuation_or_symbol(cp->cp)) break;
        prev = last;
        last = end;
        end += cp->len;
        ++count;
    }
    MarkerRun run;
    if (count >= 2 && text.substr(prev, last - prev) != text.substr(last, end - last)) {
        run.text.assign(text.substr(prev, end - prev));
        run.consumed = end;
    }
    return run;
}

/// The splice of a two-character marker, as UTF-8 (its first character).
inline std::string_view marker_splice(std::string_view marker) {
    const auto cp = jtext::decode_utf8(marker);
    return cp ? marker.substr(0, cp->len) : std::string_view{};
}

/// Live two-character marker. Empty or unusable text becomes kDefaultMarker.
inline std::string live_marker(std::string_view text) {
    std::size_t i = 0;
    while (i < text.size() && (text[i] == ' ' || text[i] == '\t')) ++i;
    if (const MarkerRun run = take_marker(text.substr(i)); !run.text.empty()) return run.text;
    return kDefaultMarker;
}

/// `[]` names no splice and no comment. The rest of the field is data as written.
inline bool marker_is_raw(std::string_view marker) {
    return take_marker(marker).text == "[]";
}

/// One logical table row (in-memory). Orthogonal to UI and storage backends.
/// The four per-field lists always have the same length, size(): kDefaultFields to
/// start, grown by ensure_size() when a file or table has more (up to kMaxFields).
/// A field at or past size() is absent (NULL, no line); read it with field_is_null(),
/// field_text() and field_has_line(), which allow that.
struct Record {
    /// Live marker: splice, then comment. Default kDefaultMarker. "[]" is raw data.
    std::string marker = kDefaultMarker;
    std::vector<std::string> fields = std::vector<std::string>(kDefaultFields);
    /// True when field i is SQL NULL / a missing jText line. False and an empty
    /// fields[i] is an empty string (the line was present and blank).
    std::vector<bool> is_null = std::vector<bool>(kDefaultFields, false);
    /// True when a source line exists for field i (including a blank line or an
    /// explicit NULL). Absent fields are is_null and not has_line.
    std::vector<bool> has_line = std::vector<bool>(kDefaultFields, false);
    /// Each field's own marker, e.g. "^#" ("" = the record's marker). A record with
    /// any of these set is saved as per-field lines "N. ^# value", one per field.
    std::vector<std::string> field_markers = std::vector<std::string>(kDefaultFields);
    /// Original primary-key value at load (empty if new / unknown). Used for UPDATE.
    std::string original_key;
    /// Comments (SPEC 3.2.1), by the field their line starts at (0-based): the comment on
    /// that line, and the comment lines standing before it. Saving puts each back.
    std::map<std::size_t, std::string> line_comments;
    std::map<std::size_t, std::vector<std::string>> comment_lines;

    /// Fields every list covers. (A caller may brace-initialize fields alone, e.g.
    /// Record{"|#", {"1", "a"}}; the shortest list is what may be indexed.)
    [[nodiscard]] std::size_t size() const {
        return std::min({fields.size(), is_null.size(), has_line.size(), field_markers.size()});
    }

    /// Grow each list to n entries (at most kMaxFields); new ones are absent (NULL, no
    /// line). Never shrinks a list.
    void ensure_size(std::size_t n) {
        n = std::min(n, kMaxFields);
        if (fields.size() < n) fields.resize(n);
        if (is_null.size() < n) is_null.resize(n, true);
        if (has_line.size() < n) has_line.resize(n, false);
        if (field_markers.size() < n) field_markers.resize(n);
    }

    /// Every field absent (NULL, no line), as a loader starts a record.
    void mark_all_absent() { std::fill(is_null.begin(), is_null.end(), true); }
};

/// Field i is NULL or absent (past the end of the record counts as absent).
inline bool field_is_null(const Record& r, std::size_t i) {
    return i >= r.size() || r.is_null[i];
}

/// Field i's text; empty when it is past the end of the record.
inline const std::string& field_text(const Record& r, std::size_t i) {
    static const std::string none;
    return i < r.size() ? r.fields[i] : none;
}

/// A source line exists for field i.
inline bool field_has_line(const Record& r, std::size_t i) {
    return i < r.size() && r.has_line[i];
}


/// Splice character as UTF-8, or empty when the marker is raw (do not split the field).
inline std::string marker_separator(std::string_view marker) {
    const MarkerRun run = take_marker(marker.empty() ? std::string_view{kDefaultMarker} : marker);
    if (run.text.empty() || run.text == "[]") return {};
    return std::string(marker_splice(run.text));
}

/// The marker field i is written with: its own, else the record's.
inline const std::string& field_marker(const Record& r, std::size_t i) {
    return (i < r.field_markers.size() && !r.field_markers[i].empty()) ? r.field_markers[i] : r.marker;
}

/// True when some field has a marker of its own (the record must be saved per field).
inline bool has_field_markers(const Record& r) {
    for (std::size_t i = 0; i < r.field_markers.size(); ++i)
        if (!r.field_markers[i].empty() && r.field_markers[i] != r.marker) return true;
    return false;
}

/// Field 1 is the record id (SPEC 2.6.4): an integer, optional leading '+'.
inline std::optional<std::int64_t> parse_record_id(std::string_view value) {
    if (!value.empty() && value.front() == '+') value.remove_prefix(1);
    if (value.empty()) return std::nullopt;
    std::int64_t id = 0;
    const auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), id);
    if (ec != std::errc{} || p != value.data() + value.size()) return std::nullopt;
    return id;
}

inline std::optional<std::int64_t> record_id_of(const Record& r) {
    if (field_is_null(r, 0) || field_text(r, 0).empty()) return std::nullopt;
    return parse_record_id(r.fields[0]);
}

/// How many record ids were generated, and how many field-1 values are not integers.
struct RecordIdFix {
    int assigned = 0;
    int not_integer = 0;
};

/// A missing or blank field 1 gets the next integer (one past the largest id, or 1).
/// A field 1 that is present and not an integer is left as written.
inline RecordIdFix ensure_record_ids(std::vector<Record>& rows) {
    RecordIdFix fix;
    std::int64_t max_id = 0;
    bool any = false;
    for (const auto& r : rows) {
        if (auto id = record_id_of(r)) {
            if (!any || *id > max_id) max_id = *id;
            any = true;
        }
    }
    std::int64_t next = any ? max_id + 1 : 1;
    for (auto& r : rows) {
        if (record_id_of(r)) continue;
        const bool missing = field_is_null(r, 0) || !field_has_line(r, 0) || field_text(r, 0).empty();
        if (!missing) {
            ++fix.not_integer;
            continue;
        }
        r.ensure_size(1);
        r.is_null[0] = false;
        r.has_line[0] = true;
        r.fields[0] = std::to_string(next++);
        if (r.original_key.empty()) r.original_key = r.fields[0];
        ++fix.assigned;
    }
    return fix;
}

/// Join first n non-trailing-empty fields for display (does not trim meaning).
inline std::string record_preview(const Record& r, std::size_t max_fields = 4) {
    std::string sep = marker_separator(r.marker);
    if (sep.empty()) sep = " ";
    std::string out;
    const std::size_t n = std::min(max_fields, r.size());
    for (std::size_t i = 0; i < n; ++i) {
        if (i > 0) out += sep;
        out += r.fields[i];
    }
    if (r.size() > max_fields) out += sep + std::string("...");
    return out;
}

} // namespace jtext_gui
