add_subdirectory(mutex)

add_library(loggerDB INTERFACE)
target_sources(loggerDB INTERFACE
    src/db.c
    src/table.c
    src/node.c
    src/path.c
    src/util.c
)
target_include_directories(loggerDB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(loggerDB INTERFACE ldb_mutex)

if (LDB_ENABLE_BENCH)
    add_subdirectory(bench)
endif()
