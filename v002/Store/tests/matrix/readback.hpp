#pragma once

// readback.hpp — read a persisted dataset back and prove it matches the in-memory store.
//
// Every backend (binary, jtext, html, json, sql = SQLite, pg-local, pg-remote) is read back
// and compared with Store::persisted_event(id) for the ids the sink was given: ids [0, cap),
// cap = Store::expected_persisted_rows() (the run's events, capped by max_persist_rows), and of
// those only the ones its PersistMode takes. A KeeperOnly sink must hold exactly the ids whose
// flags carry KeeperRecord (bit 1), a DatabaseOnly sink exactly those with DatabaseEntry (bit 2),
// and nothing else; an All sink holds every id. `expected` in the READBACK line is that count.
//
// Modes (--readback=...):
//   off     skip
//   count   row counts only (main + _ints + _floats)
//   sample  count + checksum + row comparison. Files are parsed whole, so every row is
//           compared. Databases: checksum = SUM(id), SUM(thread_id), SUM(per_thread_event_id)
//           computed by the database; sample = the first 100, last 100 and 100 evenly spaced
//           ids, fetched and compared in full.
//   all     count + checksum + every row compared (databases stream every row).
//
// Doubles must match exactly in every backend: the text formats write the shortest exact
// round-trip text (append_double), NaN and +/-Infinity included (JSON as strings). The one
// difference accepted is SQLite's, which the format forces (see docs/store/persistence.md, Compromises):
// it stores a NaN as NULL, read back as the NaN that was stored, and a -0.0 as 0.
//
// The result is printed as one line the test runner parses:
//   READBACK backend=sql mode=sample status=PASS expected=100 found=100 count=PASS
//            checksum=PASS sample=PASS checked=100 read_ms=1.234 [subset=keeper|database]
//            [pg timings...] [error="..."]
// subset= appears only for a KeeperOnly / DatabaseOnly sink (the production bench run).

#include "test_common.hpp"
#include "parser/section.hpp"

#include <jac313/Store/v002/headers/persistence/BinaryEventLogReader.hpp>
#include <jac313/Store/v002/headers/persistence/JsonEventLogReader.hpp>

#include <jac313/Store/v002/headers/persistence/PersistCommon.hpp>

#include <algorithm>
#include <bit>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#ifdef JAC313_STORE_HAS_SQL_PERSIST
#include <jac313/Store/v002/headers/persistence/Sqlite.hpp>
#endif

namespace jac313::Store::v002::matrix_test {

struct ReadbackReport {
    std::string backend;
    std::string mode;
    std::string subset;     // "" (every id) | keeper | database: the PersistMode the sink ran in
    bool ok = true;
    size_t expected = 0;
    size_t found = 0;
    size_t checked = 0;
    std::string count = "-";
    std::string checksum = "-";
    std::string sample = "-";
    std::string error;
    double read_ms = 0.0;
};

namespace readback_detail {

// FNV-1a 64 over a canonical row text; rows are summed (order-independent).
inline uint64_t fnv1a(std::string_view s) {
    uint64_t h = 1469598103934665603ULL;
    for (unsigned char c : s) { h ^= c; h *= 1099511628211ULL; }
    return h;
}

// Doubles hash as their exact text, the same for every backend. `sqlite`: -0.0 hashes as 0,
// which is what SQLite holds for it (doubles_equal).
inline uint64_t row_hash(const PersistedEvent& e, bool sqlite) {
    std::string s = std::format("{}|{}|{}|{}|{}|{}|{}", e.event_id, e.thread_id, e.per_thread_event_id,
                                e.flags, e.category, e.payload, e.timestamp_us);
    for (auto v : e.int_metrics) s += std::format("|i{}", v);
    for (auto v : e.dbl_metrics) {
        s += "|d";
        append_double(s, sqlite && v == 0.0 ? 0.0 : v);
    }
    return fnv1a(s);
}

// Exact: the same bits, or NaN for NaN. `sqlite`: also 0 for -0.0, the sign SQLite drops (it
// stores a whole REAL as an integer). The NaN SQLite stores as NULL arrives here as NaN
// already (db_fetch).
inline bool doubles_equal(double want, double got, bool sqlite) {
    if (std::isnan(want) || std::isnan(got)) return std::isnan(want) && std::isnan(got);
    if (sqlite && want == 0.0) return got == 0.0;
    return std::bit_cast<uint64_t>(want) == std::bit_cast<uint64_t>(got);
}

// Compare one stored row with the expected row; returns "" or a description of the first mismatch.
inline std::string compare_row(const PersistedEvent& want, const PersistedEvent& got, bool sqlite) {
    auto bad = [&](std::string_view what, const auto& w, const auto& g) {
        return std::format("id {}: {} expected '{}' got '{}'", want.event_id, what, w, g);
    };
    if (got.event_id != want.event_id)               return bad("id", want.event_id, got.event_id);
    if (got.thread_id != want.thread_id)             return bad("thread_id", want.thread_id, got.thread_id);
    if (got.per_thread_event_id != want.per_thread_event_id)
        return bad("per_thread_event_id", want.per_thread_event_id, got.per_thread_event_id);
    if (got.flags != want.flags)                     return bad("flags", want.flags, got.flags);
    if (got.category != want.category)               return bad("category", want.category, got.category);
    if (got.payload != want.payload)                 return bad("payload", want.payload, got.payload);
    if (got.timestamp_us != want.timestamp_us)       return bad("timestamp_us", want.timestamp_us, got.timestamp_us);
    if (got.int_metrics.size() != want.int_metrics.size())
        return bad("int metric count", want.int_metrics.size(), got.int_metrics.size());
    for (size_t k = 0; k < want.int_metrics.size(); ++k)
        if (got.int_metrics[k] != want.int_metrics[k])
            return bad(std::format("int{}", k), want.int_metrics[k], got.int_metrics[k]);
    if (got.dbl_metrics.size() != want.dbl_metrics.size())
        return bad("double metric count", want.dbl_metrics.size(), got.dbl_metrics.size());
    for (size_t k = 0; k < want.dbl_metrics.size(); ++k)
        if (!doubles_equal(want.dbl_metrics[k], got.dbl_metrics[k], sqlite))
            return bad(std::format("dbl{}", k), want.dbl_metrics[k], got.dbl_metrics[k]);
    return {};
}

// ---- small parsers ----------------------------------------------------------------------

inline bool parse_u64(std::string_view s, uint64_t& out) {
    int base = 10;
    if (s.starts_with("0x") || s.starts_with("0X")) { s.remove_prefix(2); base = 16; }
    auto [p, ec] = std::from_chars(s.data(), s.data() + s.size(), out, base);
    return !s.empty() && ec == std::errc{} && p == s.data() + s.size();
}

inline bool parse_i64(std::string_view s, int64_t& out) {
    auto [p, ec] = std::from_chars(s.data(), s.data() + s.size(), out);
    return !s.empty() && ec == std::errc{} && p == s.data() + s.size();
}

inline std::vector<std::string_view> split(std::string_view s, std::string_view sep) {
    std::vector<std::string_view> out;
    size_t pos = 0;
    while (true) {
        const auto at = s.find(sep, pos);
        if (at == std::string_view::npos) { out.push_back(s.substr(pos)); break; }
        out.push_back(s.substr(pos, at - pos));
        pos = at + sep.size();
    }
    return out;
}

inline std::string html_unescape(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '&') {
            auto rest = s.substr(i);
            if (rest.starts_with("&amp;"))  { out += '&';  i += 4; continue; }
            if (rest.starts_with("&lt;"))   { out += '<';  i += 3; continue; }
            if (rest.starts_with("&gt;"))   { out += '>';  i += 3; continue; }
            if (rest.starts_with("&quot;")) { out += '"';  i += 5; continue; }
            if (rest.starts_with("&#39;"))  { out += '\''; i += 4; continue; }
        }
        out += s[i];
    }
    return out;
}

// Rows keyed by id; metrics from the _ints/_floats companions are joined in by id.
struct RowSet {
    std::vector<PersistedEvent> rows;
    std::string error;
};

// Join companion metric rows (id + values) into main rows by id.
inline bool join_metrics(RowSet& rs,
                         const std::vector<std::pair<uint64_t, std::vector<int64_t>>>& ints,
                         const std::vector<std::pair<uint64_t, std::vector<double>>>& dbls) {
    std::vector<PersistedEvent*> by_id;
    for (auto& r : rs.rows) {
        if (r.event_id >= by_id.size()) by_id.resize(r.event_id + 1, nullptr);
        by_id[r.event_id] = &r;
    }
    for (const auto& [id, v] : ints) {
        if (id >= by_id.size() || !by_id[id]) { rs.error = std::format("_ints row for unknown id {}", id); return false; }
        by_id[id]->int_metrics = v;
    }
    for (const auto& [id, v] : dbls) {
        if (id >= by_id.size() || !by_id[id]) { rs.error = std::format("_floats row for unknown id {}", id); return false; }
        by_id[id]->dbl_metrics = v;
    }
    return true;
}

inline RowSet load_binary(const std::string& base) {
    RowSet rs;
    try {
        BinaryEventLogReader rd(base + ".bin");
        BinaryRecord r;
        while (rd.next(r)) {
            PersistedEvent e;
            e.event_id = r.event_id; e.thread_id = r.thread_id; e.per_thread_event_id = r.per_thread_event_id;
            e.flags = r.raw_flags; e.category = r.category; e.payload = r.payload; e.timestamp_us = r.timestamp_us;
            e.int_metrics = r.int_metrics; e.dbl_metrics = r.dbl_metrics;
            rs.rows.push_back(std::move(e));
        }
    } catch (const std::exception& ex) { rs.error = ex.what(); }
    return rs;
}

inline RowSet load_json(const std::string& base, size_t ic, size_t dc) {
    RowSet rs;
    try {
        {
            JsonEventLogReader rd(base + ".json");
            JsonRecord r;
            while (rd.next(r)) {
                PersistedEvent e;
                e.event_id = r.event_id; e.thread_id = r.thread_id; e.per_thread_event_id = r.per_thread_event_id;
                e.flags = r.raw_flags; e.category = r.category; e.payload = r.payload; e.timestamp_us = r.timestamp_us;
                rs.rows.push_back(std::move(e));
            }
        }
        std::vector<std::pair<uint64_t, std::vector<int64_t>>> ints;
        std::vector<std::pair<uint64_t, std::vector<double>>> dbls;
        if (ic > 0) {
            JsonEventLogReader rd(base + "_Ints.json");
            JsonRecord r;
            while (rd.next(r)) ints.emplace_back(r.event_id, r.int_metrics);
        }
        if (dc > 0) {
            JsonEventLogReader rd(base + "_Floats.json");
            JsonRecord r;
            while (rd.next(r)) dbls.emplace_back(r.event_id, r.dbl_metrics);
        }
        join_metrics(rs, ints, dbls);
    } catch (const std::exception& ex) { rs.error = ex.what(); }
    return rs;
}

// HTML: one "<tr><td>..</td>...</tr>" per row.
inline std::vector<std::vector<std::string>> html_rows(const std::string& path, std::string& err) {
    std::vector<std::vector<std::string>> out;
    std::ifstream in(path);
    if (!in) { err = "cannot open " + path; return out; }
    for (std::string line; std::getline(in, line);) {
        std::string_view l = line;
        if (!l.starts_with("<tr><td>") || !l.ends_with("</td></tr>")) continue;
        l.remove_prefix(8);
        l.remove_suffix(10);
        std::vector<std::string> cells;
        for (auto c : split(l, "</td><td>")) cells.push_back(html_unescape(c));
        out.push_back(std::move(cells));
    }
    return out;
}

inline RowSet load_html(const std::string& base, size_t ic, size_t dc) {
    RowSet rs;
    auto main = html_rows(base + ".html", rs.error);
    if (!rs.error.empty()) return rs;
    for (const auto& c : main) {
        PersistedEvent e;
        if (c.size() != 7 || !parse_u64(c[0], e.event_id) || !parse_u64(c[1], e.thread_id) ||
            !parse_u64(c[2], e.per_thread_event_id) || !parse_u64(c[3], e.flags) || !parse_u64(c[6], e.timestamp_us)) {
            rs.error = std::format("bad main row ({} cells)", c.size());
            return rs;
        }
        e.category = c[4];
        e.payload = c[5];
        rs.rows.push_back(std::move(e));
    }
    std::vector<std::pair<uint64_t, std::vector<int64_t>>> ints;
    std::vector<std::pair<uint64_t, std::vector<double>>> dbls;
    if (ic > 0) {
        for (const auto& c : html_rows(base + "_Ints.html", rs.error)) {
            uint64_t id = 0; std::vector<int64_t> v(c.size() > 0 ? c.size() - 1 : 0);
            if (c.empty() || !parse_u64(c[0], id)) { rs.error = "bad _ints row"; return rs; }
            for (size_t k = 1; k < c.size(); ++k) if (!parse_i64(c[k], v[k - 1])) { rs.error = "bad int"; return rs; }
            ints.emplace_back(id, std::move(v));
        }
    }
    if (dc > 0) {
        for (const auto& c : html_rows(base + "_Floats.html", rs.error)) {
            uint64_t id = 0; std::vector<double> v(c.size() > 0 ? c.size() - 1 : 0);
            if (c.empty() || !parse_u64(c[0], id)) { rs.error = "bad _floats row"; return rs; }
            for (size_t k = 1; k < c.size(); ++k) if (!parse_double_text(c[k], v[k - 1])) { rs.error = "bad double"; return rs; }
            dbls.emplace_back(id, std::move(v));
        }
    }
    if (rs.error.empty()) join_metrics(rs, ints, dbls);
    return rs;
}

// jText: the records of the file's (only) section, read by the jText library's parser, so
// the file must be valid jText, closers and all. A record is the lines between blank lines;
// each line carries the values from its field position on (a compact row, or one multiline
// value), so a record split around a multiline field reassembles.
inline std::vector<std::vector<std::string>> jtext_rows(const std::string& path, std::string& text,
                                                        std::string& err) {
    std::vector<std::vector<std::string>> out;
    std::ifstream in(path, std::ios::binary);
    if (!in) { err = "cannot open " + path; return out; }
    text.assign(std::istreambuf_iterator<char>(in), {});
    const auto file = jtext::parse_file_structure(std::string_view(text));
    if (!file) { err = std::format("{}: line {}: {}", path, file.error().line_no, file.error().message); return out; }
    if (file->sections.size() != 1) { err = std::format("{}: {} sections, expected 1", path, file->sections.size()); return out; }
    std::vector<std::string> record;
    for (const auto& pl : file->sections[0].data_lines) {
        // A blank line ends the record, and a line for field 1 starts the next: rows
        // that each hold a whole record follow one another directly (SPEC 2.6).
        if (pl.number == 0 || (pl.number == 1 && !record.empty())) {
            if (!record.empty()) out.push_back(std::move(record));
            record.clear();
            if (pl.number == 0) continue;
        }
        const size_t first = pl.number - 1;
        if (record.size() < first + pl.values.size()) record.resize(first + pl.values.size());
        for (size_t k = 0; k < pl.values.size(); ++k) record[first + k] = pl.values[k];
    }
    if (!record.empty()) out.push_back(std::move(record));
    return out;
}

inline RowSet load_jtext(const std::string& base, size_t ic, size_t dc) {
    RowSet rs;
    std::string text_main, text_i, text_f;
    auto main = jtext_rows(base + ".jtext", text_main, rs.error);
    if (!rs.error.empty()) return rs;
    for (const auto& c : main) {
        PersistedEvent e;
        if (c.size() != 7 || !parse_u64(c[0], e.event_id) || !parse_u64(c[1], e.thread_id) ||
            !parse_u64(c[2], e.per_thread_event_id) || !parse_u64(c[3], e.flags) || !parse_u64(c[6], e.timestamp_us)) {
            rs.error = std::format("bad main row ({} fields)", c.size());
            return rs;
        }
        e.category = std::string(c[4]);
        e.payload = std::string(c[5]);
        rs.rows.push_back(std::move(e));
    }
    std::vector<std::pair<uint64_t, std::vector<int64_t>>> ints;
    std::vector<std::pair<uint64_t, std::vector<double>>> dbls;
    if (ic > 0) {
        for (const auto& c : jtext_rows(base + "_Ints.jtext", text_i, rs.error)) {
            uint64_t id = 0; std::vector<int64_t> v(c.size() - 1);
            if (!parse_u64(c[0], id)) { rs.error = "bad _ints row"; return rs; }
            for (size_t k = 1; k < c.size(); ++k) if (!parse_i64(c[k], v[k - 1])) { rs.error = "bad int"; return rs; }
            ints.emplace_back(id, std::move(v));
        }
    }
    if (dc > 0) {
        for (const auto& c : jtext_rows(base + "_Floats.jtext", text_f, rs.error)) {
            uint64_t id = 0; std::vector<double> v(c.size() - 1);
            if (!parse_u64(c[0], id)) { rs.error = "bad _floats row"; return rs; }
            for (size_t k = 1; k < c.size(); ++k) if (!parse_double_text(c[k], v[k - 1])) { rs.error = "bad double"; return rs; }
            dbls.emplace_back(id, std::move(v));
        }
    }
    if (rs.error.empty()) join_metrics(rs, ints, dbls);
    return rs;
}

// ---- databases: a tiny uniform query interface over SQLite and PostgreSQL ----------------

using DbRows = std::vector<std::vector<std::optional<std::string>>>;

struct DbQuery {
    virtual ~DbQuery() = default;
    virtual DbRows query(const std::string& sql) = 0;
    std::string main_table, ints_table, dbls_table;
    bool sqlite = false;   // SQLite's NULL-for-NaN and -0.0-as-0 apply (db_fetch, doubles_equal)
};

#ifdef JAC313_STORE_HAS_SQL_PERSIST
struct SqliteQuery : DbQuery {
    Sqlite db;
    explicit SqliteQuery(const std::string& base) : db(base + ".db") {
        main_table = std::filesystem::path(base).filename().string();
        if (main_table.empty()) main_table = "persist";
        ints_table = main_table + "_ints";
        dbls_table = main_table + "_floats";
        sqlite = true;
    }
    DbRows query(const std::string& sql) override {
        DbRows out;
        auto st = db.prepare(sql);
        const int n = st.column_count();
        while (st.step()) {
            std::vector<std::optional<std::string>> row;
            for (int c = 0; c < n; ++c) {
                const std::string name = st.column_name(c);
                if (name.starts_with("dbl")) {
                    // REAL as a double, formatted exactly (SQLite's own text conversion keeps
                    // only 15 digits, which would fail an exact comparison).
                    auto d = st.get_by_name<std::optional<double>>(name);
                    if (!d) { row.emplace_back(std::nullopt); continue; }
                    std::string text;
                    append_double(text, *d);
                    row.push_back(std::move(text));
                } else {
                    row.push_back(st.get_by_name<std::optional<std::string>>(name));
                }
            }
            out.push_back(std::move(row));
        }
        return out;
    }
};
#endif

#ifdef JAC313_STORE_HAS_PG_PERSIST
struct PgQuery : DbQuery {
    PgConn conn;
    PgQuery(std::string_view backend, const std::string& base) : conn(pg_conninfo_for(backend)) {
        const std::string t = std::string(kPgTestSchema) + "." + pg_table_base(base);
        main_table = t;
        ints_table = t + "_ints";
        dbls_table = t + "_floats";
    }
    DbRows query(const std::string& sql) override { return conn.query(sql); }
};
#endif

inline uint64_t to_u64(const std::optional<std::string>& v) {
    if (!v) return 0;
    int64_t s = 0;
    if (parse_i64(*v, s)) return static_cast<uint64_t>(s);   // stored as signed BIGINT
    uint64_t u = 0;
    parse_u64(*v, u);
    return u;
}

// Fetch rows (main + metrics) for `where` ("" = all), ordered by id. Stored data is never
// NULL: a NULL cell is an error, with the one exception below (SQLite's NaN).
inline RowSet db_fetch(DbQuery& q, size_t ic, size_t dc, const std::string& where) {
    RowSet rs;
    auto has_null = [&](const std::vector<std::optional<std::string>>& r, std::string_view table) {
        for (const auto& c : r) if (!c) { rs.error = std::format("NULL cell in {} (id {})", table, r[0].value_or("?")); return true; }
        return false;
    };
    for (auto& r : q.query(std::format("SELECT id, thread_id, per_thread_event_id, flags_raw, category, payload, "
                                       "timestamp_us FROM {} {} ORDER BY id", q.main_table, where))) {
        if (has_null(r, q.main_table)) return rs;
        PersistedEvent e;
        e.event_id = to_u64(r[0]); e.thread_id = to_u64(r[1]); e.per_thread_event_id = to_u64(r[2]);
        e.flags = to_u64(r[3]); e.category = r[4].value_or(""); e.payload = r[5].value_or("");
        e.timestamp_us = to_u64(r[6]);
        rs.rows.push_back(std::move(e));
    }
    std::vector<std::pair<uint64_t, std::vector<int64_t>>> ints;
    std::vector<std::pair<uint64_t, std::vector<double>>> dbls;
    if (ic > 0) {
        std::string cols = "id";
        for (size_t k = 0; k < ic; ++k) cols += std::format(", int{}", k);
        for (auto& r : q.query(std::format("SELECT {} FROM {} {} ORDER BY id", cols, q.ints_table, where))) {
            if (has_null(r, q.ints_table)) return rs;
            std::vector<int64_t> v(ic);
            for (size_t k = 0; k < ic; ++k)
                if (!parse_i64(*r[k + 1], v[k])) { rs.error = std::format("bad int{} '{}'", k, *r[k + 1]); return rs; }
            ints.emplace_back(to_u64(r[0]), std::move(v));
        }
    }
    if (dc > 0) {
        std::string cols = "id";
        for (size_t k = 0; k < dc; ++k) cols += std::format(", dbl{}", k);
        for (auto& r : q.query(std::format("SELECT {} FROM {} {} ORDER BY id", cols, q.dbls_table, where))) {
            if (!r[0]) { rs.error = "NULL id in " + q.dbls_table; return rs; }
            std::vector<double> v(dc);
            for (size_t k = 0; k < dc; ++k) {
                if (!r[k + 1]) {
                    // The owner's compromise, the one NULL this project allows: SQLite stores a
                    // NaN as NULL by itself, so a NULL double read from SQLite is the NaN that
                    // was stored (compare_row then holds it to a NaN in the store). Any other
                    // NULL, and every NULL from PostgreSQL, is an error.
                    if (!q.sqlite) { rs.error = std::format("NULL dbl{} in {} (id {})", k, q.dbls_table, *r[0]); return rs; }
                    v[k] = std::numeric_limits<double>::quiet_NaN();
                } else if (!parse_double_text(*r[k + 1], v[k])) {
                    rs.error = std::format("bad dbl{} '{}'", k, *r[k + 1]);
                    return rs;
                }
            }
            dbls.emplace_back(to_u64(r[0]), std::move(v));
        }
    }
    join_metrics(rs, ints, dbls);
    return rs;
}

inline size_t db_count(DbQuery& q, const std::string& table) {
    auto r = q.query("SELECT COUNT(*) FROM " + table);
    return r.empty() ? 0 : static_cast<size_t>(to_u64(r[0][0]));
}

} // namespace readback_detail

/// Read back `backend` at `base` and compare with `store`. `subset` is the PersistMode the sink
/// ran in: All checks ids [0, cap); KeeperOnly / DatabaseOnly check only the ids whose flags carry
/// KeeperRecord / DatabaseEntry, and the dataset must hold nothing else. Prints nothing (see
/// print_readback).
template<typename Store>
ReadbackReport run_readback(const Store& store, const std::string& backend, const std::string& base,
                            size_t ic, size_t dc, const std::string& mode, const std::string& pg_cleanup = "keep-last",
                            PersistMode subset = PersistMode::All) {
    using namespace readback_detail;
    ReadbackReport rep;
    rep.backend = backend;
    rep.mode = mode;
    rep.subset = subset == PersistMode::KeeperOnly ? "keeper" : subset == PersistMode::DatabaseOnly ? "database" : "";
    const auto t0 = std::chrono::steady_clock::now();
    // The ids this sink was given, ascending: [0, cap), filtered by the sink's PersistMode with the
    // same bit test the sinks use (bit 1 KeeperRecord, bit 2 DatabaseEntry).
    const size_t cap = store.expected_persisted_rows();
    std::vector<size_t> want_ids;
    {
        const uint64_t bit = subset == PersistMode::KeeperOnly ? (1ULL << 1)
                           : subset == PersistMode::DatabaseOnly ? (1ULL << 2) : 0;
        want_ids.reserve(bit ? cap / 4 : cap);
        for (size_t id = 0; id < cap; ++id)
            if (bit == 0 || (store.persisted_event(id).flags & bit) != 0) want_ids.push_back(id);
    }
    rep.expected = want_ids.size();
    const bool sqlite = backend == "sql";
    const bool is_db = backend == "sql" || backend == "pg-local" || backend == "pg-remote";

    auto fail = [&](std::string& field, std::string msg) {
        field = "FAIL";
        rep.ok = false;
        if (rep.error.empty()) rep.error = std::move(msg);
    };

    // Expected checksum and sums over the wanted ids.
    uint64_t want_hash = 0, want_sum_id = 0, want_sum_tid = 0, want_sum_seq = 0;
    if (mode != "count") {
        for (const size_t id : want_ids) {
            const auto e = store.persisted_event(id);
            want_hash += row_hash(e, sqlite);
            want_sum_id += id;
            want_sum_tid += e.thread_id;
            want_sum_seq += e.per_thread_event_id;
        }
    }
    // wanted[id]: the sink should hold row `id` (ids [0, cap)).
    std::vector<bool> wanted(cap, false);
    for (const size_t id : want_ids) wanted[id] = true;

    // Compare a full RowSet (files, or a database in "all" mode).
    auto check_full = [&](RowSet&& rs) {
        if (!rs.error.empty()) { fail(rep.count, rs.error); return; }
        rep.found = rs.rows.size();
        // count: exactly the wanted ids, each once, and no other id
        std::vector<const PersistedEvent*> by_id(cap, nullptr);
        bool ids_ok = rs.rows.size() == rep.expected;
        size_t unwanted = 0;
        for (const auto& r : rs.rows) {
            if (r.event_id >= cap || !wanted[r.event_id]) { ids_ok = false; ++unwanted; continue; }
            if (by_id[r.event_id]) { ids_ok = false; continue; }
            by_id[r.event_id] = &r;
        }
        if (ids_ok) rep.count = "PASS";
        else if (unwanted) fail(rep.count, std::format("{} rows the sink should not hold (expected {} rows, found {})",
                                                       unwanted, rep.expected, rep.found));
        else fail(rep.count, std::format("expected {} ids once each, found {} rows", rep.expected, rep.found));
        if (mode == "count") return;
        uint64_t got_hash = 0;
        for (const auto& r : rs.rows) got_hash += row_hash(r, sqlite);
        if (got_hash == want_hash) rep.checksum = "PASS";
        else fail(rep.checksum, "row checksum differs");
        rep.sample = "PASS";
        for (const size_t id : want_ids) {
            if (!by_id[id]) continue;   // already counted as a count failure
            ++rep.checked;
            if (auto d = compare_row(store.persisted_event(id), *by_id[id], sqlite); !d.empty()) {
                fail(rep.sample, d);
                break;
            }
        }
    };

    try {
        if (backend == "binary")      check_full(load_binary(base));
        else if (backend == "json")   check_full(load_json(base, ic, dc));
        else if (backend == "html")   check_full(load_html(base, ic, dc));
        else if (backend == "jtext")  check_full(load_jtext(base, ic, dc));
        else if (is_db) {
            std::unique_ptr<DbQuery> q;
#ifdef JAC313_STORE_HAS_SQL_PERSIST
            if (backend == "sql") q = std::make_unique<SqliteQuery>(base);
#endif
#ifdef JAC313_STORE_HAS_PG_PERSIST
            if (backend != "sql") q = std::make_unique<PgQuery>(backend, base);
#endif
            if (!q) throw std::runtime_error(backend + " persistence not built in");

            if (mode == "all") {
                check_full(db_fetch(*q, ic, dc, ""));
            } else {
                // count
                rep.found = db_count(*q, q->main_table);
                bool counts_ok = rep.found == rep.expected;
                if (ic > 0) counts_ok = counts_ok && db_count(*q, q->ints_table) == rep.expected;
                if (dc > 0) counts_ok = counts_ok && db_count(*q, q->dbls_table) == rep.expected;
                if (counts_ok) rep.count = "PASS";
                else fail(rep.count, std::format("expected {} rows, found {} (or a companion table differs)",
                                                 rep.expected, rep.found));
                if (mode == "sample") {
                    // checksum: id/thread/sequence sums computed by the database
                    auto s = q->query(std::format("SELECT SUM(id), SUM(thread_id), SUM(per_thread_event_id) FROM {}",
                                                  q->main_table));
                    const size_t n = want_ids.size();
                    const bool sums_ok = !s.empty() && to_u64(s[0][0]) == want_sum_id &&
                                         to_u64(s[0][1]) == want_sum_tid && to_u64(s[0][2]) == want_sum_seq;
                    if (sums_ok) rep.checksum = "PASS";
                    else fail(rep.checksum, "SUM(id/thread_id/per_thread_event_id) differs");
                    // sample: the first 100, last 100 and 100 evenly spaced of the wanted ids
                    std::vector<size_t> ids;
                    for (size_t i = 0; i < std::min<size_t>(100, n); ++i) ids.push_back(want_ids[i]);
                    for (size_t i = n > 100 ? n - 100 : 0; i < n; ++i) ids.push_back(want_ids[i]);
                    for (size_t i = 0; i < 100 && n > 0; ++i) ids.push_back(want_ids[i * n / 100]);
                    std::sort(ids.begin(), ids.end());
                    ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
                    std::string in;
                    for (auto id : ids) in += (in.empty() ? "" : ",") + std::to_string(id);
                    auto rs = ids.empty() ? RowSet{} : db_fetch(*q, ic, dc, "WHERE id IN (" + in + ")");
                    rep.sample = "PASS";
                    if (rs.rows.size() != ids.size()) {
                        fail(rep.sample, std::format("sample: asked {} ids, got {}", ids.size(), rs.rows.size()));
                    } else {
                        for (size_t k = 0; k < ids.size(); ++k) {
                            ++rep.checked;
                            if (auto d = compare_row(store.persisted_event(ids[k]), rs.rows[k], sqlite); !d.empty()) {
                                fail(rep.sample, d);
                                break;
                            }
                        }
                    }
                }
            }
#ifdef JAC313_STORE_HAS_PG_PERSIST
            if (backend != "sql" && pg_cleanup == "delete") {
                auto* pq = static_cast<PgQuery*>(q.get());
                pq->conn.exec(std::format("DROP TABLE IF EXISTS {}, {}, {}", q->main_table, q->ints_table, q->dbls_table));
            }
#endif
        } else {
            throw std::runtime_error("no read-back for backend '" + backend + "'");
        }
    } catch (const std::exception& ex) {
        rep.ok = false;
        if (rep.error.empty()) rep.error = ex.what();
    }

    rep.read_ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
    return rep;
}

/// Print the one-line READBACK record the test runner parses.
inline void print_readback(const ReadbackReport& r, std::string_view extra = {}) {
    std::string err = r.error;
    for (auto& c : err) if (c == '"' || c == '\n') c = '\'';
    std::cout << std::format("READBACK backend={} mode={} status={} expected={} found={} count={} checksum={} "
                             "sample={} checked={} read_ms={:.3f}{}{}{}{}\n",
                             r.backend, r.mode, r.ok ? "PASS" : "FAIL", r.expected, r.found, r.count, r.checksum,
                             r.sample, r.checked, r.read_ms, r.subset.empty() ? "" : " subset=" + r.subset, extra,
                             err.empty() ? "" : " error=\"", err.empty() ? "" : err + "\"");
}

} // namespace jac313::Store::v002::matrix_test
