#pragma once

// HtmlEventSink.hpp
// Adapter that turns HtmlSplitEventLog into an IEventSink for ts_store::attach_persistence.

#include "EventSink.hpp"
#include "HtmlSplitEventLog.hpp"

#include <memory>

namespace jac313::Store::v002 {

class HtmlEventSink : public IEventSink {
public:
    HtmlEventSink(std::string_view base_name,
                  size_t int_count,
                  size_t dbl_count,
                  PersistMode mode = PersistMode::All)
        : impl_(std::make_unique<HtmlSplitEventLog>(base_name, int_count, dbl_count, mode))
    {
        targets_ = file_targets(*impl_, int_count, dbl_count);
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

    std::string_view name() const override { return "HtmlEventSink"; }

    std::vector<SinkTarget> targets() const override { return targets_; }

    [[nodiscard]] size_t main_row_count() const {
        return impl_ ? impl_->stats().main_rows : 0;
    }

private:
    std::vector<SinkTarget> targets_;   // the files written (targets())
    std::unique_ptr<HtmlSplitEventLog> impl_;
};

} // namespace jac313::Store::v002