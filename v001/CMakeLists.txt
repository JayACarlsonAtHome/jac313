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
    # CMake 4 builds a separate BMI for each use of a module with its own rule: "--precompile ... -c",
    # and clang reports that -c as unused, which -Werror stops. The -c is CMake's, not ours, so this
    # turns off only that warning and only on that one kind of step.
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_MODULE_BMI_ONLY_FLAG "--precompile;-Wno-unused-command-line-argument")
    endif()
    set(JAC313_SETUP_BUILD_MODULES ON CACHE BOOL "" FORCE)
    set(JAC313_JTEXT_BUILD_MODULES ON CACHE BOOL "" FORCE)
    set(JAC313_STORE_BUILD_MODULES ON CACHE BOOL "" FORCE)
endif()

# Setup is foundational (toolchain sensing) and has no internal deps — built first.
option(JAC313_BUILD_SETUP "Build jac313::Setup toolchain sensing" ON)
option(JAC313_BUILD_JTEXT "Build jac313::jText umbrella" ON)
option(JAC313_BUILD_STORE "Build jac313::Store umbrella" ON)
option(JAC313_BUILD_STORE_TESTS "Build jac313::Store tests" OFF)
option(JAC313_BUILD_TEST_CLI "Build jac313_test_cli local test runner" ON)

# Compiler checks, the same as v002 (see v002/docs/Cpp26_Adoption.md); here they apply at C++23.
# Format checks, always on: a printf-style call whose arguments do not match its format
# (e.g. a 64-bit value given to %d) is an error, not undefined behaviour at run time.
# Conversion warnings, always on: a value silently narrowed or changing sign (size_t into int,
# int into unsigned, 64-bit into float). -Wsign-conversion makes gcc
# report the signed/unsigned mixes clang's -Wconversion already does.
# Strict warnings, always on (they were the JAC313_STRICT option): no runtime
# effect. -Wnull-dereference is left out: on gcc 15 it fires only inside libstdc++ <streambuf>.
# -Werror, always on: every warning is a build error, so none can pile up unseen.
add_compile_options(-Werror -Wformat=2 -Wconversion -Wsign-conversion
    -Wall -Wextra -Wpedantic
    -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Woverloaded-virtual
    -Wimplicit-fallthrough)

# Runtime hardening. Changes behaviour, so keep it out of bench and valgrind builds:
# -ftrivial-auto-var-init=zero hides the uninitialized reads memcheck exists to find.
option(JAC313_HARDENED "Runtime hardening: libstdc++ assertions, zero-init locals, stack/CF protection" OFF)

# Contracts (the vendored jText's JTEXT_ and Qlite's QLITE_ PRE/POST/ASSERT). v001 is C++23,
# so there are no native C++26 contracts: the macros are runtime checks that print and abort
# (enforce), or are compiled out (ignore). Setup/contracts.jtext lists these modes per OS and toolchain.
set(JAC313_CONTRACTS "enforce" CACHE STRING "Contract evaluation: enforce | ignore (v001 has no native contracts)")
set_property(CACHE JAC313_CONTRACTS PROPERTY STRINGS enforce ignore)
if(JAC313_CONTRACTS STREQUAL "ignore")
    add_compile_definitions(JTEXT_CONTRACTS_ENABLED=0 QLITE_CONTRACTS_ENABLED=0)
elseif(NOT JAC313_CONTRACTS STREQUAL "enforce")
    message(FATAL_ERROR "JAC313_CONTRACTS=${JAC313_CONTRACTS}: v001 (C++23) has enforce or ignore")
endif()
# Every binary knows the mode it was built under: results.db records it with each run.
add_compile_definitions(JAC313_CONTRACTS_MODE="${JAC313_CONTRACTS}")
# ... and the rest of how it was built (results.db parameter.build_type / modules / strict / hardened).
if(JAC313_BUILD_MODULES)
    set(_jac313_modules_state on)
else()
    set(_jac313_modules_state off)
endif()
if(JAC313_HARDENED)
    set(_jac313_hardened_state on)
else()
    set(_jac313_hardened_state off)
endif()
add_compile_definitions(JAC313_BUILD_TYPE="${CMAKE_BUILD_TYPE}" JAC313_BUILD_MODULES="${_jac313_modules_state}"
                        JAC313_STRICT="on" JAC313_HARDENED="${_jac313_hardened_state}")
if(JAC313_CONTRACTS STREQUAL "enforce")
    set(JAC313_CONTRACTS_TERMINATE ON)   # the death tests hold only when violations terminate
else()
    set(JAC313_CONTRACTS_TERMINATE OFF)
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

if(JAC313_BUILD_JTEXT OR JAC313_BUILD_STORE)
    enable_testing()
endif()

if(JAC313_BUILD_SETUP)
    add_subdirectory(Setup)
endif()

if(JAC313_BUILD_JTEXT)
    add_subdirectory(jText)
endif()

if(JAC313_BUILD_STORE)
    if(JAC313_BUILD_JTEXT)   # jText brings in Qlite (jac313::Qlite) as well
        set(JAC313_STORE_JTEXT_PERSIST ON CACHE BOOL "" FORCE)
        set(JAC313_STORE_SQL_PERSIST ON CACHE BOOL "" FORCE)
    endif()
    add_subdirectory(Store)
endif()

if(JAC313_BUILD_TEST_CLI)
    add_subdirectory(tools)
endif()

message(STATUS "jac313 monorepo configured.")