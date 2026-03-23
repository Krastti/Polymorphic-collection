# Matrix Calculator

Выполнил студент первого курса НИЯУ МИФИ - Сальцев Ярослав Б25-507. 
Лабораторная работа по информатике №1 (Вариант 12)

---

## Архитектура

```
Браузер (HTML/CSS/JS)
        │  HTTP
        ▼
FastAPI (Python)          ← маршрутизация, JSON, валидация
        │  ctypes
        ▼
matrix_lib.so / .dll      ← C API (api.c)
        │
        ▼
matrix.c + double_type.c + complex_type.c
```

C-библиотека загружается **один раз** при старте сервера и вызывается напрямую из памяти процесса — без запуска дочерних процессов, без сокетов, без сериализации.

### Структура проекта

```
├── src/
│   ├── api/            
│   ├── matrix/
│   ├── types_info/     
│   ├── logging/        
│   └── main.c          
├── server/
│   ├── main.py         
│   ├── api.py          
│   ├── matrix_lib.py   
│   ├── models.py       
│   ├── config.py
│   └── utils.py
├── web/
│   ├── index.html
│   ├── app.js
│   └── style.css
├── tests/
│   ├── unit_tests.c
│   └── unit_tests.h
├── CMakeLists.txt
└── Dockerfile
```

---

## Запуск

### Docker (рекомендуется)

```bash
docker build -t matrix-calculator .
docker run -p 8000:8000 matrix-calculator
```

Откройте браузер: [http://localhost:8000](http://localhost:8000)

### Сборка вручную

**Требования:** CMake ≥ 3.31, GCC / Clang / MSVC, Python ≥ 3.12

#### 1. Сборка C-библиотеки

```bash
cmake -B cmake-build-debug -DBUILD_TESTS=ON
cmake --build cmake-build-debug
```

После сборки в `cmake-build-debug/` появится `libmatrix_lib.so` (Linux/macOS) или `matrix_lib.dll` (Windows).

#### 2. Запуск Python-сервера

```bash
cd server
pip install -r requirements.txt
python main.py
```

Сервер откроет браузер автоматически. API-документация: [http://localhost:8000/docs](http://localhost:8000/docs)

---

## API

| Метод | Путь | Описание |
|-------|------|----------|
| `GET` | `/health` | Статус сервера и библиотеки |
| `POST` | `/api/sum` | Сложение двух матриц |
| `POST` | `/api/multiply` | Умножение двух матриц |
| `POST` | `/api/scalar_multiply` | Умножение матрицы на скаляр |
| `POST` | `/api/lu` | LU-разложение |

### Пример запроса

```json
POST /api/sum
{
  "n": 2,
  "type": "double",
  "matrixA": [1.0, 2.0, 3.0, 4.0],
  "matrixB": [5.0, 6.0, 7.0, 8.0]
}
```

Поле `type` принимает значения `"double"` или `"complex"`. Для комплексных чисел элементы передаются в формате `"a+bi"`.

---

## Тесты

```bash
./cmake-build-debug/LaboratoryWork1.exe
```

---

## Технологии

| Слой | Технология |
|------|-----------|
| Вычисления | C11, CMake |
| Сервер | Python, FastAPI, uvicorn |
| Связь C↔Python | `ctypes` |
| Фронтенд | HTML, CSS, JavaScript |
| Контейнеризация | Docker |
