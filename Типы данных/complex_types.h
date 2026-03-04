//
// Created by Krastti on 01.03.2026.
//

#ifndef LABORATORYWORK1_COMPLEX_TYPES_H
#define LABORATORYWORK1_COMPLEX_TYPES_H
#include "double_types.h"
typedef struct
{
    double real;
    double imag;
} Complex;

const FieldInfo* get_complex_type_info(void);
#endif //LABORATORYWORK1_COMPLEX_TYPES_H