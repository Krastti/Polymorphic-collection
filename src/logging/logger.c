#include "logger.h"
#include <stdarg.h>
#include <time.h>

static LogLevel current_level = LOG_DEBUG;

static const char *LEVEL_COLORS[] = {
    "\033[36m",
    "\033[32m",
    "\033[33m",
    "\033[31m"
};
static const char *COLOR_RESET = "\033[0m";

static const char *LEVEL_NAMES[] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR"
};


void logger_set_level(LogLevel level) {
  current_level = level;
}

void logger_log(LogLevel level, const char* fmt, ...) {
  if (level < current_level) return;

  time_t now = time(NULL);
  struct tm* t = localtime(&now);

  char timebuf[20];
  strftime(timebuf, sizeof(timebuf), "%H:%M:%S", t);

  printf("%s[%s] [%s]%s ", LEVEL_COLORS[level], timebuf, LEVEL_NAMES[level], COLOR_RESET);

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);

  printf("\n");
  fflush(stdout);
}