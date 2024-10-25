#ifndef LOGGERDB_TABLE_H
#define LOGGERDB_TABLE_H

typedef struct loggerdb loggerdb;

typedef struct loggerdb_table {
    char* path;
    struct loggerdb* db;
} loggerdb_table;

int ldb_table_open(loggerdb* db, const char* name, loggerdb_table** table);
int ldb_table_close(loggerdb_table* table);

#endif