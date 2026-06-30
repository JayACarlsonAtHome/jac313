add_executable(jac313_test_cli
    jac313_test_cli/main.cpp
    jac313_test_cli/manifest.cpp
    jac313_test_cli/host_hardware.cpp
    jac313_test_cli/runner.cpp
    jac313_test_cli/report.cpp
    jac313_test_cli/process.cpp
    jac313_test_cli/compiler.cpp
    jac313_test_cli/platform.cpp
    jac313_test_cli/params.cpp
    jac313_test_cli/matrix.cpp
    jac313_test_cli/build_pipeline.cpp
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

# C++26 contracts-style shim (jac313::contracts) — runtime pre/post/assert today,
# migrates to native P2900 contracts when __cpp_contracts lands. Header-only; the
# test just needs the Setup include path.
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

# On the gcc16 axis, build the contracts tests with NATIVE ISO C++26 contracts
# (P2900): -fcontracts defines __cpp_contracts (so the shim maps to contract_assert
# and the death-test's native pre() lights up), under the 'enforce' evaluation
# semantic so a violated contract terminates. clang/gcc15 keep the runtime fallback.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 16)
    set(_jac313_contract_flags -fcontracts -fcontract-evaluation-semantic=enforce)
    target_compile_options(jac313_contracts_test          PRIVATE ${_jac313_contract_flags})
    target_compile_options(jac313_contracts_disabled_test PRIVATE ${_jac313_contract_flags})
    target_compile_options(jac313_contracts_enforce_test  PRIVATE ${_jac313_contract_flags})
    # gcc16 links in the default contract-violation handler only when -fcontracts
    # is on the LINK line too (otherwise: undefined reference handle_contract_violation).
    target_link_options(jac313_contracts_test          PRIVATE -fcontracts)
    target_link_options(jac313_contracts_disabled_test PRIVATE -fcontracts)
    target_link_options(jac313_contracts_enforce_test  PRIVATE -fcontracts)
endif()

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