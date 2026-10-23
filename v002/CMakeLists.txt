cmake_minimum_required(VERSION 3.26)

project(jac313 VERSION 0.1.0 LANGUAGES CXX
        DESCRIPTION "jac313 umbrella libraries (Qlite, jText, store)")

# Deliberate default: Debug for local development, matrix gates, and test-summary baselines.
# Release / -O3 tuning is an optional deployment path, not the repo default yet.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug default)" FORCE)
endif()

# Monorepo root. Each package is a thin versioned namespace over a legacy sibling repo.
# Legacy implementations stay in jacQlite/, jText/, ts_store/ until migrated in small chunks.

option(JAC313_BUILD_MODULES
       "Build C++23 modules (Qlite, jText, store)" OFF)

if(JAC313_BUILD_MODULES)
    if(NOT CMAKE_GENERATOR MATCHES "Ninja")
        message(FATAL_ERROR "JAC313_BUILD_MODULES requires the Ninja generator (-G Ninja)")
    endif()
    set(CMAKE_CXX_SCAN_FOR_MODULES ON)
    set(JAC313_SETUP_BUILD_MODULES ON CACHE BOOL "" FORCE)
    set(JAC313_QLITE_BUILD_MODULES ON CACHE BOOL "" FORCE)
    set(JAC313_JTEXT_BUILD_MODULES ON CACHE BOOL "" FORCE)
    set(JAC313_STORE_BUILD_MODULES ON CACHE BOOL "" FORCE)
endif()

# Setup is foundational (toolchain sensing) and has no internal deps — built first.
option(JAC313_BUILD_SETUP "Build jac313::Setup toolchain sensing" ON)
option(JAC313_BUILD_QLITE "Build jac313::Qlite umbrella" ON)
option(JAC313_BUILD_JTEXT "Build jac313::jText umbrella" ON)
option(JAC313_BUILD_STORE "Build jac313::Store umbrella" ON)
option(JAC313_BUILD_STORE_TESTS "Build jac313::Store tests" OFF)
option(JAC313_BUILD_TEST_CLI "Build jac313_test_cli local test runner" ON)

# Opt-in compiler strictness (both OFF: the default build and its baselines are unchanged).
# Measured 2026-10-16 on gcc 15.2 + clang 21 at C++26: 0 errors, 0 -Wpedantic diagnostics;
# see docs/Cpp26_Adoption.md for the warnings that remain.
# Format checks, always on: a printf-style call whose arguments do not match its format
# (e.g. a 64-bit value given to %d) is an error, not undefined behaviour at run time.
add_compile_options(-Wformat=2 -Werror=format)

option(JAC313_STRICT "Extra warnings: -Wall -Wextra -Wpedantic and friends" OFF)
option(JAC313_STRICT_WERROR "With JAC313_STRICT: treat those warnings as errors" OFF)
# Runtime hardening. Changes behaviour, so keep it out of bench and valgrind builds:
# -ftrivial-auto-var-init=zero hides the uninitialized reads memcheck exists to find.
option(JAC313_HARDENED "Runtime hardening: libstdc++ assertions, zero-init locals, stack/CF protection" OFF)

if(JAC313_STRICT)
    # -Wnull-dereference is left out: on gcc 15 it fires only inside libstdc++ <streambuf>.
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Woverloaded-virtual
        -Wimplicit-fallthrough)
    if(JAC313_STRICT_WERROR)
        add_compile_options(-Werror)
    endif()
endif()

if(JAC313_HARDENED)
    # The explicit equivalent of gcc's -fhardened, spelled so clang takes it too.
    # _FORTIFY_SOURCE needs optimization, so it is added only to non-Debug configs.
    add_compile_options(
        -D_GLIBCXX_ASSERTIONS
        -ftrivial-auto-var-init=zero
        -fstack-protector-strong -fstack-clash-protection -fcf-protection=full
        "$<$<NOT:$<CONFIG:Debug>>:-U_FORTIFY_SOURCE;-D_FORTIFY_SOURCE=3>")
endif()

if(JAC313_BUILD_STORE_TESTS)
    set(JAC313_STORE_BUILD_TESTS ON CACHE BOOL "" FORCE)
    set(JAC313_STORE_BUILD_MATRIX_TESTS ON CACHE BOOL "" FORCE)
else()
    set(JAC313_STORE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
endif()

if(JAC313_BUILD_QLITE OR JAC313_BUILD_JTEXT OR JAC313_BUILD_STORE)
    enable_testing()
endif()

if(JAC313_BUILD_SETUP)
    add_subdirectory(Setup)
endif()

if(JAC313_BUILD_QLITE)
    add_subdirectory(Qlite)
endif()

if(JAC313_BUILD_JTEXT)
    add_subdirectory(jText)
endif()

if(JAC313_BUILD_STORE)
    if(JAC313_BUILD_JTEXT)
        set(JAC313_STORE_JTEXT_PERSIST ON CACHE BOOL "" FORCE)
    endif()
    if(JAC313_BUILD_QLITE)
        set(JAC313_STORE_SQL_PERSIST ON CACHE BOOL "" FORCE)
    endif()
    add_subdirectory(Store)
endif()

if(JAC313_BUILD_TEST_CLI)
    add_subdirectory(tools)
endif()

message(STATUS "jac313 monorepo configured.")