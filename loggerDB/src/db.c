#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "loggerDB/compat.h"
#include "loggerDB/db.h"
#include "loggerDB/table.h"
#include "loggerDB/node.h"
#include "loggerDB/status.h"
#include "loggerDB/mutex.h"


int ldb_open(const char* base_path, loggerdb** db)
{
    if (!base_path)
        return LOGGERDB_ERROR;

    mkdir(base_path, S_IRWXU);

    int fd = open(base_path, O_DIRECTORY | O_RDONLY);
    if (fd < 0)
        return LOGGERDB_ERROR;

    *db = malloc(sizeof(**db));
    (*db)->fd = fd;

    return LOGGERDB_OK;
}

int ldb_close(loggerdb* db)
{
    if (!db)
        return LOGGERDB_OK;

    close(db->fd);
    free(db);

    return LOGGERDB_OK;
}
