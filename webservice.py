from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
import subprocess
import os
import sys
import threading
import webbrowser

_BASE_DIR    = os.path.dirname(os.path.abspath(__file__))
_WEB_DIR     = os.path.join(_BASE_DIR, 'web')
_EXE_NAME    = 'LaboratoryWork1.exe' if sys.platform == 'win32' else 'LaboratoryWork1'
_DEFAULT_EXE = os.path.join(_BASE_DIR, 'cmake-build-debug', _EXE_NAME)

MATRIX_EXECUTABLE = os.environ.get('MATRIX_EXE', _DEFAULT_EXE)

app = Flask(__name__, static_folder=_WEB_DIR)
CORS(app)


def open_browser():
    if os.environ.get('WERKZEUG_RUN_MAIN') == 'true':
        return
    webbrowser.open('http://localhost:5000')

def run_matrix_operation(args):
    try:
        result = subprocess.run(
            [MATRIX_EXECUTABLE] + args,
            capture_output=True, text=True, encoding='utf-8', timeout=10
        )
        if result.returncode != 0:
            return {'success': False, 'error': result.stderr.strip() or 'Программа вернула ненулевой код'}
        return {'success': True, 'output': result.stdout.strip()}
    except FileNotFoundError:
        return {'success': False, 'error': f'Исполняемый файл не найден: {MATRIX_EXECUTABLE}'}
    except subprocess.TimeoutExpired:
        return {'success': False, 'error': 'Превышено время ожидания (10 сек)'}
    except Exception as e:
        return {'success': False, 'error': str(e)}


def validate_matrix_request(data, require_two_matrices=True):
    if 'type' not in data or data['type'] not in ('double', 'complex'):
        return False, "Поле 'type' должно быть 'double' или 'complex'"
    if 'n' not in data or not isinstance(data['n'], int) or data['n'] < 1:
        return False, "Поле 'n' должно быть целым положительным числом"
    n = data['n']
    expected = n * n
    if 'matrixA' not in data or len(data['matrixA']) != expected:
        return False, f'matrixA должна содержать ровно {expected} элементов'
    if require_two_matrices:
        if 'matrixB' not in data or len(data['matrixB']) != expected:
            return False, f'matrixB должна содержать ровно {expected} элементов'
    return True, ''


def format_matrix_args(matrix, type_):
    args = []
    if type_ == 'double':
        for val in matrix:
            args.append(str(float(val)))
    else:
        for val in matrix:
            args.append(f"{float(val['real'])},{float(val['imag'])}")
    return args


# ─────────────────────────── API endpoints ────────────────────────────

@app.route('/api/sum', methods=['POST'])
def api_sum():
    data = request.get_json(silent=True) or {}
    ok, err = validate_matrix_request(data, require_two_matrices=True)
    if not ok:
        return jsonify({'success': False, 'error': err}), 400
    args = (['sum', data['type'], str(data['n'])]
            + format_matrix_args(data['matrixA'], data['type'])
            + format_matrix_args(data['matrixB'], data['type']))
    return jsonify(run_matrix_operation(args))


@app.route('/api/multiply', methods=['POST'])
def api_multiply():
    data = request.get_json(silent=True) or {}
    ok, err = validate_matrix_request(data, require_two_matrices=True)
    if not ok:
        return jsonify({'success': False, 'error': err}), 400
    args = (['multiply', data['type'], str(data['n'])]
            + format_matrix_args(data['matrixA'], data['type'])
            + format_matrix_args(data['matrixB'], data['type']))
    return jsonify(run_matrix_operation(args))


@app.route('/api/scalar_multiply', methods=['POST'])
def api_scalar_multiply():
    data = request.get_json(silent=True) or {}
    ok, err = validate_matrix_request(data, require_two_matrices=False)
    if not ok:
        return jsonify({'success': False, 'error': err}), 400
    if 'scalar' not in data:
        return jsonify({'success': False, 'error': "Поле 'scalar' обязательно"}), 400
    if data['type'] == 'double':
        scalar_args = [str(float(data['scalar']))]
    else:
        s = data['scalar']
        scalar_args = [f"{float(s['real'])},{float(s['imag'])}"]
    args = (['scalar_multiply', data['type'], str(data['n'])]
            + format_matrix_args(data['matrixA'], data['type'])
            + scalar_args)
    return jsonify(run_matrix_operation(args))


@app.route('/api/lu', methods=['POST'])
def api_lu():
    data = request.get_json(silent=True) or {}
    ok, err = validate_matrix_request(data, require_two_matrices=False)
    if not ok:
        return jsonify({'success': False, 'error': err}), 400
    args = (['lu', data['type'], str(data['n'])]
            + format_matrix_args(data['matrixA'], data['type']))
    return jsonify(run_matrix_operation(args))


@app.route('/api/health', methods=['GET'])
def api_health():
    exe_exists = os.path.isfile(MATRIX_EXECUTABLE) and os.access(MATRIX_EXECUTABLE, os.X_OK)
    return jsonify({
        'server': 'ok',
        'matrix_executable': MATRIX_EXECUTABLE,
        'executable_found': exe_exists
    })


# ── Статические файлы из папки web/ ───────────────────────────────────

@app.route('/', methods=['GET'])
def index():
    return send_from_directory(_WEB_DIR, 'index.html')

@app.route('/<path:filename>', methods=['GET'])
def static_files(filename):
    return send_from_directory(_WEB_DIR, filename)


if __name__ == '__main__':
    print('=' * 60)
    print('  Matrix Calculator web Service')
    print(f'  Исполняемый файл : {MATRIX_EXECUTABLE}')
    print(f'  Файл найден      : {os.path.isfile(MATRIX_EXECUTABLE)}')
    print(f'  Папка web/       : {_WEB_DIR}')
    print('  Адрес            : http://localhost:5000')
    print('=' * 60)
    threading.Timer(10.0, open_browser).start()
    app.run(host='0.0.0.0', port=5000, debug=False)
