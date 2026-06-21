#pragma once

// EventSink.hpp
// Abstract interface for pluggable persistence backends (jText, Binary, SQL, etc.)
// Designed to work with DoubleBufferedWriter for asynchronous background draining.

#ifndef JAC313_STORE_IMPORT_STD
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <memory>
#endif

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v001 {
#endif

/// A single event record ready for persistence.
/// This is the canonical form passed to all sinks.
struct PersistedEvent {
    size_t      event_id{0};
    size_t      thread_id{0};
    size_t      per_thread_event_id{0};
    uint64_t    flags{0};
    std::string category;
    std::string payload;
    uint64_t    timestamp_us{0};

    std::vector<int64_t> int_metrics;
    std::vector<double>  dbl_metrics;
};

/// Abstract base for any persistence backend.
/// Implementations must be thread-safe for the write_batch / flush calls
/// (DoubleBufferedWriter guarantees that only one thread calls these at a time).
class IEventSink {
public:
    virtual ~IEventSink() = default;

    /// Write a batch of events. The batch is guaranteed to be non-empty.
    /// The implementation should not assume it can hold onto the span after return.
    virtual void write_batch(std::span<const PersistedEvent> batch) = 0;

    /// Flush any internal buffers to durable storage.
    virtual void flush() = 0;

    /// Finalize (close files, etc.). Called on shutdown.
    virtual void finalize() = 0;

    /// Optional: return a human-readable name for diagnostics.
    virtual std::string_view name() const { return "IEventSink"; }
};

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v001
#endif
