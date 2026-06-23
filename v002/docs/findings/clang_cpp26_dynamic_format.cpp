// ─────────────────────────────────────────────────────────────────────────────
// Toolchain finding: clang 21 rejects well-formed C++26 std::print with a
//                    *dynamic* width spec; gcc 15 accepts it.
//
// Surfaced by jac313 v002 (the C++26 world) — Store's diagnostic/printing code
// uses dynamic-width specs like "{:>{}}" and "{:^{}}" to align columns. v001
// (C++23) builds clean under clang; v002 (C++26) does not, on the SAME source.
//
// Reproduce (RHEL 9.8, gcc-toolset-15 providing libstdc++15):
//
//     g++     -std=c++23  clang_cpp26_dynamic_format.cpp   # PASS
//     g++     -std=c++26  clang_cpp26_dynamic_format.cpp   # PASS
//     clang++ -std=c++23  clang_cpp26_dynamic_format.cpp   # PASS
//     clang++ -std=c++26  clang_cpp26_dynamic_format.cpp   # FAIL  ← the bug
//
// clang 21.1.8 + libstdc++ 15.2.1, -std=c++26, emits:
//
//   error: call to consteval function
//          'std::basic_format_string<...>::basic_format_string<char[8]>'
//          is not a constant expression
//   note:  undefined function
//          '__check_dynamic_spec<int, unsigned int, long long, unsigned long long>'
//          cannot be used in a constant expression
//
// Root cause: libstdc++'s consteval format-string check calls the *declared-but-
// undefined* helper __check_dynamic_spec for runtime-width specs. gcc's constant
// evaluator handles it; clang's C++26 evaluator refuses to call an undefined
// function during constant evaluation. The C++23 path doesn't reach it — which is
// why only the C++26 build breaks.
//
// Workaround used in v002: wrap the dynamic-width format strings in
// std::runtime_format(...), which opts out of the consteval check. See
// ../Cpp26_Adoption.md ("Toolchain finding: clang + dynamic-width std::print").
// ─────────────────────────────────────────────────────────────────────────────

#include <print>
#include <string>

int main() {
    std::string msg = "hi";
    int width = 8;                          // a *runtime* width — the trigger
    std::print("{:^{}}\n", msg, width);     // center 'msg' in a width-8 field
    // Workaround that compiles everywhere:
    // std::print(std::runtime_format("{:^{}}\n"), msg, width);
}
