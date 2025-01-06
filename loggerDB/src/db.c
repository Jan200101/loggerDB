#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "loggerDB/db.h"
#include "loggerDB/table.h"
#include "loggerDB/node.h"
#include "loggerDB/path.h"
#include "loggerDB/status.h"

#include "loggerDB/mutex.h"

#if (defined(_WIN32) || defined(__WIN32__))
#define mkdir(A, B) mkdir(A)
#endif

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
