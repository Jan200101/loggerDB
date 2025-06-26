#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "loggerDB.h"
#include "loggerDB/util.h"

#include "bench-utils.h"

static loggerdb db;
static loggerdb_table table;

int main()
{
    int res;
    ssize_t s;
    time_t t = EPOCH_DAYS(0);

    fprintf(stderr, "Deleting any previous database...");
    removeDir(DB_PATH);

    // validate the database is actually gone
    for (;t < EPOCH_DAYS(14); ++t)
    {
        assert(ldb_node_check(&table, t) != LOGGERDB_OK);
    }
    fprintf(stderr, " done\n");

    res = ldb_open(DB_PATH, &db);
    assert(res == LOGGERDB_OK);

    res = ldb_table_open(&db, TABLE_NAME, &table);
    assert(res == LOGGERDB_OK);

    fprintf(stderr, "Storing test data...");
    t = EPOCH_DAYS(3);
    for (;t < EPOCH_DAYS(7); ++t)
    {
        struct dataset data = {
            .time = t,
            .values = {
                1,
                2,
                4,
                8,
            },
        };

        s = ldb_insert_data(&table, t, &data, sizeof(data));
        assert(s >= 0);
    }
    fprintf(stderr, " done\n");


    fprintf(stderr, "Checking for invalid nodes...");
    t = EPOCH_DAYS(0);
    // None of these nodes should exist
    for (;t < EPOCH_DAYS(3); ++t)
    {
        assert(ldb_node_check(&table, t) != LOGGERDB_OK);
    }
    fprintf(stderr, " done\n");


    fprintf(stderr, "Checking for valid nodes...");
    // These should exist, we made them
    for (;t < EPOCH_DAYS(7); ++t)
    {
        assert(ldb_node_check(&table, t) == LOGGERDB_OK);
    }
    fprintf(stderr, " done\n");


    fprintf(stderr, "Checking for non existant nodes...");
    // We never reached up to this point, so they shouldn't exist
    for (;t < EPOCH_DAYS(14); ++t)
    {
        assert(ldb_node_check(&table, t) != LOGGERDB_OK);
    }
    fprintf(stderr, " done\n");

    fprintf(stderr, "Validating node contents...");
    t = EPOCH_DAYS(3);
    for (;t < EPOCH_DAYS(7); t += ldb_node_spacing())
    {
        assert(ldb_node_check(&table, t) == LOGGERDB_OK);

        loggerdb_node node;
        res = ldb_node_open(&table, t, &node);
        assert(res == LOGGERDB_OK);

        ssize_t node_size = ldb_node_size(&node, "data");
        assert(node_size > 0);

        struct dataset data;
        for (long offset = 0; offset < node_size; offset += sizeof(data))
        {
            s = ldb_node_read_offset(&node, "data", offset, &data, sizeof(data));
            assert(s > 0);

            assert(data.time >= t);
            assert(data.time < (t + ldb_node_spacing()));
            assert(data.values[0] == 1);
            assert(data.values[1] == 2);
            assert(data.values[2] == 4);
            assert(data.values[3] == 8);
        }

        ldb_node_close(&node);
    }
    fprintf(stderr, " done\n");

    res = ldb_table_close(&table);
    assert(res == LOGGERDB_OK);

    res = ldb_close(&db);
    assert(res == LOGGERDB_OK);
}