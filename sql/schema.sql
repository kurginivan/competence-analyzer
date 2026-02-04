-- ============================================================
-- Database Schema for Competence Matrix Analysis System
-- ============================================================

-- Drop existing tables if they exist (in reverse order of dependencies)
DROP TABLE IF EXISTS assessments CASCADE;
DROP TABLE IF EXISTS matrix_competencies CASCADE;
DROP TABLE IF EXISTS employees CASCADE;
DROP TABLE IF EXISTS positions CASCADE;
DROP TABLE IF EXISTS competences CASCADE;
DROP TABLE IF EXISTS matrices CASCADE;

-- Create matrices table
CREATE TABLE matrices (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL UNIQUE,
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create competences table
CREATE TABLE competences (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL UNIQUE,
    description TEXT,
    category VARCHAR(100) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create positions table
-- Positions are linked to matrices (a position corresponds to one matrix)
CREATE TABLE positions (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL UNIQUE,
    matrix_id INTEGER REFERENCES matrices(id) ON DELETE SET NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create employees table
CREATE TABLE employees (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    position_id INTEGER REFERENCES positions(id) ON DELETE SET NULL,
    department VARCHAR(255) NOT NULL,
    email VARCHAR(255) UNIQUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create matrix_competencies junction table
-- This table defines which competencies are required for each matrix
-- and what level is required
CREATE TABLE matrix_competencies (
    id SERIAL PRIMARY KEY,
    matrix_id INTEGER NOT NULL REFERENCES matrices(id) ON DELETE CASCADE,
    competence_id INTEGER NOT NULL REFERENCES competences(id) ON DELETE CASCADE,
    required_level INTEGER NOT NULL CHECK (required_level >= 1 AND required_level <= 5),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(matrix_id, competence_id)
);

-- Create assessments table
CREATE TABLE assessments (
    id SERIAL PRIMARY KEY,
    employee_id INTEGER NOT NULL REFERENCES employees(id) ON DELETE CASCADE,
    competence_id INTEGER NOT NULL REFERENCES competences(id) ON DELETE CASCADE,
    actual_level INTEGER NOT NULL CHECK (actual_level >= 1 AND actual_level <= 5),
    assessment_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    comments TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create indexes for better query performance
CREATE INDEX idx_employees_department ON employees(department);
CREATE INDEX idx_employees_position_id ON employees(position_id);
CREATE INDEX idx_competences_category ON competences(category);
CREATE INDEX idx_assessments_employee ON assessments(employee_id);
CREATE INDEX idx_assessments_competence ON assessments(competence_id);
CREATE INDEX idx_matrix_competencies_matrix ON matrix_competencies(matrix_id);
CREATE INDEX idx_matrix_competencies_competence ON matrix_competencies(competence_id);

-- Add comments to tables
COMMENT ON TABLE employees IS 'Специалисты/сотрудники организации';
COMMENT ON TABLE competences IS 'Требуемые компетенции в области информационной безопасности';
COMMENT ON TABLE matrices IS 'Матрицы компетенций для различных должностей/ролей';
COMMENT ON TABLE matrix_competencies IS 'Связь между матрицами и требуемыми компетенциями с уровнем требований';
COMMENT ON TABLE assessments IS 'Оценки компетенций сотрудников';
