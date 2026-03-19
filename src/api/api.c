#include "api.h"
#include "../matrix/matrix.h"
#include "../types_info/double_type.h"
#include "../types_info/complex_type.h"
#include <string.h>

static const FieldInfo* get_type_by_id(int type_id) {
  if (type_id == 1) return get_complex_type_info();
  return get_double_type_info();
}

API_EXPORT int api_matrix_type(void* matrix) {
  if (!matrix) return -1;
  const FieldInfo* type = ((Matrix*)matrix)->type;
  if (type == get_complex_type_info()) return API_TYPE_COMPLEX;
  return API_TYPE_DOUBLE;
}

API_EXPORT size_t api_matrix_size(void* matrix) {
  if (!matrix) return 0;
  return ((Matrix*)matrix)->n;
}

API_EXPORT void* api_create_matrix(size_t n, int type_id) {
  return matrix_create(n, get_type_by_id(type_id));
}

API_EXPORT void api_destroy_matrix(void* matrix) {
  if (matrix) matrix_destroy((Matrix*)matrix);
}

API_EXPORT void* api_matrix_sum(void* matrix1, void* matrix2) {
  return matrix_sum((Matrix*)matrix1, (Matrix*)matrix2);
}

API_EXPORT void* api_matrix_multiply(void* matrix1, void* matrix2) {
  return matrix_multiply((Matrix*)matrix1, (Matrix*)matrix2);
}

API_EXPORT void api_matrix_set(void* matrix, size_t row, size_t col, double real, double imag) {
  Matrix* m = (Matrix*)matrix;
  if (!m || row >= m->n || col >= m->n) return;

  if (m->type == get_double_type_info()) {
    matrix_set(m, row, col, &real);
  }
  else if (m->type == get_complex_type_info()) {
    Complex c;
    c.real = real;
    c.imag = imag;
    matrix_set(m, row, col, &c);
  }
}

API_EXPORT void* api_matrix_multiply_by_scalar(void* matrix, double scalar_real, double scalar_imag){
  if (!matrix) return NULL;
  Matrix* m = (Matrix*)matrix;
  Matrix* result = matrix_create(m->n, m->type);
  if (!result) return NULL;

  for (size_t i = 0; i < m->n; i++) {
    for (size_t j = 0; j < m->n; j++) {
      double real, imag;
      api_matrix_get(matrix, i, j, &real, &imag);

      if (m->type == get_double_type_info()) {
        double res = real * scalar_real;
        api_matrix_set(result, i, j, res, 0.0);
      } else {
        double res_real = real * scalar_real - imag * scalar_imag;
        double res_imag = real * scalar_imag + imag * scalar_real;
        api_matrix_set(result, i, j, res_real, res_imag);
      }
    }
  }
  return result;
}

API_EXPORT void api_matrix_get(void* matrix, size_t row, size_t col, double* real, double* imag) {
  Matrix* m = (Matrix*)matrix;

  // Инициализируем выходные значения нулями
  if (real) *real = 0.0;
  if (imag) *imag = 0.0;

  if (!m || row >= m->n || col >= m->n) return;
  if (!real || !imag) return;

  // Проверяем тип матрицы
  if (m->type == get_double_type_info()) {
    double* val = (double*)matrix_get(m, row, col);
    if (val) *real = *val;
  }
  else if (m->type == get_complex_type_info()) {
    Complex* val = (Complex*)matrix_get(m, row, col);
    if (val) {
      *real = val->real;
      *imag = val->imag;
    }
  }
}