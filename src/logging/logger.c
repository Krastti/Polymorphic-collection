#include "logger.h"
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

static const char *LEVEL_COLORS[] = {
    "\033[36m",
    "\033[32m",
    "\033[33m",
    "\033[31m"
};
static const char *COLOR_RESET = "\033[0m";

static const char *LEVEL_NAMES[] = {
    "DEBUG", "INFO", "WARN", "ERROR"
};

Logger *logger_create(const char *filepath, const LogLevel min_level, const int use_color) {
    Logger *logger = (Logger *)malloc(sizeof(Logger));
    if (!logger) return NULL;

    if (filepath) {
        logger->output = fopen(filepath, "a");
        if (!logger->output) {
            free(logger);
            return NULL;
        }
        logger->use_color = 0;
    } else {
        logger->output = stderr;
        logger->use_color = use_color;
    }

    logger->min_level = min_level;
    return logger;
}

void logger_destroy(Logger *logger) {
    if (!logger) return;

    if (logger->output && logger->output != stderr && logger->output != stdout) {
        fclose(logger->output);
    }
    free(logger);
}

void logger_log(Logger *logger, const LogLevel level, const char *file, const int line, const char *fmt, ...) {
    if (!logger || level < logger->min_level) return;

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_buf[20];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);

    if (logger->use_color) {
        fprintf(logger->output, "%s[%s] [%s] %s:%d — %s", LEVEL_COLORS[level], LEVEL_NAMES[level], time_buf, file, line, COLOR_RESET);
    } else {
        fprintf(logger->output, "[%s] [%s] %s:%d — ", LEVEL_NAMES[level], time_buf, file, line);
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(logger->output, fmt, args);
    va_end(args);

    fprintf(logger->output, "\n");
    fflush(logger->output);
}