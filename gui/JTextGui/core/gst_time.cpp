#include "jtext_gui/core/gst_time.hpp"

#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <numbers>

namespace jtext_gui {

namespace {

// Integer Julian Day Number of a proleptic-Gregorian date (Fliegel-Van Flandern).
std::int64_t gregorian_jdn(std::int64_t y, std::int64_t m, std::int64_t d) {
    const std::int64_t a = (14 - m) / 12;
    const std::int64_t yy = y + 4800 - a;
    const std::int64_t mm = m + 12 * a - 3;
    return d + (153 * mm + 2) / 5 + 365 * yy + yy / 4 - yy / 100 + yy / 400 - 32045;
}

// The inverse: JDN -> proleptic-Gregorian date.
void jdn_to_gregorian(std::int64_t jdn, std::int64_t& y, std::int64_t& m, std::int64_t& d) {
    const std::int64_t a = jdn + 32044;
    const std::int64_t b = (4 * a + 3) / 146097;
    const std::int64_t c = a - 146097 * b / 4;
    const std::int64_t dd = (4 * c + 3) / 1461;
    const std::int64_t e = c - 1461 * dd / 4;
    const std::int64_t mm = (5 * e + 2) / 153;
    d = e - (153 * mm + 2) / 5 + 1;
    m = mm + 3 - 12 * (mm / 10);
    y = 100 * b + dd - 4800 + mm / 10;
}

struct LeapStep { int y, m, d; std::int64_t secs; };

// IERS TAI-UTC from the 1972 integer-second era onward (jac314 gregorian_adapter.go leapTable).
constexpr std::array<LeapStep, 28> kLeapTable{{
    {1972, 1, 1, 10}, {1972, 7, 1, 11}, {1973, 1, 1, 12}, {1974, 1, 1, 13}, {1975, 1, 1, 14},
    {1976, 1, 1, 15}, {1977, 1, 1, 16}, {1978, 1, 1, 17}, {1979, 1, 1, 18}, {1980, 1, 1, 19},
    {1981, 7, 1, 20}, {1982, 7, 1, 21}, {1983, 7, 1, 22}, {1985, 7, 1, 23}, {1988, 1, 1, 24},
    {1990, 1, 1, 25}, {1991, 1, 1, 26}, {1992, 7, 1, 27}, {1993, 7, 1, 28}, {1994, 7, 1, 29},
    {1996, 1, 1, 30}, {1997, 7, 1, 31}, {1999, 1, 1, 32}, {2006, 1, 1, 33}, {2009, 1, 1, 34},
    {2012, 7, 1, 35}, {2015, 7, 1, 36}, {2017, 1, 1, 37},
}};

std::int64_t tai_minus_utc(std::int64_t utc_jdn) {
    std::int64_t dat = 10;
    for (const auto& s : kLeapTable) {
        if (gregorian_jdn(s.y, s.m, s.d) <= utc_jdn) dat = s.secs;
        else break;
    }
    return dat;
}

__extension__ typedef __int128 int128;   // ticks since the epoch reach ~6.3e21 (> int64)

constexpr std::int64_t kJ2000Jdn = 2451545;
// Ticks from the GST epoch (0001-01-01T00:00 TDB) to J2000.0 (2000-01-01T12:00 TDB):
// (730,119 days * 86,400 + 43,200 s) * 1e11 (jac314 gsp/gstanchor.go).
constexpr int128 kJ2000TicksSinceEpoch =
    static_cast<int128>(730119LL * 86400LL + 43200LL) * static_cast<int128>(kGstTicksPerSecond);

bool digits(std::string_view s, std::size_t at, std::size_t n, int& out) {
    if (at + n > s.size()) return false;
    out = 0;
    for (std::size_t i = at; i < at + n; ++i) {
        if (s[i] < '0' || s[i] > '9') return false;
        out = out * 10 + (s[i] - '0');
    }
    return true;
}

} // namespace

GstValue civil_utc_to_gst(int year, int month, int day, int hour, int minute, int second, int nanosecond) {
    // jac314 GregorianUTCToTDBSecondsSinceJ2000, step for step (same doubles, same rounding)
    const std::int64_t jdn = gregorian_jdn(year, month, day);
    const std::int64_t sec_noon = (jdn - kJ2000Jdn) * 86400 + std::int64_t(hour - 12) * 3600
                                  + std::int64_t(minute) * 60 + second;
    const std::int64_t tt_whole = sec_noon + tai_minus_utc(jdn) + 32;
    double frac = 0.184 + static_cast<double>(nanosecond) * 1e-9;
    const double days = static_cast<double>(tt_whole) / 86400.0;
    const double g = (357.53 + 0.98560028 * days) * std::numbers::pi / 180.0;
    frac += 0.001657 * std::sin(g);
    const auto carry = static_cast<std::int64_t>(std::floor(frac));
    frac -= static_cast<double>(carry);
    std::int64_t whole = tt_whole + carry;
    auto sub = static_cast<std::uint64_t>(std::round(frac * static_cast<double>(kGstTicksPerSecond)));
    if (sub >= kGstTicksPerSecond) { sub -= kGstTicksPerSecond; ++whole; }

    // jac314 GSTAtTDBSecondsSinceJ2000 + TAITicksToGST (Euclidean division)
    const int128 total = kJ2000TicksSinceEpoch + static_cast<int128>(whole) * kGstTicksPerSecond + sub;
    const int128 y = static_cast<int128>(kGstTicksPerEarthYear);
    int128 q = total / y, r = total % y;
    if (r < 0) { r += y; --q; }
    return {static_cast<std::int64_t>(1 + q), static_cast<std::uint64_t>(r)};
}

std::string format_gst(const GstValue& g) {
    char buf[64];
    std::snprintf(buf, sizeof buf, "%lld:%019llu", static_cast<long long>(g.year),
                  static_cast<unsigned long long>(g.tick));
    return buf;
}

std::optional<std::string> datetime_to_gst_text(std::string_view s, std::string& err) {
    // YYYY-MM-DD[T ]HH:MM:SS[.f{1,9}][Z|+HH:MM|-HH:MM]
    int y, mo, d, h, mi, se;
    if (!digits(s, 0, 4, y) || s.size() < 19 || s[4] != '-' || !digits(s, 5, 2, mo) || s[7] != '-'
        || !digits(s, 8, 2, d) || (s[10] != 'T' && s[10] != ' ') || !digits(s, 11, 2, h) || s[13] != ':'
        || !digits(s, 14, 2, mi) || s[16] != ':' || !digits(s, 17, 2, se)) {
        err = "write it as YYYY-MM-DD HH:MM:SS (optional .fraction and Z or +HH:MM)";
        return std::nullopt;
    }
    std::size_t at = 19;
    int ns = 0;
    if (at < s.size() && s[at] == '.') {
        std::size_t n = 0;
        ++at;
        while (at < s.size() && s[at] >= '0' && s[at] <= '9') {
            if (++n > 9) { err = "at most 9 fraction digits"; return std::nullopt; }
            ns = ns * 10 + (s[at++] - '0');
        }
        if (n == 0) { err = "a '.' needs fraction digits"; return std::nullopt; }
        for (std::size_t k = n; k < 9; ++k) ns *= 10;
    }
    int offset_min = 0;
    if (at < s.size()) {
        if (s[at] == 'Z' && at + 1 == s.size()) {
            ++at;
        } else if ((s[at] == '+' || s[at] == '-') && at + 6 == s.size() && s[at + 3] == ':') {
            int oh, om;
            if (!digits(s, at + 1, 2, oh) || !digits(s, at + 4, 2, om) || oh > 23 || om > 59) {
                err = "the zone must be +HH:MM or -HH:MM";
                return std::nullopt;
            }
            offset_min = (oh * 60 + om) * (s[at] == '-' ? -1 : 1);
            at += 6;
        } else {
            err = "unexpected text after the time";
            return std::nullopt;
        }
    }
    // a real calendar date and time of day
    std::int64_t cy, cm, cd;
    jdn_to_gregorian(gregorian_jdn(y, mo, d), cy, cm, cd);
    if (mo < 1 || mo > 12 || cy != y || cm != mo || cd != d) { err = "not a real calendar date"; return std::nullopt; }
    if (h > 23 || mi > 59 || se > 59) { err = "hours 00-23, minutes and seconds 00-59"; return std::nullopt; }

    // local time - offset = UTC (whole seconds; the date may change)
    std::int64_t secs = gregorian_jdn(y, mo, d) * 86400 + h * 3600 + mi * 60 + se - std::int64_t(offset_min) * 60;
    std::int64_t jdn = secs / 86400, sod = secs % 86400;
    if (sod < 0) { sod += 86400; --jdn; }
    std::int64_t uy, um, ud;
    jdn_to_gregorian(jdn, uy, um, ud);
    const GstValue g = civil_utc_to_gst(static_cast<int>(uy), static_cast<int>(um), static_cast<int>(ud),
                                        static_cast<int>(sod / 3600), static_cast<int>(sod / 60 % 60),
                                        static_cast<int>(sod % 60), ns);
    return format_gst(g);
}

std::string now_utc_datetime() {
    using namespace std::chrono;
    const auto now = system_clock::now();
    const auto day = floor<days>(now);
    const year_month_day ymd{day};
    const hh_mm_ss tod{floor<milliseconds>(now - day)};
    char buf[64];
    std::snprintf(buf, sizeof buf, "%04d-%02u-%02u %02lld:%02lld:%02lld.%03lldZ", static_cast<int>(ymd.year()),
                  static_cast<unsigned>(ymd.month()), static_cast<unsigned>(ymd.day()),
                  static_cast<long long>(tod.hours().count()), static_cast<long long>(tod.minutes().count()),
                  static_cast<long long>(tod.seconds().count()), static_cast<long long>(tod.subseconds().count()));
    return buf;
}

} // namespace jtext_gui
