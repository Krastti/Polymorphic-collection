//
// Created by Krastti on 01.03.2026.
//

#ifndef LABORATORYWORK1_DOUBLE_TYPES_H
#define LABORATORYWORK1_DOUBLE_TYPES_H
#include <stddef.h>

typedef struct {
    size_t size;                          // Размер элемента

    void* (*clone)(const void* src);      // Копирование
    void  (*destroy)(void* elem);         // Удаление
    void* (*zero)(void);                  // Создание нейтрального элемента
    void* (*one)(void);

    void (*print)(const void* elem);      // Вывод
    void (*sum)(const void* a, const void* b, void* res);  // Сложение
    void (*mul)(const void* a, const void* b, void* res);  // Умножение
    void (*mul_scalar)(void* a, const void* b, void* res); // Умножение на скаляр

    // Для LU-разложения
    int (*divide)(const void* a, const void* b, void* res);
    int (*is_zero)(const void* a);
    int (*equals)(const void* a, const void* b, double res);
} FieldInfo;

const FieldInfo* get_double_type_info(void);

#endif //LABORATORYWORK1_DOUBLE_TYPES_H