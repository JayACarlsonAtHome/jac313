// ts_store/ts_store_headers/impl_details/diagnostic.hpp
// Clean diagnostics — reports size mismatch and invalid test payloads
// Updated: uses ansi:: colors, centered boxes, safe for large numbers

#pragma once
//#include "test_constants.hpp"

inline void diagnose_failures(size_t max_report = std::numeric_limits<size_t>::max()) const
{
    if (rows_.size() != expected_size()) {
        std::println("{}[DIAGNOSE] SIZE MISMATCH — expected {:>10}, got {:>10}{}",
             ansi::bold_red(), expected_size(), rows_.size(), ansi::reset());
        return;
    }

    struct Failure {
        size_t id;
        size_t thread_id;
        size_t event_id;
        std::string_view payload;
        std::string_view expected;
    };

    std::vector<Failure> failures;
    failures.reserve(std::min(rows_.size(), max_report));

    for (size_t id = 0; id < rows_.size(); ++id) {
        if (failures.size() >= max_report) break;

        const auto& row = rows_[id];
        std::string_view payload  = row.value_storage.view();
        std::string_view expected = test_messages[row.event_id % test_messages.size()];

        bool valid = false;
        if (payload == expected)  valid = true;
        if (!valid) {
            failures.emplace_back(id, row.thread_id, row.event_id, row.value_storage.view(), expected);
        }
    }

    std::sort(failures.begin(), failures.end(),
              [](const auto& a, const auto& b) { return a.id < b.id; });

    const size_t w = thread_id_width();
    const size_t e = events_id_width();

    // Print individual failures
    for (const auto& f : failures) {

        std::print(  "{}[DIAGNOSE] ID: {:>10}{}{} | {}",     ansi::bold_red(), f.id,           ansi::reset(), ansi::gray(), ansi::reset() );
        std::print(  "{}[DIAGNOSE] Thread: {:>{}}{}{} | {}", ansi::magenta(),  f.thread_id, w, ansi::reset(), ansi::gray(), ansi::reset() );
        std::print(  "{}[DIAGNOSE] Event:  {:>{}}{}{} | {}", ansi::magenta(),  f.event_id,  e, ansi::reset(), ansi::gray(), ansi::reset() );
        std::println("{}     Actual   (len {:>4} Payload: {}{})", ansi::yellow(),  f.payload.size(), f.payload,  ansi::reset() );
        std::println("{}     Expected (len {:>4} Payload: {}{})", ansi::yellow(),  f.payload.size(), f.expected, ansi::reset() );
        std::println("");
    }

    // Summary box — centered and safe for large numbers
    constexpr int box_width = 79;  // inner width between ║ — adjust ±2 if alignment feels off

    if (failures.empty()) {
        std::string msg = std::format("ALL {} ENTRIES PASS DIAGNOSTICS", expected_size());
        std::print("{}╔═══════════════════════════════════════════════════════════════════════════════╗{}",ansi::bold_green(), ansi::reset());
        std::print("{}{:^{}}{}",ansi::bold_green(), msg, box_width, ansi::reset());
        std::print("{}╚═══════════════════════════════════════════════════════════════════════════════╝{}",ansi::bold_green(), ansi::reset());

    } else {
        std::string header = "CORRUPTED TEST PAYLOADS";
        std::string report = std::format("REPORTED {} TEST PAYLOAD FAILURE(S)", failures.size());
        std::print("{}╔═══════════════════════════════════════════════════════════════════════════════╗{}",ansi::bold_red(), ansi::reset());
        std::print("{}{:^{}}{}",ansi::bold_red(), header, box_width, ansi::reset());
        std::print("{}{:^{}}{}",ansi::bold_red(), report, box_width, ansi::reset());
        std::print("{}╚═══════════════════════════════════════════════════════════════════════════════╝{}",ansi::bold_red(), ansi::reset());
    }
}