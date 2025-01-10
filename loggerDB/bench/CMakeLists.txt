
add_executable(ldb_bench_naive naive.c)
target_link_libraries(ldb_bench_naive loggerDB)

add_executable(ldb_bench_simple simple.c)
target_link_libraries(ldb_bench_simple loggerDB)

add_executable(ldb_bench_read_write read-write.c)
target_link_libraries(ldb_bench_read_write loggerDB)
