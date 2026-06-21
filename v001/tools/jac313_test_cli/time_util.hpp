#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace jac313::test_cli {

inline std::string utc_now_formatted(const char* format) {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    gmtime_r(&t, &tm);
    std::ostringstream out;
    out << std::put_time(&tm, format);
    return out.str();
}

inline std::string utc_now_iso() {
    return utc_now_formatted("%Y-%m-%dT%H:%M:%SZ");
}

inline std::string utc_now_manifest() {
    return utc_now_formatted("%Y-%m-%d %H:%M:%SZ");
}

} // namespace jac313::test_cli