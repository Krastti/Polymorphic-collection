//
// Created by Krastti on 01.03.2026.
//
#include "complex_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define EPSILON 1e-10

static void* c_clone(const void* src)
{
    Complex* p = malloc(sizeof(Complex));
    if (p) memcpy(p, src, sizeof(Complex));
    return p;
}

static void* c_zero(void)
{
    Complex* zero = malloc(sizeof(Complex));
    if (zero) {
        zero->real = 0.0;
        zero->imag = 0.0;
    }
    return zero;
}

static void c_destroy(void* elem) {free(elem);}

static void c_print(const void* elem)
{
    const Complex* c = (const Complex*)elem;
    if (c->imag >= 0)
        printf("%.2lf + %.2lfi", c->real, c->imag);
    else
        printf("%.2lf - %.2lfi", c->real, -c->imag);
}

static void c_sum(const void* a, const void* b, void* res)
{
    const Complex* c1 = (const Complex*)a;
    const Complex* c2 = (const Complex*)b;
    Complex* r = (Complex*)res;
    r->real = c1->real + c2->real;
    r->imag = c1->imag + c2->imag;
}

static void c_mul(const void* a, const void* b, void* res)
{
    const Complex* c1 = (const Complex*)a;
    const Complex* c2 = (const Complex*)b;
    Complex* r = (Complex*)res;
    r->real = c1->real * c2->real - c1->imag * c2->imag;
    r->imag = c1->real * c2->imag + c1->imag * c2->real;
}

static void c_mul_scalar(void* value, const void* scalar, void* res)
{
    const Complex* v = (const Complex*)value;
    const Complex* s = (const Complex*)scalar;
    Complex* r = (Complex*)res;

    double real = v->real * s->real - v->imag * s->imag;
    double imag = v->real * s->imag + v->imag * s->real;

    r->real = real;
    r->imag = imag;
}

static int c_divide(const void* a, const void* b, void* res)
{
    const Complex* dividend = (const Complex*)a;
    const Complex* divisor = (const Complex*)b;
    Complex* result = (Complex*)res;

    double denominator = divisor->real * divisor->real + divisor->imag * divisor->imag;

    if (denominator < EPSILON) {
        return -1;
    }

    result->real = (dividend->real * divisor->real + dividend->imag * divisor->imag) / denominator;
    result->imag = (dividend->imag * divisor->real - dividend->real * divisor->imag) / denominator;

    return 0;
}

static int c_is_zero(const void* a)
{
    const Complex* c = (const Complex*)a;
    return (fabs(c->real) < EPSILON && fabs(c->imag) < EPSILON) ? 1 : 0;
}

static int c_equals(const void* a, const void* b, double eps)
{
    const Complex* c1 = (const Complex*)a;
    const Complex* c2 = (const Complex*)b;

    return (fabs(c1->real - c2->real) < eps &&
            fabs(c1->imag - c2->imag) < eps) ? 1 : 0;
}

static FieldInfo* instance = NULL;

const FieldInfo* get_complex_type_info(void) {
    if (!instance) {
        instance = malloc(sizeof(FieldInfo));
        if (instance) {
            instance->size = sizeof(Complex);
            instance->clone = c_clone;
            instance->zero = c_zero;
            instance->destroy = c_destroy;
            instance->print = c_print;
            instance->sum = c_sum;
            instance->mul = c_mul;
            instance->mul_scalar = c_mul_scalar;
            instance->divide = c_divide;
            instance->is_zero = c_is_zero;
            instance->equals = c_equals;
        }
    }
    return instance;
}