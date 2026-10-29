# jac313::Qlite — Qlite (one SQL API over SQLite, PostgreSQL and MariaDB), as jText brings
# it in (canonical jText vNNN/Qlite, renamed jac313::Qlite by vendor-sync). Header-only:
# the target is the include folder plus SQLite; a target that includes Postgres.hpp or
# MariaDB.hpp links libpq or libmariadb itself. This file belongs to jac313.
if(NOT TARGET jac313::Qlite)
    find_package(SQLite3 REQUIRED)
    if(TARGET SQLite3::SQLite3)
        set(JAC313_QLITE_SQLITE3_TARGET SQLite3::SQLite3)
    else()
        set(JAC313_QLITE_SQLITE3_TARGET SQLite::SQLite3)
    endif()
    add_library(jac313_Qlite INTERFACE)
    add_library(jac313::Qlite ALIAS jac313_Qlite)
    target_include_directories(jac313_Qlite INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_features(jac313_Qlite INTERFACE cxx_std_20)
    target_link_libraries(jac313_Qlite INTERFACE ${JAC313_QLITE_SQLITE3_TARGET})

    include(GNUInstallDirs)
    install(TARGETS jac313_Qlite EXPORT jac313QliteTargets)
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()
