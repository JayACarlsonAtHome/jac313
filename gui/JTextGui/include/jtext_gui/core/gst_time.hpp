#pragma once

// gst_time.hpp — civil date/time -> GST (Galactic Standard Time), as jText's GST type writes
// it: "<earth_approximate_year>:<ticks>", ticks zero-padded to 19 digits (jText SPEC 2.4.1).
//
// A C++ port of jac314's Earth Gregorian Adapter (jac314 v001/xgox/reference_impl/pkg/gst/
// gregorian_adapter.go CivilUTCToGST, and pkg/gsp gstanchor.go / gsttime.go):
//   UTC --(+ IERS leap seconds)--> TAI --(+32.184 s)--> TT --(+ annual TDB-TT term)--> TDB
// then the exact grid: epoch = year 1, tick 0 = 0001-01-01T00:00:00 TDB; one year =
// 3,155,695,200,000,000,000 ticks of 10 ps (365.2425 days); year N starts at (N-1) years.
// Integer math except the documented boundary fraction (0.184 s, sub-second, TDB-TT term).
// Leap seconds: the IERS table from 1972 (earlier dates clamp to 10 s, as jac314 does).
// Keep in step with jac314: tests/core_tests.cpp checks values computed by jac314's Go code.

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace jtext_gui {

inline constexpr std::uint64_t kGstTicksPerSecond    = 100'000'000'000ULL;           // 10 ps
inline constexpr std::uint64_t kGstTicksPerEarthYear = 3'155'695'200'000'000'000ULL;

struct GstValue {
    std::int64_t  year = 0;   // earth_approximate_year
    std::uint64_t tick = 0;   // 10 ps ticks since the start of that year
};

/// A civil UTC instant (proleptic Gregorian; ns = 0..999,999,999) -> GST.
GstValue civil_utc_to_gst(int year, int month, int day, int hour, int minute, int second, int nanosecond);

/// "2026:0000000000000000001"
std::string format_gst(const GstValue& g);

/// Parse a jText DateTime (strict ISO 8601: "YYYY-MM-DD", "T" or one space, "HH:MM:SS",
/// optional ".f" (1-9 digits), optional "Z" or "+HH:MM"/"-HH:MM"; no zone = UTC) and
/// convert it to GST text. nullopt (and `err`) when the text is not a valid DateTime.
std::optional<std::string> datetime_to_gst_text(std::string_view iso, std::string& err);

/// The current UTC time as a jText DateTime, "YYYY-MM-DD HH:MM:SS.fffZ".
std::string now_utc_datetime();

} // namespace jtext_gui
