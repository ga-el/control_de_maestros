#include <conio.h>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits> // Necesario para std::numeric_limits
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#ifndef NOMINMAX // Previene que windows.h defina min/max como macros si ya está
                 // definido
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
void ordenarAlumnosYArchivos(const std::vector<std::string>& alumnosOrdenados);
void gestionarCriteriosEvaluacion();
void agregarCriteriosEvaluacion();
void modificarCriteriosEvaluacion();
bool existenCriteriosEvaluacion();
void mostrarTablaCriterios();

// Agregar después de las declaraciones de funciones existentes
std::string obtenerApellido(const std::string& nombreCompleto) {
    std::stringstream ss(nombreCompleto);
    std::string apellido;
    ss >> apellido; // Obtiene la primera palabra (apellido)
    return apellido;
}

bool compararPorApellido(const std::string& a, const std::string& b) {
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
  std::cout << "===== SISTEMA DE GESTION ESCOLAR =====\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
  std::cout << "1. Lista de asistencias\n";
  std::cout << "2. Modificar asistencias\n";
  std::cout << "3. Lista de trabajos\n";
  std::cout << "4. Modificar calificaciones\n";
  // Opción 5 de agregar trabajo con fecha eliminada

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
  std::cout << "5. Agregar alumnos\n";  // Opción 6 anterior
  std::cout << "6. Agregar trabajos\n"; // Opción 7 anterior
  std::cout << "7. Gestionar Fechas\n"; // Opción 8 anterior

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta
  std::cout << "8. Gestionar Trabajos\n";             // Opción 9 anterior
  std::cout << "9. Gestionar Criterios de Evaluacion\n"; // Reincorporada
  std::cout << "0. Salir\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          15); // Volver a blanco
}

void mostrarCuadriculaAsistencias() {
  std::ifstream archivoAlumnos("alumnos.txt");
  std::ifstream archivoFechas("fechas.txt");
  std::ifstream archivoAsistencias("asistencias.txt");

  system("cls");
  mostrarTextoAnimado("=== CARGANDO LISTA DE ASISTENCIAS ===", 11); // Celeste
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

  const int COLUMNAS_VISIBLES = 5;   // Mostrar hasta 5 fechas a la vez
  const int ANCHO_NOMBRE = 20;       // Ancho fijo para nombres
  const int ANCHO_CALIFICACION = 18; // Mismo ancho que en trabajos

  int desplazamiento = 0;
  bool primeraVez = true;

  do {
    if (primeraVez) {
      system("cls");
      mostrarTextoAnimado("=== LISTA DE ASISTENCIAS ===", 14); // Amarillo
      esperar(300);
    } else {
      system("cls"); // Limpiar rápido
    }

    // Dibujar encabezado de la tabla
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      std::cout << "-";
    std::cout << "+";

    for (int i = desplazamiento; i < desplazamiento + COLUMNAS_VISIBLES &&
                                 i < static_cast<int>(fechas.size());
         ++i) {
      for (int j = 0; j < ANCHO_CALIFICACION; ++j)
        std::cout << "-";
      std::cout << "+";
    }
    std::cout << "\n";

    // Fila 1: "Nombre"
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "|";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
    std::cout << std::left << std::setw(ANCHO_NOMBRE) << "Nombre";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "|";

    for (int i = desplazamiento; i < desplazamiento + COLUMNAS_VISIBLES &&
                                 i < static_cast<int>(fechas.size());
         ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      std::string nombre = fechas[i];
      int padding = ANCHO_CALIFICACION - nombre.length();
      int pl = padding / 2, pr = padding - pl;
      for (int k = 0; k < pl; ++k)
        std::cout << " ";
      std::cout << nombre;
      for (int k = 0; k < pr; ++k)
        std::cout << " ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cout << "|";
    }
    std::cout << "\n";

    // Fila 2: espacio vacío debajo de "Nombre"
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "|";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      std::cout << " ";
    std::cout << "|";

    for (int i = desplazamiento; i < desplazamiento + COLUMNAS_VISIBLES &&
                                 i < static_cast<int>(fechas.size());
         ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      std::string vacio = ""; // No hay fecha extra aquí
      int padding = ANCHO_CALIFICACION - vacio.length();
      int pl = padding / 2, pr = padding - pl;
      for (int k = 0; k < pl; ++k)
        std::cout << " ";
      std::cout << vacio;
      for (int k = 0; k < pr; ++k)
        std::cout << " ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cout << "|";
    }
    std::cout << "\n";

    // Línea divisoria
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      std::cout << "-";
    std::cout << "+";

    for (int i = desplazamiento; i < desplazamiento + COLUMNAS_VISIBLES &&
                                 i < static_cast<int>(fechas.size());
         ++i) {
      for (int j = 0; j < ANCHO_CALIFICACION; ++j)
        std::cout << "-";
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

      for (int j = desplazamiento; j < desplazamiento + COLUMNAS_VISIBLES &&
                                   j < static_cast<int>(fechas.size());
           ++j) {

        std::string valor = " ";
        int colorValor = 15;

        if (i < asistencias.size() &&
            j < static_cast<int>(asistencias[i].size())) {
          valor = (asistencias[i][j] == "1") ? "Sí" : "No";
          colorValor = (asistencias[i][j] == "1") ? 10 : 12;
        }

        int padding = ANCHO_CALIFICACION - valor.length();
        int pl = padding / 2, pr = padding - pl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        for (int k = 0; k < pl; ++k)
          std::cout << " ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorValor);
        std::cout << valor;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        for (int k = 0; k < pr; ++k)
          std::cout << " ";
        std::cout << "|";
      }

      if (primeraVez)
        esperar(50); // Animación solo en carga inicial
      std::cout << "\n";
    }

    // Línea inferior
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      std::cout << "-";
    std::cout << "+";

    for (int i = desplazamiento; i < desplazamiento + COLUMNAS_VISIBLES &&
                                 i < static_cast<int>(fechas.size());
         ++i) {
      for (int j = 0; j < ANCHO_CALIFICACION; ++j)
        std::cout << "-";
      std::cout << "+";
    }
    std::cout << "\n";

    // Instrucciones
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
    std::cout << "Usa <- -> para navegar. Presiona ENTER para regresar.\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    char key = _getch();
    switch (key) {
    case 75: // Flecha izquierda
      if (desplazamiento > 0) {
        desplazamiento--;
        primeraVez = false;
      }
      break;
    case 77: // Flecha derecha
      if (desplazamiento + COLUMNAS_VISIBLES <
          static_cast<int>(fechas.size())) {
        desplazamiento++;
        primeraVez = false;
      }
      break;
    case 13: // Enter
      return;
    }

  } while (true);
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
  while (true) { // Bucle infinito hasta que se elija salir
    system("cls");
    mostrarTextoAnimado("=== MODIFICACION DE ASISTENCIAS ===", 14); // Amarillo
    esperar(500);

    std::ifstream archivoAlumnos("alumnos.txt");
    std::ifstream archivoFechas("fechas.txt");
    std::ifstream archivoAsistencias("asistencias.txt");

    if (!archivoAlumnos.is_open()) {
      mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12); // Rojo
      esperar(1000);
      return;
    }
    if (!archivoFechas.is_open()) {
      mostrarTextoAnimado("Error al abrir el archivo de fechas.", 12); // Rojo
      esperar(1000);
      return;
    }
    if (!archivoAsistencias.is_open()) {
      mostrarTextoAnimado("Error al abrir el archivo de asistencias.",
                          12); // Rojo
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

    // Si no hay fechas o alumnos, no podemos hacer nada
    if (fechas.empty() || alumnos.empty()) {
      if (fechas.empty())
        mostrarTextoAnimado("No hay fechas disponibles para modificar.",
                            14); // Amarillo
      if (alumnos.empty())
        mostrarTextoAnimado("No hay alumnos disponibles para modificar.",
                            14); // Amarillo
      esperar(1500);
      return; // Regresar directamente al menú principal
    }

    mostrarTextoAnimado("\nSelecciona la fecha a modificar:", 11); // Celeste
    std::cout << "\n";
    for (size_t i = 0; i < fechas.size(); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
      std::cout << i + 1 << ". ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
      std::cout << fechas[i] << "\n";
      esperar(100);
    }

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta para regresar
    std::cout << "0. Regresar al Menu Principal\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco

    int opcionModificar;
    std::cout << "\nOpcion: ";
    std::cin >> opcionModificar;

    // Limpiar buffer después de leer número
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Manejar opción 0 primero
    if (opcionModificar == 0) {
      mostrarTextoAnimado("Regresando al Menu Principal...", 13); // Magenta
      esperar(500);
      return; // Salimos inmediatamente y volvemos al menú principal
    }

    // Ajustar índice
    int indiceFecha = opcionModificar - 1;

    // Validar índice
    if (indiceFecha < 0 || indiceFecha >= static_cast<int>(fechas.size())) {
      mostrarTextoAnimado("Opcion invalida.", 12); // Rojo
      esperar(1000);
      continue; // Reiniciar bucle
    }

    // Mostrar formulario de edición de asistencias
    system("cls");
    mostrarTextoAnimado("=== REGISTRO DE ASISTENCIAS ===", 14); // Amarillo
    mostrarTextoAnimado("Fecha: " + fechas[indiceFecha], 11);   // Celeste

    for (size_t i = 0; i < alumnos.size(); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
      std::cout << "\n" << alumnos[i] << "\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
      std::cout << "Asistencia (1 = Sí, 0 = No): ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco

      std::string valor;
      std::cin >> valor;
      if (valor != "1" && valor != "0")
        valor = "0";

      // Actualizar asistencia
      if (i < asistencias.size() &&
          indiceFecha < static_cast<int>(asistencias[i].size())) {
        asistencias[i][indiceFecha] = valor;
      } else {
        mostrarTextoAnimado("Error: Datos incompletos para este alumno.",
                            12); // Rojo
      }
    }

    // Guardar cambios
    mostrarTextoAnimado("\nGuardando cambios...", 11); // Celeste
    mostrarBarraProgreso(1000);

    std::ofstream archivoOut("asistencias.txt");
    if (!archivoOut.is_open()) {
      mostrarTextoAnimado("Error al guardar el archivo de asistencias.",
                          12); // Rojo
      esperar(1000);
    } else {
      for (const auto &fila : asistencias) {
        for (size_t j = 0; j < fila.size(); ++j) {
          archivoOut << fila[j];
          if (j != fila.size() - 1)
            archivoOut << ",";
        }
        archivoOut << "\n";
      }
      archivoOut.close();
      mostrarTextoAnimado("\n¡Asistencias actualizadas correctamente!",
                          10); // Verde
    }

    esperar(1000);
  } // Fin del bucle infinito
}

void gestionarFechas() {
  int opcionGestionFechas;
  do {
    system("cls");
    mostrarTextoAnimado("=== GESTION DE FECHAS ===",
                        10); // Verde para el título principal

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            14); // Amarillo para las opciones
    std::cout << "\nSelecciona una opcion:\n";
    std::cout << "1. Agregar Fecha\n";
    std::cout << "2. Eliminar Fecha\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta para regresar
    std::cout << "0. Regresar al Menu Principal\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco

    std::cout << "\nSelecciona una opcion: ";
    std::cin >> opcionGestionFechas;
    std::cin.ignore();

    switch (opcionGestionFechas) {
    case 1: { // Agregar Fecha
      system("cls");
      mostrarTextoAnimado("=== AGREGAR FECHA ===",
                          10); // Verde para el título de esta sección
      esperar(500);            // Pequeña pausa

      std::string nuevaFecha;
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Celeste para la instrucción
      std::cout << "\nIntroduce la nueva fecha (ej. DD/MM/AAAA): ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              15); // Blanco para la entrada del usuario
      std::cin >> nuevaFecha;

      if (nuevaFecha.empty()) {
        mostrarTextoAnimado("Fecha invalida.", 12); // Rojo
        esperar(1000);
        break;
      }

      // Leer fechas actuales
      mostrarTextoAnimado("Leyendo fechas existentes...", 11); // Celeste
      esperar(300);
      std::vector<std::string> fechasActuales;
      std::ifstream archivoFechasIn("fechas.txt");
      std::string lineaFechas;
      if (getline(archivoFechasIn, lineaFechas)) {
        fechasActuales = split(lineaFechas, ',');
        if (!fechasActuales.empty() && fechasActuales.back().empty()) {
          fechasActuales.pop_back();
        }
      }
      archivoFechasIn.close();
      mostrarTextoAnimado("Fechas existentes leídas.", 10); // Verde

      fechasActuales.push_back(nuevaFecha); // Añadir la nueva fecha al vector

      // Escribir todas las fechas al archivo temporal
      mostrarTextoAnimado("Guardando nueva fecha...", 11); // Celeste
      std::ofstream archivoFechasTemp("fechas.temp");
      if (!archivoFechasTemp.is_open()) {
        mostrarTextoAnimado("Error al crear archivo temporal para fechas.",
                            12); // Rojo
        esperar(1000);
        break;
      }

      for (size_t i = 0; i < fechasActuales.size(); ++i) {
        archivoFechasTemp << fechasActuales[i];
        if (i < fechasActuales.size() - 1)
          archivoFechasTemp << ",";
      }
      archivoFechasTemp << "\n";
      archivoFechasTemp.close();

      // Reemplazar el archivo original
      remove("fechas.txt");
      rename("fechas.temp", "fechas.txt");
      mostrarTextoAnimado("Fecha agregada a fechas.txt", 10); // Verde

      // Actualizar asistencias.txt
      mostrarTextoAnimado("Actualizando archivo de asistencias...",
                          11); // Celeste
      mostrarBarraProgreso(
          1000); // Barra de progreso para la actualización grande

      std::vector<std::string> lineasAsistencia;
      std::ifstream archivoAsistenciasIn("asistencias.txt");
      std::string lineaAsistencia;
      while (getline(archivoAsistenciasIn, lineaAsistencia)) {
        lineasAsistencia.push_back(lineaAsistencia);
      }
      archivoAsistenciasIn.close();

      std::ofstream archivoAsistenciasOut("asistencias.temp");
      if (!archivoAsistenciasOut.is_open()) {
        mostrarTextoAnimado("Error al crear archivo temporal para asistencias.",
                            12); // Rojo
        esperar(1000);
        break;
      }

      for (const auto &l : lineasAsistencia) {
        if (!l.empty()) {
          archivoAsistenciasOut << l << ",0\n";
        } else {
          archivoAsistenciasOut << "\n";
        }
      }
      archivoAsistenciasOut.close();

      remove("asistencias.txt");
      rename("asistencias.temp", "asistencias.txt");
      mostrarTextoAnimado(
          "Archivo de asistencias actualizado con la nueva columna.",
          10); // Verde

      esperar(1000);
      break;
    }
    case 2: { // Eliminar Fecha
      system("cls");
      mostrarTextoAnimado("=== ELIMINAR FECHA ===", 12); // Rojo para eliminar

      // Leer fechas actuales
      std::vector<std::string> fechas;
      std::ifstream archivoFechasIn("fechas.txt");
      std::string lineaFechas;
      if (getline(archivoFechasIn, lineaFechas)) {
        fechas = split(lineaFechas, ',');
        if (!fechas.empty() && fechas.back().empty()) {
          fechas.pop_back();
        }
      }
      archivoFechasIn.close();

      if (fechas.empty()) {
        mostrarTextoAnimado("No hay fechas para eliminar.", 14); // Amarillo
        esperar(1000);
        break;
      }

      mostrarTextoAnimado("\nSelecciona el numero de la fecha a eliminar:\n",
                          11); // Celeste
      for (size_t i = 0; i < fechas.size(); ++i) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
        std::cout << i + 1 << ". ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
        std::cout << fechas[i] << "\n";
      }

      int opcionFechaEliminar;
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
      std::cout << "\nOpcion: ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
      std::cin >> opcionFechaEliminar;
      opcionFechaEliminar--; // Ajustar a indice base 0

      if (opcionFechaEliminar < 0 || opcionFechaEliminar >= fechas.size()) {
        mostrarTextoAnimado("Opcion invalida.", 12); // Rojo
        esperar(1000);
        break;
      }

      // Eliminar la fecha del vector
      std::string fechaEliminada = fechas[opcionFechaEliminar];
      fechas.erase(fechas.begin() + opcionFechaEliminar);

      mostrarTextoAnimado("\nEliminando fecha y actualizando archivos...",
                          11); // Mensaje antes de la carga
      mostrarBarraProgreso(
          1500); // Animación de carga (ajusta la duración si es necesario)

      // Escribir fechas sin la fecha eliminada al archivo temporal
      std::ofstream archivoFechasTemp("fechas.temp");
      if (!archivoFechasTemp.is_open()) {
        mostrarTextoAnimado("Error al crear archivo temporal para fechas.",
                            12); // Rojo
        esperar(1000);
        break;
      }
      for (size_t i = 0; i < fechas.size(); ++i) {
        archivoFechasTemp << fechas[i];
        if (i < fechas.size() - 1)
          archivoFechasTemp << ",";
      }
      archivoFechasTemp << "\n";
      archivoFechasTemp.close();

      // Reemplazar el archivo original
      remove("fechas.txt");
      rename("fechas.temp", "fechas.txt");
      mostrarTextoAnimado(
          "Fecha " + fechaEliminada + " eliminada de fechas.txt", 10); // Verde

      // Eliminar la columna correspondiente en asistencias.txt
      std::vector<std::vector<std::string>> asistencias;
      std::ifstream archivoAsistenciasIn("asistencias.txt");
      std::string lineaAsistencia;
      while (getline(archivoAsistenciasIn, lineaAsistencia)) {
        std::vector<std::string> datos = split(lineaAsistencia, ',');
        if (opcionFechaEliminar < datos.size()) {
          datos.erase(datos.begin() + opcionFechaEliminar);
        }
        asistencias.push_back(datos);
      }
      archivoAsistenciasIn.close();

      std::ofstream archivoAsistenciasOut("asistencias.temp");
      if (!archivoAsistenciasOut.is_open()) {
        mostrarTextoAnimado("Error al crear archivo temporal para asistencias.",
                            12); // Rojo
        esperar(1000);
        break;
      }

      for (size_t i = 0; i < asistencias.size(); ++i) {
        for (size_t j = 0; j < asistencias[i].size(); ++j) {
          archivoAsistenciasOut << asistencias[i][j];
          if (j < asistencias[i].size() - 1)
            archivoAsistenciasOut << ",";
        }
        archivoAsistenciasOut << "\n";
      }
      archivoAsistenciasOut.close();

      remove("asistencias.txt");
      rename("asistencias.temp", "asistencias.txt");

      mostrarTextoAnimado("Columna de asistencia correspondiente eliminada.",
                          10); // Verde

      esperar(1000);
      break;
    }
    case 0:
      mostrarTextoAnimado("Regresando al Menu Principal...", 13); // Magenta
      esperar(500);
      break;
    default:
      mostrarTextoAnimado("Opcion invalida.", 12); // Rojo
      esperar(1000);
      break;
    }
  } while (opcionGestionFechas != 0);
}

void ordenarAlumnosYArchivos(const std::vector<std::string>& alumnosOrdenados) {
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
    for (const auto& alumno : alumnosOrdenados) {
        archivoAlumnosOut << alumno << "\n";
    }
    archivoAlumnosOut.close();

    // Guardar asistencias ordenadas
    std::ofstream archivoAsistenciasOut("asistencias.txt");
    for (const auto& alumno : alumnosOrdenados) {
        if (mapaAsistencias.count(alumno) > 0) {
            const auto& asistencia = mapaAsistencias[alumno];
            for (size_t i = 0; i < asistencia.size(); ++i) {
                archivoAsistenciasOut << asistencia[i];
                if (i < asistencia.size() - 1) archivoAsistenciasOut << ",";
            }
            archivoAsistenciasOut << "\n";
        }
    }
    archivoAsistenciasOut.close();

    // Guardar calificaciones ordenadas
    std::ofstream archivoCalificacionesOut("calificaciones.txt");
    for (const auto& alumno : alumnosOrdenados) {
        if (mapaCalificaciones.count(alumno) > 0) {
            const auto& calificacion = mapaCalificaciones[alumno];
            for (size_t i = 0; i < calificacion.size(); ++i) {
                archivoCalificacionesOut << calificacion[i];
                if (i < calificacion.size() - 1) archivoCalificacionesOut << ",";
            }
            archivoCalificacionesOut << "\n";
        }
    }
    archivoCalificacionesOut.close();
}

void agregarAlumno() {
  int opcionGestionAlumnos;
  do {
    system("cls");
    mostrarTextoAnimado("=== GESTION DE ALUMNOS ===", 10);
    esperar(500);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    std::cout << "\nSelecciona una opcion:\n";
    std::cout << "1. Agregar Alumno\n";
    std::cout << "2. Eliminar Alumno\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
    std::cout << "0. Regresar al Menu Principal\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    std::cout << "\nSelecciona una opcion: ";
    std::cin >> opcionGestionAlumnos;
    std::cin.ignore();

    switch (opcionGestionAlumnos) {
    case 1: { // Agregar Alumno
      system("cls");
      mostrarTextoAnimado("=== AGREGAR ALUMNO ===", 10);
      esperar(500);

      std::string nuevoNombre;
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      std::cout << "\nIntroduce el nombre completo del nuevo alumno: ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::getline(std::cin, nuevoNombre);

      if (nuevoNombre.empty()) {
        mostrarTextoAnimado("Nombre invalido.", 12);
        esperar(1000);
        break;
      }

      mostrarTextoAnimado("Guardando alumno en alumnos.txt...", 11);
      std::ofstream archivoAlumnosOut("alumnos.txt", std::ios::app);
      if (!archivoAlumnosOut.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de alumnos para agregar.", 12);
        esperar(1000);
        break;
      }
      archivoAlumnosOut << nuevoNombre << "\n";
      archivoAlumnosOut.close();
      mostrarTextoAnimado("Alumno agregado a alumnos.txt", 10);

      mostrarTextoAnimado("Actualizando asistencias.txt...", 11);

      std::vector<std::string> fechas;
      std::ifstream archivoFechasIn("fechas.txt");
      std::string lineaFechas;
      if (getline(archivoFechasIn, lineaFechas)) {
        fechas = split(lineaFechas, ',');
        if (!fechas.empty() && fechas.back().empty()) {
          fechas.pop_back();
        }
      }
      archivoFechasIn.close();

      // Actualizar asistencias.txt
      std::ofstream archivoAsistenciasOut("asistencias.txt", std::ios::app);
      if (!archivoAsistenciasOut.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de asistencias para actualizar.", 12);
        esperar(1000);
        break;
      }

      for (size_t i = 0; i < fechas.size(); ++i) {
        archivoAsistenciasOut << "0";
        if (i < fechas.size() - 1)
          archivoAsistenciasOut << ",";
      }
      archivoAsistenciasOut << "\n";
      archivoAsistenciasOut.close();
      mostrarTextoAnimado("Asistencias.txt actualizado con la nueva fila.", 10);

      // Actualizar calificaciones.txt
      mostrarTextoAnimado("Actualizando calificaciones.txt...", 11);
      
      // Leer trabajos existentes
      std::vector<std::string> trabajos;
      std::ifstream archivoTrabajosIn("trabajos.txt");
      std::string lineaTrabajos;
      if (getline(archivoTrabajosIn, lineaTrabajos)) {
        trabajos = split(lineaTrabajos, ',');
        if (!trabajos.empty() && trabajos.back().empty()) {
          trabajos.pop_back();
        }
      }
      archivoTrabajosIn.close();

      // Agregar nueva fila de calificaciones
      std::ofstream archivoCalificacionesOut("calificaciones.txt", std::ios::app);
      if (!archivoCalificacionesOut.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de calificaciones para actualizar.", 12);
        esperar(1000);
        break;
      }

      for (size_t i = 0; i < trabajos.size(); ++i) {
        archivoCalificacionesOut << " ";
        if (i < trabajos.size() - 1)
          archivoCalificacionesOut << ",";
      }
      archivoCalificacionesOut << "\n";
      archivoCalificacionesOut.close();
      mostrarTextoAnimado("Calificaciones.txt actualizado con la nueva fila.", 10);

      // Ordenar alumnos por apellido
      std::vector<std::string> alumnos;
      std::ifstream archivoAlumnosIn("alumnos.txt");
      std::string linea;
      while (getline(archivoAlumnosIn, linea)) {
          alumnos.push_back(linea);
      }
      archivoAlumnosIn.close();
      
      std::sort(alumnos.begin(), alumnos.end(), compararPorApellido);
      ordenarAlumnosYArchivos(alumnos);

      mostrarTextoAnimado("\nAlumno " + nuevoNombre + " agregado con exito!", 10);
      esperar(1500);
      break;
    }
    case 2: { // Eliminar Alumno
      system("cls");
      mostrarTextoAnimado("=== ELIMINAR ALUMNO ===", 12);

      std::vector<std::string> alumnos;
      std::ifstream archivoAlumnosIn("alumnos.txt");
      std::string linea;
      while (getline(archivoAlumnosIn, linea)) {
        alumnos.push_back(linea);
      }
      archivoAlumnosIn.close();

      if (alumnos.empty()) {
        mostrarTextoAnimado("No hay alumnos para eliminar.", 14);
        esperar(1000);
        break;
      }

      mostrarTextoAnimado("\nSelecciona el alumno a eliminar:\n", 11);
      for (size_t i = 0; i < alumnos.size(); ++i) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
        std::cout << i + 1 << ". ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        std::cout << alumnos[i] << "\n";
      }

      int opcionAlumnoEliminar;
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
      std::cout << "\nOpcion: ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cin >> opcionAlumnoEliminar;
      opcionAlumnoEliminar--;

      if (opcionAlumnoEliminar < 0 || opcionAlumnoEliminar >= alumnos.size()) {
        mostrarTextoAnimado("Opcion invalida.", 12);
        esperar(1000);
        break;
      }

      std::string alumnoEliminado = alumnos[opcionAlumnoEliminar];
      alumnos.erase(alumnos.begin() + opcionAlumnoEliminar);

      mostrarTextoAnimado("\nEliminando alumno...", 11);
      mostrarBarraProgreso(1000);

      // Guardar lista actualizada de alumnos
      std::ofstream archivoAlumnosOut("alumnos.txt");
      if (!archivoAlumnosOut.is_open()) {
        mostrarTextoAnimado("Error al guardar cambios.", 12);
        esperar(1000);
        break;
      }
      for (const auto& alumno : alumnos) {
        archivoAlumnosOut << alumno << "\n";
      }
      archivoAlumnosOut.close();

      // Actualizar asistencias.txt
      std::vector<std::string> lineasAsistencia;
      std::ifstream archivoAsistenciasIn("asistencias.txt");
      std::string lineaAsistencia;
      while (getline(archivoAsistenciasIn, lineaAsistencia)) {
        lineasAsistencia.push_back(lineaAsistencia);
      }
      archivoAsistenciasIn.close();

      if (opcionAlumnoEliminar < lineasAsistencia.size()) {
        lineasAsistencia.erase(lineasAsistencia.begin() + opcionAlumnoEliminar);
      }

      std::ofstream archivoAsistenciasOut("asistencias.txt");
      if (!archivoAsistenciasOut.is_open()) {
        mostrarTextoAnimado("Error al actualizar asistencias.", 12);
        esperar(1000);
        break;
      }
      for (const auto& linea : lineasAsistencia) {
        archivoAsistenciasOut << linea << "\n";
      }
      archivoAsistenciasOut.close();

      // Ordenar alumnos por apellido
      std::sort(alumnos.begin(), alumnos.end(), compararPorApellido);
      ordenarAlumnosYArchivos(alumnos);

      mostrarTextoAnimado("Alumno eliminado exitosamente!", 10);
      esperar(1000);
      break;
    }
    case 0:
      mostrarTextoAnimado("Regresando al Menu Principal...", 13);
      esperar(500);
      break;
    default:
      mostrarTextoAnimado("Opcion invalida.", 12);
      esperar(1000);
      break;
    }
  } while (opcionGestionAlumnos != 0);
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

  const int COLUMNAS_VISIBLES = 5;
  const int ANCHO_NOMBRE = 20;
  const int ANCHO_CALIFICACION = 18;

  int desplazamiento = 0;

  // Mostrar la tabla inicialmente con animación completa
  bool primeraVez = true;

  do {
    if (primeraVez) {
      system("cls");
      mostrarTextoAnimado("=== LISTA DE TRABAJOS ===", 14);
      esperar(300);
    } else {
      system("cls"); // Limpiar pantalla rápido
    }

    // Dibujar tabla
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      std::cout << "-";
    std::cout << "+";

    for (int i = desplazamiento; i < desplazamiento + COLUMNAS_VISIBLES &&
                                 i < static_cast<int>(trabajosConFecha.size());
         ++i) {
      for (int j = 0; j < ANCHO_CALIFICACION; ++j)
        std::cout << "-";
      std::cout << "+";
    }
    std::cout << "\n";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "|";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
    std::cout << std::left << std::setw(ANCHO_NOMBRE) << "Nombre";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "|";

    for (int i = desplazamiento; i < desplazamiento + COLUMNAS_VISIBLES &&
                                 i < static_cast<int>(trabajosConFecha.size());
         ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      std::string nombre = trabajosConFecha[i].first;
      int padding = ANCHO_CALIFICACION - nombre.length();
      int pl = padding / 2, pr = padding - pl;
      for (int k = 0; k < pl; ++k)
        std::cout << " ";
      std::cout << nombre;
      for (int k = 0; k < pr; ++k)
        std::cout << " ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cout << "|";
    }
    std::cout << "\n";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "|";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      std::cout << " ";
    std::cout << "|";

    for (int i = desplazamiento; i < desplazamiento + COLUMNAS_VISIBLES &&
                                 i < static_cast<int>(trabajosConFecha.size());
         ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      std::string fecha = trabajosConFecha[i].second;
      int padding = ANCHO_CALIFICACION - fecha.length();
      int pl = padding / 2, pr = padding - pl;
      for (int k = 0; k < pl; ++k)
        std::cout << " ";
      std::cout << fecha;
      for (int k = 0; k < pr; ++k)
        std::cout << " ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cout << "|";
    }
    std::cout << "\n";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      std::cout << "-";
    std::cout << "+";

    for (int i = desplazamiento; i < desplazamiento + COLUMNAS_VISIBLES &&
                                 i < static_cast<int>(trabajosConFecha.size());
         ++i) {
      for (int j = 0; j < ANCHO_CALIFICACION; ++j)
        std::cout << "-";
      std::cout << "+";
    }
    std::cout << "\n";

    for (size_t i = 0; i < alumnos.size(); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cout << "|";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
      std::cout << std::left << std::setw(ANCHO_NOMBRE) << alumnos[i];
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cout << "|";

      for (int j = desplazamiento;
           j < desplazamiento + COLUMNAS_VISIBLES &&
           j < static_cast<int>(trabajosConFecha.size());
           ++j) {
        std::string valor = " ";
        int colorValor = 15;

        if (i < calificaciones.size() &&
            j < static_cast<int>(calificaciones[i].size())) {
          valor = calificaciones[i][j];
          try {
            int calif = std::stoi(valor);
            if (calif >= 6)
              colorValor = 10;
            else
              colorValor = 12;
          } catch (...) {
            valor = " ";
          }
        }

        int padding = ANCHO_CALIFICACION - valor.length();
        int pl = padding / 2, pr = padding - pl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        for (int k = 0; k < pl; ++k)
          std::cout << " ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorValor);
        std::cout << valor;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        for (int k = 0; k < pr; ++k)
          std::cout << " ";
        std::cout << "|";
      }
      if (primeraVez)
        esperar(50); // Animación solo en la carga inicial
      std::cout << "\n";
    }

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    std::cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      std::cout << "-";
    std::cout << "+";

    for (int i = desplazamiento; i < desplazamiento + COLUMNAS_VISIBLES &&
                                 i < static_cast<int>(trabajosConFecha.size());
         ++i) {
      for (int j = 0; j < ANCHO_CALIFICACION; ++j)
        std::cout << "-";
      std::cout << "+";
    }
    std::cout << "\n";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
    std::cout << "Usa <- -> para navegar. Presiona ENTER para regresar.\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    char key = _getch();
    switch (key) {
    case 75: // Flecha izquierda
      if (desplazamiento > 0) {
        desplazamiento--;
        primeraVez = false;
      }
      break;
    case 77: // Flecha derecha
      if (desplazamiento + COLUMNAS_VISIBLES <
          static_cast<int>(trabajosConFecha.size())) {
        desplazamiento++;
        primeraVez = false;
      }
      break;
    case 13: // Enter
      return;
    }

  } while (true);
}

void modificarTrabajos() {
  while (true) {
    system("cls");
    mostrarTextoAnimado("=== MODIFICACION DE TRABAJOS ===", 14);
    esperar(500);

    std::ifstream archivoAlumnos("alumnos.txt");
    std::ifstream archivoTrabajos("trabajos.txt");
    std::ifstream archivoCalificaciones("calificaciones.txt");

    if (!archivoAlumnos.is_open() || !archivoTrabajos.is_open() ||
        !archivoCalificaciones.is_open()) {
      mostrarTextoAnimado("Error al abrir alguno de los archivos necesarios.",
                          12);
      esperar(1000);
      return;
    }

    std::vector<std::string> alumnos;
    std::string linea;
    while (getline(archivoAlumnos, linea)) {
      alumnos.push_back(linea);
    }
    archivoAlumnos.close();

    std::vector<std::string> trabajos;
    std::string lineaTrabajos;
    if (getline(archivoTrabajos, lineaTrabajos)) {
      trabajos = split(lineaTrabajos, ',');
      if (!trabajos.empty() && trabajos.back().empty()) {
        trabajos.pop_back();
      }
    }
    archivoTrabajos.close();

    std::vector<std::vector<std::string>> calificaciones;
    while (getline(archivoCalificaciones, linea)) {
      auto datos = split(linea, ',');
      calificaciones.push_back(datos);
    }
    archivoCalificaciones.close();

    if (trabajos.empty() || alumnos.empty()) {
      if (trabajos.empty())
        mostrarTextoAnimado("No hay trabajos disponibles para modificar.", 14);
      if (alumnos.empty())
        mostrarTextoAnimado("No hay alumnos disponibles para modificar.", 14);
      esperar(1500);
      return;
    }

    mostrarTextoAnimado("\nSelecciona el trabajo a modificar:", 11);
    std::cout << "\n";
    for (size_t i = 0; i < trabajos.size(); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
      std::cout << i + 1 << ". ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cout << trabajos[i] << "\n";
      esperar(100);
    }

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
    std::cout << "0. Regresar al Menu Principal\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    int opcionModificar;
    std::cout << "\nOpcion: ";
    std::cin >> opcionModificar;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (opcionModificar == 0) {
      mostrarTextoAnimado("Regresando al Menu Principal...", 13);
      esperar(500);
      return;
    }

    int indiceTrabajo = opcionModificar - 1;

    if (indiceTrabajo < 0 ||
        indiceTrabajo >= static_cast<int>(trabajos.size())) {
      mostrarTextoAnimado("Opcion invalida.", 12);
      esperar(1000);
      continue;
    }

    system("cls");
    mostrarTextoAnimado("=== REGISTRO DE CALIFICACIONES ===", 14);
    mostrarTextoAnimado("Trabajo: " + trabajos[indiceTrabajo], 11);

    for (size_t i = 0; i < alumnos.size(); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
      std::cout << "\n" << alumnos[i] << "\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
      std::cout << "Calificacion (1-10): ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

      std::string valor;
      std::cin >> valor;

      // Validar que la calificación esté entre 1 y 10
      try {
        int calificacion = std::stoi(valor);
        if (calificacion < 1 || calificacion > 10) {
          valor = " ";
        }
      } catch (...) {
        valor = " ";
      }

      if (i < calificaciones.size() &&
          indiceTrabajo < static_cast<int>(calificaciones[i].size())) {
        calificaciones[i][indiceTrabajo] = valor;
      } else {
        mostrarTextoAnimado("Error: Datos incompletos para este alumno.", 12);
      }
    }

    mostrarTextoAnimado("\nGuardando cambios...", 11);
    mostrarBarraProgreso(1000);

    std::ofstream archivoOut("calificaciones.txt");
    if (!archivoOut.is_open()) {
      mostrarTextoAnimado("Error al guardar el archivo de calificaciones.", 12);
      esperar(1000);
    } else {
      for (const auto &fila : calificaciones) {
        for (size_t j = 0; j < fila.size(); ++j) {
          archivoOut << fila[j];
          if (j != fila.size() - 1)
            archivoOut << ",";
        }
        archivoOut << "\n";
      }
      archivoOut.close();
      mostrarTextoAnimado("\n¡Calificaciones actualizadas correctamente!", 10);
    }

    esperar(1000);
  }
}

void agregarTrabajo() {
  system("cls");
  mostrarTextoAnimado("=== AGREGAR TRABAJO ===", 10);
  esperar(500);

  std::string nuevoTrabajo;
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
  std::cout << "\nIntroduce el nombre del nuevo trabajo: ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
  std::getline(std::cin, nuevoTrabajo);

  if (nuevoTrabajo.empty()) {
    mostrarTextoAnimado("Nombre invalido.", 12);
    esperar(1000);
    return;
  }

  mostrarTextoAnimado("Guardando trabajo en trabajos.txt...", 11);
  std::ofstream archivoTrabajosOut("trabajos.txt", std::ios::app);
  if (!archivoTrabajosOut.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de trabajos para agregar.",
                        12);
    esperar(1000);
    return;
  }
  archivoTrabajosOut << nuevoTrabajo << ",";
  archivoTrabajosOut.close();
  mostrarTextoAnimado("Trabajo agregado a trabajos.txt", 10);

  mostrarTextoAnimado("Actualizando calificaciones.txt...", 11);

  std::vector<std::string> lineasCalificacion;
  std::ifstream archivoCalificacionesIn("calificaciones.txt");
  std::string lineaCalificacion;
  while (getline(archivoCalificacionesIn, lineaCalificacion)) {
    lineasCalificacion.push_back(lineaCalificacion);
  }
  archivoCalificacionesIn.close();

  std::ofstream archivoCalificacionesOut("calificaciones.txt");
  if (!archivoCalificacionesOut.is_open()) {
    mostrarTextoAnimado(
        "Error al abrir el archivo de calificaciones para actualizar.", 12);
    esperar(1000);
    return;
  }

  for (const auto &l : lineasCalificacion) {
    if (!l.empty()) {
      archivoCalificacionesOut << l << ",\n";
    } else {
      archivoCalificacionesOut << "\n";
    }
  }
  archivoCalificacionesOut.close();
  mostrarTextoAnimado("Calificaciones.txt actualizado con la nueva columna.",
                      10);

  mostrarTextoAnimado("\nTrabajo " + nuevoTrabajo + " agregado con exito!", 10);
  esperar(1500);
}

void agregarTrabajoConFecha() {
  system("cls");
  mostrarTextoAnimado("=== AGREGAR TRABAJO CON FECHA ===", 10);
  esperar(500);

  std::string nuevoTrabajo;
  std::string fechaTrabajo;

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
  std::cout << "\nIntroduce el nombre del nuevo trabajo: ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
  std::getline(std::cin, nuevoTrabajo);

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
  std::cout << "Introduce la fecha del trabajo (DD/MM/AAAA): ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
  std::getline(std::cin, fechaTrabajo);

  if (nuevoTrabajo.empty() || fechaTrabajo.empty()) {
    mostrarTextoAnimado("Datos invalidos.", 12);
    esperar(1000);
    return;
  }

  // Combinar nombre y fecha
  std::string trabajoCompleto = nuevoTrabajo + " (" + fechaTrabajo + ")";

  mostrarTextoAnimado("Guardando trabajo en trabajos.txt...", 11);
  std::ofstream archivoTrabajosOut("trabajos.txt", std::ios::app);
  if (!archivoTrabajosOut.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de trabajos para agregar.",
                        12);
    esperar(1000);
    return;
  }
  archivoTrabajosOut << trabajoCompleto << ",";
  archivoTrabajosOut.close();
  mostrarTextoAnimado("Trabajo agregado a trabajos.txt", 10);

  mostrarTextoAnimado("Actualizando calificaciones.txt...", 11);

  std::vector<std::string> lineasCalificacion;
  std::ifstream archivoCalificacionesIn("calificaciones.txt");
  std::string lineaCalificacion;
  while (getline(archivoCalificacionesIn, lineaCalificacion)) {
    lineasCalificacion.push_back(lineaCalificacion);
  }
  archivoCalificacionesIn.close();

  std::ofstream archivoCalificacionesOut("calificaciones.txt");
  if (!archivoCalificacionesOut.is_open()) {
    mostrarTextoAnimado(
        "Error al abrir el archivo de calificaciones para actualizar.", 12);
    esperar(1000);
    return;
  }

  for (const auto &l : lineasCalificacion) {
    if (!l.empty()) {
      archivoCalificacionesOut << l << ",\n";
    } else {
      archivoCalificacionesOut << "\n";
    }
  }
  archivoCalificacionesOut.close();
  mostrarTextoAnimado("Calificaciones.txt actualizado con la nueva columna.",
                      10);

  mostrarTextoAnimado("\nTrabajo " + trabajoCompleto + " agregado con exito!",
                      10);
  esperar(1500);
}

void gestionarTrabajos() {
  int opcionGestionTrabajos;
  do {
    system("cls");
    mostrarTextoAnimado("=== GESTION DE TRABAJOS ===", 10);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    std::cout << "\nSelecciona una opcion:\n";
    std::cout << "1. Agregar Trabajo\n";
    std::cout << "2. Eliminar Trabajo\n";
    std::cout << "3. Modificar Trabajo\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
    std::cout << "0. Regresar al Menu Principal\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    std::cout << "\nSelecciona una opcion: ";
    std::cin >> opcionGestionTrabajos;
    std::cin.ignore();

    switch (opcionGestionTrabajos) {
    case 1: { // Agregar Trabajo
      system("cls");
      mostrarTextoAnimado("=== AGREGAR TRABAJO ===", 10);
      esperar(500);

      std::string nuevoTrabajo;
      std::string fechaTrabajo;

      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      std::cout << "\nIntroduce el nombre del nuevo trabajo: ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::getline(std::cin, nuevoTrabajo);

      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      std::cout << "Introduce la fecha del trabajo (DD/MM/AAAA): ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::getline(std::cin, fechaTrabajo);

      if (nuevoTrabajo.empty() || fechaTrabajo.empty()) {
        mostrarTextoAnimado("Datos invalidos.", 12);
        esperar(1000);
        break;
      }

      std::string trabajoCompleto = nuevoTrabajo + " (" + fechaTrabajo + ")";

      mostrarTextoAnimado("Guardando trabajo...", 11);
      std::ofstream archivoTrabajosOut("trabajos.txt", std::ios::app);
      if (!archivoTrabajosOut.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de trabajos.", 12);
        esperar(1000);
        break;
      }
      archivoTrabajosOut << trabajoCompleto << ",";
      archivoTrabajosOut.close();

      // Actualizar calificaciones.txt
      std::vector<std::string> lineasCalificacion;
      std::ifstream archivoCalificacionesIn("calificaciones.txt");
      std::string lineaCalificacion;
      while (getline(archivoCalificacionesIn, lineaCalificacion)) {
        lineasCalificacion.push_back(lineaCalificacion);
      }
      archivoCalificacionesIn.close();

      std::ofstream archivoCalificacionesOut("calificaciones.txt");
      if (!archivoCalificacionesOut.is_open()) {
        mostrarTextoAnimado("Error al actualizar calificaciones.", 12);
        esperar(1000);
        break;
      }

      for (const auto &l : lineasCalificacion) {
        if (!l.empty()) {
          archivoCalificacionesOut << l << ",\n";
        } else {
          archivoCalificacionesOut << "\n";
        }
      }
      archivoCalificacionesOut.close();

      mostrarTextoAnimado("Trabajo agregado exitosamente!", 10);
      esperar(1000);
      break;
    }
    case 2: { // Eliminar Trabajo
      system("cls");
      mostrarTextoAnimado("=== ELIMINAR TRABAJO ===", 12);

      std::vector<std::string> trabajos;
      std::ifstream archivoTrabajosIn("trabajos.txt");
      std::string lineaTrabajos;
      if (getline(archivoTrabajosIn, lineaTrabajos)) {
        trabajos = split(lineaTrabajos, ',');
        if (!trabajos.empty() && trabajos.back().empty()) {
          trabajos.pop_back();
        }
      }
      archivoTrabajosIn.close();

      if (trabajos.empty()) {
        mostrarTextoAnimado("No hay trabajos para eliminar.", 14);
        esperar(1000);
        break;
      }

      mostrarTextoAnimado("\nSelecciona el trabajo a eliminar:\n", 11);
      for (size_t i = 0; i < trabajos.size(); ++i) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
        std::cout << i + 1 << ". ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        std::cout << trabajos[i] << "\n";
      }

      int opcionTrabajoEliminar;
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
      std::cout << "\nOpcion: ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cin >> opcionTrabajoEliminar;
      opcionTrabajoEliminar--;

      if (opcionTrabajoEliminar < 0 ||
          opcionTrabajoEliminar >= trabajos.size()) {
        mostrarTextoAnimado("Opcion invalida.", 12);
        esperar(1000);
        break;
      }

      std::string trabajoEliminado = trabajos[opcionTrabajoEliminar];
      trabajos.erase(trabajos.begin() + opcionTrabajoEliminar);

      mostrarTextoAnimado("\nEliminando trabajo...", 11);
      mostrarBarraProgreso(1000);

      std::ofstream archivoTrabajosOut("trabajos.txt");
      if (!archivoTrabajosOut.is_open()) {
        mostrarTextoAnimado("Error al guardar cambios.", 12);
        esperar(1000);
        break;
      }

      for (size_t i = 0; i < trabajos.size(); ++i) {
        archivoTrabajosOut << trabajos[i];
        if (i < trabajos.size() - 1)
          archivoTrabajosOut << ",";
      }
      archivoTrabajosOut << ",";
      archivoTrabajosOut.close();

      // Actualizar calificaciones.txt
      std::vector<std::vector<std::string>> calificaciones;
      std::ifstream archivoCalificacionesIn("calificaciones.txt");
      std::string lineaCalificacion;
      while (getline(archivoCalificacionesIn, lineaCalificacion)) {
        std::vector<std::string> datos = split(lineaCalificacion, ',');
        if (opcionTrabajoEliminar < datos.size()) {
          datos.erase(datos.begin() + opcionTrabajoEliminar);
        }
        calificaciones.push_back(datos);
      }
      archivoCalificacionesIn.close();

      std::ofstream archivoCalificacionesOut("calificaciones.txt");
      if (!archivoCalificacionesOut.is_open()) {
        mostrarTextoAnimado("Error al actualizar calificaciones.", 12);
        esperar(1000);
        break;
      }

      for (const auto &fila : calificaciones) {
        for (size_t j = 0; j < fila.size(); ++j) {
          archivoCalificacionesOut << fila[j];
          if (j < fila.size() - 1)
            archivoCalificacionesOut << ",";
        }
        archivoCalificacionesOut << "\n";
      }
      archivoCalificacionesOut.close();

      mostrarTextoAnimado("Trabajo eliminado exitosamente!", 10);
      esperar(1000);
      break;
    }
    case 3: { // Modificar Trabajo
      system("cls");
      mostrarTextoAnimado("=== MODIFICAR TRABAJO ===", 14);

      std::vector<std::string> trabajos;
      std::ifstream archivoTrabajosIn("trabajos.txt");
      std::string lineaTrabajos;
      if (getline(archivoTrabajosIn, lineaTrabajos)) {
        trabajos = split(lineaTrabajos, ',');
        if (!trabajos.empty() && trabajos.back().empty()) {
          trabajos.pop_back();
        }
      }
      archivoTrabajosIn.close();

      if (trabajos.empty()) {
        mostrarTextoAnimado("No hay trabajos para modificar.", 14);
        esperar(1000);
        break;
      }

      mostrarTextoAnimado("\nSelecciona el trabajo a modificar:\n", 11);
      for (size_t i = 0; i < trabajos.size(); ++i) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
        std::cout << i + 1 << ". ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        std::cout << trabajos[i] << "\n";
      }

      int opcionTrabajoModificar;
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
      std::cout << "\nOpcion: ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::cin >> opcionTrabajoModificar;
      std::cin.ignore();
      opcionTrabajoModificar--;

      if (opcionTrabajoModificar < 0 ||
          opcionTrabajoModificar >= trabajos.size()) {
        mostrarTextoAnimado("Opcion invalida.", 12);
        esperar(1000);
        break;
      }

      std::string nuevoNombre;
      std::string nuevaFecha;

      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      std::cout << "\nIntroduce el nuevo nombre del trabajo: ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::getline(std::cin, nuevoNombre);

      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      std::cout << "Introduce la nueva fecha (DD/MM/AAAA): ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      std::getline(std::cin, nuevaFecha);

      if (nuevoNombre.empty() || nuevaFecha.empty()) {
        mostrarTextoAnimado("Datos invalidos.", 12);
        esperar(1000);
        break;
      }

      std::string trabajoModificado = nuevoNombre + " (" + nuevaFecha + ")";
      trabajos[opcionTrabajoModificar] = trabajoModificado;

      mostrarTextoAnimado("\nGuardando cambios...", 11);
      mostrarBarraProgreso(1000);

      std::ofstream archivoTrabajosOut("trabajos.txt");
      if (!archivoTrabajosOut.is_open()) {
        mostrarTextoAnimado("Error al guardar cambios.", 12);
        esperar(1000);
        break;
      }

      for (size_t i = 0; i < trabajos.size(); ++i) {
        archivoTrabajosOut << trabajos[i];
        if (i < trabajos.size() - 1)
          archivoTrabajosOut << ",";
      }
      archivoTrabajosOut << ",";
      archivoTrabajosOut.close();

      mostrarTextoAnimado("Trabajo modificado exitosamente!", 10);
      esperar(1000);
      break;
    }
    case 0:
      mostrarTextoAnimado("Regresando al Menu Principal...", 13);
      esperar(500);
      break;
    default:
      mostrarTextoAnimado("Opcion invalida.", 12);
      esperar(1000);
      break;
    }
  } while (opcionGestionTrabajos != 0);
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

void gestionarCriteriosEvaluacion() {
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

    float valorAsistencias, valorTrabajos, valorExamen;
    float total = 0;

    do {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
        std::cout << "\nIngrese los porcentajes para cada criterio (debe sumar 100%):\n\n";
        
        std::cout << "Porcentaje para Asistencias (0-100): ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        std::cin >> valorAsistencias;

        std::cout << "\nPorcentaje para Trabajos (0-100): ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        std::cin >> valorTrabajos;

        std::cout << "\nPorcentaje para Examen (0-100): ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        std::cin >> valorExamen;

        total = valorAsistencias + valorTrabajos + valorExamen;

        if (total != 100) {
            mostrarTextoAnimado("\nError: Los porcentajes deben sumar 100%. Intente nuevamente.", 12);
            esperar(1500);
            system("cls");
        }
    } while (total != 100);

    std::ofstream archivo("criterios.txt");
    if (!archivo.is_open()) {
        mostrarTextoAnimado("Error al guardar los criterios.", 12);
        esperar(1000);
        return;
    }

    archivo << valorAsistencias << "," << valorTrabajos << "," << valorExamen;
    archivo.close();

    mostrarTextoAnimado("\nCriterios de evaluación guardados exitosamente!", 10);
    esperar(1500);
}

void modificarCriteriosEvaluacion() {
    system("cls");
    mostrarTextoAnimado("=== MODIFICAR CRITERIOS DE EVALUACION ===", 14);
    esperar(500);

    std::ifstream archivoIn("criterios.txt");
    if (!archivoIn.is_open()) {
        mostrarTextoAnimado("Error al leer los criterios actuales.", 12);
        esperar(1000);
        return;
    }

    std::string linea;
    std::getline(archivoIn, linea);
    archivoIn.close();

    auto valores = split(linea, ',');
    float valorAsistencias = std::stof(valores[0]);
    float valorTrabajos = std::stof(valores[1]);
    float valorExamen = std::stof(valores[2]);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
    std::cout << "\nCriterios actuales:\n";
    std::cout << "Asistencias: " << valorAsistencias << "%\n";
    std::cout << "Trabajos: " << valorTrabajos << "%\n";
    std::cout << "Examen: " << valorExamen << "%\n\n";

    float total = 0;
    do {
        std::cout << "Ingrese los nuevos porcentajes (debe sumar 100%):\n\n";
        
        std::cout << "Nuevo porcentaje para Asistencias (0-100): ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        std::cin >> valorAsistencias;

        std::cout << "\nNuevo porcentaje para Trabajos (0-100): ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        std::cin >> valorTrabajos;

        std::cout << "\nNuevo porcentaje para Examen (0-100): ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        std::cin >> valorExamen;

        total = valorAsistencias + valorTrabajos + valorExamen;

        if (total != 100) {
            mostrarTextoAnimado("\nError: Los porcentajes deben sumar 100%. Intente nuevamente.", 12);
            esperar(1500);
            system("cls");
        }
    } while (total != 100);

    std::ofstream archivoOut("criterios.txt");
    if (!archivoOut.is_open()) {
        mostrarTextoAnimado("Error al guardar los criterios.", 12);
        esperar(1000);
        return;
    }

    archivoOut << valorAsistencias << "," << valorTrabajos << "," << valorExamen;
    archivoOut.close();

    mostrarTextoAnimado("\nCriterios de evaluación actualizados exitosamente!", 10);
    esperar(1500);
}

bool existenCriteriosEvaluacion() {
    std::ifstream archivo("criterios.txt");
    return archivo.good();
}

int main() {
  setUTF8();

  int opcion;
  do {
    mostrarMenu();
    std::cout << "\nSelecciona una opcion: ";
    std::cin >> opcion;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    switch (opcion) {
    case 1:
      mostrarCuadriculaAsistencias();
      break;
    case 2:
      modificarAsistencias();
      break;
    case 3:
      mostrarCuadriculaTrabajos();
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
