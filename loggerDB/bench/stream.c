#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "loggerDB.h"
#include "loggerDB/stream.h"
#include "loggerDB/util.h"

static loggerdb* db;
static loggerdb_table* table;
static loggerdb_node* node;
static loggerdb_stream* stream;

#define DB_PATH "db_test"
#define TABLE_NAME "test_table"

struct dataset {
    time_t time;
    float values[4];
};

float rand_float(void)
{
    return (float)rand()/(float)(RAND_MAX/100);
}

int main()
{
    int res;
    ssize_t s;

    res = ldb_open(DB_PATH, &db);
    assert(res == LOGGERDB_OK);

    res = ldb_table_open(db, TABLE_NAME, &table);
    assert(res == LOGGERDB_OK);

    time_t t = 0;
    time_t max_t = ((time_t)BENCH_TIME);
    for (;t < max_t; ++t)
    {
        struct dataset data = {
            .time = t,
            .values = {
                rand_float(),
                rand_float(),
                rand_float(),
                rand_float()
            },
        };

        if (!node || ldb_node_contains(node, t))
        {
            if (node)
            {
                res = ldb_stream_close(stream);
                assert(res == LOGGERDB_OK);
                res = ldb_node_close(node);
                assert(res == LOGGERDB_OK);
            }

            res = ldb_node_open(table, t, &node);
            assert(res == LOGGERDB_OK);
            assert(node);
            res = ldb_stream_open(node, "data", sizeof(data), 0, &stream);
            assert(res == LOGGERDB_OK);
            assert(stream);
        }

        s = ldb_stream_write(stream, &data);
        assert(s > 0);
        //fprintf(stderr, ".");
    }
    res = ldb_stream_close(stream);
    assert(res == LOGGERDB_OK);
    res = ldb_node_close(node);
    assert(res == LOGGERDB_OK);

    fprintf(stderr, "Stored %zu datasets\n", t);

    res = ldb_table_close(table);
    assert(res == LOGGERDB_OK);

    res = ldb_close(db);
    assert(res == LOGGERDB_OK);
}