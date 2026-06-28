# jText command-line tools (ported from canonical jText; same sources).
# Round-trip helpers so jText <-> SQL works out of the box:
#   jtext_xtox_sql : jText -> SQL. Parses + validates a .jtFull/.jtext file and
#                    emits SQL by {N} template substitution (parser+validator via jac313::jText).
#   jtext_from_sql : DB -> jText. Reads a PostgreSQL table/query (through psql) and
#                    generates the four jText component files. std-only.
# Tests for the tools are deferred (none here yet).

if(JAC313_JTEXT_BUILD_TOOLS)
    add_executable(jtext_xtox_sql jtext_xtox_sql.cpp)
    target_link_libraries(jtext_xtox_sql PRIVATE jac313::jText)
    target_compile_features(jtext_xtox_sql PRIVATE cxx_std_26)
    set_target_properties(jtext_xtox_sql PROPERTIES CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

    add_executable(jtext_from_sql jtext_from_sql.cpp)
    target_compile_features(jtext_from_sql PRIVATE cxx_std_26)
    set_target_properties(jtext_from_sql PROPERTIES CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
endif()
