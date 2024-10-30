#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>

#include "table.h"
#include "node.h"
#include "path.h"
#include "status.h"

#define METADATA_SIZE_LIMIT 255

int ldb_node_open(loggerdb_table* table, time_t time, loggerdb_node** node)
{
    if (!table || !node)
        return LOGGERDB_INVALID;

    *node = NULL;
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

    // Round to minutes
    time = (time - time % 60);

    *node = malloc(sizeof(**node));

    (*node)->time = time;
    (*node)->mutex = mutex->alloc();
    (*node)->path = node_path;

    return LOGGERDB_OK;
}

int ldb_node_close(loggerdb_node* node)
{
    if (!node)
        return LOGGERDB_INVALID;

    free(node->path);
    mutex->free(node->mutex);
    free(node);

    return LOGGERDB_OK;
}

ssize_t ldb_node_size(loggerdb_node* node, const char* field)
{
    if (!node)
        return -LOGGERDB_INVALID;

    ssize_t ret;
    mutex->enter(node->mutex);

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

    if (!ldb_path_is_file(field_path))
    {
        free(field_path);
        ret = 0;
        goto cleanup;
    }

    // Use append to get the size to prevent double seeking on filesystems
    // where files are stored as backwards linked-lists
    int fd = open(field_path, O_RDONLY | O_APPEND);
    free(field_path);

    if (fd < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

    ret = lseek(fd, 0, SEEK_CUR);

    if (close(fd) < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

cleanup:
    mutex->leave(node->mutex);

    return ret;
}

static inline ssize_t _ldb_node_read_file(loggerdb_node* node, const char* path, long offset, void* ptr, size_t size)
{
    ssize_t ret;

    mutex->enter(node->mutex);

    int fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }


    lseek(fd, offset, SEEK_SET);

    ret = read(fd, ptr, size);

    if (close(fd) < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

cleanup:
    mutex->leave(node->mutex);

    return ret;
}

ssize_t ldb_node_read(loggerdb_node* node, const char* field, void* ptr, size_t size)
{
    if (!node)
        return -LOGGERDB_INVALID;

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
        return -LOGGERDB_ERROR;

    ssize_t ret = _ldb_node_read_file(node, field_path, 0, ptr, size);
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

    ssize_t ret = _ldb_node_read_file(node, field_path, offset, ptr, size);
    free(field_path);

    return ret;
}

ssize_t ldb_node_write(loggerdb_node* node, const char* field, void* ptr, size_t size)
{
    assert(node);

    ssize_t ret;
    mutex->enter(node->mutex);

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

    int fd = open(field_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    free(field_path);

    if (fd < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

    ssize_t bytes = write(fd, ptr, size);
    if (close(fd) < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

cleanup:
    mutex->leave(node->mutex);

    return bytes;
}

ssize_t ldb_node_append(loggerdb_node* node, const char* field, void* ptr, size_t size)
{
    assert(node);

    ssize_t ret;
    mutex->enter(node->mutex);

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

    int fd = open(field_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    free(field_path);

    if (fd < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

    ssize_t bytes = write(fd, ptr, size);
    if (close(fd) < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

cleanup:
    mutex->leave(node->mutex);

    return bytes;
}

int ldb_node_exists(loggerdb_node* node, const char* field)
{
    assert(node);

    ssize_t ret;

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
    {
        return -LOGGERDB_ERROR;
    }

    if (!ldb_path_exists(field_path))
    {
        free(field_path)
        return LOGGERDB_NOTFOUND;
    }

    return LOGGERDB_OK;
}

ssize_t ldb_node_metadata_read(loggerdb_node* node, void* ptr, size_t size)
{
    if (size > METADATA_SIZE_LIMIT)
        return -LOGGERDB_ERROR;

    return ldb_node_read(node, "metadata", ptr, size);
}

ssize_t ldb_node_metadata_write(loggerdb_node* node, void* ptr, size_t size)
{
    if (size > METADATA_SIZE_LIMIT)
        return -LOGGERDB_ERROR;

    return ldb_node_write(node, "metadata", ptr, size);
}
