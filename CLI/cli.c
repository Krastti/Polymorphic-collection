#include "cli.h"
#include "../Matrix/matrix.h"
#include "../Типы данных/double_types.h"
#include "../Типы данных/complex_types.h"
#include "../LU-разложение/lu_decomposition.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
   Использование:
     ./matrix <operation> <type> <n> <elements...>

   Операции:
     sum             <type> <n> <matrixA n*n> <matrixB n*n>
     multiply        <type> <n> <matrixA n*n> <matrixB n*n>
     scalar_multiply <type> <n> <matrixA n*n> <scalar>
     lu              <type> <n> <matrixA n*n>

   Типы:   double | complex
*/

static double cli_parse_double(const char* s) {
    return atof(s);
}

static void cli_parse_complex(const char* s, double* real, double* imag) {
    *real = 0.0;
    *imag = 0.0;
    const char* comma = strchr(s, ',');
    if (comma) {
        char buf[64];
        size_t len = (size_t)(comma - s);
        if (len >= sizeof(buf)) len = sizeof(buf) - 1;
        memcpy(buf, s, len);
        buf[len] = '\0';
        *real = atof(buf);
        *imag = atof(comma + 1);
    } else {
        *real = atof(s);
    }
}

static Matrix* cli_load_matrix(const FieldInfo* type, size_t n,
                                const char* type_str,
                                char** argv, int start) {
    Matrix* m = matrix_create(n, type);
    if (!m) return NULL;

    for (size_t i = 0; i < n * n; i++) {
        if (strcmp(type_str, "double") == 0) {
            double val = cli_parse_double(argv[start + (int)i]);
            matrix_set(m, i / n, i % n, &val);
        } else {
            double real, imag;
            cli_parse_complex(argv[start + (int)i], &real, &imag);
            Complex c = {real, imag};
            matrix_set(m, i / n, i % n, &c);
        }
    }
    return m;
}

static void cli_print_usage(const char* exe) {
    fprintf(stderr,
        "Использование: %s <operation> <type> <n> <elements...>\n"
        "\n"
        "  Операции:\n"
        "    sum             <type> <n> <A n*n> <B n*n>\n"
        "    multiply        <type> <n> <A n*n> <B n*n>\n"
        "    scalar_multiply <type> <n> <A n*n> <scalar>\n"
        "    lu              <type> <n> <A n*n>\n"
        "\n"
        "  Типы:    double | complex\n"
        "  double:  3.14\n"
        "  complex: 3.14,2.0\n",
        exe);
}

int cli_run(int argc, char* argv[]) {
    if (argc < 5) {
        cli_print_usage(argv[0]);
        return 1;
    }

    const char* operation = argv[1];
    const char* type_str  = argv[2];
    int         n_int     = atoi(argv[3]);

    if (n_int < 1) {
        fprintf(stderr, "Ошибка: n должно быть >= 1\n");
        return 1;
    }
    size_t n = (size_t)n_int;

    const FieldInfo* type = NULL;
    if (strcmp(type_str, "double") == 0)
        type = get_double_type_info();
    else if (strcmp(type_str, "complex") == 0)
        type = get_complex_type_info();
    else {
        fprintf(stderr, "Ошибка: неизвестный тип '%s' (ожидается double или complex)\n", type_str);
        return 1;
    }

    const int data_start     = 4;
    const int one_matrix_len = (int)(n * n);
    const int two_matrix_len = (int)(n * n * 2);

    /* ── sum / multiply ── */
    if (strcmp(operation, "sum") == 0 || strcmp(operation, "multiply") == 0) {
        if (argc < data_start + two_matrix_len) {
            fprintf(stderr, "Ошибка: недостаточно аргументов для двух матриц %zux%zu\n", n, n);
            return 1;
        }

        Matrix* A = cli_load_matrix(type, n, type_str, argv, data_start);
        Matrix* B = cli_load_matrix(type, n, type_str, argv, data_start + one_matrix_len);
        if (!A || !B) {
            matrix_destroy(A);
            matrix_destroy(B);
            return 1;
        }

        Matrix* R = (strcmp(operation, "sum") == 0)
                    ? matrix_sum(A, B)
                    : matrix_multiply(A, B);

        int exit_code = 0;
        if (!R) {
            fprintf(stderr, "Ошибка: операция '%s' завершилась с ошибкой\n", operation);
            exit_code = 1;
        } else {
            matrix_print(R);
            matrix_destroy(R);
        }

        matrix_destroy(A);
        matrix_destroy(B);
        return exit_code;
    }

    /* ── scalar_multiply ── */
    if (strcmp(operation, "scalar_multiply") == 0) {
        if (argc < data_start + one_matrix_len + 1) {
            fprintf(stderr, "Ошибка: недостаточно аргументов (нужен скаляр)\n");
            return 1;
        }

        Matrix* A = cli_load_matrix(type, n, type_str, argv, data_start);
        if (!A) return 1;

        const char* scalar_str = argv[data_start + one_matrix_len];

        if (strcmp(type_str, "double") == 0) {
            double scalar = cli_parse_double(scalar_str);
            matrix_scalar_multiply(A, &scalar);
        } else {
            double sr, si;
            cli_parse_complex(scalar_str, &sr, &si);
            Complex scalar = {sr, si};
            matrix_scalar_multiply(A, &scalar);
        }

        matrix_print(A);
        matrix_destroy(A);
        return 0;
    }

    /* ── lu ── */
    if (strcmp(operation, "lu") == 0) {
        if (argc < data_start + one_matrix_len) {
            fprintf(stderr, "Ошибка: недостаточно аргументов для матрицы %zux%zu\n", n, n);
            return 1;
        }

        Matrix* A = cli_load_matrix(type, n, type_str, argv, data_start);
        if (!A) return 1;

        Matrix* L = NULL;
        Matrix* U = NULL;
        int status = matrix_lu_decompose(A, &L, &U);

        int exit_code = 0;
        if (status != LU_SUCCESS) {
            fprintf(stderr, "Ошибка: LU-разложение завершилось с кодом %d\n", status);
            exit_code = 1;
        } else {
            printf("L:\n"); matrix_print(L);
            printf("U:\n"); matrix_print(U);
            matrix_destroy(L);
            matrix_destroy(U);
        }

        matrix_destroy(A);
        return exit_code;
    }

    fprintf(stderr, "Ошибка: неизвестная операция '%s'\n", operation);
    cli_print_usage(argv[0]);
    return 1;
}
