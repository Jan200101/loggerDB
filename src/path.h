#ifndef LOGGERDB_PATH_H
#define LOGGERDB_PATH_H

int ldb_path_exists(const char* path);
int ldb_path_is_file(const char* path);
int ldb_path_is_dir(const char* path);
char* ldb_path_join(const char* s1, const char* s2);

#endif