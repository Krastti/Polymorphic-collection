//
// Created by Krastti on 04.03.2026.
//

#ifndef LABORATORYWORK1_LU_DECOMPOSITION_H
#define LABORATORYWORK1_LU_DECOMPOSITION_H

#include "../Матрицы/matrix.h"

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

// b - вектор правой части
// x - выходной вектор решения
int matrix_lu_solve(const Matrix* A, const Matrix* b, Matrix** x);

#endif //LABORATORYWORK1_LU_DECOMPOSITION_H