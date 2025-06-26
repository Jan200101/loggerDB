#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "loggerDB/compat.h"
#include "loggerDB/db.h"
#include "loggerDB/table.h"
#include "loggerDB/node.h"
#include "loggerDB/status.h"
#include "loggerDB/mutex.h"

#include "path.h"

int ldb_open(const char* base_path, loggerdb* db)
{
    if (!base_path || !db)
        return LOGGERDB_ERROR;

    if (!ldb_path_exists(base_path))
    {
        mkdir(base_path, 0700);
    }
    else if (!ldb_path_is_dir(base_path))
    {
        return LOGGERDB_NOTADB;
    }

    db->path = strdup(base_path);

    return LOGGERDB_OK;
}

int ldb_close(loggerdb* db)
{
    if (!db)
        return LOGGERDB_INVALID;

    free(db->path);
    db->path = NULL;

    return LOGGERDB_OK;
}
