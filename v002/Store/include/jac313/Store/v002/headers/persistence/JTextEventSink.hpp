#pragma once

// JTextEventSink.hpp
// Adapter that turns JTextSplitEventLog into an IEventSink for use with DoubleBufferedWriter.

#include "EventSink.hpp"
#include "JTextSplitEventLog.hpp"

#include <memory>

namespace jac313::Store::v002 {

class JTextEventSink : public IEventSink {
public:
    JTextEventSink(std::string_view base_name,
                   size_t int_count,
                   size_t dbl_count,
                   PersistMode mode = PersistMode::All)
        : impl_(std::make_unique<JTextSplitEventLog>(base_name, int_count, dbl_count, mode))
    {}

    void write_batch(std::span<const PersistedEvent> batch) override {
        if (!impl_) return;

        for (const auto& e : batch) {
            impl_->append_event(
                e.event_id,
                e.thread_id,
                e.per_thread_event_id,
                e.flags,
                e.category,
                e.payload,
                e.timestamp_us,
                e.int_metrics,
                e.dbl_metrics
            );
        }
    }

    void flush() override {
        if (impl_) impl_->flush();
    }

    void finalize() override {
        if (impl_) impl_->finalize();
    }

    std::string_view name() const override { return "JTextEventSink"; }

    [[nodiscard]] size_t main_row_count() const {
        return impl_ ? impl_->stats().main_rows : 0;
    }

private:
    std::unique_ptr<JTextSplitEventLog> impl_;
};

} // namespace jac313::Store::v002
