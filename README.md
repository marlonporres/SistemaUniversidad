# Sistema de Gestión Universitaria (C++ y SQL Server)

## Descripción
Este es un sistema de consola desarrollado en C++ para la gestión de registros académicos (CRUD). Permite administrar entidades como Alumnos y Cursos conectándose de forma nativa a una base de datos relacional.

## Tecnologías Utilizadas
* **Lenguaje:** C++
* **Base de Datos:** Microsoft SQL Server
* **Conectividad:** API ODBC (Open Database Connectivity) nativa de Windows.

## Requisitos Previos
Para compilar y ejecutar este proyecto en un entorno local, se necesita:
1. Visual Studio con la carga de trabajo "Desarrollo para el escritorio con C++".
2. Microsoft SQL Server (Localhost).
3. **ODBC Driver 18 for SQL Server** instalado en el sistema.

## Estructura de la Base de Datos
El sistema espera una base de datos llamada `universidad` con (al menos) las siguientes tablas:
* `alumnos` (Id, Nombres, Apellidos)
* `cursos` (Id, codigo_curso, nombre)

## Seguridad
El código implementa consultas preparadas (`SQLPrepare` y `SQLBindParameter`) para evitar vulnerabilidades de Inyección SQL al momento de realizar transacciones DML (Insert, Update, Delete).# SistemaUniversidad
