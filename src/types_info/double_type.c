#include "field_info.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DOUBLE_EPSILON 1e-9

static void* d_clone(const void* src) {
    double* p = malloc(sizeof(double));
    if (p) memcpy(p, src, sizeof(double));
    return p;
}

static void d_destroy(void* elem) { free(elem); }

static void d_print(const void* elem) {
    printf("%.3lf", *(const double*)elem);
}

static void d_sum(const void* a, const void* b, void* res) {
    *(double*)res = *(const double*)a + *(const double*)b;
}

static void d_sub(const void* a, const void* b, void* res) {
    *(double*)res = *(const double*)a - *(const double*)b;
}

static void d_mul(const void* a, const void* b, void* res) {
    *(double*)res = *(const double*)a * *(const double*)b;
}

static void d_div(const void* a, const void* b, void* res) {
    *(double*)res = *(const double*)a / *(const double*)b;
}

static void d_zero(void* res) { *(double*)res = 0.0; }
static void d_one(void* res) { *(double*)res = 1.0; }

static void d_neg(const void* elem, void* res) {
    *(double*)res = -1.0 * *(const double*)elem;
}

static void d_frac(const void* elem, void* res) {
    if (fabs(*(const double*)elem) < DOUBLE_EPSILON) {
        *(double*)res = 0.0;
        return;
    }
    *(double*)res = 1.0 / *(const double*)elem;
}

static FieldInfo* instance = NULL;

const FieldInfo* get_double_type_info(void) {
    if (instance != NULL) return instance;

    instance = malloc(sizeof(FieldInfo));
    instance->size = sizeof(double);
    instance->clone = d_clone;
    instance->destroy = d_destroy;

    instance->print = d_print;
    instance->sum = d_sum;
    instance->sub = d_sub;
    instance->mul = d_mul;
    instance->div = d_div;

    instance->zero = d_zero;
    instance->one = d_one;

    instance->neg = d_neg;
    instance->frac = d_frac;

    return instance;
}