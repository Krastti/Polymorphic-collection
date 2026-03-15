#ifndef LABORATORYWORK1_FIELD_INFO_H
#define LABORATORYWORK1_FIELD_INFO_H
#include <stddef.h>

typedef struct {
    size_t size;

    void* (*clone)(const void* src);
    void  (*destroy)(void* elem);

    void (*print)(const void* elem);
    void (*sum)(const void* a, const void* b, void* res);
    void (*mul)(const void* a, const void* b, void* res);

    void (*zero)(void* elem);
    void (*one)(void* elem);
} FieldInfo;
#endif //LABORATORYWORK1_FIELD_INFO_H