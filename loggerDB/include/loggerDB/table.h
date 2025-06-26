#ifndef LOGGERDB_TABLE_H
#define LOGGERDB_TABLE_H

#include <stdint.h>

typedef struct loggerdb loggerdb;

typedef struct loggerdb_table {
    char* path;
    loggerdb* db;
    uint8_t init;
} loggerdb_table;

int ldb_table_open(loggerdb* db, const char* name, loggerdb_table* table);
int ldb_table_close(loggerdb_table* table);
int ldb_table_valid(loggerdb_table* table);

#endif