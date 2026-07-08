#include "HtmlSplitEventLog.hpp"

#ifndef JAC313_STORE_IMPORT_STD
#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>
#endif

#include <fcntl.h>
#include <unistd.h>

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v002 {
#endif

namespace {

std::string html_escape(std::string_view text) {
    std::string out;
    out.reserve(text.size());
    for (const char c : text) {
        switch (c) {
            case '&':  out += "&amp;";  break;
            case '<':  out += "&lt;";   break;
            case '>':  out += "&gt;";   break;
            case '"':  out += "&quot;"; break;
            case '\'': out += "&#39;";  break;
            default:   out += c;        break;
        }
    }
    return out;
}

void write_html_preamble(std::ostream& os,
                         std::string_view title,
                         const std::vector<std::string>& columns)
{
    os << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n"
       << "<meta charset=\"utf-8\">\n"
       << "<title>" << html_escape(title) << "</title>\n"
       << "<style>\n"
       << "body{font-family:system-ui,sans-serif;margin:1.5rem}\n"
       << "table{border-collapse:collapse;width:100%}\n"
       << "th,td{border:1px solid #ccc;padding:4px 8px;text-align:left}\n"
       << "th{background:#f4f4f4}\n"
       << "tr:nth-child(even){background:#fafafa}\n"
       << "</style>\n</head>\n<body>\n"
       << "<h1>" << html_escape(title) << "</h1>\n"
       << "<table>\n<thead><tr>\n";
    for (const auto& col : columns) {
        os << "<th>" << html_escape(col) << "</th>\n";
    }
    os << "</tr></thead>\n<tbody>\n";
}

void write_html_epilogue(std::ostream& os) {
    os << "</tbody>\n</table>\n</body>\n</html>\n";
}

void sync_to_disk(std::ofstream& ofs, const std::string& path) {
    if (path.empty()) return;
    ofs.flush();
    const int fd = ::open(path.c_str(), O_RDONLY);
    if (fd >= 0) { ::fsync(fd); ::close(fd); }
}

} // namespace

namespace fs = std::filesystem;

struct HtmlSplitEventLog::Impl {
    std::ofstream main_ofs;
    std::ofstream ints_ofs;
    std::ofstream floats_ofs;

    std::string main_path;
    std::string ints_path;
    std::string floats_path;

    PersistMode mode;
    size_t int_count = 0;
    size_t dbl_count = 0;

    HtmlSplitEventLogStats stats;
    bool finalized = false;
    bool body_open = false;
};

HtmlSplitEventLog::HtmlSplitEventLog(
    std::string_view base_name,
    size_t int_count,
    size_t dbl_count,
    PersistMode mode
)
    : impl_(std::make_unique<Impl>())
{
    if (base_name.empty()) {
        throw std::invalid_argument("HtmlSplitEventLog: base_name cannot be empty");
    }

    auto& i = *impl_;
    i.mode = mode;
    i.int_count = int_count;
    i.dbl_count = dbl_count;

    i.main_path = std::format("{}.html", base_name);
    i.ints_path = std::format("{}_Ints.html", base_name);
    i.floats_path = std::format("{}_Floats.html", base_name);

    const std::string table_name = [&] {
        std::string t = fs::path(std::string(base_name)).filename().string();
        return t.empty() ? "persist" : t;
    }();

    auto open_main = [&](std::ofstream& ofs, const std::string& path,
                         std::string_view purpose, std::string_view related,
                         std::string_view title, const std::vector<std::string>& columns) {
        ofs.open(path, std::ios::out | std::ios::trunc);
        if (!ofs.is_open()) {
            throw std::runtime_error("HtmlSplitEventLog: failed to open " + path);
        }
        write_file_comment_header(ofs, path, purpose, related);
        write_html_preamble(ofs, title, columns);
    };

    const std::string main_related = std::format("type=ts_store table={}", table_name);
    open_main(i.main_ofs, i.main_path, "HTML Data File", main_related,
              std::format("{} — main events", table_name),
              {"id", "thread_id", "per_thread_event_id", "flags_raw", "category", "payload", "timestamp_us"});

    if (int_count > 0) {
        std::vector<std::string> cols = {"id"};
        for (size_t k = 0; k < int_count; ++k) {
            cols.emplace_back(std::format("int{}", k));
        }
        const std::string rel = std::format("type=ts_store table={}_ints", table_name);
        open_main(i.ints_ofs, i.ints_path, "HTML Data File", rel,
                  std::format("{} — integer metrics", table_name), cols);
    }

    if (dbl_count > 0) {
        std::vector<std::string> cols = {"id"};
        for (size_t k = 0; k < dbl_count; ++k) {
            cols.emplace_back(std::format("dbl{}", k));
        }
        const std::string rel = std::format("type=ts_store table={}_floats", table_name);
        open_main(i.floats_ofs, i.floats_path, "HTML Data File", rel,
                  std::format("{} — double metrics", table_name), cols);
    }

    i.body_open = true;
}

HtmlSplitEventLog::~HtmlSplitEventLog() {
    if (impl_ && !impl_->finalized) {
        try { finalize(); } catch (...) {}
    }
}

HtmlSplitEventLog::HtmlSplitEventLog(HtmlSplitEventLog&&) noexcept = default;
HtmlSplitEventLog& HtmlSplitEventLog::operator=(HtmlSplitEventLog&&) noexcept = default;

void HtmlSplitEventLog::append_event(
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
    if (!i.body_open) return;

    if (i.mode == PersistMode::KeeperOnly) {
        constexpr uint64_t KEEPER_MASK = 1ULL << 1;
        if ((raw_flags & KEEPER_MASK) == 0) return;
    } else if (i.mode == PersistMode::DatabaseOnly) {
        return;
    }

    i.main_ofs << "<tr>"
               << "<td>" << event_id << "</td>"
               << "<td>" << thread_id << "</td>"
               << "<td>" << per_thread_event_id << "</td>"
               << "<td>0x" << std::format("{:016x}", raw_flags) << "</td>"
               << "<td>" << html_escape(category) << "</td>"
               << "<td>" << html_escape(payload) << "</td>"
               << "<td>" << timestamp_us << "</td>"
               << "</tr>\n";
    i.stats.main_rows++;

    if (i.int_count > 0 && i.ints_ofs.is_open()) {
        i.ints_ofs << "<tr><td>" << event_id << "</td>";
        for (size_t k = 0; k < int_metrics.size(); ++k) {
            i.ints_ofs << "<td>" << int_metrics[k] << "</td>";
        }
        for (size_t k = int_metrics.size(); k < i.int_count; ++k) {
            i.ints_ofs << "<td>0</td>";
        }
        i.ints_ofs << "</tr>\n";
        i.stats.ints_rows++;
    }

    if (i.dbl_count > 0 && i.floats_ofs.is_open()) {
        i.floats_ofs << "<tr><td>" << event_id << "</td>";
        for (size_t k = 0; k < dbl_metrics.size(); ++k) {
            i.floats_ofs << "<td>" << std::format("{:.10g}", dbl_metrics[k]) << "</td>";
        }
        for (size_t k = dbl_metrics.size(); k < i.dbl_count; ++k) {
            i.floats_ofs << "<td>0</td>";
        }
        i.floats_ofs << "</tr>\n";
        i.stats.floats_rows++;
    }
}

void HtmlSplitEventLog::flush() {
    auto& i = *impl_;
    i.main_ofs.flush();
    if (i.ints_ofs.is_open()) i.ints_ofs.flush();
    if (i.floats_ofs.is_open()) i.floats_ofs.flush();
    i.stats.batches_flushed++;
}

void HtmlSplitEventLog::finalize() {
    auto& i = *impl_;
    if (i.finalized) return;

    write_html_epilogue(i.main_ofs);
    if (i.ints_ofs.is_open()) write_html_epilogue(i.ints_ofs);
    if (i.floats_ofs.is_open()) write_html_epilogue(i.floats_ofs);

    sync_to_disk(i.main_ofs, i.main_path);
    if (i.ints_ofs.is_open()) sync_to_disk(i.ints_ofs, i.ints_path);
    if (i.floats_ofs.is_open()) sync_to_disk(i.floats_ofs, i.floats_path);

    i.body_open = false;
    i.finalized = true;
}

const HtmlSplitEventLogStats& HtmlSplitEventLog::stats() const {
    return impl_->stats;
}

const std::string& HtmlSplitEventLog::main_file() const { return impl_->main_path; }
const std::string& HtmlSplitEventLog::ints_file() const { return impl_->ints_path; }
const std::string& HtmlSplitEventLog::floats_file() const { return impl_->floats_path; }

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v002
#endif