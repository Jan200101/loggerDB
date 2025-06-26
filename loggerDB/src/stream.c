#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "loggerDB/node.h"
#include "loggerDB/stream.h"
#include "loggerDB/path.h"
#include "loggerDB/status.h"

int ldb_stream_open(loggerdb_node* node, const char* field, size_t membs, int flags, loggerdb_stream* stream)
{
    if (!node || !field || !stream)
        return LOGGERDB_INVALID;

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
        return LOGGERDB_ERROR;

    int fd = open(field_path, O_RDWR | O_CREAT, 0644);
    free(field_path);
    if (fd < 0)
    {
        printf("\n%s %i\n", field_path, fd);
        return LOGGERDB_FDERR;
    }

    stream->fd = fd;
    stream->membs = membs;

    return LOGGERDB_OK;
}

int ldb_stream_close(struct loggerdb_stream* stream)
{
    if (!stream)
        return LOGGERDB_INVALID;
    if (stream->fd < 0)
        return LOGGERDB_FDERR;

    close(stream->fd);
    stream->fd = -1;
    stream->membs = 0;

    return LOGGERDB_OK;
}

ssize_t ldb_stream_read(loggerdb_stream* stream, void* ptr)
{
    if (!stream || !stream->membs || !ptr)
        return -LOGGERDB_INVALID;
    if (stream->fd < 0)
        return -LOGGERDB_FDERR;

    ssize_t s = read(stream->fd, ptr, stream->membs);
    if (s < 0)
        return -LOGGERDB_FDBAD;
    if (s != stream->membs)
        return -LOGGERDB_INVALID;

    return s;
}

ssize_t ldb_stream_write(loggerdb_stream* stream, void* ptr)
{
    if (!stream || !stream->membs || !ptr)
        return -LOGGERDB_INVALID;
    if (stream->fd < 0)
        return -LOGGERDB_FDERR;

    ssize_t s = write(stream->fd, ptr, stream->membs);
    if (s < 0)
        return -LOGGERDB_FDBAD;
    if (s != stream->membs)
        return -LOGGERDB_INVALID;

    return s;
}

ssize_t ldb_stream_seek(loggerdb_stream* stream, off_t offset, int whence)
{
    if (!stream || !stream->membs)
        return -LOGGERDB_INVALID;
    if (stream->fd < 0)
        return -LOGGERDB_FDERR;

    return lseek(stream->fd, offset, whence);
}
