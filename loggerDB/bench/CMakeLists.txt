
add_executable(ldb_bench
    main.c
)
target_link_libraries(ldb_bench PRIVATE
    loggerDB
)
