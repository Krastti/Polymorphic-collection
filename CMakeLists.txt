cmake_minimum_required(VERSION 4.0)
project(LaboratoryWork1 C)

set(CMAKE_C_STANDARD 11)

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc")
endif()

add_executable(LaboratoryWork1 main.c
        "Типы данных/double_types.c"
        "Типы данных/double_types.h"
        "Типы данных/complex_types.c"
        "Типы данных/complex_types.h"
        Matrix/matrix.c
        Matrix/matrix.h
        Tests/unit_tests.c
        Tests/unit_tests.h
        Логирование/logger.c
        Логирование/logger.h
        LU-разложение/lu_decomposition.h
        LU-разложение/lu_decomposition.c
        LU-разложение/lu_benchmark.h
        LU-разложение/lu_benchmark.c
        LU-разложение/lu_benchmark_plot.c
        Tests/benchmark_tests.c
        Tests/benchmark_tests.h
        CLI/cli.c
)
