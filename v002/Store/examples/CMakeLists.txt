add_executable(jac313_store_v002_example v002_example.cpp)
target_link_libraries(jac313_store_v002_example PRIVATE jac313::Store)

macro(jac313_store_add_smoke_test target)
    if(TARGET ${target})
        add_test(NAME ${target} COMMAND ${target})
        # Time limits from tests/test_limits.txt (prevents infinite runs / stalls).
        # The binary_to_jtext family gets short timeouts because they exercise
        # the reader + convert path that previously could hang the machine.
        if("${target}" MATCHES "binary_to_jtext|binary_persist|jtext_persist|flag_routing")
            set_tests_properties(${target} PROPERTIES TIMEOUT 30)
        else()
            set_tests_properties(${target} PROPERTIES TIMEOUT 60)
        endif()
    endif()
endmacro()

add_executable(jac313_store_binary_persist_smoke binary_persist_smoke.cpp)
target_link_libraries(jac313_store_binary_persist_smoke PRIVATE
    jac313::Store
    jac313::store_binary_read
)
jac313_store_add_smoke_test(jac313_store_binary_persist_smoke)

add_executable(jac313_store_html_persist_smoke html_persist_smoke.cpp)
target_link_libraries(jac313_store_html_persist_smoke PRIVATE
    jac313::Store
    jac313::store_html_persist
)
jac313_store_add_smoke_test(jac313_store_html_persist_smoke)

if(TARGET jac313_store_jtext_persist)
    add_executable(jac313_store_jtext_persist_smoke jtext_persist_smoke.cpp)
    target_link_libraries(jac313_store_jtext_persist_smoke PRIVATE
        jac313::Store
        jac313::store_jtext_persist
    )
    jac313_store_add_smoke_test(jac313_store_jtext_persist_smoke)

    add_executable(jac313_store_binary_to_jtext_smoke binary_to_jtext_smoke.cpp)
    target_link_libraries(jac313_store_binary_to_jtext_smoke PRIVATE
        jac313::Store
        jac313::store_binary_read
        jac313::store_jtext_persist
    )
    jac313_store_add_smoke_test(jac313_store_binary_to_jtext_smoke)
endif()

if(TARGET jac313_store_sql_persist)
    add_executable(jac313_store_sql_persist_smoke sql_persist_smoke.cpp)
    target_link_libraries(jac313_store_sql_persist_smoke PRIVATE
        jac313::Store
        jac313::store_sql_persist
    )
    jac313_store_add_smoke_test(jac313_store_sql_persist_smoke)

    add_executable(jac313_store_flag_routing_smoke flag_routing_smoke.cpp)
    target_link_libraries(jac313_store_flag_routing_smoke PRIVATE
        jac313::Store
        jac313::store_binary_read
        jac313::store_sql_persist
    )
    jac313_store_add_smoke_test(jac313_store_flag_routing_smoke)
endif()