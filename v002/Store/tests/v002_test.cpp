#include <jac313/Store/v002.hpp>

static_assert(jac313::Store::v002::version() == "v002.015", "v002 version");

#include <cstdlib>
#include <iostream>
#include <source_location>
#include <string_view>

// A failed check says what and where, and ends the test.
void check(bool ok, std::string_view what, std::source_location at = std::source_location::current()) {
    if (ok) return;
    std::cerr << "FAIL: " << what << " at " << at.file_name() << ":" << at.line() << "\n";
    std::exit(EXIT_FAILURE);
}

int main() {
    using Config = jac313::Store::v002::ts_store_config<true, 6, 20, 43, 2, 2, false>;
    jac313::Store::v002::ts_store<Config> store(1, 8);

    const auto [ok, slot] = store.save_event(
        0, 1,
        Config::ValueT("jac313"),
        0,
        Config::CategoryT("test"));

    check(ok, "ok");
    check(slot < 8, "slot < 8");

    jac313::Store::v002::bounded_string<16> label("store");
    check(label.view() == "store", "label.view() == \"store\"");

    std::cout << "jac313::Store::v002 core store OK\n";
    return EXIT_SUCCESS;
}