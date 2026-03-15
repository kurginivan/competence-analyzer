--
-- PostgreSQL database dump
--

\restrict 5kY8c5yGDHfFS8aYG28p7hPmZwDQ7Yj1bPqvlhDRdxwczJRktkVL9UeM5ej6imZ

-- Dumped from database version 15.15
-- Dumped by pg_dump version 15.15

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: assessments; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.assessments (
    id integer NOT NULL,
    employee_id integer NOT NULL,
    competence_id integer NOT NULL,
    actual_level integer NOT NULL,
    assessment_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    comments text,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT assessments_actual_level_check CHECK (((actual_level >= 1) AND (actual_level <= 5)))
);


ALTER TABLE public.assessments OWNER TO postgres;

--
-- Name: TABLE assessments; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE public.assessments IS 'Оценки компетенций сотрудников';


--
-- Name: assessments_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.assessments_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.assessments_id_seq OWNER TO postgres;

--
-- Name: assessments_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.assessments_id_seq OWNED BY public.assessments.id;


--
-- Name: competences; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.competences (
    id integer NOT NULL,
    name character varying(255) NOT NULL,
    description text,
    category character varying(100) NOT NULL,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.competences OWNER TO postgres;

--
-- Name: TABLE competences; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE public.competences IS 'Требуемые компетенции в области информационной безопасности';


--
-- Name: competences_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.competences_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.competences_id_seq OWNER TO postgres;

--
-- Name: competences_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.competences_id_seq OWNED BY public.competences.id;


--
-- Name: employees; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.employees (
    id integer NOT NULL,
    name character varying(255) NOT NULL,
    position_id integer,
    department character varying(255) NOT NULL,
    email character varying(255),
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.employees OWNER TO postgres;

--
-- Name: TABLE employees; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE public.employees IS 'Специалисты/сотрудники организации';


--
-- Name: employees_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.employees_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.employees_id_seq OWNER TO postgres;

--
-- Name: employees_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.employees_id_seq OWNED BY public.employees.id;


--
-- Name: matrices; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.matrices (
    id integer NOT NULL,
    name character varying(255) NOT NULL,
    description text,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.matrices OWNER TO postgres;

--
-- Name: TABLE matrices; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE public.matrices IS 'Матрицы компетенций для различных должностей/ролей';


--
-- Name: matrices_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.matrices_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.matrices_id_seq OWNER TO postgres;

--
-- Name: matrices_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.matrices_id_seq OWNED BY public.matrices.id;


--
-- Name: matrix_competencies; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.matrix_competencies (
    id integer NOT NULL,
    matrix_id integer NOT NULL,
    competence_id integer NOT NULL,
    required_level integer NOT NULL,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT matrix_competencies_required_level_check CHECK (((required_level >= 1) AND (required_level <= 5)))
);


ALTER TABLE public.matrix_competencies OWNER TO postgres;

--
-- Name: TABLE matrix_competencies; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE public.matrix_competencies IS 'Связь между матрицами и требуемыми компетенциями с уровнем требований';


--
-- Name: matrix_competencies_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.matrix_competencies_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.matrix_competencies_id_seq OWNER TO postgres;

--
-- Name: matrix_competencies_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.matrix_competencies_id_seq OWNED BY public.matrix_competencies.id;


--
-- Name: positions; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.positions (
    id integer NOT NULL,
    name character varying(255) NOT NULL,
    matrix_id integer,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.positions OWNER TO postgres;

--
-- Name: positions_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.positions_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.positions_id_seq OWNER TO postgres;

--
-- Name: positions_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.positions_id_seq OWNED BY public.positions.id;


--
-- Name: assessments id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.assessments ALTER COLUMN id SET DEFAULT nextval('public.assessments_id_seq'::regclass);


--
-- Name: competences id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.competences ALTER COLUMN id SET DEFAULT nextval('public.competences_id_seq'::regclass);


--
-- Name: employees id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.employees ALTER COLUMN id SET DEFAULT nextval('public.employees_id_seq'::regclass);


--
-- Name: matrices id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.matrices ALTER COLUMN id SET DEFAULT nextval('public.matrices_id_seq'::regclass);


--
-- Name: matrix_competencies id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.matrix_competencies ALTER COLUMN id SET DEFAULT nextval('public.matrix_competencies_id_seq'::regclass);


--
-- Name: positions id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.positions ALTER COLUMN id SET DEFAULT nextval('public.positions_id_seq'::regclass);


--
-- Data for Name: assessments; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.assessments (id, employee_id, competence_id, actual_level, assessment_date, comments, created_at, updated_at) FROM stdin;
1	1	1	4	2026-02-15 15:48:26.875521	Excellent knowledge of symmetric and asymmetric encryption	2026-02-15 15:48:26.875521	2026-02-15 15:48:26.875521
2	1	2	3	2026-02-15 15:48:26.875521	Good understanding of PKI concepts	2026-02-15 15:48:26.875521	2026-02-15 15:48:26.875521
3	1	3	4	2026-02-15 15:48:26.875521	Expert in network security principles and practices	2026-02-15 15:48:26.875521	2026-02-15 15:48:26.875521
4	1	5	2	2026-02-15 15:48:26.875521	Basic penetration testing skills	2026-02-15 15:48:26.875521	2026-02-15 15:48:26.875521
5	1	6	3	2026-02-15 15:48:26.875521	Good access control implementation knowledge	2026-02-15 15:48:26.875521	2026-02-15 15:48:26.875521
6	1	7	3	2026-02-15 15:48:26.875521	Experienced in incident response procedures	2026-02-15 15:48:26.875521	2026-02-15 15:48:26.875521
7	1	12	2	2026-02-15 15:48:26.875521	Basic database security knowledge	2026-02-15 15:48:26.875521	2026-02-15 15:48:26.875521
8	2	3	3	2026-02-15 15:48:26.877356	Strong network security analysis skills	2026-02-15 15:48:26.877356	2026-02-15 15:48:26.877356
9	2	4	4	2026-02-15 15:48:26.877356	Excellent vulnerability assessment capabilities	2026-02-15 15:48:26.877356	2026-02-15 15:48:26.877356
10	2	6	3	2026-02-15 15:48:26.877356	Good understanding of authentication mechanisms	2026-02-15 15:48:26.877356	2026-02-15 15:48:26.877356
11	2	7	4	2026-02-15 15:48:26.877356	Expert incident responder with proven track record	2026-02-15 15:48:26.877356	2026-02-15 15:48:26.877356
12	2	8	3	2026-02-15 15:48:26.877356	Proficient in security auditing	2026-02-15 15:48:26.877356	2026-02-15 15:48:26.877356
13	2	9	2	2026-02-15 15:48:26.877356	Basic risk management understanding	2026-02-15 15:48:26.877356	2026-02-15 15:48:26.877356
14	3	1	4	2026-02-15 15:48:26.878309	Strong cryptography knowledge	2026-02-15 15:48:26.878309	2026-02-15 15:48:26.878309
15	3	3	4	2026-02-15 15:48:26.878309	Expert in network penetration testing	2026-02-15 15:48:26.878309	2026-02-15 15:48:26.878309
16	3	4	5	2026-02-15 15:48:26.878309	Master of vulnerability discovery and assessment	2026-02-15 15:48:26.878309	2026-02-15 15:48:26.878309
17	3	5	5	2026-02-15 15:48:26.878309	Exceptional penetration testing skills	2026-02-15 15:48:26.878309	2026-02-15 15:48:26.878309
18	3	6	3	2026-02-15 15:48:26.878309	Comprehensive access control testing knowledge	2026-02-15 15:48:26.878309	2026-02-15 15:48:26.878309
19	3	11	4	2026-02-15 15:48:26.878309	Excellent secure code review skills	2026-02-15 15:48:26.878309	2026-02-15 15:48:26.878309
20	4	8	5	2026-02-15 15:48:26.879241	Expert in enterprise security auditing	2026-02-15 15:48:26.879241	2026-02-15 15:48:26.879241
21	4	9	5	2026-02-15 15:48:26.879241	Master of risk management frameworks	2026-02-15 15:48:26.879241	2026-02-15 15:48:26.879241
22	4	10	5	2026-02-15 15:48:26.879241	Excellent policy creation and governance	2026-02-15 15:48:26.879241	2026-02-15 15:48:26.879241
23	4	1	3	2026-02-15 15:48:26.879241	Good cryptography understanding	2026-02-15 15:48:26.879241	2026-02-15 15:48:26.879241
24	4	3	4	2026-02-15 15:48:26.879241	Strong network security background	2026-02-15 15:48:26.879241	2026-02-15 15:48:26.879241
25	4	7	4	2026-02-15 15:48:26.879241	Experienced incident response leader	2026-02-15 15:48:26.879241	2026-02-15 15:48:26.879241
26	5	1	4	2026-02-15 15:48:26.880105	Expert cryptographic architecture design	2026-02-15 15:48:26.880105	2026-02-15 15:48:26.880105
27	5	2	4	2026-02-15 15:48:26.880105	Strong PKI implementation knowledge	2026-02-15 15:48:26.880105	2026-02-15 15:48:26.880105
28	5	3	4	2026-02-15 15:48:26.880105	Excellent network security architecture	2026-02-15 15:48:26.880105	2026-02-15 15:48:26.880105
29	5	6	4	2026-02-15 15:48:26.880105	Expert in identity and access management architecture	2026-02-15 15:48:26.880105	2026-02-15 15:48:26.880105
30	5	10	4	2026-02-15 15:48:26.880105	Good policy development skills	2026-02-15 15:48:26.880105	2026-02-15 15:48:26.880105
31	5	9	3	2026-02-15 15:48:26.880105	Advanced risk management knowledge	2026-02-15 15:48:26.880105	2026-02-15 15:48:26.880105
32	6	3	2	2026-02-15 15:48:26.881329	Basic network security monitoring	2026-02-15 15:48:26.881329	2026-02-15 15:48:26.881329
33	6	4	2	2026-02-15 15:48:26.881329	Basic vulnerability assessment	2026-02-15 15:48:26.881329	2026-02-15 15:48:26.881329
34	6	7	3	2026-02-15 15:48:26.881329	Good incident response experience	2026-02-15 15:48:26.881329	2026-02-15 15:48:26.881329
35	6	8	2	2026-02-15 15:48:26.881329	Basic security audit knowledge	2026-02-15 15:48:26.881329	2026-02-15 15:48:26.881329
36	6	9	2	2026-02-15 15:48:26.881329	Basic risk understanding	2026-02-15 15:48:26.881329	2026-02-15 15:48:26.881329
37	6	6	2	2026-02-15 15:48:26.881329	Basic access control knowledge	2026-02-15 15:48:26.881329	2026-02-15 15:48:26.881329
\.


--
-- Data for Name: competences; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.competences (id, name, description, category, created_at, updated_at) FROM stdin;
1	Cryptography	Knowledge of encryption algorithms, hash functions, and digital signatures	Cryptography	2026-02-15 15:48:26.862608	2026-02-15 15:48:26.862608
2	Public Key Infrastructure (PKI)	Understanding of certificates, CA, and key management	Cryptography	2026-02-15 15:48:26.862608	2026-02-15 15:48:26.862608
3	Network Security	Firewalls, intrusion detection systems, VPN, DLP	Network Security	2026-02-15 15:48:26.862608	2026-02-15 15:48:26.862608
4	Vulnerability Assessment	Identifying and assessing security vulnerabilities	Network Security	2026-02-15 15:48:26.862608	2026-02-15 15:48:26.862608
5	Penetration Testing	Authorized security testing and exploitation techniques	Network Security	2026-02-15 15:48:26.862608	2026-02-15 15:48:26.862608
6	Access Control	Authentication, authorization, identity management (IAM)	Access Control	2026-02-15 15:48:26.862608	2026-02-15 15:48:26.862608
7	Incident Response	Detection, analysis, and response to security incidents	Incident Management	2026-02-15 15:48:26.862608	2026-02-15 15:48:26.862608
8	Security Audit	Auditing systems for compliance and security controls	Compliance	2026-02-15 15:48:26.862608	2026-02-15 15:48:26.862608
9	Risk Management	Identifying, analyzing, and mitigating security risks	Compliance	2026-02-15 15:48:26.862608	2026-02-15 15:48:26.862608
10	Information Security Policy	Creating and maintaining security policies and procedures	Governance	2026-02-15 15:48:26.862608	2026-02-15 15:48:26.862608
11	Secure Coding	Writing secure applications and preventing common vulnerabilities	Development	2026-02-15 15:48:26.862608	2026-02-15 15:48:26.862608
12	Database Security	Protecting databases and managing access to sensitive data	Data Protection	2026-02-15 15:48:26.862608	2026-02-15 15:48:26.862608
\.


--
-- Data for Name: employees; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.employees (id, name, position_id, department, email, created_at, updated_at) FROM stdin;
1	Иван Петров	1	Information Security	ivan.petrov@company.ru	2026-02-15 15:48:26.868278	2026-02-15 15:48:26.868278
2	Мария Сидорова	2	Information Security	maria.sidorova@company.ru	2026-02-15 15:48:26.868278	2026-02-15 15:48:26.868278
3	Алексей Козлов	4	Information Security	alexey.kozlov@company.ru	2026-02-15 15:48:26.868278	2026-02-15 15:48:26.868278
4	Елена Волкова	3	Management	elena.volkova@company.ru	2026-02-15 15:48:26.868278	2026-02-15 15:48:26.868278
5	Дмитрий Новиков	5	Information Security	dmitry.novikov@company.ru	2026-02-15 15:48:26.868278	2026-02-15 15:48:26.868278
6	Ольга Федорова	6	Operations	olga.fedorova@company.ru	2026-02-15 15:48:26.868278	2026-02-15 15:48:26.868278
7	Джеффри Энштейн	4	Литл Сент Дж.	jeevacation@gmai.com	2026-02-18 04:15:08.772268	2026-02-18 04:15:08.772268
\.


--
-- Data for Name: matrices; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.matrices (id, name, description, created_at, updated_at) FROM stdin;
1	Security Engineer Matrix	Competency matrix for Security Engineers	2026-02-15 15:48:26.863638	2026-02-15 15:48:26.863638
2	Security Analyst Matrix	Competency matrix for Security Analysts	2026-02-15 15:48:26.863638	2026-02-15 15:48:26.863638
3	CISO Matrix	Competency matrix for Chief Information Security Officers	2026-02-15 15:48:26.863638	2026-02-15 15:48:26.863638
4	Pentester Matrix	Competency matrix for Penetration Testers	2026-02-15 15:48:26.863638	2026-02-15 15:48:26.863638
5	something	test	2026-02-15 15:51:21.126926	2026-02-15 15:51:21.126926
\.


--
-- Data for Name: matrix_competencies; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.matrix_competencies (id, matrix_id, competence_id, required_level, created_at, updated_at) FROM stdin;
1	1	1	4	2026-02-15 15:48:26.870347	2026-02-15 15:48:26.870347
2	1	2	3	2026-02-15 15:48:26.870347	2026-02-15 15:48:26.870347
3	1	3	4	2026-02-15 15:48:26.870347	2026-02-15 15:48:26.870347
4	1	5	2	2026-02-15 15:48:26.870347	2026-02-15 15:48:26.870347
5	1	6	3	2026-02-15 15:48:26.870347	2026-02-15 15:48:26.870347
6	1	7	3	2026-02-15 15:48:26.870347	2026-02-15 15:48:26.870347
7	1	12	2	2026-02-15 15:48:26.870347	2026-02-15 15:48:26.870347
8	2	3	3	2026-02-15 15:48:26.872423	2026-02-15 15:48:26.872423
9	2	4	4	2026-02-15 15:48:26.872423	2026-02-15 15:48:26.872423
10	2	6	3	2026-02-15 15:48:26.872423	2026-02-15 15:48:26.872423
11	2	7	4	2026-02-15 15:48:26.872423	2026-02-15 15:48:26.872423
12	2	8	3	2026-02-15 15:48:26.872423	2026-02-15 15:48:26.872423
13	2	9	2	2026-02-15 15:48:26.872423	2026-02-15 15:48:26.872423
14	3	8	5	2026-02-15 15:48:26.873541	2026-02-15 15:48:26.873541
15	3	9	5	2026-02-15 15:48:26.873541	2026-02-15 15:48:26.873541
16	3	10	5	2026-02-15 15:48:26.873541	2026-02-15 15:48:26.873541
17	3	1	3	2026-02-15 15:48:26.873541	2026-02-15 15:48:26.873541
18	3	3	4	2026-02-15 15:48:26.873541	2026-02-15 15:48:26.873541
19	3	7	4	2026-02-15 15:48:26.873541	2026-02-15 15:48:26.873541
20	4	1	4	2026-02-15 15:48:26.874622	2026-02-15 15:48:26.874622
21	4	3	4	2026-02-15 15:48:26.874622	2026-02-15 15:48:26.874622
22	4	4	5	2026-02-15 15:48:26.874622	2026-02-15 15:48:26.874622
23	4	5	5	2026-02-15 15:48:26.874622	2026-02-15 15:48:26.874622
24	4	6	3	2026-02-15 15:48:26.874622	2026-02-15 15:48:26.874622
25	4	11	4	2026-02-15 15:48:26.874622	2026-02-15 15:48:26.874622
\.


--
-- Data for Name: positions; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.positions (id, name, matrix_id, created_at, updated_at) FROM stdin;
2	Security Analyst	2	2026-02-15 15:48:26.865274	2026-02-15 15:48:26.865274
4	Pentester	4	2026-02-15 15:48:26.865274	2026-02-15 15:48:26.865274
5	Security Architect	1	2026-02-15 15:48:26.865274	2026-02-15 15:48:26.865274
38	Test Pos	5	2026-02-15 15:55:19.020355	2026-02-15 15:55:19.020355
6	SOC Analyst	5	2026-02-15 15:48:26.865274	2026-02-15 15:48:26.865274
1	Security Engineer	5	2026-02-15 15:48:26.865274	2026-02-15 15:48:26.865274
3	CISO	3	2026-02-15 15:48:26.865274	2026-02-15 15:48:26.865274
\.


--
-- Name: assessments_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.assessments_id_seq', 37, true);


--
-- Name: competences_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.competences_id_seq', 12, true);


--
-- Name: employees_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.employees_id_seq', 7, true);


--
-- Name: matrices_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.matrices_id_seq', 5, true);


--
-- Name: matrix_competencies_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.matrix_competencies_id_seq', 25, true);


--
-- Name: positions_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.positions_id_seq', 39, true);


--
-- Name: assessments assessments_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.assessments
    ADD CONSTRAINT assessments_pkey PRIMARY KEY (id);


--
-- Name: competences competences_name_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.competences
    ADD CONSTRAINT competences_name_key UNIQUE (name);


--
-- Name: competences competences_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.competences
    ADD CONSTRAINT competences_pkey PRIMARY KEY (id);


--
-- Name: employees employees_email_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.employees
    ADD CONSTRAINT employees_email_key UNIQUE (email);


--
-- Name: employees employees_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.employees
    ADD CONSTRAINT employees_pkey PRIMARY KEY (id);


--
-- Name: matrices matrices_name_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.matrices
    ADD CONSTRAINT matrices_name_key UNIQUE (name);


--
-- Name: matrices matrices_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.matrices
    ADD CONSTRAINT matrices_pkey PRIMARY KEY (id);


--
-- Name: matrix_competencies matrix_competencies_matrix_id_competence_id_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.matrix_competencies
    ADD CONSTRAINT matrix_competencies_matrix_id_competence_id_key UNIQUE (matrix_id, competence_id);


--
-- Name: matrix_competencies matrix_competencies_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.matrix_competencies
    ADD CONSTRAINT matrix_competencies_pkey PRIMARY KEY (id);


--
-- Name: positions positions_name_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.positions
    ADD CONSTRAINT positions_name_key UNIQUE (name);


--
-- Name: positions positions_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.positions
    ADD CONSTRAINT positions_pkey PRIMARY KEY (id);


--
-- Name: idx_assessments_competence; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_assessments_competence ON public.assessments USING btree (competence_id);


--
-- Name: idx_assessments_employee; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_assessments_employee ON public.assessments USING btree (employee_id);


--
-- Name: idx_competences_category; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_competences_category ON public.competences USING btree (category);


--
-- Name: idx_employees_department; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_employees_department ON public.employees USING btree (department);


--
-- Name: idx_employees_position_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_employees_position_id ON public.employees USING btree (position_id);


--
-- Name: idx_matrix_competencies_competence; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_matrix_competencies_competence ON public.matrix_competencies USING btree (competence_id);


--
-- Name: idx_matrix_competencies_matrix; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_matrix_competencies_matrix ON public.matrix_competencies USING btree (matrix_id);


--
-- Name: assessments assessments_competence_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.assessments
    ADD CONSTRAINT assessments_competence_id_fkey FOREIGN KEY (competence_id) REFERENCES public.competences(id) ON DELETE CASCADE;


--
-- Name: assessments assessments_employee_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.assessments
    ADD CONSTRAINT assessments_employee_id_fkey FOREIGN KEY (employee_id) REFERENCES public.employees(id) ON DELETE CASCADE;


--
-- Name: employees employees_position_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.employees
    ADD CONSTRAINT employees_position_id_fkey FOREIGN KEY (position_id) REFERENCES public.positions(id) ON DELETE SET NULL;


--
-- Name: matrix_competencies matrix_competencies_competence_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.matrix_competencies
    ADD CONSTRAINT matrix_competencies_competence_id_fkey FOREIGN KEY (competence_id) REFERENCES public.competences(id) ON DELETE CASCADE;


--
-- Name: matrix_competencies matrix_competencies_matrix_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.matrix_competencies
    ADD CONSTRAINT matrix_competencies_matrix_id_fkey FOREIGN KEY (matrix_id) REFERENCES public.matrices(id) ON DELETE CASCADE;


--
-- Name: positions positions_matrix_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.positions
    ADD CONSTRAINT positions_matrix_id_fkey FOREIGN KEY (matrix_id) REFERENCES public.matrices(id) ON DELETE SET NULL;


--
-- PostgreSQL database dump complete
--

\unrestrict 5kY8c5yGDHfFS8aYG28p7hPmZwDQ7Yj1bPqvlhDRdxwczJRktkVL9UeM5ej6imZ

