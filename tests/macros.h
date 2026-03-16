#ifndef LABORATORYWORK1_MACROS_H
#define LABORATORYWORK1_MACROS_H

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
#endif //LABORATORYWORK1_MACROS_H