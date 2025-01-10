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

static loggerdb* db;
static loggerdb_table* table;

#define DB_PATH "db_test"
#define TABLE_NAME "test_table"

#define DAYS(x) (60*60*24*x)

struct dataset {
    time_t time;
    float values[4];
};

static int removeDir(const char *path)
{
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d) {
        struct dirent *p;

        r = 0;
        while (!r && (p=readdir(d))) {
            char *buf;
            size_t len;

            // Skip the names "." and ".." as we don't want to recurse on them.
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);

            if (buf) {
                struct stat statbuf = {0};

                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode))
                        r = removeDir(buf);
#ifndef _WIN32
                    else if (S_ISLNK(statbuf.st_mode))
                        r = unlink(buf);
#endif
                    else
                        r = remove(buf);
                }
                else // it is very likely that we found a dangling symlink which is not detected by stat
                {
                    r = unlink(buf);
                }
                free(buf);
            }
        }
        closedir(d);
    }

    if (!r)
        r = rmdir(path);

    return r;
}

int main()
{
    int res;
    ssize_t s;
    time_t t = DAYS(0);

    fprintf(stderr, "Deleting any previous database...");
    removeDir(DB_PATH);

    // validate the database is actually gone
    for (;t < DAYS(14); ++t)
    {
        assert(ldb_node_check(table, t) != LOGGERDB_OK);
    }
    fprintf(stderr, " done\n");

    res = ldb_open(DB_PATH, &db);
    assert(res == LOGGERDB_OK);

    res = ldb_table_open(db, TABLE_NAME, &table);
    assert(res == LOGGERDB_OK);

    fprintf(stderr, "Storing test data...");
    t = DAYS(3);
    for (;t < DAYS(7); ++t)
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

        s = ldb_insert_data(table, t, &data, sizeof(data));
        assert(s >= 0);
    }
    fprintf(stderr, " done\n");


    fprintf(stderr, "Checking for invalid nodes...");
    t = DAYS(0);
    // None of these nodes should exist
    for (;t < DAYS(3); ++t)
    {
        assert(ldb_node_check(table, t) != LOGGERDB_OK);
    }
    fprintf(stderr, " done\n");


    fprintf(stderr, "Checking for valid nodes...");
    // These should exist, we made them
    for (;t < DAYS(7); ++t)
    {
        assert(ldb_node_check(table, t) == LOGGERDB_OK);
    }
    fprintf(stderr, " done\n");


    fprintf(stderr, "Checking for non existant nodes...");
    // We never reached up to this point, so they shouldn't exist
    for (;t < DAYS(14); ++t)
    {
        assert(ldb_node_check(table, t) != LOGGERDB_OK);
    }
    fprintf(stderr, " done\n");

    fprintf(stderr, "Validating node contents...");
    t = DAYS(3);
    for (;t < DAYS(7); t += ldb_node_spacing())
    {
        assert(ldb_node_check(table, t) == LOGGERDB_OK);

        loggerdb_node* node;
        res = ldb_node_open(table, t, &node);
        assert(res == LOGGERDB_OK);

        ssize_t node_size = ldb_node_size(node, "data");
        assert(node_size > 0);

        struct dataset data;
        for (long offset = 0; offset < node_size; offset += sizeof(data))
        {
            s = ldb_node_read_offset(node, "data", offset, &data, sizeof(data));
            assert(s > 0);

            assert(data.time >= t);
            assert(data.time < (t + ldb_node_spacing()));
            assert(data.values[0] == 1);
            assert(data.values[1] == 2);
            assert(data.values[2] == 4);
            assert(data.values[3] == 8);
        }

        ldb_node_close(node);
    }
    fprintf(stderr, " done\n");

    res = ldb_table_close(table);
    assert(res == LOGGERDB_OK);

    res = ldb_close(db);
    assert(res == LOGGERDB_OK);
}