#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static FILE* _log_file = NULL;
static LogLevel _current_level = LOG_INFO;
static bool _initialized = false;
static bool _log_to_console = false;

static const char* _level_to_string(LogLevel level) {
    switch (level) {
        case LOG_DEBUG: return "DEBUG";
        case LOG_INFO: return "INFO";
        case LOG_WARN: return "WARN";
        case LOG_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

static void _get_timestamp(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

bool logger_init(const char* filename, LogLevel level)
{
    if (_initialized) {
        logger_close();
    }

    _current_level = level;

    if (filename != NULL) {
        _log_file = fopen(filename, "a");

        if (_log_file == NULL) {
            fprintf(stderr, "Неудалось открыть файл %s\n", filename);
            return false;
        }
    }

    _initialized = true;
    LOG_INFO("Логирование инициализировано. Level: %s", _level_to_string(level));
    return true;
}

void logger_close(void)
{
    if (_log_file != NULL)
    {
        fclose(_log_file);
        _log_file = NULL;
    }
    _initialized = false;
}

void logger_set_level(LogLevel level) {
    _current_level = level;
}

void _log_write(LogLevel level, const char* file, int line, char* format, ...)
{
    if (!_initialized || level < _current_level){return;}

    char timestamp[32];
    _get_timestamp(timestamp, sizeof(timestamp));

    const char* filename = file;
    for (const char* p = file; *p; p++) {
        if (*p == '/' || *p == '\\') {
            filename = p + 1;
        }
    }

    char header[256];
    snprintf(header, sizeof(header), "%s [%s] %s:%d ", timestamp, _level_to_string(level), filename, line);

    va_list args;
    va_start(args, format);
    if (_log_to_console)
    {
        FILE* out = (level >= LOG_WARN) ? stderr : stdout;
        fprintf(out, "%s", header);

        va_list args_copy;
        va_copy(args_copy, args);
        vfprintf(out, format, args_copy);
        va_end(args_copy);

        fprintf(out, "\n");
        fflush(out);
    }

    if (_log_file != NULL)
    {
        fprintf(_log_file, "%s", header);
        va_start(args, format);
        vfprintf(_log_file, format, args);
        va_end(args);
        fprintf(_log_file, "\n");
        fflush(_log_file);
    }
    va_end(args);
}

LogLevel logger_get_level(void) {
    return _current_level;
}

void logger_enable_console(bool enable) {
    _log_to_console = enable;
}