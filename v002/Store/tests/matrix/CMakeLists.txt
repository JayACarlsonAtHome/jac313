# Ported ts_store matrix tests (001–008 TS/XS + flags) for jac313::Store::v002.

set(_jac313_matrix_nums 001 002 003 004 005 006 007 008)

# Opt-in: compile the lock-free matrix tests (003/006) with helgrind/DRD happens-before
# annotations so the thread checkers can see the atomic producer→consumer handoff.
# OFF by default — keeps the normal build free of any valgrind header dependency
# (the annotation macros fall back to no-ops). Build a separate tree with
# -DJAC313_STORE_HELGRIND_ANNOTATE=ON to run a clean helgrind/DRD pass.
option(JAC313_STORE_HELGRIND_ANNOTATE
       "Annotate lock-free matrix tests for helgrind/DRD (needs valgrind headers)" OFF)

function(jac313_store_link_matrix_target target)
    target_link_libraries(${target} PRIVATE jac313::Store)
    if(JAC313_STORE_HELGRIND_ANNOTATE)
        target_compile_definitions(${target} PRIVATE JAC313_HELGRIND_ANNOTATE=1)
    endif()
    if(TARGET jac313_store_jtext_persist)
        target_link_libraries(${target} PRIVATE jac313::store_jtext_persist)
        target_compile_definitions(${target} PRIVATE JAC313_STORE_HAS_JTEXT_PERSIST=1)
    endif()
    if(TARGET jac313_store_sql_persist)
        target_link_libraries(${target} PRIVATE jac313::store_sql_persist)
        target_compile_definitions(${target} PRIVATE JAC313_STORE_HAS_SQL_PERSIST=1)
    endif()
endfunction()

add_executable(jac313_store_flags test_flags.cpp)
target_compile_features(jac313_store_flags PRIVATE cxx_std_26)
jac313_store_link_matrix_target(jac313_store_flags)
add_test(NAME jac313_store_flags
set_tests_properties(jac313_store_flags PROPERTIES TIMEOUT 30)
         COMMAND jac313_store_flags --no-interactive --test-size=smoke)

# Named matrix item (like flags): per-event metric summary view at scale
# (smoke = 100 records, full = 100,000 via --test-size).
add_executable(jac313_store_metric_view test_metric_view.cpp)
target_compile_features(jac313_store_metric_view PRIVATE cxx_std_26)
jac313_store_link_matrix_target(jac313_store_metric_view)
add_test(NAME jac313_store_metric_view
         COMMAND jac313_store_metric_view --no-interactive --test-size=smoke)

# store_bench — generic, parameterized throughput instrument (single config or a
# per-machine --sweep). Honest measurement: flush-and-wait inside the timed region,
# verify last-run-only, distribution (median + low-high band) per config. Not a
# pass/fail gate; the smoke test just proves it runs. Drive the curated suite with
# store_bench --suite (hardware + version header + the flag/durable grids).
add_executable(jac313_store_bench store_bench.cpp)
target_compile_features(jac313_store_bench PRIVATE cxx_std_26)
target_include_directories(jac313_store_bench PRIVATE ${CMAKE_SOURCE_DIR}/shared)   # jac313_results_db.hpp
jac313_store_link_matrix_target(jac313_store_bench)
add_test(NAME jac313_store_bench_smoke
         COMMAND jac313_store_bench --threads 4 --events-per-thread 200 --runs 3 --persist none)

foreach(num IN LISTS _jac313_matrix_nums)
    add_executable(jac313_store_${num}_TS test_${num}_TS.cpp)
    target_compile_features(jac313_store_${num}_TS PRIVATE cxx_std_26)
    jac313_store_link_matrix_target(jac313_store_${num}_TS)
    add_test(NAME jac313_store_${num}_TS
    set_tests_properties(jac313_store_${num}_TS PROPERTIES TIMEOUT 120)
             COMMAND jac313_store_${num}_TS --no-interactive --test-size=smoke --persist=none)

    add_executable(jac313_store_${num}_XS test_${num}_XS.cpp)
    target_compile_features(jac313_store_${num}_XS PRIVATE cxx_std_26)
    jac313_store_link_matrix_target(jac313_store_${num}_XS)
    add_test(NAME jac313_store_${num}_XS
             COMMAND jac313_store_${num}_XS --no-interactive --test-size=smoke --persist=none)
endforeach()