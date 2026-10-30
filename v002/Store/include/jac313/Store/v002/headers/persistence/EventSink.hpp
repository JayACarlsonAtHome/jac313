#pragma once

// EventSink.hpp
// Abstract interface for pluggable persistence backends (jText, Binary, SQL, etc.)
// ts_store::attach_persistence gives each sink its own persistence worker thread, which hands it
// batches; several sinks may be attached to one store, each routing by its own PersistMode.

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <memory>

namespace jac313::Store::v002 {

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

/// One table or file a sink writes: what results.db records per run (run_target). A file sink
/// gives one per file (`table` = the file path); a database sink one per table, with `dbname`
/// (SQLite: the .db path) and, for a server, its `host` / `address` / `port` as the connection
/// reports them after connecting.
struct SinkTarget {
    std::string host;      // server host name ('' for a file or SQLite)
    std::string address;   // the address connected to ('' for a file or SQLite)
    int port = 0;          // 0 = none
    std::string dbname;    // the database ('' for a file sink)
    std::string table;     // the table, or the file path for a file sink
};

/// The files of a split log (main, _Ints, _Floats): the metric files only when it has such metrics.
template<typename SplitLog>
std::vector<SinkTarget> file_targets(const SplitLog& log, size_t int_count, size_t dbl_count) {
    std::vector<SinkTarget> v{{"", "", 0, "", log.main_file()}};
    if (int_count > 0) v.push_back({"", "", 0, "", log.ints_file()});
    if (dbl_count > 0) v.push_back({"", "", 0, "", log.floats_file()});
    return v;
}

/// Abstract base for any persistence backend.
/// One sink is called from one thread only (the worker it was attached with), so it needs no
/// locking of its own. With several sinks attached, each must own its own file or connection:
/// two sinks on one file interleave, and a database connection (libpq's included) is not
/// thread-safe.
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

    /// Every table or file this sink writes. Known from construction on and still valid after
    /// finalize(), so a test or the bench can report it once the run is done.
    virtual std::vector<SinkTarget> targets() const { return {}; }
};

} // namespace jac313::Store::v002
