#ifndef BENCH_UTILS
#define BENCH_UTILS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <unistd.h>

#define DB_PATH "db_test"
#define TABLE_NAME "test_table"

#define EPOCH_MINUTES(x) (60*(x))
#define EPOCH_HOURS(x) (60*EPOCH_MINUTES(x))
#define EPOCH_DAYS(x) (24*EPOCH_HOURS(x))

struct dataset {
    time_t time;
    float values[4];
};

static inline float rand_float(void)
{
    return (float)rand()/(float)(RAND_MAX/100);
}

static inline int removeDir(const char *path)
{
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d) {
        struct dirent *p;

        r = 0;
        while (!r && (p=readdir(d))) {
            char *buf;
            size_t len;

            // Skip the names "." and ".." as we don't want to recurse on them.
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);

            if (buf) {
                struct stat statbuf = {0};

                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode))
                        r = removeDir(buf);
#ifndef _WIN32
                    else if (S_ISLNK(statbuf.st_mode))
                        r = unlink(buf);
#endif
                    else
                        r = remove(buf);
                }
                else // it is very likely that we found a dangling symlink which is not detected by stat
                {
                    r = unlink(buf);
                }
                free(buf);
            }
        }
        closedir(d);
    }

    if (!r)
        r = rmdir(path);

    return r;
}


#endif
