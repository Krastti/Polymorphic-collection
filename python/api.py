from fastapi import APIRouter, HTTPException
import asyncio
import os
from models import MatrixRequest, MatrixResponse
from matrix_lib import matrix_lib
from utils import parse_matrix_value, format_result_data
from config import MatrixType, LIB_PATH

router = APIRouter()


@router.get("/health", tags=["System"])
async def health_check():
    """Проверка состояния сервера и загруженности библиотеки"""
    lib_loaded = matrix_lib is not None and matrix_lib.lib is not None
    lib_exists = os.path.exists(LIB_PATH)
    return {
        "status": "ok" if lib_loaded else "degraded",
        "server": "running",
        "library": {
            "loaded": lib_loaded,
            "path": LIB_PATH,
            "exists": lib_exists
        },
        "version": "2.0.0"
    }


async def run_in_executor(func, *args):
    """Запускает функцию в пуле потоков для неблокирующего выполнения"""
    loop = asyncio.get_event_loop()
    return await loop.run_in_executor(None, func, *args)


@router.post("/api/sum", response_model=MatrixResponse, tags=["Operations"])
async def api_sum(request: MatrixRequest):
    ok, err = request.check_size()
    if not ok:
        raise HTTPException(status_code=400, detail=err)
    if request.matrixB is None:
        raise HTTPException(status_code=400, detail="matrixB обязательна для операции сложения")

    try:
        n = request.n
        matrix_type = MatrixType.from_string(request.type)

        m1 = await run_in_executor(matrix_lib.create_matrix, n, matrix_type)
        m2 = await run_in_executor(matrix_lib.create_matrix, n, matrix_type)

        if not m1 or not m2:
            raise HTTPException(status_code=500, detail="Не удалось создать матрицы")

        for i in range(n):
            for j in range(n):
                idx = i * n + j
                r1, i1 = parse_matrix_value(request.matrixA[idx], request.type)
                r2, i2 = parse_matrix_value(request.matrixB[idx], request.type)
                await run_in_executor(matrix_lib.set_value, m1, i, j, r1, i1)
                await run_in_executor(matrix_lib.set_value, m2, i, j, r2, i2)

        result_ptr = await run_in_executor(matrix_lib.sum_matrices, m1, m2)

        await run_in_executor(matrix_lib.destroy_matrix, m1)
        await run_in_executor(matrix_lib.destroy_matrix, m2)

        if not result_ptr:
            raise HTTPException(status_code=500, detail="Ошибка сложения матриц")

        result_data = await run_in_executor(matrix_lib.get_matrix_data, result_ptr)
        await run_in_executor(matrix_lib.destroy_matrix, result_ptr)

        return MatrixResponse(
            success=True,
            operation="sum",
            type=request.type,
            size=n,
            result=format_result_data(result_data, request.type)
        )

    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


@router.post("/api/multiply", response_model=MatrixResponse, tags=["Operations"])
async def api_multiply(request: MatrixRequest):
    ok, err = request.check_size()
    if not ok:
        raise HTTPException(status_code=400, detail=err)
    if request.matrixB is None:
        raise HTTPException(status_code=400, detail="matrixB обязательна для операции умножения")

    try:
        n = request.n
        matrix_type = MatrixType.from_string(request.type)

        m1 = await run_in_executor(matrix_lib.create_matrix, n, matrix_type)
        m2 = await run_in_executor(matrix_lib.create_matrix, n, matrix_type)

        if not m1 or not m2:
            raise HTTPException(status_code=500, detail="Не удалось создать матрицы")

        for i in range(n):
            for j in range(n):
                idx = i * n + j
                r1, i1 = parse_matrix_value(request.matrixA[idx], request.type)
                r2, i2 = parse_matrix_value(request.matrixB[idx], request.type)
                await run_in_executor(matrix_lib.set_value, m1, i, j, r1, i1)
                await run_in_executor(matrix_lib.set_value, m2, i, j, r2, i2)

        result_ptr = await run_in_executor(matrix_lib.multiply_matrices, m1, m2)

        await run_in_executor(matrix_lib.destroy_matrix, m1)
        await run_in_executor(matrix_lib.destroy_matrix, m2)

        if not result_ptr:
            raise HTTPException(status_code=500, detail="Ошибка умножения матриц")

        result_data = await run_in_executor(matrix_lib.get_matrix_data, result_ptr)
        await run_in_executor(matrix_lib.destroy_matrix, result_ptr)

        return MatrixResponse(
            success=True,
            operation="multiply",
            type=request.type,
            size=n,
            result=format_result_data(result_data, request.type)
        )

    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


@router.post("/api/scalar_multiply", response_model=MatrixResponse, tags=["Operations"])
async def api_scalar_multiply(request: MatrixRequest):
    ok, err = request.check_size()
    if not ok:
        raise HTTPException(status_code=400, detail=err)
    if request.scalar is None:
        raise HTTPException(status_code=400, detail="Поле 'scalar' обязательно")

    try:
        n = request.n
        matrix_type = MatrixType.from_string(request.type)

        sr, si = parse_matrix_value(request.scalar, request.type)

        m1 = await run_in_executor(matrix_lib.create_matrix, n, matrix_type)

        if not m1:
            raise HTTPException(status_code=500, detail="Не удалось создать матрицу")

        for i in range(n):
            for j in range(n):
                idx = i * n + j
                r, im = parse_matrix_value(request.matrixA[idx], request.type)
                await run_in_executor(matrix_lib.set_value, m1, i, j, r, im)

        result_ptr = await run_in_executor(
            matrix_lib.scalar_multiply_matrix, m1, sr, si
        )

        await run_in_executor(matrix_lib.destroy_matrix, m1)

        if not result_ptr:
            raise HTTPException(status_code=500, detail="Ошибка умножения на скаляр")

        result_data = await run_in_executor(matrix_lib.get_matrix_data, result_ptr)
        await run_in_executor(matrix_lib.destroy_matrix, result_ptr)

        return MatrixResponse(
            success=True,
            operation="scalar_multiply",
            type=request.type,
            size=n,
            result=format_result_data(result_data, request.type)
        )

    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))