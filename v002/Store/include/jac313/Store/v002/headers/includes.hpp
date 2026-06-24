// ts_store/ts_store_headers/includes.hpp
// Created by jay on 1/12/26.
//
#pragma once
#ifndef JAC313_STORE_IMPORT_STD
#include <algorithm>
#include <bitset>
#include <thread>
#include <chrono>
#include <string>
#include <string_view>
#include <vector>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <memory>
#include <print>
#include <cstdio>
#include <format>
#include <utility>
#include <cctype>
#include <sys/sysinfo.h>
#endif
#include "ts_store_flags.hpp"
#include "ansi_colors.hpp"
#include "ts_store_config.hpp"
#include "impl_details/test_options.hpp"

// ----------------------------------------------------------------------------
// Runtime-format print helpers (jac313). Portable replacement for
//   std::print(std::runtime_format(fmt), args...)
// which broke two ways: gcc16's libstdc++ dropped std::runtime_format, and
// clang's std::print consteval check rejects dynamic specs ({:>{}}). These use
// the vprint path: a runtime format string (no consteval) and no runtime_format.
// Args bind as const lvalues here so std::make_format_args (P2905) accepts them.
// ----------------------------------------------------------------------------
template <class... Args>
inline void jac313_rt_print(std::string_view fmt, const Args&... args) {
    std::vprint_unicode(stdout, fmt, std::make_format_args(args...));
}
template <class... Args>
inline void jac313_rt_println(std::string_view fmt, const Args&... args) {
    std::vprint_unicode(stdout, fmt, std::make_format_args(args...));
    std::putchar('\n');
}

