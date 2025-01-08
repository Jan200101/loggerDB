#ifndef LOGGERDB_COMPAT_H
#define LOGGERDB_COMPAT_H

#if (defined(_WIN32) || defined(__WIN32__))

#define mkdir(A, B) mkdir(A)

#include <basetsd.h>
typedef SSIZE_T ssize_t;

#endif

#endif
