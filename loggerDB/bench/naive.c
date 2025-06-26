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

#define DB_PATH "db_test"
#define TABLE_NAME "test_table"

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

        s = ldb_insert_data(&table, t, &data, sizeof(data));
        assert(s >= 0);

        if ((t % EPOCH_HOURS(1)) == 0)
            fprintf(stderr, ".");
    }
    fprintf(stderr, "Stored %zu datasets\n", t);

    res = ldb_table_close(&table);
    assert(res == LOGGERDB_OK);

    res = ldb_close(&db);
    assert(res == LOGGERDB_OK);
}