#include "double_types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define EPSILON 1e-10

static void* d_clone(const void* src)
{
    double* p = malloc(sizeof(double));
    if (p) memcpy(p, src, sizeof(double));
    return p;
}

static void* d_zero(void)
{
    double* zero = malloc(sizeof(double));
    if (zero) *zero = 0.0;
    return zero;
}

static void* d_one(void)
{
    double* one = malloc(sizeof(double));
    if (one) *one = 1.0;
    return one;
}

static void d_destroy(void* elem) { free(elem); }

static void d_print(const void* elem)
{
    printf("%.2lf", *(const double*)elem);
}

static void d_sum(const void* a, const void* b, void* res)
{
    *(double*)res = *(const double*)a + *(const double*)b;
}

static void d_mul(const void* a, const void* b, void* res)
{
    *(double*)res = *(const double*)a * *(const double*)b;
}

static void d_mul_scalar(void* value, const void* scalar, void* res)
{
    *(double*)res = *(const double*)value * *(const double*)scalar;
}

static int d_divide(const void* a, const void* b, void* res)
{
    double divisor = *(const double*)b;
    if (fabs(divisor) < EPSILON) {
        return -1;
    }
    *(double*)res = *(const double*)a / divisor;
    return 0;
}

static int d_is_zero(const void* a)
{
    return fabs(*(const double*)a) < EPSILON ? 1 : 0;
}

static int d_equals(const void* a, const void* b, double eps)
{
    double diff = fabs(*(const double*)a - *(const double*)b);
    return (diff < eps) ? 1 : 0;
}

static FieldInfo* instance = NULL;

const FieldInfo* get_double_type_info(void) {
    if (!instance) {
        instance = malloc(sizeof(FieldInfo));
        if (instance) {
            instance->size = sizeof(double);
            instance->clone = d_clone;
            instance->zero = d_zero;
            instance->one = d_one;
            instance->destroy = d_destroy;
            instance->print = d_print;
            instance->sum = d_sum;
            instance->mul = d_mul;
            instance->mul_scalar = d_mul_scalar;
            instance->divide = d_divide;
            instance->is_zero = d_is_zero;
            instance->equals = d_equals;
        }
    }
    return instance;
}