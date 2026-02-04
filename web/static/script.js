const API_BASE = "/api";

// State for sorting and filtering
let assessmentsData = [];
let sortBy = "name"; // name, level, competence, position
let filterEmployee = null;
let filterCompetence = null;
let filterLevel = null;
let filterPosition = null;

// Tab switching
document.querySelectorAll(".tab-btn").forEach(btn => {
    btn.addEventListener("click", () => {
        const tab = btn.dataset.tab;
        
        // Update active button
        document.querySelectorAll(".tab-btn").forEach(b => b.classList.remove("active"));
        btn.classList.add("active");
        
        // Update active content
        document.querySelectorAll(".tab-content").forEach(c => c.classList.remove("active"));
        document.getElementById(tab).classList.add("active");
        
        // Load data for tab
        if (tab === "employees") loadEmployees();
        else if (tab === "competences") loadCompetences();
        else if (tab === "assessments") {
            loadAssessments();
            loadSelectOptions();
        } else if (tab === "matrices") {
            loadMatrices();
        } else if (tab === "analysis") {
            loadAnalysisSelects();
        }
    });
});

// Show notification
function showNotification(message, type = "success") {
    const alert = document.createElement("div");
    alert.className = `alert alert-${type}`;
    alert.textContent = message;
    
    document.querySelector(".container").insertBefore(alert, document.querySelector("header").nextSibling);
    
    setTimeout(() => alert.remove(), 4000);
}

// ===== EMPLOYEES =====
document.getElementById("empForm").addEventListener("submit", async (e) => {
    e.preventDefault();
    
    const employee = {
        name: document.getElementById("empName").value,
        position_id: parseInt(document.getElementById("empPositionSelect").value) || null,
        department: document.getElementById("empDept").value,
        email: document.getElementById("empEmail").value
    };
    
    try {
        const res = await fetch(`${API_BASE}/employees`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(employee)
        });
        
        if (res.ok) {
            showNotification("Сотрудник добавлен");
            document.getElementById("empForm").reset();
            loadEmployees();
            loadSelectOptions();
        }
    } catch (err) {
        showNotification(`Ошибка: ${err.message}`, "error");
    }
});

async function loadEmployees() {
    try {
        const res = await fetch(`${API_BASE}/employees`);
        const employees = await res.json();
        
        const tbody = document.querySelector("#empTable tbody");
        tbody.innerHTML = employees.length ? "" : "<tr><td colspan='5' class='empty-message'>Сотрудников не найдено</td></tr>";
        
        employees.forEach(emp => {
            const row = `
                <tr>
                    <td>${emp.name}</td>
                    <td>${emp.position_name || '-'}</td>
                    <td>${emp.department}</td>
                    <td>${emp.email}</td>
                    <td class="action-cell">
                        <button class="btn-danger" onclick="deleteEmployee(${emp.id})">🗑️</button>
                    </td>
                </tr>
            `;
            tbody.innerHTML += row;
        });
    } catch (err) {
        console.error("Error loading employees:", err);
    }
} 

async function deleteEmployee(id) {
    if (!confirm("Вы уверены?")) return;
    
    try {
        const res = await fetch(`${API_BASE}/employees/${id}`, { method: "DELETE" });
        if (res.ok) {
            showNotification("Сотрудник удален");
            loadEmployees();
            loadSelectOptions();
        }
    } catch (err) {
        showNotification(`Ошибка: ${err.message}`, "error");
    }
}

// ===== COMPETENCES =====
document.getElementById("compForm").addEventListener("submit", async (e) => {
    e.preventDefault();
    
    const competence = {
        name: document.getElementById("compName").value,
        description: document.getElementById("compDesc").value,
        category: "General"
    };
    
    try {
        const res = await fetch(`${API_BASE}/competences`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(competence)
        });
        
        if (res.ok) {
            showNotification("Компетенция добавлена");
            document.getElementById("compForm").reset();
            loadCompetences();
            loadSelectOptions();
        }
    } catch (err) {
        showNotification(`Ошибка: ${err.message}`, "error");
    }
});

async function loadCompetences() {
    try {
        const res = await fetch(`${API_BASE}/competences`);
        const competences = await res.json();
        
        const tbody = document.querySelector("#compTable tbody");
        tbody.innerHTML = competences.length ? "" : "<tr><td colspan='3' class='empty-message'>Компетенций не найдено</td></tr>";
        
        competences.forEach(comp => {
            const row = `
                <tr>
                    <td>${comp.name}</td>
                    <td>${comp.description}</td>
                    <td class="action-cell">
                        <button class="btn-danger" onclick="deleteCompetence(${comp.id})">🗑️</button>
                    </td>
                </tr>
            `;
            tbody.innerHTML += row;
        });
    } catch (err) {
        console.error("Error loading competences:", err);
    }
}

async function deleteCompetence(id) {
    if (!confirm("Вы уверены?")) return;
    
    try {
        const res = await fetch(`${API_BASE}/competences/${id}`, { method: "DELETE" });
        if (res.ok) {
            showNotification("Компетенция удалена");
            loadCompetences();
            loadSelectOptions();
        }
    } catch (err) {
        showNotification(`Ошибка: ${err.message}`, "error");
    }
}

// ===== ASSESSMENTS =====
document.getElementById("assForm").addEventListener("submit", async (e) => {
    e.preventDefault();
    
    const assessment = {
        employee_id: parseInt(document.getElementById("assEmp").value),
        competence_id: parseInt(document.getElementById("assComp").value),
        actual_level: parseInt(document.getElementById("assLevel").value),
        comments: document.getElementById("assComments").value
    };
    
    try {
        const res = await fetch(`${API_BASE}/assessments`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(assessment)
        });
        
        if (res.ok) {
            showNotification("Оценка добавлена");
            document.getElementById("assForm").reset();
            loadAssessments();
        }
    } catch (err) {
        showNotification(`Ошибка: ${err.message}`, "error");
    }
});

async function loadAssessments() {
    try {
        const res = await fetch(`${API_BASE}/assessments`);
        assessmentsData = await res.json();
        
        // Apply filters and sort
        applyFiltersAndSort();
    } catch (err) {
        console.error("Error loading assessments:", err);
    }
}

function applyFiltersAndSort() {
    let filtered = [...assessmentsData];
    
    // Apply filters
    if (filterEmployee) {
        filtered = filtered.filter(a => a.employee_id === parseInt(filterEmployee));
    }
    if (filterPosition) {
        filtered = filtered.filter(a => a.position_id === parseInt(filterPosition));
    }
    if (filterCompetence) {
        filtered = filtered.filter(a => a.competence_id === parseInt(filterCompetence));
    }
    if (filterLevel) {
        filtered = filtered.filter(a => a.actual_level === parseInt(filterLevel));
    }
    
    // Apply sorting
    if (sortBy === "level") {
        filtered.sort((a, b) => b.actual_level - a.actual_level);
    } else if (sortBy === "competence") {
        filtered.sort((a, b) => a.competence_name.localeCompare(b.competence_name));
    } else if (sortBy === "position") {
        filtered.sort((a, b) => (a.position_name || '').localeCompare((b.position_name || '')));
    } else {
        filtered.sort((a, b) => a.employee_name.localeCompare(b.employee_name));
    }
    
    // Render table
    renderAssessmentsTable(filtered);
}

function renderAssessmentsTable(assessments) {
    const tbody = document.querySelector("#assTable tbody");
    const filteredCount = assessments.length;
    const totalCount = assessmentsData.length;
    
    if (filteredCount === 0) {
        tbody.innerHTML = `<tr><td colspan='5' class='empty-message'>Оценок не найдено (всего: ${totalCount})</td></tr>`;
        return;
    }
    
    tbody.innerHTML = "";
    assessments.forEach(ass => {
        const row = `
            <tr>
                <td>${ass.employee_name}</td>
                <td>${ass.position_name || '-'}</td>
                <td>${ass.competence_name}</td>
                <td><span class="level-badge level-${ass.actual_level}">${ass.actual_level}/5</span></td>
                <td>${ass.comments || "-"}</td>
                <td class="action-cell">
                    <button class="btn-danger" onclick="deleteAssessment(${ass.id})">🗑️</button>
                </td>
            </tr>
        `;
        tbody.innerHTML += row;
    });
}

function sortAssessments(by) {
    sortBy = by;
    applyFiltersAndSort();
    
    // Update active sort button
    document.querySelectorAll(".sort-btn").forEach(btn => btn.classList.remove("active"));
    document.querySelector(`[data-sort="${by}"]`).classList.add("active");
}

function setFilter(filterType, value) {
    if (filterType === "employee") {
        filterEmployee = value || null;
    } else if (filterType === "position") {
        filterPosition = value || null;
    } else if (filterType === "competence") {
        filterCompetence = value || null;
    } else if (filterType === "level") {
        filterLevel = value || null;
    }
    applyFiltersAndSort();
}

function clearAllFilters() {
    filterEmployee = null;
    filterCompetence = null;
    filterLevel = null;
    document.getElementById("filterEmployee").value = "";
    document.getElementById("filterCompetence").value = "";
    document.getElementById("filterLevel").value = "";
    applyFiltersAndSort();
}

async function deleteAssessment(id) {
    if (!confirm("Вы уверены?")) return;
    
    try {
        const res = await fetch(`${API_BASE}/assessments/${id}`, { method: "DELETE" });
        if (res.ok) {
            showNotification("Оценка удалена");
            loadAssessments();
        }
    } catch (err) {
        showNotification(`Ошибка: ${err.message}`, "error");
    }
}

async function loadSelectOptions() {
    try {
        // Load employees for assessment form
        const empRes = await fetch(`${API_BASE}/employees`);
        const employees = await empRes.json();
        const empSelect = document.getElementById("assEmp");
        empSelect.innerHTML = '<option value="">Выберите сотрудника</option>';
        employees.forEach(emp => {
            empSelect.innerHTML += `<option value="${emp.id}">${emp.name}</option>`;
        });
        
        // Load for filter (employees)
        const filterEmpSelect = document.getElementById("filterEmployee");
        if (filterEmpSelect) {
            filterEmpSelect.innerHTML = '<option value="">Все сотрудники</option>';
            employees.forEach(emp => {
                filterEmpSelect.innerHTML += `<option value="${emp.id}">${emp.name}</option>`;
            });
        }
        
        // Also load for bulk form
        const bulkEmpSelect = document.getElementById("bulkEmpName");
        if (bulkEmpSelect) {
            bulkEmpSelect.innerHTML = '';
            employees.forEach(emp => {
                bulkEmpSelect.innerHTML += `<option value="${emp.id}">${emp.name}</option>`;
            });
        }

        // Load positions
        const posRes = await fetch(`${API_BASE}/positions`);
        const positions = await posRes.json();
        const empPosSelect = document.getElementById('empPositionSelect');
        if (empPosSelect) {
            empPosSelect.innerHTML = '<option value="">Выберите должность</option>';
            positions.forEach(p => empPosSelect.innerHTML += `<option value="${p.id}">${p.name}</option>`);
        }
        const bulkPosSelect = document.getElementById('bulkEmpPosition');
        if (bulkPosSelect) {
            bulkPosSelect.innerHTML = '<option value="">Выберите должность</option>';
            positions.forEach(p => bulkPosSelect.innerHTML += `<option value="${p.id}">${p.name}</option>`);
        }
        const filterPosSelect = document.getElementById('filterPosition');
        if (filterPosSelect) {
            filterPosSelect.innerHTML = '<option value="">Все должности</option>';
            positions.forEach(p => filterPosSelect.innerHTML += `<option value="${p.id}">${p.name}</option>`);
        }
        
        // Load competences for assessment form
        const compRes = await fetch(`${API_BASE}/competences`);
        const competences = await compRes.json();
        const compSelect = document.getElementById("assComp");
        compSelect.innerHTML = '<option value="">Выберите компетенцию</option>';
        competences.forEach(comp => {
            compSelect.innerHTML += `<option value="${comp.id}">${comp.name}</option>`;
        });
        
        // Load for filter (competences)
        const filterCompSelect = document.getElementById("filterCompetence");
        if (filterCompSelect) {
            filterCompSelect.innerHTML = '<option value="">Все компетенции</option>';
            competences.forEach(comp => {
                filterCompSelect.innerHTML += `<option value="${comp.id}">${comp.name}</option>`;
            });
        }
        
        // Also load for bulk form
        const bulkCompContainer = document.getElementById("bulkCompetences");
        if (bulkCompContainer) {
            bulkCompContainer.innerHTML = '<h3>Компетенции:</h3>';
            competences.forEach(comp => {
                bulkCompContainer.innerHTML += `
                    <div class="competence-item">
                        <label>
                            <input type="checkbox" class="comp-checkbox" value="${comp.id}" data-comp-name="${comp.name}">
                            <span>${comp.name}</span>
                        </label>
                        <input type="number" min="1" max="5" placeholder="Уровень" class="comp-level" style="width: 80px;">
                    </div>
                `;
            });
        }
    } catch (err) {
        console.error("Error loading select options:", err);
    }
}

// Bulk employee creation
document.getElementById("bulkForm")?.addEventListener("submit", async (e) => {
    e.preventDefault();
    
    // Create employee
    const employee = {
        name: document.getElementById("bulkEmpName").value,
        position_id: parseInt(document.getElementById("bulkEmpPosition").value) || null,
        department: document.getElementById("bulkEmpDept").value,
        email: document.getElementById("bulkEmpEmail").value
    };
    
    try {
        const empRes = await fetch(`${API_BASE}/employees`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(employee)
        });
        
        if (!empRes.ok) throw new Error("Failed to create employee");
        const createdEmp = await empRes.json();
        
        // Create assessments for checked competences
        const checkboxes = document.querySelectorAll(".comp-checkbox:checked");
        const failed = [];
        
        for (let i = 0; i < checkboxes.length; i++) {
            const compId = parseInt(checkboxes[i].value);
            // find the corresponding level input inside the same .competence-item
            const levelInput = checkboxes[i].closest('.competence-item')?.querySelector('.comp-level');
            const level = parseInt(levelInput?.value) || 1;
            
            const assessment = {
                employee_id: createdEmp.id,
                competence_id: compId,
                actual_level: level,
                comments: ""
            };
            
            const resp = await fetch(`${API_BASE}/assessments`, {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify(assessment)
            });
            if (!resp.ok) {
                const text = await resp.text();
                failed.push({ competence_id: compId, reason: text });
            }
        }
        if (failed.length) {
            showNotification(`Часть оценок не добавлена: ${failed.map(f=>f.competence_id).join(', ')}`, 'error');
        } else {
            showNotification(`Сотрудник "${createdEmp.name}" добавлен с компетенциями`);
        }
        
        showNotification(`Сотрудник "${createdEmp.name}" добавлен с компетенциями`);
        document.getElementById("bulkForm").reset();
        document.querySelectorAll(".comp-checkbox").forEach(cb => cb.checked = false);
        loadEmployees();
        loadSelectOptions();
        
        // Switch to assessments tab
        setTimeout(() => {
            document.querySelector('[data-tab="assessments"]').click();
        }, 1000);
    } catch (err) {
        showNotification(`Ошибка: ${err.message}`, "error");
    }
});

// ===== MATRICES =====
document.getElementById('matrixForm')?.addEventListener('submit', async (e) => {
    e.preventDefault();
    const matrix = {
        name: document.getElementById('matrixName').value,
        description: document.getElementById('matrixDesc').value
    };
    try {
        const res = await fetch(`${API_BASE}/matrices`, {
            method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(matrix)
        });
        if (res.ok) {
            showNotification('Матрица добавлена');
            document.getElementById('matrixForm').reset();
            loadMatrices();
        }
    } catch (err) {
        showNotification(`Ошибка: ${err.message}`, 'error');
    }
});

async function loadMatrices() {
    try {
        const res = await fetch(`${API_BASE}/matrices`);
        const matrices = await res.json();
        const tbody = document.querySelector('#matrixTable tbody');
        tbody.innerHTML = matrices.length ? '' : "<tr><td colspan='3' class='empty-message'>Матриц не найдено</td></tr>";
        // load positions to display role mapping
        const posRes = await fetch(`${API_BASE}/positions`);
        const positions = await posRes.json();
        matrices.forEach(m => {
            const posNames = positions.filter(p => p.matrix_id === m.id).map(p => p.name).join(', ') || '-';
            const row = `
                <tr>
                    <td>${m.name}</td>
                    <td>${posNames}</td>
                    <td>${m.description || '-'}</td>
                    <td class="action-cell">
                        <button class="btn-primary" onclick="openMatrix(${m.id}, '${m.name.replace(/'/g, "\'")}')">Открыть</button>
                        <button class="btn-danger" onclick="deleteMatrix(${m.id})">🗑️</button>
                    </td>
                </tr>
            `;
            tbody.innerHTML += row;
        });
    } catch (err) {
        console.error('Error loading matrices:', err);
    }
}

async function deleteMatrix(id) {
    if (!confirm('Вы уверены?')) return;
    try {
        const res = await fetch(`${API_BASE}/matrices/${id}`, { method: 'DELETE' });
        if (res.ok) { showNotification('Матрица удалена'); loadMatrices(); }
    } catch (err) { showNotification(`Ошибка: ${err.message}`, 'error'); }
}

let currentMatrixId = null;
async function openMatrix(id, name) {
    currentMatrixId = id;
    document.getElementById('matrixDetailsSection').style.display = 'block';
    document.getElementById('matrixTitle').textContent = name;
    await refreshMatrixDetails();
}

async function refreshMatrixDetails() {
    if (!currentMatrixId) return;
    try {
        // Load competencies assigned into matrix
        const res = await fetch(`${API_BASE}/matrices/${currentMatrixId}/competencies`);
        const items = await res.json();
        const container = document.getElementById('matrixCompetencesContainer');
        if (!items.length) {
            container.innerHTML = '<p class="empty-message">Компетенции в матрице не заданы</p>';
        } else {
            container.innerHTML = '<ul class="list-normal">' + items.map(it => `<li>${it.competence_name} — требуемый уровень: ${it.required_level} <button class="btn-danger" onclick="removeMatrixCompetence(${currentMatrixId}, ${it.competence_id})">Удалить</button></li>`).join('') + '</ul>';
        }
        // Populate select with all competences
        const compRes = await fetch(`${API_BASE}/competences`);
        const competences = await compRes.json();
        const compSelect = document.getElementById('matrixCompSelect');
        compSelect.innerHTML = '<option value="">Выберите компетенцию</option>';
        competences.forEach(c => { compSelect.innerHTML += `<option value="${c.id}">${c.name}</option>`; });

        // Load positions for this matrix and show them
        const posRes = await fetch(`${API_BASE}/positions`);
        const positions = await posRes.json();
        const myPositions = positions.filter(p => p.matrix_id === currentMatrixId);
        const posContainer = document.getElementById('matrixPositionsContainer');
        if (!myPositions.length) {
            posContainer.innerHTML = '<p class="empty-message">Должности не назначены</p>';
        } else {
            posContainer.innerHTML = '<ul class="list-normal">' + myPositions.map(p => `<li>${p.name} <button class="btn-danger" onclick="deletePosition(${p.id})">Удалить</button></li>`).join('') + '</ul>';
        }
    } catch (err) { console.error('Error refreshing matrix details:', err); }
}

// Add competence to matrix
document.getElementById('matrixCompForm')?.addEventListener('submit', async (e) => {
    e.preventDefault();
    if (!currentMatrixId) return showNotification('Выберите матрицу', 'error');
    const competence_id = parseInt(document.getElementById('matrixCompSelect').value);
    const required_level = parseInt(document.getElementById('matrixCompLevel').value);
    try {
        const res = await fetch(`${API_BASE}/matrices/${currentMatrixId}/competencies`, {
            method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ competence_id, required_level })
        });
        if (res.ok) { showNotification('Компетенция добавлена в матрицу'); document.getElementById('matrixCompForm').reset(); refreshMatrixDetails(); }
    } catch (err) { showNotification(`Ошибка: ${err.message}`, 'error'); }
});

// Add position to matrix
document.getElementById('matrixPositionForm')?.addEventListener('submit', async (e) => {
    e.preventDefault();
    if (!currentMatrixId) return showNotification('Выберите матрицу', 'error');
    const name = document.getElementById('matrixPositionName').value.trim();
    if (!name) return showNotification('Введите название должности', 'error');
    try {
        const res = await fetch(`${API_BASE}/positions`, {
            method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ name, matrix_id: currentMatrixId })
        });
        if (res.ok) { showNotification('Должность добавлена'); document.getElementById('matrixPositionForm').reset(); refreshMatrixDetails(); loadSelectOptions(); }
    } catch (err) { showNotification(`Ошибка: ${err.message}`, 'error'); }
});

async function deletePosition(id) {
    if (!confirm('Удалить должность?')) return;
    try {
        const res = await fetch(`${API_BASE}/positions/${id}`, { method: 'DELETE' });
        if (res.ok) { showNotification('Должность удалена'); refreshMatrixDetails(); loadSelectOptions(); }
    } catch (err) { showNotification(`Ошибка: ${err.message}`, 'error'); }
}

async function removeMatrixCompetence(matrixId, competenceId) {
    if (!confirm('Удалить компетенцию из матрицы?')) return;
    try {
        const res = await fetch(`${API_BASE}/matrices/${matrixId}/competencies/${competenceId}`, { method: 'DELETE' });
        if (res.ok) { showNotification('Удалено'); refreshMatrixDetails(); }
    } catch (err) { showNotification(`Ошибка: ${err.message}`, 'error'); }
}

// ===== ANALYSIS & REPORTS =====
async function loadAnalysisSelects() {
    try {
        const empRes = await fetch(`${API_BASE}/employees`);
        const employees = await empRes.json();
        const empSelect = document.getElementById('analysisEmployee');
        if (!empSelect) return;
        empSelect.innerHTML = '<option value="">Выберите сотрудника</option>';
        employees.forEach(emp => { empSelect.innerHTML += `<option value="${emp.id}">${emp.name}</option>`; });
    } catch (err) { console.error('Error loading analysis selects:', err); }
}

async function runAnalysisForSelected() {
    const empId = parseInt(document.getElementById('analysisEmployee').value);
    if (!empId) return showNotification('Выберите сотрудника', 'error');
    try {
        const res = await fetch(`${API_BASE}/analysis/employee/${empId}`);
        if (!res.ok) throw new Error('Analysis failed');
        const data = await res.json();
        renderAnalysisResult(data);
    } catch (err) { showNotification(`Ошибка: ${err.message}`, 'error'); }
}

function renderAnalysisResult(data) {
    const container = document.getElementById('analysisResult');
    const lines = [];
    const posName = data.employee.position_name || 'Не указана';
    lines.push(`<h3>${data.employee.name} — ${posName} (${data.employee.department})</h3>`);
    if (data.position_match_percentage !== null && data.position_match_percentage !== undefined) {
        lines.push(`<p>Соответствие должности: <strong>${data.position_match_percentage}%</strong></p>`);
    }
    lines.push('<h4>Средние уровни по компетенциям:</h4>');
    if (!data.averages.length) {
        lines.push('<p class="empty-message">Нет данных об оценках</p>');
    } else {
        lines.push('<ul>');
        data.averages.forEach(a => lines.push(`<li>${a.competence_name}: ${a.avg_level}/5</li>`));
        lines.push('</ul>');
    }
    lines.push('<h4>Пробелы (уровень < 3):</h4>');
    if (!data.gaps.length) lines.push('<p>- Пробелов не выявлено</p>'); else { lines.push('<ul>'); data.gaps.forEach(g => lines.push(`<li>${g.competence_name}: ${g.avg_level}/5</li>`)); lines.push('</ul>'); }
    lines.push('<h4>Рекомендации (топ-5):</h4>');
    lines.push('<ul>'); data.recommendations.forEach(r => lines.push(`<li>${r.competence_name}: ${r.avg_level}/5</li>`)); lines.push('</ul>');
    container.innerHTML = lines.join('');
}

async function generateReportForSelected() {
    const empId = parseInt(document.getElementById('analysisEmployee').value);
    if (!empId) return showNotification('Выберите сотрудника', 'error');
    try {
        const res = await fetch(`${API_BASE}/reports/employee/${empId}`, { method: 'POST' });
        if (!res.ok) throw new Error('Report generation failed');
        const j = await res.json();
        const reportContainer = document.getElementById('reportOutput');
        reportContainer.textContent = j.report;
        showNotification('Отчёт сгенерирован');
    } catch (err) { showNotification(`Ошибка: ${err.message}`, 'error'); }
}

// Initial load
window.addEventListener("load", () => {
    loadEmployees();
    loadSelectOptions();
});
