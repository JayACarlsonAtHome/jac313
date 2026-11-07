// Store's jText log writes its rows from views of number texts it formats itself, and reserves
// each writer's row buffer from the row limits (JTextRowLimits) once, at construction: rows
// within the limits never grow it. The three files hold exactly the expected rows.

#include <jac313/Store/v001/headers/persistence/JTextSplitEventLog.hpp>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <source_location>
#include <string>
#include <string_view>

namespace fs = std::filesystem;
namespace row_bytes = jac313::Store::v001::jtext_row_bytes;
using jac313::Store::v001::JTextRowLimits;
using jac313::Store::v001::JTextSplitEventLog;
using jac313::Store::v001::PersistMode;

// The row sizes, at compile time: " 1. " + the two formatter characters + ' ' + the fields with
// a separator between each two + '\n'.
static_assert(row_bytes::row(1, 1) == std::string_view(" 1. \xE2\x96\xAE\xE2\x96\x8C 7\n").size());
static_assert(row_bytes::row(2, 2) == std::string_view(" 1. \xE2\x96\xAE\xE2\x96\x8C 7\xE2\x96\xAE" "8\n").size());
static_assert(row_bytes::main_row(JTextRowLimits{}) == 0, "unknown limits: no reservation");
static_assert(row_bytes::main_row(JTextRowLimits{8, 0}) == 0, "an unknown payload limit: no reservation");
static_assert(row_bytes::main_row(JTextRowLimits{40, 100}) == row_bytes::row(7, 4 * 20 + 18 + 40 + 100));
static_assert(row_bytes::ints_row(2) == row_bytes::row(3, 3 * 20));
static_assert(row_bytes::floats_row(3) == row_bytes::row(4, 20 + 3 * 24));
static_assert(row_bytes::kInteger == std::string_view("-9223372036854775808").size());
static_assert(row_bytes::kInteger == std::string_view("18446744073709551615").size());

// A failed check says what and where, and ends the test.
void check(bool ok, std::string_view what, std::source_location at = std::source_location::current()) {
    if (ok) return;
    std::cerr << "FAIL: " << what << " at " << at.file_name() << ":" << at.line() << "\n";
    std::exit(EXIT_FAILURE);
}

std::string read_file(const std::string& path) {
    std::ifstream in(path);
    return {std::istreambuf_iterator<char>(in), {}};
}

// The Data block of a log file, its rows only.
std::string data_rows(const std::string& path) {
    const std::string text = read_file(path);
    constexpr std::string_view open = "=== Data ===\n", close = "=== End Data ===\n";
    const auto from = text.find(open);
    const auto to = text.find(close);
    check(from != std::string::npos && to != std::string::npos && from < to, "a Data block in " + path);
    return text.substr(from + open.size(), to - from - open.size());
}

void check_double_text_size() {
    // The longest double texts are exactly kDoubleTextChars.
    for (const double v : {-2.2250738585072014e-308, -1.7976931348623157e+308}) {
        std::string t;
        jac313::Store::v001::append_double(t, v);
        check(t.size() == jac313::Store::v001::kDoubleTextChars, "the longest double text: " + t);
    }
}

int main() {
    check_double_text_size();

    const fs::path dir = fs::temp_directory_path() / "jac313_store_jtext_row_limits_test";
    fs::remove_all(dir);
    fs::create_directories(dir);
    const std::string base = (dir / "limits").string();

    constexpr JTextRowLimits limits{40, 100};
    constexpr size_t max_main_row = row_bytes::main_row(limits);
    using I = std::numeric_limits<int64_t>;
    using D = std::numeric_limits<double>;
    const std::string long_category(limits.max_category_bytes, 'c');
    const std::string long_payload(limits.max_payload_bytes, 'p');
    const size_t max_id = std::numeric_limits<size_t>::max();
    const uint64_t max_u64 = std::numeric_limits<uint64_t>::max();
    {
        JTextSplitEventLog log(base, 2, 3, PersistMode::All, limits);
        const size_t capacity = log.main_batch_capacity();
        check(capacity >= max_main_row, "the main row buffer is reserved for the longest row");

        const std::array<int64_t, 2> ints1{-5, I::max()};
        const std::array<double, 3> dbls1{D::quiet_NaN(), -0.0, 1.5};
        log.append_event(1, 2, 3, 2, "cat", "pay load", 1000, ints1, dbls1);
        const std::array<int64_t, 2> ints2{I::min(), 0};
        const std::array<double, 3> dbls2{D::infinity(), -D::infinity(), 5e-324};
        log.append_event(max_id, max_id, max_id, max_u64, long_category, long_payload, max_u64, ints2, dbls2);
        const std::array<int64_t, 2> ints3{7, 8};
        const std::array<double, 3> dbls3{0.1, 1e308, -2.2250738585072014e-308};
        log.append_event(3, 0, 0, 0, "a|b # c", "\xE2\x96\xAE", 3000, ints3, dbls3);
        log.append_event(4, 0, 1, 0, "", "", 4000, ints3, dbls3);

        check(log.main_batch_capacity() == capacity, "rows within the limits never grow the buffer");
        log.finalize();
    }

    const std::string longest_row = " 1. \xE2\x96\xAE\xE2\x96\x8C 18446744073709551615\xE2\x96\xAE"
                                    "18446744073709551615\xE2\x96\xAE" "18446744073709551615\xE2\x96\xAE"
                                    "0xffffffffffffffff\xE2\x96\xAE" + long_category + "\xE2\x96\xAE" +
                                    long_payload + "\xE2\x96\xAE" "18446744073709551615\n";
    check(longest_row.size() == max_main_row, "the longest main row is exactly max_main_row bytes");

    check(data_rows(base + ".jtext") ==
              " 1. \xE2\x96\xAE\xE2\x96\x8C 1\xE2\x96\xAE" "2\xE2\x96\xAE" "3\xE2\x96\xAE"
              "0x0000000000000002\xE2\x96\xAE" "cat\xE2\x96\xAE" "pay load\xE2\x96\xAE" "1000\n" +
              longest_row +
              " 1. ^\xE2\x96\x8C 3^0^0^0x0000000000000000^a|b # c^\xE2\x96\xAE^3000\n"
              " 1. \xE2\x96\xAE\xE2\x96\x8C 4\xE2\x96\xAE" "0\xE2\x96\xAE" "1\xE2\x96\xAE"
              "0x0000000000000000\xE2\x96\xAE\xE2\x96\xAE\xE2\x96\xAE" "4000\n",
          "the main rows");
    check(data_rows(base + "_Ints.jtext") ==
              " 1. \xE2\x96\xAE\xE2\x96\x8C 1\xE2\x96\xAE-5\xE2\x96\xAE" "9223372036854775807\n"
              " 1. \xE2\x96\xAE\xE2\x96\x8C 18446744073709551615\xE2\x96\xAE-9223372036854775808\xE2\x96\xAE" "0\n"
              " 1. \xE2\x96\xAE\xE2\x96\x8C 3\xE2\x96\xAE" "7\xE2\x96\xAE" "8\n"
              " 1. \xE2\x96\xAE\xE2\x96\x8C 4\xE2\x96\xAE" "7\xE2\x96\xAE" "8\n",
          "the _Ints rows");
    check(data_rows(base + "_Floats.jtext") ==
              " 1. \xE2\x96\xAE\xE2\x96\x8C 1\xE2\x96\xAENaN\xE2\x96\xAE-0\xE2\x96\xAE" "1.5\n"
              " 1. \xE2\x96\xAE\xE2\x96\x8C 18446744073709551615\xE2\x96\xAEInfinity\xE2\x96\xAE-Infinity\xE2\x96\xAE"
              "5e-324\n"
              " 1. \xE2\x96\xAE\xE2\x96\x8C 3\xE2\x96\xAE" "0.1\xE2\x96\xAE" "1e+308\xE2\x96\xAE-2.2250738585072014e-308\n"
              " 1. \xE2\x96\xAE\xE2\x96\x8C 4\xE2\x96\xAE" "0.1\xE2\x96\xAE" "1e+308\xE2\x96\xAE-2.2250738585072014e-308\n",
          "the _Floats rows");

    fs::remove_all(dir);
    std::cout << "Store jText row limits: rows from views, the row buffer reserved once and never grown\n";
    return EXIT_SUCCESS;
}
