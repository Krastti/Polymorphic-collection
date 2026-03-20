#ifndef LABORATORYWORK1_API_H
#define LABORATORYWORK1_API_H

#pragma once

#ifdef _WIN32
  #ifdef API_EXPORTS
    #define API_EXPORT __declspec(dllexport)
  #else
    #define API_EXPORT __declspec(dllimport)
  #endif
#else
  #define API_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define API_TYPE_DOUBLE 0
#define API_TYPE_COMPLEX 1

API_EXPORT int api_matrix_type(void* matrix);
API_EXPORT size_t api_matrix_size(void* matrix);

API_EXPORT void* api_create_matrix(size_t n, int type_id);
API_EXPORT void api_destroy_matrix(void* matrix);

API_EXPORT void* api_matrix_sum(void* matrix1, void* matrix2);
API_EXPORT void* api_matrix_multiply(void* matrix1, void* matrix2);
API_EXPORT void* api_matrix_multiply_by_scalar(void* matrix, double scalar_real, double scalar_imag);
API_EXPORT int api_matrix_lu_decomposition(void* matrix, void** matrixL, void** matrixU);

API_EXPORT void api_matrix_set(void* matrix, size_t row, size_t col, double real, double imag);
API_EXPORT void api_matrix_get(void* matrix, size_t row, size_t col, double*real, double*imag);

#ifdef __cplusplus
}
#endif

#endif // LABORATORYWORK1_API_H
