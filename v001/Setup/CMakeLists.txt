cmake_minimum_required(VERSION 3.26)
project(jac313_Setup VERSION 0.1.0 LANGUAGES CXX)

# jac313::Setup — standalone, data-driven, activation-aware toolchain sensing
# (jac313::Setup::v001). Self-contained: its own command-runner, no deps beyond the
# C++23 standard library, so it can be reused by any build tooling and via the module
# `import jac313.setup.v001;`. Must build before consumers (e.g. jac313_test_cli).

option(JAC313_SETUP_BUILD_TESTS "Build and register tests" OFF)
option(JAC313_SETUP_BUILD_MODULES "Build jac313.setup.v001 C++23 module" OFF)

add_library(jac313_Setup STATIC
    src/v001.cpp
)
add_library(jac313::Setup ALIAS jac313_Setup)

target_include_directories(jac313_Setup PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(jac313_Setup PUBLIC cxx_std_23)

if(JAC313_SETUP_BUILD_MODULES)
    if(NOT CMAKE_GENERATOR MATCHES "Ninja")
        message(FATAL_ERROR "JAC313_SETUP_BUILD_MODULES requires the Ninja generator (-G Ninja)")
    endif()

    add_library(jac313_setup_module OBJECT)
    add_library(jac313::Setup_module ALIAS jac313_setup_module)
    target_sources(jac313_setup_module
        PUBLIC
            FILE_SET cxx_modules TYPE CXX_MODULES
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
            FILES modules/jac313.setup.v001.cppm
    )
    target_include_directories(jac313_setup_module PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_compile_features(jac313_setup_module PUBLIC cxx_std_23)
    target_link_libraries(jac313_setup_module PUBLIC jac313_Setup)
endif()

include(GNUInstallDirs)
install(TARGETS jac313_Setup EXPORT jac313SetupTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT jac313SetupTargets
    FILE jac313SetupTargets.cmake
    NAMESPACE jac313::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/jac313_Setup
)

if(JAC313_SETUP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "jac313::Setup configured (standalone toolchain sensing)")
