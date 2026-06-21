#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <type_traits>

namespace jac313::test_cli {

inline std::string format_with_commas(std::uint64_t magnitude, char sep = ',') {
    const std::string digits = std::to_string(magnitude);
    const std::size_t n = digits.size();
    const std::size_t first_group = (n - 1) % 3 + 1;

    std::string out;
    out.reserve(n + n / 3);
    out.append(digits, 0, first_group);
    for (std::size_t i = first_group; i < n; i += 3) {
        out += sep;
        out.append(digits, i, 3);
    }
    return out;
}

inline std::string format_count(std::int64_t value) {
    if (value < 0) {
        return "-" + format_with_commas(static_cast<std::uint64_t>(-value));
    }
    return format_with_commas(static_cast<std::uint64_t>(value));
}

template<typename T>
inline std::string format_count(T value) {
    if constexpr (std::is_signed_v<T>) {
        return format_count(static_cast<std::int64_t>(value));
    } else {
        return format_with_commas(static_cast<std::uint64_t>(value));
    }
}

inline std::string format_duration_human(int ms) {
    if (ms < 0) {
        ms = 0;
    }
    const std::string precise = format_count(ms) + " ms";
    if (ms < 1'000) {
        return precise;
    }

    const int total_sec = ms / 1'000;
    const int hours = total_sec / 3'600;
    const int minutes = (total_sec % 3'600) / 60;
    const int seconds = total_sec % 60;

    std::string human;
    if (hours > 0) {
        human += format_count(hours) + 'h';
    }
    if (minutes > 0) {
        if (!human.empty()) {
            human += ' ';
        }
        human += format_count(minutes) + 'm';
    }
    if (seconds > 0 || human.empty()) {
        if (!human.empty()) {
            human += ' ';
        }
        human += format_count(seconds) + 's';
    }
    return precise + " (" + human + ")";
}

inline std::string format_bytes(std::uint64_t bytes) {
    if (bytes < 1'024) {
        return format_count(bytes) + " B";
    }
    if (bytes < 1'024 * 1'024) {
        return std::format("{:.2f} KiB", static_cast<double>(bytes) / 1'024.0);
    }
    return std::format("{:.2f} MiB", static_cast<double>(bytes) / (1'024.0 * 1'024.0));
}

} // namespace jac313::test_cli