cmake_minimum_required(VERSION 3.31.6)
project(LaboratoryWork1 C)

set(CMAKE_C_STANDARD 11)

option(BUILD_TESTS "Build unit tests" ON)

if(WIN32)
    add_definitions(-DAPI_EXPORTS)
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

target_include_directories(matrix_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/api
        ${CMAKE_CURRENT_SOURCE_DIR}/src/matrix
        ${CMAKE_CURRENT_SOURCE_DIR}/src/types_info
        ${CMAKE_CURRENT_SOURCE_DIR}/src/logging
)

target_compile_definitions(matrix_lib PRIVATE API_EXPORTS)

set_target_properties(matrix_lib PROPERTIES
        C_VISIBILITY_PRESET default
        VISIBILITY_INLINES_HIDDEN OFF
)

if(WIN32)
    set_target_properties(matrix_lib PROPERTIES
            PREFIX ""
            WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif()

if(BUILD_TESTS)
    add_executable(LaboratoryWork1
            src/main.c
            tests/unit_tests.c
            tests/unit_tests.h
            ${LIB_SOURCES}
    )

    target_compile_definitions(LaboratoryWork1 PRIVATE API_EXPORTS)

    target_include_directories(LaboratoryWork1 PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/src/api
            ${CMAKE_CURRENT_SOURCE_DIR}/src/matrix
            ${CMAKE_CURRENT_SOURCE_DIR}/src/types_info
            ${CMAKE_CURRENT_SOURCE_DIR}/src/logging
    )

    if(MINGW)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc")
    endif()
endif()