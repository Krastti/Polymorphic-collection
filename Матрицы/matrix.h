//
// Created by Krastti on 01.03.2026.
//

#ifndef LABORATORYWORK1_DOUBLE_MATRIX_H
#define LABORATORYWORK1_DOUBLE_MATRIX_H

#include "../Типы данных/double_types.h"
#include "../Типы данных/complex_types.h"

typedef struct
{
    size_t n;
    void** data; // Убрать две звездочки
    const FieldInfo* type;
} Matrix;

Matrix* matrix_create(size_t n, const FieldInfo* type);
void matrix_destroy(Matrix* matrix);

Matrix* matrix_sum(const Matrix* matrix1, const Matrix* matrix2);
Matrix* matrix_multiply(const Matrix* matrix1, const Matrix* matrix2);
void matrix_scalar_multiply(Matrix* matrix, const void* scalar);

void matrix_print(const Matrix* matrix);
void matrix_set(Matrix* matrix, size_t row, size_t col, const void* value);

void fill_double_matrix(Matrix* matrix, const double values[]);
void fill_complex_matrix(Matrix* matrix, double values[][2]);
Complex* make_complex(double real, double imag);

#endif //LABORATORYWORK1_DOUBLE_MATRIX_H