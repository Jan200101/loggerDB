#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include "loggerDB/table.h"
#include "loggerDB/node.h"
#include "loggerDB/path.h"
#include "loggerDB/status.h"
#include "loggerDB/compat.h"

#define METADATA_SIZE_LIMIT 255

#define ERA_OFFSET ((int32_t)3670)
/// Every era has 146097 days
#define DAYS_IN_ERA ((int32_t)146097)
/// Every era has 400 years
#define YEARS_IN_ERA ((int32_t)400)
/// Number of days from 0000-03-01 to Unix epoch 1970-01-01
#define DAYS_TO_UNIX_EPOCH ((int32_t)719468)
/// Offset to be added to given day values
#define DAY_OFFSET (int32_t)(ERA_OFFSET * DAYS_IN_ERA + DAYS_TO_UNIX_EPOCH)
/// Offset to be added to given year values
#define YEAR_OFFSET (int32_t)(ERA_OFFSET * YEARS_IN_ERA)
/// Seconds in a single 24 hour calendar day
#define SECS_IN_DAY ((int64_t)86400)
/// Offset to be added to given second values
#define SECS_OFFSET (int64_t)(DAY_OFFSET * SECS_IN_DAY)

struct dt {
    int32_t year;
    uint8_t mon;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
};

const char* dd_lut[61] = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60"};

struct dt* datetime(const time_t* timep, struct dt* result)
{
    assert(timep);
    assert(result);

    const uint64_t secs2 = (*timep) + SECS_OFFSET;
    const uint32_t days = secs2 / SECS_IN_DAY;
    const uint64_t secs3 = secs2 % SECS_IN_DAY;

    const uint64_t prd1 = 71582789 * secs3;
    const uint64_t mins = prd1 >> 32; // secs / 60
    result->sec = (uint8_t)(((uint32_t)prd1) / 71582789);

    const uint64_t prd2 = 71582789 * mins;
    result->hour = (uint8_t)(prd2 >> 32);
    result->min = (uint8_t)(((uint32_t)prd2) / 71582789);

    //century
    const uint32_t n1 = 4 * days + 3;
    const uint32_t c = n1 / 146097;
    const uint32_t r = n1 % 146097;
    // year
    const uint32_t n2 = r | 3;
    const uint64_t p = 2939745 * ((uint64_t)n2);
    const uint32_t z = (p / ((uint64_t)1 << 32));
    const uint32_t n3 = ((p % ((uint64_t)1 << 32)) / 2939745 / 4);
    const int j = n3 >= 306;
    const uint32_t y1 = 100 * c + z + j;

    // month and day
    const uint32_t n4 = 2141 * n3 + 197913;
    const uint32_t m1 = n4 / (1 << 16);
    const uint32_t d1 = n4 % (1 << 16) / 2141;

    result->year = (((int32_t)y1) - YEAR_OFFSET);
    result->mon = (uint8_t)((j != 0 ? m1 - 12 : m1));
    result->day = d1 + 1;

    return result;
}

static int strtime(struct dt* newtime, char* buff)
{
    for (int i = 4;i;)
    {
        buff[--i] = '0' + (newtime->year % 10);
        newtime->year /= 10; 
    }
    buff[4] = '/';
    memcpy(buff+5, dd_lut[newtime->mon], 2);
    buff[7] = '/';
    memcpy(buff+8, dd_lut[newtime->day], 2);
    buff[10] = '/';
    memcpy(buff+11, dd_lut[newtime->hour], 2);
    buff[13] = '/';
    memcpy(buff+14, dd_lut[newtime->min], 2);
    buff[16] = '\0';
}

int ldb_node_check(loggerdb_table* table, time_t time)
{
    if (!table)
        return LOGGERDB_INVALID;

    struct dt newtime;

    if (!datetime(&time, &newtime))
        return LOGGERDB_ERROR;

    char timebuff[18]; // = "YYYY/MM/DD/HH/MM";
    strtime(&newtime, timebuff);

    assert(table->path);
    char* node_path = ldb_path_join(table->path, timebuff);
    if (!node_path)
        return LOGGERDB_ERROR;

    int ret = LOGGERDB_OK;
    if (!ldb_path_is_dir(node_path))
        ret = LOGGERDB_NOTFOUND;

    free(node_path);

    return ret;
}

int ldb_node_open(loggerdb_table* table, time_t time, loggerdb_node** node)
{
    if (!table || !node)
        return LOGGERDB_INVALID;

    *node = NULL;
    struct dt newtime;

    if (!datetime(&time, &newtime))
        return LOGGERDB_ERROR;

    char timebuff[18]; // = "YYYY/MM/DD/HH/MM";
    strtime(&newtime, timebuff);

    assert(table->path);
    char* node_path = ldb_path_join(table->path, timebuff);
    if (!node_path)
        return LOGGERDB_ERROR;

    char* p = node_path + strlen(table->path);

    if (!ldb_path_is_dir(node_path))
    {        
        while (*p)
        {
            while (*p != '\0' && *p != '/')
                ++p;

            char c = *p;
            *p = '\0';

            if (!ldb_path_exists(node_path))
            {
                mkdir(node_path, 0700);
            }
            else if (!ldb_path_is_dir(node_path))
            {
                free(node_path);
                return LOGGERDB_ERROR;
            }

            *p = c;

            if (c)
                ++p;
        }
    }

    // Round to minutes
    time = (time - time % ldb_node_spacing());

    *node = malloc(sizeof(**node));

    (*node)->time = time;
    (*node)->mutex = mutex->alloc();
    (*node)->path = node_path;

    return LOGGERDB_OK;
}

int ldb_node_close(loggerdb_node* node)
{
    if (!node)
        return LOGGERDB_INVALID;

    free(node->path);
    mutex->free(node->mutex);
    free(node);

    return LOGGERDB_OK;
}

int ldb_node_contains(loggerdb_node* node, time_t time)
{
    if (!node)
        return LOGGERDB_INVALID;

    // Round time down to minutes
    time = (time - time % ldb_node_spacing());    

    if (node->time == time)
        return LOGGERDB_OK;

    return LOGGERDB_ERROR;
}

int ldb_node_spacing(void)
{
    return 60;
}

ssize_t ldb_node_size(loggerdb_node* node, const char* field)
{
    if (!node)
        return -LOGGERDB_INVALID;

    ssize_t ret;
    mutex->enter(node->mutex);

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

    if (!ldb_path_is_file(field_path))
    {
        free(field_path);
        ret = 0;
        goto cleanup;
    }

    // Use append to get the size to prevent double seeking on filesystems
    // where files are stored as backwards linked-lists
    int fd = open(field_path, O_RDONLY | O_APPEND);
    free(field_path);

    if (fd < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

    ret = lseek(fd, 0, SEEK_END);

    if (close(fd) < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

cleanup:
    mutex->leave(node->mutex);

    return ret;
}

static inline ssize_t _ldb_node_read_file(loggerdb_node* node, const char* path, long offset, void* ptr, size_t size)
{
    ssize_t ret;

    mutex->enter(node->mutex);

    int fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }


    lseek(fd, offset, SEEK_SET);

    ret = read(fd, ptr, size);

    if (close(fd) < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

cleanup:
    mutex->leave(node->mutex);

    return ret;
}

ssize_t ldb_node_read(loggerdb_node* node, const char* field, void* ptr, size_t size)
{
    if (!node)
        return -LOGGERDB_INVALID;

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
        return -LOGGERDB_ERROR;

    ssize_t ret = _ldb_node_read_file(node, field_path, 0, ptr, size);
    free(field_path);

    return ret;
}

ssize_t ldb_node_read_offset(loggerdb_node* node, const char* field, long offset, void* ptr, size_t size)
{
    if (!node)
        return -LOGGERDB_INVALID;

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
        return -LOGGERDB_ERROR;

    ssize_t ret = _ldb_node_read_file(node, field_path, offset, ptr, size);
    free(field_path);

    return ret;
}

ssize_t ldb_node_write(loggerdb_node* node, const char* field, void* ptr, size_t size)
{
    assert(node);

    ssize_t ret;
    mutex->enter(node->mutex);

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

    int fd = open(field_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    free(field_path);

    if (fd < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

    ret = write(fd, ptr, size);
    if (close(fd) < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

cleanup:
    mutex->leave(node->mutex);

    return ret;
}

ssize_t ldb_node_append(loggerdb_node* node, const char* field, void* ptr, size_t size)
{
    assert(node);

    ssize_t ret;
    mutex->enter(node->mutex);

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

    int fd = open(field_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    free(field_path);

    if (fd < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

    ret = write(fd, ptr, size);
    if (close(fd) < 0)
    {
        ret = -LOGGERDB_ERROR;
        goto cleanup;
    }

cleanup:
    mutex->leave(node->mutex);

    return ret;
}

int ldb_node_exists(loggerdb_node* node, const char* field)
{
    assert(node);

    char* field_path = ldb_path_join(node->path, field);
    if (!field_path)
        return -LOGGERDB_ERROR;

    if (!ldb_path_exists(field_path))
    {
        free(field_path);
        return LOGGERDB_NOTFOUND;
    }

    return LOGGERDB_OK;
}

ssize_t ldb_node_metadata_read(loggerdb_node* node, void* ptr, size_t size)
{
    if (size > METADATA_SIZE_LIMIT)
        return -LOGGERDB_ERROR;

    return ldb_node_read(node, "metadata", ptr, size);
}

ssize_t ldb_node_metadata_write(loggerdb_node* node, void* ptr, size_t size)
{
    if (size > METADATA_SIZE_LIMIT)
        return -LOGGERDB_ERROR;

    return ldb_node_write(node, "metadata", ptr, size);
}
