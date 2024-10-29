#ifndef LOGGERDB_MUTEX_H
#define LOGGERDB_MUTEX_H

typedef struct ldb_mutex ldb_mutex;

struct ldb_mutex_methods {
    ldb_mutex* (*alloc)(void);
    void (*free)(ldb_mutex*);
    void (*enter)(ldb_mutex*);
    int (*try)(ldb_mutex*);
    void (*leave)(ldb_mutex*);
};

extern const struct ldb_mutex_methods default_mutex;
extern const struct ldb_mutex_methods* mutex;

#endif