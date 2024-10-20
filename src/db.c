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
    if (!db)
        return LOGGERDB_ERROR;

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

int ldb_table_metadata_read(loggerdb_table* table, void* ptr, size_t size);
int ldb_table_metadata_write(loggerdb_table* table, void* ptr, size_t size);
int ldb_table_read(loggerdb_table* table, size_t index, void* ptr, size_t size);
int ldb_table_write(loggerdb_table* table, size_t index, void* ptr, size_t size);

int ldb_node_open(loggerdb_table* table, loggerdb_node** node)
{
    if (!table)
        return LOGGERDB_ERROR;

    *node = malloc(sizeof(**node));
    (*node)->parent = NULL;
    (*node)->table = table;
    (*node)->path = NULL;
    (*node)->leaf = false;

    return LOGGERDB_OK;
}

int ldb_node_close(loggerdb_node* node)
{
    if (!node)
        return LOGGERDB_OK;

    free(node->path);
    free(node);

    return LOGGERDB_OK;
}

static ssize_t _ldb_node_index_from_name(const char* name)
{
    char *endptr = NULL;

    size_t index = strtoul(name, &endptr, 10);
    if (errno == 0 && name && !*endptr)
        return index;

    return -LOGGERDB_ERROR;
} 

const char* ldb_node_get_const_path(loggerdb_node* node)
{
    const char* path = node->path;
    if (!path)
        path = node->table->path;

    return path;
}

// Returns negative number on error
ssize_t ldb_node_size(loggerdb_node* node)
{
    size_t size = 0;
    DIR* dir;
    struct dirent* ent;

    ssize_t entry_index;
    char *endptr = NULL;

    if (node->leaf)
        return -LOGGERDB_INVALIDNODE;

    const char* path = ldb_node_get_const_path(node);

    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] == '.')
                continue;

            entry_index = _ldb_node_index_from_name(ent->d_name);
            if (entry_index > 0)
            {
                size++;
            }
        }
        closedir(dir);
    } else {
        return -LOGGERDB_ERROR;
    }

    return size;
}

// Gets the node that is equal to or the next smallest to the requested index
int ldb_node_get_subnode(loggerdb_node* node, size_t index, loggerdb_node** subnode)
{
    DIR* dir;
    struct dirent* ent;

    ssize_t entry_index;
    ssize_t best_index = -1;
    char *endptr = NULL;

    if (!node || node->leaf)
        return LOGGERDB_INVALIDNODE;

    const char* path = ldb_node_get_const_path(node);

    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] == '.')
                continue;

            entry_index = _ldb_node_index_from_name(ent->d_name);
            if (best_index < entry_index && entry_index <= index)
            {
                best_index = entry_index;
            }
        }
        closedir(dir);
    } else {
        return -LOGGERDB_ERROR;
    }

    if (best_index < 0)
    {
        // No node found that matches

        *subnode = NULL;
        return LOGGERDB_OK;
    }

    char buf[0x20+1];
    sprintf(buf, "%lu", best_index);

    *subnode = malloc(sizeof(**subnode));
    (*subnode)->parent = node;
    (*subnode)->table = node->table;
    (*subnode)->path = ldb_path_join(path, buf);
    (*subnode)->leaf = ldb_path_is_file((*subnode)->path);

    return LOGGERDB_OK;
}

int ldb_node_read(loggerdb_node* node, void* ptr, size_t size);

int ldb_node_write(loggerdb_node* node, void* ptr, size_t size)
{
    if (!node || !node->leaf)
        return LOGGERDB_INVALIDNODE;

    FILE* fd = fopen(node->path, "wb");
    if (!fd)
        return LOGGERDB_IOERROR;

    fwrite(ptr, size, 1, fd);
    fclose(fd);

    return LOGGERDB_OK;
}
