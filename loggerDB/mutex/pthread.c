#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <pthread.h>

#include "mutex/mutex.h"
#include "status.h"

struct ldb_mutex  {
    pthread_mutex_t mutex;
    unsigned ref;
};

ldb_mutex* pthread_mutex_alloc(void)
{
    ldb_mutex* p;
    p = malloc(sizeof(*p));
    if (p)
    {
        pthread_mutex_init(&p->mutex, 0);
        p->ref = 0;
    }

    return p;
}

void pthread_mutex_free(ldb_mutex* p)
{
    if (p)
    {
        assert(p->ref == 0);
        pthread_mutex_destroy(&p->mutex);
        free(p);
    }
}

void pthread_mutex_enter(ldb_mutex* p)
{
    pthread_mutex_lock(&p->mutex);
    p->ref++;
}

int pthread_mutex_try(ldb_mutex* p)
{
    if( pthread_mutex_trylock(&p->mutex) == 0 )
    {
        p->ref++;
        return LOGGERDB_OK;
    }

    return LOGGERDB_BUSY;
}
void pthread_mutex_leave(ldb_mutex* p)
{
    assert(p->ref != 0);

    p->ref--;
    pthread_mutex_unlock(&p->mutex);
}

const struct ldb_mutex_methods default_mutex = {
    .alloc = pthread_mutex_alloc,
    .free = pthread_mutex_free,
    .enter = pthread_mutex_enter,
    .try = pthread_mutex_try,
    .leave = pthread_mutex_leave,
};

