#include "JsonSplitEventLog.hpp"

#ifndef JAC313_STORE_IMPORT_STD
#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>
#endif

#include <fcntl.h>
#include <unistd.h>

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v001 {
#endif

namespace {

std::string json_escape(std::string_view text) {
    std::string out;
    out.reserve(text.size() + 8);
    for (const char c : text) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b";  break;
            case '\f': out += "\\f";  break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    out += std::format("\\u{:04x}", static_cast<unsigned char>(c));
                } else {
                    out += c;
                }
                break;
        }
    }
    return out;
}

void sync_to_disk(std::ofstream& ofs, const std::string& path) {
    if (path.empty()) return;
    ofs.flush();
    const int fd = ::open(path.c_str(), O_RDONLY);
    if (fd >= 0) { ::fsync(fd); ::close(fd); }
}

} // namespace

namespace fs = std::filesystem;

struct JsonSplitEventLog::Impl {
    std::ofstream main_ofs;
    std::ofstream ints_ofs;
    std::ofstream floats_ofs;

    std::string main_path;
    std::string ints_path;
    std::string floats_path;

    PersistMode mode;
    size_t int_count = 0;
    size_t dbl_count = 0;

    JsonSplitEventLogStats stats;
    bool finalized = false;
};

JsonSplitEventLog::JsonSplitEventLog(
    std::string_view base_name,
    size_t int_count,
    size_t dbl_count,
    PersistMode mode
)
    : impl_(std::make_unique<Impl>())
{
    if (base_name.empty()) {
        throw std::invalid_argument("JsonSplitEventLog: base_name cannot be empty");
    }

    auto& i = *impl_;
    i.mode = mode;
    i.int_count = int_count;
    i.dbl_count = dbl_count;

    i.main_path = std::format("{}.json", base_name);
    i.ints_path = std::format("{}_Ints.json", base_name);
    i.floats_path = std::format("{}_Floats.json", base_name);

    const std::string table_name = [&] {
        std::string t = fs::path(std::string(base_name)).filename().string();
        return t.empty() ? "persist" : t;
    }();

    auto open_json = [&](std::ofstream& ofs, const std::string& path,
                         std::string_view purpose, std::string_view related) {
        ofs.open(path, std::ios::out | std::ios::trunc);
        if (!ofs.is_open()) {
            throw std::runtime_error("JsonSplitEventLog: failed to open " + path);
        }
        write_file_comment_header(ofs, path, purpose, related);
    };

    open_json(i.main_ofs, i.main_path, "JSON Data File",
              std::format("type=ts_store table={}", table_name));

    if (int_count > 0) {
        open_json(i.ints_ofs, i.ints_path, "JSON Data File",
                  std::format("type=ts_store table={}_ints", table_name));
    }
    if (dbl_count > 0) {
        open_json(i.floats_ofs, i.floats_path, "JSON Data File",
                  std::format("type=ts_store table={}_floats", table_name));
    }
}

JsonSplitEventLog::~JsonSplitEventLog() {
    if (impl_ && !impl_->finalized) {
        try { finalize(); } catch (...) {}
    }
}

JsonSplitEventLog::JsonSplitEventLog(JsonSplitEventLog&&) noexcept = default;
JsonSplitEventLog& JsonSplitEventLog::operator=(JsonSplitEventLog&&) noexcept = default;

void JsonSplitEventLog::append_event(
    size_t event_id,
    size_t thread_id,
    size_t per_thread_event_id,
    uint64_t raw_flags,
    std::string_view category,
    std::string_view payload,
    uint64_t timestamp_us,
    const std::vector<int64_t>& int_metrics,
    const std::vector<double>& dbl_metrics
) {
    auto& i = *impl_;
    if (i.finalized) return;

    if (i.mode == PersistMode::KeeperOnly) {
        constexpr uint64_t KEEPER_MASK = 1ULL << 1;
        if ((raw_flags & KEEPER_MASK) == 0) return;
    } else if (i.mode == PersistMode::DatabaseOnly) {
        return;
    }

    i.main_ofs << std::format(
        R"({{"id":{},"thread_id":{},"per_thread_event_id":{},"flags_raw":"0x{:016x}","category":"{}","payload":"{}","timestamp_us":{}}})",
        event_id,
        thread_id,
        per_thread_event_id,
        raw_flags,
        json_escape(category),
        json_escape(payload),
        timestamp_us
    ) << "\n";
    i.stats.main_rows++;

    if (i.int_count > 0 && i.ints_ofs.is_open()) {
        std::string line = std::format(R"({{"id":{})", event_id);
        for (size_t k = 0; k < i.int_count; ++k) {
            const int64_t v = (k < int_metrics.size()) ? int_metrics[k] : 0;
            line += std::format(R"(,"int{}":{})", k, v);
        }
        line += "}\n";
        i.ints_ofs << line;
        i.stats.ints_rows++;
    }

    if (i.dbl_count > 0 && i.floats_ofs.is_open()) {
        std::string line = std::format(R"({{"id":{})", event_id);
        for (size_t k = 0; k < i.dbl_count; ++k) {
            const double v = (k < dbl_metrics.size()) ? dbl_metrics[k] : 0.0;
            line += std::format(R"(,"dbl{}":{:.10g})", k, v);
        }
        line += "}\n";
        i.floats_ofs << line;
        i.stats.floats_rows++;
    }
}

void JsonSplitEventLog::flush() {
    auto& i = *impl_;
    i.main_ofs.flush();
    if (i.ints_ofs.is_open()) i.ints_ofs.flush();
    if (i.floats_ofs.is_open()) i.floats_ofs.flush();
    i.stats.batches_flushed++;
}

void JsonSplitEventLog::finalize() {
    auto& i = *impl_;
    if (i.finalized) return;

    sync_to_disk(i.main_ofs, i.main_path);
    if (i.ints_ofs.is_open()) sync_to_disk(i.ints_ofs, i.ints_path);
    if (i.floats_ofs.is_open()) sync_to_disk(i.floats_ofs, i.floats_path);

    i.finalized = true;
}

const JsonSplitEventLogStats& JsonSplitEventLog::stats() const {
    return impl_->stats;
}

const std::string& JsonSplitEventLog::main_file() const { return impl_->main_path; }
const std::string& JsonSplitEventLog::ints_file() const { return impl_->ints_path; }
const std::string& JsonSplitEventLog::floats_file() const { return impl_->floats_path; }

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v001
#endif