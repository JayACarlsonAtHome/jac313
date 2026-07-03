#include "jac313_sha256.hpp"

#include <iostream>
#include <string>

namespace {

int failures = 0;

#define CHECK(cond)                                                            \
    do {                                                                       \
        if (!(cond)) {                                                         \
            std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__  \
                      << "\n";                                                  \
            ++failures;                                                        \
        }                                                                      \
    } while (0)

void check_eq(const std::string& got, const std::string& want, const char* label) {
    if (got != want) {
        std::cerr << "FAIL: " << label << " got=" << got << " want=" << want << "\n";
        ++failures;
    }
}

} // namespace

int main() {
    check_eq(jac313::results::sha256_hex_lower(""),
             "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
             "empty");
    check_eq(jac313::results::sha256_hex_lower("abc"),
             "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
             "abc");
    check_eq(jac313::results::sha256_hex_lower("fedora-vm"),
             jac313::results::sha256_hex_lower("fedora-vm"),
             "stable");
    CHECK(jac313::results::sha256_hex_lower("fedora-vm").size() == 64);

    if (failures == 0) {
        std::cout << "instance_hash — sha256_hex_lower known vectors\n";
        return 0;
    }
    std::cerr << failures << " check(s) failed\n";
    return 1;
}