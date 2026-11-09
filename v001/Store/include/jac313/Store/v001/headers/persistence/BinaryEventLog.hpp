#pragma once

// BinaryEventLog.hpp
// Length-prefixed binary persistence for ts_store.
// Records are assembled into a userspace buffer and written with write().
// finalize() fsyncs once. The on-disk record layout is unchanged.

#include <filesystem>
#include <string>
#include <string_view>
#include <span>
#include <vector>
#include <cstdint>
#include <chrono>
#include <format>
#include <cstring>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <stdexcept>

#include "PersistCommon.hpp"

namespace jac313::Store::v001 {

namespace detail {
    // Helper (binary has no jText dep) to emit the required // header at file start.
    inline size_t write_binary_file_header(int fd, std::string_view path) {
        auto now = std::chrono::system_clock::now();
        auto today = std::chrono::floor<std::chrono::days>(now);
        std::string date_str = std::format("{:%Y-%m-%d}", today);

        std::string h;
        h += std::format("//File:    {}\n", std::filesystem::path(std::string(path)).filename().string());
        h += std::format("//Date:    {}\n", date_str);
        h += "//Purpose: Binary Data File\n";
        h += "//\n";
        // Explicit end marker (helps any future text scanners, though our reader
        // now uses bounded binary prefix + rfind("//\\n") for safety).
        h += "// -- end text header, binary records follow --\n";
        const ssize_t written = ::write(fd, h.data(), h.size());
        if (written < 0 || static_cast<size_t>(written) != h.size()) {
            throw std::runtime_error("BinaryEventLog: header write failed");
        }
        return h.size();
    }
}

struct BinaryEventLogStats {
    size_t rows_written = 0;
    size_t bytes_written = 0;
    size_t flushes = 0;
};

class BinaryEventLog {
public:
    BinaryEventLog(std::string_view base_name,
                   size_t int_count,
                   size_t dbl_count,
                   PersistMode mode = PersistMode::All,
                   size_t internal_buffer_size = 1 << 20)
        : buf_(internal_buffer_size == 0 ? (1 << 20) : internal_buffer_size),
          mode_(mode),
          int_count_(int_count),
          dbl_count_(dbl_count)
    {
        file_path_ = std::string(base_name) + ".bin";

        fd_ = ::open(file_path_.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
        if (fd_ < 0) {
            throw std::runtime_error("BinaryEventLog: failed to open " + file_path_);
        }

        // Write standardized // header comments first (per requirement for ALL persist files).
        // The header is its own write; records then fill `buf_` and go out in full chunks.
        detail::write_binary_file_header(fd_, file_path_);
    }

    ~BinaryEventLog() {
        finalize();
    }

    void append_event(size_t event_id,
                      size_t thread_id,
                      size_t per_thread_event_id,
                      uint64_t raw_flags,
                      std::string_view category,
                      std::string_view payload,
                      uint64_t timestamp_us,
                      std::span<const int64_t> ints,
                      std::span<const double> dbls)
    {
        if (mode_ == PersistMode::KeeperOnly) {
            constexpr uint64_t KEEPER_BIT = 1ULL << 1;
            if ((raw_flags & KEEPER_BIT) == 0) return;
        } else if (mode_ == PersistMode::DatabaseOnly) {
            return;
        }

        const size_t record_size = sizeof(uint64_t) * 5
                           + sizeof(uint16_t) + category.size()
                           + sizeof(uint16_t) + payload.size()
                           + sizeof(uint16_t) + (ints.size() * sizeof(int64_t))
                           + sizeof(uint16_t) + (dbls.size() * sizeof(double));
        const size_t needed = sizeof(uint32_t) + record_size;
        const uint32_t len = static_cast<uint32_t>(record_size);
        const uint64_t id = event_id;
        const uint64_t tid = thread_id;
        const uint64_t peid = per_thread_event_id;
        const uint64_t ts = timestamp_us;
        const uint16_t cl = static_cast<uint16_t>(category.size());
        const uint16_t pl = static_cast<uint16_t>(payload.size());
        const uint16_t ic = static_cast<uint16_t>(ints.size());
        const uint16_t dc = static_cast<uint16_t>(dbls.size());

        // One contiguous store into the buffer when the record fits in what is free.
        if (needed <= buf_.size() - filled_) {
            char* d = buf_.data() + filled_;
            auto put = [&](const void* src, size_t n) {
                std::memcpy(d, src, n);
                d += n;
            };
            put(&len, sizeof(len));
            put(&id, sizeof(id));
            put(&tid, sizeof(tid));
            put(&peid, sizeof(peid));
            put(&raw_flags, sizeof(raw_flags));
            put(&ts, sizeof(ts));
            put(&cl, sizeof(cl));
            if (cl) put(category.data(), cl);
            put(&pl, sizeof(pl));
            if (pl) put(payload.data(), pl);
            put(&ic, sizeof(ic));
            if (ic) put(ints.data(), ic * sizeof(int64_t));
            put(&dc, sizeof(dc));
            if (dc) put(dbls.data(), dc * sizeof(double));
            filled_ += needed;
        } else {
            append_bytes(&len, sizeof(len));
            append_bytes(&id, sizeof(id));
            append_bytes(&tid, sizeof(tid));
            append_bytes(&peid, sizeof(peid));
            append_bytes(&raw_flags, sizeof(raw_flags));
            append_bytes(&ts, sizeof(ts));
            append_bytes(&cl, sizeof(cl));
            if (cl) append_bytes(category.data(), cl);
            append_bytes(&pl, sizeof(pl));
            if (pl) append_bytes(payload.data(), pl);
            append_bytes(&ic, sizeof(ic));
            if (ic) append_bytes(ints.data(), ic * sizeof(int64_t));
            append_bytes(&dc, sizeof(dc));
            if (dc) append_bytes(dbls.data(), dc * sizeof(double));
        }

        stats_.rows_written++;
        stats_.bytes_written += needed;
    }

    void flush() {
        drain();
        stats_.flushes++;
    }

    void finalize() {
        if (finalized_) return;
        drain();
        bool sync_failed = false;
        if (fd_ >= 0) {
            sync_failed = (::fsync(fd_) != 0);
            ::close(fd_);
            fd_ = -1;
        }
        finalized_ = true;
        if (sync_failed) {
            throw std::runtime_error("BinaryEventLog: finalize fsync failed");
        }
    }

    [[nodiscard]] const BinaryEventLogStats& stats() const { return stats_; }
    [[nodiscard]] const std::string& file_path() const { return file_path_; }

private:
    // Push the buffer to the kernel. Does not fsync; finalize() does that once.
    void drain() {
        size_t off = 0;
        while (off < filled_) {
            const ssize_t w = ::write(fd_, buf_.data() + off, filled_ - off);
            if (w < 0) {
                if (errno == EINTR) continue;
                throw std::runtime_error("BinaryEventLog: write failed");
            }
            off += static_cast<size_t>(w);
        }
        filled_ = 0;
    }

    void append_bytes(const void* src, size_t n) {
        const char* p = static_cast<const char*>(src);
        while (n > 0) {
            if (filled_ == buf_.size()) drain();
            const size_t take = std::min(n, buf_.size() - filled_);
            std::memcpy(buf_.data() + filled_, p, take);
            filled_ += take;
            p += take;
            n -= take;
        }
    }

    int fd_ = -1;
    std::vector<char> buf_;
    size_t filled_ = 0;

    std::string file_path_;
    PersistMode mode_;
    [[maybe_unused]] size_t int_count_ = 0;
    [[maybe_unused]] size_t dbl_count_ = 0;
    bool finalized_ = false;

    BinaryEventLogStats stats_;
};

} // namespace jac313::Store::v001
