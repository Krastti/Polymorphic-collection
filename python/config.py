import os
import sys

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(BASE_DIR)
WEB_DIR = os.path.join(PROJECT_ROOT, 'web')

LIB_NAME = 'matrix_lib.dll' if sys.platform == 'win32' else 'libmatrix_lib.so'
LIB_PATH = os.path.join(PROJECT_ROOT, 'cmake-build-debug', LIB_NAME)
MATRIX_LIBRARY_PATH = os.environ.get('MATRIX_LIB', LIB_PATH)

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