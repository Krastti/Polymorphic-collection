cmake_minimum_required(VERSION 4.0)
project(LaboratoryWork1 C)

set(CMAKE_C_STANDARD 11)

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc")
endif()

add_executable(LaboratoryWork1 main.c
        "src/types_info/double_type.c"
        "src/types_info/double_type.h"
        "src/types_info/complex_type.c"
        "src/types_info/complex_type.h"
        "src/types_info/field_info.h"
        src/matrix/matrix.c
        src/matrix/matrix.h
        tests/unit_tests.c
        tests/unit_tests.h
        src/logging/logger.c
        src/logging/logger.h
)
