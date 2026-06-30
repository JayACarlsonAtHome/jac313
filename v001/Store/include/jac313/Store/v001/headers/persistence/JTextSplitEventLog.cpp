#include "JTextSplitEventLog.hpp"

#ifndef JAC313_STORE_IMPORT_STD
#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>
#include <chrono>
#endif

#include "jText.h"

// POSIX, for the durable fsync in finalize() — kept textual (never `import std`), like the store's
// other POSIX use (mmap/msync in BinaryEventLog).
#include <fcntl.h>
#include <unistd.h>

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v001 {
#endif

// jText's public API now lives under jac313::jText::v001 (in-tree migration).
using namespace jac313::jText::v001;

namespace fs = std::filesystem;

struct JTextSplitEventLog::Impl {
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

    JTextSplitEventLogStats stats;
    bool finalized = false;
};

JTextSplitEventLog::JTextSplitEventLog(
    std::string_view base_name,
    size_t int_count,
    size_t dbl_count,
    PersistMode mode
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
    auto open_and_prep = [](std::ofstream& ofs, const std::string& path, std::string_view purpose,
                             std::string_view related = {}) {
        ofs.open(path, std::ios::out | std::ios::trunc);
        if (!ofs.is_open()) {
            throw std::runtime_error("JTextSplitEventLog: failed to open " + path);
        }
        write_file_comment_header(ofs, path, purpose, related);
    };

    // Compact form for ts_store event logs:
    //   //Related: type=ts_store table=TS_STORE_TEST_005_TS
    std::string main_related = std::format("type=ts_store table={}", base_name);
    open_and_prep(i.main_ofs, i.main_path, "jText Data File", main_related);
    i.main_writer = std::make_unique<JTextWriter>(i.main_ofs);

    std::string ints_related = std::format("type=ts_store table={}_ints", base_name);
    open_and_prep(i.ints_ofs, i.ints_path, "jText Data File", ints_related);
    i.ints_writer = std::make_unique<JTextWriter>(i.ints_ofs);

    std::string floats_related = std::format("type=ts_store table={}_floats", base_name);
    open_and_prep(i.floats_ofs, i.floats_path, "jText Data File", floats_related);
    i.floats_writer = std::make_unique<JTextWriter>(i.floats_ofs);

    // High-throughput default (10k batching)
    i.main_writer->enable_high_throughput_batching();
    i.ints_writer->enable_high_throughput_batching();
    i.floats_writer->enable_high_throughput_batching();

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
    const std::vector<int64_t>& int_metrics,
    const std::vector<double>& dbl_metrics
) {
    auto& i = *impl_;
    if (!i.main_writer) return;

    if (i.mode == PersistMode::KeeperOnly) {
        constexpr uint64_t KEEPER_MASK = 1ULL << 1;
        if ((raw_flags & KEEPER_MASK) == 0) return;
    } else if (i.mode == PersistMode::DatabaseOnly) {
        return;
    }

    // Main record
    {
        JTextEntry e;
        e.number = event_id;
        e.level_sep = '|';
        e.fields.emplace_back(std::to_string(thread_id));
        e.fields.emplace_back(std::to_string(per_thread_event_id));
        e.fields.emplace_back(std::format("0x{:016x}", raw_flags));
        e.fields.emplace_back(std::string(category));
        e.fields.emplace_back(std::string(payload));
        e.fields.emplace_back(std::to_string(timestamp_us));
        i.main_writer->append_entry(e);
        i.stats.main_rows++;
        i.main_ofs << "\n";  // blank separator for records (sentinel for parser)
    }

    // Ints
    if (i.int_count > 0) {
        JTextEntry e;
        e.number = event_id;
        e.level_sep = '|';
        e.fields.reserve(1 + int_metrics.size());
        e.fields.emplace_back(std::to_string(event_id));
        for (auto v : int_metrics) e.fields.emplace_back(std::to_string(v));
        i.ints_writer->append_entry(e);
        i.stats.ints_rows++;
        i.ints_ofs << "\n";  // blank separator for records
    }

    // Floats
    if (i.dbl_count > 0) {
        JTextEntry e;
        e.number = event_id;
        e.level_sep = '|';
        e.fields.reserve(1 + dbl_metrics.size());
        e.fields.emplace_back(std::to_string(event_id));
        for (auto v : dbl_metrics) e.fields.emplace_back(std::format("{:.10g}", v));
        i.floats_writer->append_entry(e);
        i.stats.floats_rows++;
        i.floats_ofs << "\n";  // blank separator for records
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
    // — the same bug binary had (Bloopers #1), which is why jText was out-throughputting the truly
    // durable backends. Flush each stream, then fsync the file, so the real durable cost lands
    // inside the clock (one sync at finalize, mirroring binary's MS_SYNC).
    auto sync_to_disk = [](std::ofstream& ofs, const std::string& path) {
        if (path.empty()) return;
        ofs.flush();
        const int fd = ::open(path.c_str(), O_RDONLY);
        if (fd >= 0) { ::fsync(fd); ::close(fd); }
    };
    sync_to_disk(i.main_ofs, i.main_path);
    sync_to_disk(i.ints_ofs, i.ints_path);
    sync_to_disk(i.floats_ofs, i.floats_path);
    i.finalized = true;
}

const JTextSplitEventLogStats& JTextSplitEventLog::stats() const {
    return impl_->stats;
}

const std::string& JTextSplitEventLog::main_file() const { return impl_->main_path; }
const std::string& JTextSplitEventLog::ints_file() const { return impl_->ints_path; }
const std::string& JTextSplitEventLog::floats_file() const { return impl_->floats_path; }

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
        write_file_comment_header(sql, sql_path, "SQL Schema File", rel);
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
        write_file_comment_header(sql, sql_path, "SQL Schema File", rel);
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
        write_file_comment_header(sql, sql_path, "SQL Schema File", rel);
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

    i.main_writer->set_purpose("ts_store split event log (main)");
    i.main_ofs << "=== jText File ===\n";
    i.main_writer->write_header();
    i.main_ofs << "=== Section: " << base_name << " ===\n";
    i.main_ofs << "=== <#include#> Schema: " << kIncRel << "ts_store_main_schema.jschma ===\n";
    // Emit SQL templates so jtext_process can generate full loadable .sql (schema + inserts) from this .jtext
    // Using simple INSERT OR IGNORE for now (small templates, tolerant of re-runs/duplicates).
    // Full ON CONFLICT DO UPDATE (true upsert) would work but produces much larger .jtext/.sql
    // because every column must be repeated in the SET clause. That can be an option later
    // (e.g. via config or different template name). The loader (jtext_process output + sqlite3)
    // should continue on any remaining errors for subsequent records.
    // Use clean table name (last path component of base) so SQL tables are "persist" (not full path from --base-name).
    std::string table_name = fs::path(std::string(base_name)).filename().string();
    if (table_name.empty()) table_name = "persist";
    i.main_ofs << "=== Template: Create Table ===\n"
               << " 1. <<< !!!CT!!!\n"
               << "CREATE TABLE IF NOT EXISTS " << table_name << " (\n"
               << "    id BIGINT PRIMARY KEY\n"
               << "  , thread_id BIGINT\n"
               << "  , per_thread_event_id BIGINT\n"
               << "  , flags_raw BIGINT\n"
               << "  , category TEXT\n"
               << "  , payload TEXT\n"
               << "  , timestamp_us BIGINT\n"
               << ");\n"
               << "!!!CT!!!\n\n";
    i.main_ofs << "=== Template: SQL Insert ===\n"
               << " 1. <<< !!!INS!!!\n"
               << "INSERT OR IGNORE INTO " << table_name << " (id, thread_id, per_thread_event_id, flags_raw, category, payload, timestamp_us)\n"
               << "VALUES ({1}, {2}, {3}, {4}, {5}, {6}, {7});\n"
               << "!!!INS!!!\n\n";
    i.main_ofs << "=== Fields ===\n";
    i.main_ofs << "=== <#include#> Fields: " << kIncRel << "ts_store_main_fields.jtFlds ===\n";
    i.main_ofs << "=== End Fields ===\n";
    i.main_ofs << "=== Data ===\n";

    if (int_count > 0 && i.ints_writer) {
        i.ints_writer->set_purpose("ts_store integer metrics");
        i.ints_ofs << "=== jText File ===\n";
        i.ints_writer->write_header();
        i.ints_ofs << "=== Section: " << base_name << "_ints ===\n";
        i.ints_ofs << "=== <#include#> Schema: " << kIncRel << "ts_store_ints_schema.jschma ===\n";
        std::string tname_i = table_name + "_ints";
        i.ints_ofs << "=== Template: Create Table ===\n"
                   << " 1. <<< !!!CT!!!\n"
                   << "CREATE TABLE IF NOT EXISTS " << tname_i << " (\n"
                   << "    id BIGINT PRIMARY KEY\n";
        for (size_t k = 0; k < int_count; ++k) {
            i.ints_ofs << "  , int" << k << " BIGINT\n";
        }
        i.ints_ofs << ");\n"
                   << "!!!CT!!!\n\n";
        i.ints_ofs << "=== Template: SQL Insert ===\n"
                   << " 1. <<< !!!INS!!!\n"
                   << "INSERT OR IGNORE INTO " << tname_i << " (id";
        for (size_t k = 0; k < int_count; ++k) i.ints_ofs << ", int" << k;
        i.ints_ofs << ")\nVALUES ({1}";
        for (size_t k = 0; k < int_count; ++k) i.ints_ofs << ", {" << (k+2) << "}";
        i.ints_ofs << ");\n"
                   << "!!!INS!!!\n\n";
        i.ints_ofs << "=== Fields ===\n";
        i.ints_ofs << "=== <#include#> Fields: " << kIncRel << "ts_store_ints_fields.jtFlds ===\n";
        i.ints_ofs << "=== End Fields ===\n";
        i.ints_ofs << "=== Data ===\n";
    }

    if (dbl_count > 0 && i.floats_writer) {
        i.floats_writer->set_purpose("ts_store double metrics");
        i.floats_ofs << "=== jText File ===\n";
        i.floats_writer->write_header();
        i.floats_ofs << "=== Section: " << base_name << "_floats ===\n";
        i.floats_ofs << "=== <#include#> Schema: " << kIncRel << "ts_store_floats_schema.jschma ===\n";
        std::string tname_f = table_name + "_floats";
        i.floats_ofs << "=== Template: Create Table ===\n"
                   << " 1. <<< !!!CT!!!\n"
                   << "CREATE TABLE IF NOT EXISTS " << tname_f << " (\n"
                   << "    id BIGINT PRIMARY KEY\n";
        for (size_t k = 0; k < dbl_count; ++k) {
            i.floats_ofs << "  , dbl" << k << " DOUBLE PRECISION\n";
        }
        i.floats_ofs << ");\n"
                   << "!!!CT!!!\n\n";
        i.floats_ofs << "=== Template: SQL Insert ===\n"
                   << " 1. <<< !!!INS!!!\n"
                   << "INSERT OR IGNORE INTO " << tname_f << " (id";
        for (size_t k = 0; k < dbl_count; ++k) i.floats_ofs << ", dbl" << k;
        i.floats_ofs << ")\nVALUES ({1}";
        for (size_t k = 0; k < dbl_count; ++k) i.floats_ofs << ", {" << (k+2) << "}";
        i.floats_ofs << ");\n"
                   << "!!!INS!!!\n\n";
        i.floats_ofs << "=== Fields ===\n";
        i.floats_ofs << "=== <#include#> Fields: " << kIncRel << "ts_store_floats_fields.jtFlds ===\n";
        i.floats_ofs << "=== End Fields ===\n";
        i.floats_ofs << "=== Data ===\n";
    }
}

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v001
#endif
