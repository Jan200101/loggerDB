#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "loggerDB/node.h"
#include "loggerDB/stream.h"
#include "loggerDB/status.h"

int ldb_stream_open(loggerdb_node* node, const char* field, size_t membs, int flags, loggerdb_stream** stream)
{
    if (!node || !field || !stream)
        return LOGGERDB_INVALID;

    int fd = openat(node->fd, field, O_RDWR | O_CREAT, S_IRWXU);
    if (fd < 0)
        return LOGGERDB_FDERR;

    *stream = malloc(sizeof(**stream));
    (*stream)->fd = fd;
    (*stream)->membs = membs;

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
    free(stream);

    return LOGGERDB_OK;
}

ssize_t ldb_stream_read(loggerdb_stream* stream, void* ptr)
{
    if (!stream || !ptr)
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
    if (!stream || !ptr)
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
    if (!stream)
        return -LOGGERDB_INVALID;
    if (stream->fd < 0)
        return -LOGGERDB_FDERR;

    return lseek(stream->fd, offset, whence);
}
