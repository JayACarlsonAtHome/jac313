if(JAC313_STORE_BUILD_TESTS)
    add_executable(jac313_store_v001_test v001_test.cpp)
    target_link_libraries(jac313_store_v001_test PRIVATE jac313::Store)
    add_test(NAME jac313_store_v001_test COMMAND jac313_store_v001_test)

    add_executable(jac313_store_metric_view_test metric_view_test.cpp)
    target_link_libraries(jac313_store_metric_view_test PRIVATE jac313::Store)
    add_test(NAME jac313_store_metric_view_test COMMAND jac313_store_metric_view_test)

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
endif()

if(JAC313_STORE_BUILD_MATRIX_TESTS)
    add_subdirectory(matrix)
endif()