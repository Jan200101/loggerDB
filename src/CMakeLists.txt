
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
    status.h
)
target_include_directories(loggerDB INTERFACE ${CMAKE_CURRENT_LIST_DIR})

if (NOT CMAKE_CROSSCOMPILING)
    add_executable(ldb_test
        main.c
    )
    target_link_libraries(ldb_test PRIVATE
        loggerDB
    )
endif()
