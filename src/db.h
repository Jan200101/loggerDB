#ifndef LOGGERDB_H
#define LOGGERDB_H

#include <stddef.h>
#include <sys/types.h>
#include <stdbool.h>
#include <time.h>

struct loggerdb_node;
struct loggerdb_table;
struct loggerdb;

typedef struct loggerdb_node {
    time_t time;
    char* path;
} loggerdb_node;

typedef struct loggerdb_table {
    char* path;
    struct loggerdb* db;
} loggerdb_table;

typedef struct loggerdb {
    char* path;
} loggerdb;

enum littedb_status {
    LOGGERDB_OK = 0,
    LOGGERDB_ERROR = 1,
    LOGGERDB_NOTADB = 2,
    LOGGERDB_NOTFOUND = 3,
    LOGGERDB_INVALID = 4,
    LOGGERDB_IOERROR = 5,

    LOGGERDB_STATUS_END,
};

int ldb_open(const char* base_path, loggerdb** db);
int ldb_close(loggerdb* db);

int ldb_table_open(loggerdb* db, const char* name, loggerdb_table** table);
int ldb_table_close(loggerdb_table* table);

int ldb_node_open(loggerdb_table* table, time_t time, loggerdb_node** node);
int ldb_node_close(loggerdb_node* node);
ssize_t ldb_node_size(loggerdb_node* node, const char* field);
ssize_t ldb_node_read(loggerdb_node* node, const char* field, void* ptr, size_t size);
ssize_t ldb_node_write(loggerdb_node* node, const char* field, void* ptr, size_t size);
ssize_t ldb_node_append(loggerdb_node* node, const char* field, void* ptr, size_t size);
ssize_t ldb_node_metadata_read(loggerdb_node* node, void* ptr, size_t size);
ssize_t ldb_node_metadata_write(loggerdb_node* node, void* ptr, size_t size);

#endif
