// script.js - Клиентская часть для Matrix Calculator

// Конфигурация API
const API_BASE = 'http://localhost:8000';

// Состояние приложения
const state = {
    matrixType: 'real',
    matrixSize: 2,
    operation: 'sum',
    matrixA: [],
    matrixB: [],
    scalar: 0
};

// Инициализация при загрузке страницы
document.addEventListener('DOMContentLoaded', () => {
    initializeUI();
    checkHealth();
});

// Проверка состояния сервера
async function checkHealth() {
    try {
        const response = await fetch(`${API_BASE}/health`);
        const data = await response.json();
        console.log('Server health:', data.status);
        updateStatusIndicator(data.status === 'ok');
    } catch (error) {
        console.error('Server not available:', error);
        updateStatusIndicator(false);
    }
}

// Обновление индикатора статуса
function updateStatusIndicator(isOnline) {
    const indicator = document.querySelector('.status-indicator');
    if (indicator) {
        indicator.className = `status-indicator ${isOnline ? 'online' : 'offline'}`;
        indicator.title = isOnline ? 'Сервер доступен' : 'Сервер недоступен';
    }
}

// Инициализация UI элементов
function initializeUI() {
    // Выбор типа матрицы
    const typeSelect = document.getElementById('matrix-type');
    if (typeSelect) {
        typeSelect.addEventListener('change', (e) => {
            state.matrixType = e.target.value;
        });
    }

    // Размер матрицы
    const sizeInput = document.getElementById('matrix-size');
    const sizeMinus = document.getElementById('size-minus');
    const sizePlus = document.getElementById('size-plus');

    if (sizeInput) {
        sizeInput.value = state.matrixSize;
        sizeInput.addEventListener('change', (e) => {
            const size = parseInt(e.target.value);
            if (size >= 1 && size <= 10) {
                state.matrixSize = size;
                renderMatrixInputs();
            }
        });
    }

    if (sizeMinus) {
        sizeMinus.addEventListener('click', () => {
            if (state.matrixSize > 1) {
                state.matrixSize--;
                sizeInput.value = state.matrixSize;
                renderMatrixInputs();
            }
        });
    }

    if (sizePlus) {
        sizePlus.addEventListener('click', () => {
            if (state.matrixSize < 10) {
                state.matrixSize++;
                sizeInput.value = state.matrixSize;
                renderMatrixInputs();
            }
        });
    }

    // Выбор операции
    const operationButtons = document.querySelectorAll('.operation-btn');
    operationButtons.forEach(btn => {
        btn.addEventListener('click', (e) => {
            operationButtons.forEach(b => b.classList.remove('active'));
            e.currentTarget.classList.add('active');
            state.operation = e.currentTarget.dataset.operation;
            updateOperationUI();
        });
    });

    // Кнопка вычисления
    const calculateBtn = document.getElementById('calculate-btn');
    if (calculateBtn) {
        calculateBtn.addEventListener('click', performCalculation);
    }

    // Применение размера матрицы
    const applySizeBtn = document.getElementById('apply-size');
    if (applySizeBtn) {
        applySizeBtn.addEventListener('click', () => {
            renderMatrixInputs();
        });
    }

    // Первоначальная отрисовка полей ввода матриц
    renderMatrixInputs();
    updateOperationUI();
}

// Обновление UI в зависимости от операции
function updateOperationUI() {
    const matrixBContainer = document.getElementById('matrix-b-container');
    const scalarContainer = document.getElementById('scalar-container');

    if (matrixBContainer) {
        matrixBContainer.style.display = (state.operation === 'lu') ? 'none' : 'block';
    }

    if (scalarContainer) {
        scalarContainer.style.display = (state.operation === 'scalar') ? 'block' : 'none';
    }
}

// Отрисовка полей ввода матриц
function renderMatrixInputs() {
    const n = state.matrixSize;

    // Матрица A
    const matrixAContainer = document.getElementById('matrix-a');
    if (matrixAContainer) {
        matrixAContainer.innerHTML = '';
        matrixAContainer.style.gridTemplateColumns = `repeat(${n}, 1fr)`;

        for (let i = 0; i < n * n; i++) {
            const input = document.createElement('input');
            input.type = state.matrixType === 'complex' ? 'text' : 'number';
            input.step = state.matrixType === 'real' ? '0.01' : undefined;
            input.placeholder = state.matrixType === 'complex' ? 'a+bi' : '0';
            input.dataset.index = i;
            input.value = state.matrixA[i] || '';
            input.addEventListener('input', (e) => {
                state.matrixA[i] = e.target.value;
            });
            matrixAContainer.appendChild(input);
        }
    }

    // Матрица B
    const matrixBContainer = document.getElementById('matrix-b');
    if (matrixBContainer && state.operation !== 'lu') {
        matrixBContainer.innerHTML = '';
        matrixBContainer.style.gridTemplateColumns = `repeat(${n}, 1fr)`;

        for (let i = 0; i < n * n; i++) {
            const input = document.createElement('input');
            input.type = state.matrixType === 'complex' ? 'text' : 'number';
            input.step = state.matrixType === 'real' ? '0.01' : undefined;
            input.placeholder = state.matrixType === 'complex' ? 'a+bi' : '0';
            input.dataset.index = i;
            input.value = state.matrixB[i] || '';
            input.addEventListener('input', (e) => {
                state.matrixB[i] = e.target.value;
            });
            matrixBContainer.appendChild(input);
        }
    }
}

// Сбор данных матрицы из UI
function collectMatrixData(containerId) {
    const container = document.getElementById(containerId);
    if (!container) return [];

    const inputs = container.querySelectorAll('input');
    const data = [];

    inputs.forEach(input => {
        let value = input.value.trim();
        if (value === '') {
            value = state.matrixType === 'complex' ? '0+0i' : '0';
        }
        data.push(value);
    });

    return data;
}

// Выполнение вычислений
async function performCalculation() {
    const calculateBtn = document.getElementById('calculate-btn');
    const resultContainer = document.getElementById('result');

    if (!calculateBtn || !resultContainer) {
        console.error('Required UI elements not found');
        return;
    }

    // Блокировка кнопки во время вычисления
    calculateBtn.disabled = true;
    calculateBtn.textContent = 'Вычисление...';
    resultContainer.innerHTML = '<div class="loading">Вычисление...</div>';

    try {
        // Сбор данных
        state.matrixA = collectMatrixData('matrix-a');
        state.matrixB = collectMatrixData('matrix-b');

        const scalarInput = document.getElementById('scalar-value');
        state.scalar = scalarInput ? parseFloat(scalarInput.value) || 0 : 0;

        // Подготовка запроса
        const requestData = {
            type: state.matrixType,
            n: state.matrixSize,
            matrixA: state.matrixA.map(v => parseFloat(v) || 0)
        };

        // Определение эндпоинта и дополнительных данных
        let endpoint = '';

        switch (state.operation) {
            case 'sum':
                endpoint = '/api/sum';
                requestData.matrixB = state.matrixB.map(v => parseFloat(v) || 0);
                break;
            case 'multiply':
                endpoint = '/api/multiply';
                requestData.matrixB = state.matrixB.map(v => parseFloat(v) || 0);
                break;
            case 'scalar':
                endpoint = '/api/scalar_multiply';
                requestData.scalar = state.scalar;
                break;
            case 'lu':
                endpoint = '/api/lu';
                break;
            default:
                throw new Error('Неизвестная операция');
        }

        // Отправка запроса
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

        // Отображение результата
        displayResult(result);

    } catch (error) {
        console.error('Calculation error:', error);
        resultContainer.innerHTML = `
            <div class="error">
                <strong>Ошибка:</strong> ${error.message}
            </div>
        `;
    } finally {
        // Разблокировка кнопки
        calculateBtn.disabled = false;
        calculateBtn.textContent = 'Вычислить →';
    }
}

// Отображение результата
function displayResult(result) {
    const resultContainer = document.getElementById('result');
    if (!resultContainer) return;

    if (!result.success) {
        resultContainer.innerHTML = `
            <div class="error">
                <strong>Ошибка:</strong> ${result.error || 'Неизвестная ошибка'}
            </div>
        `;
        return;
    }

    const n = result.size;
    const data = result.result || [];

    let html = `
        <div class="result-info">
            <span>Операция: ${result.operation}</span>
            <span>Тип: ${result.type}</span>
            <span>Размер: ${n}×${n}</span>
        </div>
        <div class="result-matrix" style="grid-template-columns: repeat(${n}, 1fr);">
    `;

    for (let i = 0; i < data.length; i++) {
        const value = data[i];
        const displayValue = typeof value === 'object'
            ? `${value.real || value[0]}${value.imag ? (value.imag >= 0 ? '+' : '') + value.imag + 'i' : ''}`
            : value;

        html += `<div class="matrix-cell">${displayValue}</div>`;
    }

    html += '</div>';
    resultContainer.innerHTML = html;
}

// Утилита для форматирования комплексных чисел
function formatComplex(real, imag) {
    if (imag === 0) return real.toString();
    if (real === 0) return `${imag}i`;
    return `${real}${imag >= 0 ? '+' : ''}${imag}i`;
}

// Экспорт функций для внешнего использования
window.MatrixCalc = {
    checkHealth,
    performCalculation,
    setState: (newState) => Object.assign(state, newState),
    getState: () => ({ ...state })
};