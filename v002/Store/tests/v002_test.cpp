#include <jac313/Store/v002.hpp>

static_assert(jac313::Store::v002::version() == "v002.003", "v002 version");

#include <cstdlib>
#include <iostream>

#define CHECK(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__ << "\n"; \
            return EXIT_FAILURE; \
        } \
    } while (0)

int main() {
    using Config = jac313::Store::v002::ts_store_config<true, 6, 20, 43, 2, 2, false>;
    jac313::Store::v002::ts_store<Config> store(1, 8);

    const auto [ok, slot] = store.save_event(
        0, 1,
        Config::ValueT("jac313"),
        0,
        Config::CategoryT("test"));

    CHECK(ok);
    CHECK(slot < 8);

    jac313::Store::v002::bounded_string<16> label("store");
    CHECK(label.view() == "store");

    std::cout << "jac313::Store::v002 core store OK\n";
    return EXIT_SUCCESS;
}