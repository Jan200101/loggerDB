#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <dirent.h>
#endif

#include "loggerDB/db.h"

#define LDB_PATH_SEP "/"

// Returns non-zero if path exists
int ldb_path_exists(const char* path)
{
#ifdef _MSC_VER
    DWORD a = GetFileAttributesA(path);
    if (a == INVALID_FILE_ATTRIBUTES)
        return 0;
    return (a & (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_DIRECTORY)) != 0;
#else
    struct stat sb = {0};
    return stat(path, &sb) == 0;
#endif
}

// Returns non-zero if path is a file
int ldb_path_is_file(const char* path)
{
#ifdef _MSC_VER
    DWORD a = GetFileAttributesA(path);
    if (a == INVALID_FILE_ATTRIBUTES)
        return 0;
    return (a & FILE_ATTRIBUTE_NORMAL) != 0;
#else
    struct stat sb = {0};
    stat(path, &sb);
    return S_ISREG(sb.st_mode);
#endif
}

// Returns non-zero if path is a directory
int ldb_path_is_dir(const char* path)
{
#ifdef _MSC_VER
    DWORD a = GetFileAttributesA(path);
    if (a == INVALID_FILE_ATTRIBUTES)
        return 0;
    return (a & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
    struct stat sb = {0};
    stat(path, &sb);
    return S_ISDIR(sb.st_mode);
#endif
}

char* ldb_path_join(const char* s1, const char* s2)
{
    if (!s1 || !s2)
        return NULL;

    size_t len = strlen(s1) + strlen(LDB_PATH_SEP) + strlen(s2);
    if (!len)
        return NULL;

    char* o = malloc(len+1);
    *o = '\0';

    strcat(o, s1);
    strcat(o, LDB_PATH_SEP);
    strcat(o, s2);

    return o;
}
