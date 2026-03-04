//
// Created by Krastti on 04.03.2026.
//

#include "menu.h"
#include "../Логирование/logger.h"

void printHeader(void) {
    printSeparator();
    printf("%s\n", MENU_TITLE);
    printf("%s\n", MENU_SUBTITLE);
    printSeparator();
}

void printSeparator(void) {
    printf("================================================================================\n");
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void printError(const char* message) {
    printf("[ОШИБКА] %s\n", message);
    LOG_ERROR(message);
}

void printInfo(const char* message) {
    printf("[ИНФО] %s\n", message);
    LOG_INFO(message);
}

void printMainMenu(void) {
    printf("\n--- МЕНЮ ---\n");
    printf("1. Выбрать тип элементов (Вещественные / Комплексные)\n");
    printf("2. Создать/Заполнить Матрицу A\n");
    printf("3. Создать/Заполнить Матрицу B\n");
    printf("4. Отобразить Матрицу A\n");
    printf("5. Отобразить Матрицу B\n");
    printf("6. Операция: C = A + B (Сложение)\n");
    printf("7. Операция: C = A * B (Умножение матриц)\n");
    printf("8. Операция: C = A * scalar (Умножение на скаляр)\n");
    printf("9. Освободить память\n");
    printf("0. Выход\n");
    printf("---------------------\n");
}

int readInt(const char* prompt, int* value) {
    printf("%s", prompt);
    if (scanf("%d", value) != 1) {
        clearInputBuffer();
        return 0;
    }
    clearInputBuffer();
    return 1;
}

int readDouble(const char* prompt, double* value) {
    printf("%s", prompt);
    if (scanf("%lf", value) != 1) {
        clearInputBuffer();
        return 0;
    }
    clearInputBuffer();
    return 1;
}

int menuSelectElementType(void) {
    int choice;
    printf("\n>>> Выберите тип элементов для коллекции:\n");
    printf("1. Вещественные числа (double)\n");
    printf("2. Комплексные числа (complex: действительная + мнимая)\n");
    printf("Ваш выбор: ");

    if (scanf("%d", &choice) != 1) {
        clearInputBuffer();
        return 0;
    }
    clearInputBuffer();

    if (choice != 1 && choice != 2) {
        return 0;
    }

    return choice;
}

int menuInputScalar(int type, double* real_out, double* imag_out) {
    if (type == 1) {
        // Вещественный скаляр
        if (!readDouble("Введите вещественный скаляр: ", real_out)) {
            return 0;
        }
        *imag_out = 0.0;
    } else if (type == 2) {
        // Комплексный скаляр
        if (!readDouble("Введите действительную часть скаляра: ", real_out)) {
            return 0;
        }
        if (!readDouble("Введите мнимую часть скаляра: ", imag_out)) {
            return 0;
        }
    } else {
        return 0;
    }
    return 1;
}