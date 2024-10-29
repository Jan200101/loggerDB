#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "db.h"
#include "table.h"
#include "node.h"
#include "path.h"
#include "status.h"

#include "mutex/mutex.h"

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