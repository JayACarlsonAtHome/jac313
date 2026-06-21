#pragma once

// BinaryEventLog.hpp
// Blazing-fast binary persistence for ts_store (non-debug path).
// Uses length-prefixed records for maximum speed and safety.
// This is the "production" writer. jText remains the debug/human-readable path.

#ifndef JAC313_STORE_IMPORT_STD
#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <cstdint>
#include <chrono>
#include <format>
#include <cstring>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <stdexcept>
#endif

#include "PersistCommon.hpp"

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v001 {
#endif

namespace detail {
    // Helper (binary has no jText dep) to emit the required // header at file start.
    inline size_t write_binary_file_header(int fd, std::string_view full_path) {
        auto now = std::chrono::system_clock::now();
        auto today = std::chrono::floor<std::chrono::days>(now);
        std::string date_str = std::format("{:%Y-%m-%d}", today);

        std::string h;
        h += std::format("//File:    {}\n", full_path);
        h += std::format("//Date:    {}\n", date_str);
        h += "//Purpose: Binary Data File\n";
        h += "//\n";
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
                   size_t internal_buffer_size = 64 * 1024 * 1024)
        : mode_(mode),
          int_count_(int_count),
          dbl_count_(dbl_count),
          buffer_size_(internal_buffer_size)
    {
        file_path_ = std::string(base_name) + ".bin";

        fd_ = ::open(file_path_.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
        if (fd_ < 0) {
            throw std::runtime_error("BinaryEventLog: failed to open " + file_path_);
        }

        // Write standardized // header comments first (per requirement for ALL persist files)
        size_t header_size = detail::write_binary_file_header(fd_, file_path_);

        // Pre-allocate (mmap covers header prefix + data area)
        size_t initial_map = buffer_size_ + header_size + 4096;
        ::posix_fallocate(fd_, 0, static_cast<off_t>(initial_map));

        mapped_ = static_cast<char*>(::mmap(nullptr, initial_map, PROT_READ | PROT_WRITE,
                                            MAP_SHARED, fd_, 0));
        if (mapped_ == MAP_FAILED) {
            ::close(fd_);
            throw std::runtime_error("BinaryEventLog: mmap failed");
        }

        write_pos_ = header_size;
        file_size_ = initial_map;
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
                      const std::vector<int64_t>& ints,
                      const std::vector<double>& dbls)
    {
        if (mode_ == PersistMode::KeeperOnly) {
            constexpr uint64_t KEEPER_BIT = 1ULL << 1;
            if ((raw_flags & KEEPER_BIT) == 0) return;
        } else if (mode_ == PersistMode::DatabaseOnly) {
            return;
        }

        size_t record_size = sizeof(uint64_t) * 5
                           + sizeof(uint16_t) + category.size()
                           + sizeof(uint16_t) + payload.size()
                           + sizeof(uint16_t) + (ints.size() * sizeof(int64_t))
                           + sizeof(uint16_t) + (dbls.size() * sizeof(double));

        size_t needed = sizeof(uint32_t) + record_size;

        if (write_pos_ + needed > file_size_) {
            size_t new_size = file_size_ * 2;
            if (::ftruncate(fd_, static_cast<off_t>(new_size)) != 0) {
                throw std::runtime_error("BinaryEventLog: ftruncate failed");
            }
            ::munmap(mapped_, file_size_);
            mapped_ = static_cast<char*>(::mmap(nullptr, new_size, PROT_READ | PROT_WRITE,
                                                MAP_SHARED, fd_, 0));
            if (mapped_ == MAP_FAILED) {
                throw std::runtime_error("BinaryEventLog: remap failed");
            }
            file_size_ = new_size;
        }

        // Write length + data directly into mapped memory (very fast)
        uint32_t len = static_cast<uint32_t>(record_size);
        std::memcpy(mapped_ + write_pos_, &len, sizeof(len));
        write_pos_ += sizeof(len);

        auto w64 = [&](uint64_t v) { std::memcpy(mapped_ + write_pos_, &v, sizeof(v)); write_pos_ += sizeof(v); };

        w64(event_id);
        w64(thread_id);
        w64(per_thread_event_id);
        w64(raw_flags);
        w64(timestamp_us);

        uint16_t cl = static_cast<uint16_t>(category.size());
        std::memcpy(mapped_ + write_pos_, &cl, sizeof(cl)); write_pos_ += sizeof(cl);
        if (!category.empty()) { std::memcpy(mapped_ + write_pos_, category.data(), cl); write_pos_ += cl; }

        uint16_t pl = static_cast<uint16_t>(payload.size());
        std::memcpy(mapped_ + write_pos_, &pl, sizeof(pl)); write_pos_ += sizeof(pl);
        if (!payload.empty()) { std::memcpy(mapped_ + write_pos_, payload.data(), pl); write_pos_ += pl; }

        uint16_t ic = static_cast<uint16_t>(ints.size());
        std::memcpy(mapped_ + write_pos_, &ic, sizeof(ic)); write_pos_ += sizeof(ic);
        if (!ints.empty()) { std::memcpy(mapped_ + write_pos_, ints.data(), ints.size()*sizeof(int64_t)); write_pos_ += ints.size()*sizeof(int64_t); }

        uint16_t dc = static_cast<uint16_t>(dbls.size());
        std::memcpy(mapped_ + write_pos_, &dc, sizeof(dc)); write_pos_ += sizeof(dc);
        if (!dbls.empty()) { std::memcpy(mapped_ + write_pos_, dbls.data(), dbls.size()*sizeof(double)); write_pos_ += dbls.size()*sizeof(double); }

        stats_.rows_written++;
        stats_.bytes_written += sizeof(uint32_t) + record_size;
    }

    void flush() {
        if (mapped_ && write_pos_ > 0) {
            ::msync(mapped_, write_pos_, MS_ASYNC);
            stats_.flushes++;
        }
    }

    void finalize() {
        if (finalized_) return;
        if (mapped_) {
            ::msync(mapped_, write_pos_, MS_SYNC);
            ::munmap(mapped_, file_size_);
            mapped_ = nullptr;
        }
        if (fd_ >= 0) {
            if (::ftruncate(fd_, static_cast<off_t>(write_pos_)) != 0) {
                throw std::runtime_error("BinaryEventLog: finalize ftruncate failed");
            }
            ::close(fd_);
            fd_ = -1;
        }
        finalized_ = true;
    }

    [[nodiscard]] const BinaryEventLogStats& stats() const { return stats_; }
    [[nodiscard]] const std::string& file_path() const { return file_path_; }

private:
    int fd_ = -1;
    char* mapped_ = nullptr;
    size_t write_pos_ = 0;
    size_t file_size_ = 0;

    std::string file_path_;
    PersistMode mode_;
    [[maybe_unused]] size_t int_count_ = 0;
    [[maybe_unused]] size_t dbl_count_ = 0;
    size_t buffer_size_;
    bool finalized_ = false;

    BinaryEventLogStats stats_;
};

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v001
#endif
