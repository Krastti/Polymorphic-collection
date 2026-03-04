cmake_minimum_required(VERSION 4.0)
project(LaboratoryWork1 C)

set(CMAKE_C_STANDARD 11)

add_executable(LaboratoryWork1 main.c
        "Типы данных/double_types.c"
        "Типы данных/double_types.h"
        "Типы данных/complex_types.c"
        "Типы данных/complex_types.h"
        Матрицы/matrix.c
        Матрицы/matrix.h
        Тесты/tests.c
        Тесты/tests.h
        Логирование/logger.c
        Логирование/logger.h
        Меню/menu.h
        Меню/menu.c
        LU-разложение/lu_decomposition.h
        LU-разложение/lu_decomposition.c
)
