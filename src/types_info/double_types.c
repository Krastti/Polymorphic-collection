#include "field_info.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void* d_clone(const void* src)
{
    double* p = malloc(sizeof(double));
    if (p) memcpy(p, src, sizeof(double));
    return p;
}

static void d_destroy(void* elem) { free(elem); }

static void d_print(const void* elem)
{
    printf("%.3lf", *(const double*)elem);
}

static void d_sum(const void* a, const void* b, void* res)
{
    *(double*)res = *(const double*)a + *(const double*)b;
}

static void d_mul(const void* a, const void* b, void* res)
{
    *(double*)res = *(const double*)a * *(const double*)b;
}

static void d_zero(void* res) { *(double*)res = 0.0; }
static void d_one(void* res) { *(double*)res = 1.0; }

static FieldInfo* instance = NULL;

const FieldInfo* get_double_type_info(void)
{
    if (instance != NULL) return instance;

    instance = malloc(sizeof(FieldInfo));
    instance->size = sizeof(double);
    instance->clone = d_clone;
    instance->destroy = d_destroy;

    instance->print = d_print;
    instance->sum = d_sum;
    instance->mul = d_mul;

    instance->zero = d_zero;
    instance->one = d_one;
    return instance;
}