#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "loggerDB/table.h"
#include "loggerDB/status.h"
#include "loggerDB/db.h"
#include "loggerDB/compat.h"

int ldb_table_open(loggerdb* db, const char* name, loggerdb_table** table)
{
    if (!db || !table)
        return LOGGERDB_INVALID;

    mkdirat(db->fd, name, S_IRWXU);

    int fd = openat(db->fd, name, O_DIRECTORY | O_RDONLY);
    if (fd < 0)
        return LOGGERDB_ERROR;

    *table = malloc(sizeof(**table));
    (*table)->fd = fd;
    (*table)->db = db;

    return LOGGERDB_OK;
}

int ldb_table_close(loggerdb_table* table)
{
    if (!table)
        return LOGGERDB_OK;

    close(table->fd);
    free(table);

    return LOGGERDB_OK;
}

