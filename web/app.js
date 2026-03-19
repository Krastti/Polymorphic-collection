// Конфигурация
const API_BASE = '';
let currentSize = 3;
let currentType = 'double';
let currentOperation = 'sum';

// Элементы DOM
const elements = {
    matrixType: document.getElementById('matrix-type'),
    matrixSize: document.getElementById('matrix-size'),
    sizeDisplay: document.getElementById('size-display'),
    sizeIncrease: document.getElementById('size-increase'),
    sizeDecrease: document.getElementById('size-decrease'),
    operationCards: document.querySelectorAll('.op-btn'),
    operationSymbol: document.getElementById('operation-symbol'),
    scalarSection: document.getElementById('scalar-section'),
    scalarValue: document.getElementById('scalar-value'),
    matrixAGrid: document.getElementById('matrix-a-grid'),
    matrixBGrid: document.getElementById('matrix-b-grid'),
    matrixBHint: document.getElementById('matrix-b-hint'),
    addMatrixB: document.getElementById('add-matrix-b'),
    applySettings: document.getElementById('apply-settings'),
    calculateBtn: document.getElementById('calculate-btn'),
    resultGrid: document.getElementById('result-grid'),
    resultInfo: document.getElementById('result-info'),
    resultPlaceholder: document.getElementById('result-placeholder'),
    resultOutput: document.getElementById('result-output'),
    errorMessage: document.getElementById('error-message'),
    statusIndicator: document.getElementById('status-indicator'),
    statusDot: document.getElementById('status-dot'),
    statusText: document.getElementById('status-text'),
    loadingSpinner: document.getElementById('loading-spinner')
};

// Инициализация
document.addEventListener('DOMContentLoaded', () => {
    checkHealth();
    setupEventListeners();
    applySettings();
});

// Проверка здоровья API
async function checkHealth() {
    try {
        const response = await fetch(`${API_BASE}/health`);
        const data = await response.json();

        if (data.status === 'ok') {
            elements.statusDot.className = 'status-dot connected';
            elements.statusText.textContent = 'Сервер готов';
        } else {
            elements.statusDot.className = 'status-dot error';
            elements.statusText.textContent = 'Ограниченный режим';
        }
    } catch (error) {
        elements.statusDot.className = 'status-dot error';
        elements.statusText.textContent = 'Сервер недоступен';
        console.error('Health check failed:', error);
    }
}

// Настройка обработчиков событий
function setupEventListeners() {
    elements.applySettings.addEventListener('click', applySettings);
    elements.calculateBtn.addEventListener('click', calculate);
    elements.addMatrixB.addEventListener('click', () => fillRandom());

    elements.sizeIncrease.addEventListener('click', () => {
        const current = parseInt(elements.matrixSize.value) || 3;
        if (current < 10) {
            elements.matrixSize.value = current + 1;
            elements.sizeDisplay.textContent = current + 1;
            applySettings();
        }
    });

    elements.sizeDecrease.addEventListener('click', () => {
        const current = parseInt(elements.matrixSize.value) || 3;
        if (current > 1) {
            elements.matrixSize.value = current - 1;
            elements.sizeDisplay.textContent = current - 1;
            applySettings();
        }
    });

    elements.matrixType.addEventListener('change', () => {
        currentType = elements.matrixType.value;
        applySettings();
    });

    elements.operationCards.forEach(card => {
        card.addEventListener('click', () => {
            elements.operationCards.forEach(c => c.classList.remove('active'));
            card.classList.add('active');
            currentOperation = card.dataset.operation;
            updateOperationUI();
        });
    });
}

// Обновление UI в зависимости от операции
function updateOperationUI() {
    const symbols = {
        'sum': '+',
        'multiply': '×',
        'scalar_multiply': 'λ',
        'lu': '∇'
    };

    elements.operationSymbol.textContent = symbols[currentOperation] || '+';

    if (currentOperation === 'scalar_multiply') {
        elements.scalarSection.style.display = 'block';
        elements.matrixBGrid.parentElement.style.opacity = '0.5';
        elements.matrixBHint.textContent = 'не используется';
    } else if (currentOperation === 'lu') {
        elements.scalarSection.style.display = 'none';
        elements.matrixBGrid.parentElement.style.opacity = '0.5';
        elements.matrixBHint.textContent = 'не нужна для LU';
    } else {
        elements.scalarSection.style.display = 'none';
        elements.matrixBGrid.parentElement.style.opacity = '1';
        elements.matrixBHint.textContent = '';
    }
}

// Применение настроек
function applySettings() {
    currentSize = parseInt(elements.matrixSize.value) || 3;
    currentType = elements.matrixType.value;

    if (currentSize < 1) currentSize = 1;
    if (currentSize > 10) currentSize = 10;

    elements.sizeDisplay.textContent = currentSize;

    createMatrixGrid(elements.matrixAGrid, currentSize, 'A');
    createMatrixGrid(elements.matrixBGrid, currentSize, 'B');

    elements.resultGrid.style.display = 'none';
    elements.resultGrid.innerHTML = '';
    elements.resultInfo.innerHTML = '';
    elements.resultPlaceholder.style.display = 'block';
    elements.resultOutput.style.borderStyle = 'dashed';
    hideError();

    updateOperationUI();
}

// Создание сетки матрицы
function createMatrixGrid(container, size, label) {
    container.innerHTML = '';
    container.style.gridTemplateColumns = `repeat(${size}, 1fr)`;

    for (let i = 0; i < size * size; i++) {
        const input = document.createElement('input');
        input.type = 'text';
        input.className = 'matrix-cell';
        input.dataset.index = i;
        input.dataset.matrix = label;

        if (currentType === 'complex') {
            input.placeholder = 'a, b';
        } else {
            input.placeholder = '0';
        }

        container.appendChild(input);
    }
}

// Получение данных матрицы из inputs
function getMatrixData(matrixLabel) {
    const inputs = document.querySelectorAll(`.matrix-cell[data-matrix="${matrixLabel}"]`);
    const data = [];

    inputs.forEach(input => {
        const value = input.value.trim();
        if (currentType === 'complex') {
            data.push(parseComplexValue(value));
        } else {
            data.push(parseFloat(value) || 0);
        }
    });

    return data;
}

// Парсинг комплексного значения
function parseComplexValue(value) {
    if (!value) return { real: 0, imag: 0 };

    // Формат: {real, imag}
    if (value.includes('{') || value.includes('real')) {
        try {
            const obj = JSON.parse(value.replace(/'/g, '"'));
            return {
                real: parseFloat(obj.real) || 0,
                imag: parseFloat(obj.imag) || 0
            };
        } catch {
            return { real: 0, imag: 0 };
        }
    }

    // Формат: real, imag
    const parts = value.split(',').map(v => parseFloat(v.trim()) || 0);
    return {
        real: parts[0] || 0,
        imag: parts[1] || 0
    };
}

// Заполнение случайными значениями
function fillRandom() {
    const inputs = document.querySelectorAll('.matrix-cell');
    inputs.forEach(input => {
        if (currentType === 'complex') {
            const real = (Math.random() * 10 - 5).toFixed(2);
            const imag = (Math.random() * 10 - 5).toFixed(2);
            input.value = `${real}, ${imag}`;
        } else {
            input.value = (Math.random() * 10 - 5).toFixed(2);
        }
    });
}

// Выполнение вычислений
async function calculate() {
    const matrixA = getMatrixData('A');
    const matrixB = currentOperation !== 'scalar_multiply' && currentOperation !== 'lu' ? getMatrixData('B') : null;
    const scalar = currentOperation === 'scalar_multiply' ? elements.scalarValue.value : null;

    // Валидация
    if (currentOperation !== 'scalar_multiply' && currentOperation !== 'lu' && !matrixB) {
        showError('Матрица B обязательна для этой операции');
        return;
    }

    if (currentOperation === 'scalar_multiply' && !scalar) {
        showError('Скаляр обязателен для этой операции');
        return;
    }

    // Подготовка запроса
    const requestData = {
        type: currentType,
        n: currentSize,
        matrixA: matrixA
    };

    if (matrixB) {
        requestData.matrixB = matrixB;
    }

    if (scalar) {
        requestData.scalar = parseComplexValue(scalar);
    }

    // Определение endpoint
    let endpoint = '';
    switch (currentOperation) {
        case 'sum':
            endpoint = '/api/sum';
            break;
        case 'multiply':
            endpoint = '/api/multiply';
            break;
        case 'scalar_multiply':
            endpoint = '/api/scalar_multiply';
            break;
        case 'lu':
            endpoint = '/api/lu';
            break;
    }

    try {
        setLoading(true);
        hideError();

        const response = await fetch(`${API_BASE}${endpoint}`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(requestData)
        });

        const result = await response.json();

        if (!response.ok) {
            throw new Error(result.detail || 'Ошибка вычисления');
        }

        if (result.success) {
            displayResult(result);
        } else {
            showError(result.error || 'Неизвестная ошибка');
        }

    } catch (error) {
        showError(error.message);
        console.error('Calculation error:', error);
    } finally {
        setLoading(false);
    }
}

// Отображение результата
function displayResult(result) {
    elements.resultPlaceholder.style.display = 'none';
    elements.resultGrid.style.display = 'grid';
    elements.resultOutput.style.borderStyle = 'solid';

    elements.resultInfo.innerHTML = `
        <div class="result-info-item">
            <strong>Операция:</strong> ${getOperationName(result.operation)}
        </div>
        <div class="result-info-item">
            <strong>Тип:</strong> ${result.type}
        </div>
        <div class="result-info-item">
            <strong>Размер:</strong> ${result.size} × ${result.size}
        </div>
    `;

    // Создание сетки результата
    elements.resultGrid.innerHTML = '';
    elements.resultGrid.style.gridTemplateColumns = `repeat(${result.size}, 1fr)`;

    result.result.forEach(value => {
        const cell = document.createElement('div');
        cell.className = 'matrix-cell result-cell';

        if (result.type === 'complex') {
            cell.textContent = `${value.real.toFixed(4)}${value.imag >= 0 ? '+' : ''}${value.imag.toFixed(4)}i`;
        } else {
            cell.textContent = typeof value === 'number' ? value.toFixed(4) : value;
        }

        elements.resultGrid.appendChild(cell);
    });
}

// Получение названия операции
function getOperationName(operation) {
    const names = {
        'sum': 'Сложение',
        'multiply': 'Умножение',
        'scalar_multiply': 'Умножение на скаляр',
        'lu': 'LU-разложение'
    };
    return names[operation] || operation;
}

// Показать ошибку
function showError(message) {
    elements.errorMessage.textContent = message;
    elements.errorMessage.classList.add('show');
    setTimeout(() => {
        hideError();
    }, 5000);
}

// Скрыть ошибку
function hideError() {
    elements.errorMessage.classList.remove('show');
}

// Установка состояния загрузки
function setLoading(loading) {
    elements.calculateBtn.disabled = loading;
    elements.applySettings.disabled = loading;

    if (loading) {
        elements.calculateBtn.classList.add('loading');
        elements.loadingSpinner.style.display = 'block';
    } else {
        elements.calculateBtn.classList.remove('loading');
        elements.loadingSpinner.style.display = 'none';
    }
}

// Периодическая проверка здоровья
setInterval(() => {
    checkHealth();
}, 60000);