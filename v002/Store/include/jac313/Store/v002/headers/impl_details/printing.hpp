// ts_store/ts_store_headers/impl_details/printing.hpp
// Updated for std::string-based storage (dynamic allocation)
// Converted to std::print / std::println (C++23) — no std::ostream parameter

#pragma once

#ifndef JAC313_STORE_IMPORT_STD
#include <print>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <cctype>
#endif

// (assume ansi:: namespace and Config / row_data types are defined elsewhere)
private:
struct ColumnWidths {
    size_t id;
    size_t time;
    size_t severity;
    size_t category;
    size_t thread;
    size_t event;
    size_t payload;
    size_t total() const {
        return id + time + severity + category + thread + event + payload + 6 * 3; // 6 gaps × 3 spaces
    }
};

ColumnWidths set_effective_widths() const
{
    ColumnWidths w;

    w.id       = std::max(id_width(),      3ul);
    w.time     = std::max(Config::use_timestamps ? 18ul : 10ul, 10ul);
    w.severity = StoreFlags::get_severity_string_width();
    w.category = std::max(Config::max_category_length, 8ul);
    w.thread   = std::max(thread_id_width(),           6ul);
    w.event    = std::max(events_id_width(),           5ul);
    w.payload  = Config::max_payload_length;
    return w;
}

void print_table_separator_line(const ColumnWidths& w) const {
        jac313_rt_println("{}{:=>{}}{}", ansi::dim(), "", w.total(), ansi::reset());
}

void print_table_header(const ColumnWidths& w, std::string_view space_pad) const
{
    std::string h_severity = "SEVERITY"; h_severity.resize(w.severity, '.');
    std::string h_cat      = "CATEGORY"; h_cat.resize(w.category,      '.');
    std::string h_payload  = "PAYLOAD";  h_payload.resize(w.payload,   '.');

    jac313_rt_print("{}{:>{}}{}", ansi::bold(),         "ID",          w.id,      space_pad);
    jac313_rt_print("{:>{}}{}",                       "TIME (µs)",   w.time,    space_pad);
    jac313_rt_print("{:>{}}{}",   h_severity,         w.severity,    space_pad);
    jac313_rt_print("{:>{}}{}",   h_cat,              w.category,    space_pad);
    jac313_rt_print("{:>{}}{}",   "Thread",           w.thread,      space_pad);
    jac313_rt_print("{:>{}}{}",   "Event",            w.event,       space_pad);
    jac313_rt_print("{:<{}}{}",   h_payload,          w.payload,     space_pad);

    std::println("");
}

void print_single_row(size_t id, const row_data& r,
                      const ColumnWidths& widths,
                      std::string_view space_pad ) const
{
    // ID
    jac313_rt_print("{}{:>{}}{}", ansi::cyan(), id, widths.id, space_pad);

    // TIME
    if constexpr (Config::use_timestamps) {
        jac313_rt_print("{}{:>{}}{}", ansi::yellow(), r.ts_us, widths.time, space_pad);
    } else {
        jac313_rt_print("{:>{}}{}", "-", widths.time, space_pad);
    }

    // TYPE (padded)
    StoreFlags row_flags(r.event_flags);
    std::string severity = row_flags.get_severity_string();
    std::string severity_padded = severity;
    severity_padded.resize(widths.severity, '.');
    jac313_rt_print("{}{:<{}}{}", ansi::bold_magenta(), severity_padded, widths.severity, space_pad);

    // CATEGORY (padded)
    std::string cat_padded{ r.category_storage.view() };
    cat_padded.resize(widths.category, '.');
    jac313_rt_print("{}{:<{}}{}", ansi::bold_yellow(), cat_padded, widths.category, space_pad);

    // Thread & Event
    jac313_rt_print("{}{:>{}}{}", ansi::magenta(),      r.thread_id, widths.thread,  space_pad);
    jac313_rt_print("{}{:>{}}{}", ansi::bold_green(),   r.event_id,  widths.event,   space_pad);

    // PAYLOAD (padded)
    std::string payload_padded{ r.value_storage.view() };
    payload_padded.resize(widths.payload, '.');
    jac313_rt_print("{}{:<{}}", ansi::blue(), payload_padded, widths.payload);

    std::print(" FLAGS: {} ", row_flags.to_string());
    std::println();
}

void print_summary(size_t total, size_t rows_printed, bool early_exit) const
{
    if (early_exit || rows_printed < total) {
        std::println("{}Output stopped after {} rows (of total {}).{}",
                     ansi::dim(), rows_printed, total, ansi::reset());
    }

    std::println("{}Note: Output is zero-indexed → last printed entry is ID {}. Expected last ID = {}.{}",
                 ansi::dim(), rows_printed > 0 ? rows_printed - 1 : 0, expected_size() - 1, ansi::reset());

    std::println("{}Total entries stored: {} (expected: {}){}",
                 ansi::dim(), total, expected_size(), ansi::reset());
}

public:
void debug_print_widths() const {
    auto w = set_effective_widths();
    std::cout << "  id     = " << w.id << "\n"
              << "  time   = " << w.time << "\n"
              << "  type   = " << w.type << "\n"
              << "  cat    = " << w.category << "\n"
              << "  thread = " << w.thread << "\n"
              << "  event  = " << w.event << "   ← ← ← this is usually the problem\n"
              << "  payload= " << w.payload << "\n"
              << "  total  ≈ " << w.total() << "\n"
              << "  events_per_thread_ raw = " << get_max_events() << "\n"
              << "  expected_size      = " << expected_size() << "\n\n";
}

//================================================================================================
//
//  Main print function is below, everything above is just helper functions.
//
//================================================================================================

inline void print(size_t max_rows = 10'000) const
{
    std::string space_pad(3, ' ');
    if (rows_.empty()) {
        std::println("ts_store is empty.\n");
        return;
    }

    const auto widths = set_effective_widths();

    if (max_rows == 0) {
        max_rows = std::numeric_limits<size_t>::max();
    }

    std::vector<size_t> ids;
    ids.reserve(rows_.size());
    for (size_t i = 0; i < rows_.size(); ++i) {
        ids.push_back(i);
    }

    //const size_t total = ids.size();



    std::println("{}ts_store <{}", ansi::bold_white(), ansi::reset());
    std::println("   Threads    = {}", get_max_threads());
    std::println("   Events     = {}", get_max_events());
    std::println("   ValueT     = bounded_string<{}> (fixed, max {} codepoints)", Config::max_payload_length, Config::max_payload_length);
    std::println("   Time Stamp = {}>", Config::use_timestamps ? "On" : "Off");


    print_table_separator_line(widths);
    print_table_header(widths, space_pad);

    constexpr size_t PAUSE_EVERY = 1000;
    size_t batch_start_row = 0;
    size_t rows_printed = 0;
    size_t pause_count = 0;

    bool early_exit = false;
    bool bResult1 = false;
    bool bResult2 = false;
    bool bResult3 = false;
    bool bResult4 = false;

    for (size_t i = 0; i < ids.size() && rows_printed < max_rows; ++i)  {
        size_t row_id = ids[i];
        const auto& row = rows_[row_id];
        print_single_row(row_id, row, widths, space_pad);
        ++rows_printed;
        ++pause_count;

        bResult1 = (pause_count >= PAUSE_EVERY);
        bResult2 = (rows_printed + 1 < max_rows);
        bResult3 = (rows_printed + 1 < ids.size());
        bResult4 = (rows_printed > batch_start_row);

        if (bResult1 && bResult2 && bResult3 && bResult4)   {
            std::print("{}--- Paused after {} rows printed (rows {} to {}) --- press ENTER to continue, Q to quit, E to jump to end ---{}",
                   ansi::dim(), rows_printed, batch_start_row, rows_printed - 1, ansi::reset());

            char choice = press_any_key();

            if (choice == 'Q') {
                early_exit = true;
                std::println("{}Early exit requested.{}", ansi::dim(), ansi::reset());
                break;  // exit the for-loop immediately
            }
            else if (choice == 'E') {
                std::println("{}Jumping to last {} rows...{}", ansi::dim(), PAUSE_EVERY, ansi::reset());
                rows_printed = ids.size() - PAUSE_EVERY;
                i = rows_printed - 1;
            }

            pause_count = 0;
            batch_start_row = rows_printed;
        }
    }

    print_table_separator_line(widths);



    print_summary(ids.size(), rows_printed, early_exit);
}