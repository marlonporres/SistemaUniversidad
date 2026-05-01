# Sistema Universidad - Gestión Académica (CRUD)

## 📌 Descripción del Proyecto
Este proyecto es un sistema integral desarrollado en **C++** para la gestión académica de una universidad. Implementa operaciones completas de **CRUD** (Crear, Leer, Actualizar, Eliminar) conectándose a una base de datos relacional en **SQL Server** mediante la librería **ODBC**. 

El sistema está diseñado con una arquitectura modular y un menú interactivo en consola, cumpliendo con los estándares de diseño estructurado requeridos para el curso de **Programación I**.

## 🚀 Características Principales
* **Gestión de Alumnos:** Registro, visualización, actualización de datos y eliminación de estudiantes en el sistema.
* **Gestión de Cursos y Secciones:** Administración del catálogo de cursos disponibles y control de las secciones asignadas.
* **Relaciones Complejas:** Manejo de relaciones de muchos a muchos mediante tablas intermedias para las asignaciones académicas.
* **Consultas Avanzadas:** Implementación de consultas SQL complejas utilizando `LEFT JOIN` y funciones de agrupación como `STRING_AGG` para desplegar las asignaciones de forma consolidada y eficiente.
* **Integridad Referencial:** Validaciones de seguridad en el código y en la base de datos para prevenir inconsistencias (ej. bloqueos al intentar borrar datos que están siendo referenciados en otras tablas).
* **Interfaz de Consola Interactiva:** Menú principal robusto que delega operaciones a submenús específicos, manteniendo una experiencia de usuario clara.

## 🛠️ Tecnologías y Herramientas
* **Lenguaje:** C++
* **Base de Datos:** Microsoft SQL Server
* **Conector:** API de ODBC (Open Database Connectivity)
* **Entorno de Desarrollo (IDE):** Microsoft Visual Studio 2022
* **Control de Versiones:** Git y GitHub

## ⚙️ Estructura y Limpieza del Repositorio
El repositorio ha sido optimizado aplicando un archivo `.gitignore` estricto para mantener únicamente el código fuente (`main.cpp`, archivos `.h`, scripts `.sql` y archivos de solución `.sln` / `.vcxproj`), excluyendo todos los binarios de compilación (`Debug/`, `x64/`), cachés locales y la carpeta oculta `.vs`.

---
**Desarrollado por:** Marlon Iván Porres Umanzor
**Universidad Mariano Gálvez de Guatemala**
