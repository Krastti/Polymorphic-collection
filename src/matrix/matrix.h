#ifndef LABORATORYWORK1_DOUBLE_MATRIX_H
#define LABORATORYWORK1_DOUBLE_MATRIX_H

#include "../types_info/double_type.h"

typedef struct
{
    size_t n;
    const FieldInfo* type;
    void *data;
} Matrix;

Matrix* matrix_create(const size_t n, const FieldInfo* type);
Matrix* matrix_sum(const Matrix* matrix1, const Matrix* matrix2);
Matrix* matrix_multiply(const Matrix* matrix1, const Matrix* matrix2);
Matrix* matrix_multiply_by_scalar(const Matrix* matrix, const void* scalar);

void matrix_print(const Matrix* matrix);
void matrix_destroy(Matrix* matrix);
void matrix_set(Matrix* matrix, size_t row, size_t col, const void* value);

#endif //LABORATORYWORK1_DOUBLE_MATRIX_H