if(JAC313_STORE_BUILD_TESTS)
    add_executable(jac313_store_v002_test v002_test.cpp)
    target_link_libraries(jac313_store_v002_test PRIVATE jac313::Store)
    add_test(NAME jac313_store_v002_test COMMAND jac313_store_v002_test)
    set_tests_properties(jac313_store_v002_test PROPERTIES TIMEOUT 120)

    # A store filled past its capacity must terminate (contract in save_event).
    add_executable(jac313_store_contracts_test contracts_store_test.cpp)
    target_link_libraries(jac313_store_contracts_test PRIVATE jac313::Store)
    add_test(NAME jac313_store_contracts_test COMMAND jac313_store_contracts_test)
    if(NOT JAC313_CONTRACTS_TERMINATE)   # observe / ignore: a violation does not end the program
        set_tests_properties(jac313_store_contracts_test PROPERTIES DISABLED TRUE)
    endif()

    add_executable(jac313_store_metric_view_test metric_view_test.cpp)
    target_link_libraries(jac313_store_metric_view_test PRIVATE jac313::Store)
    add_test(NAME jac313_store_metric_view_test COMMAND jac313_store_metric_view_test)

    add_executable(jac313_store_json_reader_test json_reader_test.cpp)
    target_link_libraries(jac313_store_json_reader_test PRIVATE
        jac313::Store
        jac313::store_json_persist
    )
    add_test(NAME jac313_store_json_reader_test COMMAND jac313_store_json_reader_test)
    set_tests_properties(jac313_store_json_reader_test PROPERTIES TIMEOUT 60)

    add_executable(jac313_store_binary_reader_test binary_reader_test.cpp)
    target_link_libraries(jac313_store_binary_reader_test PRIVATE
        jac313::Store
        jac313::store_binary_read
    )
    if(TARGET jac313_store_jtext_persist)
        target_compile_definitions(jac313_store_binary_reader_test PRIVATE JAC313_HAS_JTEXT_PERSIST=1)
        target_link_libraries(jac313_store_binary_reader_test PRIVATE jac313::store_jtext_persist)
    endif()
    add_test(NAME jac313_store_binary_reader_test COMMAND jac313_store_binary_reader_test)

    if(TARGET jac313_store_jtext_persist)
        add_executable(jac313_store_jtext_text_test jtext_text_test.cpp)
        target_link_libraries(jac313_store_jtext_text_test PRIVATE jac313::Store jac313::store_jtext_persist)
        add_test(NAME jac313_store_jtext_text_test COMMAND jac313_store_jtext_text_test)
    endif()
    # Toughened limit from test_limits.txt table (reader was part of the stall path)
    set_tests_properties(jac313_store_binary_reader_test PROPERTIES TIMEOUT 60)

    # C++26 saturating-arithmetic safety for ts_store capacity / memory guard.
    add_executable(jac313_store_saturating_capacity_test saturating_capacity_test.cpp)
    target_link_libraries(jac313_store_saturating_capacity_test PRIVATE jac313::Store)
    add_test(NAME jac313_store_saturating_capacity_test COMMAND jac313_store_saturating_capacity_test)
endif()

if(JAC313_STORE_BUILD_MATRIX_TESTS)
    add_subdirectory(matrix)
endif()