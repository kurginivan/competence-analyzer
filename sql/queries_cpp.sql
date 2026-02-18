-- name: employees_insert_with_position
INSERT INTO employees (name, position_id, department, email)
VALUES ($1, $2, $3, $4) RETURNING id;

-- name: employees_insert_without_position
INSERT INTO employees (name, position_id, department, email)
VALUES ($1, NULL, $2, $3) RETURNING id;

-- name: employees_get_by_id
SELECT id, name, position_id, department, email, created_at, updated_at
FROM employees WHERE id = $1;

-- name: employees_get_all
SELECT id, name, position_id, department, email, created_at, updated_at
FROM employees ORDER BY id;

-- name: employees_delete
DELETE FROM employees WHERE id = $1;

-- name: competences_insert
INSERT INTO competences (name, description, category)
VALUES ($1, $2, $3) RETURNING id;

-- name: competences_get_all
SELECT id, name, description, category, created_at, updated_at
FROM competences ORDER BY category, name;

-- name: competences_delete
DELETE FROM competences WHERE id = $1;

-- name: matrices_insert
INSERT INTO matrices (name, description)
VALUES ($1, $2) RETURNING id;

-- name: matrices_get_by_id
SELECT id, name, description, created_at, updated_at
FROM matrices WHERE id = $1;

-- name: matrices_get_all
SELECT id, name, description, created_at, updated_at
FROM matrices ORDER BY id;

-- name: matrices_delete
DELETE FROM matrices WHERE id = $1;

-- name: matrix_competencies_upsert
INSERT INTO matrix_competencies (matrix_id, competence_id, required_level)
VALUES ($1, $2, $3) ON CONFLICT (matrix_id, competence_id)
DO UPDATE SET required_level = $3;

-- name: matrix_competencies_delete
DELETE FROM matrix_competencies WHERE matrix_id = $1 AND competence_id = $2;

-- name: matrix_competencies_get_by_matrix
SELECT competence_id, required_level FROM matrix_competencies
WHERE matrix_id = $1;

-- name: assessments_insert
INSERT INTO assessments (employee_id, competence_id, actual_level, comments)
VALUES ($1, $2, $3, $4) RETURNING id;

-- name: assessments_get_by_employee
SELECT id, employee_id, competence_id, actual_level, comments,
assessment_date, created_at, updated_at FROM assessments
WHERE employee_id = $1 ORDER BY assessment_date DESC;

-- name: assessments_delete
DELETE FROM assessments WHERE id = $1;

-- name: positions_insert_with_matrix
INSERT INTO positions (name, matrix_id)
VALUES ($1, $2) RETURNING id;

-- name: positions_insert_without_matrix
INSERT INTO positions (name, matrix_id)
VALUES ($1, NULL) RETURNING id;

-- name: positions_get_by_id
SELECT id, name, matrix_id, created_at, updated_at
FROM positions WHERE id = $1;

-- name: positions_get_all
SELECT id, name, matrix_id, created_at, updated_at
FROM positions ORDER BY id;

-- name: positions_get_by_matrix
SELECT id, name, matrix_id, created_at, updated_at
FROM positions WHERE matrix_id = $1 ORDER BY id;

-- name: positions_update_with_matrix
UPDATE positions SET name = $1, matrix_id = $2 WHERE id = $3;

-- name: positions_update_without_matrix
UPDATE positions SET name = $1, matrix_id = NULL WHERE id = $2;

-- name: positions_delete
DELETE FROM positions WHERE id = $1;
