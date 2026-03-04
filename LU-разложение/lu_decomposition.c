//
// Created by Krastti on 04.03.2026.
//
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "lu_decomposition.h"
#include "../Логирование/logger.h"

int matrix_lu_decompose(const Matrix* A, Matrix** L, Matrix** U)
{
    LOG_DEBUG("Начало LU-разложения: матрица %zux%zu", A ? A->n : 0, A ? A->n : 0);

    if (!A) {
        LOG_ERROR("LU-разложение: матрица A равна NULL");
        return LU_NULL_POINTER;
    }
    if (!L || !U) {
        LOG_ERROR("LU-разложение: выходные указатели L или U равны NULL");
        return LU_NULL_POINTER;
    }
    if (A->n == 0) {
        LOG_ERROR("LU-разложение: размер матрицы равен 0");
        return LU_SIZE_MISMATCH;
    }

    LOG_INFO("Создание матриц L и U размером %zux%zu", A->n, A->n);

    *L = matrix_create(A->n, A->type);
    *U = matrix_create(A->n, A->type);

    if (!*L || !*U) {
        LOG_ERROR("LU-разложение: не удалось выделить память для матриц L или U");
        if (*L) matrix_destroy(*L);
        if (*U) matrix_destroy(*U);
        *L = NULL;
        *U = NULL;
        return LU_MEMORY_ERROR;
    }

    void* one = A->type->one();
    void* zero_elem = A->type->zero();

    if (!one || !zero_elem) {
        LOG_ERROR("LU-разложение: не удалось создать временные элементы (one/zero)");
        if (one) A->type->destroy(one);
        if (zero_elem) A->type->destroy(zero_elem);
        matrix_destroy(*L);
        matrix_destroy(*U);
        *L = NULL;
        *U = NULL;
        return LU_MEMORY_ERROR;
    }

    LOG_DEBUG("Инициализация L (единицы на диагонали) и U (нули)");

    for (size_t i = 0; i < A->n; i++) {
        for (size_t j = 0; j < A->n; j++) {
            if (i == j) {
                memcpy((*L)->data[i * A->n + j], one, A->type->size);
            } else {
                memcpy((*L)->data[i * A->n + j], zero_elem, A->type->size);
            }
            memcpy((*U)->data[i * A->n + j], zero_elem, A->type->size);
        }
    }

    void* sum_temp = A->type->clone(zero_elem);
    void* mul_temp = A->type->clone(zero_elem);
    void* div_temp = A->type->clone(zero_elem);
    void* neg_sum = A->type->clone(zero_elem);

    if (!sum_temp || !mul_temp || !div_temp || !neg_sum) {
        LOG_ERROR("LU-разложение: не удалось выделить память для временных переменных");
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

    LOG_DEBUG("Начало основного цикла разложения");

    for (size_t i = 0; i < A->n; i++) {
        LOG_DEBUG("Обработка строки %zu из %zu", i + 1, A->n);

        for (size_t j = 0; j < A->n; j++) {
            memcpy(sum_temp, zero_elem, A->type->size);

            for (size_t k = 0; k < i; k++) {
                void* l_ik = (*L)->data[i * A->n + k];
                void* u_kj = (*U)->data[k * A->n + j];
                A->type->mul(l_ik, u_kj, mul_temp);
                A->type->sum(sum_temp, mul_temp, sum_temp);
            }

            void* a_ij = A->data[i * A->n + j];

            if (i == 0) {
                memcpy(sum_temp, a_ij, A->type->size);
            } else {
                double neg_one = -1.0;
                A->type->mul_scalar(sum_temp, &neg_one, neg_sum);
                A->type->sum(a_ij, neg_sum, sum_temp);
            }

            if (j >= i) {
                memcpy((*U)->data[i * A->n + j], sum_temp, A->type->size);
                LOG_DEBUG("U[%zu][%zu] вычислен", i, j);
            } else {
                void* u_jj = (*U)->data[j * A->n + j];

                if (A->type->is_zero && A->type->is_zero(u_jj)) {
                    LOG_ERROR("LU-разложение: обнаружен ноль на диагонали U[%zu][%zu] - сингулярная матрица", j, j);
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
                    A->type->divide(sum_temp, u_jj, div_temp);
                    memcpy((*L)->data[i * A->n + j], div_temp, A->type->size);
                    LOG_DEBUG("L[%zu][%zu] вычислен", i, j);
                } else {
                    LOG_ERROR("LU-разложение: операция деления не определена для данного типа");
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
    }

    A->type->destroy(sum_temp);
    A->type->destroy(mul_temp);
    A->type->destroy(div_temp);
    A->type->destroy(neg_sum);
    A->type->destroy(one);
    A->type->destroy(zero_elem);

    LOG_INFO("LU-разложение успешно завершено");
    return LU_SUCCESS;
}

int matrix_lu_verify(const Matrix* A, const Matrix* L, const Matrix* U, double epsilon)
{
    LOG_DEBUG("Начало проверки LU-разложения (epsilon = %f)", epsilon);

    if (!A || !L || !U) {
        LOG_ERROR("LU-проверка: одна из матриц равна NULL");
        return 0;
    }

    Matrix* LU = matrix_multiply(L, U);
    if (!LU) {
        LOG_ERROR("LU-проверка: не удалось вычислить произведение L * U");
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
                LOG_WARN("LU-проверка: несовпадение A[%zu][%zu]", i, j);
            }
        }
    }

    matrix_destroy(LU);

    if (match) {
        LOG_INFO("LU-проверка пройдена: L * U = A");
    } else {
        LOG_ERROR("LU-проверка не пройдена: найдено %zu несовпадений", mismatch_count);
    }

    return match;
}