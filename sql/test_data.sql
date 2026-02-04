-- ============================================================
-- Test Data for Competence Matrix Analysis System
-- ============================================================

-- Clear existing data (if any)
DELETE FROM assessments;
DELETE FROM matrix_competencies;
DELETE FROM employees;
DELETE FROM competences;
DELETE FROM matrices;

-- ==================== INSERT COMPETENCES ====================

INSERT INTO competences (name, description, category) VALUES
('Cryptography', 'Knowledge of encryption algorithms, hash functions, and digital signatures', 'Cryptography'),
('Public Key Infrastructure (PKI)', 'Understanding of certificates, CA, and key management', 'Cryptography'),
('Network Security', 'Firewalls, intrusion detection systems, VPN, DLP', 'Network Security'),
('Vulnerability Assessment', 'Identifying and assessing security vulnerabilities', 'Network Security'),
('Penetration Testing', 'Authorized security testing and exploitation techniques', 'Network Security'),
('Access Control', 'Authentication, authorization, identity management (IAM)', 'Access Control'),
('Incident Response', 'Detection, analysis, and response to security incidents', 'Incident Management'),
('Security Audit', 'Auditing systems for compliance and security controls', 'Compliance'),
('Risk Management', 'Identifying, analyzing, and mitigating security risks', 'Compliance'),
('Information Security Policy', 'Creating and maintaining security policies and procedures', 'Governance'),
('Secure Coding', 'Writing secure applications and preventing common vulnerabilities', 'Development'),
('Database Security', 'Protecting databases and managing access to sensitive data', 'Data Protection');

-- ==================== INSERT MATRICES ====================

INSERT INTO matrices (name, description) VALUES
('Security Engineer Matrix', 'Competency matrix for Security Engineers'),
('Security Analyst Matrix', 'Competency matrix for Security Analysts'),
('CISO Matrix', 'Competency matrix for Chief Information Security Officers'),
('Pentester Matrix', 'Competency matrix for Penetration Testers');

-- ==================== INSERT POSITIONS ====================

INSERT INTO positions (name, matrix_id) VALUES
('Security Engineer', 1),
('Security Analyst', 2),
('CISO', 3),
('Pentester', 4),
('Security Architect', 1),
('SOC Analyst', 2);

-- ==================== INSERT EMPLOYEES ====================

INSERT INTO employees (name, position_id, department, email) VALUES
('Иван Петров', 1, 'Information Security', 'ivan.petrov@company.ru'),
('Мария Сидорова', 2, 'Information Security', 'maria.sidorova@company.ru'),
('Алексей Козлов', 4, 'Information Security', 'alexey.kozlov@company.ru'),
('Елена Волкова', 3, 'Management', 'elena.volkova@company.ru'),
('Дмитрий Новиков', 5, 'Information Security', 'dmitry.novikov@company.ru'),
('Ольга Федорова', 6, 'Operations', 'olga.fedorova@company.ru');

-- ==================== INSERT MATRIX COMPETENCIES ====================

-- Security Engineer Matrix (ID: 1)
INSERT INTO matrix_competencies (matrix_id, competence_id, required_level) VALUES
(1, 1, 4),  -- Cryptography: Expert
(1, 2, 3),  -- PKI: Advanced
(1, 3, 4),  -- Network Security: Expert
(1, 5, 2),  -- Penetration Testing: Intermediate
(1, 6, 3),  -- Access Control: Advanced
(1, 7, 3),  -- Incident Response: Advanced
(1, 12, 2); -- Database Security: Intermediate

-- Security Analyst Matrix (ID: 2)
INSERT INTO matrix_competencies (matrix_id, competence_id, required_level) VALUES
(2, 3, 3),  -- Network Security: Advanced
(2, 4, 4),  -- Vulnerability Assessment: Expert
(2, 6, 3),  -- Access Control: Advanced
(2, 7, 4),  -- Incident Response: Expert
(2, 8, 3),  -- Security Audit: Advanced
(2, 9, 2);  -- Risk Management: Intermediate

-- CISO Matrix (ID: 3)
INSERT INTO matrix_competencies (matrix_id, competence_id, required_level) VALUES
(3, 8, 5),  -- Security Audit: Master
(3, 9, 5),  -- Risk Management: Master
(3, 10, 5), -- Information Security Policy: Master
(3, 1, 3),  -- Cryptography: Advanced
(3, 3, 4),  -- Network Security: Expert
(3, 7, 4);  -- Incident Response: Expert

-- Pentester Matrix (ID: 4)
INSERT INTO matrix_competencies (matrix_id, competence_id, required_level) VALUES
(4, 1, 4),  -- Cryptography: Expert
(4, 3, 4),  -- Network Security: Expert
(4, 4, 5),  -- Vulnerability Assessment: Master
(4, 5, 5),  -- Penetration Testing: Master
(4, 6, 3),  -- Access Control: Advanced
(4, 11, 4); -- Secure Coding: Expert

-- ==================== INSERT ASSESSMENTS ====================

-- Ivan Petrov (Security Engineer) - ID: 1
INSERT INTO assessments (employee_id, competence_id, actual_level, comments) VALUES
(1, 1, 4, 'Excellent knowledge of symmetric and asymmetric encryption'),
(1, 2, 3, 'Good understanding of PKI concepts'),
(1, 3, 4, 'Expert in network security principles and practices'),
(1, 5, 2, 'Basic penetration testing skills'),
(1, 6, 3, 'Good access control implementation knowledge'),
(1, 7, 3, 'Experienced in incident response procedures'),
(1, 12, 2, 'Basic database security knowledge');

-- Maria Sidorova (Security Analyst) - ID: 2
INSERT INTO assessments (employee_id, competence_id, actual_level, comments) VALUES
(2, 3, 3, 'Strong network security analysis skills'),
(2, 4, 4, 'Excellent vulnerability assessment capabilities'),
(2, 6, 3, 'Good understanding of authentication mechanisms'),
(2, 7, 4, 'Expert incident responder with proven track record'),
(2, 8, 3, 'Proficient in security auditing'),
(2, 9, 2, 'Basic risk management understanding');

-- Alexey Kozlov (Pentester) - ID: 3
INSERT INTO assessments (employee_id, competence_id, actual_level, comments) VALUES
(3, 1, 4, 'Strong cryptography knowledge'),
(3, 3, 4, 'Expert in network penetration testing'),
(3, 4, 5, 'Master of vulnerability discovery and assessment'),
(3, 5, 5, 'Exceptional penetration testing skills'),
(3, 6, 3, 'Comprehensive access control testing knowledge'),
(3, 11, 4, 'Excellent secure code review skills');

-- Elena Volkova (CISO) - ID: 4
INSERT INTO assessments (employee_id, competence_id, actual_level, comments) VALUES
(4, 8, 5, 'Expert in enterprise security auditing'),
(4, 9, 5, 'Master of risk management frameworks'),
(4, 10, 5, 'Excellent policy creation and governance'),
(4, 1, 3, 'Good cryptography understanding'),
(4, 3, 4, 'Strong network security background'),
(4, 7, 4, 'Experienced incident response leader');

-- Dmitry Novikov (Security Architect) - ID: 5
INSERT INTO assessments (employee_id, competence_id, actual_level, comments) VALUES
(5, 1, 4, 'Expert cryptographic architecture design'),
(5, 2, 4, 'Strong PKI implementation knowledge'),
(5, 3, 4, 'Excellent network security architecture'),
(5, 6, 4, 'Expert in identity and access management architecture'),
(5, 10, 4, 'Good policy development skills'),
(5, 9, 3, 'Advanced risk management knowledge');

-- Olga Fedorova (SOC Analyst) - ID: 6
INSERT INTO assessments (employee_id, competence_id, actual_level, comments) VALUES
(6, 3, 2, 'Basic network security monitoring'),
(6, 4, 2, 'Basic vulnerability assessment'),
(6, 7, 3, 'Good incident response experience'),
(6, 8, 2, 'Basic security audit knowledge'),
(6, 9, 2, 'Basic risk understanding'),
(6, 6, 2, 'Basic access control knowledge');
