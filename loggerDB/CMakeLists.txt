add_subdirectory(mutex)

add_library(loggerDB INTERFACE)
target_sources(loggerDB INTERFACE
    db.c
    db.h
    table.c
    table.h
    node.c
    node.h
    path.c
    path.h
    util.c
    util.h
    status.h
)
target_include_directories(loggerDB INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(loggerDB INTERFACE ldb_mutex)

if (NOT CMAKE_CROSSCOMPILING)
    add_executable(ldb_test
        main.c
    )
    target_link_libraries(ldb_test PRIVATE
        loggerDB
    )
endif()
