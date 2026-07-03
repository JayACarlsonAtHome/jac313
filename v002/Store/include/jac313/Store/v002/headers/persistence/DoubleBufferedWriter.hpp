#pragma once

// DoubleBufferedWriter.hpp
// High-throughput asynchronous persistence layer.
// Hot path calls submit_event() (very cheap).
// Background thread drains full batches to the plugged-in IEventSink.
//
// Supports any sink: JTextEventSink, BinaryEventSink, future SQL sinks, etc.
// "Plug and play" design as requested.

#include "EventSink.hpp"

#ifndef JAC313_STORE_IMPORT_STD
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>   // ::write(2, ...) — worker's fail-safe error line (mirrors BinaryEventLog)
#endif

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v002 {
#endif

class DoubleBufferedWriter {
public:
    explicit DoubleBufferedWriter(std::unique_ptr<IEventSink> sink,
                                  size_t batch_size = 10'000)
        : sink_(std::move(sink)),
          batch_size_(batch_size)
    {
        if (!sink_) {
            throw std::invalid_argument("DoubleBufferedWriter requires a non-null sink");
        }
        active_buffer_.reserve(batch_size_);
        drain_buffer_.reserve(batch_size_);

        worker_ = std::thread([this] { worker_loop(); });
    }

    ~DoubleBufferedWriter() {
        stop();
    }

    // Hot path submission. Very low overhead (amortized).
    // Thread-safe.
    void submit_event(PersistedEvent&& event) {
        if (stopped_.load(std::memory_order_relaxed)) return;

        std::lock_guard<std::mutex> lock(buffer_mutex_);
        active_buffer_.push_back(std::move(event));

        if (active_buffer_.size() >= batch_size_) {
            swap_and_signal();
        }
    }

    // Optional: force a flush of the current partial batch.
    void flush() {
        std::lock_guard<std::mutex> lock(buffer_mutex_);
        if (!active_buffer_.empty()) {
            swap_and_signal();
        }
        // Also ask the sink to flush its own buffers
        if (sink_) {
            // We signal the worker to do a flush after the next drain
            pending_flush_.store(true, std::memory_order_relaxed);
            cv_.notify_one();
        }
    }

    void finalize() {
        stop();
    }

    [[nodiscard]] size_t get_batch_size() const { return batch_size_; }

private:
    void swap_and_signal() {
        // Must be called under buffer_mutex_
        // Append (do not swap): if drain still holds an unconsumed batch, swapping would
        // move that batch into active and clear() would drop those events.
        drain_buffer_.insert(drain_buffer_.end(),
                             std::make_move_iterator(active_buffer_.begin()),
                             std::make_move_iterator(active_buffer_.end()));
        active_buffer_.clear();
        cv_.notify_one();
    }

    void worker_loop() {
        while (true) {
            std::unique_lock<std::mutex> lock(buffer_mutex_);

            cv_.wait(lock, [this] {
                return !drain_buffer_.empty() ||
                       pending_flush_.load(std::memory_order_relaxed) ||
                       stop_requested_.load(std::memory_order_relaxed);
            });

            // Take ownership of the drain buffer
            std::vector<PersistedEvent> batch;
            batch.swap(drain_buffer_);

            bool do_flush = pending_flush_.exchange(false, std::memory_order_relaxed);
            bool should_stop = stop_requested_.load(std::memory_order_relaxed);

            lock.unlock();

            // Sink I/O (mmap/ftruncate/SQLite/fsync) can throw. An exception escaping this worker
            // thread would call std::terminate and take the whole process down, so contain it: log
            // once, latch sink_failed_, and keep consuming buffers (so producers never block) until
            // stop. Once failed we skip the sink rather than throw on every subsequent batch.
            if (!sink_failed_.load(std::memory_order_relaxed)) {
                try {
                    if (!batch.empty()) {
                        sink_->write_batch(batch);
                    }
                    if (do_flush || should_stop) {
                        sink_->flush();
                    }
                    if (should_stop) {
                        sink_->finalize();
                    }
                } catch (const std::exception& e) {
                    if (!sink_failed_.exchange(true)) {
                        const std::string msg =
                            "DoubleBufferedWriter: sink error — persistence disabled for the rest of "
                            "this run: " + std::string(e.what()) + "\n";
                        [[maybe_unused]] const auto written = ::write(2, msg.data(), msg.size());
                    }
                }
            }

            if (should_stop) {
                break;
            }
        }
    }

    void stop() {
        if (stopped_.exchange(true)) return;

        {
            std::lock_guard<std::mutex> lock(buffer_mutex_);
            if (!active_buffer_.empty()) {
                swap_and_signal();
            }
            stop_requested_.store(true, std::memory_order_relaxed);
            cv_.notify_one();  // notify under the lock: the predicate is published here,
                               // and it keeps thread-checkers (helgrind/DRD) from flagging
                               // a signal with no associated lock held.
        }

        if (worker_.joinable()) {
            worker_.join();
        }
    }

    std::unique_ptr<IEventSink> sink_;
    size_t batch_size_;

    std::vector<PersistedEvent> active_buffer_;
    std::vector<PersistedEvent> drain_buffer_;

    std::mutex buffer_mutex_;
    std::condition_variable cv_;
    std::thread worker_;

    std::atomic<bool> stop_requested_{false};
    std::atomic<bool> stopped_{false};
    std::atomic<bool> pending_flush_{false};
    std::atomic<bool> sink_failed_{false};   // set once if a sink call throws; disables further sink I/O
};

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v002
#endif
