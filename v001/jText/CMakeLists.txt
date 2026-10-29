cmake_minimum_required(VERSION 3.26)

project(jac313_jText VERSION 0.1.0 LANGUAGES CXX)

# jac313::jText — in-tree structured-text library.
# Migrated from the legacy sibling jText repo; no sibling checkout needed.
# The compiled core (jtext_core) lives here; the public API is the global
# namespace (::JTextWriter, ...), wrapped under jac313::jText::v001 by the umbrella.

option(JAC313_JTEXT_BUILD_EXAMPLES "Build example programs" ON)
option(JAC313_JTEXT_BUILD_TESTS "Build and register tests" ON)
option(JAC313_JTEXT_BUILD_TOOLS "Build the jText command-line tools (jtext_xtox_sql, jtext_from_sql)" ON)
option(JAC313_JTEXT_BUILD_MODULES "Build jac313.jtext.v001 C++23 modules" OFF)

# In-tree compiled core (keeps the target name jtext_core so existing consumers,
# e.g. store's jText persistence, link unchanged).
if(NOT TARGET jtext_core)
    add_library(jtext_core STATIC
        jText.cpp
        src/parser/line.cpp
        src/parser/section.cpp
        src/parser/close.cpp
        src/validator/validator.cpp
        src/export/xml_export.cpp
    )
    target_include_directories(jtext_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}        # jText.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src    # parser/, validator/
    )
    target_compile_features(jtext_core PUBLIC cxx_std_23)
    set_target_properties(jtext_core PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
endif()

add_library(jac313_jText INTERFACE)
add_library(jac313::jText ALIAS jac313_jText)

target_include_directories(jac313_jText INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(jac313_jText INTERFACE cxx_std_23)
target_link_libraries(jac313_jText INTERFACE jtext_core)

if(JAC313_JTEXT_BUILD_MODULES)
    # Enable module dependency scanning for this directory tree (incl. the module
    # test). The umbrella root sets this; setting it here too makes a STANDALONE
    # jText module build work.
    set(CMAKE_CXX_SCAN_FOR_MODULES ON)

    add_library(jac313_jtext_module OBJECT)
    add_library(jac313::jText_module ALIAS jac313_jtext_module)

    # Textual partition modules — include jText.h in the global
    # module fragment, re-export under the namespace, and link the compiled core.
    target_sources(jac313_jtext_module
        PUBLIC
            FILE_SET cxx_modules TYPE CXX_MODULES
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
            FILES
                modules/jac313.jtext.v001.core.cppm
                modules/jac313.jtext.v001.reader.cppm
                modules/jac313.jtext.v001.writer.cppm
                modules/jac313.jtext.v001.cppm
    )
    target_link_libraries(jac313_jtext_module PUBLIC jac313_jText)

    target_compile_features(jac313_jtext_module PUBLIC cxx_std_23)
endif()

if(JAC313_JTEXT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(JAC313_JTEXT_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if(JAC313_JTEXT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(JAC313_JTEXT_BUILD_MODULES AND JAC313_JTEXT_BUILD_TESTS)
    add_subdirectory(tests/module)
endif()

message(STATUS "jac313::jText configured (in-tree)")