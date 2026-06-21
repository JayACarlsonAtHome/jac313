#pragma once

// BinaryEventSink.hpp
// Adapter that turns BinaryEventLog into an IEventSink for use with DoubleBufferedWriter.

#include "EventSink.hpp"
#include "BinaryEventLog.hpp"

#ifndef JAC313_STORE_IMPORT_STD
#include <memory>
#endif

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v002 {
#endif

class BinaryEventSink : public IEventSink {
public:
    BinaryEventSink(std::string_view base_name,
                    size_t int_count,
                    size_t dbl_count,
                    PersistMode mode = PersistMode::All,
                    size_t internal_buffer_size = 64 * 1024 * 1024)
        : impl_(std::make_unique<BinaryEventLog>(base_name, int_count, dbl_count, mode, internal_buffer_size))
    {}

    void write_batch(std::span<const PersistedEvent> batch) override {
        if (!impl_) return;

        for (const auto& e : batch) {
            std::vector<int64_t> ints = e.int_metrics;
            std::vector<double>  dbls = e.dbl_metrics;

            impl_->append_event(
                e.event_id,
                e.thread_id,
                e.per_thread_event_id,
                e.flags,
                e.category,
                e.payload,
                e.timestamp_us,
                ints,
                dbls
            );
        }
    }

    void flush() override {
        if (impl_) impl_->flush();
    }

    void finalize() override {
        if (impl_) {
            impl_->finalize();
            impl_.reset();
        }
    }

    std::string_view name() const override { return "BinaryEventSink"; }

private:
    std::unique_ptr<BinaryEventLog> impl_;
};

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v002
#endif
