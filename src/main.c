#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "db.h"

static loggerdb* db;
static loggerdb_table* table;
static loggerdb_node* node;

#define DB_PATH "db_test"
#define TABLE_NAME "test_table"


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

    while (true)
    {
        res = ldb_node_open(table, time(NULL), &node);
        if (res != LOGGERDB_OK)
        {
            printf("Failed to open node (%i)\n", res);
            return -1;
        }

        char bufout[] = "abc";
        char bufin[] = "efg";

        printf("[*] Appending '%s' to node\n", bufout);
        s = ldb_node_append(node, "daten", bufout, strlen(bufout));
        if (s < 0)
        {
            printf("Failed to write to node (%i)\n", -s);
            return -1;
        }

        printf("[*] Reading from node: ");
        s = ldb_node_read(node, "daten", bufin, strlen(bufout));
        if (s < 0)
        {
            printf("Failed to read from node (%i)\n", -s);
            return -1;
        }
        printf("'%s'\n", bufin);

        printf("[*] node size: %li\n", ldb_node_size(node, "daten"));

        printf("[*] Reading metadata from node\n");
        s = ldb_node_metadata_read(node, bufin, strlen(bufout));
        if (s < 0)
        {
            printf("[+] Failed to read metadata from node (%i) (expected)\n", -s);
        }

        res = ldb_node_close(node);
        if (res != LOGGERDB_OK)
        {
            printf("Failed to close root node (%i)\n", res);
            return -1;
        }

        struct timespec tim;
        tim.tv_sec  = 0;
        tim.tv_nsec = 500000000L;
        nanosleep(&tim, NULL);
        printf("\n");
    }

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