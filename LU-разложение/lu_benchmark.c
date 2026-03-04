//
// Created by Krastti on 04.03.2026.
//

#include "lu_decomposition.h"
#include "lu_benchmark.h"
#include "../Логирование/logger.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

double get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

int matrix_lu_decompose_crout(const Matrix* A, Matrix** L, Matrix** U)
{
    LOG_DEBUG("Начало Crout LU-разложения: матрица %zux%zu", A ? A->n : 0, A ? A->n : 0);

    if (!A) {
        LOG_ERROR("Crout LU: матрица A равна NULL");
        return LU_NULL_POINTER;
    }
    if (!L || !U) {
        LOG_ERROR("Crout LU: выходные указатели L или U равны NULL");
        return LU_NULL_POINTER;
    }
    if (A->n == 0) {
        LOG_ERROR("Crout LU: размер матрицы равен 0");
        return LU_SIZE_MISMATCH;
    }

    LOG_INFO("Создание матриц L и U (Crout) размером %zux%zu", A->n, A->n);

    *L = matrix_create(A->n, A->type);
    *U = matrix_create(A->n, A->type);

    if (!*L || !*U) {
        LOG_ERROR("Crout LU: не удалось выделить память для матриц L или U");
        if (*L) matrix_destroy(*L);
        if (*U) matrix_destroy(*U);
        *L = NULL;
        *U = NULL;
        return LU_MEMORY_ERROR;
    }

    void* one = A->type->one();
    void* zero_elem = A->type->zero();

    if (!one || !zero_elem) {
        LOG_ERROR("Crout LU: не удалось создать временные элементы");
        if (one) A->type->destroy(one);
        if (zero_elem) A->type->destroy(zero_elem);
        matrix_destroy(*L);
        matrix_destroy(*U);
        *L = NULL;
        *U = NULL;
        return LU_MEMORY_ERROR;
    }

    // Инициализация: U имеет единицы на диагонали
    for (size_t i = 0; i < A->n; i++) {
        for (size_t j = 0; j < A->n; j++) {
            memcpy((*L)->data[i * A->n + j], zero_elem, A->type->size);
            if (i == j) {
                memcpy((*U)->data[i * A->n + j], one, A->type->size);
            } else {
                memcpy((*U)->data[i * A->n + j], zero_elem, A->type->size);
            }
        }
    }

    void* sum_temp = A->type->clone(zero_elem);
    void* mul_temp = A->type->clone(zero_elem);
    void* div_temp = A->type->clone(zero_elem);
    void* neg_sum = A->type->clone(zero_elem);

    if (!sum_temp || !mul_temp || !div_temp || !neg_sum) {
        LOG_ERROR("Crout LU: не удалось выделить память для временных переменных");
        A->type->destroy(sum_temp);
        A->type->destroy(mul_temp);
        A->type->destroy(div_temp);
        A->type->destroy(neg_sum);
        A->type->destroy(one);
        A->type->destroy(zero_elem);
        matrix_destroy(*L);
        matrix_destroy(*U);
        *L = NULL;
        *U = NULL;
        return LU_MEMORY_ERROR;
    }

    LOG_DEBUG("Начало основного цикла Crout разложения");

    // Алгоритм Crout
    for (size_t j = 0; j < A->n; j++) {
        LOG_DEBUG("Обработка столбца %zu из %zu", j + 1, A->n);

        // Вычисление L[i][j] для i >= j
        for (size_t i = j; i < A->n; i++) {
            memcpy(sum_temp, zero_elem, A->type->size);

            for (size_t k = 0; k < j; k++) {
                void* l_ik = (*L)->data[i * A->n + k];
                void* u_kj = (*U)->data[k * A->n + j];
                A->type->mul(l_ik, u_kj, mul_temp);
                A->type->sum(sum_temp, mul_temp, sum_temp);
            }

            void* a_ij = A->data[i * A->n + j];

            if (j == 0) {
                memcpy(sum_temp, a_ij, A->type->size);
            } else {
                double neg_one = -1.0;
                A->type->mul_scalar(sum_temp, &neg_one, neg_sum);
                A->type->sum(a_ij, neg_sum, sum_temp);
            }

            memcpy((*L)->data[i * A->n + j], sum_temp, A->type->size);
            LOG_DEBUG("L[%zu][%zu] вычислен", i, j);
        }

        // Вычисление U[j][k] для k > j
        for (size_t k = j + 1; k < A->n; k++) {
            memcpy(sum_temp, zero_elem, A->type->size);

            for (size_t i = 0; i < j; i++) {
                void* l_ji = (*L)->data[j * A->n + i];
                void* u_ik = (*U)->data[i * A->n + k];
                A->type->mul(l_ji, u_ik, mul_temp);
                A->type->sum(sum_temp, mul_temp, sum_temp);
            }

            void* a_jk = A->data[j * A->n + k];
            void* l_jj = (*L)->data[j * A->n + j];

            if (j == 0) {
                memcpy(sum_temp, a_jk, A->type->size);
            } else {
                double neg_one = -1.0;
                A->type->mul_scalar(sum_temp, &neg_one, neg_sum);
                A->type->sum(a_jk, neg_sum, sum_temp);
            }

            if (A->type->is_zero && A->type->is_zero(l_jj)) {
                LOG_ERROR("Crout LU: ноль на диагонали L[%zu][%zu]", j, j);
                A->type->destroy(sum_temp);
                A->type->destroy(mul_temp);
                A->type->destroy(div_temp);
                A->type->destroy(neg_sum);
                A->type->destroy(one);
                A->type->destroy(zero_elem);
                matrix_destroy(*L);
                matrix_destroy(*U);
                *L = NULL;
                *U = NULL;
                return LU_SINGULAR_MATRIX;
            }

            if (A->type->divide) {
                A->type->divide(sum_temp, l_jj, div_temp);
                memcpy((*U)->data[j * A->n + k], div_temp, A->type->size);
                LOG_DEBUG("U[%zu][%zu] вычислен", j, k);
            } else {
                LOG_ERROR("Crout LU: операция деления не определена");
                A->type->destroy(sum_temp);
                A->type->destroy(mul_temp);
                A->type->destroy(div_temp);
                A->type->destroy(neg_sum);
                A->type->destroy(one);
                A->type->destroy(zero_elem);
                matrix_destroy(*L);
                matrix_destroy(*U);
                *L = NULL;
                *U = NULL;
                return LU_NULL_POINTER;
            }
        }
    }

    A->type->destroy(sum_temp);
    A->type->destroy(mul_temp);
    A->type->destroy(div_temp);
    A->type->destroy(neg_sum);
    A->type->destroy(one);
    A->type->destroy(zero_elem);

    LOG_INFO("Crout LU-разложение успешно завершено");
    return LU_SUCCESS;
}

int matrix_lu_verify_crout(const Matrix* A, const Matrix* L, const Matrix* U, double epsilon)
{
    LOG_DEBUG("Начало проверки Crout LU-разложения (epsilon = %f)", epsilon);

    if (!A || !L || !U) {
        LOG_ERROR("Crout LU-проверка: одна из матриц равна NULL");
        return 0;
    }

    Matrix* LU = matrix_multiply(L, U);
    if (!LU) {
        LOG_ERROR("Crout LU-проверка: не удалось вычислить L * U");
        return 0;
    }

    int match = 1;
    size_t mismatch_count = 0;

    for (size_t i = 0; i < A->n && match; i++) {
        for (size_t j = 0; j < A->n && match; j++) {
            void* a_val = A->data[i * A->n + j];
            void* lu_val = LU->data[i * A->n + j];

            if (A->type->equals && !A->type->equals(a_val, lu_val, epsilon)) {
                match = 0;
                mismatch_count++;
                LOG_WARN("Crout LU-проверка: несовпадение A[%zu][%zu]", i, j);
            }
        }
    }

    matrix_destroy(LU);

    if (match) {
        LOG_INFO("Crout LU-проверка пройдена: L * U = A");
    } else {
        LOG_ERROR("Crout LU-проверка не пройдена: найдено %zu несовпадений", mismatch_count);
    }

    return match;
}

LUBenchmarkResult lu_benchmark(const Matrix* A, double epsilon)
{
    LUBenchmarkResult result = {0};
    result.matrix_size = A ? A->n : 0;

    LOG_INFO("=== Начало бенчмарка LU-разложения ===");
    if (A != NULL) {
        printf("\n=== Бенчмарк LU-разложения (матрица %zux%zu) ===\n", A->n, A->n);
    }

    // Тест Doolittle
    printf("\n1. Тест Doolittle (L с единицами на диагонали):\n");
    double start_doolittle = get_time_ms();

    Matrix *L_d = NULL, *U_d = NULL;
    int err_d = matrix_lu_decompose(A, &L_d, &U_d);

    double end_doolittle = get_time_ms();
    result.time_doolittle = end_doolittle - start_doolittle;
    result.success_doolittle = (err_d == LU_SUCCESS);

    if (result.success_doolittle) {
        printf("    Успешно за %.4f мс\n", result.time_doolittle);

        if (matrix_lu_verify(A, L_d, U_d, epsilon)) {
            printf("    Проверка пройдена: L * U = A\n");
        } else {
            printf("    Проверка не пройдена\n");
            result.success_doolittle = 0;
        }
    } else {
        printf("    Ошибка (код %d)\n", err_d);
    }

    // Тест Crout
    printf("\n2. Тест Crout (U с единицами на диагонали):\n");
    double start_crout = get_time_ms();

    Matrix *L_c = NULL, *U_c = NULL;
    int err_c = matrix_lu_decompose_crout(A, &L_c, &U_c);

    double end_crout = get_time_ms();
    result.time_crout = end_crout - start_crout;
    result.success_crout = (err_c == LU_SUCCESS);

    if (result.success_crout) {
        printf("    Успешно за %.4f мс\n", result.time_crout);

        if (matrix_lu_verify_crout(A, L_c, U_c, epsilon)) {
            printf("    Проверка пройдена: L * U = A\n");
        } else {
            printf("    Проверка не пройдена\n");
            result.success_crout = 0;
        }
    } else {
        printf("    Ошибка (код %d)\n", err_c);
    }

    // Сравнение результатов
    printf("\n=== Результаты сравнения ===\n");
    if (result.success_doolittle && result.success_crout) {
        double speedup = result.time_doolittle / result.time_crout;
        printf("Doolittle: %.4f мс\n", result.time_doolittle);
        printf("Crout:     %.4f мс\n", result.time_crout);

        if (speedup > 1.0) {
            printf("Crout быстрее в %.2f раз\n", speedup);
        } else if (speedup < 1.0) {
            printf("Doolittle быстрее в %.2f раз\n", 1.0 / speedup);
        } else {
            printf("Оба метода показали одинаковое время\n");
        }
    }

    // Очистка памяти
    matrix_destroy(L_d);
    matrix_destroy(U_d);
    matrix_destroy(L_c);
    matrix_destroy(U_c);

    LOG_INFO("Бенчмарк завершен");
    return result;
}

void lu_benchmark_print(const LUBenchmarkResult* result)
{
    if (!result) return;

    printf("\n=== Сводка бенчмарка ===\n");
    printf("Размер матрицы: %zux%zu\n", result->matrix_size, result->matrix_size);
    printf("Doolittle: %s (%.4f мс)\n",
           result->success_doolittle ? " Успешно" : " Ошибка",
           result->time_doolittle);
    printf("Crout:     %s (%.4f мс)\n",
           result->success_crout ? " Успешно" : " Ошибка",
           result->time_crout);
}