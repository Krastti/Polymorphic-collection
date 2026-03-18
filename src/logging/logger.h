#ifndef LABORATORYWORK1_LOGGER_H
#define LABORATORYWORK1_LOGGER_H

#include <stdio.h>

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO  = 1,
    LOG_WARN  = 2,
    LOG_ERROR = 3
} LogLevel;

typedef struct {
    FILE     *output;
    LogLevel  min_level;
    int       use_color;
} Logger;

Logger *logger_create(const char *filepath, LogLevel min_level, int use_color);
void logger_destroy(Logger *logger);

void logger_log(Logger *logger, LogLevel level, const char *file, int line, const char *fmt, ...);

#define LOG_DEBUG(logger, fmt, ...) \
logger_log(logger, LOG_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(logger, fmt, ...) \
logger_log(logger, LOG_INFO,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(logger, fmt, ...) \
logger_log(logger, LOG_WARN,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(logger, fmt, ...) \
logger_log(logger, LOG_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

extern Logger *g_logger;

#endif //LABORATORYWORK1_LOGGER_H