#include <sys/types.h>
#include <assert.h>
#include <time.h>

#include "table.h"
#include "node.h"
#include "status.h"
#include "util.h"

#define DATA_FIELD "data"

ssize_t ldb_insert_data(loggerdb_table* table, time_t time, void* ptr, size_t size)
{
    assert(table);
    assert(ptr);

    int r;
    ssize_t bytes;

    loggerdb_node* node;
    if ((r = ldb_node_open(table, time, &node)) != LOGGERDB_OK)
        return -r;

    bytes = ldb_node_append(node, DATA_FIELD, ptr, size);

    if ((r = ldb_node_close(node)) != LOGGERDB_OK)
        return -r;

    return bytes;
}
