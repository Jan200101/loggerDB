#include <stddef.h>

#include "mutex/mutex.h"
#include "status.h"

ldb_mutex* null_mutex_alloc(void) { return NULL; }
void null_mutex_free(ldb_mutex*) {}
void null_mutex_enter(ldb_mutex*) {}
int null_mutex_try(ldb_mutex*) { return LOGGERDB_OK; }
void null_mutex_leave(ldb_mutex*) {}

const struct ldb_mutex_methods default_mutex = {
    .alloc = null_mutex_alloc,
    .free = null_mutex_free,
    .enter = null_mutex_enter,
    .try = null_mutex_try,
    .leave = null_mutex_leave,
};

