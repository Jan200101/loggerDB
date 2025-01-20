#include <stdlib.h>
#include <stddef.h>

#include "loggerDB/mutex.h"
#include "loggerDB/status.h"

#include "FreeRTOS.h"
#include "semphr.h"

struct ldb_mutex  {
    SemaphoreHandle_t mutex;
    unsigned ref;
};

ldb_mutex* freertos_mutex_alloc(void)
{
    ldb_mutex* p;
    p = malloc(sizeof(*p));
    if (p)
    {
        p->mutex = xSemaphoreCreateMutex();
        p->ref = 0;
    }

    return p;
}

void freertos_mutex_free(ldb_mutex* p)
{
    assert(p);
    assert(p->ref == 0);
    vSemaphoreDelete(p->mutex);
    free(p);
}

void freertos_mutex_enter(ldb_mutex* p)
{
    xSemaphoreTake(p->mutex, portMAX_DELAY);
    p->ref++;
}

int freertos_mutex_try(ldb_mutex* p)
{
    if (xSemaphoreTake(p->mutex, 0) == pdTRUE)
    {
        p->ref++;
        return LOGGERDB_OK;
    }

    return LOGGERDB_BUSY;
}

void freertos_mutex_leave(ldb_mutex* p)
{
    assert(p->ref != 0);

    p->ref--;
    xSemaphoreGive(p->mutex);
}

const struct ldb_mutex_methods default_mutex = {
    .alloc = freertos_mutex_alloc,
    .free = freertos_mutex_free,
    .enter = freertos_mutex_enter,
    .try = freertos_mutex_try,
    .leave = freertos_mutex_leave,
};

