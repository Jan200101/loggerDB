#ifndef LOGGERDB_H
#define LOGGERDB_H

#include <stddef.h>
#include <sys/types.h>
#include <stdbool.h>
#ifndef LOGGERDB_NODE_MAX
#define LOGGERDB_NODE_MAX
#endif

struct loggerdb_node;
struct loggerdb_table;
struct loggerdb;

typedef struct loggerdb_node {
    char* path;
    struct loggerdb_node* parent;
    struct loggerdb_table* table;
    bool leaf;
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
    LOGGERDB_NOTFOUND = 2,
    LOGGERDB_NOTADB = 3,
    LOGGERDB_INVALIDNODE = 4,
    LOGGERDB_IOERROR = 5,

    LOGGERDB_STATUS_END,
};

int ldb_open(const char* base_path, loggerdb** db);
int ldb_close(loggerdb* db);

int ldb_table_open(loggerdb* db, const char* name, loggerdb_table** table);
int ldb_table_close(loggerdb_table* table);
int ldb_table_metadata_read(loggerdb_table* table, void* ptr, size_t size);
int ldb_table_metadata_write(loggerdb_table* table, void* ptr, size_t size);
int ldb_table_read(loggerdb_table* table, size_t index, void* ptr, size_t size);
int ldb_table_write(loggerdb_table* table, size_t index, void* ptr, size_t size);

int ldb_node_open(loggerdb_table* table, loggerdb_node** node);
int ldb_node_close(loggerdb_node* node);
const char* ldb_node_get_const_path(loggerdb_node* node);
ssize_t ldb_node_size(loggerdb_node* node);
int ldb_node_get_subnode(loggerdb_node* node, size_t index, loggerdb_node** subnode);
int ldb_node_read(loggerdb_node* node, void* ptr, size_t size);
int ldb_node_write(loggerdb_node* node, void* ptr, size_t size);

#endif
