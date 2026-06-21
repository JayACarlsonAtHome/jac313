// ts_store/ts_store_headers/impl_details/testing.hpp

#pragma once

inline void test_run(bool is_debug = false) noexcept
{
    std::vector<std::thread> threads;
    threads.reserve(max_threads_);
    for (size_t t = 0; t < max_threads_; ++t) {
        threads.emplace_back([this, t, is_debug] {

            for (size_t i = 0; i < events_per_thread_; ++i) {

                std::string_view msg_sv = test_messages[i % test_messages.size()];
                std::string payload = std::string(msg_sv);
                std::string_view cat_sv = categories[t % categories.size()];

                uint64_t raw_flags = 0;
                raw_flags = set_user_flag(raw_flags, StoreFlags::UserFlag::LogConsole);
                raw_flags = set_user_flag(raw_flags, StoreFlags::UserFlag::KeeperRecord);
                raw_flags = set_user_flag(raw_flags, StoreFlags::UserFlag::HotCacheHint);
                raw_flags = set_severity(raw_flags, static_cast<StoreFlags::Severity>(i % 8));

                if (!payload.empty()) {
                    raw_flags = set_internal_flag(raw_flags, StoreFlags::InternalFlag::HasData);
                }

                std::array<int64_t, Config::the_IntMetrics> ints{};
                std::array<double,  Config::the_DblMetrics> dbls{};
                for (size_t k = 0; k < Config::the_IntMetrics; ++k) ints[k] = static_cast<int64_t>(i * 100 + k);
                for (size_t k = 0; k < Config::the_DblMetrics; ++k) dbls[k] = static_cast<double>(i) * 0.01 + static_cast<double>(k) * 0.001;

                auto [ok, id] = save_event(t, i, std::move(payload), raw_flags, std::string(cat_sv), is_debug, ints, dbls);
                if (!ok) continue;

                std::this_thread::yield();

                auto [ok2, val] = select(id);
                if (ok2 && std::string_view(val) == payload) {
                    // payload survived round-trip — success
                }
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }
}