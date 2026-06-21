#include <jac313/Store/v001.hpp>

#include <cstdint>
#include <iostream>

int main() {
    using Config = jac313::Store::v001::ts_store_config<true, 6, 20, 43, 2, 2, false>;
    jac313::Store::v001::ts_store<Config> store(1, 4);

    const auto [ok, slot] = store.save_event(
        0, 0,
        Config::ValueT("payload"),
        0,
        Config::CategoryT("demo"));

    if (!ok) {
        std::cerr << "save_event failed\n";
        return 1;
    }

    std::cout << "jac313::Store::v001 saved event at slot " << slot << "\n";
    return 0;
}