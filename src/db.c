#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#include "db.h"

#include <stdio.h>

#define LDB_PATH_SEP "/"

// Returns non-zero if path exists
static int ldb_path_exists(const char* path)
{
    struct stat sb = {0};
    return stat(path, &sb) == 0;
}

// Returns non-zero if path is a file
static int ldb_path_is_file(const char* path)
{
    struct stat sb = {0};
    stat(path, &sb);
    return S_ISREG(sb.st_mode);
}

// Returns non-zero if path is a directory
static int ldb_path_is_dir(const char* path)
{
    struct stat sb = {0};
    stat(path, &sb);
    return S_ISDIR(sb.st_mode);
}

static char* ldb_path_join(const char* s1, const char* s2)
{
    if (!s1 || !s2)
        return NULL;

    size_t len = strlen(s1) + strlen(LDB_PATH_SEP) + strlen(s2);
    if (!len)
        return NULL;

    char* o = malloc(len+1);
    *o = '\0';

    strcat(o, s1);
    strcat(o, LDB_PATH_SEP);
    strcat(o, s2);

    return o;
}

int ldb_open(const char* base_path, loggerdb** db)
{
    if (!base_path)
        return LOGGERDB_ERROR;

    if (!ldb_path_exists(base_path))
    {
        mkdir(base_path, 0700);
    }
    else if (!ldb_path_is_dir(base_path))
    {
        return LOGGERDB_NOTADB;
    }

    *db = malloc(sizeof(**db));
    (*db)->path = strdup(base_path);

    return LOGGERDB_OK;
}

int ldb_close(loggerdb* db)
{
    if (!db)
        return LOGGERDB_OK;

    free(db->path);
    free(db);

    return LOGGERDB_OK;
}


int ldb_table_open(loggerdb* db, const char* name, loggerdb_table** table)
{
    if (!db || !table)
        return LOGGERDB_INVALID;

    char* table_path = ldb_path_join(db->path, name);
    if (!table_path)
        return LOGGERDB_ERROR;

    if (!ldb_path_exists(table_path))
        mkdir(table_path, 0700);
    else if (!ldb_path_is_dir(table_path))
        return LOGGERDB_ERROR;

    *table = malloc(sizeof(**table));
    (*table)->path = table_path;
    (*table)->db = db;

    return LOGGERDB_OK;
}

int ldb_table_close(loggerdb_table* table)
{
    if (!table)
        return LOGGERDB_OK;

    free(table->path);
    free(table);

    return LOGGERDB_OK;
}

int ldb_node_open(loggerdb_table* table, time_t time, loggerdb_node** node)
{
    if (!table || !node)
        return LOGGERDB_INVALID;

    struct tm newtime;

    if (!localtime_r(&time, &newtime))
        return LOGGERDB_ERROR;

    char timebuff[20]; // = "YYYY/MM/DD/HH/MM";
    strftime(timebuff, 20, "%Y/%m/%d/%H/%M", &newtime);

    char* node_path = ldb_path_join(table->path, timebuff);
    if (!node_path)
        return LOGGERDB_ERROR;

    if (!ldb_path_exists(node_path))
    {
        char* p = node_path + strlen(table->path) + 1;

        while (*p)
        {
            while (*p != '\0' && *p != '/')
            {
                ++p;
            }

            char c = *p;
            *p = '\0';

            if (!ldb_path_exists(node_path))
            {
                mkdir(node_path, 0700);
            }
            else if (!ldb_path_is_dir(node_path))
            {
                free(node_path);
                return LOGGERDB_ERROR;
            }

            *p = c;

            ++p;
        }
    }

    *node = malloc(sizeof(**node));
    (*node)->time = time;
    (*node)->path = node_path;

    return LOGGERDB_OK;
}

int ldb_node_close(loggerdb_node* node)
{
    if (!node)
        return LOGGERDB_INVALID;

    free(node->path);
    free(node);

    return LOGGERDB_OK;
}

ssize_t ldb_node_size(loggerdb_node* node, const char* field)
{
    if (!node)
        return -LOGGERDB_INVALID;

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
        return -LOGGERDB_ERROR;

    if (!ldb_path_is_file(field_path))
    {
        free(field_path);
        return 0;
    }

    // Use append to get the size to prevent double seeking on filesystems
    // where files are stored as backwards linked-lists
    FILE* fd = fopen(field_path, "ab");
    free(field_path);
    if (!fd)
        return -LOGGERDB_ERROR;

    int bytes = ftell(fd);
    fclose(fd);

    return bytes;
}

ssize_t ldb_node_read(loggerdb_node* node, const char* field, void* ptr, size_t size)
{
    if (!node)
        return -LOGGERDB_INVALID;

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
        return -LOGGERDB_ERROR;

    FILE* fd = fopen(field_path, "rb");
    free(field_path);
    if (!fd)
        return -LOGGERDB_ERROR;

    int bytes = fread(ptr, size, 1, fd);
    fclose(fd);

    return bytes;
}

ssize_t ldb_node_write(loggerdb_node* node, const char* field, void* ptr, size_t size)
{
    if (!node)
        return -LOGGERDB_INVALID;

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
        return -LOGGERDB_ERROR;

    FILE* fd = fopen(field_path, "wb");
    free(field_path);
    if (!fd)
        return -LOGGERDB_ERROR;

    int bytes = fwrite(ptr, size, 1, fd);
    fclose(fd);

    return bytes;
}

ssize_t ldb_node_append(loggerdb_node* node, const char* field, void* ptr, size_t size)
{
    if (!node)
        return -LOGGERDB_INVALID;

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
        return -LOGGERDB_ERROR;

    FILE* fd = fopen(field_path, "ab");

    free(field_path);
    if (!fd)
        return -LOGGERDB_ERROR;

    int bytes = fwrite(ptr, size, 1, fd);
    fclose(fd);

    return bytes;
}

ssize_t ldb_node_metadata_read(loggerdb_node* node, void* ptr, size_t size)
{
    return ldb_node_read(node, "metadata", ptr, size);
}

ssize_t ldb_node_metadata_write(loggerdb_node* node, void* ptr, size_t size)
{
    return ldb_node_write(node, "metadata", ptr, size);
}
