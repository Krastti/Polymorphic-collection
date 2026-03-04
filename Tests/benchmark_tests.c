//
// Created by Krastti on 04.03.2026.
//
#define EPSILON 1e-6

#include "../Логирование/logger.h"
#include "../LU-разложение/lu_benchmark.h"
#include "../LU-разложение/lu_decomposition.h"

#include <stdio.h>
#include <stdlib.h>


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

void test_lu_benchmark_with_html(void)
{
    const FieldInfo* type = get_double_type_info();

    size_t sizes[] = {10, 25, 50, 100, 200, 400, 800};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    double times_d[7], times_c[7];
    int success_d[7] = {0}, success_c[7] = {0};

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