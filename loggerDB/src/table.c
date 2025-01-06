#include <stdlib.h>
#include <sys/stat.h>

#include "loggerDB/table.h"
#include "loggerDB/path.h"
#include "loggerDB/status.h"
#include "loggerDB/db.h"

#if (defined(_WIN32) || defined(__WIN32__))
#define mkdir(A, B) mkdir(A)
#endif

int ldb_table_open(loggerdb* db, const char* name, loggerdb_table** table)
{
    if (!db || !table)
        return LOGGERDB_INVALID;

    char* table_path = ldb_path_join(db->path, name);
    if (!table_path)
        return LOGGERDB_ERROR;

    if (!ldb_path_exists(table_path))
        mkdir(table_path, 0700);
    else if (!ldb_path_is_dir(table_path))
        return LOGGERDB_ERROR;

    *table = malloc(sizeof(**table));
    (*table)->path = table_path;
    (*table)->db = db;

    return LOGGERDB_OK;
}

int ldb_table_close(loggerdb_table* table)
{
    if (!table)
        return LOGGERDB_OK;

    free(table->path);
    free(table);

    return LOGGERDB_OK;
}

