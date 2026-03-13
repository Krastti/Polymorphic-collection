
const API_BASE = 'http://localhost:5000/api';

const typeEl    = document.getElementById('type');
const sizeDisp  = document.getElementById('size-display');
const matAEl    = document.getElementById('matrix-a');
const matBEl    = document.getElementById('matrix-b');
const scalarEl  = document.getElementById('scalar-input');
const resultEl  = document.getElementById('result-output');
const spinner   = document.getElementById('spinner');
const errorBox  = document.getElementById('error-box');
const opSymbol  = document.getElementById('op-symbol');
const panelB    = document.getElementById('panel-b');
const panelSc   = document.getElementById('panel-scalar');
const badge     = document.getElementById('health-badge');

let currentN  = 2;
let currentOp = 'sum';

const opSymbols = { sum: '+', multiply: '×', scalar_multiply: '×', lu: '∇' };

/* ── Health check ── */
async function checkHealth() {
  try {
    const r = await fetch(`${API_BASE}/health`);
    const j = await r.json();
    const dot = badge.querySelector('.dot');
    if (j.executable_found) {
      dot.className = 'dot ok';
      badge.innerHTML = '<span class="dot ok"></span> Сервер готов';
    } else {
      dot.className = 'dot err';
      badge.innerHTML = '<span class="dot err"></span> Программа не найдена';
    }
  } catch {
    const dot = badge.querySelector('.dot');
    dot.className = 'dot err';
    badge.innerHTML = '<span class="dot err"></span> Нет соединения';
  }
}
checkHealth();

/* ── Размер ── */
document.getElementById('dec').addEventListener('click', () => {
  const v = parseInt(sizeDisp.textContent);
  if (v > 1) sizeDisp.textContent = v - 1;
});
document.getElementById('inc').addEventListener('click', () => {
  const v = parseInt(sizeDisp.textContent);
  if (v < 6) sizeDisp.textContent = v + 1;
});

/* ── Построение сетки ── */
function buildGrid(container, n, type) {
  container.innerHTML = '';
  container.style.gridTemplateColumns = type === 'complex'
    ? `repeat(${n}, auto)`
    : `repeat(${n}, 70px)`;

  for (let i = 0; i < n * n; i++) {
    if (type === 'complex') {
      const cell = document.createElement('div');
      cell.className = 'cell-complex';
      const re = document.createElement('input');
      re.placeholder = 're'; re.dataset.part = 'real';
      const im = document.createElement('input');
      im.placeholder = 'im'; im.dataset.part = 'imag';
      cell.appendChild(re); cell.appendChild(im);
      container.appendChild(cell);
    } else {
      const inp = document.createElement('input');
      inp.placeholder = '0';
      container.appendChild(inp);
    }
  }
}

function buildScalar(type) {
  scalarEl.innerHTML = '';
  if (type === 'complex') {
    ['real', 'imag'].forEach((part, i) => {
      const lbl = document.createElement('div');
      lbl.className = 'scalar-label';
      lbl.textContent = i === 0 ? 'Вещественная' : 'Мнимая';
      const inp = document.createElement('input');
      inp.placeholder = part === 'real' ? 're' : 'im';
      inp.dataset.part = part;
      scalarEl.appendChild(lbl);
      scalarEl.appendChild(inp);
    });
  } else {
    const lbl = document.createElement('div');
    lbl.className = 'scalar-label'; lbl.textContent = 'Значение';
    const inp = document.createElement('input');
    inp.placeholder = '1.0';
    scalarEl.appendChild(lbl);
    scalarEl.appendChild(inp);
  }
}

function updatePanelVisibility() {
  const showB  = currentOp === 'sum' || currentOp === 'multiply';
  const showSc = currentOp === 'scalar_multiply';
  panelB.style.opacity  = showB  ? '1' : '0.3';
  panelSc.style.opacity = showSc ? '1' : '0.3';
  opSymbol.textContent = opSymbols[currentOp] || '+';
}

function rebuild() {
  currentN = parseInt(sizeDisp.textContent) || 2;
  const t  = typeEl.value;
  buildGrid(matAEl, currentN, t);
  buildGrid(matBEl, currentN, t);
  buildScalar(t);
  updatePanelVisibility();
}

document.getElementById('btn-resize').addEventListener('click', rebuild);
typeEl.addEventListener('change', rebuild);
rebuild();

/* ── Операции ── */
document.querySelectorAll('.op-btn').forEach(btn => {
  btn.addEventListener('click', () => {
    document.querySelectorAll('.op-btn').forEach(b => b.classList.remove('active'));
    btn.classList.add('active');
    currentOp = btn.dataset.op;
    updatePanelVisibility();
  });
});

/* ── Чтение данных ── */
function readMatrix(container, n, type) {
  const cells = [...container.children];
  return Array.from({ length: n * n }, (_, i) => {
    if (type === 'complex') {
      const inputs = cells[i].querySelectorAll('input');
      return { real: parseFloat(inputs[0].value) || 0, imag: parseFloat(inputs[1].value) || 0 };
    }
    return parseFloat(cells[i].value) || 0;
  });
}

function readScalar(type) {
  const inputs = scalarEl.querySelectorAll('input');
  if (type === 'complex') {
    return { real: parseFloat(inputs[0]?.value) || 0, imag: parseFloat(inputs[1]?.value) || 0 };
  }
  return parseFloat(inputs[0]?.value) || 1;
}

/* ── Результат / ошибка ── */
function showResult(text) {
  errorBox.classList.add('hidden');
  resultEl.innerHTML = '';
  resultEl.textContent = text;
  resultEl.style.borderColor = 'rgba(52,211,153,0.3)';
}
function showError(msg) {
  errorBox.textContent = msg;
  errorBox.classList.remove('hidden');
  resultEl.innerHTML = '<span class="placeholder">Результат появится здесь…</span>';
  resultEl.style.borderColor = 'rgba(248,113,113,0.3)';
}
function clearStatus() {
  errorBox.classList.add('hidden');
  resultEl.style.borderColor = '';
}

/* ── Вызов API ── */
async function callApi(endpoint, body) {
  clearStatus();
  spinner.classList.remove('hidden');
  resultEl.innerHTML = '';

  try {
    const res  = await fetch(`${API_BASE}/${endpoint}`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(body)
    });
    const json = await res.json();
    json.success ? showResult(json.output) : showError(json.error || 'Неизвестная ошибка');
  } catch (e) {
    showError('Не удалось подключиться к серверу: ' + e.message);
  } finally {
    spinner.classList.add('hidden');
  }
}

document.getElementById('btn-calc').addEventListener('click', () => {
  const type = typeEl.value;
  const n    = currentN;
  const A    = readMatrix(matAEl, n, type);
  const B    = readMatrix(matBEl, n, type);
  const sc   = readScalar(type);
  const base = { type, n, matrixA: A };

  if (currentOp === 'sum' || currentOp === 'multiply') {
    callApi(currentOp, { ...base, matrixB: B });
  } else if (currentOp === 'scalar_multiply') {
    callApi('scalar_multiply', { ...base, scalar: sc });
  } else if (currentOp === 'lu') {
    callApi('lu', base);
  }
});
