// ts_store/ts_store_headers/impl_details/persist_worker.hpp
// NO namespace — this file is included inside the ts_store class (private section).
//
// A persistence worker: one thread that reads the store's rows IN PLACE (a sliding view) for ONE
// sink. The store keeps one worker per attached sink; each has its own watermark, start id, batch
// and thread, and walks the same read-only rows at its own pace. Workers share nothing but the
// rows and the store's atomics (next_id_, max_persist_rows_, epoch_).
// The rows are preallocated and never overwritten within a run, so nothing is copied or queued on
// the producers' side: save_event's last statement marks its row ready (a release store of the
// current epoch) and returns. The worker keeps a watermark (the next id not yet persisted) and,
// each round, takes the contiguous run of ready rows from there (capped by next_id_, by
// max_persist_rows_ and by the batch size), copies them into ONE reusable batch (its strings and
// vectors keep their capacity, so allocations stop after warm-up, all on this thread), hands the
// batch to the sink and advances the watermark. An empty round waits ~1 ms on a condition variable
// (or until flush / finalize is asked for). Only this thread ever calls its sink.
//
// The watermark starts at the id the next save gets when the worker is attached: rows saved before
// that are not persisted. clear() (allowed only while nothing was saved since) rewinds it to 0.

class persist_worker {
public:
    persist_worker(ts_store& store, std::unique_ptr<IEventSink> sink, size_t batch_size, size_t start)
        : store_(store)
        , sink_(std::move(sink))
        , batch_size_(batch_size == 0 ? 1 : batch_size)
        , start_(start)
        , watermark_(start)
        , attached_(std::chrono::steady_clock::now())
    {
        thread_ = std::thread([this] { run(); });
    }

    persist_worker(const persist_worker&) = delete;
    persist_worker& operator=(const persist_worker&) = delete;

    ~persist_worker() {
        try {
            finalize();
        } catch (const std::exception& e) {
            log_stderr(std::string("ts_store persistence: ") + e.what() + "\n");
        }
    }

    // Write every row that is ready now (from the watermark up to the first not-ready one), then
    // flush the sink. Returns once that is done.
    void flush() {
        std::unique_lock lock(mutex_);
        if (exited_) return;
        const uint64_t ticket = ++flush_requested_;
        wake_.notify_all();
        done_.wait(lock, [&] { return flush_done_ >= ticket || exited_; });
    }

    // clear() restarted the ids: from now on persist from id 0. Returns once the worker took it.
    void restart_at_zero() {
        std::unique_lock lock(mutex_);
        start_ = 0;
        if (exited_) {
            watermark_.store(0, std::memory_order_release);
            return;
        }
        restart_requested_ = true;
        wake_.notify_all();
        done_.wait(lock, [&] { return !restart_requested_ || exited_; });
    }

    // Write every row below next_id_, flush and finalize the sink, join the thread. Each such row
    // must become ready within kReadyTimeout; one that does not (a producer thread still running)
    // ends the drain with a std::runtime_error naming it. Idempotent.
    void finalize() {
        {
            std::lock_guard lock(mutex_);
            finalize_requested_ = true;
            wake_.notify_all();
        }
        if (thread_.joinable()) thread_.join();
        if (!error_.empty()) {
            const std::string msg = std::move(error_);
            error_.clear();
            throw std::runtime_error(msg);
        }
    }

    // True once the sink threw (or finalize found a row that never became ready).
    [[nodiscard]] bool failed() const noexcept { return failed_.load(std::memory_order_relaxed); }

    // The watermark: rows [start_id(), persisted()) have been handed to the sink (or skipped after
    // a failure).
    [[nodiscard]] size_t persisted() const noexcept { return watermark_.load(std::memory_order_acquire); }

    // The first id this worker persists (owner thread only: attach / clear).
    [[nodiscard]] size_t start_id() const noexcept { return start_; }

    // Microseconds from attach to the sink's finalize returning; 0 until the drain is done.
    [[nodiscard]] uint64_t drain_time_us() const noexcept { return drain_us_.load(std::memory_order_acquire); }

    // The sink's name (IEventSink::name()), for one report line per sink.
    [[nodiscard]] std::string_view sink_name() const { return sink_->name(); }

    // The tables / files the sink writes (IEventSink::targets()).
    [[nodiscard]] std::vector<SinkTarget> sink_targets() const { return sink_->targets(); }

private:
    static constexpr auto kIdleWait     = std::chrono::milliseconds(1);
    static constexpr auto kReadyTimeout = std::chrono::seconds(10);

    static void log_stderr(const std::string& msg) noexcept {
        [[maybe_unused]] const auto written = ::write(2, msg.data(), msg.size());   // fd 2 = stderr
    }

    // Highest id (exclusive) the sink may see right now.
    [[nodiscard]] size_t limit() const noexcept {
        size_t end = std::min(store_.next_id_.load(std::memory_order_acquire), store_.rows_.size());
        const size_t cap = store_.max_persist_rows_.load(std::memory_order_relaxed);
        if (cap != 0) end = std::min(end, cap);
        return end;
    }

    // End of the contiguous run of ready rows starting at `from`, at most `to`.
    [[nodiscard]] size_t ready_end(size_t from, size_t to) const noexcept {
        const uint32_t epoch = store_.epoch_.load(std::memory_order_relaxed);
        size_t i = from;
        while (i < to && store_.rows_[i].ready.load(std::memory_order_acquire) == epoch) {
            JAC313_HAPPENS_AFTER(&store_.rows_[i].ready);
            ++i;
        }
        return i;
    }

    // One batch of ready rows from the watermark, below `end`. Returns the number of rows taken.
    size_t write_ready(size_t end) {
        const size_t from = watermark_.load(std::memory_order_relaxed);
        if (from >= end) return 0;
        const size_t to = (end - from > batch_size_) ? from + batch_size_ : end;
        const size_t stop = ready_end(from, to);
        if (stop != from) write_rows(from, stop);
        return stop - from;
    }

    void write_rows(size_t from, size_t to) {
        const size_t n = to - from;
        if (!failed()) {
            if (batch_.size() < n) batch_.resize(n);
            for (size_t k = 0; k < n; ++k) store_.fill_persisted_event(from + k, batch_[k]);
            guarded([&] { sink_->write_batch(std::span<const PersistedEvent>(batch_.data(), n)); });
        }
        // Advance even after a failure: nothing waits on the sink.
        watermark_.store(to, std::memory_order_release);
    }

    // Sink I/O (mmap/ftruncate/SQLite/fsync) can throw. An exception escaping this thread would call
    // std::terminate, so contain it: log once, latch failed_, and skip the sink from then on.
    template <class F>
    void guarded(F&& f) noexcept {
        if (failed()) return;
        try {
            f();
        } catch (const std::exception& e) {
            fail("sink error — persistence disabled for the rest of this run: " + std::string(e.what()));
        } catch (...) {
            fail("sink error — persistence disabled for the rest of this run");
        }
    }

    void fail(const std::string& why) noexcept {
        if (!failed_.exchange(true)) log_stderr("ts_store persistence: " + why + "\n");
    }

    // finalize: every row below next_id_ (and the cap), waiting a bounded time for each to be ready.
    void drain_all() {
        const size_t end = limit();
        auto deadline = std::chrono::steady_clock::now() + kReadyTimeout;
        while (watermark_.load(std::memory_order_relaxed) < end) {
            if (write_ready(end) != 0) {
                deadline = std::chrono::steady_clock::now() + kReadyTimeout;
                continue;
            }
            if (std::chrono::steady_clock::now() > deadline) {
                const std::string why = std::format("row {} never became ready: a producer thread was "
                                                    "still running", watermark_.load(std::memory_order_relaxed));
                error_ = "ts_store persistence: " + why;
                fail(why);
                break;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        guarded([&] { sink_->flush(); });
        guarded([&] { sink_->finalize(); });
        const auto us = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - attached_).count();
        drain_us_.store(us > 0 ? static_cast<uint64_t>(us) : 1, std::memory_order_release);
    }

    void run() {
        uint64_t flush_seen = 0;
        std::unique_lock lock(mutex_);
        for (;;) {
            if (restart_requested_) {   // between rounds: no batch in flight
                watermark_.store(0, std::memory_order_release);
                restart_requested_ = false;
                done_.notify_all();
            }
            const bool finalizing = finalize_requested_;
            const uint64_t flush_ticket = flush_requested_;
            lock.unlock();

            if (finalizing) {
                drain_all();
                break;
            }
            size_t wrote = write_ready(limit());
            if (flush_ticket != flush_seen) {
                while (write_ready(limit()) != 0) {}
                guarded([&] { sink_->flush(); });
                flush_seen = flush_ticket;
                wrote = 1;   // go round again without waiting
            }

            lock.lock();
            flush_done_ = flush_seen;
            done_.notify_all();
            if (wrote == 0 && !finalize_requested_ && !restart_requested_ && flush_requested_ == flush_seen)
                wake_.wait_for(lock, kIdleWait);
        }
        lock.lock();
        exited_ = true;
        done_.notify_all();
    }

    ts_store&                   store_;
    std::unique_ptr<IEventSink> sink_;
    const size_t                batch_size_;
    size_t                      start_;            // owner thread only (attach / clear)
    std::vector<PersistedEvent> batch_;            // worker thread only; reused every round

    std::atomic<size_t> watermark_{0};             // next id not yet persisted
    std::atomic<bool>   failed_{false};
    const std::chrono::steady_clock::time_point attached_;
    std::atomic<uint64_t> drain_us_{0};            // set once by the worker when finalize is done

    std::mutex              mutex_;                // guards the request fields below
    std::condition_variable wake_;                 // worker waits here (1 ms timeout)
    std::condition_variable done_;                 // flush() waits here
    bool     finalize_requested_ = false;
    bool     restart_requested_ = false;
    bool     exited_ = false;
    uint64_t flush_requested_ = 0;
    uint64_t flush_done_ = 0;
    std::string error_;                            // set by the worker, read after join

    std::thread thread_;                           // last: started after every member above exists
};
