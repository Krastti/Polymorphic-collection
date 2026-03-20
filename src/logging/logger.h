#ifndef LABORATORYWORK1_LOGGER_H
#define LABORATORYWORK1_LOGGER_H

#include <stdio.h>

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO  = 1,
    LOG_WARN  = 2,
    LOG_ERROR = 3
} LogLevel;

void logger_set_level(LogLevel level);
void logger_log(LogLevel level, const char* fmt, ...);

#define LOG_DEBUG(fmt, ...) logger_log(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  logger_log(LOG_INFO, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  logger_log(LOG_WARN, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) logger_log(LOG_ERROR, fmt, ##__VA_ARGS__)
#endif //LABORATORYWORK1_LOGGER_H