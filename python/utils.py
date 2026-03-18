from typing import Any, List, Tuple


def parse_matrix_value(value: Any, matrix_type: str) -> Tuple[float, float]:
    """
    Парсит значение элемента матрицы

    Args:
        value: Число, dict {'real': ..., 'imag': ...}, или список [real, imag]
        matrix_type: 'double' или 'complex'

    Returns:
        Кортеж (real, imag)
    """
    if matrix_type == 'double':
        return float(value), 0.0
    else:  # complex
        if isinstance(value, dict):
            return float(value.get('real', 0)), float(value.get('imag', 0))
        elif isinstance(value, (list, tuple)) and len(value) >= 2:
            return float(value[0]), float(value[1])
        else:
            return float(value), 0.0


def format_result_data(data: List[List[Tuple[float, float]]], matrix_type: str) -> List[Any]:
    """
    Форматирует результат для JSON ответа
    Args:
        data: Матрица как список кортежей (real, imag)
        matrix_type: 'double' или 'complex'

    Returns:
        Отформатированные данные для JSON
    """

    if matrix_type == 'double':
        return [r for r, i in data]
    else:
        return [{'real': r, 'imag': i} for r, i in data]