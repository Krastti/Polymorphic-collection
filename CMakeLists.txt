cmake_minimum_required(VERSION 4.0)
project(LaboratoryWork1 C)

set(CMAKE_C_STANDARD 11)

if(WIN32)
    add_definitions(-D_API_EXPORTS)
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

set(CMAKE_C_VISIBILITY_PRESET default)
set(CMAKE_VISIBILITY_INLINES_HIDDEN OFF)

set(LIB_SOURCES
        src/api/api.c
        src/api/api.h
        src/matrix/matrix.c
        src/matrix/matrix.h
        src/types_info/double_type.c
        src/types_info/double_type.h
        src/types_info/complex_type.c
        src/types_info/complex_type.h
        src/types_info/field_info.h
        src/logging/logger.c
        src/logging/logger.h
)

add_library(matrix_lib SHARED ${LIB_SOURCES})

add_executable(LaboratoryWork1 src/main.c tests/unit_tests.c tests/unit_tests.h ${LIB_SOURCES})

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc")
endif()