#ifndef LOGGERDB_NODE_H
#define LOGGERDB_NODE_H

#include <time.h>

#include "loggerDB/mutex.h"

typedef struct loggerdb_table loggerdb_table;

typedef struct loggerdb_node {
    time_t time;
    ldb_mutex* mutex;
    char* path;
} loggerdb_node;

int ldb_node_open(loggerdb_table* table, time_t time, loggerdb_node** node);
int ldb_node_close(loggerdb_node* node);
int ldb_node_contains(loggerdb_node* node, time_t time);
ssize_t ldb_node_size(loggerdb_node* node, const char* field);
ssize_t ldb_node_read(loggerdb_node* node, const char* field, void* ptr, size_t size);
ssize_t ldb_node_read_offset(loggerdb_node* node, const char* field, long offset, void* ptr, size_t size);
ssize_t ldb_node_write(loggerdb_node* node, const char* field, void* ptr, size_t size);
ssize_t ldb_node_append(loggerdb_node* node, const char* field, void* ptr, size_t size);
int ldb_node_exists(loggerdb_node* node, const char* field);
ssize_t ldb_node_metadata_read(loggerdb_node* node, void* ptr, size_t size);
ssize_t ldb_node_metadata_write(loggerdb_node* node, void* ptr, size_t size);

#endif