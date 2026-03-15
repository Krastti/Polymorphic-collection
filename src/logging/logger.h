#ifndef LABORATORYWORK1_LOGGER_H
#define LABORATORYWORK1_LOGGER_H

#include  <stdbool.h>

typedef enum
{
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3,
} LogLevel;

bool logger_init(const char* filename, LogLevel level);

void logger_close(void);

void logger_set_level(LogLevel level);

void _log_write(LogLevel level, const char* file, int line, char* format, ...);

void logger_enable_console(bool enable);

LogLevel log_get_level(void);

#define LOG_DEBUG(fmt, ...) _log_write(LOG_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) _log_write(LOG_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) _log_write(LOG_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) _log_write(LOG_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif //LABORATORYWORK1_LOGGER_H