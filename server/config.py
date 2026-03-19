import os
import sys

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(BASE_DIR)
WEB_DIR = os.path.join(PROJECT_ROOT, 'web')

LIB_NAME = 'matrix_lib.dll' if sys.platform == 'win32' else 'libmatrix_lib.so'

def _find_lib() -> str:
    """
    Ищет библиотеку по приоритету:
      1. Переменная окружения MATRIX_LIB (явный путь к файлу)
      2. Стандартная CMake-директория build/lib/
      3. Список fallback-директорий (CI, локальная сборка, Docker)
    """
    # 1. Явный путь через env — наивысший приоритет
    env_path = os.environ.get('MATRIX_LIB')
    if env_path:
        return env_path

    # 2. Стандартный CMake out-of-source build
    candidates = [
        os.path.join(PROJECT_ROOT, 'build', 'lib', LIB_NAME),
        os.path.join(PROJECT_ROOT, 'build', LIB_NAME),
        os.path.join(PROJECT_ROOT, 'cmake-build-release', LIB_NAME),
        os.path.join(PROJECT_ROOT, 'cmake-build-debug', LIB_NAME),
        os.path.join('/usr', 'local', 'lib', LIB_NAME),
        os.path.join('/app', 'lib', LIB_NAME),
    ]

    for path in candidates:
        if os.path.exists(path):
            return path

    return os.path.join(PROJECT_ROOT, 'build', LIB_NAME)

LIB_PATH = _find_lib()

HOST = '0.0.0.0'
PORT = 8000
DEBUG = True

MAX_MATRIX_SIZE = 500

class MatrixType:
    DOUBLE = 0
    COMPLEX = 1

    @classmethod
    def from_string(cls, type_str: str) -> int:
        return cls.COMPLEX if type_str == 'complex' else cls.DOUBLE

    @classmethod
    def to_string(cls, type_id: int) -> str:
        return 'complex' if type_id == cls.COMPLEX else 'double'

LOG_LEVEL = 'info'