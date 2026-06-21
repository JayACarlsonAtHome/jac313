// format_locale.hpp — locale-aware integer formatting for CLI and benchmark output.
//
// Priority:
//   1. TS_STORE_THOUSANDS_SEP  (single char: ',' or '.')
//   2. TS_STORE_NUMBER_LOCALE / system LC_NUMERIC / LANG → std::locale("")
//   3. Comma grouping by default (readable US-style output)

#pragma once

#ifndef JAC313_STORE_IMPORT_STD
#include <cstdint>
#include <cstdlib>
#include <locale>
#include <sstream>
#include <string>
#endif

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v002 {
#endif

namespace detail {

inline std::string format_with_separator(std::uint64_t magnitude, char sep) {
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

inline bool env_set(const char* name) {
    const char* value = std::getenv(name);
    return value != nullptr && value[0] != '\0';
}

inline bool use_system_number_locale() {
    return env_set("TS_STORE_NUMBER_LOCALE")
        || env_set("LC_NUMERIC")
        || env_set("LANG");
}

inline std::locale number_locale() {
    if (const char* name = std::getenv("TS_STORE_NUMBER_LOCALE")) {
        if (name[0] != '\0') {
            try {
                return std::locale(name);
            } catch (...) {
            }
        }
    }
    try {
        return std::locale("");
    } catch (...) {
        return std::locale::classic();
    }
}

inline std::string format_with_locale(std::int64_t value) {
    std::ostringstream oss;
    oss.imbue(number_locale());
    oss << value;
    return oss.str();
}

inline std::string format_with_locale(std::uint64_t value) {
    std::ostringstream oss;
    oss.imbue(number_locale());
    oss << value;
    return oss.str();
}

}  // namespace detail

inline std::string format_locale_int(std::int64_t value) {
    if (const char* sep_env = std::getenv("TS_STORE_THOUSANDS_SEP")) {
        const char sep = sep_env[0];
        if (sep != '\0' && sep_env[1] == '\0') {
            if (value < 0) {
                return "-" + detail::format_with_separator(
                    static_cast<std::uint64_t>(-value), sep);
            }
            return detail::format_with_separator(
                static_cast<std::uint64_t>(value), sep);
        }
    }
    if (detail::use_system_number_locale()) {
        return detail::format_with_locale(value);
    }
    if (value < 0) {
        return "-" + detail::format_with_separator(
            static_cast<std::uint64_t>(-value), ',');
    }
    return detail::format_with_separator(static_cast<std::uint64_t>(value), ',');
}

inline std::string format_locale_int(std::uint64_t value) {
    if (const char* sep_env = std::getenv("TS_STORE_THOUSANDS_SEP")) {
        const char sep = sep_env[0];
        if (sep != '\0' && sep_env[1] == '\0') {
            return detail::format_with_separator(value, sep);
        }
    }
    if (detail::use_system_number_locale()) {
        return detail::format_with_locale(value);
    }
    return detail::format_with_separator(value, ',');
}

inline std::string format_locale_int(int value) {
    return format_locale_int(static_cast<std::int64_t>(value));
}

inline std::string format_locale_int(long long value) {
    return format_locale_int(static_cast<std::int64_t>(value));
}

#ifndef JAC313_STORE_IMPORT_STD
}  // namespace jac313::Store::v002
#endif