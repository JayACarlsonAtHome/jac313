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
    jac313_test_cli/contract_modes.cpp
    jac313_test_cli/settings.cpp
    jac313_test_cli/vendor.cpp
    jac313_test_cli/run_identity.cpp
    jac313_test_cli/run_results.cpp
)
target_compile_features(jac313_test_cli PRIVATE cxx_std_23)
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
# jText reads the CLI's settings (settings.cpp: Setup/*.jtext, tests/*.jtext) and the PostgreSQL
# connections file (connections.cpp), so the CLI needs it.
if(NOT TARGET jac313_jText)
    message(FATAL_ERROR "jac313_test_cli reads its settings with jText: configure with JAC313_BUILD_JTEXT=ON")
endif()
target_link_libraries(jac313_test_cli PRIVATE jac313::jText)

# Unit tests for the tool's own primitives (RunIdentity, ...). Registered with ctest
# so they run as part of the standard suite.
add_executable(jac313_run_identity_test
    jac313_test_cli/run_identity.cpp
    jac313_test_cli/tests/run_identity_test.cpp
)
target_include_directories(jac313_run_identity_test PRIVATE jac313_test_cli)
target_compile_features(jac313_run_identity_test PRIVATE cxx_std_23)
add_test(NAME jac313_run_identity_test COMMAND jac313_run_identity_test)

# Regression: the OS-dedup bug, driven against a real SQLite DB (needs Qlite).
add_executable(jac313_identity_dedup_test
    jac313_test_cli/run_identity.cpp
    jac313_test_cli/tests/identity_dedup_test.cpp
)
target_include_directories(jac313_identity_dedup_test PRIVATE jac313_test_cli)
target_compile_features(jac313_identity_dedup_test PRIVATE cxx_std_23)
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_identity_dedup_test PRIVATE jac313::Qlite)
endif()
add_test(NAME jac313_identity_dedup_test COMMAND jac313_identity_dedup_test)

add_executable(jac313_host_group_test
    jac313_test_cli/tests/host_group_test.cpp
)
target_include_directories(jac313_host_group_test PRIVATE ${CMAKE_SOURCE_DIR}/shared)
target_compile_features(jac313_host_group_test PRIVATE cxx_std_23)
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_host_group_test PRIVATE jac313::Qlite)
endif()
add_test(NAME jac313_host_group_test COMMAND jac313_host_group_test)

# The settings tables (Setup/*.jtext, tests/*.jtext) read through settings.cpp, validated, and
# checked against one another (every recipe on every OS family, every toolchain activated).
add_executable(jac313_settings_tables_test
    jac313_test_cli/settings.cpp
    jac313_test_cli/tests/settings_tables_test.cpp
)
target_include_directories(jac313_settings_tables_test PRIVATE jac313_test_cli)
target_compile_features(jac313_settings_tables_test PRIVATE cxx_std_23)
target_link_libraries(jac313_settings_tables_test PRIVATE jac313::jText)
if(TARGET jac313::Setup)
    target_link_libraries(jac313_settings_tables_test PRIVATE jac313::Setup)
endif()
target_compile_definitions(jac313_settings_tables_test PRIVATE JAC313_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
add_test(NAME jac313_settings_tables_test COMMAND jac313_settings_tables_test)

# The PostgreSQL connections file (connections.cpp): the name column (absent in older files),
# writing it back, and the name naming the server in results.db.
add_executable(jac313_connections_test
    jac313_test_cli/connections.cpp
    jac313_test_cli/tests/connections_test.cpp
)
target_include_directories(jac313_connections_test PRIVATE jac313_test_cli ${CMAKE_SOURCE_DIR}/shared)
target_compile_features(jac313_connections_test PRIVATE cxx_std_23)
target_link_libraries(jac313_connections_test PRIVATE jac313::jText)
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_connections_test PRIVATE jac313::Qlite)
endif()
add_test(NAME jac313_connections_test COMMAND jac313_connections_test)

# The run summary JSON (summary.cpp): its contract and the "settings" list Jac313Runner shows.
add_executable(jac313_summary_settings_test
    jac313_test_cli/summary.cpp
    jac313_test_cli/tests/summary_settings_test.cpp
)
target_include_directories(jac313_summary_settings_test PRIVATE jac313_test_cli ${CMAKE_SOURCE_DIR}/shared)
target_compile_features(jac313_summary_settings_test PRIVATE cxx_std_23)
target_link_libraries(jac313_summary_settings_test PRIVATE jac313::jText)   # the failures jText (summary.cpp)
add_test(NAME jac313_summary_settings_test COMMAND jac313_summary_settings_test)

# Setup/jtext_table.sh: the sh + awk reader bsir.sh uses on compilers.jtext and recipes.jtext.
add_test(NAME jac313_jtext_table_sh_test COMMAND bash ${CMAKE_SOURCE_DIR}/Setup/jtext_table_test.sh)

add_executable(jac313_results_schema_test
    jac313_test_cli/tests/results_schema_test.cpp
)
target_include_directories(jac313_results_schema_test PRIVATE ${CMAKE_SOURCE_DIR}/shared)
target_compile_features(jac313_results_schema_test PRIVATE cxx_std_23)
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_results_schema_test PRIVATE jac313::Qlite)
endif()
add_test(NAME jac313_results_schema_test COMMAND jac313_results_schema_test)

# The production bench run's per-sink batches from io_best_fit (results::production_batches).
add_executable(jac313_production_batches_test
    jac313_test_cli/tests/production_batches_test.cpp
)
target_include_directories(jac313_production_batches_test PRIVATE ${CMAKE_SOURCE_DIR}/shared)
target_compile_features(jac313_production_batches_test PRIVATE cxx_std_23)
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_production_batches_test PRIVATE jac313::Qlite)
endif()
add_test(NAME jac313_production_batches_test COMMAND jac313_production_batches_test)

add_executable(jac313_instance_hash_test
    jac313_test_cli/tests/instance_hash_test.cpp
)
target_include_directories(jac313_instance_hash_test PRIVATE ${CMAKE_SOURCE_DIR}/shared)
target_compile_features(jac313_instance_hash_test PRIVATE cxx_std_23)
add_test(NAME jac313_instance_hash_test COMMAND jac313_instance_hash_test)

# jac313_wipe_all — standalone results.db reset tool (schema + testType enum kept;
# data + machine + os + current_host pin cleared). Built by bootstrap; symlinked at the v001 root.
add_executable(jac313_wipe_all jac313_wipe_all.cpp)
target_compile_features(jac313_wipe_all PRIVATE cxx_std_23)
target_include_directories(jac313_wipe_all PRIVATE ${CMAKE_SOURCE_DIR}/shared)   # jac313_results_db.hpp
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_wipe_all PRIVATE jac313::Qlite)
endif()

# jac313_wipe_one — delete ONE run (by run_id) + its testRun rows, then rebuild safeness
# (shared dimensions + host pin kept). Built by bootstrap; symlinked at the v001 root.
add_executable(jac313_wipe_one jac313_wipe_one.cpp)
target_compile_features(jac313_wipe_one PRIVATE cxx_std_23)
target_include_directories(jac313_wipe_one PRIVATE ${CMAKE_SOURCE_DIR}/shared)   # jac313_results_db.hpp
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_wipe_one PRIVATE jac313::Qlite)
endif()

# jac313_wipe_jac — wipe a WHOLE machine (machine_id): all its runs + testRuns + every row
# that names it (machine, io_best_fit, package_version, fleet_sync, current_host pin). Built by bootstrap; symlinked at the v001 root.
add_executable(jac313_wipe_jac jac313_wipe_jac.cpp)
target_compile_features(jac313_wipe_jac PRIVATE cxx_std_23)
target_include_directories(jac313_wipe_jac PRIVATE ${CMAKE_SOURCE_DIR}/shared)   # jac313_results_db.hpp
if(TARGET jac313::Qlite)
    target_link_libraries(jac313_wipe_jac PRIVATE jac313::Qlite)
endif()

# COMPILED-IN-DIR: where these programs were compiled; each refuses to run anywhere else.
foreach(tool jac313_test_cli jac313_wipe_all jac313_wipe_one jac313_wipe_jac)
    target_compile_definitions(${tool} PRIVATE JAC313_COMPILED_IN_DIR="${CMAKE_SOURCE_DIR}")
    target_include_directories(${tool} PRIVATE ${CMAKE_SOURCE_DIR}/shared)
endforeach()
