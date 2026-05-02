#include <iostream>
#include <windows.h>
#include <sqlext.h>
#include <string>
#include <locale.h>

#pragma comment(lib, "odbc32.lib")

using namespace std;

// =======================================================================
// 1. FUNCIONES BASE Y DE CONEXIÓN
// =======================================================================

void mostrarError(SQLSMALLINT tipoHandle, SQLHANDLE handle) {
    SQLCHAR sqlState[6], message[256];
    SQLINTEGER nativeError;
    SQLSMALLINT textLength;

    if (SQLGetDiagRec(tipoHandle, handle, 1, sqlState, &nativeError,
        message, sizeof(message), &textLength) == SQL_SUCCESS) {
        cout << "SQLSTATE: " << sqlState << endl;
        cout << "Error: " << message << endl;
    }
}

SQLHDBC conectar() {
    SQLHENV hEnv = NULL;
    SQLHDBC hDbc = NULL;
    SQLRETURN ret;

    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

    SQLCHAR connStr[] =
        "DRIVER={ODBC Driver 18 for SQL Server};"
        "SERVER=localhost;"
        "DATABASE=universidad;"
        "Trusted_Connection=yes;"
        "TrustServerCertificate=yes;";

    SQLCHAR outConnStr[1024];
    SQLSMALLINT outConnStrLen;

    ret = SQLDriverConnect(
        hDbc, NULL, connStr, SQL_NTS,
        outConnStr, sizeof(outConnStr), &outConnStrLen, SQL_DRIVER_NOPROMPT
    );

    if (!(ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)) {
        cout << "Error al conectar." << endl;
        mostrarError(SQL_HANDLE_DBC, hDbc);
        return NULL;
    }
    return hDbc;
}

// =======================================================================
// 2. CRUD ALUMNOS 
// =======================================================================

void listarAlumnos(SQLHDBC hDbc) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    SQLCHAR query[] =
        "SELECT a.id, a.Nombres, a.Apellidos, ISNULL(a.Carnet, 'N/A'), "
        "ISNULL(s.nombre, 'Sin seccion'), "
        "ISNULL(STRING_AGG(c.nombre, ', '), 'Sin cursos') "
        "FROM alumnos a "
        "LEFT JOIN alumnos_seccion ase ON a.id = ase.alumno__id "
        "LEFT JOIN secciones s ON ase.seccion_id = s.id "
        "LEFT JOIN alumnos_cursos ac ON a.id = ac.idAlumnos "
        "LEFT JOIN cursos c ON ac.idCursos = c.id "
        "GROUP BY a.id, a.Nombres, a.Apellidos, a.Carnet, s.nombre";

    SQLRETURN ret = SQLExecDirect(hStmt, query, SQL_NTS);

    if (!(ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)) {
        cout << "Error al consultar alumnos." << endl;
        mostrarError(SQL_HANDLE_STMT, hStmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }

    SQLINTEGER id = 0;
    SQLCHAR nombres[51] = { 0 }, apellidos[51] = { 0 }, carnet[21] = { 0 };
    SQLCHAR seccion[51] = { 0 }, cursosStr[256] = { 0 };

    cout << "\n--- LISTADO COMPLETO DE ALUMNOS (JOIN) ---\n";
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
        SQLGetData(hStmt, 2, SQL_C_CHAR, nombres, sizeof(nombres), NULL);
        SQLGetData(hStmt, 3, SQL_C_CHAR, apellidos, sizeof(apellidos), NULL);
        SQLGetData(hStmt, 4, SQL_C_CHAR, carnet, sizeof(carnet), NULL);
        SQLGetData(hStmt, 5, SQL_C_CHAR, seccion, sizeof(seccion), NULL);
        SQLGetData(hStmt, 6, SQL_C_CHAR, cursosStr, sizeof(cursosStr), NULL);

        cout << "Id: " << id << " | Carnet: " << carnet << " | Alumno: " << nombres << " " << apellidos << "\n";
        cout << "    -> Seccion asignada: " << seccion << "\n";
        cout << "    -> Cursos asignados: " << cursosStr << "\n\n";
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void insertarAlumno(SQLHDBC hDbc, const char* nombres, const char* apellidos, const char* carnet) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLCHAR query[] = "INSERT INTO alumnos (Nombres, Apellidos, Carnet) VALUES (?, ?, ?)";
    SQLPrepare(hStmt, query, SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)nombres, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)apellidos, 0, NULL);
    SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)carnet, 0, NULL);

    if (SQLExecute(hStmt) == SQL_SUCCESS) cout << "Alumno insertado correctamente.\n";
    else mostrarError(SQL_HANDLE_STMT, hStmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void actualizarAlumno(SQLHDBC hDbc, int id, const char* nuevosNombres, const char* nuevosApellidos, const char* nuevoCarnet) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLCHAR query[] = "UPDATE alumnos SET Nombres = ?, Apellidos = ?, Carnet = ? WHERE Id = ?";
    SQLPrepare(hStmt, query, SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)nuevosNombres, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)nuevosApellidos, 0, NULL);
    SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)nuevoCarnet, 0, NULL);
    SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    if (SQLExecute(hStmt) == SQL_SUCCESS) cout << "Alumno actualizado correctamente.\n";
    else mostrarError(SQL_HANDLE_STMT, hStmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void eliminarAlumno(SQLHDBC hDbc, int id) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLCHAR query[] = "DELETE FROM alumnos WHERE Id = ?";
    SQLPrepare(hStmt, query, SQL_NTS);
    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    if (SQLExecute(hStmt) == SQL_SUCCESS) cout << "Alumno eliminado correctamente.\n";
    else mostrarError(SQL_HANDLE_STMT, hStmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

// =======================================================================
// 3. CRUD CURSOS
// =======================================================================

void listarCursos(SQLHDBC hDbc) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLCHAR query[] = "SELECT id, codigo_curso, nombre FROM cursos";

    if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS) {
        SQLINTEGER id = 0;
        SQLCHAR nombre[51] = { 0 }, codigo_curso[21] = { 0 };

        cout << "\n--- LISTADO DE CURSOS ---\n";
        while (SQLFetch(hStmt) == SQL_SUCCESS) {
            SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
            SQLGetData(hStmt, 2, SQL_C_CHAR, codigo_curso, sizeof(codigo_curso), NULL);
            SQLGetData(hStmt, 3, SQL_C_CHAR, nombre, sizeof(nombre), NULL);
            cout << "Id: " << id << " | Codigo: " << codigo_curso << " | Nombre: " << nombre << endl;
        }
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void insertarCurso(SQLHDBC hDbc, const char* codigo, const char* nombre) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLCHAR query[] = "INSERT INTO cursos (codigo_curso, nombre) VALUES (?, ?)";
    SQLPrepare(hStmt, query, SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)codigo, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)nombre, 0, NULL);

    if (SQLExecute(hStmt) == SQL_SUCCESS) cout << "Curso guardado con éxito.\n";
    else mostrarError(SQL_HANDLE_STMT, hStmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void actualizarCurso(SQLHDBC hDbc, int id, const char* nuevoCodigo, const char* nuevoNombre) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLCHAR query[] = "UPDATE cursos SET codigo_curso = ?, nombre = ? WHERE Id = ?";
    SQLPrepare(hStmt, query, SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)nuevoCodigo, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)nuevoNombre, 0, NULL);
    SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    if (SQLExecute(hStmt) == SQL_SUCCESS) cout << "Curso actualizado.\n";
    else mostrarError(SQL_HANDLE_STMT, hStmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void eliminarCurso(SQLHDBC hDbc, int id) {

    SQLHSTMT hStmtCheck = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmtCheck);
    SQLCHAR queryCheck[] = "SELECT COUNT(*) FROM alumnos_cursos WHERE idCursos = ?";
    SQLPrepare(hStmtCheck, queryCheck, SQL_NTS);
    SQLBindParameter(hStmtCheck, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    SQLExecute(hStmtCheck);
    SQLINTEGER conteo = 0;
    if (SQLFetch(hStmtCheck) == SQL_SUCCESS) {
        SQLGetData(hStmtCheck, 1, SQL_C_LONG, &conteo, 0, NULL);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmtCheck);

    if (conteo > 0) {
        cout << "\n[ERROR] No se puede eliminar: El curso tiene " << conteo << " alumno(s) asignado(s).\n";
        return;
    }

    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLCHAR query[] = "DELETE FROM cursos WHERE Id = ?";
    SQLPrepare(hStmt, query, SQL_NTS);
    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    if (SQLExecute(hStmt) == SQL_SUCCESS) cout << "Curso eliminado correctamente.\n";
    else mostrarError(SQL_HANDLE_STMT, hStmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

// =======================================================================
// 4. CRUD SECCIONES 
// =======================================================================

void listarSecciones(SQLHDBC hDbc) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLCHAR query[] = "SELECT id, nombre, jornada FROM secciones";

    if (SQLExecDirect(hStmt, query, SQL_NTS) == SQL_SUCCESS) {
        SQLINTEGER id = 0;
        SQLCHAR nombre[51] = { 0 }, jornada[51] = { 0 };

        cout << "\n--- LISTADO DE SECCIONES ---\n";
        while (SQLFetch(hStmt) == SQL_SUCCESS) {
            SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
            SQLGetData(hStmt, 2, SQL_C_CHAR, nombre, sizeof(nombre), NULL);
            SQLGetData(hStmt, 3, SQL_C_CHAR, jornada, sizeof(jornada), NULL);
            cout << "Id: " << id << " | Nombre: " << nombre << " | Jornada: " << jornada << endl;
        }
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void insertarSeccion(SQLHDBC hDbc, const char* nombre, const char* jornada) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLCHAR query[] = "INSERT INTO secciones (nombre, jornada) VALUES (?, ?)";
    SQLPrepare(hStmt, query, SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)nombre, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)jornada, 0, NULL);

    if (SQLExecute(hStmt) == SQL_SUCCESS) cout << "Sección guardada.\n";
    else mostrarError(SQL_HANDLE_STMT, hStmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void actualizarSeccion(SQLHDBC hDbc, int id, const char* nuevoNombre, const char* nuevaJornada) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLCHAR query[] = "UPDATE secciones SET nombre = ?, jornada = ? WHERE Id = ?";
    SQLPrepare(hStmt, query, SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)nuevoNombre, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)nuevaJornada, 0, NULL);
    SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    if (SQLExecute(hStmt) == SQL_SUCCESS) cout << "Sección actualizada.\n";
    else mostrarError(SQL_HANDLE_STMT, hStmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void eliminarSeccion(SQLHDBC hDbc, int id) {
    
    SQLHSTMT hStmtCheck = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmtCheck);
    SQLCHAR queryCheck[] = "SELECT COUNT(*) FROM alumnos_seccion WHERE seccion_id = ?";
    SQLPrepare(hStmtCheck, queryCheck, SQL_NTS);
    SQLBindParameter(hStmtCheck, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    SQLExecute(hStmtCheck);
    SQLINTEGER conteo = 0;
    if (SQLFetch(hStmtCheck) == SQL_SUCCESS) {
        SQLGetData(hStmtCheck, 1, SQL_C_LONG, &conteo, 0, NULL);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmtCheck);

    if (conteo > 0) {
        cout << "\n[ERROR] No se puede eliminar: Hay " << conteo << " alumno(s) en esta seccion.\n";
        return;
    }

    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLCHAR query[] = "DELETE FROM secciones WHERE Id = ?";
    SQLPrepare(hStmt, query, SQL_NTS);
    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);

    if (SQLExecute(hStmt) == SQL_SUCCESS) cout << "Sección eliminada.\n";
    else mostrarError(SQL_HANDLE_STMT, hStmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

// =======================================================================
// 5. FUNCIONES PARA RELACIONES (Asignaciones)
// =======================================================================

void asignarCursoAlumno(SQLHDBC hDbc, int idAlumno, int idCurso) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLCHAR query[] = "INSERT INTO alumnos_cursos (idAlumnos, idCursos) VALUES (?, ?)";
    SQLPrepare(hStmt, query, SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idAlumno, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idCurso, 0, NULL);

    if (SQLExecute(hStmt) == SQL_SUCCESS) cout << "¡Curso asignado correctamente!\n";
    else mostrarError(SQL_HANDLE_STMT, hStmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void asignarSeccionAlumno(SQLHDBC hDbc, int idAlumno, int idSeccion) {
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLCHAR query[] = "INSERT INTO alumnos_seccion (alumno__id, seccion_id) VALUES (?, ?)";
    SQLPrepare(hStmt, query, SQL_NTS);

    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idAlumno, 0, NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idSeccion, 0, NULL);

    if (SQLExecute(hStmt) == SQL_SUCCESS) cout << "¡Seccion asignada correctamente!\n";
    else mostrarError(SQL_HANDLE_STMT, hStmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}


// =======================================================================
// 6. MENÚS INTERACTIVOS
// =======================================================================

void menuAlumnosInteractivo(SQLHDBC hDbc) {
    int opcion;
    do {
        cout << "\n--- GESTION DE ALUMNOS ---\n";
        cout << "1. Ver Alumnos (Con detalle de Asignaciones)\n";
        cout << "2. Agregar Alumno\n";
        cout << "3. Modificar Alumno\n";
        cout << "4. Eliminar Alumno\n";
        cout << "5. Regresar\n";
        cout << "Elige una opcion: ";
        cin >> opcion;

        if (opcion == 1) {
            listarAlumnos(hDbc);
        }
        else if (opcion == 2) {
            string nombres, apellidos, carnet;
            cout << "Ingrese Nombres: "; cin.ignore(); getline(cin, nombres);
            cout << "Ingrese Apellidos: "; getline(cin, apellidos);
            cout << "Ingrese Carnet: "; getline(cin, carnet);
            insertarAlumno(hDbc, nombres.c_str(), apellidos.c_str(), carnet.c_str());
        }
        else if (opcion == 3) {
            int id; string nombres, apellidos, carnet;
            listarAlumnos(hDbc);
            cout << "Ingrese el ID del alumno a modificar: "; cin >> id;
            cout << "Ingrese Nuevos Nombres: "; cin.ignore(); getline(cin, nombres);
            cout << "Ingrese Nuevos Apellidos: "; getline(cin, apellidos);
            cout << "Ingrese Nuevo Carnet: "; getline(cin, carnet);
            actualizarAlumno(hDbc, id, nombres.c_str(), apellidos.c_str(), carnet.c_str());
        }
        else if (opcion == 4) {
            int id;
            listarAlumnos(hDbc);
            cout << "Ingrese el ID del alumno a eliminar: "; cin >> id;
            eliminarAlumno(hDbc, id);
        }
    } while (opcion != 5);
}

void menuCursosInteractivo(SQLHDBC hDbc) {
    int opcion;
    do {
        cout << "\n--- GESTION DE CURSOS ---\n";
        cout << "1. Ver Cursos\n"; cout << "2. Agregar Curso\n"; cout << "3. Modificar Curso\n";
        cout << "4. Eliminar Curso\n"; cout << "5. Regresar\n";
        cout << "Elige una opcion: "; cin >> opcion;

        if (opcion == 1) listarCursos(hDbc);
        else if (opcion == 2) {
            string codigo, nombre;
            cout << "Ingrese Codigo (ej. ABC-01): "; cin.ignore(); getline(cin, codigo);
            cout << "Ingrese Nombre del Curso: "; getline(cin, nombre);
            insertarCurso(hDbc, codigo.c_str(), nombre.c_str());
        }
        else if (opcion == 3) {
            int id; string codigo, nombre;
            listarCursos(hDbc);
            cout << "ID del curso a modificar: "; cin >> id;
            cout << "Nuevo Codigo: "; cin.ignore(); getline(cin, codigo);
            cout << "Nuevo Nombre: "; getline(cin, nombre);
            actualizarCurso(hDbc, id, codigo.c_str(), nombre.c_str());
        }
        else if (opcion == 4) {
            int id; listarCursos(hDbc);
            cout << "ID del curso a eliminar: "; cin >> id;
            eliminarCurso(hDbc, id);
        }
    } while (opcion != 5);
}

void menuSeccionesInteractivo(SQLHDBC hDbc) {
    int opcion;
    do {
        cout << "\n--- GESTION DE SECCIONES ---\n";
        cout << "1. Ver Secciones\n"; cout << "2. Agregar Seccion\n"; cout << "3. Modificar Seccion\n";
        cout << "4. Eliminar Seccion\n"; cout << "5. Regresar\n";
        cout << "Elige una opcion: "; cin >> opcion;

        if (opcion == 1) listarSecciones(hDbc);
        else if (opcion == 2) {
            string nombre, jornada;
            cout << "Nombre de Seccion: "; cin.ignore(); getline(cin, nombre);
            cout << "Jornada: "; getline(cin, jornada);
            insertarSeccion(hDbc, nombre.c_str(), jornada.c_str());
        }
        else if (opcion == 3) {
            int id; string nombre, jornada;
            listarSecciones(hDbc);
            cout << "ID de la seccion a modificar: "; cin >> id;
            cout << "Nuevo Nombre: "; cin.ignore(); getline(cin, nombre);
            cout << "Nueva Jornada: "; getline(cin, jornada);
            actualizarSeccion(hDbc, id, nombre.c_str(), jornada.c_str());
        }
        else if (opcion == 4) {
            int id; listarSecciones(hDbc);
            cout << "ID de la seccion a eliminar: "; cin >> id;
            eliminarSeccion(hDbc, id);
        }
    } while (opcion != 5);
}

void menuAlumnosCursosInteractivo(SQLHDBC hDbc) {
    int opcion;
    do {
        cout << "\n--- ASIGNACION DE CURSOS ---\n";
        cout << "1. Asignar nuevo curso a un alumno\n";
        cout << "2. Regresar\n";
        cout << "Elige una opcion: "; cin >> opcion;

        if (opcion == 1) {
            int idAlumno, idCurso;
            listarAlumnos(hDbc);
            cout << "Ingrese ID del Alumno: "; cin >> idAlumno;
            listarCursos(hDbc);
            cout << "Ingrese ID del Curso: "; cin >> idCurso;
            asignarCursoAlumno(hDbc, idAlumno, idCurso);
        }
    } while (opcion != 2);
}

void menuAlumnosSeccionesInteractivo(SQLHDBC hDbc) {
    int opcion;
    do {
        cout << "\n--- ASIGNACION DE SECCIONES ---\n";
        cout << "1. Asignar nueva seccion a un alumno\n";
        cout << "2. Regresar\n";
        cout << "Elige una opcion: "; cin >> opcion;

        if (opcion == 1) {
            int idAlumno, idSeccion;
            listarAlumnos(hDbc);
            cout << "Ingrese ID del Alumno: "; cin >> idAlumno;
            listarSecciones(hDbc);
            cout << "Ingrese ID de la Seccion: "; cin >> idSeccion;
            asignarSeccionAlumno(hDbc, idAlumno, idSeccion);
        }
    } while (opcion != 2);
}

// =======================================================================
// 7. PROGRAMA PRINCIPAL
// =======================================================================

int main() {
    setlocale(LC_ALL, "spanish");
    cout << "Iniciando sistema... Conectando a la base de datos...\n";
    SQLHDBC hDbc = conectar();

    if (hDbc == NULL) {
        system("pause");
        return 1;
    }
    cout << "¡Conexion Exitosa!\n";

    int opcionPrincipal;
    do {
        cout << "\n====== SISTEMA UNIVERSIDAD ======\n";
        cout << "1. Alumnos\n";
        cout << "2. Cursos\n";
        cout << "3. Secciones\n";
        cout << "4. Alumnos - Cursos (Asignar)\n";
        cout << "5. Alumnos - Seccion (Asignar)\n";
        cout << "6. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcionPrincipal;

        switch (opcionPrincipal) {
        case 1: menuAlumnosInteractivo(hDbc); break;
        case 2: menuCursosInteractivo(hDbc); break;
        case 3: menuSeccionesInteractivo(hDbc); break;
        case 4: menuAlumnosCursosInteractivo(hDbc); break;
        case 5: menuAlumnosSeccionesInteractivo(hDbc); break;
        case 6: cout << "Cerrando sistema...\n"; break;
        default: cout << "Opcion no valida.\n";
        }
    } while (opcionPrincipal != 6);

    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    return 0;
}