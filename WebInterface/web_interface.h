//
// Created by Krastti on 04.03.2026.
//
#ifndef LABORATORYWORK1_WEB_INTERFACE_H
#define LABORATORYWORK1_WEB_INTERFACE_H

#include <stdbool.h>
#include <stddef.h>

#define HTML_FILENAME_REAL "WebInterface/matrix_interface.html"
#define HTML_FILENAME_COMPLEX "WebInterface/matrix_interface_complex.html"

// element_type - тип элементов
bool web_interface_open(int element_type);

void web_interface_cleanup(void);

#endif //LABORATORYWORK1_WEB_INTERFACE_H