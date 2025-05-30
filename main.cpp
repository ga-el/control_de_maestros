#include <algorithm>
#include <conio.h>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits> // Necesario para std::numeric_limits
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

// Declaraciones de funciones
void setUTF8();
std::vector<std::string> split(const std::string &linea, char delim);
void mostrarMenu();
void mostrarCuadriculaAsistencias();
void modificarAsistencias();
void gestionarFechas();
void agregarAlumno();
void esperar(int milisegundos);
void mostrarTextoAnimado(const std::string &texto, int color);
void mostrarBarraProgreso(int duracion);
void mostrarCuadriculaTrabajos();
void modificarTrabajos();
void agregarTrabajo();
void agregarTrabajoConFecha();
void gestionarTrabajos();
void ordenarAlumnosYArchivos(const std::vector<std::string> &alumnosOrdenados);
void gestionarCriteriosEvaluacion();
void agregarCriteriosEvaluacion();
void modificarCriteriosEvaluacion();
bool existenCriteriosEvaluacion();
void mostrarTablaCriterios();
void modificarCalificacionAlumno();
void calcularPromedioParcial();
void calcularPromedioSemestre();
void gen();
bool validarEntradaNumerica(const std::string& entrada, float& valor, float min, float max);
bool validarFormatoFecha(const std::string& fecha);
bool archivoExiste(const std::string& nombreArchivo);
bool eliminarArchivo(const std::string& nombreArchivo);
bool renombrarArchivo(const std::string& viejoNombre, const std::string& nuevoNombre);

// Agregar después de las declaraciones de funciones existentes
std::string obtenerApellido(const std::string &nombreCompleto) {
  std::stringstream ss(nombreCompleto);
  std::string apellido;
  ss >> apellido; // Obtiene la primera palabra (apellido)
  return apellido;
}

bool compararPorApellido(const std::string &a, const std::string &b) {
  return obtenerApellido(a) < obtenerApellido(b);
}

void setUTF8() { system("chcp 65001 > nul"); }

std::vector<std::string> split(const std::string &linea, char delim) {
  std::stringstream ss(linea);
  std::string parte;
  std::vector<std::string> resultado;

  while (getline(ss, parte, delim)) {
    resultado.push_back(parte);
  }
  return resultado;
}

void mostrarMenu() {
  system("cls");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); // Color celeste
  std::cout << "===== SISTEMA DE GESTION ESCOLAR =====\n\n";

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
  std::cout << "=== CONSULTAS ===\n";
  std::cout << "1.  Lista de asistencias\n";
  std::cout << "2.  Lista de trabajos\n\n";

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
  std::cout << "=== REGISTROS ===\n";
  std::cout << "3.  Registrar asistencias\n";
  std::cout << "4.  Registrar trabajos\n";
  std::cout << "5.  Agregar alumnos\n";
  std::cout << "6.  Agregar trabajos\n\n";

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta
  std::cout << "=== GESTION ===\n";
  std::cout << "7.  Gestionar Fechas\n";
  std::cout << "8.  Gestionar Trabajos\n";
  std::cout << "9.  Gestionar Criterios de Evaluacion\n";
  std::cout << "10. Modificar Calificacion de Alumno\n";
  std::cout << "11. Modificar Asistencia de Alumno\n\n";

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          14); // Amarillo - Promedios en consultas
  std::cout << "=== PROMEDIOS ===\n";
  std::cout << "12. Calcular Promedio Parcial\n";
  std::cout << "13. Calcular Promedio Semestre\n\n";

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3); // Cian
  std::cout << "0.  Salir\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
}

void mostrarCuadriculaAsistencias() {
  std::ifstream archivoAlumnos("alumnos.txt");
  std::ifstream archivoFechas("fechas.txt");
  std::ifstream archivoAsistencias("asistencias.txt");

  system("cls");
  mostrarTextoAnimado("=== CARGANDO LISTA DE ASISTENCIAS ===", 11);
  esperar(500);

  if (!archivoAlumnos.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
    esperar(1000);
    return;
  }
  if (!archivoFechas.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de fechas.", 12);
    esperar(1000);
    return;
  }
  if (!archivoAsistencias.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de asistencias.", 12);
    esperar(1000);
    return;
  }

  std::vector<std::string> alumnos;
  std::string linea;
  while (getline(archivoAlumnos, linea)) {
    alumnos.push_back(linea);
  }
  archivoAlumnos.close();

  std::vector<std::string> fechas;
  std::string lineaFechas;
  if (getline(archivoFechas, lineaFechas)) {
    fechas = split(lineaFechas, ',');
    if (!fechas.empty() && fechas.back().empty()) {
      fechas.pop_back();
    }
  }
  archivoFechas.close();

  std::vector<std::vector<std::string>> asistencias;
  while (getline(archivoAsistencias, linea)) {
    auto datos = split(linea, ',');
    asistencias.push_back(datos);
  }
  archivoAsistencias.close();

  // Calcular el ancho máximo del nombre
  size_t maxAnchoNombre = 0;
  for (const auto& alumno : alumnos) {
    if (alumno.length() > maxAnchoNombre) {
      maxAnchoNombre = alumno.length();
    }
  }
  const int ANCHO_NOMBRE = std::max(30, static_cast<int>(maxAnchoNombre) + 2);
  const int ANCHO_COLUMNA_DATOS = 15;

  // Calcular cuántas columnas caben en la pantalla
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  int anchoPantalla = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  int columnasVisibles = (anchoPantalla - ANCHO_NOMBRE - 2) / (ANCHO_COLUMNA_DATOS + 1);

  int inicioColumnas = 0;
  bool salir = false;

  while (!salir) {
    system("cls");
    mostrarTextoAnimado("=== LISTA DE ASISTENCIAS ===", 14);
    esperar(300);

    // Dibujar encabezado de la tabla
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i) std::cout << "-";
    std::cout << "+";
    for (int i = 0; i < std::min(columnasVisibles, static_cast<int>(fechas.size()) - inicioColumnas); ++i) {
      for (int j = 0; j < ANCHO_COLUMNA_DATOS; ++j) std::cout << "-";
      std::cout << "+";
    }
    std::cout << "\n";

    // Fila 1: "Nombre" y Fechas
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "|";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
    std::cout << std::left << std::setw(ANCHO_NOMBRE) << "Nombre";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "|";
    for (int i = 0; i < std::min(columnasVisibles, static_cast<int>(fechas.size()) - inicioColumnas); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      std::string fecha = fechas[inicioColumnas + i];
      int padding = ANCHO_COLUMNA_DATOS - fecha.length();
      int pl = padding / 2, pr = padding - pl;
      for (int k = 0; k < pl; ++k) std::cout << " ";
      std::cout << fecha;
      for (int k = 0; k < pr; ++k) std::cout << " ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cout << "|";
    }
    std::cout << "\n";

    // Línea divisoria
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i) std::cout << "-";
    std::cout << "+";
    for (int i = 0; i < std::min(columnasVisibles, static_cast<int>(fechas.size()) - inicioColumnas); ++i) {
      for (int j = 0; j < ANCHO_COLUMNA_DATOS; ++j) std::cout << "-";
      std::cout << "+";
    }
    std::cout << "\n";

    // Filas de asistencia
    for (size_t i = 0; i < alumnos.size(); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cout << "|";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
      std::cout << std::left << std::setw(ANCHO_NOMBRE) << alumnos[i];
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cout << "|";

      for (int j = 0; j < std::min(columnasVisibles, static_cast<int>(fechas.size()) - inicioColumnas); ++j) {
        std::string valor = " ";
        int colorValor = 15;

        if (i < asistencias.size() && inicioColumnas + j < asistencias[i].size()) {
          valor = (asistencias[i][inicioColumnas + j] == "1") ? "Sí" : "No";
          colorValor = (asistencias[i][inicioColumnas + j] == "1") ? 10 : 12;
        }

        int padding = ANCHO_COLUMNA_DATOS - valor.length();
        int pl = padding / 2, pr = padding - pl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        for (int k = 0; k < pl; ++k) std::cout << " ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorValor);
        std::cout << valor;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        for (int k = 0; k < pr; ++k) std::cout << " ";
        std::cout << "|";
      }

      std::cout << "\n";
    }

    // Línea inferior
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i) std::cout << "-";
    std::cout << "+";
    for (int i = 0; i < std::min(columnasVisibles, static_cast<int>(fechas.size()) - inicioColumnas); ++i) {
      for (int j = 0; j < ANCHO_COLUMNA_DATOS; ++j) std::cout << "-";
      std::cout << "+";
    }
    std::cout << "\n";

    // Instrucciones de navegación
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
    std::cout << "\nFlechas Izq/Der: Navegar | ENTER: Regresar al Menu Principal\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    // Manejo de teclas
    int tecla = _getch();
    if (tecla == 224) { // Tecla de flecha
      tecla = _getch();
      if (tecla == 75 && inicioColumnas > 0) { // Flecha izquierda
        inicioColumnas--;
      } else if (tecla == 77 && inicioColumnas + columnasVisibles < fechas.size()) { // Flecha derecha
        inicioColumnas++;
      }
    } else if (tecla == 13) { // ENTER
      salir = true;
    }
  }
}

void esperar(int milisegundos) { Sleep(milisegundos); }

void mostrarTextoAnimado(const std::string &texto, int color) {
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
  for (char c : texto) {
    std::cout << c;
    std::cout.flush();
    esperar(20);
  }
  std::cout << "\n";
}

void mostrarBarraProgreso(int duracion) {
  std::cout << "[";
  for (int i = 0; i < 50; i++) {
    std::cout << "=";
    std::cout.flush();
    esperar(duracion / 50);
  }
  std::cout << "]\n";
}

void modificarAsistencias() {
    system("cls");
    mostrarTextoAnimado("=== MODIFICAR ASISTENCIAS ===", 14);
    esperar(500);

    std::ifstream archivoAlumnos("alumnos.txt");
    if (!archivoAlumnos.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
        esperar(1000);
        return;
    }

    std::vector<std::string> alumnos;
    std::string linea;
    while (getline(archivoAlumnos, linea)) {
        alumnos.push_back(linea);
    }
    archivoAlumnos.close();

    if (alumnos.empty()) {
        mostrarTextoAnimado("No hay alumnos registrados.", 12);
        esperar(1000);
        return;
    }

    std::ifstream archivoFechas("fechas.txt");
    if (!archivoFechas.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de fechas.", 12);
        esperar(1000);
        return;
    }

    std::string lineaFechas;
    std::vector<std::string> fechas;
    if (getline(archivoFechas, lineaFechas)) {
        fechas = split(lineaFechas, ',');
        if (!fechas.empty() && fechas.back().empty()) {
            fechas.pop_back();
        }
    }
    archivoFechas.close();

    if (fechas.empty()) {
        mostrarTextoAnimado("No hay fechas registradas.", 12);
        esperar(1000);
        return;
    }

    std::cout << "\nSeleccione la fecha:\n\n";
    for (size_t i = 0; i < fechas.size(); ++i) {
        std::cout << i + 1 << ". " << fechas[i] << "\n";
    }

    float seleccionFecha;
    std::string entrada;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer before the loop
    do {
        std::cout << "\nIngrese el numero de la fecha (o 0 para regresar): ";
        std::getline(std::cin, entrada);
        if (!validarEntradaNumerica(entrada, seleccionFecha, 0, fechas.size())) {
            mostrarTextoAnimado("Seleccion invalida. Intente de nuevo.", 12);
            continue;
        }
        break;
    } while (true);

    if (seleccionFecha == 0) {
        mostrarTextoAnimado("Regresando al menu principal...", 13);
        esperar(500);
        return;
    }

    std::ifstream archivoAsistencias("asistencias.txt");
    if (!archivoAsistencias.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de asistencias.", 12);
        esperar(1000);
        return;
    }

    std::vector<std::vector<std::string>> asistencias;
    while (getline(archivoAsistencias, linea)) {
        asistencias.push_back(split(linea, ','));
    }
    archivoAsistencias.close();

    system("cls");
    mostrarTextoAnimado("=== REGISTRO DE ASISTENCIAS ===", 14);
    mostrarTextoAnimado("Fecha: " + fechas[static_cast<int>(seleccionFecha) - 1], 11);

    for (size_t i = 0; i < alumnos.size(); ++i) {
        std::cout << "\n" << alumnos[i] << "\n";
        std::cout << "Asistencia (1 = Presente, 0 = Ausente): ";

        float asistencia;
        do {
            std::getline(std::cin, entrada);
            if (!validarEntradaNumerica(entrada, asistencia, 0, 1)) {
                mostrarTextoAnimado("Valor invalido. Debe ser 0 o 1.", 12);
                std::cout << "Asistencia (1 = Presente, 0 = Ausente): ";
                continue;
            }
            break;
        } while (true);

        if (i < asistencias.size() && static_cast<int>(seleccionFecha) - 1 < asistencias[i].size()) {
            asistencias[i][static_cast<int>(seleccionFecha) - 1] = std::to_string(static_cast<int>(asistencia));
        } else {
            mostrarTextoAnimado("Error: Datos incompletos para este alumno.", 12);
        }
    }

    std::ofstream archivoAsistenciasOut("asistencias.txt");
    if (!archivoAsistenciasOut.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de asistencias para escritura.", 12);
        esperar(1000);
        return;
    }

    for (const auto& fila : asistencias) {
        for (size_t i = 0; i < fila.size(); ++i) {
            archivoAsistenciasOut << fila[i];
            if (i < fila.size() - 1) {
                archivoAsistenciasOut << ",";
            }
        }
        archivoAsistenciasOut << "\n";
    }
    archivoAsistenciasOut.close();

    mostrarTextoAnimado("Asistencias actualizadas correctamente.", 10);
    esperar(1000);
}

void gestionarFechas() {
    system("cls");
    mostrarTextoAnimado("=== GESTIONAR FECHAS ===", 14);
    esperar(500);

    std::ifstream archivoFechas("fechas.txt");
    if (!archivoFechas.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de fechas.", 12);
        esperar(1000);
        return;
    }

    std::string linea;
    std::vector<std::string> fechas;
    if (getline(archivoFechas, linea)) {
        fechas = split(linea, ',');
        if (!fechas.empty() && fechas.back().empty()) {
            fechas.pop_back();
        }
    }
    archivoFechas.close();

    std::cout << "\nFechas actuales:\n\n";
    for (size_t i = 0; i < fechas.size(); ++i) {
        std::cout << i + 1 << ". " << fechas[i] << "\n";
    }

    std::cout << "\n1. Agregar fecha\n";
    std::cout << "2. Eliminar fecha\n";
    std::cout << "0. Volver al menu principal\n\n";

    float opcion;
    std::string entrada;
    do {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
        std::cout << "Seleccione una opcion: ";
        std::getline(std::cin, entrada);
        if (!validarEntradaNumerica(entrada, opcion, 0, 2)) {
            mostrarTextoAnimado("Opcion invalida. Intente de nuevo.", 12);
            continue;
        }
        break;
    } while (true);

    switch (static_cast<int>(opcion)) {
        case 1: {
            std::string nuevaFecha;
            do {
                std::cout << "\nIngrese la nueva fecha (DD/MM/AAAA): ";
                std::getline(std::cin, nuevaFecha);
                if (!validarFormatoFecha(nuevaFecha)) {
                    mostrarTextoAnimado("Formato de fecha invalido. Use DD/MM/AAAA.", 12);
                    continue;
                }
                break;
            } while (true);

            fechas.push_back(nuevaFecha);
            break;
        }
        case 2: {
            if (fechas.empty()) {
                mostrarTextoAnimado("No hay fechas para eliminar.", 12);
                esperar(1000);
                return;
            }

            float indice;
            do {
                std::cout << "\nIngrese el numero de la fecha a eliminar: ";
                std::getline(std::cin, entrada);
                if (!validarEntradaNumerica(entrada, indice, 1, fechas.size())) {
                    mostrarTextoAnimado("Seleccion invalida. Intente de nuevo.", 12);
                    continue;
                }
                break;
            } while (true);

            fechas.erase(fechas.begin() + static_cast<int>(indice) - 1);
            break;
        }
        case 0:
            return;
    }

    std::ofstream archivoFechasOut("fechas.txt");
    if (!archivoFechasOut.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de fechas para escritura.", 12);
        esperar(1000);
        return;
    }

    for (size_t i = 0; i < fechas.size(); ++i) {
        archivoFechasOut << fechas[i];
        if (i < fechas.size() - 1) {
            archivoFechasOut << ",";
        }
    }
    archivoFechasOut.close();

    mostrarTextoAnimado("Fechas actualizadas correctamente.", 10);
    esperar(1000);
}

void ordenarAlumnosYArchivos(const std::vector<std::string> &alumnosOrdenados) {
  // Leer todos los archivos
  std::vector<std::vector<std::string>> asistencias;
  std::vector<std::vector<std::string>> calificaciones;

  // Leer asistencias
  std::ifstream archivoAsistenciasIn("asistencias.txt");
  std::string lineaAsistencia;
  while (getline(archivoAsistenciasIn, lineaAsistencia)) {
    asistencias.push_back(split(lineaAsistencia, ','));
  }
  archivoAsistenciasIn.close();

  // Leer calificaciones
  std::ifstream archivoCalificacionesIn("calificaciones.txt");
  std::string lineaCalificacion;
  while (getline(archivoCalificacionesIn, lineaCalificacion)) {
    calificaciones.push_back(split(lineaCalificacion, ','));
  }
  archivoCalificacionesIn.close();

  // Crear mapas para mantener el orden original
  std::map<std::string, std::vector<std::string>> mapaAsistencias;
  std::map<std::string, std::vector<std::string>> mapaCalificaciones;

  // Leer alumnos actuales
  std::vector<std::string> alumnosActuales;
  std::ifstream archivoAlumnosIn("alumnos.txt");
  std::string linea;
  while (getline(archivoAlumnosIn, linea)) {
    alumnosActuales.push_back(linea);
  }
  archivoAlumnosIn.close();

  // Mapear datos actuales
  for (size_t i = 0; i < alumnosActuales.size(); ++i) {
    if (i < asistencias.size()) {
      mapaAsistencias[alumnosActuales[i]] = asistencias[i];
    }
    if (i < calificaciones.size()) {
      mapaCalificaciones[alumnosActuales[i]] = calificaciones[i];
    }
  }

  // Guardar alumnos ordenados
  std::ofstream archivoAlumnosOut("alumnos.txt");
  for (const auto &alumno : alumnosOrdenados) {
    archivoAlumnosOut << alumno << "\n";
  }
  archivoAlumnosOut.close();

  // Guardar asistencias ordenadas
  std::ofstream archivoAsistenciasOut("asistencias.txt");
  for (const auto &alumno : alumnosOrdenados) {
    if (mapaAsistencias.count(alumno) > 0) {
      const auto &asistencia = mapaAsistencias[alumno];
      for (size_t i = 0; i < asistencia.size(); ++i) {
        archivoAsistenciasOut << asistencia[i];
        if (i < asistencia.size() - 1)
          archivoAsistenciasOut << ",";
      }
      archivoAsistenciasOut << "\n";
    }
  }
  archivoAsistenciasOut.close();

  // Guardar calificaciones ordenadas
  std::ofstream archivoCalificacionesOut("calificaciones.txt");
  for (const auto &alumno : alumnosOrdenados) {
    if (mapaCalificaciones.count(alumno) > 0) {
      const auto &calificacion = mapaCalificaciones[alumno];
      for (size_t i = 0; i < calificacion.size(); ++i) {
        archivoCalificacionesOut << calificacion[i];
        if (i < calificacion.size() - 1)
          archivoCalificacionesOut << ",";
      }
      archivoCalificacionesOut << "\n";
    }
  }
  archivoCalificacionesOut.close();
}

void agregarAlumno() {
    system("cls");
    mostrarTextoAnimado("=== AGREGAR ALUMNO ===", 14);
    esperar(500);

    std::string nombre;
    do {
        std::cout << "\nIngrese el nombre completo del alumno: ";
        std::getline(std::cin, nombre);
        if (nombre.empty()) {
            mostrarTextoAnimado("El nombre no puede estar vacio.", 12);
            continue;
        }
        break;
    } while (true);

    std::ifstream archivoAlumnos("alumnos.txt");
    if (!archivoAlumnos.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
        esperar(1000);
        return;
    }

    std::vector<std::string> alumnos;
    std::string linea;
    while (getline(archivoAlumnos, linea)) {
        alumnos.push_back(linea);
    }
    archivoAlumnos.close();

    // Verificar si el alumno ya existe
    for (const auto& alumno : alumnos) {
        if (alumno == nombre) {
            mostrarTextoAnimado("El alumno ya existe en el sistema.", 12);
            esperar(1000);
            return;
        }
    }

    // Agregar el nuevo alumno
    alumnos.push_back(nombre);

    // Ordenar alumnos por apellido
    std::sort(alumnos.begin(), alumnos.end(), compararPorApellido);

    // Guardar la lista actualizada
    std::ofstream archivoAlumnosOut("alumnos.txt");
    if (!archivoAlumnosOut.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de alumnos para escritura.", 12);
        esperar(1000);
        return;
    }

    for (const auto& alumno : alumnos) {
        archivoAlumnosOut << alumno << "\n";
    }
    archivoAlumnosOut.close();

    // Actualizar archivo de asistencias
    std::ifstream archivoFechas("fechas.txt");
    if (!archivoFechas.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de fechas.", 12);
        esperar(1000);
        return;
    }

    std::string lineaFechas;
    std::vector<std::string> fechas;
    if (getline(archivoFechas, lineaFechas)) {
        fechas = split(lineaFechas, ',');
        if (!fechas.empty() && fechas.back().empty()) {
            fechas.pop_back();
        }
    }
    archivoFechas.close();

    std::ofstream archivoAsistencias("asistencias.txt", std::ios::app);
    if (!archivoAsistencias.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de asistencias.", 12);
        esperar(1000);
        return;
    }

    archivoAsistencias << nombre;
    for (size_t i = 0; i < fechas.size(); ++i) {
        archivoAsistencias << ",0";
    }
    archivoAsistencias << "\n";
    archivoAsistencias.close();

    mostrarTextoAnimado("Alumno agregado correctamente.", 10);
    esperar(1000);
}

void mostrarCuadriculaTrabajos() {
  std::ifstream archivoAlumnos("alumnos.txt");
  std::ifstream archivoTrabajos("trabajos.txt");
  std::ifstream archivoCalificaciones("calificaciones.txt");

  system("cls");
  mostrarTextoAnimado("=== CARGANDO LISTA DE TRABAJOS ===", 11);
  esperar(500);

  if (!archivoAlumnos.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
    esperar(1000);
    return;
  }
  if (!archivoTrabajos.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de trabajos.", 12);
    esperar(1000);
    return;
  }
  if (!archivoCalificaciones.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de calificaciones.", 12);
    esperar(1000);
    return;
  }

  std::vector<std::string> alumnos;
  std::string linea;
  while (getline(archivoAlumnos, linea)) {
    alumnos.push_back(linea);
  }
  archivoAlumnos.close();

  std::vector<std::pair<std::string, std::string>> trabajosConFecha;
  std::string lineaTrabajos;
  if (getline(archivoTrabajos, lineaTrabajos)) {
    std::vector<std::string> trabajos = split(lineaTrabajos, ',');
    if (!trabajos.empty() && trabajos.back().empty()) {
      trabajos.pop_back();
    }
    for (const auto &trabajo : trabajos) {
      size_t open_paren = trabajo.find('(');
      size_t close_paren = trabajo.find(')');
      if (open_paren != std::string::npos && close_paren != std::string::npos &&
          close_paren > open_paren) {
        std::string nombre = trabajo.substr(0, open_paren);
        nombre.erase(0, nombre.find_first_not_of(" \t\n\r\f\v"));
        nombre.erase(nombre.find_last_not_of(" \t\n\r\f\v") + 1);
        std::string fecha =
            trabajo.substr(open_paren + 1, close_paren - open_paren - 1);
        trabajosConFecha.push_back({nombre, fecha});
      } else {
        std::string nombre = trabajo;
        nombre.erase(0, nombre.find_first_not_of(" \t\n\r\f\v"));
        nombre.erase(nombre.find_last_not_of(" \t\n\r\f\v") + 1);
        trabajosConFecha.push_back({nombre, ""});
      }
    }
  }
  archivoTrabajos.close();

  std::vector<std::vector<std::string>> calificaciones;
  while (getline(archivoCalificaciones, linea)) {
    auto datos = split(linea, ',');
    calificaciones.push_back(datos);
  }
  archivoCalificaciones.close();

  // Calcular el ancho máximo del nombre
  size_t maxAnchoNombre = 0;
  for (const auto& alumno : alumnos) {
    if (alumno.length() > maxAnchoNombre) {
      maxAnchoNombre = alumno.length();
    }
  }
  const int ANCHO_NOMBRE = std::max(30, static_cast<int>(maxAnchoNombre) + 2);
  const int ANCHO_COLUMNA_DATOS = 15;

  // Calcular cuántas columnas caben en la pantalla
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  int anchoPantalla = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  int columnasVisibles = (anchoPantalla - ANCHO_NOMBRE - 2) / (ANCHO_COLUMNA_DATOS + 1);

  int inicioColumnas = 0;
  bool salir = false;

  while (!salir) {
    system("cls");
    mostrarTextoAnimado("=== LISTA DE TRABAJOS ===", 14);
    esperar(300);

    // Dibujar encabezado superior
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i) std::cout << "-";
    std::cout << "+";
    for (int i = 0; i < std::min(columnasVisibles, static_cast<int>(trabajosConFecha.size()) - inicioColumnas); ++i) {
      for (int j = 0; j < ANCHO_COLUMNA_DATOS; ++j) std::cout << "-";
      std::cout << "+";
    }
    std::cout << "\n";

    // Dibujar fila de nombres de trabajos
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "|";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
    std::cout << std::left << std::setw(ANCHO_NOMBRE) << "Nombre";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "|";
    for (int i = 0; i < std::min(columnasVisibles, static_cast<int>(trabajosConFecha.size()) - inicioColumnas); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      std::string nombre = trabajosConFecha[inicioColumnas + i].first;
      int padding = ANCHO_COLUMNA_DATOS - nombre.length();
      int pl = padding / 2, pr = padding - pl;
      for (int k = 0; k < pl; ++k) std::cout << " ";
      std::cout << nombre;
      for (int k = 0; k < pr; ++k) std::cout << " ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cout << "|";
    }
    std::cout << "\n";

    // Dibujar fila de fechas de trabajos
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "|";
    for (int i = 0; i < ANCHO_NOMBRE; ++i) std::cout << " ";
    std::cout << "|";
    for (int i = 0; i < std::min(columnasVisibles, static_cast<int>(trabajosConFecha.size()) - inicioColumnas); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      std::string fecha = trabajosConFecha[inicioColumnas + i].second;
      int padding = ANCHO_COLUMNA_DATOS - fecha.length();
      int pl = padding / 2, pr = padding - pl;
      for (int k = 0; k < pl; ++k) std::cout << " ";
      std::cout << fecha;
      for (int k = 0; k < pr; ++k) std::cout << " ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cout << "|";
    }
    std::cout << "\n";

    // Dibujar línea divisoria entre encabezado y datos
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i) std::cout << "-";
    std::cout << "+";
    for (int i = 0; i < std::min(columnasVisibles, static_cast<int>(trabajosConFecha.size()) - inicioColumnas); ++i) {
      for (int j = 0; j < ANCHO_COLUMNA_DATOS; ++j) std::cout << "-";
      std::cout << "+";
    }
    std::cout << "\n";

    // Dibujar filas de calificaciones
    for (size_t i = 0; i < alumnos.size(); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cout << "|";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
      std::cout << std::left << std::setw(ANCHO_NOMBRE) << alumnos[i];
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cout << "|";

      for (int j = 0; j < std::min(columnasVisibles, static_cast<int>(trabajosConFecha.size()) - inicioColumnas); ++j) {
        std::string valor = " ";
        int colorValor = 15;

        if (i < calificaciones.size() && inicioColumnas + j < calificaciones[i].size()) {
          valor = calificaciones[i][inicioColumnas + j];
          try {
            float calif = std::stof(valor);
            if (calif >= 6.0)
              colorValor = 10; // Verde
            else
              colorValor = 12; // Rojo
          } catch (...) {
            valor = " ";
            colorValor = 15; // Blanco por defecto
          }
        }

        int padding = ANCHO_COLUMNA_DATOS - valor.length();
        int pl = padding / 2, pr = padding - pl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        for (int k = 0; k < pl; ++k) std::cout << " ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorValor);
        std::cout << valor;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        for (int k = 0; k < pr; ++k) std::cout << " ";
        std::cout << "|";
      }

      std::cout << "\n";
    }

    // Dibujar línea inferior
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i) std::cout << "-";
    std::cout << "+";
    for (int i = 0; i < std::min(columnasVisibles, static_cast<int>(trabajosConFecha.size()) - inicioColumnas); ++i) {
      for (int j = 0; j < ANCHO_COLUMNA_DATOS; ++j) std::cout << "-";
      std::cout << "+";
    }
    std::cout << "\n";

    // Instrucciones de navegación
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
    std::cout << "\nFlechas <- ->: Navegar | ENTER: Regresar al Menu Principal\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    // Manejo de teclas
    int tecla = _getch();
    if (tecla == 224) { // Tecla de flecha
      tecla = _getch();
      if (tecla == 75 && inicioColumnas > 0) { // Flecha izquierda
        inicioColumnas--;
      } else if (tecla == 77 && inicioColumnas + columnasVisibles < trabajosConFecha.size()) { // Flecha derecha
        inicioColumnas++;
      }
    } else if (tecla == 13) { // ENTER
      salir = true;
    }
  }
}

void calcularPromedioParcial() {
  std::ifstream archivoAlumnos("alumnos.txt");
  std::ifstream archivoAsistencias("asistencias.txt");
  std::ifstream archivoCalificaciones("calificaciones.txt");
  std::ifstream archivoCriterios("criterios.txt");

  system("cls");
  mostrarTextoAnimado("=== CALCULO DE PROMEDIO PARCIAL ===", 11);
  esperar(500);

  if (!archivoAlumnos.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
    esperar(1000);
    return;
  }
  if (!archivoAsistencias.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de asistencias.", 12);
    esperar(1000);
    return;
  }
  if (!archivoCalificaciones.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de calificaciones.", 12);
    esperar(1000);
    return;
  }
  if (!archivoCriterios.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de criterios de evaluacion. "
                        "Asegurate de haberlos configurado.",
                        14); // Amarillo
    esperar(1500);
    return;
  }

  std::vector<std::string> alumnos;
  std::string linea;
  while (getline(archivoAlumnos, linea)) {
    alumnos.push_back(linea);
  }
  archivoAlumnos.close();

  std::vector<std::vector<std::string>> asistencias;
  while (getline(archivoAsistencias, linea)) {
    auto datos = split(linea, ',');
    asistencias.push_back(datos);
  }
  archivoAsistencias.close();

  std::vector<std::vector<std::string>> calificaciones;
  while (getline(archivoCalificaciones, linea)) {
    auto datos = split(linea, ',');
    calificaciones.push_back(datos);
  }
  archivoCalificaciones.close();

  // Leer criterios de evaluacion
  float pesoAsistencias = 0, pesoTrabajos = 0, pesoExamen = 0;
  std::string lineaCriterios;
  if (getline(archivoCriterios, lineaCriterios)) {
    auto criterios = split(lineaCriterios, ',');
    if (criterios.size() >= 3) {
      try {
        pesoAsistencias = std::stof(criterios[0]);
        pesoTrabajos = std::stof(criterios[1]);
        pesoExamen = std::stof(criterios[2]);
      } catch (...) {
        mostrarTextoAnimado(
            "Error al leer los pesos de los criterios de evaluacion.", 12);
        esperar(1000);
        return;
      }
    } else {
      mostrarTextoAnimado("Formato incorrecto en criterios.txt.", 12);
      esperar(1000);
      return;
    }
  }
  archivoCriterios.close();

  // Encontrar el indice del examen en trabajos.txt (asumimos que el ultimo
  // trabajo es el examen)
  int indiceExamen = -1;
  std::ifstream archivoTrabajosIn("trabajos.txt");
  std::string lineaTrabajos;
  if (getline(archivoTrabajosIn, lineaTrabajos)) {
    auto trabajos = split(lineaTrabajos, ',');
    if (!trabajos.empty()) {
      indiceExamen = trabajos.size() - 1; // El último elemento es el examen
    }
  }
  archivoTrabajosIn.close();

  if (indiceExamen == -1) {
    mostrarTextoAnimado("No se encontro ningun trabajo/examen. Asegurate de "
                        "haber agregado uno.",
                        14);
    esperar(1500);
    return;
  }

  std::cout << "\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
  std::cout << "Promedios Parciales (Escala 0-10):\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
  esperar(300);

  for (size_t i = 0; i < alumnos.size(); ++i) {
    float promedioParcial = 0;
    std::string nombreAlumno = alumnos[i];

    // Contribucion asistencias
    float totalFechas = 0;
    float asistenciasPresentes = 0;
    if (i < asistencias.size()) {
      totalFechas = asistencias[i].size();
      for (const auto &asis : asistencias[i]) {
        if (asis == "1") {
          asistenciasPresentes++;
        }
      }
    }
    float porcentajeAsistencia =
        (totalFechas > 0) ? (asistenciasPresentes / totalFechas) * 100.0 : 0;
    float contribucionAsistencia =
        (porcentajeAsistencia / 100.0) * (pesoAsistencias / 100.0);
    promedioParcial += contribucionAsistencia;

    // Contribucion trabajos (promedio de calificaciones de trabajos)
    float sumaCalificacionesTrabajos = 0;
    int numTrabajosCalificados = 0;
    float calificacionExamen = 0;

    if (i < calificaciones.size()) {
      for (size_t j = 0; j < calificaciones[i].size(); ++j) {
        if (static_cast<int>(j) == indiceExamen) {
          try {
            calificacionExamen = std::stof(calificaciones[i][j]);
          } catch (...) {
            calificacionExamen = 0;
          }
        } else if (static_cast<int>(j) <
                   indiceExamen) { // Considerar solo trabajos ANTES del examen
          try {
            sumaCalificacionesTrabajos += std::stof(calificaciones[i][j]);
            numTrabajosCalificados++;
          } catch (...) {
            // Ignorar calificaciones invalidas
          }
        }
      }
    }
    float promedioTrabajos =
        (numTrabajosCalificados > 0)
            ? sumaCalificacionesTrabajos / numTrabajosCalificados
            : 0;
    float contribucionTrabajos =
        (promedioTrabajos / 10.0) *
        (pesoTrabajos / 100.0); // Asumiendo trabajos calificados sobre 10
    promedioParcial += contribucionTrabajos;

    // Contribucion examen
    float contribucionExamen =
        (calificacionExamen / 10.0) *
        (pesoExamen / 100.0); // Asumiendo examen calificado sobre 10
    promedioParcial += contribucionExamen;

  
    promedioParcial /= 10.0;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
    std::cout << nombreAlumno << ": ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
    std::cout << std::fixed << std::setprecision(2) << promedioParcial << "\n";
    esperar(100);
  }

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3); // Cian
  std::cout << "\nPresiona ENTER para regresar al menu principal.";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
  std::cin.get(); // Esperar a que el usuario presione Enter
}

void calcularPromedioSemestre() {
  system("cls");
  mostrarTextoAnimado("=== CALCULO DE PROMEDIO SEMESTRAL ===", 11);
  esperar(500);

  
  mostrarTextoAnimado(
      "Esta función promediaría los promedios de los 3 parciales.", 14);
  mostrarTextoAnimado(
      "Como solo hay datos de un parcial, se mostrará ese promedio.", 14);
  esperar(2000);



  calcularPromedioParcial(); 
}

void modificarTrabajos() {
  system("cls");
  mostrarTextoAnimado("=== MODIFICACION DE TRABAJOS ===", 14);
  esperar(500);

  std::ifstream archivoAlumnos("alumnos.txt");
  std::ifstream archivoTrabajos("trabajos.txt");
  std::ifstream archivoCalificaciones("calificaciones.txt");

  if (!archivoAlumnos.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
    esperar(1000);
    return;
  }
  if (!archivoTrabajos.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de trabajos.", 12);
    esperar(1000);
    return;
  }
  if (!archivoCalificaciones.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de calificaciones.", 12);
    esperar(1000);
    return;
  }

  std::vector<std::string> alumnos;
  std::string linea;
  while (getline(archivoAlumnos, linea)) {
    alumnos.push_back(linea);
  }
  archivoAlumnos.close();

  std::vector<std::pair<std::string, std::string>> trabajosConFecha;
  std::string lineaTrabajos;
  if (getline(archivoTrabajos, lineaTrabajos)) {
    std::vector<std::string> trabajos = split(lineaTrabajos, ',');
    if (!trabajos.empty() && trabajos.back().empty()) {
      trabajos.pop_back();
    }
    for (const auto &trabajo : trabajos) {
      size_t open_paren = trabajo.find('(');
      size_t close_paren = trabajo.find(')');
      if (open_paren != std::string::npos && close_paren != std::string::npos &&
          close_paren > open_paren) {
        std::string nombre = trabajo.substr(0, open_paren);
        nombre.erase(0, nombre.find_first_not_of(" \t\n\r\f\v"));
        nombre.erase(nombre.find_last_not_of(" \t\n\r\f\v") + 1);
        std::string fecha =
            trabajo.substr(open_paren + 1, close_paren - open_paren - 1);
        trabajosConFecha.push_back({nombre, fecha});
      } else {
        std::string nombre = trabajo;
        nombre.erase(0, nombre.find_first_not_of(" \t\n\r\f\v"));
        nombre.erase(nombre.find_last_not_of(" \t\n\r\f\v") + 1);
        trabajosConFecha.push_back({nombre, ""});
      }
    }
  }
  archivoTrabajos.close();

  std::vector<std::vector<std::string>> calificaciones;
  while (getline(archivoCalificaciones, linea)) {
    auto datos = split(linea, ',');
    calificaciones.push_back(datos);
  }
  archivoCalificaciones.close();

  if (trabajosConFecha.empty()) {
    mostrarTextoAnimado("No hay trabajos para modificar.", 14);
    esperar(1000);
    return;
  }

  mostrarTextoAnimado("\nSelecciona el trabajo a modificar:", 11);
  for (size_t i = 0; i < trabajosConFecha.size(); ++i) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
    std::cout << i + 1 << ". ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << trabajosConFecha[i].first;
    if (!trabajosConFecha[i].second.empty()) {
      std::cout << " (" << trabajosConFecha[i].second << ")";
    }
    std::cout << "\n";
  }

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
  std::cout << "0. Regresar\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

  int opcionTrabajo;
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
  std::cout << "\nOpción: ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
  std::cin >> opcionTrabajo;
  opcionTrabajo--;

  if (opcionTrabajo == -1) { // Si el usuario ingreso 0
      mostrarTextoAnimado("Regresando...", 13);
      esperar(500);
      return; // Salir de la funcion
  }

  if (opcionTrabajo < 0 ||
      opcionTrabajo >= static_cast<int>(trabajosConFecha.size())) {
    mostrarTextoAnimado("Opción inválida.", 12);
    esperar(1000);
    return;
  }

  system("cls");
  mostrarTextoAnimado("=== MODIFICANDO TRABAJO ===", 14);
  mostrarTextoAnimado("Trabajo: " + trabajosConFecha[opcionTrabajo].first, 11);

  for (size_t i = 0; i < alumnos.size(); ++i) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
    std::cout << "\n" << alumnos[i] << "\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    std::cout << "Calificación (0-10): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    std::string valor;
    std::cin >> valor;
    float calificacion;
    try {
      calificacion = std::stof(valor);
      if (calificacion < 0)
        calificacion = 0;
      if (calificacion > 10)
        calificacion = 10;
    } catch (...) {
      calificacion = 0;
    }

    // Actualizar calificación
    if (i < calificaciones.size() &&
        opcionTrabajo < static_cast<int>(calificaciones[i].size())) {
      calificaciones[i][opcionTrabajo] = std::to_string(calificacion);
    }
  }

  // Guardar cambios
  mostrarTextoAnimado("\nGuardando cambios...", 11);
  mostrarBarraProgreso(1000);

  std::ofstream archivoCalificacionesOut("calificaciones.txt");
  if (!archivoCalificacionesOut.is_open()) {
    mostrarTextoAnimado("Error al guardar el archivo de calificaciones.", 12);
    esperar(1000);
  } else {
    for (const auto &fila : calificaciones) {
      for (size_t j = 0; j < fila.size(); ++j) {
        archivoCalificacionesOut << fila[j];
        if (j != fila.size() - 1)
          archivoCalificacionesOut << ",";
      }
      archivoCalificacionesOut << "\n";
    }
    archivoCalificacionesOut.close();
    mostrarTextoAnimado("\n¡Calificaciones actualizadas correctamente!", 10);
  }

  // Esperar a que el usuario presione Enter o 0 para regresar al menu de gestion de trabajos
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
  std::cout << "\nPresiona ENTER o 0 para regresar al menu de gestion de trabajos.";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer before get()
  std::cin.get();

  esperar(1000);
}

void agregarTrabajo() {
    system("cls");
    mostrarTextoAnimado("=== AGREGAR TRABAJO ===", 14);
    esperar(500);

    std::string nombreTrabajo;
    do {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
        std::cout << "\nIngrese el nombre del trabajo: ";
        std::getline(std::cin, nombreTrabajo);
        if (nombreTrabajo.empty()) {
            mostrarTextoAnimado("El nombre no puede estar vacio.", 12);
            continue;
        }
        break;
    } while (true);

    std::string fecha;
    do {
        std::cout << "\nIngrese la fecha del trabajo (DD/MM/AAAA) o deje en blanco: ";
        std::getline(std::cin, fecha);
        if (!fecha.empty() && !validarFormatoFecha(fecha)) {
            mostrarTextoAnimado("Formato de fecha invalido. Use DD/MM/AAAA.", 12);
            continue;
        }
        break;
    } while (true);

    std::ifstream archivoTrabajosIn("trabajos.txt");
    if (!archivoTrabajosIn.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de trabajos.", 12);
        esperar(1000);
        return;
    }

    std::vector<std::string> trabajos;
    std::string linea;
    if (getline(archivoTrabajosIn, linea)) {
        trabajos = split(linea, ',');
        if (!trabajos.empty() && trabajos.back().empty()) {
            trabajos.pop_back();
        }
    }
    archivoTrabajosIn.close();

    // Verificar si el trabajo ya existe
    std::string trabajoCompleto = fecha.empty() ? nombreTrabajo : nombreTrabajo + " (" + fecha + ")";
    for (const auto& trabajo : trabajos) {
        if (trabajo == trabajoCompleto) {
            mostrarTextoAnimado("El trabajo ya existe en el sistema.", 12);
            esperar(1000);
            return;
        }
    }

    // Agregar el nuevo trabajo
    trabajos.push_back(trabajoCompleto);

    // Guardar la lista actualizada
    std::ofstream archivoTrabajosOut("trabajos.txt");
    if (!archivoTrabajosOut.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de trabajos para escritura.", 12);
        esperar(1000);
        return;
    }

    for (size_t i = 0; i < trabajos.size(); ++i) {
        archivoTrabajosOut << trabajos[i];
        if (i < trabajos.size() - 1) {
            archivoTrabajosOut << ",";
        }
    }
    archivoTrabajosOut.close();

    // Actualizar archivo de calificaciones
    std::ifstream archivoAlumnos("alumnos.txt");
    if (!archivoAlumnos.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
        esperar(1000);
        return;
    }

    std::vector<std::string> alumnos;
    while (getline(archivoAlumnos, linea)) {
        alumnos.push_back(linea);
    }
    archivoAlumnos.close();

    std::ofstream archivoCalificaciones("calificaciones.txt", std::ios::app);
    if (!archivoCalificaciones.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de calificaciones.", 12);
        esperar(1000);
        return;
    }

    for (const auto& alumno : alumnos) {
        archivoCalificaciones << alumno << "," << trabajoCompleto << ",0\n";
    }
    archivoCalificaciones.close();

    mostrarTextoAnimado("Trabajo agregado correctamente.", 10);
    esperar(1000);
}

void gestionarTrabajos() {
  system("cls");
  mostrarTextoAnimado("=== GESTION DE TRABAJOS ===", 10);
  esperar(500);

  while (true) {
    system("cls");
    mostrarTextoAnimado("=== GESTION DE TRABAJOS ===", 10);
    mostrarTextoAnimado("\n1. Agregar trabajo", 11);
    mostrarTextoAnimado("2. Eliminar trabajo", 12);
    mostrarTextoAnimado("0. Regresar", 14);

    int opcion;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    std::cout << "\nOpción: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cin >> opcion;

    switch (opcion) {
    case 1:
      agregarTrabajo();
      break;
    case 2:
      modificarTrabajos();
      break;
    case 0:
      return;
    default:
      mostrarTextoAnimado("Opción inválida.", 12);
      esperar(1000);
    }
  }
}

void gestionarCriteriosEvaluacion() {
  system("cls");
  mostrarTextoAnimado("=== GESTION DE CRITERIOS DE EVALUACION ===", 10);
  esperar(500);

  while (true) {
    system("cls");
    mostrarTextoAnimado("=== GESTION DE CRITERIOS DE EVALUACION ===", 10);
    esperar(300);

    bool criteriosExisten = existenCriteriosEvaluacion();

    if (criteriosExisten) {
      mostrarTablaCriterios();
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
      std::cout << "Selecciona una opcion:\n";
      std::cout << "1. Modificar criterios\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
      std::cout << "0. Regresar\n";
    } else {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
      std::cout << "Selecciona una opcion:\n";
      std::cout << "1. Agregar criterios\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
      std::cout << "0. Regresar\n";
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    int opcion;
    std::cout << "\nOpcion: ";
    std::cin >> opcion;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (criteriosExisten) { // Si existen, opcion 1 es modificar, opcion 0 es regresar
      switch (opcion) {
      case 1:
        modificarCriteriosEvaluacion();
        break;
      case 0:
        mostrarTextoAnimado("Regresando...", 13);
        esperar(500);
        return; // Salir de la funcion
      default:
        mostrarTextoAnimado("Opcion invalida.", 12);
        esperar(1000);
        break;
      }
    } else { // Si no existen, opcion 1 es agregar, opcion 0 es regresar
      switch (opcion) {
      case 1:
        agregarCriteriosEvaluacion();
        break;
      case 0:
        mostrarTextoAnimado("Regresando...", 13);
        esperar(500);
        return; // Salir de la funcion
      default:
        mostrarTextoAnimado("Opcion invalida.", 12);
        esperar(1000);
        break;
      }
    }
  }
}

void modificarCalificacionAlumno() {
    if (!existenCriteriosEvaluacion()) {
        mostrarTextoAnimado("No hay criterios de evaluacion definidos.", 12);
        esperar(1000);
        return;
    }

    std::ifstream archivoAlumnos("alumnos.txt");
    if (!archivoAlumnos.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
        esperar(1000);
        return;
    }

    std::vector<std::string> alumnos;
    std::string linea;
    while (getline(archivoAlumnos, linea)) {
        alumnos.push_back(linea);
    }
    archivoAlumnos.close();

    if (alumnos.empty()) {
        mostrarTextoAnimado("No hay alumnos registrados.", 12);
        esperar(1000);
        return;
    }

    system("cls");
    mostrarTextoAnimado("=== MODIFICAR CALIFICACION ===", 14);
    std::cout << "\nSeleccione el alumno:\n\n";

    for (size_t i = 0; i < alumnos.size(); ++i) {
        std::cout << i + 1 << ". " << alumnos[i] << "\n";
    }

    float seleccion;
    std::string entrada;
    do {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
        std::cout << "\nIngrese el numero del alumno (o 0 para regresar): ";
        std::getline(std::cin, entrada);
        if (!validarEntradaNumerica(entrada, seleccion, 0, alumnos.size())) {
            mostrarTextoAnimado("Seleccion invalida. Intente de nuevo.", 12);
            continue;
        }
        break;
    } while (true);

    if (seleccion == 0) {
        mostrarTextoAnimado("Regresando al menu principal...", 13);
        esperar(500);
        return;
    }

    std::string alumnoSeleccionado = alumnos[static_cast<int>(seleccion) - 1];
    mostrarTablaCriterios();

    std::ifstream archivoCriterios("criterios.txt");
    if (!archivoCriterios.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de criterios.", 12);
        esperar(1000);
        return;
    }

    std::vector<std::string> criterios;
    while (getline(archivoCriterios, linea)) {
        criterios.push_back(linea);
    }
    archivoCriterios.close();

    std::cout << "\nSeleccione el criterio a modificar:\n\n";
    for (size_t i = 0; i < criterios.size(); ++i) {
        std::cout << i + 1 << ". " << criterios[i] << "\n";
    }

    float criterioSeleccionado;
    do {
        std::cout << "\nIngrese el numero del criterio (o 0 para regresar): ";
        std::getline(std::cin, entrada);
        if (!validarEntradaNumerica(entrada, criterioSeleccionado, 0, criterios.size())) {
            mostrarTextoAnimado("Seleccion invalida. Intente de nuevo.", 12);
            continue;
        }
        break;
    } while (true);

    if (criterioSeleccionado == 0) {
        mostrarTextoAnimado("Regresando al menu principal...", 13);
        esperar(500);
        return;
    }

    float nuevaCalificacion;
    do {
        std::cout << "\nIngrese la nueva calificacion (0-10): ";
        std::getline(std::cin, entrada);
        if (!validarEntradaNumerica(entrada, nuevaCalificacion, 0, 10)) {
            mostrarTextoAnimado("Calificacion invalida. Debe ser un numero entre 0 y 10.", 12);
            continue;
        }
        break;
    } while (true);

    std::ifstream archivoCalificaciones("calificaciones.txt");
    if (!archivoCalificaciones.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de calificaciones.", 12);
        esperar(1000);
        return;
    }

    std::vector<std::string> calificaciones;
    while (getline(archivoCalificaciones, linea)) {
        calificaciones.push_back(linea);
    }
    archivoCalificaciones.close();

    bool encontrado = false;
    for (size_t i = 0; i < calificaciones.size(); ++i) {
        auto datos = split(calificaciones[i], ',');
        if (datos.size() >= 3 && datos[0] == alumnoSeleccionado) {
            std::stringstream ss;
            ss << alumnoSeleccionado << "," << criterios[static_cast<int>(criterioSeleccionado) - 1] << "," << nuevaCalificacion;
            calificaciones[i] = ss.str();
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        std::stringstream ss;
        ss << alumnoSeleccionado << "," << criterios[static_cast<int>(criterioSeleccionado) - 1] << "," << nuevaCalificacion;
        calificaciones.push_back(ss.str());
    }

    std::ofstream archivoCalificacionesOut("calificaciones.txt");
    if (!archivoCalificacionesOut.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de calificaciones para escritura.", 12);
        esperar(1000);
        return;
    }

    for (const auto& cal : calificaciones) {
        archivoCalificacionesOut << cal << "\n";
    }
    archivoCalificacionesOut.close();

    mostrarTextoAnimado("Calificacion actualizada correctamente.", 10);
    esperar(1000);
}

void mostrarTablaCriterios() {
  if (!existenCriteriosEvaluacion()) {
    return;
  }

  std::ifstream archivoIn("criterios.txt");
  std::string linea;
  std::getline(archivoIn, linea);
  archivoIn.close();

  auto valores = split(linea, ',');
  float valorAsistencias = std::stof(valores[0]);
  float valorTrabajos = std::stof(valores[1]);
  float valorExamen = std::stof(valores[2]);

  // Dibujar tabla
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
  std::cout << "\n┌────────────────────┬──────────────┐\n";
  std::cout << "│ Criterio          │ Porcentaje   │\n";
  std::cout << "├────────────────────┼──────────────┤\n";

  // Asistencias
  std::cout << "│ Asistencias       │ ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
  std::cout << std::right << std::setw(10) << valorAsistencias << "% ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
  std::cout << "│\n";

  // Trabajos
  std::cout << "│ Trabajos          │ ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
  std::cout << std::right << std::setw(10) << valorTrabajos << "% ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
  std::cout << "│\n";

  // Examen
  std::cout << "│ Examen            │ ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
  std::cout << std::right << std::setw(10) << valorExamen << "% ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
  std::cout << "│\n";

  std::cout << "└────────────────────┴──────────────┘\n\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void gestionarCriterios() {
  int opcion;
  do {
    system("cls");
    mostrarTextoAnimado("=== GESTION DE CRITERIOS DE EVALUACION ===", 10);
    esperar(500);

    // Mostrar tabla de criterios si existen
    mostrarTablaCriterios();

    if (!existenCriteriosEvaluacion()) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
      std::cout << "Selecciona una opcion:\n";
      std::cout << "1. Agregar Criterios de Evaluacion\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
      std::cout << "0. Regresar al Menu Principal\n";
    } else {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
      std::cout << "Selecciona una opcion:\n";
      std::cout << "1. Modificar Criterios de Evaluacion\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
      std::cout << "0. Regresar al Menu Principal\n";
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    std::cout << "\nSelecciona una opcion: ";
    std::cin >> opcion;
    std::cin.ignore();

    switch (opcion) {
    case 1:
      if (!existenCriteriosEvaluacion()) {
        agregarCriteriosEvaluacion();
      } else {
        modificarCriteriosEvaluacion();
      }
      break;
    case 0:
      mostrarTextoAnimado("Regresando al Menu Principal...", 13);
      esperar(500);
      break;
    default:
      mostrarTextoAnimado("Opcion invalida.", 12);
      esperar(1000);
      break;
    }
  } while (opcion != 0);
}

void agregarCriteriosEvaluacion() {
  system("cls");
  mostrarTextoAnimado("=== AGREGAR CRITERIOS DE EVALUACION ===", 10);
  esperar(500);

  if (existenCriteriosEvaluacion()) {
    mostrarTextoAnimado("Ya existen criterios de evaluación.", 12);
    mostrarTextoAnimado("Use la opción de modificar para cambiar los valores.",
                        12);
    esperar(2000);
    return;
  }

  float valorAsistencias, valorTrabajos, valorExamen;
  bool valoresValidos = false;

  do {
    system("cls");
    mostrarTextoAnimado("=== AGREGAR CRITERIOS DE EVALUACION ===", 10);
    mostrarTextoAnimado("\nIngrese los porcentajes (deben sumar 100%):", 11);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    std::cout << "\nPorcentaje para Asistencias (0-100): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cin >> valorAsistencias;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    std::cout << "Porcentaje para Trabajos (0-100): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cin >> valorTrabajos;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    std::cout << "Porcentaje para Examen (0-100): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cin >> valorExamen;

    float suma = valorAsistencias + valorTrabajos + valorExamen;
    if (std::abs(suma - 100.0f) < 0.01f) {
      valoresValidos = true;
    } else {
      mostrarTextoAnimado("\nLos porcentajes deben sumar 100%.", 12);
      mostrarTextoAnimado("Suma actual: " + std::to_string(suma) + "%", 12);
      esperar(2000);
    }
  } while (!valoresValidos);

  std::ofstream archivoCriterios("criterios.txt");
  if (!archivoCriterios.is_open()) {
    mostrarTextoAnimado("Error al guardar los criterios.", 12);
    esperar(1000);
    return;
  }

  archivoCriterios << valorAsistencias << "," << valorTrabajos << ","
                   << valorExamen;
  archivoCriterios.close();

  mostrarTextoAnimado("\n¡Criterios guardados correctamente!", 10);
  esperar(1000);
}

void modificarCriteriosEvaluacion() {
  system("cls");
  mostrarTextoAnimado("=== MODIFICAR CRITERIOS DE EVALUACION ===", 10);
  esperar(500);

  if (!existenCriteriosEvaluacion()) {
    mostrarTextoAnimado("No hay criterios de evaluacion para modificar.", 12);
    mostrarTextoAnimado("Use la opcion de agregar para crear nuevos criterios.",
                        12);
    esperar(2000);
    return;
  }

  std::ifstream archivoCriterios("criterios.txt");
  if (!archivoCriterios.is_open()) {
    mostrarTextoAnimado("Error al leer los criterios actuales.", 12);
    esperar(1000);
    return;
  }

  std::string linea;
  getline(archivoCriterios, linea);
  archivoCriterios.close();

  auto valores = split(linea, ',');
  float valorAsistencias = std::stof(valores[0]);
  float valorTrabajos = std::stof(valores[1]);
  float valorExamen = std::stof(valores[2]);

  bool valoresValidos = false;
  do {
    system("cls");
    mostrarTextoAnimado("=== MODIFICAR CRITERIOS DE EVALUACION ===", 10);
    mostrarTextoAnimado("\nValores actuales:", 11);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    std::cout << "Asistencias: " << std::fixed << std::setprecision(1) << valorAsistencias << "%\n";
    std::cout << "Trabajos: " << std::fixed << std::setprecision(1) << valorTrabajos << "%\n";
    std::cout << "Examen: " << std::fixed << std::setprecision(1) << valorExamen << "%\n";

    mostrarTextoAnimado("\nIngrese los nuevos porcentajes (deben sumar 100%):",
                        11);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    std::cout << "\nPorcentaje para Asistencias (0-100): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cin >> valorAsistencias;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    std::cout << "Porcentaje para Trabajos (0-100): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cin >> valorTrabajos;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    std::cout << "Porcentaje para Examen (0-100): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cin >> valorExamen;

    float suma = valorAsistencias + valorTrabajos + valorExamen;
    if (std::abs(suma - 100.0f) < 0.01f) { // La validacion sigue siendo que sumen 100%
      valoresValidos = true;
    } else {
      mostrarTextoAnimado("\nLos porcentajes deben sumar 100%.", 12);
      mostrarTextoAnimado("Suma actual: " + std::to_string(suma) + "%", 12); // Mantengo el string para mostrar la suma
      esperar(2000);
    }
  } while (!valoresValidos);

  std::ofstream archivoCriteriosOut("criterios.txt");
  if (!archivoCriteriosOut.is_open()) {
    mostrarTextoAnimado("Error al guardar los criterios.", 12);
    esperar(1000);
    return;
  }

  archivoCriteriosOut << valorAsistencias << "," << valorTrabajos << ","
                      << valorExamen;
  archivoCriteriosOut.close();

  mostrarTextoAnimado("\n¡Criterios actualizados correctamente!", 10);
  esperar(1000);
}

bool existenCriteriosEvaluacion() {
  std::ifstream archivoCriterios("criterios.txt");
  if (!archivoCriterios.is_open()) {
    mostrarTextoAnimado("Error al leer los criterios.", 12);
    esperar(1000);
    return false;
  }

  std::string linea;
  getline(archivoCriterios, linea);
  archivoCriterios.close();

  auto valores = split(linea, ',');
  if (valores.size() == 3) {
    try {
      std::stof(valores[0]);
      std::stof(valores[1]);
      std::stof(valores[2]);
      return true;
    } catch (...) {
      mostrarTextoAnimado("Error al leer los criterios.", 12);
      esperar(1000);
      return false;
    }
  } else {
    mostrarTextoAnimado("Formato incorrecto en criterios.txt.", 12);
    esperar(1000);
    return false;
  }
}

void mostrar() {
  std::ifstream archivoCriterios("criterios.txt");
  if (!archivoCriterios.is_open()) {
    mostrarTextoAnimado("Error al leer los criterios.", 12);
    esperar(1000);
    return;
  }

  std::string linea;
  getline(archivoCriterios, linea);
  archivoCriterios.close();

  auto valores = split(linea, ',');
  if (valores.size() == 3) {
    try {
      float valorAsistencias = std::stof(valores[0]);
      float valorTrabajos = std::stof(valores[1]);
      float valorExamen = std::stof(valores[2]);

      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
      std::cout << "Asistencias: " << valorAsistencias << "%\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
      std::cout << "Trabajos: " << valorTrabajos << "%\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
      std::cout << "Examen: " << valorExamen << "%\n";
    } catch (...) {
      mostrarTextoAnimado("Error al mostrar los criterios.", 12);
      esperar(1000);
    }
  } else {
    mostrarTextoAnimado("Formato incorrecto en criterios.txt.", 12);
    esperar(1000);
  }
}

// Funciones de utilidad para manejo de archivos
bool archivoExiste(const std::string& nombreArchivo) {
    std::ifstream archivo(nombreArchivo);
    return archivo.good();
}

bool eliminarArchivo(const std::string& nombreArchivo) {
    if (archivoExiste(nombreArchivo)) {
        return remove(nombreArchivo.c_str()) == 0;
    }
    return true;
}

bool renombrarArchivo(const std::string& viejoNombre, const std::string& nuevoNombre) {
    if (!archivoExiste(viejoNombre)) {
        return false;
    }
    if (archivoExiste(nuevoNombre)) {
        if (!eliminarArchivo(nuevoNombre)) {
            return false;
        }
    }
    return rename(viejoNombre.c_str(), nuevoNombre.c_str()) == 0;
}

// Función para validar formato de fecha
bool validarFormatoFecha(const std::string& fecha) {
    if (fecha.empty()) return true; // Permitir fechas vacías
    
    // Formato esperado: DD/MM/AAAA
    if (fecha.length() != 10) return false;
    if (fecha[2] != '/' || fecha[5] != '/') return false;
    
    try {
        int dia = std::stoi(fecha.substr(0, 2));
        int mes = std::stoi(fecha.substr(3, 2));
        int anio = std::stoi(fecha.substr(6, 4));
        
        if (dia < 1 || dia > 31) return false;
        if (mes < 1 || mes > 12) return false;
        if (anio < 2000 || anio > 2100) return false;
        
        // Validaciones adicionales según el mes
        if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && dia > 30) return false;
        if (mes == 2) {
            bool esBisiesto = (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
            if ((esBisiesto && dia > 29) || (!esBisiesto && dia > 28)) return false;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

// Función para validar entrada numérica
bool validarEntradaNumerica(const std::string& entrada, float& valor, float min, float max) {
    try {
        valor = std::stof(entrada);
        return valor >= min && valor <= max;
    } catch (...) {
        return false;
    }
}

void gen() {
    std::ifstream archivo("alumnos.txt");
    if (!archivo.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de alumnos", 12);
        return;
    }

    std::ofstream temp("temp.txt");
    if (!temp.is_open()) {
        mostrarTextoAnimado("Error al crear archivo temporal", 12);
        archivo.close();
        return;
    }

    std::string linea;
    bool error = false;
    while (std::getline(archivo, linea)) {
        if (!linea.empty()) {
            temp << linea << std::endl;
        }
    }

    archivo.close();
    temp.close();

    if (error) {
        mostrarTextoAnimado("Error al procesar el archivo", 12);
        eliminarArchivo("temp.txt");
        return;
    }

    if (!renombrarArchivo("temp.txt", "alumnos.txt")) {
        mostrarTextoAnimado("Error al actualizar el archivo", 12);
        eliminarArchivo("temp.txt");
        return;
    }

    mostrarTextoAnimado("Archivo actualizado correctamente", 10);
}

// Función para animar texto
void animarTexto(const std::string& texto, int velocidad) {
    for (char c : texto) {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(velocidad));
    }
    std::cout << std::endl;
}

int main() {
  setUTF8();

  int opcion;
  do {
    mostrarMenu();
    std::cout << "\nSelecciona una opcion: ";
    std::cin >> opcion;

    switch (opcion) {
    case 1:
      mostrarCuadriculaAsistencias();
      break;
    case 2:
      mostrarCuadriculaTrabajos();
      break;
    case 3:
      modificarAsistencias(); // Corregido de 'mos()' a 'modificarAsistencias()'
      break;
    case 4:
      modificarTrabajos();
      break;
    case 5:
      agregarAlumno();
      break;
    case 6:
      agregarTrabajo();
      break;
    case 7:
      gestionarFechas();
      break;
    case 8:
      gestionarTrabajos();
      break;
    case 9:
      gestionarCriteriosEvaluacion();
      break;
    case 10:
      modificarCalificacionAlumno();
      break;
    case 11:
      modificarAsistencias(); // Corresponde a Modificar Asistencia de Alumno
      break;
    case 12:
      calcularPromedioParcial();
      break;
    case 13:
      calcularPromedioSemestre();
      break;
    case 0:
      mostrarTextoAnimado("Saliendo del sistema...", 13);
      esperar(1000);
      break;
    default:
      mostrarTextoAnimado("Opcion invalida", 12);
      esperar(1000);
      break;
    }
  } while (opcion != 0);

  return 0;
  
}

