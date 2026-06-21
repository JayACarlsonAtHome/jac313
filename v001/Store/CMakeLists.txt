cmake_minimum_required(VERSION 3.26)

# Native `import std;` (JAC313_STORE_IMPORT_STD) needs CMake's experimental std-module
# feature armed before project() enables CXX. Mirrors Qlite/jText; the umbrella root also
# sets this. UUID is pinned to the CMake we standardize on (4.3.3).
if(JAC313_STORE_IMPORT_STD AND NOT CMAKE_EXPERIMENTAL_CXX_IMPORT_STD)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
endif()

project(jac313_Store VERSION 0.1.0 LANGUAGES CXX)

# jac313::Store — core in-tree implementation (jac313::Store::v001 namespace).

option(JAC313_STORE_BUILD_EXAMPLES "Build example programs" ON)
option(JAC313_STORE_BUILD_TESTS "Build and register tests" OFF)
option(JAC313_STORE_BUILD_MATRIX_TESTS
       "Build ported ts_store matrix tests (001-008 TS/XS, flags)" ON)
option(JAC313_STORE_BUILD_MODULES "Build jac313.store.v001 C++23 module (in-tree)" OFF)
# import-std pilot (g++-15 only). When ON, the module is built MODULE-NATIVE against
# `import std;` (one interface unit over the guarded headers + .cpp) instead of the
# textual-wrapper module. Native CMake std module -> needs CMake >= 3.30. Clang keeps the
# textual wrapper (leave OFF). The compiled sink libs / textual headers are unchanged.
option(JAC313_STORE_IMPORT_STD "Build the jac313.store.v001 module against `import std;` (g++-15 pilot, native CMake std module)" OFF)
option(JAC313_STORE_JTEXT_PERSIST "Build in-tree jText split persistence (needs jac313::jText)" OFF)
option(JAC313_STORE_SQL_PERSIST "Build in-tree SQLite persistence (needs jac313::Qlite)" OFF)

add_library(jac313_Store INTERFACE)
add_library(jac313::Store ALIAS jac313_Store)

target_include_directories(jac313_Store INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(jac313_Store INTERFACE cxx_std_23)

add_library(jac313_store_binary_read STATIC
    include/jac313/Store/v001/headers/persistence/BinaryEventLogReader.cpp
)
add_library(jac313::store_binary_read ALIAS jac313_store_binary_read)
target_include_directories(jac313_store_binary_read
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(jac313_store_binary_read PUBLIC cxx_std_23)
target_link_libraries(jac313_store_binary_read PUBLIC jac313_Store)

if(JAC313_STORE_JTEXT_PERSIST)
    # In the monorepo jtext_core is built first by jac313/jText. For a standalone
    # store build, pull the in-tree jac313/jText (library only).
    if(NOT TARGET jtext_core)
        set(JAC313_JTEXT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(JAC313_JTEXT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../jText
                         ${CMAKE_CURRENT_BINARY_DIR}/jText EXCLUDE_FROM_ALL)
    endif()

    add_library(jac313_store_jtext_persist STATIC
        include/jac313/Store/v001/headers/persistence/JTextSplitEventLog.cpp
        include/jac313/Store/v001/headers/persistence/BinaryEventLogReader_jtext.cpp
    )
    add_library(jac313::store_jtext_persist ALIAS jac313_store_jtext_persist)
    target_include_directories(jac313_store_jtext_persist
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_compile_features(jac313_store_jtext_persist PUBLIC cxx_std_23)
    # jtext_core (PUBLIC) brings jText.h on the include path transitively.
    target_link_libraries(jac313_store_jtext_persist PUBLIC
        jac313_Store
        jac313_store_binary_read
        jtext_core
    )
    message(STATUS "jac313::Store jText persistence ENABLED")
endif()

if(JAC313_STORE_SQL_PERSIST)
    add_library(jac313_store_sql_persist STATIC
        include/jac313/Store/v001/headers/persistence/SqlEventSink.cpp
    )
    add_library(jac313::store_sql_persist ALIAS jac313_store_sql_persist)
    target_include_directories(jac313_store_sql_persist
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_compile_features(jac313_store_sql_persist PUBLIC cxx_std_23)
    target_link_libraries(jac313_store_sql_persist PUBLIC jac313_Store)
    # In-tree jac313::Qlite (built first in the monorepo) provides the header + SQLite3.
    # Standalone store build: fall back to the sibling Qlite include dir + system SQLite3.
    if(TARGET jac313_Qlite)
        target_link_libraries(jac313_store_sql_persist PUBLIC jac313_Qlite)
    else()
        find_package(SQLite3 REQUIRED)
        target_include_directories(jac313_store_sql_persist
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Qlite/include)
        target_link_libraries(jac313_store_sql_persist PUBLIC SQLite::SQLite3)
    endif()
    message(STATUS "jac313::Store SQLite persistence ENABLED")
endif()

if(JAC313_STORE_BUILD_MODULES)
    if(NOT CMAKE_GENERATOR MATCHES "Ninja")
        message(FATAL_ERROR "JAC313_STORE_BUILD_MODULES requires the Ninja generator (-G Ninja)")
    endif()

    set(CMAKE_CXX_SCAN_FOR_MODULES ON)   # standalone-build safety (umbrella sets it too)
    add_library(jac313_store_module OBJECT)
    add_library(jac313::store_module ALIAS jac313_store_module)
    target_compile_features(jac313_store_module PUBLIC cxx_std_23)

    if(JAC313_STORE_IMPORT_STD)
        # Module-native `import std;` (gcc pilot). One interface unit over the guarded
        # headers + .cpp (self-contained for store's code), so it does NOT link the sink
        # static libs — only the include dirs + external defs (jtext_core, SQLite3) via
        # the umbrella INTERFACE targets.
        if(CMAKE_VERSION VERSION_LESS 3.30)
            message(FATAL_ERROR
                "JAC313_STORE_IMPORT_STD needs CMake >= 3.30 for native `import std;` "
                "(have ${CMAKE_VERSION}). See the root README \"Newer CMake on Linux Mint\".")
        endif()
        if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            message(FATAL_ERROR
                "JAC313_STORE_IMPORT_STD is a g++-15 pilot; compiler is "
                "${CMAKE_CXX_COMPILER_ID}. Leave it OFF for clang (textual wrapper).")
        endif()
        target_sources(jac313_store_module
            PUBLIC
                FILE_SET cxx_modules TYPE CXX_MODULES
                BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
                FILES modules/jac313.store.v001.importstd.cppm
        )
        set_target_properties(jac313_store_module PROPERTIES CXX_MODULE_STD ON)
        target_link_libraries(jac313_store_module PUBLIC jac313_Store)
        if(TARGET jac313_Qlite)
            target_link_libraries(jac313_store_module PUBLIC jac313_Qlite)
        endif()
        if(TARGET jac313_jText)
            target_link_libraries(jac313_store_module PUBLIC jac313_jText)
        endif()
        message(STATUS "jac313::Store module: import std ENABLED (native CXX_MODULE_STD)")
    else()
        # Textual-wrapper module (clang / fallback) — links the compiled sink libs.
        target_sources(jac313_store_module
            PUBLIC
                FILE_SET cxx_modules TYPE CXX_MODULES
                BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
                FILES modules/jac313.store.v001.cppm
        )
        target_link_libraries(jac313_store_module PUBLIC jac313_Store jac313_store_binary_read)
        if(TARGET jac313_store_sql_persist)
            target_link_libraries(jac313_store_module PUBLIC jac313_store_sql_persist)
        endif()
        if(TARGET jac313_store_jtext_persist)
            target_link_libraries(jac313_store_module PUBLIC jac313_store_jtext_persist)
        endif()
        if(TARGET jac313_Qlite)
            target_link_libraries(jac313_store_module PUBLIC jac313_Qlite)
        endif()
    endif()
endif()

if(JAC313_STORE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(JAC313_STORE_BUILD_TESTS OR JAC313_STORE_BUILD_MATRIX_TESTS)
    enable_testing()
endif()

if(JAC313_STORE_BUILD_TESTS OR JAC313_STORE_BUILD_MATRIX_TESTS)
    add_subdirectory(tests)
endif()

if(JAC313_STORE_BUILD_TESTS AND JAC313_STORE_BUILD_MODULES)
    add_subdirectory(tests/module)
endif()

message(STATUS "jac313::Store configured (in-tree core headers)")