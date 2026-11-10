// contracts_sample.cpp: v001's C++23 library contracts (jac321, vendored in Qlite) in use.
// Build, from v001/:
//   g++ -std=c++23 -Wall -Wextra -Werror -IjText/Qlite/include docs/contracts/contracts_sample.cpp -lsqlite3 -o contracts_sample
// The last line breaks a contract on purpose: the program stops there (exit 134).

#include <jac313/Qlite/v001.hpp>
#include <iostream>

namespace jc = jac313::Qlite::v001::jac321;

// A contract of your own: a precondition, checked by plain C++23 code.
// Abort stops the program on a broken contract; Throw throws jc::ContractError instead.
template <class Policy = jc::Abort>
double percent_of(double part, double whole) {
    jc::check<Policy>(jc::Kind::pre, [&] { return whole > 0; }, "whole > 0");
    return 100.0 * part / whole;
}

int main() {
    jac313::Qlite::v001::Sqlite db(":memory:");
    db.exec("CREATE TABLE t(a INTEGER, b TEXT)");
    db.exec("INSERT INTO t VALUES(1, 'one')");
    auto st = db.prepare("SELECT a, b FROM t");
    st.step();
    std::int64_t a = 0; std::string b, c;
    st.get(a, b);                         // checked once: the row has at least 2 columns
    std::cout << a << " " << b << std::endl;
    std::cout << percent_of(1, 4) << std::endl;
    try { percent_of<jc::Throw>(1, 0); }
    catch (const jc::ContractError& e) { std::cout << "Throw: " << e.what() << std::endl; }
    st.get(a, b, c);                      // 3 values from a 2-column row: the contract stops it
}
