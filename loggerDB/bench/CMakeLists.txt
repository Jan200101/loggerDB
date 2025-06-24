math(EXPR BENCH_TIME "60*60*24*31" OUTPUT_FORMAT DECIMAL)

add_executable(ldb_bench_naive naive.c)
target_link_libraries(ldb_bench_naive loggerDB)
target_compile_definitions(ldb_bench_naive PRIVATE BENCH_TIME=${BENCH_TIME})

add_executable(ldb_bench_simple simple.c)
target_link_libraries(ldb_bench_simple loggerDB)
target_compile_definitions(ldb_bench_simple PRIVATE BENCH_TIME=${BENCH_TIME})

add_executable(ldb_bench_stream stream.c)
target_link_libraries(ldb_bench_stream loggerDB)
target_compile_definitions(ldb_bench_stream PRIVATE BENCH_TIME=${BENCH_TIME})

add_executable(ldb_bench_read_write read-write.c)
target_link_libraries(ldb_bench_read_write loggerDB)
target_compile_definitions(ldb_bench_read_write PRIVATE BENCH_TIME=${BENCH_TIME})
