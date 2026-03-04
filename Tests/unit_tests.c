//
// Created by Krastti on 01.03.2026.
//
#include  "../Типы данных/double_types.h"
#include  "../Типы данных/complex_types.h"
#include  "../Matrix/matrix.h"

#include <math.h>
#include <stdio.h>

static int tests_run = 0;
static int tests_failed = 0;
static int tests_passed = 0;

#define EPSILON 1e-6

#define ASSERT_TRUE(cond) \
    do { if (!(cond)) { \
        printf("\033[31m[FAIL]\033[0m %s at line %d\n", #cond, __LINE__); \
        tests_failed++; } \
        else { tests_passed++; } \
    } while (0)

#define ASSERT_EQ_INT(a, b) \
    do { if ((a) != (b)) { \
        printf("\033[31m[FAIL]\033[0m %zu != %zu at line %d\n", (size_t)(a), (size_t)(b), __LINE__); \
        tests_failed++; } \
        else { tests_passed++; } \
    } while (0)

#define ASSERT_NOT_NULL(ptr) \
    do { if ((ptr) == NULL) { \
        printf("\033[31m[FAIL]\033[0m Expected non-NULL at line %d\n", __LINE__); \
        tests_failed++; } \
        else { tests_passed++; } \
    } while (0)

#define ASSERT_NULL(ptr) \
    do { if ((ptr) != NULL) { \
        printf("\033[31m[FAIL]\033[0m Expected NULL at line %d\n", __LINE__); \
        tests_failed++; } \
        else { tests_passed++; } \
    } while (0)

#define ASSERT_NEAR_DOUBLE(a, b) \
    do { if (fabs((a) - (b)) > EPSILON) { \
        printf("\033[31m[FAIL]\033[0m %f != %f (diff=%e) at line %d\n", (a), (b), fabs((a)-(b)), __LINE__); \
        tests_failed++; } \
        else { tests_passed++; } \
    } while (0)

#define ASSERT_NEAR_COMPLEX(a_real, a_imag, b_real, b_imag) \
    do { \
        double diff_real = fabs((a_real) - (b_real)); \
        double diff_imag = fabs((a_imag) - (b_imag)); \
        if (diff_real > EPSILON || diff_imag > EPSILON) { \
            printf("\033[31m[FAIL]\033[0m (%.4f+%.4fi) != (%.4f+%.4fi) at line %d\n", \
                (a_real), (a_imag), (b_real), (b_imag), __LINE__); \
            tests_failed++; } \
        else { tests_passed++; } \
    } while (0)

#define RUN_TEST(func) \
    do { printf("\033[33m[TEST]\033[0m %s... ", #func); \
        tests_run++; \
        func(); \
        if (tests_failed == 0) printf("\033[32mOK\033[0m\n"); \
        else printf("\033[31mFAILED\033[0m\n"); \
    } while (0)

// Получить элемент матрицы для проверки
static void* matrix_get_element(const Matrix* m, size_t row, size_t col) {
    if (!m || row >= m->n || col >= m->n) return NULL;
    return m->data[row * m->n + col];
}

// Проверка значения double в матрице
static int check_double_element(const Matrix* m, size_t row, size_t col, double expected) {
    double* val = (double*)matrix_get_element(m, row, col);
    if (!val) return 0;
    return (fabs(*val - expected) <= EPSILON);
}

// Проверка значения Complex в матрице
static int check_complex_element(const Matrix* m, size_t row, size_t col,
                                  double exp_real, double exp_imag) {
    Complex* val = (Complex*)matrix_get_element(m, row, col);
    if (!val) return 0;
    return (fabs(val->real - exp_real) <= EPSILON &&
            fabs(val->imag - exp_imag) <= EPSILON);
}

void test_matrix_create_double() {
    Matrix* m = matrix_create(3, get_double_type_info());
    ASSERT_NOT_NULL(m);
    ASSERT_EQ_INT(m->n, 3);
    ASSERT_TRUE(m->type == get_double_type_info());
    ASSERT_NOT_NULL(m->data);
    matrix_destroy(m);
}

void test_matrix_create_complex() {
    Matrix* m = matrix_create(2, get_complex_type_info());
    ASSERT_NOT_NULL(m);
    ASSERT_EQ_INT(m->n, 2);
    ASSERT_TRUE(m->type == get_complex_type_info());
    matrix_destroy(m);
}

void test_matrix_create_null_type() {
    Matrix* m = matrix_create(2, NULL);
    ASSERT_NULL(m);
}

void test_matrix_create_zero_size() {
    Matrix* m = matrix_create(0, get_double_type_info());
    ASSERT_NOT_NULL(m);
    ASSERT_EQ_INT(m->n, 0);
    matrix_destroy(m);
}

// Уничтожение NULL-матрицы
void test_matrix_destroy_null() {
    matrix_destroy(NULL);
    ASSERT_TRUE(1);
}

void test_fill_double_matrix() {
    Matrix* m = matrix_create(2, get_double_type_info());
    double values[] = {1.0, 2.0, 3.0, 4.0};
    fill_double_matrix(m, values);

    ASSERT_TRUE(check_double_element(m, 0, 0, 1.0));
    ASSERT_TRUE(check_double_element(m, 0, 1, 2.0));
    ASSERT_TRUE(check_double_element(m, 1, 0, 3.0));
    ASSERT_TRUE(check_double_element(m, 1, 1, 4.0));

    matrix_destroy(m);
}

void test_fill_complex_matrix() {
    Matrix* m = matrix_create(2, get_complex_type_info());
    double values[][2] = {
        {1.0, 2.0},
        {3.0, 4.0},
        {5.0, 6.0},
        {7.0, 8.0}
    };
    fill_complex_matrix(m, values);

    ASSERT_TRUE(check_complex_element(m, 0, 0, 1.0, 2.0));
    ASSERT_TRUE(check_complex_element(m, 0, 1, 3.0, 4.0));
    ASSERT_TRUE(check_complex_element(m, 1, 0, 5.0, 6.0));
    ASSERT_TRUE(check_complex_element(m, 1, 1, 7.0, 8.0));

    matrix_destroy(m);
}

void test_fill_null_matrix() {
    double values[] = {1.0, 2.0, 3.0, 4.0};
    fill_double_matrix(NULL, values);
    ASSERT_TRUE(1);
}

void test_matrix_sum_double_basic() {
    Matrix* A = matrix_create(2, get_double_type_info());
    Matrix* B = matrix_create(2, get_double_type_info());

    double dataA[] = {1, 2, 3, 4};
    double dataB[] = {5, 6, 7, 8};
    fill_double_matrix(A, dataA);
    fill_double_matrix(B, dataB);

    Matrix* C = matrix_sum(A, B);
    ASSERT_NOT_NULL(C);
    ASSERT_EQ_INT(C->n, 2);

    ASSERT_TRUE(check_double_element(C, 0, 0, 6.0));
    ASSERT_TRUE(check_double_element(C, 0, 1, 8.0));
    ASSERT_TRUE(check_double_element(C, 1, 0, 10.0));
    ASSERT_TRUE(check_double_element(C, 1, 1, 12.0));

    matrix_destroy(A);
    matrix_destroy(B);
    matrix_destroy(C);
}

void test_matrix_sum_double_zero() {
    Matrix* A = matrix_create(2, get_double_type_info());
    Matrix* Z = matrix_create(2, get_double_type_info());

    double dataA[] = {5, 6, 7, 8};
    fill_double_matrix(A, dataA);

    Matrix* C = matrix_sum(A, Z);
    ASSERT_NOT_NULL(C);

    ASSERT_TRUE(check_double_element(C, 0, 0, 5.0));
    ASSERT_TRUE(check_double_element(C, 0, 1, 6.0));
    ASSERT_TRUE(check_double_element(C, 1, 0, 7.0));
    ASSERT_TRUE(check_double_element(C, 1, 1, 8.0));

    matrix_destroy(A);
    matrix_destroy(Z);
    matrix_destroy(C);
}

void test_matrix_sum_null_pointer() {
    Matrix* A = matrix_create(2, get_double_type_info());
    Matrix* C = matrix_sum(NULL, A);
    ASSERT_NULL(C);
    C = matrix_sum(A, NULL);
    ASSERT_NULL(C);
    matrix_destroy(A);
}

void test_matrix_sum_type_mismatch() {
    Matrix* A = matrix_create(2, get_double_type_info());
    Matrix* B = matrix_create(2, get_complex_type_info());

    Matrix* C = matrix_sum(A, B);
    ASSERT_NULL(C);

    matrix_destroy(A);
    matrix_destroy(B);
}

void test_matrix_sum_size_mismatch() {
    Matrix* A = matrix_create(2, get_double_type_info());
    Matrix* B = matrix_create(3, get_double_type_info());

    Matrix* C = matrix_sum(A, B);
    ASSERT_NULL(C);

    matrix_destroy(A);
    matrix_destroy(B);
}

void test_matrix_sum_complex_basic() {
    Matrix* A = matrix_create(2, get_complex_type_info());
    Matrix* B = matrix_create(2, get_complex_type_info());

    double dataA[][2] = {{1, 1}, {2, 2}, {3, 3}, {4, 4}};
    double dataB[][2] = {{1, 0}, {0, 1}, {0, 0}, {1, 1}};
    fill_complex_matrix(A, dataA);
    fill_complex_matrix(B, dataB);

    Matrix* C = matrix_sum(A, B);
    ASSERT_NOT_NULL(C);

    ASSERT_TRUE(check_complex_element(C, 0, 0, 2.0, 1.0));
    ASSERT_TRUE(check_complex_element(C, 0, 1, 2.0, 3.0));
    ASSERT_TRUE(check_complex_element(C, 1, 0, 3.0, 3.0));
    ASSERT_TRUE(check_complex_element(C, 1, 1, 5.0, 5.0));

    matrix_destroy(A);
    matrix_destroy(B);
    matrix_destroy(C);
}

void test_matrix_multiply_double_basic() {
    Matrix* A = matrix_create(2, get_double_type_info());
    Matrix* B = matrix_create(2, get_double_type_info());

    double dataA[4] = {1, 2, 3, 4};
    double dataB[4] = {2, 0, 1, 2};
    fill_double_matrix(A, dataA);
    fill_double_matrix(B, dataB);

    Matrix* C = matrix_multiply(A, B);
    ASSERT_NOT_NULL(C);

    ASSERT_TRUE(check_double_element(C, 0, 0, 4.0));
    ASSERT_TRUE(check_double_element(C, 0, 1, 4.0));
    ASSERT_TRUE(check_double_element(C, 1, 0, 10.0));
    ASSERT_TRUE(check_double_element(C, 1, 1, 8.0));

    matrix_destroy(A);
    matrix_destroy(B);
    matrix_destroy(C);
}

void test_matrix_multiply_identity() {
    Matrix* A = matrix_create(2, get_double_type_info());
    Matrix* I = matrix_create(2, get_double_type_info());

    double dataA[] = {5, 6, 7, 8};
    double dataI[] = {1, 0, 0, 1};
    fill_double_matrix(A, dataA);
    fill_double_matrix(I, dataI);

    Matrix* C = matrix_multiply(A, I);
    ASSERT_NOT_NULL(C);

    ASSERT_TRUE(check_double_element(C, 0, 0, 5.0));
    ASSERT_TRUE(check_double_element(C, 0, 1, 6.0));
    ASSERT_TRUE(check_double_element(C, 1, 0, 7.0));
    ASSERT_TRUE(check_double_element(C, 1, 1, 8.0));

    matrix_destroy(A);
    matrix_destroy(I);
    matrix_destroy(C);
}

void test_matrix_multiply_zero() {
    Matrix* A = matrix_create(2, get_double_type_info());
    Matrix* Z = matrix_create(2, get_double_type_info());

    double dataA[] = {1, 2, 3, 4};
    fill_double_matrix(A, dataA);

    Matrix* C = matrix_multiply(A, Z);
    ASSERT_NOT_NULL(C);

    ASSERT_TRUE(check_double_element(C, 0, 0, 0.0));
    ASSERT_TRUE(check_double_element(C, 0, 1, 0.0));
    ASSERT_TRUE(check_double_element(C, 1, 0, 0.0));
    ASSERT_TRUE(check_double_element(C, 1, 1, 0.0));

    matrix_destroy(A);
    matrix_destroy(Z);
    matrix_destroy(C);
}

void test_matrix_multiply_null_pointer() {
    Matrix* A = matrix_create(2, get_double_type_info());
    Matrix* C = matrix_multiply(NULL, A);
    ASSERT_NULL(C);
    C = matrix_multiply(A, NULL);
    ASSERT_NULL(C);
    matrix_destroy(A);
}

void test_matrix_multiply_type_mismatch() {
    Matrix* A = matrix_create(2, get_double_type_info());
    Matrix* B = matrix_create(2, get_complex_type_info());

    Matrix* C = matrix_multiply(A, B);
    ASSERT_NULL(C);

    matrix_destroy(A);
    matrix_destroy(B);
}

void test_matrix_multiply_size_mismatch() {
    Matrix* A = matrix_create(2, get_double_type_info());
    Matrix* B = matrix_create(3, get_double_type_info());

    Matrix* C = matrix_multiply(A, B);
    ASSERT_NULL(C);

    matrix_destroy(A);
    matrix_destroy(B);
}

void test_matrix_multiply_complex_basic() {
    Matrix* A = matrix_create(2, get_complex_type_info());
    Matrix* B = matrix_create(2, get_complex_type_info());

    double dataA[][2] = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};
    double dataB[][2] = {{0, 0}, {0, 1}, {0, 1}, {0, 0}};
    fill_complex_matrix(A, dataA);
    fill_complex_matrix(B, dataB);

    Matrix* C = matrix_multiply(A, B);
    ASSERT_NOT_NULL(C);

    ASSERT_TRUE(check_complex_element(C, 0, 0, 0.0, 0.0));
    ASSERT_TRUE(check_complex_element(C, 0, 1, -1.0, 0.0));
    ASSERT_TRUE(check_complex_element(C, 1, 0, -1.0, 0.0));
    ASSERT_TRUE(check_complex_element(C, 1, 1, 0.0, 0.0));

    matrix_destroy(A);
    matrix_destroy(B);
    matrix_destroy(C);
}

void test_matrix_scalar_multiply_double_basic() {
    Matrix* A = matrix_create(2, get_double_type_info());
    double dataA[] = {1, 2, 3, 4};
    fill_double_matrix(A, dataA);

    double scalar = 3.0;
    matrix_scalar_multiply(A, &scalar);

    ASSERT_TRUE(check_double_element(A, 0, 0, 3.0));
    ASSERT_TRUE(check_double_element(A, 0, 1, 6.0));
    ASSERT_TRUE(check_double_element(A, 1, 0, 9.0));
    ASSERT_TRUE(check_double_element(A, 1, 1, 12.0));

    matrix_destroy(A);
}

void test_matrix_scalar_multiply_double_zero() {
    Matrix* A = matrix_create(2, get_double_type_info());
    double dataA[] = {5, 6, 7, 8};
    fill_double_matrix(A, dataA);

    double scalar = 0.0;
    matrix_scalar_multiply(A, &scalar);

    ASSERT_TRUE(check_double_element(A, 0, 0, 0.0));
    ASSERT_TRUE(check_double_element(A, 0, 1, 0.0));
    ASSERT_TRUE(check_double_element(A, 1, 0, 0.0));
    ASSERT_TRUE(check_double_element(A, 1, 1, 0.0));

    matrix_destroy(A);
}

void test_matrix_scalar_multiply_double_one() {
    Matrix* A = matrix_create(2, get_double_type_info());
    double dataA[] = {1, 2, 3, 4};
    fill_double_matrix(A, dataA);

    double scalar = 1.0;
    matrix_scalar_multiply(A, &scalar);

    ASSERT_TRUE(check_double_element(A, 0, 0, 1.0));
    ASSERT_TRUE(check_double_element(A, 0, 1, 2.0));
    ASSERT_TRUE(check_double_element(A, 1, 0, 3.0));
    ASSERT_TRUE(check_double_element(A, 1, 1, 4.0));

    matrix_destroy(A);
}

void test_matrix_scalar_multiply_null() {
    Matrix* A = matrix_create(2, get_double_type_info());
    matrix_scalar_multiply(NULL, &((double){1.0}));
    matrix_scalar_multiply(A, NULL);
    ASSERT_TRUE(1);
    matrix_destroy(A);
}

void test_matrix_scalar_multiply_complex_basic() {
    Matrix* A = matrix_create(2, get_complex_type_info());
    double dataA[][2] = {{1, 0}, {0, 1}, {0, 0}, {1, 1}};
    fill_complex_matrix(A, dataA);

    Complex scalar = {0.0, 1.0};  /* i */
    matrix_scalar_multiply(A, &scalar);

    ASSERT_TRUE(check_complex_element(A, 0, 0, 0.0, 1.0));
    ASSERT_TRUE(check_complex_element(A, 0, 1, -1.0, 0.0));
    ASSERT_TRUE(check_complex_element(A, 1, 0, 0.0, 0.0));
    ASSERT_TRUE(check_complex_element(A, 1, 1, -1.0, 1.0));

    matrix_destroy(A);
}

void test_matrix_1x1_double() {
    Matrix* A = matrix_create(1, get_double_type_info());
    Matrix* B = matrix_create(1, get_double_type_info());

    double dataA[] = {5.0};
    double dataB[] = {3.0};
    fill_double_matrix(A, dataA);
    fill_double_matrix(B, dataB);

    Matrix* C = matrix_sum(A, B);
    ASSERT_NOT_NULL(C);
    ASSERT_TRUE(check_double_element(C, 0, 0, 8.0));

    Matrix* D = matrix_multiply(A, B);
    ASSERT_NOT_NULL(D);
    ASSERT_TRUE(check_double_element(D, 0, 0, 15.0));

    matrix_destroy(A);
    matrix_destroy(B);
    matrix_destroy(C);
    matrix_destroy(D);
}

void test_matrix_3x3_double() {
    Matrix* A = matrix_create(3, get_double_type_info());
    Matrix* B = matrix_create(3, get_double_type_info());

    double dataA[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    double dataB[] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    fill_double_matrix(A, dataA);
    fill_double_matrix(B, dataB);

    Matrix* C = matrix_sum(A, B);
    ASSERT_NOT_NULL(C);
    ASSERT_EQ_INT(C->n, 3);

    /* Проверка нескольких элементов */
    ASSERT_TRUE(check_double_element(C, 0, 0, 10.0));
    ASSERT_TRUE(check_double_element(C, 1, 1, 10.0));
    ASSERT_TRUE(check_double_element(C, 2, 2, 10.0));

    matrix_destroy(A);
    matrix_destroy(B);
    matrix_destroy(C);
}

void test_matrix_negative_values() {
    Matrix* A = matrix_create(2, get_double_type_info());
    double dataA[] = {-1, -2, -3, -4};
    fill_double_matrix(A, dataA);

    double scalar = -2.0;
    matrix_scalar_multiply(A, &scalar);

    ASSERT_TRUE(check_double_element(A, 0, 0, 2.0));
    ASSERT_TRUE(check_double_element(A, 0, 1, 4.0));
    ASSERT_TRUE(check_double_element(A, 1, 0, 6.0));
    ASSERT_TRUE(check_double_element(A, 1, 1, 8.0));

    matrix_destroy(A);
}

void test_matrix_floating_point_precision() {
    Matrix* A = matrix_create(2, get_double_type_info());
    double dataA[] = {0.1, 0.2, 0.3, 0.4};
    fill_double_matrix(A, dataA);

    Matrix* B = matrix_create(2, get_double_type_info());
    double dataB[] = {0.1, 0.2, 0.3, 0.4};
    fill_double_matrix(B, dataB);

    Matrix* C = matrix_sum(A, B);
    ASSERT_NOT_NULL(C);

    ASSERT_TRUE(check_double_element(C, 0, 0, 0.2));
    ASSERT_TRUE(check_double_element(C, 1, 1, 0.8));

    matrix_destroy(A);
    matrix_destroy(B);
    matrix_destroy(C);
}

void test_matrix_memory_leak_check() {
    /* Создаём и уничтожаем множество матриц для проверки утечек памяти */
    for (int i = 0; i < 10; i++) {
        Matrix* m = matrix_create(2, get_double_type_info());
        double data[] = {1, 2, 3, 4};
        fill_double_matrix(m, data);
        matrix_destroy(m);
    }
    ASSERT_TRUE(1);
}


void test_matrix_print_double() {
    printf("\n\033[36m[VISUAL]\033[0m Matrix Print (Double):\n");
    Matrix* m = matrix_create(2, get_double_type_info());
    double data[] = {1.5, 2.5, 3.5, 4.5};
    fill_double_matrix(m, data);
    matrix_print(m);
    matrix_destroy(m);
    ASSERT_TRUE(1);
}

void test_matrix_print_complex() {
    printf("\n\033[36m[VISUAL]\033[0m Matrix Print (Complex):\n");
    Matrix* m = matrix_create(2, get_complex_type_info());
    double data[][2] = {{1, 2}, {3, 4}, {5, 6}, {7, 8}};
    fill_complex_matrix(m, data);
    matrix_print(m);
    matrix_destroy(m);
    ASSERT_TRUE(1);
}

int unit_tests() {
    printf("\033[1m========================================\033[0m\n");
    printf("\033[1m  Unit-тесты                            \033[0m\n");
    printf("\033[1m========================================\033[0m\n\n");

    printf("\033[1m--- Создание & Уничтожение ---\033[0m\n");
    RUN_TEST(test_matrix_create_double);
    RUN_TEST(test_matrix_create_complex);
    RUN_TEST(test_matrix_create_null_type);
    RUN_TEST(test_matrix_create_zero_size);
    RUN_TEST(test_matrix_destroy_null);

    printf("\n\033[1m--- Заполнение матриц ---\033[0m\n");
    RUN_TEST(test_fill_double_matrix);
    RUN_TEST(test_fill_complex_matrix);
    RUN_TEST(test_fill_null_matrix);

    printf("\n\033[1m--- Сумма вещественных матриц ---\033[0m\n");
    RUN_TEST(test_matrix_sum_double_basic);
    RUN_TEST(test_matrix_sum_double_zero);
    RUN_TEST(test_matrix_sum_null_pointer);
    RUN_TEST(test_matrix_sum_type_mismatch);
    RUN_TEST(test_matrix_sum_size_mismatch);

    printf("\n\033[1m--- Сумма комплексных матриц ---\033[0m\n");
    RUN_TEST(test_matrix_sum_complex_basic);

    printf("\n\033[1m--- Умножение вещественных матриц ---\033[0m\n");
    RUN_TEST(test_matrix_multiply_double_basic);
    RUN_TEST(test_matrix_multiply_identity);
    RUN_TEST(test_matrix_multiply_zero);
    RUN_TEST(test_matrix_multiply_null_pointer);
    RUN_TEST(test_matrix_multiply_type_mismatch);
    RUN_TEST(test_matrix_multiply_size_mismatch);

    printf("\n\033[1m--- Умножение комплексных матриц ---\033[0m\n");
    RUN_TEST(test_matrix_multiply_complex_basic);

    printf("\n\033[1m--- Умножение на вещественный скаляр ---\033[0m\n");
    RUN_TEST(test_matrix_scalar_multiply_double_basic);
    RUN_TEST(test_matrix_scalar_multiply_double_zero);
    RUN_TEST(test_matrix_scalar_multiply_double_one);
    RUN_TEST(test_matrix_scalar_multiply_null);

    printf("\n\033[1m--- Умножение на комплексный скаляр ---\033[0m\n");
    RUN_TEST(test_matrix_scalar_multiply_complex_basic);

    printf("\n\033[1m--- Граничные случаи & Обработка ошибок ---\033[0m\n");
    RUN_TEST(test_matrix_1x1_double);
    RUN_TEST(test_matrix_3x3_double);
    RUN_TEST(test_matrix_negative_values);
    RUN_TEST(test_matrix_floating_point_precision);
    RUN_TEST(test_matrix_memory_leak_check);

    printf("\n\033[1m--- Визуальные тесты---\033[0m\n");
    RUN_TEST(test_matrix_print_double);
    RUN_TEST(test_matrix_print_complex);

    printf("\n\033[1m========================================\033[0m\n");
    printf("\033[1m  Итоги тестов:                       \033[0m\n");
    printf("\033[1m========================================\033[0m\n");
    printf("Пройдено:  \033[33m%d\033[0m\n", tests_run);
    printf("Провалено: \033[31m%d\033[0m\n", tests_failed);
    printf("Успешных ассертов: \033[32m%d\033[0m\n", tests_passed);
    printf("\033[1m========================================\033[0m\n");

    if (tests_failed == 0) {
        printf("\n\033[32m Все тесты пройдены!\033[0m\n\n");
        return 0;
    } else {
        printf("\n\033[31m Некоторые тесты не прошли проверку!\033[0m\n\n");
        return 1;
    }
}