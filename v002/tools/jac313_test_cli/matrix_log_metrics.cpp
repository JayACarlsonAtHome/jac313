#include "matrix_log_metrics.hpp"

#include <cctype>
#include <sstream>

namespace jac313::test_cli {

namespace {

std::optional<std::uint64_t> parse_ops_per_sec_line(const std::string& line) {
    if (line.find("ops/sec") == std::string::npos) {
        return std::nullopt;
    }
    const auto arrow = line.find("→");
    if (arrow == std::string::npos) {
        return std::nullopt;
    }

    std::string digits;
    for (char ch : line.substr(arrow + 1)) {
        if (std::isdigit(static_cast<unsigned char>(ch))) {
            digits += ch;
        } else if (ch == ',') {
            continue;
        } else if (!digits.empty()) {
            break;
        }
    }
    if (digits.empty()) {
        return std::nullopt;
    }
    return std::stoull(digits);
}

} // namespace

std::optional<std::uint64_t> peak_ops_from_text(const std::string& text) {
    std::istringstream in(text);
    std::string line;
    std::optional<std::uint64_t> peak;
    while (std::getline(in, line)) {
        if (line.find("Fastest run") == std::string::npos) {
            continue;
        }
        if (const auto ops = parse_ops_per_sec_line(line)) {
            peak = *ops;
        }
    }
    return peak;
}

std::optional<std::uint64_t> persist_log_bytes(const std::filesystem::path& log_dir,
                                               const std::string& persist)
{
    if (!std::filesystem::exists(log_dir) || !std::filesystem::is_directory(log_dir)) {
        return std::nullopt;
    }

    std::uint64_t total = 0;
    bool found = false;

    if (persist == "binary") {
        const std::filesystem::path bin = log_dir / "persist.bin";
        if (std::filesystem::is_regular_file(bin)) {
            return std::filesystem::file_size(bin);
        }
        for (const auto& entry : std::filesystem::directory_iterator(log_dir)) {
            if (!entry.is_regular_file()) {
                continue;
            }
            if (entry.path().extension() == ".bin") {
                total += std::filesystem::file_size(entry.path());
                found = true;
            }
        }
        return found ? std::optional(total) : std::nullopt;
    }

    if (persist == "jtext") {
        for (const auto& entry : std::filesystem::directory_iterator(log_dir)) {
            if (!entry.is_regular_file()) {
                continue;
            }
            const std::string name = entry.path().filename().string();
            if (!name.ends_with(".jtext") || !name.starts_with("persist")) {
                continue;
            }
            total += std::filesystem::file_size(entry.path());
            found = true;
        }
        return found ? std::optional(total) : std::nullopt;
    }

    if (persist == "sql") {
        const std::filesystem::path db = log_dir / "persist.db";
        if (std::filesystem::is_regular_file(db)) {
            total += std::filesystem::file_size(db);
            found = true;
        }
        const std::filesystem::path sql = log_dir / "persist.sql";
        if (std::filesystem::is_regular_file(sql)) {
            total += std::filesystem::file_size(sql);
            found = true;
        }
        return found ? std::optional(total) : std::nullopt;
    }

    return std::nullopt;
}

} // namespace jac313::test_cli