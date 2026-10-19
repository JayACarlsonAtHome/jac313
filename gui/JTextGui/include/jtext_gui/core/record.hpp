#pragma once

#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace jtext_gui {

/// Max fields shown in the simple editor panel (v001 fixed width).
inline constexpr std::size_t kMaxFields = 10;

/// Marker written when a line has none. Splice `|`, comment `#`.
inline constexpr const char* kDefaultMarker = "|#";

/// Printable ASCII that is not a letter or a digit. Same test as the jText parser.
inline bool is_marker_char(char c) {
    const auto u = static_cast<unsigned char>(c);
    if (u < 0x21 || u > 0x7E) return false;
    if ((u >= '0' && u <= '9') || (u >= 'A' && u <= 'Z') || (u >= 'a' && u <= 'z'))
        return false;
    return true;
}

/// A marker run at the start of text. The live marker is the right two characters.
/// A longer opener (`#|#`, `^|#`) drops what is in front. Equal right-two characters
/// are not a marker.
struct MarkerRun {
    std::string text;
    std::size_t consumed = 0;
};

inline MarkerRun take_marker(std::string_view text) {
    std::size_t end = 0;
    while (end < text.size() && is_marker_char(text[end])) ++end;
    MarkerRun run;
    if (end >= 2 && text[end - 2] != text[end - 1]) {
        run.text.assign(text.data() + (end - 2), 2);
        run.consumed = end;
    }
    return run;
}

/// Live two-character marker. Empty or unusable text becomes "|#".
inline std::string live_marker(std::string_view text) {
    std::size_t i = 0;
    while (i < text.size() && (text[i] == ' ' || text[i] == '\t')) ++i;
    if (const MarkerRun run = take_marker(text.substr(i)); !run.text.empty()) return run.text;
    return kDefaultMarker;
}

/// `[]` names no splice and no comment. The rest of the field is data as written.
inline bool marker_is_raw(std::string_view marker) {
    const MarkerRun run = take_marker(marker);
    return run.text.size() == 2 && run.text[0] == '[' && run.text[1] == ']';
}

/// One logical table row (in-memory). Orthogonal to UI and storage backends.
struct Record {
    /// Live marker: splice, then comment. Default "|#". "[]" is raw data.
    std::string marker = kDefaultMarker;
    std::array<std::string, kMaxFields> fields{};
    /// True when field i is SQL NULL / a missing jText line. False and an empty
    /// fields[i] is an empty string (the line was present and blank).
    std::array<bool, kMaxFields> is_null{};
    /// True when a source line exists for field i (including a blank line or an
    /// explicit NULL). Absent fields are is_null and not has_line.
    std::array<bool, kMaxFields> has_line{};
    /// Each field's own marker, e.g. "^#" ("" = the record's marker). A record with
    /// any of these set is saved as per-field lines "N. ^# value", one per field.
    std::array<std::string, kMaxFields> field_markers{};
    /// Original primary-key value at load (empty if new / unknown). Used for UPDATE.
    std::string original_key;
};

/// Splice character, or 0 when the marker is raw (do not split the field).
inline char marker_separator(std::string_view marker) {
    const MarkerRun run = take_marker(marker.empty() ? std::string_view{kDefaultMarker} : marker);
    if (run.text.size() != 2 || (run.text[0] == '[' && run.text[1] == ']')) return 0;
    return run.text[0];
}

/// The marker field i is written with: its own, else the record's.
inline const std::string& field_marker(const Record& r, std::size_t i) {
    return (i < kMaxFields && !r.field_markers[i].empty()) ? r.field_markers[i] : r.marker;
}

/// True when some field has a marker of its own (the record must be saved per field).
inline bool has_field_markers(const Record& r) {
    for (std::size_t i = 0; i < kMaxFields; ++i)
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
    if (r.is_null[0] || r.fields[0].empty()) return std::nullopt;
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
        const bool missing = r.is_null[0] || !r.has_line[0] || r.fields[0].empty();
        if (!missing) {
            ++fix.not_integer;
            continue;
        }
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
    char sep = marker_separator(r.marker);
    if (sep == 0) sep = ' ';
    std::string out;
    const std::size_t n = std::min(max_fields, kMaxFields);
    for (std::size_t i = 0; i < n; ++i) {
        if (i > 0) out += sep;
        out += r.fields[i];
    }
    if (kMaxFields > max_fields) out += sep + std::string("...");
    return out;
}

} // namespace jtext_gui
