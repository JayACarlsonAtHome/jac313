# Contracts in v001 (C++23, no compiler support needed)

v001 is the C++23 version, and C++23 has no contracts. Instead of waiting for compilers,
v001 carries **experimental, non-standard contracts written as ordinary C++23 code**. They
need nothing from the compiler beyond `-std=c++23`: no `-fcontracts`, no `pre`/`post`
keywords, the same behaviour on GCC and Clang. v002 (C++26) uses the standard contracts,
so the two can be compared, in what they can say and in what they cost.

The contracts come from **jac321**, a separate repository of contract experiments. They
are copied into Qlite by jac321's `tools/vendor.sh` (into
`jText/Qlite/include/jac313/Qlite/v001/jac321/`, namespace
`jac313::Qlite::v001::jac321`). Never edit the copy: change jac321 and copy again.

## The idea

- **A contract is a call to `check`.** It takes the kind (`pre`, `post`, `invariant`,
  `assertion`), the condition as a lambda, and the text to report.
- **What a broken contract does is a template parameter, chosen at compile time.**
  `Abort` stops the program, `Throw` throws `ContractError`, `ReportContinue` reports and
  goes on, `Ignore` skips the check. A policy that does not evaluate never runs the
  condition at all.
- **A type can be the proof.** A value that must be in range is checked once, when it is
  made, and after that its type says it is in range, so nothing checks it again.
- **One switch turns them off for a whole build.** `BuildOff` is picked by a `consteval`
  function from the library's "contracts off" constant, and every check disappears.
- **A broken contract during constant evaluation is a compile error.**

## Where v001 uses them today: Qlite's column and bind numbers

Reading column 3 of a 2-column row, or binding parameter 4 of a 3-parameter statement, is
a silent bug in SQLite: it reads NULL or ignores the bind without a word. In v001 those
numbers are types:

```cpp
// Qlite/v001/Database.body.hpp
using ColumnIndex = jac321::Index<ColumnTag, 0, IndexPolicy>;   // 0 <= i < columns
using BindIndex   = jac321::Index<BindTag, 1, IndexPolicy>;     // 1 <= i <= parameters

consteval auto index_policy() noexcept {
    if constexpr ((QLITE_CONTRACTS_ENABLED) != 0) return ContractPolicy{};   // report and abort
    else return jac321::BuildOff{};                                      // no checks at all
}
```

- A `ColumnIndex` is checked once when it is made; every function that takes one needs no
  check of its own.
- The tag keeps the two kinds apart: a column number does not compile where a bind number
  is expected.
- `get(a, b, c)` reads three columns with **one** check (`FirstColumns<3>`: the row has at
  least 3 columns), not three:

```cpp
// Qlite/v001/Sqlite.body.hpp
template<typename... Ts>
void get(Ts&... values) {
    const FirstColumns<sizeof...(Ts)> cols{sqlite3_data_count(stmt_)};   // the one check
    [&]<std::size_t... I>(std::index_sequence<I...>) {
        ((values = cell<Ts>(cols.template at<I>())), ...);   // at<I>() is proven by I < N
    }(std::index_sequence_for<Ts...>{});
}
```

The same holds for the PostgreSQL and MariaDB statements.

## Sample: using it

[contracts/contracts_sample.cpp](contracts/contracts_sample.cpp) builds with only
`-std=c++23` (from `v001/`):

```bash
g++ -std=c++23 -Wall -Wextra -Werror -IjText/Qlite/include docs/contracts/contracts_sample.cpp -lsqlite3 -o contracts_sample
```

```cpp
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
```

Output (GCC 16; Clang 22 prints the same, apart from how it spells the function):

```text
1 one
25
Throw: jac321 precondition violated: (whole > 0) at docs/contracts/contracts_sample.cpp:14 ...
Qlite contract precondition violated: (count >= 3) at jText/Qlite/include/jac313/Qlite/v001/Sqlite.body.hpp:303 in void jac313::Qlite::v001::Sqlite::Statement::get(Ts& ...) ...
Aborted (core dumped)
```

The last line is the point: without the contract, SQLite would hand back an empty `c`
and the program would carry on with a wrong value.

## What comes next

These are experiments. More of v001 will use them as they prove themselves; the open
ones (old values in postconditions, class invariants, contract levels, a policy that logs
to SQLite, a `Sorted` type, and what each one costs) are worked out in jac321 first. The
benchmark is meant to time them against v002's standard C++26 contracts.
