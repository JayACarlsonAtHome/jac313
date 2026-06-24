// Verifies the C++26 saturating-arithmetic property that ts_store's capacity /
// memory-guard math now relies on (ts_store.hpp expected_size() + total_est):
// an overflowing (threads x events) or (N x per_row) estimate must SATURATE to
// SIZE_MAX, not wrap to a small value — otherwise the constructor's memory check
// could be fooled into permitting an impossibly large allocation.

#include <cstddef>
#include <iostream>
#include <limits>
#include <numeric>
#include <jac313/Store/v002/headers/sat_compat.hpp>

int main() {
    constexpr std::size_t MAX = std::numeric_limits<std::size_t>::max();
    int failures = 0;
    auto check = [&](bool ok, const char* what) {
        if (!ok) { std::cerr << "FAIL: " << what << "\n"; ++failures; }
    };

    // exact for normal magnitudes (the common path expected_size() takes)
    check(jac313::mul_sat<std::size_t>(5, 20) == 100, "mul_sat exact 5*20");
    check(jac313::add_sat<std::size_t>(100, 50) == 150, "add_sat exact 100+50");

    // overflow saturates to SIZE_MAX (the safety property)
    check(jac313::mul_sat<std::size_t>(MAX / 2 + 1, 4) == MAX, "mul_sat overflow saturates");
    check(jac313::mul_sat<std::size_t>(MAX, MAX) == MAX, "mul_sat MAX*MAX saturates");
    check(jac313::add_sat<std::size_t>(MAX, std::size_t{1} << 24) == MAX, "add_sat overflow saturates");

    if (failures == 0) {
        std::cout << "saturating capacity math — exact in range, saturates on overflow\n";
        return 0;
    }
    return 1;
}
