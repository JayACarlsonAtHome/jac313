cmake_minimum_required(VERSION 3.20)

# Native `import std;` (JAC313_QLITE_IMPORT_STD) needs CMake's experimental std-module
# feature ARMED before project() enables CXX. The -D value is already in the cache here,
# before option() formally runs below. UUID is pinned to the CMake version we standardize
# on (4.3.3); bump it if you move to a different CMake (see root README). Only the
# umbrella root sets this for in-tree builds; this covers a standalone Qlite configure.
if(JAC313_QLITE_IMPORT_STD AND NOT CMAKE_EXPERIMENTAL_CXX_IMPORT_STD)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
endif()

project(jac313_Qlite VERSION 0.1.0 LANGUAGES CXX)

# jac313::Qlite — in-tree SQLite wrapper (jac313::Qlite::v001).
# Migrated from the legacy jacQlite repo / jac::qlite; no sibling checkout needed.
# Header-only; depends on system SQLite3.

option(JAC313_QLITE_BUILD_EXAMPLES "Build example programs" ON)
option(JAC313_QLITE_BUILD_TESTS "Build and register tests" ON)
option(JAC313_QLITE_BUILD_MODULES "Build jac313.qlite.v001 C++23 module" OFF)
# import-std pilot (g++-15 only). When ON, the module is built module-native against
# `import std;` instead of textually including the public header. Uses CMake's native
# std-module support (CMAKE_EXPERIMENTAL_CXX_IMPORT_STD gate + CXX_MODULE_STD), which
# requires CMake >= 3.30 — install one user-locally on older distros (see the root
# README, "Newer CMake on Linux Mint"). Clang keeps the textual front-end (leave OFF).
option(JAC313_QLITE_IMPORT_STD "Build the jac313.qlite.v001 module against `import std;` (g++-15 pilot, native CMake std module)" OFF)

find_package(SQLite3 REQUIRED)
# FindSQLite3 renamed its imported target SQLite::SQLite3 -> SQLite3::SQLite3 in newer
# CMake (old name still works but warns). Prefer the new name when present so we stay
# quiet on CMake >= ~3.31 yet keep building on 3.28 (which only defines the old name).
if(TARGET SQLite3::SQLite3)
    set(JAC313_QLITE_SQLITE3_TARGET SQLite3::SQLite3)
else()
    set(JAC313_QLITE_SQLITE3_TARGET SQLite::SQLite3)
endif()

add_library(jac313_Qlite INTERFACE)
add_library(jac313::Qlite ALIAS jac313_Qlite)

target_include_directories(jac313_Qlite INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(jac313_Qlite INTERFACE cxx_std_20)
target_link_libraries(jac313_Qlite INTERFACE ${JAC313_QLITE_SQLITE3_TARGET})

if(JAC313_QLITE_BUILD_MODULES)
    # Enable module dependency scanning for targets in this directory tree (incl. the
    # module test executable, which `import`s the module). The umbrella root sets this
    # for in-tree builds; setting it here too makes a STANDALONE Qlite module build work.
    set(CMAKE_CXX_SCAN_FOR_MODULES ON)

    add_library(jac313_qlite_module OBJECT)
    add_library(jac313::Qlite_module ALIAS jac313_qlite_module)
    target_sources(jac313_qlite_module
        PUBLIC
            FILE_SET cxx_modules TYPE CXX_MODULES
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
            FILES modules/jac313.qlite.v001.cppm
    )
    target_link_libraries(jac313_qlite_module PUBLIC jac313_Qlite)

    if(JAC313_QLITE_IMPORT_STD)
        # Module-native `import std;` via CMake's NATIVE std-module support. CMake
        # builds + links the std module itself (no hand-rolled bits/std.cc target);
        # we only flip CXX_MODULE_STD on this target. The experimental feature is
        # armed by CMAKE_EXPERIMENTAL_CXX_IMPORT_STD, which must be set BEFORE the
        # project() call enables CXX — see the root CMakeLists.txt (umbrella build)
        # and the standalone guard at the top of this file.
        if(CMAKE_VERSION VERSION_LESS 3.30)
            message(FATAL_ERROR
                "JAC313_QLITE_IMPORT_STD needs CMake >= 3.30 for native `import std;` "
                "(have ${CMAKE_VERSION}). Install a newer CMake without root — see the "
                "root README section \"Newer CMake on Linux Mint\".")
        endif()
        if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            message(FATAL_ERROR
                "JAC313_QLITE_IMPORT_STD is a g++-15 pilot; compiler is "
                "${CMAKE_CXX_COMPILER_ID}. Leave it OFF for clang (textual front-end).")
        endif()
        set_target_properties(jac313_qlite_module PROPERTIES CXX_MODULE_STD ON)
        target_compile_features(jac313_qlite_module PUBLIC cxx_std_23)
        target_compile_definitions(jac313_qlite_module PRIVATE JAC313_QLITE_IMPORT_STD)
        message(STATUS "jac313::Qlite module: import std ENABLED (native CXX_MODULE_STD)")
    else()
        target_compile_features(jac313_qlite_module PUBLIC cxx_std_20)
    endif()
endif()

include(GNUInstallDirs)
install(TARGETS jac313_Qlite
    EXPORT jac313QliteTargets
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT jac313QliteTargets
    FILE jac313QliteTargets.cmake
    NAMESPACE jac313::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/jac313_Qlite
)

if(JAC313_QLITE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(JAC313_QLITE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(JAC313_QLITE_BUILD_MODULES AND JAC313_QLITE_BUILD_TESTS)
    add_subdirectory(tests/module)
endif()

message(STATUS "jac313::Qlite configured (in-tree SQLite wrapper)")