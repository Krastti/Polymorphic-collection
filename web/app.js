'use strict';

const API_BASE = '';

const ENDPOINTS = {
  sum:             '/api/sum',
  multiply:        '/api/multiply',
  scalar_multiply: '/api/scalar_multiply',
  lu:              '/api/lu',
};

const OP_SYMBOLS = {
  sum:             '+',
  multiply:        '×',
  scalar_multiply: 'λ',
  lu:              '∇',
};

const OP_NAMES = {
  sum:             'Сложение',
  multiply:        'Умножение',
  scalar_multiply: 'Умножение на скаляр',
  lu:              'LU-разложение',
};

let state = {
  size:      2,
  type:      'double',
  operation: 'sum',
};

const el = {
  matrixType:     () => document.getElementById('matrix-type'),
  matrixSize:     () => document.getElementById('matrix-size'),
  sizeDisplay:    () => document.getElementById('size-display'),
  sizeIncrease:   () => document.getElementById('size-increase'),
  sizeDecrease:   () => document.getElementById('size-decrease'),
  opBtns:         () => document.querySelectorAll('.op-btn'),
  opSymbol:       () => document.getElementById('operation-symbol'),
  scalarSection:  () => document.getElementById('scalar-section'),
  scalarValue:    () => document.getElementById('scalar-value'),
  matrixAGrid:    () => document.getElementById('matrix-a-grid'),
  matrixBGrid:    () => document.getElementById('matrix-b-grid'),
  matrixBPanel:   () => document.getElementById('matrix-b-panel'),
  matrixBHint:    () => document.getElementById('matrix-b-hint'),
  addMatrixB:     () => document.getElementById('add-matrix-b'),
  applySettings:  () => document.getElementById('apply-settings'),
  calculateBtn:   () => document.getElementById('calculate-btn'),
  resultGrid:     () => document.getElementById('result-grid'),
  resultInfo:     () => document.getElementById('result-info'),
  resultPlaceholder: () => document.getElementById('result-placeholder'),
  resultOutput:   () => document.getElementById('result-output'),
  errorMessage:   () => document.getElementById('error-message'),
  statusDot:      () => document.getElementById('status-dot'),
  statusText:     () => document.getElementById('status-text'),
  loadingSpinner: () => document.getElementById('loading-spinner'),
};

document.addEventListener('DOMContentLoaded', () => {
  setupEventListeners();
  applySettings();
  checkHealth();
  setInterval(checkHealth, 60_000);
});

async function checkHealth() {
  const dot  = el.statusDot();
  const text = el.statusText();
  if (!dot || !text) return;

  try {
    const res  = await fetch(`${API_BASE}/health`);
    const data = await res.json();

    if (data.status === 'ok') {
      dot.className  = 'status-dot connected';
      text.textContent = `Сервер готов · v${data.version ?? ''}`;
    } else {
      dot.className  = 'status-dot error';
      text.textContent = 'Ограниченный режим';
    }
  } catch {
    dot.className  = 'status-dot error';
    text.textContent = 'Недоступен';
  }
}

function setupEventListeners() {
  el.applySettings()?.addEventListener('click', applySettings);
  el.calculateBtn()?.addEventListener('click', calculate);

  // Счётчик размера (+/-)
  el.sizeIncrease()?.addEventListener('click', () => changeSize(+1));
  el.sizeDecrease()?.addEventListener('click', () => changeSize(-1));

  // Смена типа матрицы
  el.matrixType()?.addEventListener('change', () => {
    state.type = el.matrixType().value;
    applySettings();
  });

  // Выбор операции
  el.opBtns().forEach(btn => {
    btn.addEventListener('click', () => {
      el.opBtns().forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      state.operation = btn.dataset.operation;
      updateOperationUI();
    });
  });
}

function changeSize(delta) {
  const sizeEl = el.matrixSize();
  if (!sizeEl) return;
  const current = parseInt(sizeEl.value) || state.size;
  const next    = Math.max(1, Math.min(10, current + delta));
  sizeEl.value  = next;
  const disp    = el.sizeDisplay();
  if (disp) disp.textContent = next;
  applySettings();
}

function applySettings() {
  const sizeEl = el.matrixSize();
  const typeEl = el.matrixType();

  state.size = Math.max(1, Math.min(10, parseInt(sizeEl?.value) || 3));
  state.type = typeEl?.value ?? state.type;

  const disp = el.sizeDisplay();
  if (disp) disp.textContent = state.size;

  buildMatrixGrid(el.matrixAGrid(), state.size, 'A');
  buildMatrixGrid(el.matrixBGrid(), state.size, 'B');
  clearResult();
  hideError();
  updateOperationUI();
}

function updateOperationUI() {
  const symEl = el.opSymbol();
  if (symEl) symEl.textContent = OP_SYMBOLS[state.operation] ?? '+';

  const scalar = el.scalarSection();
  const bPanel = el.matrixBPanel();
  const bHint  = el.matrixBHint();

  const needsB      = state.operation === 'sum' || state.operation === 'multiply';
  const needsScalar = state.operation === 'scalar_multiply';

  if (scalar) scalar.style.display = needsScalar ? 'block' : 'none';
  if (bPanel) bPanel.style.opacity  = needsB ? '1' : '0.4';
  if (bHint) {
    bHint.textContent = state.operation === 'lu'
      ? 'не нужна для LU'
      : needsScalar ? 'не нужна'
      : '';
  }
}

function buildMatrixGrid(container, size, label) {
  if (!container) return;
  container.innerHTML = '';
  container.style.gridTemplateColumns = `repeat(${size}, 1fr)`;

  for (let i = 0; i < size * size; i++) {
    const inp = document.createElement('input');
    inp.type          = 'text';
    inp.className     = 'matrix-cell';
    inp.dataset.index  = i;
    inp.dataset.matrix = label;
    inp.placeholder   = state.type === 'complex' ? 'a, b' : '0';

    // Навигация стрелками по ячейкам
    inp.addEventListener('keydown', e => navigateCells(e, size));
    container.appendChild(inp);
  }
}

function navigateCells(e, size) {
  const cells = [...e.target.closest('[id]').querySelectorAll('.matrix-cell')];
  const idx   = cells.indexOf(e.target);
  if (idx === -1) return;

  const moves = { ArrowRight: 1, ArrowLeft: -1, ArrowDown: size, ArrowUp: -size };
  const delta = moves[e.key];
  if (delta === undefined) return;

  e.preventDefault();
  const next = cells[idx + delta];
  if (next) next.focus();
}

function getMatrixData(label) {
  return [...document.querySelectorAll(`.matrix-cell[data-matrix="${label}"]`)]
    .map(inp => {
      const raw = inp.value.trim();
      return state.type === 'complex'
        ? parseComplexInput(raw)
        : (parseFloat(raw) || 0);
    });
}

function parseComplexInput(v) {
  if (!v) return { real: 0, imag: 0 };

  // JSON-объект
  if (v.startsWith('{')) {
    try {
      const o = JSON.parse(v.replace(/'/g, '"'));
      return { real: parseFloat(o.real) || 0, imag: parseFloat(o.imag) || 0 };
    } catch { /* fallthrough */ }
  }

  // "a, b" или просто "a"
  const parts = v.split(',').map(x => parseFloat(x.trim()));
  return { real: parts[0] || 0, imag: parts[1] || 0 };
}

function rnd() { return (Math.random() * 10 - 5).toFixed(2); }

async function calculate() {
  hideError();

  // Валидация скаляра
  if (state.operation === 'scalar_multiply') {
    const sv = el.scalarValue()?.value?.trim();
    if (!sv) { showError('Введите значение скаляра'); return; }
  }

  const body = buildRequestBody();

  setLoading(true);
  try {
    const url = `${API_BASE}${ENDPOINTS[state.operation]}`;
    const res = await fetch(url, {
      method:  'POST',
      headers: { 'Content-Type': 'application/json' },
      body:    JSON.stringify(body),
    });

    const data = await res.json();

    if (!res.ok) {
      throw new Error(data.detail ?? `HTTP ${res.status}`);
    }

    if (data.success) {
      displayResult(data);
    } else {
      showError(data.error ?? 'Неизвестная ошибка');
    }
  } catch (err) {
    showError(err.message ?? String(err));
  } finally {
    setLoading(false);
  }
}

/** Формирует тело POST-запроса согласно MatrixRequest */
function buildRequestBody() {
  const body = {
    type:    state.type,
    n:       state.size,
    matrixA: getMatrixData('A'),
  };

  const op = state.operation;

  if (op === 'sum' || op === 'multiply') {
    body.matrixB = getMatrixData('B');
  }

  if (op === 'scalar_multiply') {
    const sv = el.scalarValue()?.value?.trim() ?? '0';
    body.scalar = state.type === 'complex'
      ? parseComplexInput(sv)
      : (parseFloat(sv) || 0);
  }

  return body;
}

function displayResult(result) {
  clearResult();

  const placeholder = el.resultPlaceholder();
  const grid        = el.resultGrid();
  const info        = el.resultInfo();
  const output      = el.resultOutput();

  if (placeholder) placeholder.style.display = 'none';
  if (output) output.classList.add('has-result');

  // Мета-информация
  if (info) {
    info.innerHTML = `
      <div class="meta-item"><b>Операция</b> ${OP_NAMES[result.operation] ?? result.operation}</div>
      <div class="meta-item"><b>Тип</b> ${result.type}</div>
      <div class="meta-item"><b>Размер</b> ${result.size}×${result.size}</div>`;
  }

  if (result.operation === 'lu') {
    renderLUResult(result);
  } else {
    renderMatrixResult(grid, result.result, result.size, result.type);
  }
}

function renderMatrixResult(container, flatData, size, type) {
  if (!container) return;
  container.style.display               = 'grid';
  container.style.gridTemplateColumns   = `repeat(${size}, 1fr)`;
  container.innerHTML = '';

  flatData.forEach(v => {
    const cell = document.createElement('div');
    cell.className   = 'matrix-cell result-cell';
    cell.textContent = formatCellValue(v, type);
    container.appendChild(cell);
  });
}

function renderLUResult(result) {
  const grid = el.resultGrid();
  if (!grid) return;

  grid.style.display             = 'block';
  grid.style.gridTemplateColumns = '';
  grid.innerHTML = '';

  [['L', result.L], ['U', result.U]].forEach(([name, data]) => {
    const wrapper = document.createElement('div');
    wrapper.style.marginBottom = '16px';

    const label = document.createElement('div');
    label.textContent = `Матрица ${name}`;
    label.style.cssText = `
      font-size: 0.72rem; font-weight: 700; letter-spacing: 1px;
      text-transform: uppercase; color: var(--accent);
      margin-bottom: 8px; font-family: var(--mono, monospace);`;
    wrapper.appendChild(label);

    const subGrid = document.createElement('div');
    subGrid.style.display               = 'grid';
    subGrid.style.gridTemplateColumns   = `repeat(${result.size}, 1fr)`;
    subGrid.style.gap                   = '6px';

    data.forEach(v => {
      const cell = document.createElement('div');
      cell.className   = 'matrix-cell result-cell';
      cell.textContent = formatCellValue(v, result.type);
      subGrid.appendChild(cell);
    });

    wrapper.appendChild(subGrid);
    grid.appendChild(wrapper);
  });
}

function formatCellValue(v, type) {
  if (type === 'complex' && v && typeof v === 'object') {
    const r = fmtNum(v.real, 3);
    const i = fmtNum(v.imag, 3);
    return `${r}${v.imag >= 0 ? '+' : ''}${i}i`;
  }
  return fmtNum(v, 4);
}

function fmtNum(n, decimals = 4) {
  const num = typeof n === 'number' ? n : parseFloat(n);
  if (isNaN(num)) return '—';
  // Убираем лишние нули: 3.0000 → 3, 1.5000 → 1.5
  return parseFloat(num.toFixed(decimals)).toString();
}

function clearResult() {
  const grid = el.resultGrid();
  const info = el.resultInfo();
  const ph   = el.resultPlaceholder();
  const out  = el.resultOutput();

  if (grid) { grid.innerHTML = ''; grid.style.display = 'none'; }
  if (info) info.innerHTML = '';
  if (ph)   ph.style.display = 'block';
  if (out)  out.classList.remove('has-result');
}

let _errorTimer = null;

function showError(msg) {
  const errEl = el.errorMessage();
  if (!errEl) return;
  errEl.textContent = msg;
  errEl.classList.add('show');
  clearTimeout(_errorTimer);
  _errorTimer = setTimeout(hideError, 6000);
}

function hideError() {
  el.errorMessage()?.classList.remove('show');
}

function setLoading(on) {
  const btn     = el.calculateBtn();
  const applyEl = el.applySettings();
  const spinner = el.loadingSpinner();

  if (btn)     btn.disabled     = on;
  if (applyEl) applyEl.disabled = on;
  if (spinner) spinner.classList.toggle('show', on);
}