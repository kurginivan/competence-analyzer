import os
import psycopg2
from psycopg2.extras import RealDictCursor
from fastapi import FastAPI, HTTPException
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
from pydantic import BaseModel
from typing import List, Optional

app = FastAPI()

# Database configuration
DB_HOST = os.getenv("DATABASE_HOST", "localhost")
DB_PORT = os.getenv("DATABASE_PORT", "5432")
DB_USER = os.getenv("DATABASE_USER", "postgres")
DB_PASSWORD = os.getenv("DATABASE_PASSWORD", "postgres")
DB_NAME = os.getenv("DATABASE_NAME", "competence_analyzer")

def get_db():
    """Get database connection"""
    try:
        conn = psycopg2.connect(
            host=DB_HOST,
            port=DB_PORT,
            user=DB_USER,
            password=DB_PASSWORD,
            database=DB_NAME,
            connect_timeout=10
        )
        return conn
    except Exception as e:
        print(f"Database connection error: {e}")
        raise HTTPException(status_code=500, detail=f"Database error: {str(e)}")

# Models
class Employee(BaseModel):
    id: Optional[int] = None
    name: str
    position_id: Optional[int] = None
    department: str
    email: str

class Position(BaseModel):
    id: Optional[int] = None
    name: str
    matrix_id: Optional[int] = None

class Competence(BaseModel):
    id: Optional[int] = None
    name: str
    description: str
    category: Optional[str] = None

class Assessment(BaseModel):
    id: Optional[int] = None
    employee_id: int
    competence_id: int
    actual_level: int
    comments: Optional[str] = None

# Routes - Employees
@app.get("/api/employees")
def get_employees():
    """Get all employees with position info"""
    try:
        conn = get_db()
        cur = conn.cursor(cursor_factory=RealDictCursor)
        cur.execute(
            "SELECT e.id, e.name, e.department, e.email, p.id AS position_id, p.name AS position_name, p.matrix_id AS position_matrix_id "
            "FROM employees e LEFT JOIN positions p ON e.position_id = p.id ORDER BY e.name"
        )
        employees = cur.fetchall()
        cur.close()
        conn.close()
        return employees
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/employees/{emp_id}")
def get_employee(emp_id: int):
    """Get single employee with position"""
    try:
        conn = get_db()
        cur = conn.cursor(cursor_factory=RealDictCursor)
        cur.execute(
            "SELECT e.id, e.name, e.department, e.email, p.id AS position_id, p.name AS position_name, p.matrix_id AS position_matrix_id "
            "FROM employees e LEFT JOIN positions p ON e.position_id = p.id WHERE e.id = %s",
            (emp_id,)
        )
        employee = cur.fetchone()
        cur.close()
        conn.close()
        if not employee:
            raise HTTPException(status_code=404, detail="Employee not found")
        return employee
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/employees")
def create_employee(emp: Employee):
    """Create new employee and return the created record (with position info)"""
    try:
        conn = get_db()
        cur = conn.cursor()
        cur.execute(
            "INSERT INTO employees (name, position_id, department, email) VALUES (%s, %s, %s, %s) RETURNING id",
            (emp.name, emp.position_id, emp.department, emp.email)
        )
        emp_id = cur.fetchone()[0]
        conn.commit()
        # Fetch created employee with position info
        cur = conn.cursor(cursor_factory=RealDictCursor)
        cur.execute(
            "SELECT e.id, e.name, e.department, e.email, p.id AS position_id, p.name AS position_name, p.matrix_id AS position_matrix_id "
            "FROM employees e LEFT JOIN positions p ON e.position_id = p.id WHERE e.id = %s",
            (emp_id,)
        )
        employee = cur.fetchone()
        cur.close()
        conn.close()
        return employee
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.put("/api/employees/{emp_id}")
def update_employee(emp_id: int, emp: Employee):
    """Update employee and return the updated record"""
    try:
        conn = get_db()
        cur = conn.cursor()
        cur.execute(
            "UPDATE employees SET name=%s, position_id=%s, department=%s, email=%s WHERE id=%s",
            (emp.name, emp.position_id, emp.department, emp.email, emp_id)
        )
        conn.commit()
        # Fetch updated employee with position info
        cur = conn.cursor(cursor_factory=RealDictCursor)
        cur.execute(
            "SELECT e.id, e.name, e.department, e.email, p.id AS position_id, p.name AS position_name, p.matrix_id AS position_matrix_id "
            "FROM employees e LEFT JOIN positions p ON e.position_id = p.id WHERE e.id = %s",
            (emp_id,)
        )
        employee = cur.fetchone()
        cur.close()
        conn.close()
        return employee
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.delete("/api/employees/{emp_id}")
def delete_employee(emp_id: int):
    """Delete employee"""
    try:
        conn = get_db()
        cur = conn.cursor()
        cur.execute("DELETE FROM employees WHERE id=%s", (emp_id,))
        conn.commit()
        cur.close()
        conn.close()
        return {"status": "deleted"}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# Routes - Positions
@app.get("/api/positions")
def get_positions():
    try:
        conn = get_db()
        cur = conn.cursor(cursor_factory=RealDictCursor)
        cur.execute("SELECT p.id, p.name, p.matrix_id, m.name AS matrix_name FROM positions p LEFT JOIN matrices m ON p.matrix_id = m.id ORDER BY p.name")
        positions = cur.fetchall()
        cur.close()
        conn.close()
        return positions
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/positions")
def create_position(pos: Position):
    try:
        conn = get_db()
        cur = conn.cursor(cursor_factory=RealDictCursor)
        cur.execute(
            "INSERT INTO positions (name, matrix_id) VALUES (%s, %s) RETURNING id, name, matrix_id",
            (pos.name, pos.matrix_id)
        )
        pos_record = cur.fetchone()
        conn.commit()
        cur.close()
        # Fetch with matrix name
        cur = conn.cursor(cursor_factory=RealDictCursor)
        cur.execute(
            "SELECT p.id, p.name, p.matrix_id, m.name AS matrix_name FROM positions p LEFT JOIN matrices m ON p.matrix_id = m.id WHERE p.id = %s",
            (pos_record['id'],)
        )
        result = cur.fetchone()
        cur.close()
        conn.close()
        return result
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.put("/api/positions/{pos_id}")
def update_position(pos_id: int, pos: Position):
    try:
        conn = get_db()
        cur = conn.cursor()
        cur.execute(
            "UPDATE positions SET name=%s, matrix_id=%s WHERE id=%s",
            (pos.name, pos.matrix_id, pos_id)
        )
        conn.commit()
        # Fetch updated record
        cur = conn.cursor(cursor_factory=RealDictCursor)
        cur.execute(
            "SELECT p.id, p.name, p.matrix_id, m.name AS matrix_name FROM positions p LEFT JOIN matrices m ON p.matrix_id = m.id WHERE p.id = %s",
            (pos_id,)
        )
        result = cur.fetchone()
        cur.close()
        conn.close()
        return result
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/positions/by-matrix/{matrix_id}")
def get_positions_by_matrix(matrix_id: int):
    """Get all positions for a specific matrix"""
    try:
        conn = get_db()
        cur = conn.cursor(cursor_factory=RealDictCursor)
        cur.execute(
            "SELECT p.id, p.name, p.matrix_id, m.name AS matrix_name FROM positions p LEFT JOIN matrices m ON p.matrix_id = m.id WHERE p.matrix_id = %s ORDER BY p.name",
            (matrix_id,)
        )
        positions = cur.fetchall()
        cur.close()
        conn.close()
        return positions
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.delete("/api/positions/{pos_id}")
def delete_position(pos_id: int):
    try:
        conn = get_db()
        cur = conn.cursor()
        cur.execute("DELETE FROM positions WHERE id=%s", (pos_id,))
        conn.commit()
        cur.close()
        conn.close()
        return {"status": "deleted"}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# Routes - Competences
@app.get("/api/competences")
def get_competences():
    """Get all competences"""
    try:
        conn = get_db()
        cur = conn.cursor(cursor_factory=RealDictCursor)
        cur.execute("SELECT * FROM competences ORDER BY id")
        competences = cur.fetchall()
        cur.close()
        conn.close()
        return competences
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/competences")
def create_competence(comp: Competence):
    """Create new competence"""
    try:
        conn = get_db()
        cur = conn.cursor()
        cur.execute(
            "INSERT INTO competences (name, description, category) VALUES (%s, %s, %s) RETURNING id",
            (comp.name, comp.description, comp.category or "General")
        )
        comp_id = cur.fetchone()[0]
        conn.commit()
        cur.close()
        conn.close()
        return {"id": comp_id, **comp.dict()}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.delete("/api/competences/{comp_id}")
def delete_competence(comp_id: int):
    """Delete competence"""
    try:
        conn = get_db()
        cur = conn.cursor()
        cur.execute("DELETE FROM competences WHERE id=%s", (comp_id,))
        conn.commit()
        cur.close()
        conn.close()
        return {"status": "deleted"}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# Routes - Matrices
class Matrix(BaseModel):
    id: Optional[int] = None
    name: str
    description: Optional[str] = None

class MatrixCompetencyIn(BaseModel):
    competence_id: int
    required_level: int

@app.get("/api/matrices")
def get_matrices():
    try:
        conn = get_db()
        cur = conn.cursor(cursor_factory=RealDictCursor)
        cur.execute("SELECT * FROM matrices ORDER BY name")
        matrices = cur.fetchall()
        cur.close()
        conn.close()
        return matrices
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/matrices")
def create_matrix(m: Matrix):
    try:
        conn = get_db()
        cur = conn.cursor()
        cur.execute(
            "INSERT INTO matrices (name, description) VALUES (%s, %s) RETURNING id",
            (m.name, m.description or "")
        )
        mat_id = cur.fetchone()[0]
        conn.commit()
        cur.close()
        conn.close()
        return {"id": mat_id, **m.dict()}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.delete("/api/matrices/{matrix_id}")
def delete_matrix(matrix_id: int):
    try:
        conn = get_db()
        cur = conn.cursor()
        cur.execute("DELETE FROM matrices WHERE id=%s", (matrix_id,))
        conn.commit()
        cur.close()
        conn.close()
        return {"status": "deleted"}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/matrices/{matrix_id}/competencies")
def get_matrix_competencies(matrix_id: int):
    try:
        conn = get_db()
        cur = conn.cursor(cursor_factory=RealDictCursor)
        cur.execute(
            "SELECT mc.id, mc.competence_id, c.name AS competence_name, mc.required_level FROM matrix_competencies mc JOIN competences c ON mc.competence_id = c.id WHERE mc.matrix_id = %s ORDER BY c.name",
            (matrix_id,)
        )
        items = cur.fetchall()
        cur.close()
        conn.close()
        return items
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/matrices/{matrix_id}/competencies")
def add_competence_to_matrix(matrix_id: int, item: MatrixCompetencyIn):
    try:
        conn = get_db()
        cur = conn.cursor()
        cur.execute(
            "INSERT INTO matrix_competencies (matrix_id, competence_id, required_level) VALUES (%s, %s, %s) RETURNING id",
            (matrix_id, item.competence_id, item.required_level)
        )
        mc_id = cur.fetchone()[0]
        conn.commit()
        cur.close()
        conn.close()
        return {"id": mc_id, **item.dict()}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.delete("/api/matrices/{matrix_id}/competencies/{competence_id}")
def remove_competence_from_matrix(matrix_id: int, competence_id: int):
    try:
        conn = get_db()
        cur = conn.cursor()
        cur.execute("DELETE FROM matrix_competencies WHERE matrix_id=%s AND competence_id=%s", (matrix_id, competence_id))
        conn.commit()
        cur.close()
        conn.close()
        return {"status": "deleted"}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# Routes - Analysis & Reports
@app.get("/api/analysis/employee/{emp_id}")
def analyze_employee(emp_id: int):
    try:
        conn = get_db()
        cur = conn.cursor(cursor_factory=RealDictCursor)
        # Employee basic info with position
        cur.execute("SELECT e.id, e.name, e.department, e.email, p.id AS position_id, p.name AS position_name, p.matrix_id AS position_matrix_id FROM employees e LEFT JOIN positions p ON e.position_id = p.id WHERE e.id=%s", (emp_id,))
        emp = cur.fetchone()
        if not emp:
            raise HTTPException(status_code=404, detail="Employee not found")

        # Average level per competence for employee
        cur.execute("""
            SELECT c.id AS competence_id, c.name AS competence_name, ROUND(AVG(a.actual_level)::numeric,2) AS avg_level
            FROM assessments a
            JOIN competences c ON a.competence_id = c.id
            WHERE a.employee_id = %s
            GROUP BY c.id, c.name
            ORDER BY avg_level ASC
        """, (emp_id,))
        averages = cur.fetchall()

        # Gaps: avg_level < 3
        gaps = [row for row in averages if float(row['avg_level']) < 3]

        # Recommendations: top 5 lowest avg levels
        recommendations = averages[:5]

        # Position matching percentage (if position -> matrix defined)
        match_percentage = None
        if emp.get('position_matrix_id'):
            cur.execute("SELECT competence_id, required_level FROM matrix_competencies WHERE matrix_id = %s", (emp['position_matrix_id'],))
            requirements = cur.fetchall()
            if requirements:
                total = len(requirements)
                met = 0
                # build a map from competence_id to avg_level
                avg_map = {int(r['competence_id']): float(r['avg_level']) for r in averages}
                for req in requirements:
                    cid = int(req['competence_id'])
                    req_level = int(req['required_level'])
                    avg_level = avg_map.get(cid, 0.0)
                    if avg_level >= req_level:
                        met += 1
                match_percentage = round((met / total) * 100.0, 2)
        cur.close()
        conn.close()
        return {"employee": emp, "averages": averages, "gaps": gaps, "recommendations": recommendations, "position_match_percentage": match_percentage}
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/reports/employee/{emp_id}")
def generate_employee_report(emp_id: int, matrix_id: Optional[int] = None):
    try:
        if matrix_id:
            # Analyze with specific matrix
            analysis_response = analyze_with_matrix(emp_id, {'matrix_id': matrix_id})
            emp = analysis_response['employee']
            averages = analysis_response['averages']
            gaps = analysis_response['gaps']
            recommendations = analysis_response['recommendations']
            match = analysis_response.get('match_percentage')
        else:
            # Analyze with position's matrix or all
            analysis = analyze_employee(emp_id)
            emp = analysis['employee']
            averages = analysis['averages']
            gaps = analysis['gaps']
            recommendations = analysis['recommendations']
            match = analysis.get('position_match_percentage')

        lines = []
        pos_name = emp.get('position_name') or 'Без должности'
        lines.append(f"Отчёт по сотруднику: {emp['name']} ({pos_name} - {emp.get('department','')})")
        if match is not None:
            lines.append(f"Соответствие матрице: {match}%")
        lines.append("")
        lines.append("Средние уровни по компетенциям:")
        for row in averages:
            lines.append(f"- {row['competence_name']}: {row['avg_level']}/5")
        lines.append("")
        lines.append("Пробелы (уровень < 3):")
        if gaps:
            for r in gaps:
                lines.append(f"- {r['competence_name']}: {r['avg_level']}/5")
        else:
            lines.append("- Пробелов не выявлено")
        lines.append("")
        lines.append("Рекомендации (топ-5):")
        for r in recommendations:
            lines.append(f"- {r['competence_name']}: {r['avg_level']}/5")

        report_text = "\n".join(lines)
        return {"report": report_text}
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/employees/{emp_id}/available-matrices")
def get_available_matrices(emp_id: int):
    """Get matrices available for employee analysis (based on position match or all if no position)"""
    try:
        conn = get_db()
        cur = conn.cursor(cursor_factory=RealDictCursor)
        
        # Get employee and their position
        cur.execute(
            "SELECT e.id, e.position_id FROM employees e WHERE e.id=%s",
            (emp_id,)
        )
        emp = cur.fetchone()
        if not emp:
            raise HTTPException(status_code=404, detail="Employee not found")
        
        # If employee has a position, find matrices for that position
        available_matrices = []
        if emp.get('position_id'):
            cur.execute(
                "SELECT DISTINCT m.id, m.name, m.description FROM matrices m "
                "JOIN positions p ON m.id = p.matrix_id WHERE p.id=%s",
                (emp['position_id'],)
            )
            available_matrices = cur.fetchall()
        
        # If no position-specific matrices, return all matrices
        if not available_matrices:
            cur.execute(
                "SELECT id, name, description FROM matrices ORDER BY name"
            )
            available_matrices = cur.fetchall()
        
        cur.close()
        conn.close()
        return {
            "position_id": emp.get('position_id'),
            "matrices": available_matrices
        }
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/employees/{emp_id}/analyze")
def analyze_with_matrix(emp_id: int, body: dict):
    """Analyze employee against specific matrix. Body: {matrix_id: int}"""
    try:
        matrix_id = body.get('matrix_id')
        if not matrix_id:
            raise HTTPException(status_code=400, detail="matrix_id required")
        
        conn = get_db()
        cur = conn.cursor(cursor_factory=RealDictCursor)
        
        # Get employee info
        cur.execute(
            "SELECT e.id, e.name, e.department, e.email, p.id AS position_id, p.name AS position_name "
            "FROM employees e LEFT JOIN positions p ON e.position_id = p.id WHERE e.id=%s",
            (emp_id,)
        )
        emp = cur.fetchone()
        if not emp:
            raise HTTPException(status_code=404, detail="Employee not found")
        
        # Get employee's competency assessments (averages)
        cur.execute("""
            SELECT c.id AS competence_id, c.name AS competence_name, ROUND(AVG(a.actual_level)::numeric,2) AS avg_level
            FROM assessments a
            JOIN competences c ON a.competence_id = c.id
            WHERE a.employee_id = %s
            GROUP BY c.id, c.name
            ORDER BY avg_level ASC
        """, (emp_id,))
        averages = cur.fetchall()
        
        # Get matrix requirements
        cur.execute(
            "SELECT competence_id, required_level FROM matrix_competencies WHERE matrix_id = %s",
            (matrix_id,)
        )
        requirements = cur.fetchall()
        
        # Calculate match
        match_percentage = None
        unmet_competencies = []
        if requirements:
            total = len(requirements)
            met = 0
            avg_map = {int(r['competence_id']): float(r['avg_level']) for r in averages}
            
            for req in requirements:
                cid = int(req['competence_id'])
                req_level = int(req['required_level'])
                avg_level = avg_map.get(cid, 0.0)
                
                if avg_level >= req_level:
                    met += 1
                else:
                    # Find competence name
                    comp_name = next((r['competence_name'] for r in averages if int(r['competence_id']) == cid), f"ID:{cid}")
                    unmet_competencies.append({
                        "competence_id": cid,
                        "competence_name": comp_name,
                        "required_level": req_level,
                        "actual_level": avg_level
                    })
            
            match_percentage = round((met / total) * 100.0, 2)
        
        # Get gaps and recommendations
        gaps = [row for row in averages if float(row['avg_level']) < 3]
        recommendations = averages[:5]
        
        cur.close()
        conn.close()
        
        return {
            "employee": emp,
            "matrix_id": matrix_id,
            "averages": averages,
            "gaps": gaps,
            "recommendations": recommendations,
            "match_percentage": match_percentage,
            "unmet_competencies": unmet_competencies
        }
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# Routes - Assessments
@app.get("/api/assessments")
def get_assessments(position_id: Optional[int] = None):
    """Get all assessments, optional filter by position_id"""
    try:
        conn = get_db()
        cur = conn.cursor(cursor_factory=RealDictCursor)
        base_query = """
            SELECT a.*, e.name as employee_name, p.id as position_id, p.name as position_name, c.name as competence_name
            FROM assessments a
            JOIN employees e ON a.employee_id = e.id
            LEFT JOIN positions p ON e.position_id = p.id
            JOIN competences c ON a.competence_id = c.id
        """
        params = ()
        if position_id:
            base_query += " WHERE p.id = %s"
            params = (position_id,)
        base_query += " ORDER BY a.id DESC"
        cur.execute(base_query, params)
        assessments = cur.fetchall()
        cur.close()
        conn.close()
        return assessments
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/assessments")
def create_assessment(ass: Assessment):
    """Create new assessment and return created row with employee/competence names"""
    try:
        conn = get_db()
        cur = conn.cursor()
        cur.execute(
            "INSERT INTO assessments (employee_id, competence_id, actual_level, comments) VALUES (%s, %s, %s, %s) RETURNING id",
            (ass.employee_id, ass.competence_id, ass.actual_level, ass.comments or "")
        )
        ass_id = cur.fetchone()[0]
        conn.commit()
        # Fetch created assessment with related names
        cur = conn.cursor(cursor_factory=RealDictCursor)
        cur.execute(
            "SELECT a.*, e.name AS employee_name, c.name AS competence_name FROM assessments a "
            "JOIN employees e ON a.employee_id = e.id JOIN competences c ON a.competence_id = c.id WHERE a.id = %s",
            (ass_id,)
        )
        ass_row = cur.fetchone()
        cur.close()
        conn.close()
        return ass_row
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.delete("/api/assessments/{ass_id}")
def delete_assessment(ass_id: int):
    """Delete assessment"""
    try:
        conn = get_db()
        cur = conn.cursor()
        cur.execute("DELETE FROM assessments WHERE id=%s", (ass_id,))
        conn.commit()
        cur.close()
        conn.close()
        return {"status": "deleted"}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# Health check
@app.get("/api/health")
def health_check():
    """Health check endpoint"""
    try:
        conn = get_db()
        cur = conn.cursor()
        cur.execute("SELECT 1")
        cur.close()
        conn.close()
        return {"status": "ok", "database": "connected"}
    except Exception as e:
        return {"status": "error", "database": str(e)}

# Static files
app.mount("/", StaticFiles(directory="static", html=True), name="static")

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
