#ifndef LOGGERDB_H
#define LOGGERDB_H

#include <stddef.h>
#include <sys/types.h>
#include <stdbool.h>
#include <time.h>

typedef struct loggerdb {
    int fd;
} loggerdb;

int ldb_open(const char* base_path, loggerdb** db);
int ldb_close(loggerdb* db);

#endif
