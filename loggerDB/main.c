#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "loggerdb.h"
#include "util.h"

static loggerdb* db;
static loggerdb_table* table;

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
    if (res != LOGGERDB_OK)
    {
        printf("Failed to open DB_PATH %s (%i)\n", DB_PATH, res);
        return -1;
    }

    res = ldb_table_open(db, TABLE_NAME, &table);
    if (res != LOGGERDB_OK)
    {
        printf("Failed to open table (%i)\n", res);
        return -1;
    }

    time_t t = 0;
    for (;t < 60*60*24*31; ++t)
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

        s = ldb_insert_data(table, t, &data, sizeof(data));
        if (s < 0)
        {
            printf("Failed to write to node (%i)\n", -s);
            break;
        }
    }
    fprintf(stderr, "Stored %zu datasets\n", t);

    res = ldb_table_close(table);
    if (res != LOGGERDB_OK)
    {
        printf("Failed to close table (%i)\n", res);
        return -1;
    }

    res = ldb_close(db);
    if (res != LOGGERDB_OK)
    {
        printf("Failed to close database (%i)\n", res);
        return -1;
    }
}