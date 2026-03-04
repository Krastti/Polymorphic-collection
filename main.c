#include "Матрицы/matrix.h"
#include "Тесты/tests.h"
#include "Логирование/logger.h"
#include "LU-разложение/lu_decomposition.h"
#include  "LU-разложение/lu_benchmark.h"
#include "Меню/menu.h"

#define EPSILON 1e-6

#include <windows.h>
#include <stdio.h>

void print_menu(void)
{
    printf("\n===========================================\n");
    printf("        МЕНЮ ПРОГРАММЫ (Вариант 12)       \n");
    printf("===========================================\n");
    printf("1. Тест LU-разложения (вещественные числа)\n");
    printf("2. Тест LU-разложения (комплексные числа) \n");
    printf("3. Бенчмарк\n");
    printf("4. Тест матричного сложения               \n");
    printf("5. Тест матричного умножения              \n");
    printf("6. Тест умножения на скаляр               \n");
    printf("7. Unit-тесты\n");
    printf("8. График бенчмарков\n");
    printf("0. Выход                                  \n");
    printf("===========================================\n");
    printf("Выберите пункт меню: ");
}

void test_lu_benchmark(void)
{
    LOG_INFO("=== Бенчмарк LU-разложения ===");

    const FieldInfo* type = get_double_type_info();

    // Тест на разных размерах
    size_t sizes[] = {10, 50, 100, 200};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("\n=== Бенчмарк LU-разложения на разных размерах ===\n");
    printf("%-10s | %-15s | %-15s | %-10s\n", "Размер", "Doolittle (мс)", "Crout (мс)", "Быстрее");
    printf("%-10s-+-%-15s-+-%-15s-+-%-10s\n",
           "----------", "---------------", "---------------", "----------");

    for (size_t i = 0; i < num_sizes; i++) {
        Matrix* A = matrix_create(sizes[i], type);

        // Заполняем случайными значениями
        double* values = malloc(sizes[i] * sizes[i] * sizeof(double));
        for (size_t j = 0; j < sizes[i] * sizes[i]; j++) {
            values[j] = (double)(rand() % 100) / 10.0 + 1.0; // 1.0 - 10.0
        }
        fill_double_matrix(A, values);
        free(values);

        LUBenchmarkResult result = lu_benchmark(A, EPSILON);

        const char* faster = "";
        if (result.success_doolittle && result.success_crout) {
            if (result.time_doolittle < result.time_crout) {
                faster = "Doolittle";
            } else {
                faster = "Crout";
            }
        } else {
            faster = "Ошибка";
        }

        printf("%-10zu | %-15.4f | %-15.4f | %-10s\n",
               sizes[i], result.time_doolittle, result.time_crout, faster);

        matrix_destroy(A);
    }

    LOG_INFO("Бенчмарк завершен");
}

void test_lu_double(void)
{
    LOG_INFO("=== Тест LU-разложения (вещественные числа) ===");
    printf("\n=== Тест LU-разложения (вещественные числа) ===\n");

    const FieldInfo* type = get_double_type_info();
    Matrix* A = matrix_create(3, type);

    if (!A) {
        LOG_ERROR("Не удалось создать матрицу A");
        printf("Ошибка: не удалось создать матрицу\n");
        return;
    }

    double values[] = {2.0, 1.0, 1.0, 4.0, 3.0, 3.0, 6.0, 5.0, 8.0};
    fill_double_matrix(A, values);

    printf("\nИсходная матрица A:\n");
    matrix_print(A);

    Matrix *L = NULL, *U = NULL;
    int err = matrix_lu_decompose(A, &L, &U);

    if (err == LU_SUCCESS) {
        printf("Матрица L:\n");
        matrix_print(L);

        printf("Матрица U:\n");
        matrix_print(U);

        if (matrix_lu_verify(A, L, U, EPSILON)) {
            LOG_INFO("LU-разложение проверено: L * U = A");
            printf("✓ LU-разложение проверено: L * U = A\n");
        } else {
            LOG_ERROR("LU-разложение не прошло проверку");
            printf("✗ Ошибка проверки LU-разложения\n");
        }
    } else {
        LOG_ERROR("Ошибка LU-разложения: код %d", err);
        printf("✗ Ошибка LU-разложения: код %d\n", err);
    }

    matrix_destroy(A);
    matrix_destroy(L);
    matrix_destroy(U);

    LOG_INFO("Тест LU-разложения (double) завершен");
}

void test_lu_complex(void)
{
    LOG_INFO("=== Тест LU-разложения (комплексные числа) ===");
    printf("\n=== Тест LU-разложения (комплексные числа) ===\n");

    const FieldInfo* type = get_complex_type_info();
    Matrix* A = matrix_create(3, type);

    if (!A) {
        LOG_ERROR("Не удалось создать матрицу A");
        printf("Ошибка: не удалось создать матрицу\n");
        return;
    }

    double values[][2] = {
        {2.0, 0.0}, {1.0, 0.0}, {1.0, 0.0},
        {4.0, 0.0}, {3.0, 0.0}, {3.0, 0.0},
        {6.0, 0.0}, {5.0, 0.0}, {8.0, 0.0}
    };
    fill_complex_matrix(A, values);

    printf("\nИсходная матрица A:\n");
    matrix_print(A);

    Matrix *L = NULL, *U = NULL;
    int err = matrix_lu_decompose(A, &L, &U);

    if (err == LU_SUCCESS) {
        printf("Матрица L:\n");
        matrix_print(L);

        printf("Матрица U:\n");
        matrix_print(U);

        if (matrix_lu_verify(A, L, U, EPSILON)) {
            LOG_INFO("LU-разложение проверено: L * U = A");
            printf("✓ LU-разложение проверено: L * U = A\n");
        } else {
            LOG_ERROR("LU-разложение не прошло проверку");
            printf("✗ Ошибка проверки LU-разложения\n");
        }
    } else {
        LOG_ERROR("Ошибка LU-разложения: код %d", err);
        printf("✗ Ошибка LU-разложения: код %d\n", err);
    }

    matrix_destroy(A);
    matrix_destroy(L);
    matrix_destroy(U);

    LOG_INFO("Тест LU-разложения (complex) завершен");
}

void test_matrix_sum(void)
{
    LOG_INFO("=== Тест матричного сложения ===");
    printf("\n=== Тест матричного сложения ===\n");

    const FieldInfo* type = get_double_type_info();
    Matrix* A = matrix_create(2, type);
    Matrix* B = matrix_create(2, type);

    if (!A || !B) {
        LOG_ERROR("Не удалось создать матрицы");
        printf("Ошибка: не удалось создать матрицы\n");
        if (A) matrix_destroy(A);
        if (B) matrix_destroy(B);
        return;
    }

    double values_a[] = {1.0, 2.0, 3.0, 4.0};
    double values_b[] = {5.0, 6.0, 7.0, 8.0};

    fill_double_matrix(A, values_a);
    fill_double_matrix(B, values_b);

    printf("Матрица A:\n");
    matrix_print(A);

    printf("Матрица B:\n");
    matrix_print(B);

    Matrix* C = matrix_sum(A, B);

    if (C) {
        printf("Результат A + B:\n");
        matrix_print(C);
        LOG_INFO("Матричное сложение выполнено успешно");
        printf("✓ Матричное сложение выполнено\n");
        matrix_destroy(C);
    } else {
        LOG_ERROR("Ошибка матричного сложения");
        printf("✗ Ошибка матричного сложения\n");
    }

    matrix_destroy(A);
    matrix_destroy(B);

    LOG_INFO("Тест матричного сложения завершен");
}

void test_matrix_multiply(void)
{
    LOG_INFO("=== Тест матричного умножения ===");
    printf("\n=== Тест матричного умножения ===\n");

    const FieldInfo* type = get_double_type_info();
    Matrix* A = matrix_create(2, type);
    Matrix* B = matrix_create(2, type);

    if (!A || !B) {
        LOG_ERROR("Не удалось создать матрицы");
        printf("Ошибка: не удалось создать матрицы\n");
        if (A) matrix_destroy(A);
        if (B) matrix_destroy(B);
        return;
    }

    double values_a[] = {1.0, 2.0, 3.0, 4.0};
    double values_b[] = {5.0, 6.0, 7.0, 8.0};

    fill_double_matrix(A, values_a);
    fill_double_matrix(B, values_b);

    printf("Матрица A:\n");
    matrix_print(A);

    printf("Матрица B:\n");
    matrix_print(B);

    Matrix* C = matrix_multiply(A, B);

    if (C) {
        printf("Результат A * B:\n");
        matrix_print(C);
        LOG_INFO("Матричное умножение выполнено успешно");
        printf("✓ Матричное умножение выполнено\n");
        matrix_destroy(C);
    } else {
        LOG_ERROR("Ошибка матричного умножения");
        printf("✗ Ошибка матричного умножения\n");
    }

    matrix_destroy(A);
    matrix_destroy(B);

    LOG_INFO("Тест матричного умножения завершен");
}

void test_scalar_multiply(void)
{
    LOG_INFO("=== Тест умножения на скаляр ===");
    printf("\n=== Тест умножения на скаляр ===\n");

    const FieldInfo* type = get_double_type_info();
    Matrix* A = matrix_create(2, type);

    if (!A) {
        LOG_ERROR("Не удалось создать матрицу");
        printf("Ошибка: не удалось создать матрицу\n");
        return;
    }

    double values[] = {1.0, 2.0, 3.0, 4.0};
    fill_double_matrix(A, values);

    printf("Матрица A:\n");
    matrix_print(A);

    double scalar = 2.0;
    printf("Скаляр: %.2f\n", scalar);

    matrix_scalar_multiply(A, &scalar);

    printf("Результат A * 2:\n");
    matrix_print(A);

    LOG_INFO("Умножение на скаляр выполнено успешно");
    printf("✓ Умножение на скаляр выполнено\n");

    matrix_destroy(A);

    LOG_INFO("Тест умножения на скаляр завершен");
}

void test_lu_benchmark_with_html(void)
{
    const FieldInfo* type = get_double_type_info();

    size_t sizes[] = {10, 25, 50, 100, 200, 400};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    double times_d[6], times_c[6];
    int success_d[6] = {0}, success_c[6] = {0};

    printf("\n=== Запуск бенчмарка LU-разложения ===\n");

    for (size_t idx = 0; idx < num_sizes; idx++) {
        size_t n = sizes[idx];
        printf("Тестирование %zux%zu... ", n, n);

        Matrix* A = matrix_create(n, type);
        double* values = malloc(n * n * sizeof(double));

        for (size_t i = 0; i < n * n; i++) {
            values[i] = (double)(rand() % 90) + 5.0;
        }
        fill_double_matrix(A, values);
        free(values);

        // Doolittle
        Matrix *L_d = NULL, *U_d = NULL;
        clock_t start = clock();
        int err_d = matrix_lu_decompose(A, &L_d, &U_d);
        clock_t end = clock();

        times_d[idx] = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
        success_d[idx] = (err_d == LU_SUCCESS) && matrix_lu_verify(A, L_d, U_d, 1e-6);
        matrix_destroy(L_d);
        matrix_destroy(U_d);

        // Crout
        Matrix *L_c = NULL, *U_c = NULL;
        start = clock();
        int err_c = matrix_lu_decompose_crout(A, &L_c, &U_c);
        end = clock();

        times_c[idx] = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
        success_c[idx] = (err_c == LU_SUCCESS) && matrix_lu_verify_crout(A, L_c, U_c, 1e-6);
        matrix_destroy(L_c);
        matrix_destroy(U_c);
        matrix_destroy(A);

        printf("D: %.3f мс, C: %.3f мс\n", times_d[idx], times_c[idx]);
    }

    // Сохранение в HTML
    lu_benchmark_save_html("benchmark_report.html", sizes, times_d, times_c,
                           success_d, success_c, num_sizes);

    // Автоматическое открытие в браузере (Windows)
    #ifdef _WIN32
    system("start benchmark_report.html");
    #endif

    printf("\n✓ Бенчмарк завершён!\n");
}

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    if (!logger_init("matrix.log", LOG_ERROR)) {
        fprintf(stderr, "Не удалось инициализировать логер.\n");
        return 1;
    }

    printf("============================================\n");
    printf("   Лабораторная работа №1 (Вариант 12)     \n");
    printf("   Квадратная матрица                      \n");
    printf("   Вещественные и комплексные числа        \n");
    printf("============================================\n");

    int choice;
    do {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        }

        switch (choice) {
        case 1:
            test_lu_double();
            break;
        case 2:
            test_lu_complex();
            break;
        case 3:
            test_lu_benchmark();
            break;
        case 4:
            test_matrix_sum();
            break;
        case 5:
            test_matrix_multiply();
            break;
        case 6:
            test_scalar_multiply();
            break;
        case 7:
            unit_tests();
            break;
        case 8:
            test_lu_benchmark_with_html();
            break;
        case 0:
            LOG_INFO("Завершение работы программы");
            printf("\nЗавершение работы программы...\n");
            break;
        default:
            LOG_WARN("Неверный выбор меню: %d", choice);
            printf("Неверный выбор. Попробуйте снова.\n");
            break;
        }
    } while (choice != 0);

    LOG_INFO("Программа завершила работу");
    logger_close();
    return 0;
}