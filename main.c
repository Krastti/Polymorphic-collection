#include "Матрицы/matrix.h"
#include "Тесты/tests.h"
#include "Логирование/logger.h"
#include "Меню/menu.h"

#include <windows.h>
#include <stdio.h>

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    if (!logger_init("matrix.log", LOG_DEBUG)) {
        fprintf(stderr, "Не удалось инициализировать логер.\n");
        return 1;
    }

    unit_tests();

    LOG_INFO("Программа завершила работу");
    logger_close();
    return 0;
}