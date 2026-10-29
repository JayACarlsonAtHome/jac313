# jText command-line tools (ported from canonical jText; same sources).
# Round-trip helpers so jText <-> SQL works out of the box:
#   jtext_xtox_sql : jText -> SQL. Parses + validates a .jtFull/.jtext file and
#                    emits SQL by {N} template substitution (parser+validator via jac313::jText).
#   jtext_to_xml   : the canonical XML export (XML_EXPORT.md), the compare standard
#                    for every round trip.
#   jtext_to_ods / jtext_from_ods : jText <-> OpenDocument spreadsheet (ODS.md).
#   jtext_from_xml : the XML export back into jText (either scope).
#   jtext_to_html / jtext_from_html : that XML as HTML tables, and back (HTML_EXPORT.md).
#   jtext_from_sql : DB -> jText. Reads a PostgreSQL (libpq), SQLite or MariaDB table
#                    and generates the four jText component files, every row and comment.
# Tests for the tools live in canonical jText (tools/tests is not ported).

if(JAC313_JTEXT_BUILD_TOOLS)
    add_executable(jtext_xtox_sql jtext_xtox_sql.cpp)
    target_link_libraries(jtext_xtox_sql PRIVATE jac313::jText)
    target_compile_features(jtext_xtox_sql PRIVATE cxx_std_26)
    set_target_properties(jtext_xtox_sql PROPERTIES CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

    add_executable(jtext_to_xml jtext_to_xml.cpp)
    target_link_libraries(jtext_to_xml PRIVATE jac313::jText)
    target_compile_features(jtext_to_xml PRIVATE cxx_std_26)
    set_target_properties(jtext_to_xml PROPERTIES CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

    # jtext_from_sql reads each database through its client library (never a text dump).
    # A backend whose library is installed is built from from_sql/<db>.cpp; one whose
    # library is missing from from_sql/<db>_none.cpp, which says so when asked.
    add_executable(jtext_from_sql jtext_from_sql.cpp)
    target_include_directories(jtext_from_sql PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(jtext_from_sql PRIVATE jac313::jText jac313::Qlite)
    target_compile_features(jtext_from_sql PRIVATE cxx_std_26)
    set_target_properties(jtext_from_sql PROPERTIES CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
    find_package(PostgreSQL QUIET)
    find_package(SQLite3 QUIET)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(JTEXT_MARIADB QUIET IMPORTED_TARGET libmariadb)
    endif()
    foreach(db postgres sqlite mariadb)
        if(db STREQUAL "postgres")
            set(lib PostgreSQL::PostgreSQL)
        elseif(db STREQUAL "sqlite")
            set(lib SQLite3::SQLite3)
        else()
            set(lib PkgConfig::JTEXT_MARIADB)
        endif()
        if(TARGET ${lib})
            target_sources(jtext_from_sql PRIVATE from_sql/${db}.cpp)
            target_link_libraries(jtext_from_sql PRIVATE ${lib})
        else()
            target_sources(jtext_from_sql PRIVATE from_sql/${db}_none.cpp)
        endif()
    endforeach()

    # jtext_to_ods / jtext_from_ods (ODS.md): the archive through libzip, content.xml
    # through libxml2; built where both are installed (libzip-devel, libxml2-devel).
    find_package(LibXml2 QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(JTEXT_LIBZIP QUIET IMPORTED_TARGET libzip)
    endif()
    if(TARGET LibXml2::LibXml2 AND TARGET PkgConfig::JTEXT_LIBZIP)
        add_library(jtext_ods STATIC ods/ods.cpp)
        target_include_directories(jtext_ods PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
        target_link_libraries(jtext_ods PUBLIC jac313::jText LibXml2::LibXml2 PkgConfig::JTEXT_LIBZIP)
        target_compile_features(jtext_ods PUBLIC cxx_std_26)
        foreach(tool jtext_to_ods jtext_from_ods)
            add_executable(${tool} ${tool}.cpp)
            target_link_libraries(${tool} PRIVATE jtext_ods)
            set_target_properties(${tool} PROPERTIES CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
        endforeach()
    endif()

    # jtext_from_xml (XML_EXPORT.md): the XML export back into jText; jtext_to_html /
    # jtext_from_html (HTML_EXPORT.md): that XML as HTML tables, and back. Through libxml2.
    if(TARGET LibXml2::LibXml2)
        foreach(tool jtext_from_xml jtext_to_html jtext_from_html)
            add_executable(${tool} ${tool}.cpp)
            target_include_directories(${tool} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
            target_link_libraries(${tool} PRIVATE jac313::jText LibXml2::LibXml2)
            target_compile_features(${tool} PRIVATE cxx_std_26)
            set_target_properties(${tool} PROPERTIES CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
        endforeach()
    endif()
endif()
