cmake_minimum_required(VERSION 3.26)

project(jac313_Store VERSION 0.1.0 LANGUAGES CXX)

# jac313::Store — core in-tree implementation (jac313::Store::v001 namespace).

option(JAC313_STORE_BUILD_EXAMPLES "Build example programs" ON)
option(JAC313_STORE_BUILD_TESTS "Build and register tests" OFF)
option(JAC313_STORE_BUILD_MATRIX_TESTS
       "Build ported ts_store matrix tests (001-008 TS/XS, flags)" ON)
option(JAC313_STORE_BUILD_MODULES "Build jac313.store.v001 C++23 module (in-tree)" OFF)
option(JAC313_STORE_JTEXT_PERSIST "Build in-tree jText split persistence (needs jac313::jText)" OFF)
option(JAC313_STORE_SQL_PERSIST "Build in-tree SQLite persistence (needs jac313::Qlite)" OFF)
# PostgreSQL persistence (pg-local / pg-remote backends): ON by default, built only when libpq 17+
# is found (PGDG installs under /usr/pgsql-18, then 17; 18 is preferred). Optional, so Store builds
# without PostgreSQL.
option(JAC313_STORE_PG_PERSIST "Build PostgreSQL persistence when libpq 17+ is found" ON)

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

add_library(jac313_store_html_persist STATIC
    include/jac313/Store/v001/headers/persistence/HtmlSplitEventLog.cpp
)
add_library(jac313::store_html_persist ALIAS jac313_store_html_persist)
target_include_directories(jac313_store_html_persist
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(jac313_store_html_persist PUBLIC cxx_std_23)
target_link_libraries(jac313_store_html_persist PUBLIC jac313_Store)
message(STATUS "jac313::Store HTML persistence ENABLED")

add_library(jac313_store_json_persist STATIC
    include/jac313/Store/v001/headers/persistence/JsonSplitEventLog.cpp
    include/jac313/Store/v001/headers/persistence/JsonEventLogReader.cpp
)
add_library(jac313::store_json_persist ALIAS jac313_store_json_persist)
target_include_directories(jac313_store_json_persist
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(jac313_store_json_persist PUBLIC cxx_std_23)
target_link_libraries(jac313_store_json_persist PUBLIC jac313_Store)
message(STATUS "jac313::Store JSON persistence ENABLED")

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

if(JAC313_STORE_PG_PERSIST)
    find_path(JAC313_LIBPQ_INCLUDE_DIR libpq-fe.h
              HINTS /usr/pgsql-18/include /usr/pgsql-17/include
              PATH_SUFFIXES postgresql pgsql)
    find_library(JAC313_LIBPQ_LIBRARY NAMES pq
                 HINTS /usr/pgsql-18/lib /usr/pgsql-17/lib)
    set(_jac313_libpq_ok FALSE)
    if(JAC313_LIBPQ_INCLUDE_DIR AND JAC313_LIBPQ_LIBRARY)
        # The version macro is in pg_config.h, or (RHEL multilib) pg_config_<arch>.h.
        file(GLOB _jac313_pg_cfgs "${JAC313_LIBPQ_INCLUDE_DIR}/pg_config*.h")
        set(_jac313_pg_major "")
        foreach(_cfg IN LISTS _jac313_pg_cfgs)
            file(STRINGS "${_cfg}" _jac313_pg_ver_line REGEX "^#define PG_MAJORVERSION_NUM [0-9]+")
            if(_jac313_pg_ver_line)
                string(REGEX MATCH "[0-9]+$" _jac313_pg_major "${_jac313_pg_ver_line}")
                break()
            endif()
        endforeach()
        if(_jac313_pg_major AND _jac313_pg_major GREATER_EQUAL 17)
            set(_jac313_libpq_ok TRUE)
        endif()
    endif()
    if(_jac313_libpq_ok)
        add_library(jac313_store_pg_persist STATIC
            include/jac313/Store/v001/headers/persistence/PgClient.cpp
            include/jac313/Store/v001/headers/persistence/PgEventSink.cpp
        )
        add_library(jac313::store_pg_persist ALIAS jac313_store_pg_persist)
        target_include_directories(jac313_store_pg_persist
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
            PRIVATE ${JAC313_LIBPQ_INCLUDE_DIR}
        )
        target_compile_features(jac313_store_pg_persist PUBLIC cxx_std_23)
        target_link_libraries(jac313_store_pg_persist PUBLIC jac313_Store ${JAC313_LIBPQ_LIBRARY})
        message(STATUS "jac313::Store PostgreSQL persistence ENABLED (libpq ${_jac313_pg_major}: ${JAC313_LIBPQ_LIBRARY})")
    else()
        message(STATUS "jac313::Store PostgreSQL persistence DISABLED (libpq 17+ not found; ./bootstrap.sh provisions it)")
    endif()
endif()

if(JAC313_STORE_BUILD_MODULES)
    if(NOT CMAKE_GENERATOR MATCHES "Ninja")
        message(FATAL_ERROR "JAC313_STORE_BUILD_MODULES requires the Ninja generator (-G Ninja)")
    endif()

    set(CMAKE_CXX_SCAN_FOR_MODULES ON)   # standalone-build safety (umbrella sets it too)
    add_library(jac313_store_module OBJECT)
    add_library(jac313::store_module ALIAS jac313_store_module)
    target_compile_features(jac313_store_module PUBLIC cxx_std_23)

    # Textual-wrapper module — links the compiled sink libs.
    target_sources(jac313_store_module
        PUBLIC
            FILE_SET cxx_modules TYPE CXX_MODULES
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
            FILES modules/jac313.store.v001.cppm
    )
    target_link_libraries(jac313_store_module PUBLIC jac313_Store jac313_store_binary_read jac313_store_html_persist jac313_store_json_persist)
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