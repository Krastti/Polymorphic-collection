//
// Created by Krastti on 04.03.2026.
//

#ifndef LABORATORYWORK1_LU_BENCHMARK_H
#define LABORATORYWORK1_LU_BENCHMARK_H

#include "../Матрицы/matrix.h"
#include <time.h>

#define LU_DOOLITTLE 0  // L имеет единицы на диагонали
#define LU_CROUT     1  // U имеет единицы на диагонали

typedef struct {
    double time_doolittle;
    double time_crout;
    size_t matrix_size;
    int success_doolittle;
    int success_crout;
} LUBenchmarkResult;

int matrix_lu_decompose_crout(const Matrix* A, Matrix** L, Matrix** U);

int matrix_lu_verify_crout(const Matrix* A, const Matrix* L, const Matrix* U, double epsilon);

LUBenchmarkResult lu_benchmark(const Matrix* A, double epsilon);

void lu_benchmark_print(const LUBenchmarkResult* result);

double get_time_ms(void);

void lu_benchmark_save_html(const char* filename, size_t* sizes, double* times_doolittle, double* times_crout, int* success_d,int* success_c, size_t num_sizes);
#endif //LABORATORYWORK1_LU_BENCHMARK_H
