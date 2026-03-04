//
// Created by Krastti on 01.03.2026.
//
#include "matrix.h"
#include "../Логирование/logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Matrix* matrix_create(size_t n, const FieldInfo* type)
{
    // Проверка на корректность передаваемого типа
    if (!type) {
        LOG_ERROR("Передаваемый тип некорректный!");
        return NULL;
    }

    // Выделение памяти под матрицу
    Matrix* matrix = malloc(sizeof(Matrix));
    if (!matrix) {
        LOG_ERROR("Не удалось выделить память под структуру Matrix!");
        return NULL;
    }

    // Инициализация
    matrix->n = n;
    matrix->type = type;
    matrix->data = calloc(n * n, sizeof(void*));

    if (!matrix->data) {
        LOG_ERROR("Не удалось выделить память под данные матрицы!");
        free(matrix);
        return NULL;
    }

    // Заполнение матрицы нулями
    void* zero = type->zero();
    for (size_t i = 0; i < n * n; i++)
    {
        matrix->data[i] = type->clone(zero);
        if (!matrix->data[i]) {
            LOG_ERROR("Не удалось клонировать элемент матрицы на позиции %zu", i);
            matrix_destroy(matrix);
            type->destroy(zero);
            return NULL;
        }
    }
    type->destroy(zero);

    LOG_INFO("Матрица %zux%zu успешно создана", n, n);
    return matrix;
}

void matrix_destroy (Matrix* matrix)
{
    if (!matrix) {
        LOG_ERROR("Попытка уничтожить несуществующую матрицу!");
        return;
    }

    if (matrix->data) {
        for (size_t i = 0; i < matrix->n * matrix->n; i++)
        {
            matrix->type->destroy(matrix->data[i]);
        }
        free(matrix->data);
    }

    LOG_INFO("Матрица %zux%zu успешно уничтожена", matrix->n, matrix->n);
    free(matrix);
}

Matrix* matrix_sum(const Matrix* matrix1, const Matrix* matrix2)
{
    // Проверка на существование
    if (!matrix1 || !matrix2) {
        LOG_ERROR("Одна из передаваемых матриц не существует!");
        return NULL;
    }

    // Проверка на полиморфизм
    if (matrix1->type != matrix2->type) {
        LOG_ERROR("У матриц разные типы данных!");
        return NULL;
    }

    // Проверка размерности матриц
    if (matrix1->n != matrix2->n) {
        LOG_ERROR("Размеры матриц не совпадают! (%zu vs %zu)", matrix1->n, matrix2->n);
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->n, matrix1->type);
    if (!result) {
        LOG_ERROR("Не удалось создать матрицу результата для операции сложения!");
        return NULL;
    }

    void* zero = matrix1->type->zero();

    for (size_t i = 0; i < matrix1->n; i++) {
        for (size_t j = 0; j < matrix1->n; j++) {
            void* element_a = matrix1->data[i * matrix1->n + j];
            void* element_b = matrix2->data[i * matrix2->n + j];

            matrix1->type->sum(element_a, element_b, zero);

            memcpy(result->data[i * matrix1->n + j], zero, matrix1->type->size);
        }
    }
    matrix1->type->destroy(zero);

    LOG_INFO("Операция сложения матриц %zux%zu выполнена успешно", matrix1->n, matrix1->n);
    return result;
}

Matrix* matrix_multiply(const Matrix* matrix1, const Matrix* matrix2)
{
    // Проверка на существование
    if (!matrix1 || !matrix2) {
        LOG_ERROR("Одна из передаваемых матриц не существует!");
        return NULL;
    }

    // Проверка на полиморфизм
    if (matrix1->type != matrix2->type) {
        LOG_ERROR("У матриц разные типы данных!");
        return NULL;
    }

    // Проверка размерности матриц
    if (matrix1->n != matrix2->n) {
        LOG_ERROR("Размеры матриц не совпадают! (%zu vs %zu)", matrix1->n, matrix2->n);
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->n, matrix1->type);
    if (!result) {
        LOG_ERROR("Не удалось создать матрицу результата для операции умножения!");
        return NULL;
    }

    void* product = matrix1->type->zero();
    void* zero = matrix1->type->zero();

    for (size_t i = 0; i < matrix1->n; i++){
        for (size_t j = 0; j < matrix1->n; j++)
        {
            memcpy(result->data[i * matrix1->n + j], zero, matrix1->type->size);

            for (size_t k = 0; k < matrix1->n; k++)
            {
                void* matrix1_ik = matrix1->data[i * matrix1->n + k];
                void* matrix2_kj = matrix2->data[k * matrix1->n + j];

                matrix1->type->mul(matrix1_ik, matrix2_kj, product);

                matrix1->type->sum(result->data[i * matrix1->n + j], product, result->data[i * matrix1->n + j]);
            }
        }
    }

    matrix1->type->destroy(product);
    matrix1->type->destroy(zero);

    LOG_INFO("Операция умножения матриц %zux%zu выполнена успешно", matrix1->n, matrix1->n);
    return result;
}

void matrix_scalar_multiply(Matrix* matrix, const void* scalar)
{
    if (!matrix || !scalar) {
        LOG_ERROR("Возникла ошибка при умножении на скаляр!");
        return;
    }

    for (size_t i = 0; i < matrix->n * matrix->n; i++)
    {
        matrix->type->mul_scalar(matrix->data[i], scalar, matrix->data[i]);
    }

    LOG_INFO("Операция умножения матрицы %zux%zu на скаляр выполнена успешно", matrix->n, matrix->n);
}

void matrix_print(const Matrix* matrix)
{
    if (!matrix) {
        LOG_ERROR("Попытка печати несуществующей матрицы!");
        return;
    }

    if (matrix->n == 0) {
        LOG_INFO("Печать пустой матрицы (0x0)");
        printf("\n");
    }

    else if (matrix->n == 1) {
        for (size_t i = 0; i < matrix->n; i++) {
            matrix->type->print(matrix->data[i * matrix->n]);
        }
    }

    else {
        LOG_INFO("Печать матрицы %zux%zu", matrix->n, matrix->n);
        printf("Матрица %zux%zu:\n", matrix->n, matrix->n);
        for (size_t i = 0; i < matrix->n; i++)
        {
            printf("| ");
            for (size_t j = 0; j < matrix->n; j++)
            {
                matrix->type->print(matrix->data[i * matrix->n + j]);
                printf(" ");
            }
            printf("|\n");
        }
    }
    printf("\n");
}

void matrix_set(Matrix* matrix, size_t row, size_t col, const void* value)
{
    if (!matrix || row >= matrix->n || col >= matrix->n)
    {
        LOG_ERROR("Возникла ошибка при установке значения матрицы! (row=%zu, col=%zu, n=%zu)", row, col, matrix ? matrix->n : 0);
        return;
    }
    void* elem = matrix->data[row * matrix->n + col];
    memcpy(elem, value, matrix->type->size);

    LOG_DEBUG("Установлено значение в матрице [%zu][%zu]", row, col);
}

Complex* make_complex(double real, double imag)
{
    Complex* complex = malloc(sizeof(Complex));
    if (complex) {
        complex->real = real;
        complex->imag = imag;
        LOG_DEBUG("Создано комплексное число: %.2f + %.2fi", real, imag);
    } else {
        LOG_ERROR("Не удалось выделить память под комплексное число!");
    }

    return complex;
}

void fill_complex_matrix(Matrix* matrix, double values[][2])
{
    if (!matrix) {
        LOG_ERROR("Матрица не существует!");
        return;
    }

    size_t n = matrix->n;

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            int idx = i * n + j;

            Complex* elem = make_complex(values[idx][0], values[idx][1]);
            if (elem)
            {
                matrix_set(matrix, i, j, elem);
                free(elem);
            }
        }
    }
    LOG_INFO("Матрица %zux%zu заполнена комплексными значениями", n, n);
}

void fill_double_matrix(Matrix* matrix, const double values[])
{
    if (!matrix) {
        LOG_ERROR("Матрица не существует!");
        return;
    }

    size_t n = matrix->n;

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++)
        {
            int idx = i * n + j;
            double elem = values[idx];
            matrix_set(matrix, i, j, &elem);
        }
    }
    LOG_INFO("Матрица %zux%zu заполнена вещественными значениями", n, n);
}