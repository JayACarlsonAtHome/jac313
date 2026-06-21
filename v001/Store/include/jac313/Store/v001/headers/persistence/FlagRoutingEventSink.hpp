#pragma once

// FlagRoutingEventSink.hpp
// Routes each batch to file and/or SQL sinks based on per-event user flags.
// KeeperRecord (bit 1) → file_sink (use PersistMode::KeeperOnly on the inner sink).
// DatabaseEntry (bit 2) → sql_sink (use PersistMode::DatabaseOnly on the inner sink).

#include "EventSink.hpp"

#ifndef JAC313_STORE_IMPORT_STD
#include <memory>
#include <vector>
#endif

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v001 {
#endif

class FlagRoutingEventSink : public IEventSink {
public:
    FlagRoutingEventSink(std::unique_ptr<IEventSink> file_sink,
                         std::unique_ptr<IEventSink> sql_sink)
        : file_sink_(std::move(file_sink))
        , sql_sink_(std::move(sql_sink))
    {}

    void write_batch(std::span<const PersistedEvent> batch) override {
        if (batch.empty()) return;

        static constexpr uint64_t KEEPER_MASK    = 1ULL << 1;
        static constexpr uint64_t DATABASE_MASK  = 1ULL << 2;

        std::vector<PersistedEvent> file_batch;
        std::vector<PersistedEvent> sql_batch;
        file_batch.reserve(batch.size());
        sql_batch.reserve(batch.size());

        for (const auto& e : batch) {
            if ((e.flags & KEEPER_MASK) != 0) {
                file_batch.push_back(e);
            }
            if ((e.flags & DATABASE_MASK) != 0) {
                sql_batch.push_back(e);
            }
        }

        if (!file_batch.empty() && file_sink_) {
            file_sink_->write_batch(file_batch);
        }
        if (!sql_batch.empty() && sql_sink_) {
            sql_sink_->write_batch(sql_batch);
        }
    }

    void flush() override {
        if (file_sink_) file_sink_->flush();
        if (sql_sink_)  sql_sink_->flush();
    }

    void finalize() override {
        if (file_sink_) file_sink_->finalize();
        if (sql_sink_)  sql_sink_->finalize();
    }

    std::string_view name() const override { return "FlagRoutingEventSink"; }

private:
    std::unique_ptr<IEventSink> file_sink_;
    std::unique_ptr<IEventSink> sql_sink_;
};

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v001
#endif