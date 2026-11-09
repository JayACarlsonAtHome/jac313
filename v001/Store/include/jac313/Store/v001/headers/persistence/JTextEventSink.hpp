#pragma once

// JTextEventSink.hpp
// Adapter that turns JTextSplitEventLog into an IEventSink for ts_store::attach_persistence.
// `limits` (jtext_row_limits_for<Config>() where the store's Config is known) lets the log
// reserve its main row buffer once; each event's fields reach the writers as views.

#include "EventSink.hpp"
#include "JTextSplitEventLog.hpp"

#include <memory>

namespace jac313::Store::v001 {

class JTextEventSink : public IEventSink {
public:
    JTextEventSink(std::string_view base_name,
                   size_t int_count,
                   size_t dbl_count,
                   PersistMode mode = PersistMode::All,
                   JTextRowLimits limits = {})
        : impl_(std::make_unique<JTextSplitEventLog>(base_name, int_count, dbl_count, mode, limits))
    {
        targets_ = file_targets(*impl_, 1, 1);   // a jText log always writes all three files
    }

    void write_batch(std::span<const PersistedEventView> batch) override {
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

    std::vector<SinkTarget> targets() const override { return targets_; }

    [[nodiscard]] size_t main_row_count() const {
        return impl_ ? impl_->stats().main_rows : 0;
    }

private:
    std::vector<SinkTarget> targets_;   // the files written (targets())
    std::unique_ptr<JTextSplitEventLog> impl_;
};

} // namespace jac313::Store::v001
