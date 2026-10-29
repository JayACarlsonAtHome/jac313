#include "jac313_sha256.hpp"

#include <iostream>
#include <string>
#include <source_location>
#include <string_view>
#include <cstdlib>

namespace {

int failures = 0;

// A failed check says what and where, and counts toward failures.
void check(bool ok, std::string_view what, std::source_location at = std::source_location::current()) {
    if (ok) return;
    std::cerr << "FAIL: " << what << " at " << at.file_name() << ":" << at.line() << "\n";
    ++failures;
}

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
    check(jac313::results::sha256_hex_lower("fedora-vm").size() == 64, "jac313::results::sha256_hex_lower(\"fedora-vm\").size() == 64");

    if (failures == 0) {
        std::cout << "instance_hash — sha256_hex_lower known vectors\n";
        return 0;
    }
    std::cerr << failures << " check(s) failed\n";
    return 1;
}