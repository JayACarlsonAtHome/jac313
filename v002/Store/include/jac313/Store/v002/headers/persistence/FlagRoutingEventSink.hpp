#pragma once

// FlagRoutingEventSink.hpp
// Routes each batch to file and/or SQL sinks based on per-event user flags.
// KeeperRecord (bit 1) → file_sink (use PersistMode::KeeperOnly on the inner sink).
// DatabaseEntry (bit 2) → sql_sink (use PersistMode::DatabaseOnly on the inner sink).
// Both inner sinks get the same batch, uncopied: each one skips the events its PersistMode
// excludes, which is what does the routing.

#include "EventSink.hpp"

#include <memory>

namespace jac313::Store::v002 {

class FlagRoutingEventSink : public IEventSink {
public:
    FlagRoutingEventSink(std::unique_ptr<IEventSink> file_sink,
                         std::unique_ptr<IEventSink> sql_sink)
        : file_sink_(std::move(file_sink))
        , sql_sink_(std::move(sql_sink))
    {}

    void write_batch(std::span<const PersistedEvent> batch) override {
        if (batch.empty()) return;
        if (file_sink_) file_sink_->write_batch(batch);
        if (sql_sink_)  sql_sink_->write_batch(batch);
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

} // namespace jac313::Store::v002
