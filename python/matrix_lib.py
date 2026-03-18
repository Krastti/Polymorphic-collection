import ctypes
import os
import threading
from typing import Optional

from config import LIB_PATH
class MatrixLibrary:
    def __init__(self):
        self.lib = None
        self.lock = threading.Lock()
        self._load()
        self._setup()

    def _load(self):
        """Загружает библиотеку"""
        if not os.path.exists(LIB_PATH):
            raise FileNotFoundError(f"Не найдено: {LIB_PATH}")
        self.lib = ctypes.CDLL(LIB_PATH)
        print(f"✓ Загружено: {LIB_PATH}")

    def _setup(self):
        lib = self.lib

        lib.api_create_matrix.argtypes = [ctypes.c_size_t, ctypes.c_int]
        lib.api_create_matrix.restype = ctypes.c_void_p

        lib.api_destroy_matrix.argtypes = [ctypes.c_void_p]

        lib.api_matrix_get.argtypes = [
            ctypes.c_void_p, ctypes.c_size_t, ctypes.c_size_t,
            ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double)
        ]

        lib.api_matrix_sum.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
        lib.api_matrix_sum.restype = ctypes.c_void_p

        lib.api_matrix_multiply.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
        lib.api_matrix_multiply.restype = ctypes.c_void_p

        lib.api_matrix_set.argtypes = [
            ctypes.c_void_p, ctypes.c_size_t, ctypes.c_size_t,
            ctypes.c_double, ctypes.c_double
        ]

        lib.api_matrix_get.argtypes = [
            ctypes.c_void_p, ctypes.c_size_t, ctypes.c_size_t,
            ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double)
        ]

        lib.api_matrix_size.argtypes = [ctypes.c_void_p]
        lib.api_matrix_size.restype = ctypes.c_size_t

    def create_matrix(self, n, type_id):
        with self.lock:
            return self.lib.api_create_matrix(n, type_id)

    def destroy_matrix(self, ptr):
        if ptr:
            with self.lock:
                self.lib.api_destroy_matrix(ptr)

    def get_matrix_data(self, ptr):
        """Извлекает данные матрицы из C в список Python"""
        if not ptr:
            return []

        with self.lock:
            n = self.lib.api_matrix_size(ptr)
            data = []
            for i in range(n):
                for j in range(n):
                    r = ctypes.c_double()
                    im = ctypes.c_double()
                    self.lib.api_matrix_get(ptr, i, j, ctypes.byref(r), ctypes.byref(im))
                    data.append((r.value, im.value))
            return data

    def set_value(self, ptr, row, col, real, imag=0.0):
        with self.lock:
            self.lib.api_matrix_set(ptr, row, col, real, imag)

    def get_value(self, ptr, row, col):
        with self.lock:
            r = ctypes.c_double()
            i = ctypes.c_double()
            self.lib.api_matrix_get(ptr, row, col, ctypes.byref(r), ctypes.byref(i))
            return r.value, i.value

    def sum_matrices(self, m1, m2):
        with self.lock:
            return self.lib.api_matrix_sum(m1, m2)

    def multiply_matrices(self, m1, m2):
        with self.lock:
            return self.lib.api_matrix_multiply(m1, m2)

    def get_size(self, ptr):
        with self.lock:
            return self.lib.api_matrix_size(ptr)

    def cleanup(self):
        """Завершение пула потоков"""
        if hasattr(self, "executor"):
            self.executor.shutdown(wait=True)

matrix_lib: Optional[MatrixLibrary] = None
try:
    matrix_lib = MatrixLibrary()
except FileNotFoundError as e:
    print(f"⚠️  Предупреждение: {e}")
except Exception as e:
    print(f"⚠️  Предупреждение: Ошибка инициализации: {e}")