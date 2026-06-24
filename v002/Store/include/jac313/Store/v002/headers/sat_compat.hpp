// File:    Store/.../headers/sat_compat.hpp
// Purpose: Portable saturating integer arithmetic for jac313.
//          C++26 std::add_sat / std::mul_sat are absent or renamed in some
//          libstdc++ snapshots (gcc16 ships only the internal saturating_add/
//          saturating_mul and advertises __cpp_lib_saturation_arithmetic without
//          the standard names). These shims match std::*_sat semantics and build
//          on gcc15, gcc16 and clang via the compiler overflow builtins.
#pragma once
#ifndef JAC313_STORE_IMPORT_STD
#include <limits>
#include <type_traits>
#endif

namespace jac313 {

template <class T>
[[nodiscard]] constexpr T add_sat(T a, T b) noexcept {
    static_assert(std::is_integral_v<T>, "add_sat requires an integral type");
    T r{};
    if (!__builtin_add_overflow(a, b, &r)) return r;
    if constexpr (std::is_unsigned_v<T>) return std::numeric_limits<T>::max();
    else return a < 0 ? std::numeric_limits<T>::min() : std::numeric_limits<T>::max();
}

template <class T>
[[nodiscard]] constexpr T mul_sat(T a, T b) noexcept {
    static_assert(std::is_integral_v<T>, "mul_sat requires an integral type");
    T r{};
    if (!__builtin_mul_overflow(a, b, &r)) return r;
    if constexpr (std::is_unsigned_v<T>) return std::numeric_limits<T>::max();
    else return (a < 0) != (b < 0) ? std::numeric_limits<T>::min()
                                   : std::numeric_limits<T>::max();
}

} // namespace jac313
