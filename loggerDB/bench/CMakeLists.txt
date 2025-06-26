math(EXPR BENCH_TIME "60*60*24*1" OUTPUT_FORMAT DECIMAL)

add_library(ldb_bench_interface INTERFACE)
target_link_libraries(ldb_bench_interface INTERFACE loggerDB)
target_compile_definitions(ldb_bench_interface INTERFACE BENCH_TIME=${BENCH_TIME})
target_compile_definitions(ldb_bench_interface INTERFACE DB_PATH="db_test")
target_compile_definitions(ldb_bench_interface INTERFACE TABLE_NAME="test_table")

add_executable(ldb_bench_naive naive.c)
target_link_libraries(ldb_bench_naive ldb_bench_interface)

add_executable(ldb_bench_simple simple.c)
target_link_libraries(ldb_bench_simple ldb_bench_interface)

add_executable(ldb_bench_stream stream.c)
target_link_libraries(ldb_bench_stream ldb_bench_interface)

add_executable(ldb_bench_read_write read-write.c)
target_link_libraries(ldb_bench_read_write ldb_bench_interface)
