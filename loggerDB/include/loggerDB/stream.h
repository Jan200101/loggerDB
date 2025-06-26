#ifndef LOGGERDB_STREAM_H
#define LOGGERDB_STREAM_H

#include <sys/types.h>

typedef struct loggerdb_node loggerdb_node;

typedef struct loggerdb_stream {
    int fd;
    size_t membs;
} loggerdb_stream;

int ldb_stream_open(loggerdb_node* node, const char* field, size_t membs, int flags, loggerdb_stream* stream);
int ldb_stream_close(struct loggerdb_stream*);
ssize_t ldb_stream_read(loggerdb_stream* stream, void* ptr);
ssize_t ldb_stream_write(loggerdb_stream* stream, void* ptr);
ssize_t ldb_stream_seek(loggerdb_stream* stream, off_t offset, int whence);

#endif
