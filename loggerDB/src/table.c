#include <stdlib.h>
#include <sys/stat.h>

#include "loggerDB/table.h"
#include "loggerDB/path.h"
#include "loggerDB/status.h"
#include "loggerDB/db.h"
#include "loggerDB/compat.h"

int ldb_table_open(loggerdb* db, const char* name, loggerdb_table* table)
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

    table->path = table_path;
    table->db = db;

    return LOGGERDB_OK;
}

int ldb_table_close(loggerdb_table* table)
{
    if (!table)
        return LOGGERDB_INVALID;

    free(table->path);
    table->path = NULL;
    table->db = NULL;

    return LOGGERDB_OK;
}

