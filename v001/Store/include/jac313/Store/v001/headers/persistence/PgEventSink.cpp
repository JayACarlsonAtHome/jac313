#include "PgEventSink.hpp"

#include <charconv>
#include <chrono>
#include <cstdint>
#include <format>

namespace jac313::Store::v001 {

namespace {

constexpr uint64_t KEEPER_MASK   = 1ULL << 1;
constexpr uint64_t DATABASE_MASK = 1ULL << 2;

void append_int(std::string& out, int64_t v) {
    char buf[24];
    auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), v);
    out.append(buf, p);
}

} // namespace

PgEventSink::PgEventSink(std::string_view backend,
                         std::string_view persist_base,
                         size_t int_count,
                         size_t dbl_count,
                         PersistMode mode,
                         std::shared_ptr<PgTimings> timings)
    : timings_(std::move(timings))
    , table_base_(pg_table_base(persist_base))
    , int_count_(int_count)
    , dbl_count_(dbl_count)
    , mode_(mode)
{
    conn_ = std::make_unique<PgConn>(pg_conninfo_for(backend));
    if (timings_) {
        timings_->connect_ms = conn_->connect_ms();
        timings_->ping_median_ms = conn_->ping_median_ms();
    }

    const std::string s(kPgTestSchema);
    const std::string t = s + "." + table_base_;
    std::string ddl = std::format("CREATE SCHEMA IF NOT EXISTS {};\n"
                                  "DROP TABLE IF EXISTS {}, {}_ints, {}_floats;\n"
                                  "CREATE TABLE {} (\n"
                                  "    id                  BIGINT PRIMARY KEY\n"
                                  "  , thread_id           BIGINT\n"
                                  "  , per_thread_event_id BIGINT\n"
                                  "  , flags_raw           BIGINT\n"
                                  "  , category            TEXT\n"
                                  "  , payload             TEXT\n"
                                  "  , timestamp_us        BIGINT\n"
                                  ");\n", s, t, t, t, t);
    if (int_count_ > 0) {
        ddl += std::format("CREATE TABLE {}_ints (id BIGINT PRIMARY KEY", t);
        for (size_t k = 0; k < int_count_; ++k) ddl += std::format(", int{} BIGINT", k);
        ddl += ");\n";
    }
    if (dbl_count_ > 0) {
        ddl += std::format("CREATE TABLE {}_floats (id BIGINT PRIMARY KEY", t);
        for (size_t k = 0; k < dbl_count_; ++k) ddl += std::format(", dbl{} DOUBLE PRECISION", k);
        ddl += ");\n";
    }
    conn_->exec(ddl);
}

PgEventSink::~PgEventSink() {
    try { finalize(); } catch (...) {}
}

void PgEventSink::write_batch(std::span<const PersistedEvent> batch) {
    if (!conn_ || finalized_ || batch.empty()) return;

    main_buf_.clear();
    ints_buf_.clear();
    dbls_buf_.clear();
    size_t rows = 0;
    for (const auto& e : batch) {
        if (mode_ == PersistMode::KeeperOnly) {
            if ((e.flags & KEEPER_MASK) == 0) continue;
        } else if (mode_ == PersistMode::DatabaseOnly) {
            if ((e.flags & DATABASE_MASK) == 0) continue;
        }
        ++rows;
        const auto id = static_cast<int64_t>(e.event_id);
        append_int(main_buf_, id);                                   main_buf_ += '\t';
        append_int(main_buf_, static_cast<int64_t>(e.thread_id));    main_buf_ += '\t';
        append_int(main_buf_, static_cast<int64_t>(e.per_thread_event_id)); main_buf_ += '\t';
        append_int(main_buf_, static_cast<int64_t>(e.flags));        main_buf_ += '\t';
        pg_copy_append_text(main_buf_, e.category);                  main_buf_ += '\t';
        pg_copy_append_text(main_buf_, e.payload);                   main_buf_ += '\t';
        append_int(main_buf_, static_cast<int64_t>(e.timestamp_us)); main_buf_ += '\n';

        if (int_count_ > 0) {
            append_int(ints_buf_, id);
            for (size_t k = 0; k < int_count_; ++k) {
                ints_buf_ += '\t';
                append_int(ints_buf_, k < e.int_metrics.size() ? e.int_metrics[k] : 0);
            }
            ints_buf_ += '\n';
        }
        if (dbl_count_ > 0) {
            append_int(dbls_buf_, id);
            for (size_t k = 0; k < dbl_count_; ++k) {
                dbls_buf_ += '\t';
                pg_copy_append_double(dbls_buf_, k < e.dbl_metrics.size() ? e.dbl_metrics[k] : 0.0);
            }
            dbls_buf_ += '\n';
        }
    }
    if (rows == 0) return;

    const auto t0 = std::chrono::steady_clock::now();
    const std::string t = std::string(kPgTestSchema) + "." + table_base_;
    conn_->exec("BEGIN");
    try {
        conn_->copy_in(std::format("COPY {} (id, thread_id, per_thread_event_id, flags_raw, "
                                   "category, payload, timestamp_us) FROM STDIN", t), main_buf_);
        if (int_count_ > 0) conn_->copy_in(std::format("COPY {}_ints FROM STDIN", t), ints_buf_);
        if (dbl_count_ > 0) conn_->copy_in(std::format("COPY {}_floats FROM STDIN", t), dbls_buf_);
        conn_->exec("COMMIT");
    } catch (...) {
        try { conn_->exec("ROLLBACK"); } catch (...) {}
        throw;
    }
    if (timings_) {
        const double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
        timings_->batch_ms.push_back(ms);
        timings_->write_total_ms += ms;
    }
}

void PgEventSink::finalize() {
    if (finalized_) return;
    finalized_ = true;
    conn_.reset();   // every batch already committed
}

} // namespace jac313::Store::v001
