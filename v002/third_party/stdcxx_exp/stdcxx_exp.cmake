# libstdc++exp for an OS whose GCC 16 does not ship it. Included by v002/CMakeLists.txt where
# -fcontracts goes on the link line. Locked per OS: each OS that needs it has its own folder here
# with the source and a LOCKED.jtext that says why. Nothing is generic: an OS without a folder,
# or a compiler that already has libstdc++exp.a, gets nothing from this file.
#
#   rhel9/   RHEL 9:  gcc-toolset-16 deletes libstdc++exp.a (rhel9/LOCKED.jtext).
#   rhel10/  RHEL 10: gcc-toolset-16 deletes libstdc++exp.a (rhel10/LOCKED.jtext).

set(_jac313_os_id "")
set(_jac313_os_major "")
if(EXISTS "/etc/os-release")
    file(STRINGS "/etc/os-release" _jac313_os_lines REGEX "^(ID|VERSION_ID)=")
    foreach(_line IN LISTS _jac313_os_lines)
        if(_line MATCHES "^ID=\"?([^\"]*)\"?$")
            set(_jac313_os_id "${CMAKE_MATCH_1}")
        elseif(_line MATCHES "^VERSION_ID=\"?([0-9]+)")
            set(_jac313_os_major "${CMAKE_MATCH_1}")
        endif()
    endforeach()
endif()

set(_jac313_exp_dir "")
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 16 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 17)
    if(_jac313_os_id STREQUAL "rhel" AND _jac313_os_major STREQUAL "9")
        set(_jac313_exp_dir "${CMAKE_CURRENT_LIST_DIR}/rhel9")
    elseif(_jac313_os_id STREQUAL "rhel" AND _jac313_os_major STREQUAL "10")
        set(_jac313_exp_dir "${CMAKE_CURRENT_LIST_DIR}/rhel10")
    endif()
endif()

if(_jac313_exp_dir)
    execute_process(COMMAND "${CMAKE_CXX_COMPILER}" -print-file-name=libstdc++exp.a
                    OUTPUT_VARIABLE _jac313_have_exp OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(IS_ABSOLUTE "${_jac313_have_exp}" AND EXISTS "${_jac313_have_exp}")
        # The toolset ships it after all: use the toolset's, and the locked copy is unused.
        message(STATUS "libstdc++exp: ${_jac313_have_exp} (the toolset's; ${_jac313_exp_dir} not used)")
    else()
        # Built at configure time, outside every target, so jac313's -Werror / strict flags never
        # apply to GCC's own source. Same compiler, same -std, same -fcontracts as the tree.
        set(_out "${CMAKE_BINARY_DIR}/stdcxx_exp")
        file(MAKE_DIRECTORY "${_out}")
        # A change to the locked source re-runs the configure, which rebuilds the library.
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_jac313_exp_dir}/contract26.cc")
        execute_process(
            COMMAND "${CMAKE_CXX_COMPILER}" -std=c++26 -fcontracts -O2 -fPIC -g
                    -c "${_jac313_exp_dir}/contract26.cc" -o "${_out}/contract26.o"
            RESULT_VARIABLE _rc ERROR_VARIABLE _err)
        if(NOT _rc EQUAL 0)
            message(FATAL_ERROR "libstdc++exp: compiling ${_jac313_exp_dir}/contract26.cc failed:\n${_err}")
        endif()
        file(REMOVE "${_out}/libstdc++exp.a")
        execute_process(COMMAND "${CMAKE_AR}" rcs "${_out}/libstdc++exp.a" "${_out}/contract26.o"
                        RESULT_VARIABLE _rc ERROR_VARIABLE _err)
        if(NOT _rc EQUAL 0)
            message(FATAL_ERROR "libstdc++exp: ${CMAKE_AR} failed:\n${_err}")
        endif()
        # g++ adds -lstdc++exp itself when -fcontracts is on the link line; -L lets it find this one.
        add_link_options("-L${_out}")
        message(STATUS "libstdc++exp: built ${_out}/libstdc++exp.a from ${_jac313_exp_dir}/contract26.cc "
                       "(this OS's GCC 16 has none; ${_jac313_exp_dir}/LOCKED.jtext)")
    endif()
endif()
