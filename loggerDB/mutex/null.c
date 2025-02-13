#include <stddef.h>

#include "loggerDB/mutex.h"
#include "loggerDB/status.h"

ldb_mutex* null_mutex_alloc(void) { return NULL; }
void null_mutex_free(ldb_mutex* p) {}
void null_mutex_enter(ldb_mutex* p) {}
int null_mutex_try(ldb_mutex* p) { return LOGGERDB_OK; }
void null_mutex_leave(ldb_mutex* p) {}

const struct ldb_mutex_methods default_mutex = {
    .alloc = null_mutex_alloc,
    .free = null_mutex_free,
    .enter = null_mutex_enter,
    .try = null_mutex_try,
    .leave = null_mutex_leave,
};

