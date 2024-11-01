#ifndef LOGGERDB_UTIL_H
#define LOGGERDB_UTIL_H

#include <sys/types.h>

ssize_t ldb_insert_data(loggerdb_table* table, time_t time, void* ptr, size_t size);

#endif
