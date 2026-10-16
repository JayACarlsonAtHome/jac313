#pragma once

// PgEventSink.hpp
// PostgreSQL persistence backend ("pg-local" / "pg-remote"), the network counterpart of
// SqlEventSink. Each write batch is one transaction of COPY ... FROM STDIN per table (main,
// _ints, _floats), the fastest bulk path libpq offers. Built only with JAC313_STORE_PG_PERSIST.
//
// Tables: store_test.<pg_table_base(base)>{,_ints,_floats}. They are recreated when the sink
// opens, so a dataset always holds exactly one run ("keep last run"); read-back may drop them
// afterwards ("delete"). Timings (connect, ping, per-batch COPY) are reported through an
// optional shared PgTimings so tests can show local vs remote latency.

#include "EventSink.hpp"
#include "PersistCommon.hpp"
#include "PgClient.hpp"

#ifndef JAC313_STORE_IMPORT_STD
#include <memory>
#include <string>
#include <string_view>
#endif

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v002 {
#endif

class PgEventSink : public IEventSink {
public:
    PgEventSink(std::string_view backend,        // "pg-local" | "pg-remote"
                std::string_view persist_base,   // same base path the file sinks use
                size_t int_count,
                size_t dbl_count,
                PersistMode mode = PersistMode::All,
                std::shared_ptr<PgTimings> timings = {});
    ~PgEventSink() override;

    void write_batch(std::span<const PersistedEvent> batch) override;
    void flush() override {}
    void finalize() override;
    std::string_view name() const override { return "PgEventSink"; }

    [[nodiscard]] const std::string& table_base() const noexcept { return table_base_; }

private:
    std::unique_ptr<PgConn> conn_;
    std::shared_ptr<PgTimings> timings_;
    std::string table_base_;   // unqualified; schema is kPgTestSchema
    size_t int_count_ = 0;
    size_t dbl_count_ = 0;
    PersistMode mode_ = PersistMode::All;
    bool finalized_ = false;
    std::string main_buf_, ints_buf_, dbls_buf_;   // reused COPY buffers
};

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v002
#endif
