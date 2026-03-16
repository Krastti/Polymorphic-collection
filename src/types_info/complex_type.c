#include "complex_type.h"
#include "field_info.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Complex make_complex(const double real, const double imag)
{
    Complex c;
    c.real = real;
    c.imag = imag;
    return c;
}

static void* c_clone(const void* src)
{
    Complex* p = malloc(sizeof(Complex));
    if (p) memcpy(p, src, sizeof(Complex));
    return p;
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

static void c_sub(const void* a, const void* b, void* res)
{
    const Complex* c1 = (const Complex*)a;
    const Complex* c2 = (const Complex*)b;
    Complex* r = (Complex*)res;
    r->real = c1->real - c2->real;
    r->imag = c1->imag - c2->imag;
}

static void c_mul(const void* a, const void* b, void* res)
{
    const Complex* c1 = (const Complex*)a;
    const Complex* c2 = (const Complex*)b;
    Complex* r = (Complex*)res;
    r->real = c1->real * c2->real - c1->imag * c2->imag;
    r->imag = c1->real * c2->imag + c1->imag * c2->real;
}

static void c_div(const void* a, const void* b, void* res)
{
    const Complex* c1 = (const Complex*)a;
    const Complex* c2 = (const Complex*)b;
    Complex* r = (Complex*)res;
    double denom = c2->real * c2->real + c2->imag * c2->imag;
    if (denom == 0) return;
    r->real = (c1->real * c2->real + c1->imag * c2->imag) / denom;
    r->imag = (c1->imag * c2->real - c1->real * c2->imag) / denom;
}

static void c_zero(void* res)
{
    Complex* с = (Complex*)res;
    с->real = 0.0;
    с->imag = 0.0;
}

static void c_one(void* res)
{
    Complex* с = (Complex*)res;
    с->real = 1.0;
    с->imag = 0.0;
}

static void c_neg(const void* elem, void* res)
{
    const Complex* c = (const Complex*)elem;
    Complex* r = (Complex*)res;
    r->real = -c->real;
    r->imag = -c->imag;
}

static void c_frac(const void* elem, void* res)
{
    const Complex* c = (const Complex*)elem;
    Complex* r = (Complex*)res;
    double denom = c->real * c->real + c->imag * c->imag;
    if (denom == 0.0) return;
    r->real = c->real / denom;
    r->imag = -c->imag / denom;
}

static FieldInfo* instance = NULL;

const FieldInfo* get_complex_type_info(void)
{
    if (instance == NULL) return instance;

    instance = malloc(sizeof(FieldInfo));
    instance->size = sizeof(Complex);
    instance->clone = c_clone;
    instance->destroy = c_destroy;

    instance->print = c_print;
    instance->sum = c_sum;
    instance->sub = c_sub;
    instance->mul = c_mul;
    instance->div = c_div;

    instance->zero = c_zero;
    instance->one = c_one;

    instance->neg = c_neg;
    instance->frac = c_frac;
    return instance;
}