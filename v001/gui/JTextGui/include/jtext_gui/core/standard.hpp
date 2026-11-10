#pragma once
// jText_GUI's versions are its C++ standards: v001 is C++23, v002 is C++26, and nothing else.
// Every v001 header includes this, so building v001 code as any other standard (the U++ window
// included, whose build method defaults to C++26) stops at compile time.
// 202302L is C++23 in GCC 14+ and Clang 18+.
static_assert(__cplusplus == 202302L, "jText_GUI v001 is C++23: build it with -std=c++23");
