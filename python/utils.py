from typing import Any, List, Tuple

def parse_matrix_value(value: Any, matrix_type: str) -> Tuple[float, float]:
    if matrix_type == 'double':
        if isinstance(value, dict):
            return float(value.get('real', 0)), 0.0
        return float(value), 0.0
    else:
        if isinstance(value, dict):
            return float(value.get('real', 0)), float(value.get('imag', 0))
        elif isinstance(value, (list, tuple)) and len(value) >= 2:
            return float(value[0]), float(value[1])
        else:
            return float(value), 0.0

def format_result_data(data: List[Tuple[float, float]], matrix_type: str) -> List[Any]:
    """
    Форматирует результат для JSON ответа
    data: Плоский список кортежей (real, imag)
    n: размер матрицы
    """
    if matrix_type == 'double':
        return [r for r, i in data]
    else:
        return [{'real': r, 'imag': i} for r, i in data]