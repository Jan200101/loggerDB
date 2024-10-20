#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "db.h"

static loggerdb* db;
static loggerdb_table* table;
static loggerdb_node* root_node;
static loggerdb_node* sub_node;

#define DB_PATH "db_test"
#define TABLE_NAME "test_table"

struct test {
    uint32_t val;
};

int main()
{
    int res;
    printf("[+] Opening DB\n");
    res = ldb_open(DB_PATH, &db);
    if (res != LOGGERDB_OK)
    {
        printf("Failed to open DB_PATH %s (%i)\n", DB_PATH, res);
        return -1;
    }

    printf("[+] Opening table\n");
    res = ldb_table_open(db, TABLE_NAME, &table);
    if (res != LOGGERDB_OK)
    {
        printf("Failed to open table (%i)\n", res);
        return -1;
    }

    printf("[+] Getting root node\n");
    res = ldb_node_open(table, &root_node);
    if (res != LOGGERDB_OK)
    {
        printf("Failed to open node (%i)\n", res);
        return -1;
    }
    printf("[*] root node path: %s\n", ldb_node_get_const_path(root_node));

    printf("[*] Getting root node size\n");
    ssize_t s = ldb_node_size(root_node);
    if (s < 0)
    {
        printf("Failed to get node size (%li)", -s);
        return -1;
    }
    printf("[*] root node size: %li\n", s);


    printf("[*] Fetching sub node for 10\n");
    res = ldb_node_get_subnode(root_node, 10, &sub_node);
    if (res != LOGGERDB_OK)
    {
        printf("Failed to get sub node (%li)", -s);
        return -1;
    }

    printf("[*] sub node path: %s\n", ldb_node_get_const_path(sub_node));


    char buf[] = "abc";

    printf("[*] Writing test data '%s' to sub node\n", buf);
    res = ldb_node_write(sub_node, buf, strlen(buf));
    if (res != LOGGERDB_OK)
    {
        printf("Failed to get sub node (%li)", -s);
        return -1;
    }

    printf("[*] Read test data from sub node: ");
    res = ldb_node_write(sub_node, buf, strlen(buf));
    if (res != LOGGERDB_OK)
    {
        printf("Failed to get sub node (%li)", -s);
        return -1;
    }


    res = ldb_node_close(sub_node);
    if (res != LOGGERDB_OK)
    {
        printf("Failed to close sub node (%i)\n", res);
        return -1;
    }

    res = ldb_node_close(root_node);
    if (res != LOGGERDB_OK)
    {
        printf("Failed to close root node (%i)\n", res);
        return -1;
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