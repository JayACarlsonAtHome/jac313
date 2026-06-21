// ts_store/ts_store_headers/impl_details/verify_checks.hpp
// Rewritten for std::format + ANSI colors (C++20 compatible, no fmt dependency)

#pragma once


[[nodiscard]] inline bool verify_level01() const {

    const size_t current = rows_.size();
    const size_t expected = expected_size();

    if (current != expected || next_id_.load(std::memory_order_acquire) != expected) {
        std::cout << ansi::bold() << ansi::red()
                  << std::format("     [VERIFY] SIZE/MISMATCH — entries {} (expected {}), next_id {} (expected {})\n",
                                 current, expected, next_id_.load(), expected)
                  << ansi::reset();
        return false;
    }

    std::vector<std::vector<bool>> seen(max_threads_, std::vector<bool>(events_per_thread_, false));

    for (size_t id = 0; id < rows_.size(); ++id) {
        const auto& row = rows_[id];
        if (row.thread_id >= max_threads_ || row.event_id >= events_per_thread_) {
            std::cout  << ansi::bold() << ansi::red()
                       << std::format("[VERIFY] OUT-OF-RANGE — thread_id {} or event_id {} at ID {}\033[0m\n",
                                     row.thread_id, row.event_id, id)
                       << ansi::reset();
            return false;
        }
        if (seen[row.thread_id][row.event_id]) {
            std::cout  << ansi::bold() << ansi::red()
                       << std::format("[VERIFY] DUPLICATE (thread {}, event {}) at ID {}\n", row.thread_id, row.event_id, id)
                       << ansi::reset();
            return false;
        }
        seen[row.thread_id][row.event_id] = true;
    }

    std::cout  << ansi::green() << std::format("[VERIFY] ALL {} ENTRIES STRUCTURALLY PERFECT — UNIQUE PAIRS, FULL COVERAGE\n", expected) << ansi::reset();
    return true;
}


[[nodiscard]] inline bool verify_level02() const {

    bool all_good = true;

    for (size_t id = 0; id < rows_.size(); ++id) {
        const auto& row = rows_[id];
        bool valid = false;
        std::string pay_load{ row.value_storage.view() };
        std::string expected = Config::utf8_truncate(test_messages[row.event_id % test_messages.size()], Config::max_payload_length);
        size_t utf8_xactualx_len = Config::utf8_length(pay_load);
        size_t utf8_expected_len = Config::utf8_length(expected);
        if ((pay_load == expected) && (utf8_xactualx_len <= Config::max_payload_length))  valid = true;
        if (!valid) {
            std::cout  << ansi::bold() << ansi::red()
                       << std::format("[TEST-VERIFY] CORRUPTED PAYLOAD at ID:{}  Thread_ID:{}  Event_ID:{} \n"
                                      "              Actual   (len {}): {}\n"
                                      "              Expected (len {}): {}\n",
                       id, row.thread_id, row.event_id, utf8_xactualx_len, pay_load, utf8_expected_len, expected)
                       << ansi::reset();
            all_good = false;
        }
        if (all_good) {
            std::cout << ansi::green() << std::format("[TEST-VERIFY] TEST PAYLOADS VALID  ID:{}  Thread_ID:{}  Event_ID:{} \n"
                                                "                  Actual   Len({}): {}\n"
                                                "                  Expected Len({}): {}\n",
                      id, row.thread_id, row.event_id, utf8_xactualx_len, pay_load, utf8_expected_len, expected)
                      << ansi::reset();
        } else {
            std::cout  << ansi::bold() << ansi::red() << std::format("[TEST-VERIFY] ONE OR MORE CORRUPTED\n") << ansi::reset();
        }
    }
    return all_good;
}




