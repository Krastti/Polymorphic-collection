//
// Created by Krastti on 04.03.2026.
//

#ifndef LABORATORYWORK1_MENU_H
#define LABORATORYWORK1_MENU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MENU_TITLE "ЛАБОРАТОРНАЯ РАБОТА №1. ВАРИАНТ 12"
#define MENU_SUBTITLE "Полиморфная коллекция: Квадратная матрица"

#define MENU_EXIT             0
#define MENU_SELECT_TYPE      1
#define MENU_CREATE_MATRIX_A  2
#define MENU_CREATE_MATRIX_B  3
#define MENU_PRINT_MATRIX_A   4
#define MENU_PRINT_MATRIX_B   5
#define MENU_OP_ADD           6
#define MENU_OP_MULTIPLY      7
#define MENU_OP_SCALAR        8
#define MENU_RESET            9

#define INPUT_BUFFER_SIZE 256

void printHeader(void);

void printMainMenu(void);

void printSeparator(void);

void clearInputBuffer(void);

// prompt - текст запроса
// value - указатель на переменную для сохранения результата
int readInt(const char* prompt, int* value);
int readDouble(const char* prompt, double* value);

int menuSelectElementType(void);

// type - тип элементов
// real_out - указатель для вещественной части
// imag_out - указатель для мнимой части
int menuInputScalar(int type, double* real_out, double* imag_out);

void printError(const char* message);

void printInfo(const char* message);

#endif //LABORATORYWORK1_MENU_H