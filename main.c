#include "Логирование/logger.h"
#include "Menu/menu.h"
#include "WebInterface/web_interface.h"

#include <windows.h>
#include <stdio.h>

int choice;
int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    if (!logger_init("matrix.log", LOG_DEBUG)) {
        fprintf(stderr, "Не удалось инициализировать логер.\n");
        return 1;
    }

    if (!menu_init()) {
        LOG_ERROR("Не удалось инициализировать меню");
        return 1;
    }

    printHeader();
    int mode = menuSelectMode();

    if (mode == MODE_EXIT) {
        LOG_INFO("Пользователь выбрал выход");
        menu_cleanup();
        logger_close();
        return 0;
    }

    if (mode == MODE_WEB_INTERFACE) {
        LOG_INFO("Запуск веб-интерфейса");

        // Выбор типа элементов для веб-интерфейса
        int element_type = menuSelectElementType();

        if (element_type == 0) {
            LOG_ERROR("Не удалось выбрать тип элементов");
            menu_cleanup();
            logger_close();
            return 1;
        }

        // Генерация и открытие HTML файла
        if (!web_interface_open(element_type)) {
            LOG_ERROR("Не удалось инициализировать веб-интерфейс");
            menu_cleanup();
            logger_close();
            return 1;
        }

        printf("\n[ИНФО] Откройте файл matrix_interface.html в браузере для работы\n");
        printf("[ИНФО] Нажмите Enter для завершения программы...\n");
        getchar();

    } else if (mode == MODE_INTERACTIVE) {
        LOG_INFO("Запуск интерактивного режима");
        printHeader();

    } else if (mode == MODE_TESTING) {
        LOG_INFO("Запуск режима тестирования");
        // Здесь будут автоматические тесты
        // run_all_tests();
    }

    LOG_INFO("Программа завершила работу");
    web_interface_cleanup();
    menu_cleanup();
    logger_close();
    return 0;
}