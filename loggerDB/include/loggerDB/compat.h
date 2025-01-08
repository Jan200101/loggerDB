#ifndef LOGGERDB_COMPAT_H
#define LOGGERDB_COMPAT_H

#if defined(_MSC_VER)

#define mkdir(A, B) mkdir(A)

#include <BaseTsd.h>
typedef SSIZE_T ssize_t;

#endif

#endif
