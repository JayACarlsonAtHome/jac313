// ts_store/ts_store_headers/impl_details/memory_guard.hpp
// FINAL — NO concepts, NO dependencies, NO bullshit

#pragma once

#ifndef JAC313_STORE_IMPORT_STD
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/sysinfo.h>
#include <cstdlib>
#include <type_traits>
#include <string_view>
#endif

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v001 {
#endif

template<typename ValueT, typename TypeT, typename CategoryT,
         size_t BufferSize, size_t TypeSize, size_t CategorySize, bool UseTimestamps>
struct memory_guard {
    memory_guard(size_t max_threads, size_t events_per_thread)
    {
        const size_t N = size_t(max_threads) * events_per_thread;

        // Hardcoded logic — no concepts, no circular deps
        const size_t totalEvents = max_threads * events_per_thread;
        constexpr bool value_trivial = std::is_trivially_copyable_v<ValueT> &&
            requires { std::string_view(std::declval<ValueT>()); };

        constexpr bool type_trivial  = !std::is_same_v<TypeT,  std::string_view>;
        constexpr bool cat_trivial   = !std::is_same_v<CategoryT, std::string_view>;

        constexpr size_t value_bytes = value_trivial ? sizeof(ValueT) : BufferSize;
        constexpr size_t type_bytes  = type_trivial  ? sizeof(TypeT)  : TypeSize;
        constexpr size_t cat_bytes   = cat_trivial   ? sizeof(CategoryT) : CategorySize;

        const size_t row_bytes = 8 + type_bytes + cat_bytes + value_bytes + (UseTimestamps ? 8 : 0);
        const size_t total_bytes = totalEvents * (row_bytes + 40) + (150ULL << 20);

        std::cout << "Memory guard: Threads: " << max_threads << ", " << "Events: " << events_per_thread << ",  ("
                  << (N/1000) << "k) \n"
                  << "     Payload:  " << std::setw(7) << value_bytes << " Bytes, \n"
                  << "     Type:     " << std::setw(7) << type_bytes  << " Bytes, \n"
                  << "     Category: " << std::setw(7) << cat_bytes   << " Bytes, \n"
                  << "  Total Bytes: " << std::setw(7) << (total_bytes >> 20) << " MiB (Padded for safety) \n\n";

        struct sysinfo info{};
        size_t avail = 8ULL << 30;
        if (sysinfo(&info) == 0)
            avail = info.freeram + info.bufferram + info.sharedram;

        if (total_bytes > avail * 0.92) {
            std::cerr << "   NOT ENOUGH RAM — aborting\n";
            std::exit(1);
        }
        std::cout << "   ***  RAM check: PASSED  ***\n\n";
    }
};

#ifndef JAC313_STORE_IMPORT_STD
} // namespace
#endif