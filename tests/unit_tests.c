#include "unit_tests.h"
#include "../src/matrix/matrix.h"
#include "../src/types_info/double_type.h"
#include "../src/types_info/complex_type.h"
#include <stdio.h>
#include <math.h>

static int tests_run = 0;
static int tests_failed = 0;
static int tests_passed = 0;

static void assert_true(const int condition, const char *test_name) {
    tests_run++;
    if (condition) {
        tests_passed++;
        printf(" [PASS] %s\n", test_name);
    } else {
        tests_failed++;
        printf(" [FAIL] %s\n", test_name);
    }
}

static void assert_false(const int condition, const char *test_name) {
    assert_true(!condition, test_name);
}

static void assert_null(const void *ptr, const char *test_name) {
    assert_true(ptr == NULL, test_name);
}

static void assert_not_null(const void *ptr, const char *test_name) {
    assert_true(ptr != NULL, test_name);
}

static void assert_double_equal(const double actual, const double expected, const char *test_name) {
    int ok = fabs(actual - expected) < 1e-9;
    tests_run++;
    if (!ok) {
        printf(" [FAIL] %s (expected=%0.9f, actual=%.9f)\n", test_name, expected, actual);
        tests_failed++;
    } else {
        printf(" [PASS] %s\n", test_name);
        tests_passed++;
    }
}

static void test_create_returns_non_null(void) {
    Matrix *matrix = matrix_create(3, get_double_type_info());
    assert_not_null(matrix, "test_create_returns_non_null");
    matrix_destroy(matrix);
}

static void test_create_1x1(void) {
    Matrix *matrix = matrix_create(1, get_double_type_info());
    assert_not_null(matrix, "test_create_1x1");
    matrix_destroy(matrix);
}

static void test_create_0x0(void) {
    Matrix *matrix = matrix_create(0, get_double_type_info());
    assert_not_null(matrix, "test_create_0x0");
}

static void test_create_null_type(void) {
    Matrix *matrix = matrix_create(3, NULL);
    assert_null(matrix, "test_create_null_type");
}

static void test_get_set_double(void) {
    Matrix *matrix = matrix_create(2, get_double_type_info());
    double value = 3.0;
    matrix_set(matrix, 0, 1, &value);
    double *got = (double *)matrix_get(matrix, 0, 1);
    assert_not_null(got, "test_get_set_double");
    assert_double_equal(*got, 3.0, "test_get_set_double");
    matrix_destroy(matrix);
}

static void test_get_set_complex(void) {
    Matrix *matrix = matrix_create(2, get_complex_type_info());
    Complex value = make_complex(3.0, 4.0);
    matrix_set(matrix, 0, 1, &value);
    Complex *got = (Complex *)matrix_get(matrix, 0, 1);
    assert_not_null(got, "test_get_set_complex");
    assert_double_equal(got->real, 3.0, "test_get_set_complex");
    assert_double_equal(got->imag, 4.0, "test_get_set_complex");
    matrix_destroy(matrix);
}

static void test_get_out_of_bounds(void) {
    Matrix *matrix = matrix_create(2, get_double_type_info());
    void *got = matrix_get(matrix, 3, 3);
    assert_null(got, "test_get_out_of_bounds");
    matrix_destroy(matrix);
}

static void test_get_null_matrix(void) {
    void *got = matrix_get(NULL, 0, 0);
    assert_null(got, "test_get_null_matrix");
}

static void test_set_null_matrix(void) {
    double value = 5.0;
    matrix_set(NULL, 0, 0, &value);
    assert_true(1, "test_set_null_matrix");
}

static void test_set_out_of_bounds_row(void) {
    Matrix *matrix = matrix_create(2, get_double_type_info());
    double value = 5.0;
    matrix_set(matrix, 5, 0, &value);
    assert_not_null(matrix, "test_set_out_of_bounds_row");
    matrix_destroy(matrix);
}

static void test_set_out_of_bounds_col(void) {
    Matrix *matrix = matrix_create(2, get_double_type_info());
    double value = 5.0;
    matrix_set(matrix, 0, 5, &value);
    assert_not_null(matrix, "test_set_out_of_bounds_col");
    matrix_destroy(matrix);
}

static void test_sum_double(void) {
    Matrix *matrix1 = matrix_create(2, get_double_type_info());
    Matrix *matrix2 = matrix_create(2, get_double_type_info());
    double va[4] = {1.0, 2.0, 3.0, 4.0};
    double vb[4] = {5.0, 6.0, 7.0, 8.0};
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            matrix_set(matrix1, i, j, &va[i*2+j]);
            matrix_set(matrix2, i, j, &vb[i*2+j]);
        }
    }
    Matrix *result = matrix_sum(matrix1, matrix2);
    assert_not_null(result, "test_sum_double");
    assert_double_equal(*(double *)matrix_get(result, 0, 0), 6.0, "test_sum_double");
    assert_double_equal(*(double *)matrix_get(result, 0, 1), 8.0, "test_sum_double");
    assert_double_equal(*(double *)matrix_get(result, 1, 0), 10.0, "test_sum_double");
    assert_double_equal(*(double *)matrix_get(result, 1, 1), 12.0, "test_sum_double");
    matrix_destroy(matrix1);
    matrix_destroy(matrix2);
    matrix_destroy(result);
}

static void test_sum_with_zero_matrix(void) {
    Matrix *matrix = matrix_create(2, get_double_type_info());
    Matrix *zero = matrix_create(2, get_double_type_info());
    double value = 1.0;
    matrix_set(matrix, 0, 1, &value);
    Matrix *result = matrix_sum(matrix, zero);
    assert_double_equal(*(double *)matrix_get(result, 0, 1), 1.0, "test_sum_with_zero_matrix");
    assert_double_equal(*(double *)matrix_get(result, 0, 0), 0.0, "test_sum_with_zero_matrix");
    matrix_destroy(matrix);
    matrix_destroy(zero);
    matrix_destroy(result);
}

static void test_sum_incompatible_sizes(void) {
    Matrix *matrix1 = matrix_create(2, get_double_type_info());
    Matrix *matrix2 = matrix_create(3, get_double_type_info());
    Matrix *result = matrix_sum(matrix1, matrix2);
    assert_null(result, "test_sum_incompatible_sizes");
    matrix_destroy(matrix1);
    matrix_destroy(matrix2);
}

static void test_sum_complex(void) {
    Matrix *matrix1 = matrix_create(2, get_complex_type_info());
    Matrix *matrix2 = matrix_create(2, get_complex_type_info());
    Complex va = make_complex(1.0, 2.0);
    Complex vb = make_complex(3.0, -1.0);
    matrix_set(matrix1, 0, 0, &va);
    matrix_set(matrix2, 0, 0, &vb);
    Matrix *result = matrix_sum(matrix1, matrix2);
    Complex *res = (Complex *)matrix_get(result, 0, 0);
    assert_double_equal(res->real, 4.0, "test_sum_complex");
    assert_double_equal(res->imag, 1.0, "test_sum_complex");
    matrix_destroy(matrix1);
    matrix_destroy(matrix2);
    matrix_destroy(result);
}

static void test_sum_null_matrix1(void) {
    Matrix *matrix2 = matrix_create(2, get_double_type_info());
    Matrix *result = matrix_sum(NULL, matrix2);
    assert_null(result, "test_sum_null_matrix1");
    matrix_destroy(matrix2);
}

static void test_sum_null_matrix2(void) {
    Matrix *matrix1 = matrix_create(2, get_double_type_info());
    Matrix *result = matrix_sum(matrix1, NULL);
    assert_null(result, "test_sum_null_matrix2");
    matrix_destroy(matrix1);
}

static void test_sum_both_null(void) {
    Matrix *result = matrix_sum(NULL, NULL);
    assert_null(result, "test_sum_both_null");
}

static void test_sum_different_types(void) {
    Matrix *matrix1 = matrix_create(2, get_double_type_info());
    Matrix *matrix2 = matrix_create(2, get_complex_type_info());
    Matrix *result = matrix_sum(matrix1, matrix2);
    assert_null(result, "test_sum_different_types");
    matrix_destroy(matrix1);
    matrix_destroy(matrix2);
}

static void test_mul_double_2x2(void) {
    Matrix *matrix1 = matrix_create(2, get_double_type_info());
    Matrix *matrix2 = matrix_create(2, get_double_type_info());
    double va[4] = {1.0, 2.0, 3.0, 4.0};
    double vb[4] = {2.0, 0.0, 1.0, 2.0};
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            matrix_set(matrix1, i, j, &va[i*2+j]);
            matrix_set(matrix2, i, j, &vb[i*2+j]);
        }
    }
    Matrix *result = matrix_multiply(matrix1, matrix2);
    assert_not_null(result, "test_mul_double_2x2");
    assert_double_equal(*(double *)matrix_get(result, 0, 0), 4.0, "test_mul_double_2x2");
    assert_double_equal(*(double *)matrix_get(result, 0, 1), 4.0, "test_mul_double_2x2");
    assert_double_equal(*(double *)matrix_get(result, 1, 0), 10.0, "test_mul_double_2x2");
    assert_double_equal(*(double *)matrix_get(result, 1, 1), 8.0, "test_mul_double_2x2");
    matrix_destroy(matrix1);
    matrix_destroy(matrix2);
    matrix_destroy(result);
}

static void test_mul_by_identity(void) {
    Matrix *matrix1 = matrix_create(2, get_double_type_info());
    Matrix *matrix2 = matrix_create(2, get_double_type_info());
    double va[4] = {1.0, 2.0, 3.0, 4.0};
    double vb[4] = {1.0, 0.0, 0.0, 1.0};
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            matrix_set(matrix1, i, j, &va[i*2+j]);
            matrix_set(matrix2, i, j, &vb[i*2+j]);
        }
    }
    Matrix *result = matrix_multiply(matrix1, matrix2);
    assert_not_null(result, "test_mul_by_identity");
    assert_double_equal(*(double *)matrix_get(result, 0, 0), 1.0, "test_mul_by_identity");
    assert_double_equal(*(double *)matrix_get(result, 0, 1), 2.0, "test_mul_by_identity");
    assert_double_equal(*(double *)matrix_get(result, 1, 0), 3.0, "test_mul_by_identity");
    assert_double_equal(*(double *)matrix_get(result, 1, 1), 4.0, "test_mul_by_identity");
    matrix_destroy(matrix1);
    matrix_destroy(matrix2);
    matrix_destroy(result);
}

static void test_mul_by_zero(void) {
    Matrix *matrix1 = matrix_create(2, get_double_type_info());
    Matrix *matrix2 = matrix_create(2, get_double_type_info());
    double value = 5.0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            matrix_set(matrix1, i, j, &value);
        }
    }
    Matrix *result = matrix_multiply(matrix1, matrix2);
    assert_double_equal(*(double *)matrix_get(result, 0, 0), 0.0, "test_mul_by_zero");
    assert_double_equal(*(double *)matrix_get(result, 0, 1), 0.0, "test_mul_by_zero");
    assert_double_equal(*(double *)matrix_get(result, 1, 0), 0.0, "test_mul_by_zero");
    assert_double_equal(*(double *)matrix_get(result, 1, 1), 0.0, "test_mul_by_zero");
    matrix_destroy(matrix1);
    matrix_destroy(matrix2);
    matrix_destroy(result);
}

static void test_mul_incompatible_size(void) {
    Matrix *matrix1 = matrix_create(2, get_double_type_info());
    Matrix *matrix2 = matrix_create(3, get_double_type_info());
    Matrix *result = matrix_multiply(matrix1, matrix2);
    assert_null(result, "test_mul_incompatible_size");
    matrix_destroy(matrix1);
    matrix_destroy(matrix2);
}

static void test_mul_1x1(void) {
    Matrix *matrix1 = matrix_create(1, get_double_type_info());
    Matrix *matrix2 = matrix_create(1, get_double_type_info());
    double va = 4.0, vb = 5.0;
    matrix_set(matrix1, 0, 0, &va);
    matrix_set(matrix2, 0, 0, &vb);
    Matrix *result = matrix_multiply(matrix1, matrix2);
    assert_double_equal(*(double *)matrix_get(result, 0, 0), 20.0, "test_mul_1x1");
    matrix_destroy(matrix1);
    matrix_destroy(matrix2);
    matrix_destroy(result);
}

static void test_mul_null_matrix1(void) {
    Matrix *matrix2 = matrix_create(2, get_double_type_info());
    Matrix *result = matrix_multiply(NULL, matrix2);
    assert_null(result, "test_mul_null_matrix1");
    matrix_destroy(matrix2);
}

static void test_mul_null_matrix2(void) {
    Matrix *matrix1 = matrix_create(2, get_double_type_info());
    Matrix *result = matrix_multiply(matrix1, NULL);
    assert_null(result, "test_mul_null_matrix2");
    matrix_destroy(matrix1);
}

static void test_mul_both_null(void) {
    Matrix *result = matrix_multiply(NULL, NULL);
    assert_null(result, "test_mul_both_null");
}

static void test_mul_different_types(void) {
    Matrix *matrix1 = matrix_create(2, get_double_type_info());
    Matrix *matrix2 = matrix_create(2, get_complex_type_info());
    Matrix *result = matrix_multiply(matrix1, matrix2);
    assert_null(result, "test_mul_different_types");
    matrix_destroy(matrix1);
    matrix_destroy(matrix2);
}

static void test_mul_by_scalar_double(void) {
    Matrix *matrix = matrix_create(2, get_double_type_info());
    double va[4] = {1.0, 2.0, 3.0, 4.0};
    double scalar = 2.0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            matrix_set(matrix, i, j, &va[i*2+j]);
        }
    }
    Matrix *result = matrix_multiply_by_scalar(matrix, &scalar);
    assert_not_null(result, "test_mul_by_scalar_double");
    assert_double_equal(*(double *)matrix_get(result, 0, 0), 2.0, "test_mul_by_scalar_double");
    assert_double_equal(*(double *)matrix_get(result, 0, 1), 4.0, "test_mul_by_scalar_double");
    assert_double_equal(*(double *)matrix_get(result, 1, 0), 6.0, "test_mul_by_scalar_double");
    assert_double_equal(*(double *)matrix_get(result, 1, 1), 8.0, "test_mul_by_scalar_double");
    matrix_destroy(matrix);
    matrix_destroy(result);
}

static void test_mul_by_scalar_null_matrix(void) {
    double scalar = 2.0;
    Matrix *result = matrix_multiply_by_scalar(NULL, &scalar);
    assert_null(result, "test_mul_by_scalar_null_matrix");
}

static void test_mul_by_scalar_null_scalar(void) {
    Matrix *matrix = matrix_create(2, get_double_type_info());
    Matrix *result = matrix_multiply_by_scalar(matrix, NULL);
    assert_null(result, "test_mul_by_scalar_null_scalar");
    matrix_destroy(matrix);
}

static void test_mul_by_scalar_complex(void) {
    Matrix *matrix = matrix_create(2, get_complex_type_info());
    Complex va = make_complex(1.0, 2.0);
    Complex scalar = make_complex(2.0, 0.0);
    matrix_set(matrix, 0, 0, &va);
    Matrix *result = matrix_multiply_by_scalar(matrix, &scalar);
    Complex *res = (Complex *)matrix_get(result, 0, 0);
    assert_not_null(result, "test_mul_by_scalar_complex");
    assert_double_equal(res->real, 2.0, "test_mul_by_scalar_complex");
    assert_double_equal(res->imag, 4.0, "test_mul_by_scalar_complex");
    matrix_destroy(matrix);
    matrix_destroy(result);
}

static void test_print_null_matrix(void) {
    matrix_print(NULL);
    assert_true(1, "test_print_null_matrix");
}

static void test_print_0x0_matrix(void) {
    Matrix *matrix = matrix_create(0, get_double_type_info());
    assert_not_null(matrix, "test_print_0x0_matrix");
}

static void test_print_1x1_matrix(void) {
    Matrix *matrix = matrix_create(1, get_double_type_info());
    double value = 5.0;
    matrix_set(matrix, 0, 0, &value);
    matrix_print(matrix);
    assert_not_null(matrix, "test_print_1x1_matrix");
    matrix_destroy(matrix);
}

static void test_print_2x2_matrix(void) {
    Matrix *matrix = matrix_create(2, get_double_type_info());
    double va[4] = {1.0, 2.0, 3.0, 4.0};
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            matrix_set(matrix, i, j, &va[i*2+j]);
        }
    }
    matrix_print(matrix);
    assert_not_null(matrix, "test_print_2x2_matrix");
    matrix_destroy(matrix);
}

static void test_lu_decomposition_basic(void) {
    Matrix *matrix = matrix_create(2, get_double_type_info());
    double va[4] = {4.0, 3.0, 6.0, 3.0};
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            matrix_set(matrix, i, j, &va[i*2+j]);
        }
    }
    Matrix *L = NULL;
    Matrix *U = NULL;
    bool result = matrix_lu_decomposition(matrix, &L, &U);
    assert_true(result, "test_lu_decomposition_basic");
    assert_not_null(L, "test_lu_decomposition_basic_L");
    assert_not_null(U, "test_lu_decomposition_basic_U");
    assert_double_equal(*(double *)matrix_get(L, 0, 0), 1.0, "test_lu_decomposition_basic_L00");
    matrix_destroy(matrix);
    matrix_destroy(L);
    matrix_destroy(U);
}

static void test_lu_decomposition_null_matrix(void) {
    Matrix *L = NULL;
    Matrix *U = NULL;
    bool result = matrix_lu_decomposition(NULL, &L, &U);
    assert_false(result, "test_lu_decomposition_null_matrix");
}

static void test_lu_decomposition_null_out_L(void) {
    Matrix *matrix = matrix_create(2, get_double_type_info());
    Matrix *U = NULL;
    bool result = matrix_lu_decomposition(matrix, NULL, &U);
    assert_false(result, "test_lu_decomposition_null_out_L");
    matrix_destroy(matrix);
}

static void test_lu_decomposition_null_out_U(void) {
    Matrix *matrix = matrix_create(2, get_double_type_info());
    Matrix *L = NULL;
    bool result = matrix_lu_decomposition(matrix, &L, NULL);
    assert_false(result, "test_lu_decomposition_null_out_U");
    matrix_destroy(matrix);
}

static void test_lu_decomposition_3x3(void) {
    Matrix *matrix = matrix_create(3, get_double_type_info());
    double va[9] = {2.0, 1.0, 1.0, 4.0, 3.0, 3.0, 8.0, 7.0, 9.0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix_set(matrix, i, j, &va[i*3+j]);
        }
    }
    Matrix *L = NULL;
    Matrix *U = NULL;
    bool result = matrix_lu_decomposition(matrix, &L, &U);
    assert_true(result, "test_lu_decomposition_3x3");
    assert_not_null(L, "test_lu_decomposition_3x3_L");
    assert_not_null(U, "test_lu_decomposition_3x3_U");
    matrix_destroy(matrix);
    matrix_destroy(L);
    matrix_destroy(U);
}

static void test_destroy_null(void) {
    matrix_destroy(NULL);
    assert_true(1, "test_destroy_null");
}

void run_all_tests(void) {
    printf("\n=== Тесты создания матриц ===\n");
    test_create_returns_non_null();
    test_create_1x1();
    test_create_0x0();
    test_create_null_type();

    printf("\n=== Тесты получения/установки значений ===\n");
    test_get_set_double();
    test_get_set_complex();
    test_get_out_of_bounds();
    test_get_null_matrix();
    test_set_null_matrix();
    test_set_out_of_bounds_row();
    test_set_out_of_bounds_col();

    printf("\n=== Тесты сложения матриц ===\n");
    test_sum_double();
    test_sum_with_zero_matrix();
    test_sum_incompatible_sizes();
    test_sum_complex();
    test_sum_null_matrix1();
    test_sum_null_matrix2();
    test_sum_both_null();
    test_sum_different_types();

    printf("\n=== Тесты умножения матриц ===\n");
    test_mul_double_2x2();
    test_mul_by_identity();
    test_mul_by_zero();
    test_mul_incompatible_size();
    test_mul_1x1();
    test_mul_null_matrix1();
    test_mul_null_matrix2();
    test_mul_both_null();
    test_mul_different_types();

    printf("\n=== Тесты умножения на скаляр ===\n");
    test_mul_by_scalar_double();
    test_mul_by_scalar_null_matrix();
    test_mul_by_scalar_null_scalar();
    test_mul_by_scalar_complex();

    printf("\n=== Тесты печати матриц ===\n");
    test_print_null_matrix();
    test_print_0x0_matrix();
    test_print_1x1_matrix();
    test_print_2x2_matrix();

    printf("\n=== Тесты LU-разложения ===\n");
    test_lu_decomposition_basic();
    test_lu_decomposition_null_matrix();
    test_lu_decomposition_null_out_L();
    test_lu_decomposition_null_out_U();
    test_lu_decomposition_3x3();

    printf("\n=== Тесты уничтожения матриц ===\n");
    test_destroy_null();

    printf("\n========================================\n");
    printf("Тестов запущено: %d\n", tests_run);
    printf("Тестов пройдено: %d\n", tests_passed);
    printf("Тестов провалено: %d\n", tests_failed);
    printf("========================================\n");

    if (tests_run == tests_passed) {
      printf("\nВсе тесты пройдены. Нажмите чтобы продолжить\n");
    } else {
      printf("\nНе все тесты прошли проверку. Нажмите чтобы продолжить");
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar();
}