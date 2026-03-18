#ifndef LABORATORYWORK1_COMPLEX_TYPES_H
#define LABORATORYWORK1_COMPLEX_TYPES_H
#include "field_info.h"

typedef struct {
    double real;
    double imag;
} Complex;

Complex make_complex(const double real, const double imag);

const FieldInfo* get_complex_type_info(void);
#endif //LABORATORYWORK1_COMPLEX_TYPES_H