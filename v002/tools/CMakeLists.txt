add_executable(jac313_test_cli
    jac313_test_cli/main.cpp
    jac313_test_cli/manifest.cpp
    jac313_test_cli/matrix_manifest.cpp
    jac313_test_cli/host_hardware.cpp
    jac313_test_cli/matrix_log_metrics.cpp
    jac313_test_cli/results_db.cpp
    jac313_test_cli/test_summary_render.cpp
    jac313_test_cli/runner.cpp
    jac313_test_cli/report.cpp
    jac313_test_cli/process.cpp
    jac313_test_cli/compiler.cpp
    jac313_test_cli/platform.cpp
    jac313_test_cli/params.cpp
    jac313_test_cli/matrix.cpp
    jac313_test_cli/build_pipeline.cpp
    jac313_test_cli/build_benchmark.cpp
    jac313_test_cli/run_identity.cpp
)
target_compile_features(jac313_test_cli PRIVATE cxx_std_26)
if(TARGET jac313::Setup)
    target_link_libraries(jac313_test_cli PRIVATE jac313::Setup)
endif()
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_test_cli PRIVATE jac313::Qlite)
endif()

# Unit tests for the tool's own primitives (RunIdentity, ...). Registered with ctest
# so they run as part of the standard suite.
add_executable(jac313_run_identity_test
    jac313_test_cli/run_identity.cpp
    jac313_test_cli/tests/run_identity_test.cpp
)
target_include_directories(jac313_run_identity_test PRIVATE jac313_test_cli)
target_compile_features(jac313_run_identity_test PRIVATE cxx_std_26)
add_test(NAME jac313_run_identity_test COMMAND jac313_run_identity_test)

# Regression: the OS-dedup bug, driven against a real SQLite DB (needs Qlite).
add_executable(jac313_identity_dedup_test
    jac313_test_cli/run_identity.cpp
    jac313_test_cli/tests/identity_dedup_test.cpp
)
target_include_directories(jac313_identity_dedup_test PRIVATE jac313_test_cli)
target_compile_features(jac313_identity_dedup_test PRIVATE cxx_std_26)
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_identity_dedup_test PRIVATE jac313::Qlite)
endif()
add_test(NAME jac313_identity_dedup_test COMMAND jac313_identity_dedup_test)