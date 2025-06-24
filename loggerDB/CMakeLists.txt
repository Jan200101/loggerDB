add_subdirectory(mutex)

add_library(loggerDB INTERFACE)
target_sources(loggerDB INTERFACE
    src/db.c
    src/table.c
    src/node.c
    src/stream.c
    src/path.c
    src/util.c
)
target_include_directories(loggerDB INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(loggerDB INTERFACE ldb_mutex)
target_compile_definitions(loggerDB INTERFACE LDB_VERSION="${LDB_VERSION}")
target_compile_definitions(loggerDB INTERFACE LDB_VERSION_MAJOR=${LDB_VERSION_MAJOR})
target_compile_definitions(loggerDB INTERFACE LDB_VERSION_MINOR=${LDB_VERSION_MINOR})
target_compile_definitions(loggerDB INTERFACE LDB_VERSION_PATCH=${LDB_VERSION_PATCH})

if (LDB_ENABLE_BENCH)
    add_subdirectory(bench)
endif()
