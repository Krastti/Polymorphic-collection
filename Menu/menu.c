//
// Created by Krastti on 04.03.2026.
//

#include "menu.h"
#include "../WebInterface/web_interface.h"
#include <errno.h>
#include <limits.h>

static bool _menu_initialized = false;

bool menu_init(void)
{
    if (_menu_initialized) {
        LOG_WARN("Меню уже инициализировано");
        return true;
    }

    LOG_INFO("Инициализация модуля меню");
    _menu_initialized = true;

    return true;
}

void menu_cleanup(void)
{
    if (!_menu_initialized) {
        LOG_WARN("Попытка очистки неинициализированного меню");
        return;
    }

    LOG_INFO("Завершение работы модуля меню");
    _menu_initialized = false;
}

int menuSelectMode(void)
{
    LOG_INFO("Запрос выбора режима работы программы");

    int choice;
    printf("\n================================================================================\n");
    printf("ВЫБОР РЕЖИМА РАБОТЫ\n");
    printf("================================================================================\n");
    printf("1. Интерактивный режим (консольное меню)\n");
    printf("2. Режим тестирования (автоматические тесты операций)\n");
    printf("3. Веб-интерфейс (HTML страница с JavaScript)\n");
    printf("0. Выход\n");
    printf("================================================================================\n");
    printf("Ваш выбор: ");

    if (scanf("%d", &choice) != 1) {
        LOG_ERROR("Неверный формат выбора режима работы");
        clearInputBuffer();
        return MODE_EXIT;
    }

    clearInputBuffer();

    if (choice < MODE_EXIT || choice > MODE_WEB_INTERFACE) {
        LOG_ERROR("Недопустимый выбор режима работы: %d", choice);
        printWarning("Выбран режим по умолчанию: Интерактивный");
        return MODE_INTERACTIVE;
    }

    if (choice == MODE_EXIT) {
        LOG_INFO("Пользователь выбрал выход из программы");
        return MODE_EXIT;
    }

    const char* mode_name;
    switch (choice) {
        case MODE_INTERACTIVE:   mode_name = "Интерактивный (консоль)"; break;
        case MODE_TESTING:       mode_name = "Тестирование"; break;
        case MODE_WEB_INTERFACE: mode_name = "Веб-интерфейс (HTML)"; break;
        default:                 mode_name = "Неизвестный"; break;
    }

    LOG_INFO("Выбран режим работы: %s (код: %d)", mode_name, choice);
    printf(">>> Выбран режим: %s\n\n", mode_name);

    return choice;
}

void printHeader(void)
{
    LOG_DEBUG("Вывод заголовка меню");
    printSeparator();
    printf("%s\n", MENU_TITLE);
    printf("%s\n", MENU_SUBTITLE);
    printSeparator();
}

void printSeparator(void)
{
    printf("================================================================================\n");
}

void clearInputBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void printError(const char* message)
{
    printf("[ОШИБКА] %s\n", message);
    LOG_ERROR("Ошибка в меню: %s", message);
}

void printInfo(const char* message)
{
    printf("[ИНФО] %s\n", message);
    LOG_INFO("Информация в меню: %s", message);
}

void printWarning(const char* message)
{
    printf("[ПРЕДУПРЕЖДЕНИЕ] %s\n", message);
    LOG_WARN("Предупреждение в меню: %s", message);
}

void printMainMenu(void)
{
    LOG_DEBUG("Вывод главного меню");
    printf("\n--- ГЛАВНОЕ МЕНЮ ---\n");
    printf("1. Выбрать тип элементов (Вещественные / Комплексные)\n");
    printf("2. Создать/Заполнить Матрицу A\n");
    printf("3. Создать/Заполнить Матрицу B\n");
    printf("4. Отобразить Матрицу A\n");
    printf("5. Отобразить Матрицу B\n");
    printf("6. Операция: C = A + B (Сложение матриц)\n");
    printf("7. Операция: C = A * B (Умножение матриц)\n");
    printf("8. Операция: C = A * scalar (Умножение на скаляр)\n");
    printf("9. Освободить память (сброс матриц)\n");
    printf("0. Выход из программы\n");
    printf("---------------------\n");
}

int readInt(const char* prompt, int* value)
{
    LOG_DEBUG("Чтение целочисленного значения: %s", prompt);

    printf("%s", prompt);

    if (scanf("%d", value) != 1) {
        LOG_ERROR("Неверный формат ввода целого числа");
        clearInputBuffer();
        return 0;
    }

    clearInputBuffer();
    LOG_DEBUG("Успешно прочитано значение: %d", *value);

    return 1;
}

int readDouble(const char* prompt, double* value)
{
    LOG_DEBUG("Чтение вещественного значения: %s", prompt);

    printf("%s", prompt);

    if (scanf("%lf", value) != 1) {
        LOG_ERROR("Неверный формат ввода вещественного числа");
        clearInputBuffer();
        return 0;
    }

    clearInputBuffer();
    LOG_DEBUG("Успешно прочитано значение: %lf", *value);

    return 1;
}

int readMatrixSize(const char* prompt, size_t* value)
{
    LOG_DEBUG("Чтение размера матрицы: %s", prompt);

    int temp;
    printf("%s", prompt);

    if (scanf("%d", &temp) != 1) {
        LOG_ERROR("Неверный формат ввода размера матрицы");
        clearInputBuffer();
        return 0;
    }

    clearInputBuffer();

    if (temp <= 0) {
        LOG_ERROR("Размер матрицы должен быть положительным: %d", temp);
        return 0;
    }

    if (temp > 1000) {
        LOG_WARN("Большой размер матрицы может потребовать много памяти: %d", temp);
    }

    *value = (size_t)temp;
    LOG_DEBUG("Успешно прочитан размер матрицы: %zu", *value);

    return 1;
}

int menuSelectElementType(void)
{
    LOG_INFO("Запрос выбора типа элементов");

    int choice;
    printf("\n>>> Выберите тип элементов для коллекции:\n");
    printf("1. Вещественные числа (double)\n");
    printf("2. Комплексные числа (complex: действительная + мнимая часть)\n");
    printf("Ваш выбор: ");

    if (scanf("%d", &choice) != 1) {
        LOG_ERROR("Неверный формат выбора типа элементов");
        clearInputBuffer();
        return 0;
    }

    clearInputBuffer();

    if (choice != 1 && choice != 2) {
        LOG_ERROR("Недопустимый выбор типа элементов: %d", choice);
        return 0;
    }

    const char* type_name = (choice == 1) ? "Вещественные" : "Комплексные";
    LOG_INFO("Выбран тип элементов: %s", type_name);
    printf(">>> Выбран тип: %s\n", type_name);

    return choice;
}

int menuInputScalar(int type, double* real_out, double* imag_out)
{
    LOG_DEBUG("Ввод скаляра, тип: %d", type);

    if (type == 1) {
        LOG_INFO("Ввод вещественного скаляра");

        if (!readDouble("Введите вещественный скаляр: ", real_out)) {
            LOG_ERROR("Ошибка ввода вещественного скаляра");
            return 0;
        }

        *imag_out = 0.0;
        LOG_INFO("Введён вещественный скаляр: %lf", *real_out);

    } else if (type == 2) {
        LOG_INFO("Ввод комплексного скаляра");

        if (!readDouble("Введите действительную часть скаляра: ", real_out)) {
            LOG_ERROR("Ошибка ввода действительной части скаляра");
            return 0;
        }

        if (!readDouble("Введите мнимую часть скаляра: ", imag_out)) {
            LOG_ERROR("Ошибка ввода мнимой части скаляра");
            return 0;
        }

        LOG_INFO("Введён комплексный скаляр: %lf + %lfi", *real_out, *imag_out);

    } else {
        LOG_ERROR("Неизвестный тип скаляра: %d", type);
        return 0;
    }

    return 1;
}