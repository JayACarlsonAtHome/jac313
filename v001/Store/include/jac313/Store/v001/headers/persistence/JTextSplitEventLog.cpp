#include "JTextSplitEventLog.hpp"

#include <array>
#include <charconv>
#include <concepts>
#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "jText.h"

// POSIX, for the durable fsync in finalize() — like the store's
// other POSIX use (mmap/msync in BinaryEventLog).
#include <fcntl.h>
#include <unistd.h>

namespace jac313::Store::v001 {

// jText's public API now lives under jac313::jText::v001 (in-tree migration).
using namespace jac313::jText::v001;

namespace fs = std::filesystem;

namespace {

// A number's text, in a buffer the log owns: no string per field.
using IntegerText = std::array<char, jtext_row_bytes::kInteger>;
using FlagsText   = std::array<char, jtext_row_bytes::kFlags>;
using DoubleText  = std::array<char, jtext_row_bytes::kDouble>;

std::string_view text_between(const char* first, const char* last) {
    return {first, static_cast<size_t>(last - first)};
}

// An integer in decimal (the text std::to_string gives).
template<std::integral T>
std::string_view integer_text(IntegerText& out, T v) {
    return text_between(out.data(), std::to_chars(out.data(), out.data() + out.size(), v).ptr);
}

// The flags as "0x" and 16 lower-case hex digits, zero-padded.
std::string_view flags_text(FlagsText& out, uint64_t flags) {
    constexpr std::string_view digits = "0123456789abcdef";
    out[0] = '0';
    out[1] = 'x';
    for (size_t k = 0; k < 16; ++k) out[2 + k] = digits[(flags >> (60 - 4 * k)) & 0xFu];
    return {out.data(), out.size()};
}

// A double as append_double writes it (NaN, Infinity, -Infinity included).
std::string_view double_text(DoubleText& out, double v) {
    return text_between(out.data(), double_to_chars(out.data(), out.data() + out.size(), v));
}

// The text buffers and field views of a metric row (_Ints or _Floats): field 1 is the event id,
// then one per metric. Sized in the constructor; a row with more metrics than that grows them.
template<typename Text>
struct MetricRow {
    std::vector<Text> texts;
    std::vector<std::string_view> fields;

    void size_for(size_t metric_count) {
        if (texts.size() < metric_count) texts.resize(metric_count);
        if (fields.size() < 1 + metric_count) fields.resize(1 + metric_count);
    }
};

} // namespace

struct JTextSplitEventLog::Impl {
    StreamBuffer main_buf;
    StreamBuffer ints_buf;
    StreamBuffer floats_buf;
    std::ofstream main_ofs;
    std::ofstream ints_ofs;
    std::ofstream floats_ofs;

    std::unique_ptr<JTextWriter> main_writer;
    std::unique_ptr<JTextWriter> ints_writer;
    std::unique_ptr<JTextWriter> floats_writer;

    std::string main_path;
    std::string ints_path;
    std::string floats_path;

    PersistMode mode;
    size_t int_count = 0;
    size_t dbl_count = 0;

    // The main row's number texts (event id, thread id, per-thread id, timestamp; the flags)
    // and its seven field views. The event id's text is field 1 of the metric rows too.
    std::array<IntegerText, 4> main_numbers{};
    FlagsText flags{};
    std::array<std::string_view, 7> main_fields{};
    MetricRow<IntegerText> ints;
    MetricRow<DoubleText> floats;

    JTextSplitEventLogStats stats;
    bool finalized = false;
};

JTextSplitEventLog::JTextSplitEventLog(
    std::string_view base_name,
    size_t int_count,
    size_t dbl_count,
    PersistMode mode,
    JTextRowLimits limits
)
    : impl_(std::make_unique<Impl>())
{
    if (base_name.empty()) {
        throw std::invalid_argument("JTextSplitEventLog: base_name cannot be empty");
    }

    auto& i = *impl_;
    i.mode = mode;
    i.int_count = int_count;
    i.dbl_count = dbl_count;

    i.main_path = std::format("{}.jtext", base_name);
    i.ints_path = std::format("{}_Ints.jtext", base_name);
    i.floats_path = std::format("{}_Floats.jtext", base_name);

    // Open files ourselves first so we can prepend the required standardized // header comments.
    // Then attach JTextWriter to the open stream (its # header comes after our // block).
    auto open_and_prep = [](std::ofstream& ofs, StreamBuffer& buf, const std::string& path,
                             std::string_view purpose, std::string_view related = {}) {
        ofs.open(path, std::ios::out | std::ios::trunc);
        if (!ofs.is_open()) {
            throw std::runtime_error("JTextSplitEventLog: failed to open " + path);
        }
        buf.arm(ofs);
        write_file_comment_header(ofs, path, purpose, related);
    };

    // Compact form for ts_store event logs:
    //   //Related: type=ts_store table=TS_STORE_TEST_005_TS
    std::string main_related = std::format("type=ts_store table={}", base_name);
    open_and_prep(i.main_ofs, i.main_buf, i.main_path, "jText Data File", main_related);
    i.main_writer = std::make_unique<JTextWriter>(i.main_ofs);
    i.main_writer->set_profile(JTextProfile::Full);
    i.main_writer->set_filename(i.main_path);

    std::string ints_related = std::format("type=ts_store table={}_ints", base_name);
    open_and_prep(i.ints_ofs, i.ints_buf, i.ints_path, "jText Data File", ints_related);
    i.ints_writer = std::make_unique<JTextWriter>(i.ints_ofs);
    i.ints_writer->set_profile(JTextProfile::Full);
    i.ints_writer->set_filename(i.ints_path);

    std::string floats_related = std::format("type=ts_store table={}_floats", base_name);
    open_and_prep(i.floats_ofs, i.floats_buf, i.floats_path, "jText Data File", floats_related);
    i.floats_writer = std::make_unique<JTextWriter>(i.floats_ofs);
    i.floats_writer->set_profile(JTextProfile::Full);
    i.floats_writer->set_filename(i.floats_path);

    // Rows are written with the writer's default formatter, U+25AE / U+258C (SPEC v0.8),
    // which it changes only for a row whose data holds one of them. No JTextWriter
    // auto-batching: the ofstream buffer does the batching. Each writer formats a row into its
    // one-row buffer, reserved here for the longest one-line row (jtext_row_bytes), so it does
    // not grow while rows are written: the main one when the row limits are known, the metric
    // ones always (their rows hold numbers only). A multiline row may be longer and grows it.
    i.main_writer->reserve_batch_bytes(jtext_row_bytes::main_row(limits));
    if (int_count > 0) i.ints_writer->reserve_batch_bytes(jtext_row_bytes::ints_row(int_count));
    if (dbl_count > 0) i.floats_writer->reserve_batch_bytes(jtext_row_bytes::floats_row(dbl_count));
    i.ints.size_for(int_count);
    i.floats.size_for(dbl_count);

    // Write SQL companions and headers immediately (early open requirement)
    write_sql_companions(base_name, int_count, dbl_count);
    write_all_headers(base_name, int_count, dbl_count);
}

JTextSplitEventLog::~JTextSplitEventLog() {
    if (impl_ && !impl_->finalized) {
        try { finalize(); } catch (...) {}
    }
}

JTextSplitEventLog::JTextSplitEventLog(JTextSplitEventLog&&) noexcept = default;
JTextSplitEventLog& JTextSplitEventLog::operator=(JTextSplitEventLog&&) noexcept = default;

void JTextSplitEventLog::append_event(
    size_t event_id,
    size_t thread_id,
    size_t per_thread_event_id,
    uint64_t raw_flags,
    std::string_view category,
    std::string_view payload,
    uint64_t timestamp_us,
    std::span<const int64_t> int_metrics,
    std::span<const double> dbl_metrics
) {
    auto& i = *impl_;
    if (!i.main_writer) return;

    if (i.mode == PersistMode::KeeperOnly) {
        constexpr uint64_t KEEPER_MASK = 1ULL << 1;
        if ((raw_flags & KEEPER_MASK) == 0) return;
    } else if (i.mode == PersistMode::DatabaseOnly) {
        return;
    }

    const std::string_view id = integer_text(i.main_numbers[0], event_id);

    // Main record (jText line number = field position: a compact row starts at field 1)
    i.main_fields = {id,
                     integer_text(i.main_numbers[1], thread_id),
                     integer_text(i.main_numbers[2], per_thread_event_id),
                     flags_text(i.flags, raw_flags),
                     category,
                     payload,
                     integer_text(i.main_numbers[3], timestamp_us)};
    i.main_writer->append_row_views(1, i.main_fields);
    i.main_writer->end_record();
    i.stats.main_rows++;

    // Ints: field 1 = event_id (links to main record)
    if (i.int_count > 0) {
        i.ints.size_for(int_metrics.size());
        i.ints.fields[0] = id;
        for (size_t k = 0; k < int_metrics.size(); ++k)
            i.ints.fields[1 + k] = integer_text(i.ints.texts[k], int_metrics[k]);
        i.ints_writer->append_row_views(1, std::span(i.ints.fields).first(1 + int_metrics.size()));
        i.ints_writer->end_record();
        i.stats.ints_rows++;
    }

    // Floats: field 1 = event_id (links to main record); exact, NaN, Infinity, -Infinity
    if (i.dbl_count > 0) {
        i.floats.size_for(dbl_metrics.size());
        i.floats.fields[0] = id;
        for (size_t k = 0; k < dbl_metrics.size(); ++k)
            i.floats.fields[1 + k] = double_text(i.floats.texts[k], dbl_metrics[k]);
        i.floats_writer->append_row_views(1, std::span(i.floats.fields).first(1 + dbl_metrics.size()));
        i.floats_writer->end_record();
        i.stats.floats_rows++;
    }
}

void JTextSplitEventLog::flush() {
    auto& i = *impl_;
    if (i.main_writer) i.main_writer->flush();
    if (i.ints_writer) i.ints_writer->flush();
    if (i.floats_writer) i.floats_writer->flush();
    i.stats.batches_flushed++;
}

void JTextSplitEventLog::finalize() {
    auto& i = *impl_;
    if (i.finalized) return;
    if (i.main_writer) i.main_writer->finalize();
    if (i.ints_writer) i.ints_writer->finalize();
    if (i.floats_writer) i.floats_writer->finalize();
    // Durably flush to disk. The jText writers only push bytes into the std::ofstream (i.e. the OS
    // page cache); without this, "durable jText" was timing buffered writes that were never fsync'd
    // — the same bug binary had, which is why jText was out-throughputting the truly durable
    // backends. Flush each stream, then fsync the file, so the real durable cost lands inside
    // the clock (one sync at finalize, mirroring binary's MS_SYNC).
    sync_text_file(i.main_ofs, i.main_path);
    sync_text_file(i.ints_ofs, i.ints_path);
    sync_text_file(i.floats_ofs, i.floats_path);
    i.finalized = true;
}

const JTextSplitEventLogStats& JTextSplitEventLog::stats() const {
    return impl_->stats;
}

const std::string& JTextSplitEventLog::main_file() const { return impl_->main_path; }
const std::string& JTextSplitEventLog::ints_file() const { return impl_->ints_path; }
const std::string& JTextSplitEventLog::floats_file() const { return impl_->floats_path; }

size_t JTextSplitEventLog::main_batch_capacity() const {
    return impl_->main_writer ? impl_->main_writer->batch_capacity() : 0;
}

void JTextSplitEventLog::write_sql_companions(
    std::string_view base_name,
    size_t int_count,
    size_t dbl_count
) {
    // (SQL generation logic - kept functional, no macros)
    std::string table_name = fs::path(std::string(base_name)).filename().string();
    if (table_name.empty()) table_name = "persist";
    {
        std::string sql_path = std::format("{}.sql", base_name);
        std::ofstream sql(sql_path);
        std::string rel = std::format("type=ts_store table={}", table_name);
        write_sql_comment_header(sql, sql_path, "SQL Schema File", rel);
        sql << "CREATE TABLE IF NOT EXISTS " << table_name << " (\n"
            << "    id BIGINT PRIMARY KEY\n"
            << "  , thread_id BIGINT\n"
            << "  , per_thread_event_id BIGINT\n"
            << "  , flags_raw BIGINT\n"
            << "  , category TEXT\n"
            << "  , payload TEXT\n"
            << "  , timestamp_us BIGINT\n"
            << ");\n\n";
    }

    if (int_count > 0) {
        std::string tname_i = table_name + "_ints";
        std::string sql_path = std::format("{}_Ints.sql", base_name);
        std::ofstream sql(sql_path);
        std::string rel = std::format("type=ts_store table={}", tname_i);
        write_sql_comment_header(sql, sql_path, "SQL Schema File", rel);
        sql << "CREATE TABLE IF NOT EXISTS " << tname_i << " (\n"
            << "    id BIGINT PRIMARY KEY\n";
        for (size_t i = 0; i < int_count; ++i) {
            sql << "  , int" << i << " BIGINT\n";
        }
        sql << ");\n\n";
    }

    if (dbl_count > 0) {
        std::string tname_f = table_name + "_floats";
        std::string sql_path = std::format("{}_Floats.sql", base_name);
        std::ofstream sql(sql_path);
        std::string rel = std::format("type=ts_store table={}", tname_f);
        write_sql_comment_header(sql, sql_path, "SQL Schema File", rel);
        sql << "CREATE TABLE IF NOT EXISTS " << tname_f << " (\n"
            << "    id BIGINT PRIMARY KEY\n";
        for (size_t i = 0; i < dbl_count; ++i) {
            sql << "  , dbl" << i << " DOUBLE PRECISION\n";
        }
        sql << ");\n\n";
    }
}

void JTextSplitEventLog::write_all_headers(std::string_view base_name, size_t int_count, size_t dbl_count) {
    auto& i = *impl_;
    if (!i.main_writer) return;

    // 3 common Schema files + 3 common Field List files live in tests/jtext_includes/
    // (committed, re-used across tests).
    // Each per-test Data .jtext (main + _Ints + _Floats) references the applicable ones
    // via include markers. Only the actual data rows are unique per test run.
    // Relative path: from test_results/jText_logs/<test>/ back to tests/.
    const std::string kIncRel = "../../../tests/jtext_includes/";

    // Each file: the full-profile header, one section with its schema include, the SQL
    // templates (jtext_process makes loadable .sql from them), the Fields block naming the
    // shared field list, then the Data block. The writer closes every block at finalize().
    // INSERT OR IGNORE keeps the templates small and tolerant of re-runs; a full upsert
    // (ON CONFLICT DO UPDATE) would repeat every column and could become an option later.
    // The table is the last path component of base_name ("persist", not the full path).
    std::string table_name = fs::path(std::string(base_name)).filename().string();
    if (table_name.empty()) table_name = "persist";

    auto open_file = [&](JTextWriter& w, std::string_view purpose, std::string_view section,
                         std::string_view kind, const std::string& create, const std::string& insert) {
        w.set_purpose(purpose);
        w.write_header();
        w.begin_section(section);
        w.write_include("Schema", kIncRel + std::format("ts_store_{}_schema.jschma", kind));
        w.write_template("Create Table", create);
        w.write_template("SQL Insert", insert);
        w.begin_fields();
        w.write_include("Fields", kIncRel + std::format("ts_store_{}_fields.jtFlds", kind));
        w.end_fields();
        w.begin_data();
    };

    open_file(*i.main_writer, "ts_store split event log (main)", base_name, "main",
              std::format("CREATE TABLE IF NOT EXISTS {} (\n"
                          "    id BIGINT PRIMARY KEY\n"
                          "  , thread_id BIGINT\n"
                          "  , per_thread_event_id BIGINT\n"
                          "  , flags_raw BIGINT\n"
                          "  , category TEXT\n"
                          "  , payload TEXT\n"
                          "  , timestamp_us BIGINT\n"
                          ");", table_name),
              std::format("INSERT OR IGNORE INTO {} (id, thread_id, per_thread_event_id, flags_raw, category, payload, timestamp_us)\n"
                          "VALUES ({{1}}, {{2}}, {{3}}, {{4}}, {{5}}, {{6}}, {{7}});", table_name));

    auto metric_file = [&](JTextWriter& w, std::string_view purpose, std::string_view kind,
                           std::string_view prefix, std::string_view sql_type, size_t count) {
        const std::string table = std::format("{}_{}", table_name, kind);
        std::string create = std::format("CREATE TABLE IF NOT EXISTS {} (\n    id BIGINT PRIMARY KEY\n", table);
        std::string columns = "id", values = "{1}";
        for (size_t k = 0; k < count; ++k) {
            create += std::format("  , {}{} {}\n", prefix, k, sql_type);
            columns += std::format(", {}{}", prefix, k);
            values += std::format(", {{{}}}", k + 2);
        }
        create += ");";
        open_file(w, purpose, std::format("{}_{}", base_name, kind), kind, create,
                  std::format("INSERT OR IGNORE INTO {} ({})\nVALUES ({});", table, columns, values));
    };
    if (int_count > 0 && i.ints_writer)
        metric_file(*i.ints_writer, "ts_store integer metrics", "ints", "int", "BIGINT", int_count);
    if (dbl_count > 0 && i.floats_writer)
        metric_file(*i.floats_writer, "ts_store double metrics", "floats", "dbl", "DOUBLE PRECISION", dbl_count);
}

} // namespace jac313::Store::v001
