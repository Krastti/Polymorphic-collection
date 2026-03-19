import ctypes
import os
import threading
from typing import Optional
from config import LIB_PATH, MatrixType


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
        """Настраивает типы аргументов и возвращаемые значения для функций C API"""
        if not self.lib:
            return

        lib = self.lib

        # api_create_matrix
        lib.api_create_matrix.argtypes = [ctypes.c_size_t, ctypes.c_int]
        lib.api_create_matrix.restype = ctypes.c_void_p

        # api_destroy_matrix
        lib.api_destroy_matrix.argtypes = [ctypes.c_void_p]
        lib.api_destroy_matrix.restype = None

        # api_matrix_sum
        lib.api_matrix_sum.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
        lib.api_matrix_sum.restype = ctypes.c_void_p

        # api_matrix_multiply
        lib.api_matrix_multiply.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
        lib.api_matrix_multiply.restype = ctypes.c_void_p

        # api_matrix_multiply_by_scalar (скалярное умножение)
        lib.api_matrix_multiply_by_scalar.argtypes = [
            ctypes.c_void_p, ctypes.c_double, ctypes.c_double
        ]
        lib.api_matrix_multiply_by_scalar.restype = ctypes.c_void_p

        # api_matrix_set
        lib.api_matrix_set.argtypes = [
            ctypes.c_void_p,
            ctypes.c_size_t,
            ctypes.c_size_t,
            ctypes.c_double,
            ctypes.c_double
        ]
        lib.api_matrix_set.restype = None

        # api_matrix_get
        lib.api_matrix_get.argtypes = [
            ctypes.c_void_p,
            ctypes.c_size_t,
            ctypes.c_size_t,
            ctypes.POINTER(ctypes.c_double),
            ctypes.POINTER(ctypes.c_double)
        ]
        lib.api_matrix_get.restype = None

        # api_matrix_size
        lib.api_matrix_size.argtypes = [ctypes.c_void_p]
        lib.api_matrix_size.restype = ctypes.c_size_t

        # api_matrix_type
        lib.api_matrix_type.argtypes = [ctypes.c_void_p]
        lib.api_matrix_type.restype = ctypes.c_int

    def create_matrix(self, n: int, matrix_type: MatrixType) -> Optional[ctypes.c_void_p]:
        """Создаёт матрицу в C библиотеке"""
        with self.lock:
            if not self.lib:
                return None
            type_id = 0 if matrix_type == MatrixType.DOUBLE else 1
            return self.lib.api_create_matrix(n, type_id)

    def destroy_matrix(self, ptr: ctypes.c_void_p) -> None:
        """Уничтожает матрицу в C библиотеке"""
        if ptr:
            with self.lock:
                if self.lib:
                    self.lib.api_destroy_matrix(ptr)

    def get_matrix_data(self, ptr: ctypes.c_void_p) -> list:
        """Извлекает данные матрицы из C в список Python"""
        if not ptr or not self.lib:
            return []

        with self.lock:
            n = self.lib.api_matrix_size(ptr)
            data = []
            for i in range(n):
                for j in range(n):
                    r = ctypes.c_double()
                    im = ctypes.c_double()
                    self.lib.api_matrix_get(
                        ptr, i, j, ctypes.byref(r), ctypes.byref(im)
                    )
                    data.append((r.value, im.value))
            return data

    def set_value(
        self, ptr: ctypes.c_void_p, row: int, col: int, real: float, imag: float = 0.0
    ) -> None:
        """Устанавливает значение элемента матрицы"""
        with self.lock:
            if self.lib and ptr:
                self.lib.api_matrix_set(ptr, row, col, real, imag)

    def get_value(
        self, ptr: ctypes.c_void_p, row: int, col: int
    ) -> tuple[float, float]:
        """Получает значение элемента матрицы"""
        with self.lock:
            if not self.lib or not ptr:
                return (0.0, 0.0)
            r = ctypes.c_double()
            im = ctypes.c_double()
            self.lib.api_matrix_get(
                ptr, row, col, ctypes.byref(r), ctypes.byref(im)
            )
            return (r.value, im.value)

    def sum_matrices(
        self, m1: ctypes.c_void_p, m2: ctypes.c_void_p
    ) -> Optional[ctypes.c_void_p]:
        """Складывает две матрицы"""
        with self.lock:
            if not self.lib or not m1 or not m2:
                return None
            return self.lib.api_matrix_sum(m1, m2)

    def multiply_matrices(
        self, m1: ctypes.c_void_p, m2: ctypes.c_void_p
    ) -> Optional[ctypes.c_void_p]:
        """Умножает две матрицы"""
        with self.lock:
            if not self.lib or not m1 or not m2:
                return None
            return self.lib.api_matrix_multiply(m1, m2)

    def scalar_multiply_matrix(
        self, m: ctypes.c_void_p, scalar_real: float, scalar_imag: float
    ) -> Optional[ctypes.c_void_p]:
        """Умножает матрицу на скаляр"""
        with self.lock:
            if not self.lib or not m:
                return None
            return self.lib.api_matrix_multiply_by_scalar(m, scalar_real, scalar_imag)

    def get_size(self, ptr: ctypes.c_void_p) -> int:
        """Получает размер матрицы"""
        with self.lock:
            if not self.lib or not ptr:
                return 0
            return self.lib.api_matrix_size(ptr)

    def get_type(self, ptr: ctypes.c_void_p) -> int:
        """Получает тип матрицы (0 = double, 1 = complex)"""
        with self.lock:
            if not self.lib or not ptr:
                return -1
            return self.lib.api_matrix_type(ptr)

    def cleanup(self):
        """Освобождает ресурсы"""
        pass


# Глобальный экземпляр библиотеки
matrix_lib: Optional[MatrixLibrary] = None

try:
    matrix_lib = MatrixLibrary()
except FileNotFoundError as e:
    print(f"⚠️ Предупреждение: {e}")
except Exception as e:
    print(f"⚠️ Предупреждение: Ошибка инициализации: {e}")