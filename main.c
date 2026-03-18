#include "src/logging/logger.h"
#include "tests/unit_tests.h"
#include <stdio.h>
#include <windows.h>

Logger *g_logger = NULL;

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    g_logger = logger_create(NULL, LOG_DEBUG, 1);
    if (!g_logger) {
        fprintf(stderr, "Не удалось создать логгер\n");
        return 1;
    }
    LOG_DEBUG(g_logger, "Программа запущена");

    run_all_tests();

    logger_destroy(g_logger);
    return 0;
}
