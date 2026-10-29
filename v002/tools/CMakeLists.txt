add_executable(jac313_test_cli
    jac313_test_cli/main.cpp
    jac313_test_cli/manifest.cpp
    jac313_test_cli/host_hardware.cpp
    jac313_test_cli/host_pin.cpp
    jac313_test_cli/fleet_registry.cpp
    jac313_test_cli/runner.cpp
    jac313_test_cli/report.cpp
    jac313_test_cli/process.cpp
    jac313_test_cli/compiler.cpp
    jac313_test_cli/platform.cpp
    jac313_test_cli/params.cpp
    jac313_test_cli/matrix.cpp
    jac313_test_cli/kept_logs.cpp
    jac313_test_cli/test_logs.cpp
    jac313_test_cli/build_pipeline.cpp
    jac313_test_cli/summary.cpp
    jac313_test_cli/connections.cpp
    jac313_test_cli/vendor.cpp
    jac313_test_cli/run_identity.cpp
)
target_compile_features(jac313_test_cli PRIVATE cxx_std_26)
target_include_directories(jac313_test_cli PRIVATE ${CMAKE_SOURCE_DIR}/shared)   # jac313_results_db.hpp
if(TARGET jac313::Setup)
    target_link_libraries(jac313_test_cli PRIVATE jac313::Setup)
endif()
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_test_cli PRIVATE jac313::Qlite)
endif()
# libpq (Store's PgClient) for the fleet registry on the shared server (fleet_registry.cpp)
if(TARGET jac313::store_pg_persist)
    target_link_libraries(jac313_test_cli PRIVATE jac313::store_pg_persist)
    target_compile_definitions(jac313_test_cli PRIVATE JAC313_TEST_CLI_HAS_PG=1)
endif()
# jText reads the PostgreSQL connections file (connections.cpp)
if(TARGET jac313_jText)
    target_link_libraries(jac313_test_cli PRIVATE jac313::jText)
    target_compile_definitions(jac313_test_cli PRIVATE JAC313_TEST_CLI_HAS_JTEXT=1)
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

add_executable(jac313_host_group_test
    jac313_test_cli/tests/host_group_test.cpp
)
target_include_directories(jac313_host_group_test PRIVATE ${CMAKE_SOURCE_DIR}/shared)
target_compile_features(jac313_host_group_test PRIVATE cxx_std_26)
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_host_group_test PRIVATE jac313::Qlite)
endif()
add_test(NAME jac313_host_group_test COMMAND jac313_host_group_test)

add_executable(jac313_instance_hash_test
    jac313_test_cli/tests/instance_hash_test.cpp
)
target_include_directories(jac313_instance_hash_test PRIVATE ${CMAKE_SOURCE_DIR}/shared)
target_compile_features(jac313_instance_hash_test PRIVATE cxx_std_26)
add_test(NAME jac313_instance_hash_test COMMAND jac313_instance_hash_test)

# Contracts (jac313::contracts): native P2900 contract_assert on gcc16, runtime
# pre/post/assert on clang/gcc15. Header-only; the test just needs the Setup include path.
add_executable(jac313_contracts_test
    jac313_test_cli/tests/contracts_test.cpp
)
target_compile_features(jac313_contracts_test PRIVATE cxx_std_26)
if(TARGET jac313::Setup)
    target_link_libraries(jac313_contracts_test PRIVATE jac313::Setup)
endif()
add_test(NAME jac313_contracts_test COMMAND jac313_contracts_test)

# Same shim with JAC313_CONTRACTS_ENABLED=0 — verifies the checks compile out to no-ops.
add_executable(jac313_contracts_disabled_test
    jac313_test_cli/tests/contracts_disabled_test.cpp
)
target_compile_features(jac313_contracts_disabled_test PRIVATE cxx_std_26)
if(TARGET jac313::Setup)
    target_link_libraries(jac313_contracts_disabled_test PRIVATE jac313::Setup)
endif()
add_test(NAME jac313_contracts_disabled_test COMMAND jac313_contracts_disabled_test)

# Death-test: a VIOLATED contract must terminate the program (covers the enforce/
# abort path the happy-path test omits). Forks a child that violates a contract.
add_executable(jac313_contracts_enforce_test
    jac313_test_cli/tests/contracts_enforce_test.cpp)
target_compile_features(jac313_contracts_enforce_test PRIVATE cxx_std_26)
if(TARGET jac313::Setup)
    target_link_libraries(jac313_contracts_enforce_test PRIVATE jac313::Setup)
endif()
add_test(NAME jac313_contracts_enforce_test COMMAND jac313_contracts_enforce_test)
# Native contracts on gcc16 come from the top-level CMakeLists.txt (-fcontracts on the whole tree).

# jac313_wipe_all — standalone results.db reset tool (schema + testType enum kept;
# data + host_spec + current_host pin cleared). Built by bootstrap; symlinked at the v002 root.
add_executable(jac313_wipe_all jac313_wipe_all.cpp)
target_compile_features(jac313_wipe_all PRIVATE cxx_std_26)
target_include_directories(jac313_wipe_all PRIVATE ${CMAKE_SOURCE_DIR}/shared)   # jac313_results_db.hpp
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_wipe_all PRIVATE jac313::Qlite)
endif()

# jac313_wipe_one — delete ONE run (by run_id) + its testRun rows, then rebuild safeness
# (shared dimensions + host pin kept). Built by bootstrap; symlinked at the v002 root.
add_executable(jac313_wipe_one jac313_wipe_one.cpp)
target_compile_features(jac313_wipe_one PRIVATE cxx_std_26)
target_include_directories(jac313_wipe_one PRIVATE ${CMAKE_SOURCE_DIR}/shared)   # jac313_results_db.hpp
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_wipe_one PRIVATE jac313::Qlite)
endif()

# jac313_wipe_jac — wipe a WHOLE machine (group_id): all its runs + testRuns + specs
# (host_spec, io_best_fit, current_host pin). Built by bootstrap; symlinked at the v002 root.
add_executable(jac313_wipe_jac jac313_wipe_jac.cpp)
target_compile_features(jac313_wipe_jac PRIVATE cxx_std_26)
target_include_directories(jac313_wipe_jac PRIVATE ${CMAKE_SOURCE_DIR}/shared)   # jac313_results_db.hpp
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_wipe_jac PRIVATE jac313::Qlite)
endif()