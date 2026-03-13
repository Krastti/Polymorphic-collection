#include "Логирование/logger.h"
#include "Tests/unit_tests.h"
#include "Tests/benchmark_tests.h"
#include "CLI/cli.h"

#include <stdio.h>
#include <windows.h>

void clearInputBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(const int argc, char* argv[]) {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    if (!logger_init("matrix.log", LOG_DEBUG)) {
        fprintf(stderr, "Не удалось инициализировать логер.\n");
        return 1;
    }

    // CLI-режим для Python-сервиса если передано много аргументов
    if (argc > 1) {
        int result = cli_run(argc, argv);
        logger_close();
        return result;
    }

    printf("Выберите режим тестирования\n");
    printf("1. UNIT-тесты\n");
    printf("2. Benchmarks LU\n");

    int choice;
    printf("Ваш выбор: ");
    scanf("%d", &choice);

    switch (choice)
    {
        case 1:
            unit_tests();
            getchar();
            break;
        case 2:
            test_lu_benchmark_with_html();
            break;
        default: break;
    }

    LOG_INFO("Программа завершила работу");
    logger_close();
    return 0;
}
