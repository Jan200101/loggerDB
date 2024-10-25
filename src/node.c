#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "table.h"
#include "node.h"
#include "path.h"
#include "status.h"

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

    char* p = node_path + strlen(table->path);

    if (!ldb_path_is_dir(node_path))
    {        
        while (*p)
        {
            while (*p != '\0' && *p != '/')
                ++p;

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

            if (c)
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

static inline ssize_t _ldb_node_read_file(const char* path, long offset, void* ptr, size_t size)
{
    FILE* fd = fopen(path, "rb");
    if (!fd)
        return -LOGGERDB_ERROR;

    fseek(fd, offset, SEEK_SET);

    int bytes = fread(ptr, size, 1, fd);
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

    ssize_t ret = _ldb_node_read_file(field_path, 0, ptr, size);
    free(field_path);

    return ret;
}

ssize_t ldb_node_read_offset(loggerdb_node* node, const char* field, long offset, void* ptr, size_t size)
{
    if (!node)
        return -LOGGERDB_INVALID;

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
        return -LOGGERDB_ERROR;

    ssize_t ret = _ldb_node_read_file(field_path, offset, ptr, size);
    free(field_path);

    return ret;
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
