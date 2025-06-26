#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "loggerDB.h"
#include "loggerDB/util.h"

#include "bench-utils.h"

static loggerdb db;
static loggerdb_table table;
static loggerdb_node node;

int main()
{
    int res;
    ssize_t s;

    fprintf(stderr, "Deleting any previous database...\n");
    removeDir(DB_PATH);

    res = ldb_open(DB_PATH, &db);
    assert(res == LOGGERDB_OK);

    res = ldb_table_open(&db, TABLE_NAME, &table);
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

        if (ldb_node_valid(&node) != LOGGERDB_OK || ldb_node_contains(&node, t) != LOGGERDB_OK)
        {
            if (ldb_node_valid(&node) == LOGGERDB_OK)
            {
                res = ldb_node_close(&node);
                assert(res == LOGGERDB_OK);
            }

            res = ldb_node_open(&table, t, &node);
            assert(res == LOGGERDB_OK);
            assert(ldb_node_valid(&node) == LOGGERDB_OK);
        }

        s = ldb_node_append(&node, "data", &data, sizeof(data));
        assert(s >= 0);

        if ((t % EPOCH_HOURS(1)) == 0)
            fprintf(stderr, ".");
    }
    res = ldb_node_close(&node);
    assert(res == LOGGERDB_OK);

    fprintf(stderr, "Stored %zu datasets\n", t);

    res = ldb_table_close(&table);
    assert(res == LOGGERDB_OK);

    res = ldb_close(&db);
    assert(res == LOGGERDB_OK);
}