import os
import threading
import webbrowser
import atexit
import logging

from fastapi import FastAPI
from fastapi.responses import FileResponse, JSONResponse
from fastapi.middleware.cors import CORSMiddleware
import uvicorn

from config import HOST, PORT, WEB_DIR, LIB_PATH
from api import router

logger = logging.getLogger(__name__)

app = FastAPI(
    title="Matrix Calculator API",
    description="Веб-сервис для операций с матрицами",
    version="2.0.0"
)

# CORS для работы с браузером
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

app.include_router(router)

@app.get("/", tags=["Web"])
async def index():
    """Главная страница"""
    index_path = os.path.join(WEB_DIR, 'index.html')
    if os.path.exists(index_path):
        logger.info(f"Serving index.html: {index_path}")
        logger.info(f"File size: {os.path.getsize(index_path)} bytes")
        return FileResponse(
            index_path,
            headers={
                "Cache-Control": "no-store, no-cache, must-revalidate, max-age=0",
                "Pragma": "no-cache",
                "Expires": "0"
            }
        )
    return JSONResponse(
        status_code=404,
        content={"error": "index.html не найден"}
    )

@app.get("/{path:path}")
async def static_files(path: str):
    """Обслуживание статики с контролем кэша"""
    file_path = os.path.join(WEB_DIR, path)

    if os.path.exists(file_path) and os.path.isfile(file_path):
        if path.endswith(('.html', '.htm')):
            headers = {
                "Cache-Control": "no-store, no-cache, must-revalidate, max-age=0",
                "Pragma": "no-cache",
                "Expires": "0"
            }
        elif path.endswith(('.css', '.js')):
            headers = {
                "Cache-Control": "public, max-age=300, must-revalidate"
            }
        else:
            headers = {
                "Cache-Control": "public, max-age=31536000, immutable"
            }

        return FileResponse(file_path, headers=headers)

    return JSONResponse(status_code=404, content={"error": f"Файл не найден: {path}"})

def open_browser():
    """Открывает браузер после запуска сервера"""
    webbrowser.open(f'http://localhost:{PORT}')


if __name__ == '__main__':
    print('=' * 60)
    print('  Matrix Calculator Web Service (FastAPI)')
    print(f'  Библиотека         : {LIB_PATH}')
    print(f'  Файл найден        : {os.path.isfile(LIB_PATH)}')
    print(f'  Папка web/         : {WEB_DIR}')
    print(f'  Адрес              : http://localhost:{PORT}')
    print(f'  Документация       : http://localhost:{PORT}/docs')
    print('=' * 60)

    # Открываем браузер
    threading.Timer(2.0, open_browser).start()

    # Запускаем сервер
    uvicorn.run(app, host=HOST, port=PORT, log_level='info')