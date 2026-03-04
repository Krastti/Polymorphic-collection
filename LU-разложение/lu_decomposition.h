//
// Created by Krastti on 04.03.2026.
//

#ifndef LABORATORYWORK1_LU_DECOMPOSITION_H
#define LABORATORYWORK1_LU_DECOMPOSITION_H

#include "../Matrix/matrix.h"

#define LU_SUCCESS          0
#define LU_NULL_POINTER    -1
#define LU_SIZE_MISMATCH   -2
#define LU_SINGULAR_MATRIX -3
#define LU_MEMORY_ERROR    -4

// A - Исходная матрица
// L - Нижняя треугольная матрица
// U - Верхняя треугольная матрица
int matrix_lu_decompose(const Matrix* A, Matrix** L, Matrix** U);

// epsilon - Допустимая погрешность
int matrix_lu_verify(const Matrix* A, const Matrix* L, const Matrix* U, double epsilon);
#endif //LABORATORYWORK1_LU_DECOMPOSITION_H