cmake_minimum_required(VERSION 3.26)

# Native `import std;` for the Qlite pilot (-DJAC313_QLITE_IMPORT_STD=ON) requires
# CMake's experimental std-module feature, which MUST be armed before project()
# enables CXX. Only armed when the build opts in (the -D value is already in the
# cache at this point). The gate UUID is specific to the CMake version we standardize
# on (4.3.3, installed user-locally — see README "Newer CMake on Linux Mint"); it must
# be updated for a different CMake version.
if(JAC313_QLITE_IMPORT_STD OR JAC313_JTEXT_IMPORT_STD OR JAC313_STORE_IMPORT_STD)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
endif()

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