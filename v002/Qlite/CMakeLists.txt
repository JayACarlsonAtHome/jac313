cmake_minimum_required(VERSION 3.20)

project(jac313_Qlite VERSION 0.1.0 LANGUAGES CXX)

# jac313::Qlite — in-tree SQLite wrapper (jac313::Qlite::v002).
# Migrated from the legacy jacQlite repo / jac::qlite; no sibling checkout needed.
# Header-only; depends on system SQLite3.

option(JAC313_QLITE_BUILD_EXAMPLES "Build example programs" ON)
option(JAC313_QLITE_BUILD_TESTS "Build and register tests" ON)
option(JAC313_QLITE_BUILD_MODULES "Build jac313.qlite.v002 C++23 module" OFF)

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
            FILES modules/jac313.qlite.v002.cppm
    )
    target_link_libraries(jac313_qlite_module PUBLIC jac313_Qlite)

    target_compile_features(jac313_qlite_module PUBLIC cxx_std_20)
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