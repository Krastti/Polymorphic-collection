#include "matrix.h"
#include "logging/logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void* matrix_at_i(const Matrix* matrix, const size_t i) {
    if (!matrix->data) {return NULL;}
    return (char*)matrix->data + i * matrix->type->size;
}

static inline void* matrix_at_ij(const Matrix* matrix, const size_t i, const size_t j) {
    return matrix_at_i(matrix, i * matrix->n + j);
}

Matrix* matrix_create(const size_t n, const FieldInfo* type) {
    if (!type) {
      LOG_ERROR("Передаваемый тип некорректный!");
      return NULL;
    }

    Matrix* matrix = malloc(sizeof(Matrix));
    if (!matrix) {
      LOG_ERROR("Не удалось выделить память под структуру Matrix!");
      return NULL;
    }

    if (n == 0) {
      LOG_ERROR("Размер матрицы должен быть больше нуля!");
      free(matrix);
      return NULL;
    }

    matrix->n = n;
    matrix->type = type;
    matrix->data = calloc(n * n, type->size);

    if (!matrix->data) {
      LOG_ERROR("Не удалось выделить память под данные матрицы!");
      free(matrix);
      return NULL;
    }

    for (size_t i = 0; i < n * n; i++) {
      type->zero(matrix_at_i(matrix, i));
    }

    LOG_INFO("Матрица %zux%zu успешно создана", n, n);
    return matrix;
}

Matrix* matrix_sum(const Matrix* matrix1, const Matrix* matrix2) {
    if (!matrix1 || !matrix2) {
        LOG_ERROR("Одна из передаваемых матриц не существует!");
        return NULL;
    }

    if (matrix1->type != matrix2->type) {
        LOG_ERROR("У матриц разные типы данных!");
        return NULL;
    }

    if (matrix1->n != matrix2->n) {
        LOG_ERROR("Размеры матриц не совпадают! (%zu vs %zu)", matrix1->n, matrix2->n);
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->n, matrix1->type);
    if (!result) {
        LOG_ERROR("Не удалось создать матрицу результата для операции сложения!");
        return NULL;
    }

    for (size_t i = 0; i < result->n * result->n; i++) {
        void* element_matrix1 = matrix_at_i(matrix1, i);
        void* element_matrix2 = matrix_at_i(matrix2, i);
        void* element_result = matrix_at_i(result, i);

        matrix1->type->sum(element_matrix1, element_matrix2, element_result);
    }

    LOG_INFO("Операция сложения матриц %zux%zu выполнена успешно", matrix1->n, matrix1->n);
    return result;
}

Matrix* matrix_multiply(const Matrix* matrix1, const Matrix* matrix2) {
    if (!matrix1 || !matrix2) {
        LOG_ERROR("Одна из передаваемых матриц не существует!");
        return NULL;
    }

    if (matrix1->type != matrix2->type) {
        LOG_ERROR("У матриц разные типы данных!");
        return NULL;
    }

    if (matrix1->n != matrix2->n) {
        LOG_ERROR("Размеры матриц не совпадают! (%zu vs %zu)", matrix1->n, matrix2->n);
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->n, matrix1->type);
    if (!result) {
        LOG_ERROR("Не удалось создать матрицу результата для операции умножения!");
        return NULL;
    }

    void* tmp = malloc(result->type->size);
    if (!tmp) {
        LOG_ERROR("Не удалось выделить память для промежуточного элемента умножения");
        matrix_destroy(result);
        return NULL;
    }

    for (size_t i = 0; i < result->n; i++) {
        for (size_t j = 0; j < result->n; j++) {

            void* product = matrix_at_ij(result, i, j);
            for (size_t k = 0; k < result->n; k++) {
                result->type->mul(matrix_at_ij(matrix1, i, k), matrix_at_ij(matrix2, k, j), tmp);
                result->type->sum(product, tmp, product);
            }
        }
    }

    LOG_INFO("Операция умножения матриц %zux%zu выполнена успешно", matrix1->n, matrix1->n);
    free(tmp);
    return result;
}

Matrix* matrix_multiply_by_scalar(const Matrix* matrix, const void* scalar) {
    if (!matrix || !scalar) {
        LOG_ERROR("Возникла ошибка при умножении на скаляр!");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->n, matrix->type);
    if (!result) {
        LOG_ERROR("Не удалось создать матрицу результата для операции умножения на скаляр!");
        return NULL;
    }

    for (size_t i = 0; i < result->n * result->n; i++) {
        void* element = matrix_at_i(matrix, i);
        void* res = matrix_at_i(result, i);
        matrix->type->mul(element, scalar, res);
    }
    LOG_INFO("Операция умножения матрицы %zux%zu на скаляр выполнена успешно", matrix->n, matrix->n);
    return result;
}

void matrix_print(const Matrix* matrix) {
    if (!matrix) {
        LOG_ERROR("Попытка печати несуществующей матрицы!");
        return;
    }

    if (matrix->n == 0) {
        LOG_INFO("Печать пустой матрицы (0x0)");
        printf("\n");
    } else {
        LOG_INFO("Печать матрицы %zux%zu", matrix->n, matrix->n);
        printf("Матрица %zux%zu:\n", matrix->n, matrix->n);
        for (size_t i = 0; i < matrix->n; i++) {
            printf("| ");
            for (size_t j = 0; j < matrix->n; j++)
            {
                matrix->type->print(matrix_at_ij(matrix, i, j));
                printf(" ");
            }
            printf("|\n");
        }
    }
    printf("\n");
}

void matrix_destroy(Matrix* matrix) {
    if (!matrix) {
        LOG_ERROR("Попытка уничтожить несуществующую матрицу!");
        return;
    }

    LOG_INFO("Матрица %zux%zu успешно уничтожена", matrix->n, matrix->n);
    free(matrix->data);
    free(matrix);
}

void matrix_set(Matrix* matrix, const size_t row, const size_t col, const void* value) {
    if (!matrix || row >= matrix->n || col >= matrix->n) {
        LOG_ERROR("Возникла ошибка при установке значения матрицы! (row=%zu, col=%zu, n=%zu)", row, col, matrix ? matrix->n : 0);
        return;
    }

    void* element = matrix_at_ij(matrix, row, col);
    void* new_element = matrix->type->clone(value);
    if (!new_element) {
        LOG_ERROR("Не удалось клонировать значение при установке [%zu][%zu]", row, col);
        return;
    }

    memcpy(element, new_element, matrix->type->size);
    free(new_element);
    LOG_DEBUG("Установлено значение в матрице [%zu][%zu]", row, col);
}

void* matrix_get(const Matrix* matrix, const size_t row, const size_t col) {
    if (!matrix || row >= matrix->n || col >= matrix->n) {
        LOG_ERROR("Возникла ошибка при получении значения матрицы! (row=%zu, col=%zu, n=%zu)", row, col, matrix ? matrix->n : 0);
        return NULL;
    }
    return matrix_at_ij(matrix, row, col);
}

bool matrix_lu_decomposition(const Matrix* matrix, Matrix** out_L, Matrix** out_U) {
    if (!matrix || !out_L || !out_U) {
        LOG_ERROR("Возникла ошибка при LU-разложении!");
        return false;
    }

    if (matrix->n == 0) {
        LOG_ERROR("Нельзя выполнить LU-разложение матрицы 0x0!");
        return false;
    }

    Matrix* L = matrix_create(matrix->n, matrix->type);
    Matrix* U = matrix_create(matrix->n, matrix->type);
    if (!L || !U) {
        LOG_ERROR("Не удалось создать матрицу L и U для LU-разложения!");
        if (L) matrix_destroy(L);
        if (U) matrix_destroy(U);
        return false;
    }

    void *tmp = malloc(matrix->type->size);
    void *product = malloc(matrix->type->size);
    if (!tmp || !product) {
        LOG_ERROR("Не удалось выделить временную память!");
        free(tmp);
        free(product);
        matrix_destroy(L);
        matrix_destroy(U);
        return false;
    }

    for (size_t k = 0; k < matrix->n; k++) {

        for (size_t j = k; j < matrix->n; j++) {
            matrix->type->zero(product);

            for (size_t m = 0; m < k; m++) {
                matrix->type->mul(matrix_at_ij(L, k, m), matrix_at_ij(U, m, j), tmp);
                matrix->type->sum(product, tmp, product);
            }

            matrix->type->sub(matrix_at_ij(matrix, k, j), product, matrix_at_ij(U, k, j));
        }

        for (size_t i = k + 1; i < matrix->n; i++) {
            matrix->type->zero(product);

            for (size_t m = 0; m < k; m++) {
                matrix->type->mul(matrix_at_ij(L, i, m), matrix_at_ij(U, m, k), tmp);
                matrix->type->sum(product, tmp, product);
            }
            matrix->type->sub(matrix_at_ij(matrix, i, k), product, tmp);
            matrix->type->div(tmp, matrix_at_ij(U, k, k), matrix_at_ij(L, i, k));
        }

        matrix->type->one(matrix_at_ij(L, k, k));
    }

    free(tmp);
    free(product);
    *out_L = L;
    *out_U = U;
    return true;
}