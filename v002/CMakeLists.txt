cmake_minimum_required(VERSION 3.26)
# CMake 4.3.3 is required (the pinned CMake: Setup/recipes.jtext pinned_tools; ./bsir.sh installs it
# into cmake_install). 4.3.0 compiled each imported module's BMI with -c plus --precompile, so
# clang and clang-scan-deps warned and every -Werror Clang module build failed; 4.3.3 has a BMI
# rule without -c (docs/Modules.md). Checked here rather than by raising cmake_minimum_required,
# which would also switch every policy up to 4.3 to NEW.
if(CMAKE_VERSION VERSION_LESS 4.3.3)
    message(FATAL_ERROR "jac313 needs CMake 4.3.3 or newer (this is ${CMAKE_VERSION}). "
                        "./bsir.sh installs the pinned CMake into cmake_install beside the repository.")
endif()

# Every warning is an error, CMake's own too: dev (AUTHOR_WARNING) and deprecation warnings stop
# the configure. A plain message(WARNING) has no such switch: use FATAL_ERROR/SEND_ERROR instead.
set(CMAKE_SUPPRESS_DEVELOPER_ERRORS FALSE CACHE INTERNAL "")
set(CMAKE_ERROR_DEPRECATED TRUE CACHE INTERNAL "")

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
    set(JAC313_JTEXT_BUILD_MODULES ON CACHE BOOL "" FORCE)
    set(JAC313_STORE_BUILD_MODULES ON CACHE BOOL "" FORCE)
endif()

# Setup is foundational (toolchain sensing) and has no internal deps — built first.
option(JAC313_BUILD_SETUP "Build jac313::Setup toolchain sensing" ON)
option(JAC313_BUILD_JTEXT "Build jac313::jText umbrella" ON)
option(JAC313_BUILD_STORE "Build jac313::Store umbrella" ON)
option(JAC313_BUILD_STORE_TESTS "Build jac313::Store tests" OFF)
option(JAC313_BUILD_TEST_CLI "Build jac313_test_cli local test runner" ON)

# Opt-in compiler strictness (both OFF: the default build and its baselines are unchanged).
# Measured 2026-10-16 on gcc 15.2 + clang 21 at C++26: 0 errors, 0 -Wpedantic diagnostics;
# see docs/Cpp26_Adoption.md for the warnings that remain.
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

# Every warning is an error outside the compiler too:
# - the linker: --fatal-warnings;
# - clang-scan-deps (C++ module scanning): it has no -Werror and ignores the compiler's, so CMake
#   runs it through a wrapper, written here, that fails when it prints anything on stderr (its
#   real output, the dependency file, is stdout). project() bakes the scanner's path into the
#   scan rule (CMAKE_CXX_SCANDEP_SOURCE), so the path is swapped there. A parent project that
#   already swapped it is kept.
add_link_options(-Wl,--fatal-warnings)
if(CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS AND CMAKE_CXX_SCANDEP_SOURCE
   AND NOT CMAKE_CXX_SCANDEP_SOURCE MATCHES "clang-scan-deps-strict")
    set(strict_scan_deps "${CMAKE_BINARY_DIR}/clang-scan-deps-strict")
    file(WRITE "${strict_scan_deps}" "#!/usr/bin/env bash
# Written by CMake: runs ${CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS}; any diagnostic it prints fails the build.
err=\"\$(mktemp)\"
trap 'rm -f \"\$err\"' EXIT
\"${CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS}\" \"\$@\" 2>\"\$err\"
rc=\$?
if [ -s \"\$err\" ]; then
    cat \"\$err\" >&2
    echo 'clang-scan-deps printed a diagnostic: every warning is an error' >&2
    [ \$rc -eq 0 ] && rc=1
fi
exit \$rc
")
    file(CHMOD "${strict_scan_deps}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
         GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    string(REPLACE "\"${CMAKE_CXX_COMPILER_CLANG_SCAN_DEPS}\"" "\"${strict_scan_deps}\""
           CMAKE_CXX_SCANDEP_SOURCE "${CMAKE_CXX_SCANDEP_SOURCE}")
    if(NOT CMAKE_CXX_SCANDEP_SOURCE MATCHES "clang-scan-deps-strict")
        message(FATAL_ERROR "could not route clang-scan-deps through ${strict_scan_deps}")
    endif()
endif()

# Runtime hardening. Changes behaviour, so keep it out of bench and valgrind builds:
# -ftrivial-auto-var-init=zero hides the uninitialized reads memcheck exists to find.
option(JAC313_HARDENED "Runtime hardening: libstdc++ assertions, zero-init locals, stack/CF protection" OFF)

if(JAC313_HARDENED)
    # The explicit equivalent of gcc's -fhardened, spelled so clang takes it too.
    # _FORTIFY_SOURCE needs optimization, so it is added only to non-Debug configs.
    add_compile_options(
        -D_GLIBCXX_ASSERTIONS
        -ftrivial-auto-var-init=zero
        -fstack-protector-strong -fstack-clash-protection -fcf-protection=full
        "$<$<NOT:$<CONFIG:Debug>>:-U_FORTIFY_SOURCE;-D_FORTIFY_SOURCE=3>")
endif()

# Contracts (JAC313_ / JTEXT_ / QLITE_ PRE/POST/ASSERT). JAC313_CONTRACTS picks how they run;
# Setup/contracts.jtext lists the modes each toolchain has on each OS, and jac313_test_cli
# --contracts checks a choice against it. GCC 16 builds the whole tree with NATIVE ISO C++26
# contracts (P2900): -fcontracts defines __cpp_contracts, so the macros become contract_assert
# under -fcontract-evaluation-semantic=<mode>. -fcontracts must be on the link line too, or gcc16
# leaves the default contract-violation handler out (undefined handle_contract_violation).
# Other compilers have no native contracts: the macros are runtime checks that print and abort
# (enforce), or are compiled out (ignore).
set(JAC313_CONTRACTS "enforce" CACHE STRING "Contract evaluation: enforce | observe | quick_enforce | ignore")
set_property(CACHE JAC313_CONTRACTS PROPERTY STRINGS enforce observe quick_enforce ignore)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 16)
    if(NOT JAC313_CONTRACTS MATCHES "^(enforce|observe|quick_enforce|ignore)$")
        message(FATAL_ERROR "JAC313_CONTRACTS=${JAC313_CONTRACTS}: use enforce, observe, quick_enforce or ignore")
    endif()
    add_compile_options(-fcontracts -fcontract-evaluation-semantic=${JAC313_CONTRACTS})
    add_link_options(-fcontracts)
elseif(JAC313_CONTRACTS STREQUAL "ignore")
    add_compile_definitions(JAC313_CONTRACTS_ENABLED=0 JTEXT_CONTRACTS_ENABLED=0 QLITE_CONTRACTS_ENABLED=0)
elseif(NOT JAC313_CONTRACTS STREQUAL "enforce")
    message(FATAL_ERROR "JAC313_CONTRACTS=${JAC313_CONTRACTS} needs native C++26 contracts (GCC 16); "
                        "this compiler has enforce or ignore")
endif()
# Every binary knows the mode it was built under: results.db records it with each run
# (jac313::contracts::contracts_mode, Setup/include/jac313/Setup/contracts.hpp).
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
# The death tests (a violated contract must terminate) only hold when violations terminate.
if(JAC313_CONTRACTS MATCHES "^(enforce|quick_enforce)$")
    set(JAC313_CONTRACTS_TERMINATE ON)
else()
    set(JAC313_CONTRACTS_TERMINATE OFF)
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