#include <algorithm>
#include <chrono>
#include <conio.h>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits> // Necesario para numeric_limits
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

using namespace std;

// Declaraciones de funciones
void setUTF8();
vector<string> split(const string &linea, char delim);
void mostrarMenu();
void mostrarCuadriculaAsistencias(bool animacionRapida = false);
void modificarAsistencias();
void modificarAsistenciaDeUnAlumno();
void gestionarFechas();
void agregarAlumno();
void esperar(int milisegundos);
void mostrarTextoAnimado(const string &texto, int color);
void mostrarBarraProgreso(int duracion);
void mostrarCuadriculaTrabajos();
void modificarTrabajos();
void agregarTrabajo();
void agregarTrabajoConFecha();
void gestionarTrabajos();
void ordenarAlumnosYArchivos(const vector<string> &alumnosOrdenados);
void gestionarCriteriosEvaluacion();
void agregarCriteriosEvaluacion();
void modificarCriteriosEvaluacion();
bool existenCriteriosEvaluacion();
void mostrarTablaCriterios();
void modificarCalificacionAlumno();
void calcularPromedioParcial();
void calcularPromedioSemestre();
void gen();
bool validarEntradaNumerica(const string &entrada, float &valor, float min,
                            float max);
bool validarFormatoFecha(const string &fecha);
bool archivoExiste(const string &nombreArchivo);
bool eliminarArchivo(const string &nombreArchivo);
bool renombrarArchivo(const string &viejoNombre, const string &nuevoNombre);
void eliminarTrabajo();
void inicializarArchivos();
void registrarTrabajos();
void animacionSalida(const std::string &nombreSeccion,
                     bool esSalidaSistema = false, int color = 11);
void calcularPromedioFinal();

// Agregar después de las declaraciones de funciones existentes
string obtenerApellido(const string &nombreCompleto) {
  stringstream ss(nombreCompleto);
  string apellido;
  ss >> apellido; // Obtiene la primera palabra (apellido)
  return apellido;
}

bool compararPorApellido(const string &a, const string &b) {
  return obtenerApellido(a) < obtenerApellido(b);
}

void setUTF8() { system("chcp 65001 > nul"); }

vector<string> split(const string &linea, char delim) {
  stringstream ss(linea);
  string parte;
  vector<string> resultado;

  while (getline(ss, parte, delim)) {
    resultado.push_back(parte);
  }
  return resultado;
}

void mostrarMenu() {
  system("cls");

  // Obtener el ancho de la consola
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  int anchoPantalla = csbi.srWindow.Right - csbi.srWindow.Left + 1;

  // Función auxiliar para centrar texto
  auto centrarTexto = [anchoPantalla](const string &texto) {
    int espacios = (anchoPantalla - texto.length()) / 2;
    return string(espacios, ' ') + texto;
  };

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); // Color celeste
  cout << centrarTexto("===== SISTEMA DE GESTION ESCOLAR =====\n\n");

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
  cout << centrarTexto("=== CONSULTAS ===\n");
  cout << centrarTexto("1.  Lista de asistencias\n");
  cout << centrarTexto("2.  Lista de trabajos\n\n");

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
  cout << centrarTexto("=== REGISTROS ===\n");
  cout << centrarTexto("3.  Registrar asistencias\n");
  cout << centrarTexto("4.  Registrar trabajos\n");
  cout << centrarTexto("5.  Agregar alumnos\n\n");

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta
  cout << centrarTexto("=== GESTION ===\n");
  cout << centrarTexto("6.  Gestionar Fechas\n");
  cout << centrarTexto("7.  Gestionar Trabajos\n");
  cout << centrarTexto("8.  Gestionar Criterios de Evaluacion\n");
  cout << centrarTexto("9.  Modificar Calificacion de Alumno\n");
  cout << centrarTexto("10. Modificar Asistencia de Alumno\n\n");

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
  cout << centrarTexto("=== PROMEDIOS ===\n");
  cout << centrarTexto("11. Calcular Promedio Parcial\n");
  cout << centrarTexto("12. Calcular Promedio Semestre\n\n");

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3); // Cian
  cout << centrarTexto("0.  Salir\n");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
}

void mostrarCuadriculaAsistencias(bool animacionRapida) {
  ifstream archivoAlumnos("alumnos.txt");
  ifstream archivoFechas("fechas.txt");
  ifstream archivoAsistencias("asistencias.txt");

  system("cls");
  mostrarTextoAnimado("=== CARGANDO LISTA DE ASISTENCIAS ===", 11);
  esperar(animacionRapida ? 50 : 500);

  if (!archivoAlumnos.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
    esperar(animacionRapida ? 50 : 1000);
    animacionSalida("asistencias", false, 11);
    return;
  }
  if (!archivoFechas.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de fechas.", 12);
    esperar(animacionRapida ? 50 : 1000);
    animacionSalida("fechas", false, 14);
    return;
  }
  if (!archivoAsistencias.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de asistencias.", 12);
    esperar(animacionRapida ? 50 : 1000);
    animacionSalida("asistencias", false, 11);
    return;
  }

  vector<string> alumnos;
  string linea;
  while (getline(archivoAlumnos, linea)) {
    alumnos.push_back(linea);
  }
  archivoAlumnos.close();

  vector<string> fechas;
  string lineaFechas;
  if (getline(archivoFechas, lineaFechas)) {
    fechas = split(lineaFechas, ',');
    if (!fechas.empty() && fechas.back().empty()) {
      fechas.pop_back();
    }
  }
  archivoFechas.close();

  vector<vector<string>> asistencias;
  while (getline(archivoAsistencias, linea)) {
    auto datos = split(linea, ',');
    asistencias.push_back(datos);
  }
  archivoAsistencias.close();

  // Calcular el ancho máximo del nombre
  size_t maxAnchoNombre = 0;
  for (const auto &alumno : alumnos) {
    if (alumno.length() > maxAnchoNombre) {
      maxAnchoNombre = alumno.length();
    }
  }
  // Ajustar el ancho para que sea múltiplo de 4 y mínimo 30
  const int ANCHO_NOMBRE =
      max(30, static_cast<int>((maxAnchoNombre + 3) / 4 * 4 + 2));
  const int ANCHO_COLUMNA_DATOS = 15;

  // Calcular cuántas columnas caben en la pantalla
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  int anchoPantalla = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  int columnasVisibles =
      (anchoPantalla - ANCHO_NOMBRE - 2) / (ANCHO_COLUMNA_DATOS + 1);

  int inicioColumnas = 0;
  bool salir = false;

  while (!salir) {
    system("cls");
    mostrarTextoAnimado("=== LISTA DE ASISTENCIAS ===", 14);
    esperar(animacionRapida ? 30 : 300);

    // Dibujar encabezado de la tabla
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      cout << "-";
    cout << "+";
    for (int i = 0; i < min(columnasVisibles,
                            static_cast<int>(fechas.size()) - inicioColumnas);
         ++i) {
      for (int j = 0; j < ANCHO_COLUMNA_DATOS; ++j)
        cout << "-";
      cout << "+";
    }
    cout << "\n";

    // Fila 1: Nombre y Fechas
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "|";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
    cout << left << setw(ANCHO_NOMBRE) << "Nombre";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "|";
    for (int i = 0; i < min(columnasVisibles,
                            static_cast<int>(fechas.size()) - inicioColumnas);
         ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      string fecha = fechas[inicioColumnas + i];
      int padding = ANCHO_COLUMNA_DATOS - fecha.length();
      int pl = padding / 2, pr = padding - pl;
      for (int k = 0; k < pl; ++k)
        cout << " ";
      cout << fecha;
      for (int k = 0; k < pr; ++k)
        cout << " ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      cout << "|";
    }
    cout << "\n";

    // Línea divisoria
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      cout << "-";
    cout << "+";
    for (int i = 0; i < min(columnasVisibles,
                            static_cast<int>(fechas.size()) - inicioColumnas);
         ++i) {
      for (int j = 0; j < ANCHO_COLUMNA_DATOS; ++j)
        cout << "-";
      cout << "+";
    }
    cout << "\n";

    // Filas de asistencia
    for (size_t i = 0; i < alumnos.size(); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      cout << "|";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
      cout << left << setw(ANCHO_NOMBRE) << alumnos[i];
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      cout << "|";

      // Imprimir todos los valores de asistencia en una sola línea
      for (int j = 0; j < min(columnasVisibles,
                              static_cast<int>(fechas.size()) - inicioColumnas);
           ++j) {
        string valor = " ";
        int colorValor = 15;
        if (i < asistencias.size() &&
            inicioColumnas + j < asistencias[i].size()) {
          valor = asistencias[i][inicioColumnas + j];
          colorValor = (valor == "1") ? 10 : 12;
          valor = (valor == "1") ? "S" : "N";
        }
        int padding = ANCHO_COLUMNA_DATOS - valor.length();
        int pl = padding / 2, pr = padding - pl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        for (int k = 0; k < pl; ++k)
          cout << " ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorValor);
        cout << valor;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        for (int k = 0; k < pr; ++k)
          cout << " ";
        cout << "|";
      }
      cout << "\n";
    }

    // Línea inferior
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      cout << "-";
    cout << "+";
    for (int i = 0; i < min(columnasVisibles,
                            static_cast<int>(fechas.size()) - inicioColumnas);
         ++i) {
      for (int j = 0; j < ANCHO_COLUMNA_DATOS; ++j)
        cout << "-";
      cout << "+";
    }
    cout << "\n";

    // Instrucciones de navegación
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
    cout << "\nFlechas Izq/Der: Navegar | ENTER: Regresar al Menu Principal\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    // Manejo de teclas
    int tecla = _getch();
    if (tecla == 224) { // Tecla especial (flechas)
      int flecha = _getch();
      if (flecha == 75) { // Izquierda
        if (inicioColumnas > 0)
          inicioColumnas--;
        // Animación rápida al navegar
        animacionRapida = true;
      } else if (flecha == 77) { // Derecha
        if (inicioColumnas + columnasVisibles < fechas.size())
          inicioColumnas++;
        // Animación rápida al navegar
        animacionRapida = true;
      }
    } else if (tecla == 13) { // ENTER
      salir = true;
    }
  }
}

void esperar(int milisegundos) { Sleep(milisegundos); }

void mostrarTextoAnimado(const string &texto, int color) {
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
  for (char c : texto) {
    cout << c;
    cout.flush();
    esperar(20);
  }
  cout << "\n";
}

void mostrarBarraProgreso(int duracion) {
  cout << "[";
  for (int i = 0; i < 50; i++) {
    cout << "=";
    cout.flush();
    esperar(duracion / 50);
  }
  cout << "]\n";
}

void modificarAsistencias() {
  system("cls");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("=== MODIFICAR ASISTENCIAS ===", 13);
  esperar(500);

  ifstream archivoAlumnos("alumnos.txt");
  if (!archivoAlumnos.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
    esperar(1000);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return;
  }

  vector<string> alumnos;
  string linea;
  while (getline(archivoAlumnos, linea)) {
    alumnos.push_back(linea);
  }
  archivoAlumnos.close();

  if (alumnos.empty()) {
    mostrarTextoAnimado("No hay alumnos registrados.", 12);
    esperar(1000);
    animacionSalida("asistencias", false, 11);
    return;
  }

  ifstream archivoFechas("fechas.txt");
  if (!archivoFechas.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    mostrarTextoAnimado("Error al abrir el archivo de fechas.", 12);
    esperar(1000);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return;
  }

  string lineaFechas;
  vector<string> fechas;
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
    animacionSalida("asistencias", false, 11);
    return;
  }

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9); // Azul neón
  cout << "\nSeleccione la fecha:\n\n";
  for (size_t i = 0; i < fechas.size(); ++i) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    cout << i + 1 << ". ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << fechas[i] << "\n";
  }

  float seleccionFecha;
  string entrada;
  do {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    cout << "\nIngrese el numero de la fecha (o 0 para regresar): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    getline(cin, entrada);

    if (!validarEntradaNumerica(entrada, seleccionFecha, 0, fechas.size())) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
      mostrarTextoAnimado("Seleccion invalida. Intente de nuevo.", 12);
      continue;
    }
    break;
  } while (true);

  if (seleccionFecha == 0) {
    animacionSalida("asistencias", false, 11);
    return;
  }

  ifstream archivoAsistencias("asistencias.txt");
  if (!archivoAsistencias.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    mostrarTextoAnimado("Error al abrir el archivo de asistencias.", 12);
    esperar(1000);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return;
  }

  vector<vector<string>> asistencias;
  while (getline(archivoAsistencias, linea)) {
    asistencias.push_back(split(linea, ','));
  }
  archivoAsistencias.close();

  system("cls");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("=== REGISTRO DE ASISTENCIAS ===", 13);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9); // Azul neón
  mostrarTextoAnimado("Fecha: " + fechas[static_cast<int>(seleccionFecha) - 1],
                      9);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

  for (size_t i = 0; i < alumnos.size(); ++i) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    cout << "\n" << alumnos[i] << "\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << "Asistencia (1 = Presente, 0 = Ausente): ";

    float asistencia;
    do {
      getline(cin, entrada);
      if (!validarEntradaNumerica(entrada, asistencia, 0, 1)) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                12); // Rojo neón
        mostrarTextoAnimado("Valor invalido. Debe ser 0 o 1.", 12);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
        cout << "Asistencia (1 = Presente, 0 = Ausente): ";
        continue;
      }
      break;
    } while (true);

    // Rellenar con ceros si faltan columnas
    int columnasNecesarias = static_cast<int>(fechas.size());
    if (i >= asistencias.size()) {
      // Si no hay fila para este alumno, crearla
      asistencias.push_back(vector<string>(columnasNecesarias, "0"));
    } else if (asistencias[i].size() < columnasNecesarias) {
      asistencias[i].resize(columnasNecesarias, "0");
    }
    // Registrar la asistencia
    asistencias[i][static_cast<int>(seleccionFecha) - 1] =
        to_string(static_cast<int>(asistencia));
  }

  ofstream archivoAsistenciasOut("asistencias.txt");
  if (!archivoAsistenciasOut.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    mostrarTextoAnimado(
        "Error al abrir el archivo de asistencias para escritura.", 12);
    esperar(1000);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return;
  }

  for (const auto &fila : asistencias) {
    for (size_t i = 0; i < fila.size(); ++i) {
      archivoAsistenciasOut << fila[i];
      if (i < fila.size() - 1) {
        archivoAsistenciasOut << ",";
      }
    }
    archivoAsistenciasOut << "\n";
  }
  archivoAsistenciasOut.close();

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  mostrarTextoAnimado("Asistencias actualizadas correctamente.", 10);
  esperar(1000);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
  cout << "\n==============================================\n";
  cout << "||   ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  cout << "Presiona ENTER para regresar al menú principal...";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  cout << "   ||\n";
  cout << "==============================================\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
  cin.sync(); // Limpia el buffer de entrada
  cin.get();  // Espera solo un ENTER
}

void gestionarFechas() {
  system("cls");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9); // Azul neón
  mostrarTextoAnimado("=== GESTIONAR FECHAS ===", 9);
  esperar(500);

  ifstream archivoFechas("fechas.txt");
  if (!archivoFechas.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de fechas.", 12);
    esperar(1000);
    animacionSalida("fechas", false, 9);
    return;
  }

  string linea;
  vector<string> fechas;
  if (getline(archivoFechas, linea)) {
    fechas = split(linea, ',');
    if (!fechas.empty() && fechas.back().empty()) {
      fechas.pop_back();
    }
  }
  archivoFechas.close();

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  cout << "\nFechas actuales:\n";
  for (size_t i = 0; i < fechas.size(); ++i) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    cout << "  - ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << fechas[i] << "\n";
    esperar(100);
  }

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9); // Azul neón
  cout << "\nOpciones:\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  cout << "  1. ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  cout << "Agregar fecha\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  cout << "  2. ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  cout << "Eliminar fecha\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  cout << "  0. ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  cout << "Volver al menu principal\n\n";

  float opcion;
  string entrada;
  do {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    cout << "Seleccione una opción: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    getline(cin, entrada);
    if (!validarEntradaNumerica(entrada, opcion, 0, 2)) {
      mostrarTextoAnimado("Opcion invalida. Intente de nuevo.", 12);
      continue;
    }
    break;
  } while (true);

  // Limpiar pantalla después de seleccionar opción
  system("cls");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9); // Azul neón
  mostrarTextoAnimado("=== GESTIONAR FECHAS ===", 9);
  esperar(500);

  switch (static_cast<int>(opcion)) {
  case 1: {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    mostrarTextoAnimado("=== AGREGAR FECHA ===", 13);
    esperar(500);

    string nuevaFecha;
    do {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              13); // Magenta neón
      cout << "\nIngrese la nueva fecha (DD/MM/AAAA): ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      getline(cin, nuevaFecha);
      if (!validarFormatoFecha(nuevaFecha)) {
        mostrarTextoAnimado("Formato de fecha invalido. Use DD/MM/AAAA.", 12);
        continue;
      }
      break;
    } while (true);

    fechas.push_back(nuevaFecha);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
    mostrarTextoAnimado("Fecha agregada correctamente.", 10);
    esperar(1000);
    break;
  }
  case 2: {
    if (fechas.empty()) {
      mostrarTextoAnimado("No hay fechas para eliminar.", 12);
      esperar(1000);
      animacionSalida("fechas", false, 9);
      return;
    }

    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("=== ELIMINAR FECHA ===", 12);
    esperar(500);

    // Mostrar fechas disponibles con marco ASCII simple
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            14); // Amarillo neón
    cout << "+-----------------------------+\n";
    cout << "|     FECHAS DISPONIBLES      |\n";
    cout << "+-----------------------------+\n";
    for (size_t i = 0; i < fechas.size(); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              14); // Amarillo neón
      cout << "| ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
      cout << i + 1 << ". ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      cout << fechas[i];
      // Rellenar espacios para alinear
      int espacios = 24 - fechas[i].length();
      for (int j = 0; j < espacios; j++)
        cout << " ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              14); // Amarillo neón
      cout << "|\n";
      esperar(100);
    }
    cout << "+-----------------------------+\n\n";

    float indice;
    do {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
      cout << "Ingrese el numero de la fecha a eliminar (1-" << fechas.size()
           << "): ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      getline(cin, entrada);
      if (!validarEntradaNumerica(entrada, indice, 1, fechas.size())) {
        mostrarTextoAnimado("Seleccion invalida. Intente de nuevo.", 12);
        continue;
      }
      break;
    } while (true);

    // Confirmar eliminación
    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("=== CONFIRMAR ELIMINACIÓN ===", 12);
    esperar(500);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            14); // Amarillo neón
    cout << "\n¿Está seguro que desea eliminar la fecha: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << fechas[static_cast<int>(indice) - 1];
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            14); // Amarillo neón
    cout << "?\n";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    cout << "\n1. Sí, eliminar\n";
    cout << "2. No, cancelar\n\n";

    float confirmacion;
    do {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
      cout << "Seleccione una opción: ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      getline(cin, entrada);
      if (!validarEntradaNumerica(entrada, confirmacion, 1, 2)) {
        mostrarTextoAnimado("Opción inválida. Intente de nuevo.", 12);
        continue;
      }
      break;
    } while (true);

    if (static_cast<int>(confirmacion) == 1) {
      fechas.erase(fechas.begin() + static_cast<int>(indice) - 1);
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              10); // Verde neón
      mostrarTextoAnimado("Fecha eliminada correctamente.", 10);
      esperar(1000);
    } else {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              14); // Amarillo neón
      mostrarTextoAnimado("Operación cancelada.", 14);
      esperar(1000);
    }
    break;
  }
  case 0:
    animacionSalida("fechas", false, 9);
    return;
  }

  ofstream archivoFechasOut("fechas.txt");
  if (!archivoFechasOut.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de fechas para escritura.",
                        12);
    esperar(1000);
    animacionSalida("fechas", false, 9);
    return;
  }

  for (size_t i = 0; i < fechas.size(); ++i) {
    archivoFechasOut << fechas[i];
    if (i < fechas.size() - 1) {
      archivoFechasOut << ",";
    }
  }
  archivoFechasOut.close();

  // Animación especial al guardar cambios
  system("cls");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9); // Azul neón
  mostrarTextoAnimado("Guardando cambios", 9);
  cout << "\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  for (int i = 0; i < 3; i++) {
    cout << ".";
    cout.flush();
    esperar(300);
  }
  cout << "\n\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  mostrarTextoAnimado("Cambios guardados correctamente.", 10);
  esperar(1000);
  animacionSalida("fechas", false, 9);
}

void ordenarAlumnosYArchivos(const vector<string> &alumnosOrdenados) {
  // Leer todos los archivos
  vector<vector<string>> asistencias;
  vector<vector<string>> calificaciones;

  // Leer asistencias
  ifstream archivoAsistenciasIn("asistencias.txt");
  string lineaAsistencia;
  while (getline(archivoAsistenciasIn, lineaAsistencia)) {
    asistencias.push_back(split(lineaAsistencia, ','));
  }
  archivoAsistenciasIn.close();

  // Leer calificaciones
  ifstream archivoCalificacionesIn("calificaciones.txt");
  string lineaCalificacion;
  while (getline(archivoCalificacionesIn, lineaCalificacion)) {
    calificaciones.push_back(split(lineaCalificacion, ','));
  }
  archivoCalificacionesIn.close();

  // Crear mapas para mantener el orden original
  map<string, vector<string>> mapaAsistencias;
  map<string, vector<string>> mapaCalificaciones;

  // Leer alumnos actuales
  vector<string> alumnosActuales;
  ifstream archivoAlumnosIn("alumnos.txt");
  string linea;
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
  ofstream archivoAlumnosOut("alumnos.txt");
  for (const auto &alumno : alumnosOrdenados) {
    archivoAlumnosOut << alumno << "\n";
  }
  archivoAlumnosOut.close();

  // Guardar asistencias ordenadas
  ofstream archivoAsistenciasOut("asistencias.txt");
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
  ofstream archivoCalificacionesOut("calificaciones.txt");
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
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); // Cian
  mostrarTextoAnimado("=== AGREGAR ALUMNO ===", 11);
  esperar(500);

  string nombre;
  do {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
    cout << "\nIngrese el nombre completo del alumno: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
    getline(cin, nombre);
    if (nombre.empty()) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
      mostrarTextoAnimado("El nombre no puede estar vacio.", 12);
      continue;
    }
    break;
  } while (true);

  ifstream archivoAlumnos("alumnos.txt");
  if (!archivoAlumnos.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
    mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
    esperar(1000);
    animacionSalida("alumnos", false, 14);
    return;
  }

  vector<string> alumnos;
  string linea;
  while (getline(archivoAlumnos, linea)) {
    alumnos.push_back(linea);
  }
  archivoAlumnos.close();

  // Verificar si el alumno ya existe
  for (const auto &alumno : alumnos) {
    if (alumno == nombre) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
      mostrarTextoAnimado("El alumno ya existe en el sistema.", 12);
      esperar(1000);
      animacionSalida("alumnos", false, 14);
      return;
    }
  }

  // Agregar el nuevo alumno
  alumnos.push_back(nombre);

  // Ordenar alumnos por apellido
  sort(alumnos.begin(), alumnos.end(), compararPorApellido);

  // Animación de guardando alumno
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
  cout << "\nGuardando alumno";
  for (int i = 0; i < 3; ++i) {
    cout << ".";
    cout.flush();
    esperar(200);
  }
  cout << "\n";
  esperar(500);

  // Guardar la lista actualizada
  ofstream archivoAlumnosOut("alumnos.txt");
  if (!archivoAlumnosOut.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
    mostrarTextoAnimado("Error al abrir el archivo de alumnos para escritura.",
                        12);
    esperar(1000);
    animacionSalida("alumnos", false, 14);
    return;
  }

  for (const auto &alumno : alumnos) {
    archivoAlumnosOut << alumno << "\n";
  }
  archivoAlumnosOut.close();

  // Actualizar archivo de asistencias
  ifstream archivoFechas("fechas.txt");
  if (!archivoFechas.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
    mostrarTextoAnimado("Error al abrir el archivo de fechas.", 12);
    esperar(1000);
    animacionSalida("alumnos", false, 14);
    return;
  }

  string lineaFechas;
  vector<string> fechas;
  if (getline(archivoFechas, lineaFechas)) {
    fechas = split(lineaFechas, ',');
    if (!fechas.empty() && fechas.back().empty()) {
      fechas.pop_back();
    }
  }
  archivoFechas.close();

  ofstream archivoAsistencias("asistencias.txt", ios::app);
  if (!archivoAsistencias.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
    mostrarTextoAnimado("Error al abrir el archivo de asistencias.", 12);
    esperar(1000);
    animacionSalida("alumnos", false, 14);
    return;
  }

  archivoAsistencias << nombre;
  for (size_t i = 0; i < fechas.size(); ++i) {
    archivoAsistencias << ",0";
  }
  archivoAsistencias << "\n";
  archivoAsistencias.close();

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
  mostrarTextoAnimado("Alumno agregado correctamente.", 10);
  esperar(1000);

  // Animación de salida personalizada para esta sección
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); // Cian
  cout << "\nSaliendo de agregar alumno";
  for (int i = 0; i < 3; ++i) {
    cout << ".";
    cout.flush();
    esperar(200);
  }
  cout << "\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); // Cian
  mostrarTextoAnimado("Regresando al menú principal...", 11);
  esperar(500);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void mostrarCuadriculaTrabajos() {
  ifstream archivoAlumnos("alumnos.txt");
  ifstream archivoTrabajos("trabajos.txt");
  ifstream archivoCalificaciones("calificaciones.txt");

  system("cls");
  mostrarTextoAnimado("=== CARGANDO LISTA DE TRABAJOS ===", 11);
  esperar(500);

  if (!archivoAlumnos.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
    esperar(1000);
    animacionSalida("trabajos", false, 13);
    return;
  }
  if (!archivoTrabajos.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de trabajos.", 12);
    esperar(1000);
    animacionSalida("trabajos", false, 13);
    return;
  }
  if (!archivoCalificaciones.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de calificaciones.", 12);
    esperar(1000);
    animacionSalida("trabajos", false, 13);
    return;
  }

  vector<string> alumnos;
  string linea;
  while (getline(archivoAlumnos, linea)) {
    alumnos.push_back(linea);
  }
  archivoAlumnos.close();

  vector<pair<string, string>> trabajosConFecha;
  string lineaTrabajos;
  if (getline(archivoTrabajos, lineaTrabajos)) {
    vector<string> trabajos = split(lineaTrabajos, ',');
    if (!trabajos.empty() && trabajos.back().empty()) {
      trabajos.pop_back();
    }
    for (const auto &trabajo : trabajos) {
      size_t open_paren = trabajo.find('(');
      size_t close_paren = trabajo.find(')');
      if (open_paren != string::npos && close_paren != string::npos &&
          close_paren > open_paren) {
        string nombre = trabajo.substr(0, open_paren);
        nombre.erase(0, nombre.find_first_not_of(" \t\n\r\f\v"));
        nombre.erase(nombre.find_last_not_of(" \t\n\r\f\v") + 1);
        string fecha =
            trabajo.substr(open_paren + 1, close_paren - open_paren - 1);
        trabajosConFecha.push_back({nombre, fecha});
      } else {
        string nombre = trabajo;
        nombre.erase(0, nombre.find_first_not_of(" \t\n\r\f\v"));
        nombre.erase(nombre.find_last_not_of(" \t\n\r\f\v") + 1);
        trabajosConFecha.push_back({nombre, ""});
      }
    }
  }
  archivoTrabajos.close();

  // Leer calificaciones y asegurar alineación exacta con trabajos
  vector<vector<string>> calificaciones;
  while (getline(archivoCalificaciones, linea)) {
    auto datos = split(linea, ',');
    // datos[0] es el nombre, lo demás son calificaciones
    vector<string> fila;
    for (size_t i = 1; i <= trabajosConFecha.size(); ++i) {
      if (i < datos.size() && !datos[i].empty())
        fila.push_back(datos[i]);
      else
        fila.push_back("0");
    }
    calificaciones.push_back(fila);
  }
  archivoCalificaciones.close();

  // Calcular el ancho máximo del nombre
  size_t maxAnchoNombre = 0;
  for (const auto &alumno : alumnos) {
    if (alumno.length() > maxAnchoNombre) {
      maxAnchoNombre = alumno.length();
    }
  }
  const int ANCHO_NOMBRE = max(30, static_cast<int>(maxAnchoNombre) + 2);
  const int ANCHO_COLUMNA_DATOS = 15;

  // Calcular cuántas columnas caben en la pantalla
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  int anchoPantalla = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  int columnasVisibles =
      (anchoPantalla - ANCHO_NOMBRE - 2) / (ANCHO_COLUMNA_DATOS + 1);

  int inicioColumnas = 0;
  bool salir = false;

  while (!salir) {
    system("cls");
    mostrarTextoAnimado("=== LISTA DE TRABAJOS ===", 14);
    esperar(300);

    // Dibujar encabezado superior
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      cout << "-";
    cout << "+";
    for (int i = 0;
         i < min(columnasVisibles,
                 static_cast<int>(trabajosConFecha.size()) - inicioColumnas);
         ++i) {
      for (int j = 0; j < ANCHO_COLUMNA_DATOS; ++j)
        cout << "-";
      cout << "+";
    }
    cout << "\n";

    // Dibujar fila de nombres de trabajos
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "|";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
    cout << left << setw(ANCHO_NOMBRE) << "Nombre";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "|";
    for (int i = 0;
         i < min(columnasVisibles,
                 static_cast<int>(trabajosConFecha.size()) - inicioColumnas);
         ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      string nombre = trabajosConFecha[inicioColumnas + i].first;
      int padding = ANCHO_COLUMNA_DATOS - nombre.length();
      int pl = padding / 2, pr = padding - pl;
      for (int k = 0; k < pl; ++k)
        cout << " ";
      cout << nombre;
      for (int k = 0; k < pr; ++k)
        cout << " ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      cout << "|";
    }
    cout << "\n";

    // Dibujar fila de fechas de trabajos
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "|";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      cout << " ";
    cout << "|";
    for (int i = 0;
         i < min(columnasVisibles,
                 static_cast<int>(trabajosConFecha.size()) - inicioColumnas);
         ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      string fecha = trabajosConFecha[inicioColumnas + i].second;
      int padding = ANCHO_COLUMNA_DATOS - fecha.length();
      int pl = padding / 2, pr = padding - pl;
      for (int k = 0; k < pl; ++k)
        cout << " ";
      cout << fecha;
      for (int k = 0; k < pr; ++k)
        cout << " ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      cout << "|";
    }
    cout << "\n";

    // Dibujar línea divisoria entre encabezado y datos
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      cout << "-";
    cout << "+";
    for (int i = 0;
         i < min(columnasVisibles,
                 static_cast<int>(trabajosConFecha.size()) - inicioColumnas);
         ++i) {
      for (int j = 0; j < ANCHO_COLUMNA_DATOS; ++j)
        cout << "-";
      cout << "+";
    }
    cout << "\n";

    // Dibujar filas de calificaciones
    for (size_t i = 0; i < alumnos.size(); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      cout << "|";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
      cout << left << setw(ANCHO_NOMBRE) << alumnos[i];
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      cout << "|";

      for (int j = 0;
           j < min(columnasVisibles,
                   static_cast<int>(trabajosConFecha.size()) - inicioColumnas);
           ++j) {
        string valor = "0";
        int colorValor = 15;
        if (i < calificaciones.size() &&
            inicioColumnas + j < calificaciones[i].size()) {
          valor = calificaciones[i][inicioColumnas + j];
          if (valor.empty())
            valor = "0";
          try {
            float calif = stof(valor);
            // Formatear a un decimal
            stringstream ss;
            ss << fixed << setprecision(1) << calif;
            valor = ss.str();
            if (calif == 0.0f)
              colorValor = 12; // Rojo para ceros
            else if (calif >= 6.0)
              colorValor = 10; // Verde
            else
              colorValor = 12; // Rojo
          } catch (...) {
            valor = "0.0";
            colorValor = 12;
          }
        } else {
          valor = "0.0";
          colorValor = 12; // Rojo para ceros por defecto
        }
        int padding = ANCHO_COLUMNA_DATOS - valor.length();
        int pl = padding / 2, pr = padding - pl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        for (int k = 0; k < pl; ++k)
          cout << " ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorValor);
        cout << valor;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        for (int k = 0; k < pr; ++k)
          cout << " ";
        cout << "|";
      }
      cout << "\n";
    }

    // Dibujar línea inferior
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "+";
    for (int i = 0; i < ANCHO_NOMBRE; ++i)
      cout << "-";
    cout << "+";
    for (int i = 0;
         i < min(columnasVisibles,
                 static_cast<int>(trabajosConFecha.size()) - inicioColumnas);
         ++i) {
      for (int j = 0; j < ANCHO_COLUMNA_DATOS; ++j)
        cout << "-";
      cout << "+";
    }
    cout << "\n";

    // Instrucciones de navegación
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
    cout << "\nFlechas <- ->: Navegar | ENTER: Regresar al Menu Principal\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    // Manejo de teclas
    int tecla = _getch();
    if (tecla == 224) { // Tecla de flecha
      tecla = _getch();
      if (tecla == 75 && inicioColumnas > 0) { // Flecha izquierda
        inicioColumnas--;
      } else if (tecla == 77 && inicioColumnas + columnasVisibles <
                                    trabajosConFecha.size()) { // Flecha derecha
        inicioColumnas++;
      }
    } else if (tecla == 13) { // ENTER
      salir = true;
    }
  }
}

void calcularPromedioParcial() {
  system("cls");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("=== CALCULO DE PROMEDIO PARCIAL ===", 13);
  mostrarBarraProgreso(700);
  esperar(300);

  ifstream archivoAlumnos("alumnos.txt");
  ifstream archivoAsistencias("asistencias.txt");
  ifstream archivoCalificaciones("calificaciones.txt");
  ifstream archivoCriterios("criterios.txt");

  if (!archivoAlumnos.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
    esperar(1000);
    animacionSalida("promedios", false, 11);
    return;
  }
  if (!archivoAsistencias.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    mostrarTextoAnimado("Error al abrir el archivo de asistencias.", 12);
    esperar(1000);
    animacionSalida("promedios", false, 11);
    return;
  }
  if (!archivoCalificaciones.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    mostrarTextoAnimado("Error al abrir el archivo de calificaciones.", 12);
    esperar(1000);
    animacionSalida("promedios", false, 11);
    return;
  }
  if (!archivoCriterios.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
    mostrarTextoAnimado("Error al abrir el archivo de criterios de evaluacion. "
                        "Asegurate de haberlos configurado.",
                        14);
    esperar(1500);
    animacionSalida("promedios", false, 11);
    return;
  }

  vector<string> alumnos;
  string linea;
  while (getline(archivoAlumnos, linea)) {
    alumnos.push_back(linea);
  }
  archivoAlumnos.close();

  vector<vector<string>> asistencias;
  while (getline(archivoAsistencias, linea)) {
    auto datos = split(linea, ',');
    asistencias.push_back(datos);
  }
  archivoAsistencias.close();

  vector<vector<string>> calificaciones;
  while (getline(archivoCalificaciones, linea)) {
    auto datos = split(linea, ',');
    calificaciones.push_back(datos);
  }
  archivoCalificaciones.close();

  // Leer criterios de evaluacion
  float pesoAsistencias = 0, pesoTrabajos = 0, pesoExamen = 0;
  string lineaCriterios;
  if (getline(archivoCriterios, lineaCriterios)) {
    auto criterios = split(lineaCriterios, ',');
    if (criterios.size() >= 3) {
      try {
        pesoAsistencias = stof(criterios[0]);
        pesoTrabajos = stof(criterios[1]);
        pesoExamen = stof(criterios[2]);
      } catch (...) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        mostrarTextoAnimado(
            "Error al leer los pesos de los criterios de evaluacion.", 12);
        esperar(1000);
        animacionSalida("promedios", false, 11);
        return;
      }
    } else {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
      mostrarTextoAnimado("Formato incorrecto en criterios.txt.", 12);
      esperar(1000);
      animacionSalida("promedios", false, 11);
      return;
    }
  }
  archivoCriterios.close();

  // Encontrar el indice del examen en trabajos.txt (asumimos que el ultimo
  // trabajo es el examen)
  int indiceExamen = -1;
  ifstream archivoTrabajosIn("trabajos.txt");
  string lineaTrabajos;
  if (getline(archivoTrabajosIn, lineaTrabajos)) {
    auto trabajos = split(lineaTrabajos, ',');
    if (!trabajos.empty()) {
      indiceExamen = trabajos.size() - 1; // El último elemento es el examen
    }
  }
  archivoTrabajosIn.close();

  if (indiceExamen == -1) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    mostrarTextoAnimado("No se encontro ningun trabajo/examen. Asegurate de "
                        "haber agregado uno.",
                        14);
    esperar(1500);
    animacionSalida("promedios", false, 11);
    return;
  }

  cout << "\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  mostrarTextoAnimado("Promedios Parciales (Escala 0-10):", 11);
  esperar(300);

  for (size_t i = 0; i < alumnos.size(); ++i) {
    float promedioParcial = 0;
    string nombreAlumno = alumnos[i];

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
            calificacionExamen = stof(calificaciones[i][j]);
          } catch (...) {
            calificacionExamen = 0;
          }
        } else if (static_cast<int>(j) <
                   indiceExamen) { // Considerar solo trabajos ANTES del examen
          try {
            sumaCalificacionesTrabajos += stof(calificaciones[i][j]);
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

    promedioParcial *= 10.0; // Mostrar en escala 0-10

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << nombreAlumno << ": ";
    if (promedioParcial >= 6.0) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              10); // Verde neón
    } else {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
    }
    cout << fixed << setprecision(1) << promedioParcial << "\n";
    esperar(100);
  }

  cout << "\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  cout << "==============================================\n";
  cout << "||   ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  cout << "Presiona ENTER para continuar...";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  cout << "   ||\n";
  cout << "==============================================\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
  cin.sync();
  cin.get();

  animacionSalida("promedios", false, 11);
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
  mostrarTextoAnimado("=== MODIFICAR CALIFICACIONES ===", 14);
  esperar(500);

  // Verificar archivos necesarios
  if (!archivoExiste("alumnos.txt") || !archivoExiste("trabajos.txt") ||
      !archivoExiste("calificaciones.txt")) {
    mostrarTextoAnimado("Error: Faltan archivos necesarios.", 12);
    esperar(1000);
    return;
  }

  // Leer alumnos
  vector<string> alumnos;
  ifstream archivoAlumnos("alumnos.txt");
  string linea;
  while (getline(archivoAlumnos, linea)) {
    if (!linea.empty()) {
      alumnos.push_back(linea);
    }
  }
  archivoAlumnos.close();

  if (alumnos.empty()) {
    mostrarTextoAnimado("No hay alumnos registrados.", 12);
    esperar(1000);
    animacionSalida("trabajos", false, 13);
    return;
  }

  // Leer trabajos
  vector<pair<string, string>> trabajosConFecha;
  ifstream archivoTrabajos("trabajos.txt");
  if (getline(archivoTrabajos, linea)) {
    vector<string> trabajos = split(linea, ',');
    for (const auto &trabajo : trabajos) {
      if (trabajo.empty())
        continue;
      size_t posFecha = trabajo.find(" (");
      if (posFecha != string::npos) {
        string nombre = trabajo.substr(0, posFecha);
        string fecha =
            trabajo.substr(posFecha + 2, trabajo.length() - posFecha - 3);
        trabajosConFecha.push_back({nombre, fecha});
      } else {
        trabajosConFecha.push_back({trabajo, ""});
      }
    }
  }
  archivoTrabajos.close();

  if (trabajosConFecha.empty()) {
    mostrarTextoAnimado("No hay trabajos registrados.", 12);
    esperar(1000);
    animacionSalida("trabajos", false, 13);
    return;
  }

  // Leer calificaciones (rellenando con '0' si faltan columnas)
  vector<vector<string>> calificaciones(
      alumnos.size(), vector<string>(trabajosConFecha.size(), "0"));
  ifstream archivoCalificaciones("calificaciones.txt");
  while (getline(archivoCalificaciones, linea)) {
    if (linea.empty())
      continue;
    auto datos = split(linea, ',');
    if (datos.size() >= 1) {
      string nombreAlumno = datos[0];
      auto it = find(alumnos.begin(), alumnos.end(), nombreAlumno);
      if (it != alumnos.end()) {
        size_t indiceAlumno = distance(alumnos.begin(), it);
        // CORREGIDO: i < trabajosConFecha.size() y datos[i+1] corresponde a
        // columna i
        for (size_t i = 0; i < trabajosConFecha.size(); ++i) {
          if ((i + 1) < datos.size() && !datos[i + 1].empty()) {
            calificaciones[indiceAlumno][i] = datos[i + 1];
          } else {
            calificaciones[indiceAlumno][i] = "0";
          }
        }
      }
    }
  }
  archivoCalificaciones.close();

  // Seleccionar alumno
  cout << "\nSeleccione el alumno:\n";
  for (size_t i = 0; i < alumnos.size(); ++i) {
    cout << i + 1 << ". " << alumnos[i] << "\n";
  }

  float seleccionAlumno;
  string entrada;
  cin.sync(); // Limpia el buffer antes de pedir el número
  do {
    cout << "\nIngrese el numero del alumno (o 0 para regresar): ";
    getline(cin, entrada);
    if (!validarEntradaNumerica(entrada, seleccionAlumno, 0, alumnos.size())) {
      mostrarTextoAnimado("Seleccion invalida. Intente de nuevo.", 12);
      continue;
    }
    break;
  } while (true);

  if (seleccionAlumno == 0) {
    animacionSalida("trabajos", false, 13);
    return;
  }

  size_t indiceAlumno = static_cast<size_t>(seleccionAlumno) - 1;

  // Mostrar trabajos y calificaciones del alumno
  system("cls");
  mostrarTextoAnimado("=== TRABAJOS Y CALIFICACIONES ===", 14);
  cout << "\nAlumno: " << alumnos[indiceAlumno] << "\n\n";

  // Dibujar encabezado
  cout << string(80, '=') << "\n";
  cout << "| " << setw(4) << "No." << " | " << setw(40) << "Trabajo" << " | "
       << setw(10) << "Calificacion" << " |\n";
  cout << string(80, '=') << "\n";

  // Mostrar trabajos y calificaciones
  for (size_t i = 0; i < trabajosConFecha.size(); ++i) {
    cout << "| " << setw(4) << i + 1 << " | " << setw(40)
         << trabajosConFecha[i].first;
    if (!trabajosConFecha[i].second.empty()) {
      cout << " (" << trabajosConFecha[i].second << ")";
    }
    cout << " | " << setw(10) << calificaciones[indiceAlumno][i] << " |\n";
  }
  cout << string(80, '=') << "\n";

  // Seleccionar trabajo para modificar
  float seleccionTrabajo;
  do {
    cout << "\nIngrese el numero del trabajo a modificar (o 0 para regresar): ";
    getline(cin, entrada);
    if (!validarEntradaNumerica(entrada, seleccionTrabajo, 0,
                                trabajosConFecha.size())) {
      mostrarTextoAnimado("Seleccion invalida. Intente de nuevo.", 12);
      continue;
    }
    break;
  } while (true);

  if (seleccionTrabajo == 0) {
    animacionSalida("trabajos", false, 13);
    return;
  }

  // CORREGIDO: El índice de trabajo es seleccionTrabajo - 1
  size_t indiceTrabajo = static_cast<size_t>(seleccionTrabajo) - 1;
  if (indiceTrabajo >= trabajosConFecha.size()) {
    mostrarTextoAnimado("Trabajo inválido.", 12);
    esperar(1000);
    return;
  }

  // Ingresar nueva calificación
  float calificacion;
  do {
    cout << "\nIngrese la nueva calificacion (0-10): ";
    getline(cin, entrada);
    if (!validarEntradaNumerica(entrada, calificacion, 0, 10)) {
      mostrarTextoAnimado(
          "Calificacion invalida. Debe ser un numero entre 0 y 10.", 12);
      continue;
    }
    break;
  } while (true);

  // Actualizar calificación en la columna correcta
  stringstream ss;
  ss << fixed << setprecision(1) << calificacion;
  calificaciones[indiceAlumno][indiceTrabajo] = ss.str();

  // Guardar cambios
  ofstream archivoCalificacionesOut("calificaciones.txt");
  if (!archivoCalificacionesOut.is_open()) {
    mostrarTextoAnimado(
        "Error al abrir el archivo de calificaciones para escritura.", 12);
    esperar(1000);
    return;
  }

  for (size_t i = 0; i < alumnos.size(); ++i) {
    archivoCalificacionesOut << alumnos[i];
    for (size_t j = 0; j < trabajosConFecha.size(); ++j) {
      archivoCalificacionesOut << "," << calificaciones[i][j];
    }
    archivoCalificacionesOut << "\n";
  }
  archivoCalificacionesOut.close();

  mostrarTextoAnimado("Calificacion actualizada correctamente.", 10);
  esperar(1000);
}

void agregarTrabajo() {
  system("cls");
  // Animación de carga inicial
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("=== AGREGAR TRABAJO ===", 13);
  mostrarBarraProgreso(800);
  esperar(300);

  string nombreTrabajo;
  do {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << "\nIngrese el nombre del trabajo: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            14); // Amarillo neón
    getline(cin, nombreTrabajo);
    if (nombreTrabajo.empty()) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
      mostrarTextoAnimado("El nombre no puede estar vacio.", 12);
      continue;
    }
    break;
  } while (true);

  string fecha;
  do {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << "\nIngrese la fecha del trabajo (DD/MM/AAAA) o deje en blanco: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            14); // Amarillo neón
    getline(cin, fecha);
    if (!fecha.empty() && !validarFormatoFecha(fecha)) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
      mostrarTextoAnimado("Formato de fecha invalido. Use DD/MM/AAAA.", 12);
      continue;
    }
    break;
  } while (true);

  ifstream archivoTrabajosIn("trabajos.txt");
  if (!archivoTrabajosIn.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("Error al abrir el archivo de trabajos.", 12);
    esperar(1000);
    return;
  }

  vector<string> trabajos;

  string linea;
  if (getline(archivoTrabajosIn, linea)) {
    trabajos = split(linea, ',');
    if (!trabajos.empty() && trabajos.back().empty()) {
      trabajos.pop_back();
    }
  }
  archivoTrabajosIn.close();

  // Verificar si el trabajo ya existe
  string trabajoCompleto =
      fecha.empty() ? nombreTrabajo : nombreTrabajo + " (" + fecha + ")";
  for (const auto &trabajo : trabajos) {
    if (trabajo == trabajoCompleto) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
      mostrarTextoAnimado("El trabajo ya existe en el sistema.", 12);
      esperar(1000);
      return;
    }
  }

  // Animación llamativa de guardado
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("Guardando trabajo...", 13);
  mostrarBarraProgreso(900);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

  // Agregar el nuevo trabajo
  trabajos.push_back(trabajoCompleto);

  // Guardar la lista actualizada
  ofstream archivoTrabajosOut("trabajos.txt");
  if (!archivoTrabajosOut.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("Error al abrir el archivo de trabajos para escritura.",
                        12);
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
  ifstream archivoAlumnos("alumnos.txt");
  if (!archivoAlumnos.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
    esperar(1000);
    return;
  }

  vector<string> alumnos;
  while (getline(archivoAlumnos, linea)) {
    alumnos.push_back(linea);
  }
  archivoAlumnos.close();

  ofstream archivoCalificaciones("calificaciones.txt", ios::app);
  if (!archivoCalificaciones.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("Error al abrir el archivo de calificaciones.", 12);
    esperar(1000);
    return;
  }

  for (const auto &alumno : alumnos) {
    archivoCalificaciones << alumno << "," << trabajoCompleto << ",0\n";
  }
  archivoCalificaciones.close();

  // Animación de éxito
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  mostrarTextoAnimado("¡Trabajo agregado correctamente!", 10);
  mostrarBarraProgreso(600);
  esperar(1000);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void gestionarTrabajos() {
  system("cls");
  // Animación de carga inicial
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("=== GESTION DE TRABAJOS ===", 13);
  mostrarBarraProgreso(800);
  esperar(300);

  while (true) {
    system("cls");
    // Título con efecto de brillo
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    mostrarTextoAnimado("=== GESTION DE TRABAJOS ===", 13);

    // Opciones con colores neón
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    mostrarTextoAnimado("\n1. Agregar trabajo", 11);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("2. Eliminar trabajo", 12);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            14); // Amarillo neón
    mostrarTextoAnimado("0. Regresar", 14);

    int opcion;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    cout << "\nOpción: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante

    cin >> opcion;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    switch (opcion) {
    case 1:
      agregarTrabajo();
      break;
    case 2:
      eliminarTrabajo();
      break;
    case 0:
      // Animación de salida personalizada
      system("cls");
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              13); // Magenta neón
      mostrarTextoAnimado("Saliendo de Gestion de Trabajos", 13);
      mostrarBarraProgreso(600);
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      cout << "\nRegresando al menu principal";
      for (int i = 0; i < 3; ++i) {
        cout << ".";
        cout.flush();
        esperar(200);
      }
      cout << "\n";
      esperar(500);
      return;
    default:
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
      mostrarTextoAnimado("Opción inválida.", 12);
      esperar(1000);
    }
  }
}

void eliminarTrabajo() {
  system("cls");
  // Animación de carga inicial
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("=== ELIMINAR TRABAJO ===", 13);
  mostrarBarraProgreso(800);
  esperar(300);

  // Leer trabajos
  vector<string> trabajos;
  ifstream archivoTrabajos("trabajos.txt");
  string linea;
  if (getline(archivoTrabajos, linea)) {
    trabajos = split(linea, ',');
    if (!trabajos.empty() && trabajos.back().empty()) {
      trabajos.pop_back();
    }
  }
  archivoTrabajos.close();

  if (trabajos.empty()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("No hay trabajos registrados.", 12);
    esperar(1000);
    animacionSalida("trabajos", false, 13);
    return;
  }

  // Mostrar lista de trabajos con colores neón
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  cout << "\nLista de trabajos:\n";
  for (size_t i = 0; i < trabajos.size(); ++i) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    cout << i + 1 << ". ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            14); // Amarillo neón
    cout << trabajos[i] << "\n";
  }
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
  cout << "0. Cancelar\n";

  float seleccion;
  string entrada;
  do {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << "\nIngrese el numero del trabajo a eliminar (o 0 para cancelar): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            14); // Amarillo neón
    getline(cin, entrada);
    if (!validarEntradaNumerica(entrada, seleccion, 0, trabajos.size())) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
      mostrarTextoAnimado("Seleccion invalida. Intente de nuevo.", 12);
      continue;
    }
    break;
  } while (true);

  if (seleccion == 0) {
    animacionSalida("trabajos", false, 13);
    return;
  }

  size_t indiceEliminar = static_cast<size_t>(seleccion) - 1;
  string trabajoEliminado = trabajos[indiceEliminar];

  // Animación llamativa de eliminado
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
  mostrarTextoAnimado("Eliminando trabajo...", 12);
  mostrarBarraProgreso(900);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

  // Eliminar el trabajo de la lista
  trabajos.erase(trabajos.begin() + indiceEliminar);

  // Guardar la lista actualizada de trabajos
  ofstream archivoTrabajosOut("trabajos.txt");
  for (size_t i = 0; i < trabajos.size(); ++i) {
    archivoTrabajosOut << trabajos[i];
    if (i < trabajos.size() - 1) {
      archivoTrabajosOut << ",";
    }
  }
  archivoTrabajosOut.close();

  // Actualizar calificaciones: eliminar la columna correspondiente
  vector<vector<string>> calificaciones;
  ifstream archivoCalificaciones("calificaciones.txt");
  while (getline(archivoCalificaciones, linea)) {
    if (!linea.empty())
      calificaciones.push_back(split(linea, ','));
  }
  archivoCalificaciones.close();

  for (auto &fila : calificaciones) {
    // fila[0] es el nombre del alumno, las calificaciones empiezan en 1
    if (fila.size() > indiceEliminar + 1) {
      fila.erase(fila.begin() + indiceEliminar + 1);
    }
  }

  ofstream archivoCalificacionesOut("calificaciones.txt");
  for (const auto &fila : calificaciones) {
    for (size_t i = 0; i < fila.size(); ++i) {
      archivoCalificacionesOut << fila[i];
      if (i < fila.size() - 1)
        archivoCalificacionesOut << ",";
    }
    archivoCalificacionesOut << "\n";
  }
  archivoCalificacionesOut.close();

  // Animación de éxito
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  mostrarTextoAnimado("¡Trabajo eliminado correctamente!", 10);
  mostrarBarraProgreso(600);
  esperar(1000);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void gestionarCriteriosEvaluacion() {
  system("cls");
  // Animación de carga inicial
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("=== GESTION DE CRITERIOS DE EVALUACION ===", 13);
  mostrarBarraProgreso(800);
  esperar(300);

  while (true) {
    system("cls");
    // Título con efecto de brillo
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    mostrarTextoAnimado("=== GESTION DE CRITERIOS DE EVALUACION ===", 13);
    esperar(300);

    bool criteriosExisten = existenCriteriosEvaluacion();

    if (criteriosExisten) {
      mostrarTablaCriterios();
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      cout << "Selecciona una opcion:\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              14); // Amarillo neón
      cout << "1. Modificar criterios\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
      cout << "0. Regresar\n";
    } else {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      cout << "Selecciona una opcion:\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              14); // Amarillo neón
      cout << "1. Agregar criterios\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
      cout << "0. Regresar\n";
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    cout << "\nOpcion: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            14); // Amarillo neón

    int opcion;
    cin >> opcion;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (criteriosExisten) {
      switch (opcion) {
      case 1:
        modificarCriteriosEvaluacion();
        break;
      case 0:
        // Animación de salida personalizada
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                13); // Magenta neón
        mostrarTextoAnimado("Saliendo de Gestion de Criterios", 13);
        mostrarBarraProgreso(600);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                11); // Azul brillante
        cout << "\nRegresando al menu principal";
        for (int i = 0; i < 3; ++i) {
          cout << ".";
          cout.flush();
          esperar(200);
        }
        cout << "\n";
        esperar(500);
        return;
      default:
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                12); // Rojo neón
        mostrarTextoAnimado("Opcion invalida.", 12);
        esperar(1000);
        break;
      }
    } else {
      switch (opcion) {
      case 1:
        agregarCriteriosEvaluacion();
        break;
      case 0:
        // Animación de salida personalizada
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                13); // Magenta neón
        mostrarTextoAnimado("Saliendo de Gestion de Criterios", 13);
        mostrarBarraProgreso(600);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                11); // Azul brillante
        cout << "\nRegresando al menu principal";
        for (int i = 0; i < 3; ++i) {
          cout << ".";
          cout.flush();
          esperar(200);
        }
        cout << "\n";
        esperar(500);
        return;
      default:
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                12); // Rojo neón
        mostrarTextoAnimado("Opcion invalida.", 12);
        esperar(1000);
        break;
      }
    }
  }
}

void modificarCalificacionAlumno() {
  system("cls");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("=== MODIFICAR CALIFICACIÓN DE ALUMNO ===", 13);
  mostrarBarraProgreso(700);
  esperar(300);

  if (!existenCriteriosEvaluacion()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
    mostrarTextoAnimado("No hay criterios de evaluación definidos.", 12);
    esperar(1000);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta
    mostrarTextoAnimado("Saliendo de Modificar Calificación...", 13);
    mostrarBarraProgreso(600);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    mostrarTextoAnimado("Regresando al menú principal...", 11);
    esperar(600);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return;
  }

  ifstream archivoAlumnos("alumnos.txt");
  if (!archivoAlumnos.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
    mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
    esperar(1000);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta
    mostrarTextoAnimado("Saliendo de Modificar Calificación...", 13);
    mostrarBarraProgreso(600);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    mostrarTextoAnimado("Regresando al menú principal...", 11);
    esperar(600);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return;
  }

  vector<string> alumnos;
  string linea;
  while (getline(archivoAlumnos, linea)) {
    alumnos.push_back(linea);
  }
  archivoAlumnos.close();

  if (alumnos.empty()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
    mostrarTextoAnimado("No hay alumnos registrados.", 12);
    esperar(1000);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta
    mostrarTextoAnimado("Saliendo de Modificar Calificación...", 13);
    mostrarBarraProgreso(600);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    mostrarTextoAnimado("Regresando al menú principal...", 11);
    esperar(600);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return;
  }

  // Selección de alumno
  while (true) {
    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    mostrarTextoAnimado("=== MODIFICAR CALIFICACIÓN DE ALUMNO ===", 13);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << "\nSeleccione el alumno:\n\n";
    for (size_t i = 0; i < alumnos.size(); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              10); // Verde neón para el número
      cout << i + 1 << ". ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante para el texto
      cout << alumnos[i] << "\n";
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
    cout << "0. Cancelar y regresar\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << "\nOpción: ";
    string entrada;
    float seleccion;
    getline(cin, entrada);
    if (!validarEntradaNumerica(entrada, seleccion, 0, alumnos.size())) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
      mostrarTextoAnimado("Selección inválida. Intente de nuevo.", 12);
      esperar(700);
      continue;
    }
    if (seleccion == 0) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta
      mostrarTextoAnimado("Saliendo de Modificar Calificación...", 13);
      mostrarBarraProgreso(600);
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      mostrarTextoAnimado("Regresando al menú principal...", 11);
      esperar(600);
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      return;
    }
    string alumnoSeleccionado = alumnos[static_cast<int>(seleccion) - 1];
    // Animación de transición
    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    mostrarTextoAnimado(
        "=== MODIFICAR CALIFICACIÓN DE: [" + alumnoSeleccionado + "] ===", 13);
    mostrarBarraProgreso(500);
    esperar(300);

    // Mostrar criterios
    mostrarTablaCriterios();
    ifstream archivoCriterios("criterios.txt");
    if (!archivoCriterios.is_open()) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
      mostrarTextoAnimado("Error al abrir el archivo de criterios.", 12);
      esperar(1000);
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta
      mostrarTextoAnimado("Saliendo de Modificar Calificación...", 13);
      mostrarBarraProgreso(600);
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      mostrarTextoAnimado("Regresando al menú principal...", 11);
      esperar(600);
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
      return;
    }
    vector<string> criterios;
    string lineaCriterio;
    while (getline(archivoCriterios, lineaCriterio)) {
      criterios.push_back(lineaCriterio);
    }
    archivoCriterios.close();

    // Selección de criterio
    while (true) {
      system("cls");
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              13); // Magenta neón
      mostrarTextoAnimado("=== MODIFICAR CALIFICACIÓN DE: [" +
                              alumnoSeleccionado + "] ===",
                          13);
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      cout << "\nSeleccione el criterio a modificar:\n\n";
      for (size_t i = 0; i < criterios.size(); ++i) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                10); // Verde neón para el número
        cout << i + 1 << ". ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                11); // Azul brillante para el texto
        cout << criterios[i] << "\n";
      }
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
      cout << "0. Cancelar y regresar\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      cout << "\nOpción: ";
      string entradaCriterio;
      float criterioSeleccionado;
      getline(cin, entradaCriterio);
      if (!validarEntradaNumerica(entradaCriterio, criterioSeleccionado, 0,
                                  criterios.size())) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
        mostrarTextoAnimado("Selección inválida. Intente de nuevo.", 12);
        esperar(700);
        continue;
      }
      if (criterioSeleccionado == 0) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta
        mostrarTextoAnimado("Saliendo de Modificar Calificación...", 13);
        mostrarBarraProgreso(600);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                11); // Azul brillante
        mostrarTextoAnimado("Regresando al menú principal...", 11);
        esperar(600);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        return;
      }
      string criterioElegido =
          criterios[static_cast<int>(criterioSeleccionado) - 1];
      // Animación de transición
      system("cls");
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              13); // Magenta neón
      mostrarTextoAnimado("=== MODIFICAR CALIFICACIÓN DE: [" +
                              alumnoSeleccionado + "] ===",
                          13);
      mostrarBarraProgreso(500);
      esperar(300);

      // Ingreso de calificación
      while (true) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                11); // Azul brillante
        cout << "\nIngrese la nueva calificación (0-10): ";
        string entradaCalif;
        float nuevaCalificacion;
        getline(cin, entradaCalif);
        if (!validarEntradaNumerica(entradaCalif, nuevaCalificacion, 0, 10)) {
          SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
          mostrarTextoAnimado(
              "Calificación inválida. Debe ser un número entre 0 y 10.", 12);
          esperar(700);
          continue;
        }
        // Animación de guardado
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
        mostrarTextoAnimado("Guardando calificación...", 10);
        mostrarBarraProgreso(900);
        esperar(300);

        // Guardar calificación
        ifstream archivoCalificaciones("calificaciones.txt");
        vector<string> calificaciones;
        string lineaCalif;
        while (getline(archivoCalificaciones, lineaCalif)) {
          calificaciones.push_back(lineaCalif);
        }
        archivoCalificaciones.close();

        bool encontrado = false;
        for (size_t i = 0; i < calificaciones.size(); ++i) {
          auto datos = split(calificaciones[i], ',');
          if (datos.size() >= 3 && datos[0] == alumnoSeleccionado &&
              datos[1] == criterioElegido) {
            stringstream ss;
            ss << alumnoSeleccionado << "," << criterioElegido << ","
               << nuevaCalificacion;
            calificaciones[i] = ss.str();
            encontrado = true;
            break;
          }
        }
        if (!encontrado) {
          stringstream ss;
          ss << alumnoSeleccionado << "," << criterioElegido << ","
             << nuevaCalificacion;
          calificaciones.push_back(ss.str());
        }
        ofstream archivoCalificacionesOut("calificaciones.txt");
        if (!archivoCalificacionesOut.is_open()) {
          SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
          mostrarTextoAnimado(
              "Error al abrir el archivo de calificaciones para escritura.",
              12);
          esperar(1000);
          SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                  13); // Magenta
          mostrarTextoAnimado("Saliendo de Modificar Calificación...", 13);
          mostrarBarraProgreso(600);
          SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                  11); // Azul brillante
          mostrarTextoAnimado("Regresando al menú principal...", 11);
          esperar(600);
          SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
          return;
        }
        for (const auto &cal : calificaciones) {
          archivoCalificacionesOut << cal << "\n";
        }
        archivoCalificacionesOut.close();
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
        mostrarTextoAnimado("¡Calificación actualizada correctamente!", 10);
        mostrarBarraProgreso(700);
        esperar(700);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        return;
      }
    }
  }
}

void mostrarTablaCriterios() {
  if (!existenCriteriosEvaluacion()) {
    return;
  }

  ifstream archivoIn("criterios.txt");
  string linea;
  getline(archivoIn, linea);
  archivoIn.close();

  auto valores = split(linea, ',');
  float valorAsistencias = stof(valores[0]);
  float valorTrabajos = stof(valores[1]);
  float valorExamen = stof(valores[2]);

  // --- Estilo clásico, bien alineado y centrado ---
  const int anchoTabla = 38;
  string titulo = "CRITERIOS DE EVALUACIÓN";
  int espaciosTitulo = (anchoTabla - titulo.length()) / 2;

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  cout << "\n";
  cout << "+" << string(anchoTabla, '-') << "+\n";
  cout << "|" << string(espaciosTitulo, ' ') << titulo
       << string(anchoTabla - espaciosTitulo - titulo.length(), ' ') << "|\n";
  cout << "+" << string(anchoTabla, '-') << "+\n";

  // Asistencias
  cout << "|  ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  cout << "● Asistencias";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
  cout << string(15 - 11, ' ');
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
  cout << right << setw(7) << fixed << setprecision(1) << valorAsistencias
       << "%  |\n";

  // Trabajos
  cout << "|  ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo neón
  cout << "● Trabajos";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
  cout << string(15 - 8, ' ');
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
  cout << right << setw(7) << fixed << setprecision(1) << valorTrabajos
       << "%  |\n";

  // Examen
  cout << "|  ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
  cout << "● Examen";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
  cout << string(15 - 6, ' ');
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
  cout << right << setw(7) << fixed << setprecision(1) << valorExamen
       << "%  |\n";

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
  cout << "+" << string(anchoTabla, '-') << "+\n\n";
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

      cout << "Selecciona una opcion:\n";

      cout << "1. Agregar Criterios de Evaluacion\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);

      cout << "0. Regresar al Menu Principal\n";
    } else {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);

      cout << "Selecciona una opcion:\n";

      cout << "1. Modificar Criterios de Evaluacion\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);

      cout << "0. Regresar al Menu Principal\n";
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    cout << "\nSelecciona una opcion: ";

    cin >> opcion;

    cin.ignore();

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
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("=== AGREGAR CRITERIOS DE EVALUACION ===", 13);
  mostrarBarraProgreso(800);
  esperar(300);

  if (existenCriteriosEvaluacion()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("Ya existen criterios de evaluación.", 12);
    mostrarTextoAnimado("Use la opción de modificar para cambiar los valores.",
                        12);
    esperar(2000);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return;
  }

  float valorAsistencias, valorTrabajos, valorExamen;
  bool valoresValidos = false;

  do {
    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    mostrarTextoAnimado("=== AGREGAR CRITERIOS DE EVALUACION ===", 13);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    mostrarTextoAnimado("\nIngrese los porcentajes (deben sumar 100%):", 11);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << "\nPorcentaje para Asistencias (0-100): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cin >> valorAsistencias;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << "Porcentaje para Trabajos (0-100): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cin >> valorTrabajos;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << "Porcentaje para Examen (0-100): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cin >> valorExamen;

    float suma = valorAsistencias + valorTrabajos + valorExamen;
    if (abs(suma - 100.0f) < 0.01f) {
      valoresValidos = true;
    } else {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
      mostrarTextoAnimado("\nLos porcentajes deben sumar 100%.", 12);
      mostrarTextoAnimado("Suma actual: " + to_string(suma) + "%", 12);
      esperar(2000);
    }
  } while (!valoresValidos);

  // Animación llamativa de guardado
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("Guardando criterios...", 13);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo neón
  mostrarBarraProgreso(900);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

  ofstream archivoCriterios("criterios.txt");
  if (!archivoCriterios.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("Error al guardar los criterios.", 12);
    esperar(1000);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return;
  }

  archivoCriterios << valorAsistencias << "," << valorTrabajos << ","
                   << valorExamen;
  archivoCriterios.close();

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  mostrarTextoAnimado("¡Criterios guardados correctamente!", 10);
  esperar(1000);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void modificarCriteriosEvaluacion() {
  system("cls");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("=== MODIFICAR CRITERIOS DE EVALUACION ===", 13);
  mostrarBarraProgreso(800);
  esperar(300);

  if (!existenCriteriosEvaluacion()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("No hay criterios de evaluacion para modificar.", 12);
    mostrarTextoAnimado("Use la opcion de agregar para crear nuevos criterios.",
                        12);
    esperar(2000);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return;
  }

  ifstream archivoCriterios("criterios.txt");
  if (!archivoCriterios.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("Error al leer los criterios actuales.", 12);
    esperar(1000);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return;
  }

  string linea;
  getline(archivoCriterios, linea);
  archivoCriterios.close();

  auto valores = split(linea, ',');
  float valorAsistencias = stof(valores[0]);
  float valorTrabajos = stof(valores[1]);
  float valorExamen = stof(valores[2]);

  bool valoresValidos = false;
  do {
    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    mostrarTextoAnimado("=== MODIFICAR CRITERIOS DE EVALUACION ===", 13);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    mostrarTextoAnimado("\nValores actuales:", 11);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << "Asistencias: " << fixed << setprecision(1) << valorAsistencias
         << "%\n";
    cout << "Trabajos: " << fixed << setprecision(1) << valorTrabajos << "%\n";
    cout << "Examen: " << fixed << setprecision(1) << valorExamen << "%\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    mostrarTextoAnimado("\nIngrese los nuevos porcentajes (deben sumar 100%):",
                        11);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << "\nPorcentaje para Asistencias (0-100): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cin >> valorAsistencias;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << "Porcentaje para Trabajos (0-100): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cin >> valorTrabajos;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << "Porcentaje para Examen (0-100): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cin >> valorExamen;

    float suma = valorAsistencias + valorTrabajos + valorExamen;
    if (abs(suma - 100.0f) < 0.01f) {
      valoresValidos = true;
    } else {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
      mostrarTextoAnimado("\nLos porcentajes deben sumar 100%.", 12);
      mostrarTextoAnimado("Suma actual: " + to_string(suma) + "%", 12);
      esperar(2000);
    }
  } while (!valoresValidos);

  // Animación llamativa de guardado
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("Guardando criterios...", 13);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo neón
  mostrarBarraProgreso(900);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

  ofstream archivoCriteriosOut("criterios.txt");
  if (!archivoCriteriosOut.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("Error al guardar los criterios.", 12);
    esperar(1000);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    return;
  }

  archivoCriteriosOut << valorAsistencias << "," << valorTrabajos << ","
                      << valorExamen;
  archivoCriteriosOut.close();

  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  mostrarTextoAnimado("¡Criterios actualizados correctamente!", 10);
  esperar(1000);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

bool existenCriteriosEvaluacion() {

  ifstream archivoCriterios("criterios.txt");
  if (!archivoCriterios.is_open()) {
    mostrarTextoAnimado("Error al leer los criterios.", 12);
    esperar(1000);
    return false;
  }

  string linea;
  getline(archivoCriterios, linea);
  archivoCriterios.close();

  auto valores = split(linea, ',');
  if (valores.size() == 3) {
    try {

      stof(valores[0]);

      stof(valores[1]);

      stof(valores[2]);
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

  ifstream archivoCriterios("criterios.txt");
  if (!archivoCriterios.is_open()) {
    mostrarTextoAnimado("Error al leer los criterios.", 12);
    esperar(1000);
    return;
  }

  string linea;
  getline(archivoCriterios, linea);
  archivoCriterios.close();

  auto valores = split(linea, ',');
  if (valores.size() == 3) {
    try {
      float valorAsistencias = stof(valores[0]);
      float valorTrabajos = stof(valores[1]);
      float valorExamen = stof(valores[2]);

      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);

      cout << "Asistencias: " << valorAsistencias << "%\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);

      cout << "Trabajos: " << valorTrabajos << "%\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);

      cout << "Examen: " << valorExamen << "%\n";
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
bool archivoExiste(const string &nombreArchivo) {

  ifstream archivo(nombreArchivo);
  return archivo.good();
}

bool eliminarArchivo(const string &nombreArchivo) {
  if (archivoExiste(nombreArchivo)) {
    return remove(nombreArchivo.c_str()) == 0;
  }
  return true;
}

bool renombrarArchivo(const string &viejoNombre, const string &nuevoNombre) {
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
bool validarFormatoFecha(const string &fecha) {
  if (fecha.empty())
    return true; // Permitir fechas vacías

  // Formato esperado: DD/MM/AAAA
  if (fecha.length() != 10)
    return false;
  if (fecha[2] != '/' || fecha[5] != '/')
    return false;

  try {
    int dia = stoi(fecha.substr(0, 2));
    int mes = stoi(fecha.substr(3, 2));
    int anio = stoi(fecha.substr(6, 4));

    if (dia < 1 || dia > 31)
      return false;
    if (mes < 1 || mes > 12)
      return false;
    if (anio < 2000 || anio > 2100)
      return false;

    // Validaciones adicionales según el mes
    if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && dia > 30)
      return false;
    if (mes == 2) {
      bool esBisiesto = (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
      if ((esBisiesto && dia > 29) || (!esBisiesto && dia > 28))
        return false;
    }

    return true;
  } catch (...) {
    return false;
  }
}

// Función para validar entrada numérica
bool validarEntradaNumerica(const string &entrada, float &valor, float min,
                            float max) {
  try {
    valor = stof(entrada);
    return valor >= min && valor <= max;
  } catch (...) {
    return false;
  }
}

void gen() {

  ifstream archivo("alumnos.txt");
  if (!archivo.is_open()) {
    mostrarTextoAnimado("Error al abrir el archivo de alumnos", 12);
    return;
  }

  ofstream temp("temp.txt");
  if (!temp.is_open()) {
    mostrarTextoAnimado("Error al crear archivo temporal", 12);
    archivo.close();
    return;
  }

  string linea;
  bool error = false;
  while (getline(archivo, linea)) {
    if (!linea.empty()) {
      temp << linea << endl;
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
void animarTexto(const string &texto, int velocidad) {
  for (char c : texto) {

    cout << c << flush;

    this_thread::sleep_for(chrono::milliseconds(velocidad));
  }

  cout << endl;
}

// --- IMPLEMENTACIÓN DE inicializarArchivos ---
void inicializarArchivos() {
  vector<string> archivos = {"alumnos.txt",        "fechas.txt",
                             "asistencias.txt",    "trabajos.txt",
                             "calificaciones.txt", "criterios.txt"};
  for (const auto &archivo : archivos) {
    if (!archivoExiste(archivo)) {
      ofstream nuevoArchivo(archivo);
      nuevoArchivo.close();
    }
  }
  // Inicializar criterios.txt si está vacío
  ifstream archivoCriterios("criterios.txt");
  if (archivoCriterios.peek() == ifstream::traits_type::eof()) {
    archivoCriterios.close();
    ofstream criteriosOut("criterios.txt");
    if (criteriosOut.is_open()) {
      criteriosOut << "30,40,30"; // Porcentaje por defecto
      criteriosOut.close();
    }
  } else {
    archivoCriterios.close();
  }
}

// --- IMPLEMENTACIÓN DE registrarTrabajos ---
void registrarTrabajos() {
  system("cls");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("=== REGISTRO DE TRABAJOS ===", 13);
  esperar(500);

  ifstream archivoAlumnos("alumnos.txt");
  if (!archivoAlumnos.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
    esperar(1000);
    animacionSalida("trabajos", false, 13);
    return;
  }

  vector<string> alumnos;
  string linea;
  while (getline(archivoAlumnos, linea)) {
    alumnos.push_back(linea);
  }
  archivoAlumnos.close();

  if (alumnos.empty()) {
    mostrarTextoAnimado("No hay alumnos registrados.", 12);
    esperar(1000);
    animacionSalida("trabajos", false, 13);
    return;
  }

  ifstream archivoTrabajos("trabajos.txt");
  if (!archivoTrabajos.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    mostrarTextoAnimado("Error al abrir el archivo de trabajos.", 12);
    esperar(1000);
    animacionSalida("trabajos", false, 13);
    return;
  }

  vector<pair<string, string>> trabajosConFecha;
  string lineaTrabajos;
  if (getline(archivoTrabajos, lineaTrabajos)) {
    vector<string> trabajos = split(lineaTrabajos, ',');
    for (const auto &trabajo : trabajos) {
      if (trabajo.empty())
        continue;
      size_t posFecha = trabajo.find(" (");
      if (posFecha != string::npos) {
        string nombre = trabajo.substr(0, posFecha);
        string fecha =
            trabajo.substr(posFecha + 2, trabajo.length() - posFecha - 3);
        trabajosConFecha.push_back({nombre, fecha});
      } else {
        trabajosConFecha.push_back({trabajo, ""});
      }
    }
  }
  archivoTrabajos.close();

  if (trabajosConFecha.empty()) {
    mostrarTextoAnimado("No hay trabajos registrados.", 12);
    esperar(1000);
    animacionSalida("trabajos", false, 13);
    return;
  }

  // Seleccionar trabajo
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9); // Azul neón
  cout << "\nSeleccione el trabajo:\n\n";
  for (size_t i = 0; i < trabajosConFecha.size(); ++i) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    cout << i + 1 << ". ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << trabajosConFecha[i].first;
    if (!trabajosConFecha[i].second.empty()) {
      cout << " (" << trabajosConFecha[i].second << ")";
    }
    cout << "\n";
  }

  float seleccionTrabajo;
  string entrada;
  do {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    cout << "\nIngrese el numero del trabajo (o 0 para regresar): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    getline(cin, entrada);
    if (!validarEntradaNumerica(entrada, seleccionTrabajo, 0,
                                trabajosConFecha.size())) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
      mostrarTextoAnimado("Seleccion invalida. Intente de nuevo.", 12);
      continue;
    }
    break;
  } while (true);

  if (seleccionTrabajo == 0) {
    animacionSalida("trabajos", false, 13);
    return;
  }

  // Animación al seleccionar trabajo
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  mostrarTextoAnimado(
      "Trabajo seleccionado: " +
          trabajosConFecha[static_cast<int>(seleccionTrabajo) - 1].first,
      10);
  esperar(500);

  // Borrar la lista de trabajos
  system("cls");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("=== REGISTRO DE TRABAJOS ===", 13);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  cout << "\nTrabajo seleccionado: "
       << trabajosConFecha[static_cast<int>(seleccionTrabajo) - 1].first;
  if (!trabajosConFecha[static_cast<int>(seleccionTrabajo) - 1]
           .second.empty()) {
    cout << " ("
         << trabajosConFecha[static_cast<int>(seleccionTrabajo) - 1].second
         << ")";
  }
  cout << "\n";

  // Seleccionar alumno
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9); // Azul neón
  cout << "\nSeleccione el alumno:\n\n";
  for (size_t i = 0; i < alumnos.size(); ++i) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    cout << i + 1 << ". ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << alumnos[i] << "\n";
  }

  float seleccionAlumno;
  do {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            13); // Magenta neón
    cout << "\nIngrese el numero del alumno (o 0 para regresar): ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    getline(cin, entrada);
    if (!validarEntradaNumerica(entrada, seleccionAlumno, 0, alumnos.size())) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
      mostrarTextoAnimado("Seleccion invalida. Intente de nuevo.", 12);
      continue;
    }
    break;
  } while (true);

  if (seleccionAlumno == 0) {
    animacionSalida("trabajos", false, 13);
    return;
  }

  // Animación al seleccionar alumno
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  mostrarTextoAnimado("Alumno seleccionado: " +
                          alumnos[static_cast<int>(seleccionAlumno) - 1],
                      10);
  esperar(500);

  // Borrar la lista de alumnos
  system("cls");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("=== REGISTRO DE TRABAJOS ===", 13);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  cout << "\nTrabajo seleccionado: "
       << trabajosConFecha[static_cast<int>(seleccionTrabajo) - 1].first;
  if (!trabajosConFecha[static_cast<int>(seleccionTrabajo) - 1]
           .second.empty()) {
    cout << " ("
         << trabajosConFecha[static_cast<int>(seleccionTrabajo) - 1].second
         << ")";
  }
  cout << "\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  cout << "Alumno seleccionado: "
       << alumnos[static_cast<int>(seleccionAlumno) - 1] << "\n";

  // Ingresar calificación
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  cout << "\nIngrese la calificacion (0-10): ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  float calificacion;
  do {
    getline(cin, entrada);
    if (!validarEntradaNumerica(entrada, calificacion, 0, 10)) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
      mostrarTextoAnimado(
          "Calificacion invalida. Debe ser un numero entre 0 y 10.", 12);
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
      cout << "Ingrese la calificacion (0-10): ";
      continue;
    }
    break;
  } while (true);

  // Animación al guardar calificación
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  mostrarTextoAnimado("Guardando calificación...", 10);
  esperar(500);

  // Guardar calificación
  ifstream archivoCalificaciones("calificaciones.txt");
  vector<vector<string>> calificaciones(
      alumnos.size(), vector<string>(trabajosConFecha.size(), "0"));
  while (getline(archivoCalificaciones, linea)) {
    if (linea.empty())
      continue;
    auto datos = split(linea, ',');
    if (datos.size() >= 1) {
      string nombreAlumno = datos[0];
      auto it = find(alumnos.begin(), alumnos.end(), nombreAlumno);
      if (it != alumnos.end()) {
        size_t indiceAlumno = distance(alumnos.begin(), it);
        for (size_t i = 0; i < trabajosConFecha.size(); ++i) {
          if ((i + 1) < datos.size() && !datos[i + 1].empty()) {
            calificaciones[indiceAlumno][i] = datos[i + 1];
          } else {
            calificaciones[indiceAlumno][i] = "0";
          }
        }
      }
    }
  }
  archivoCalificaciones.close();

  size_t idxAlumno = static_cast<size_t>(seleccionAlumno) - 1;
  size_t idxTrabajo = static_cast<size_t>(seleccionTrabajo) - 1;
  calificaciones[idxAlumno][idxTrabajo] = to_string(calificacion);

  ofstream archivoCalificacionesOut("calificaciones.txt");
  if (!archivoCalificacionesOut.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    mostrarTextoAnimado(
        "Error al abrir el archivo de calificaciones para escritura.", 12);
    esperar(1000);
    animacionSalida("trabajos", false, 13);
    return;
  }

  for (size_t i = 0; i < alumnos.size(); ++i) {
    archivoCalificacionesOut << alumnos[i];
    for (size_t j = 0; j < trabajosConFecha.size(); ++j) {
      archivoCalificacionesOut << "," << calificaciones[i][j];
    }
    archivoCalificacionesOut << "\n";
  }
  archivoCalificacionesOut.close();

  // Animación de éxito
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  mostrarTextoAnimado("Calificacion registrada correctamente.", 10);
  esperar(1000);

  // Animación de salida personalizada para esta sección
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  cout << "\nSaliendo de registro de trabajos";
  for (int i = 0; i < 3; ++i) {
    cout << ".";
    cout.flush();
    esperar(200);
  }
  cout << "\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  mostrarTextoAnimado("Regresando al menú principal...", 11);
  esperar(500);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void modificarAsistenciaDeUnAlumno() {
  system("cls");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  mostrarTextoAnimado("=== MODIFICAR ASISTENCIA DE UN ALUMNO ===", 11);
  mostrarBarraProgreso(700);
  esperar(300);

  ifstream archivoAlumnos("alumnos.txt");
  if (!archivoAlumnos.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo neón
    mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
    esperar(1000);
    animacionSalida("asistencias", false, 11);
    return;
  }

  vector<string> alumnos;
  string linea;
  while (getline(archivoAlumnos, linea)) {
    alumnos.push_back(linea);
  }
  archivoAlumnos.close();

  if (alumnos.empty()) {
    mostrarTextoAnimado("No hay alumnos registrados.", 12);
    esperar(1000);
    animacionSalida("asistencias", false, 11);
    return;
  }

  ifstream archivoFechas("fechas.txt");
  if (!archivoFechas.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    mostrarTextoAnimado("Error al abrir el archivo de fechas.", 12);
    esperar(1000);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
    animacionSalida("asistencias", false, 11);
    return;
  }

  string lineaFechas;
  vector<string> fechas;
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
    animacionSalida("asistencias", false, 11);
    return;
  }

  // Selección de alumno
  while (true) {
    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    mostrarTextoAnimado("=== MODIFICAR ASISTENCIA DE UN ALUMNO ===", 11);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << "\nSeleccione el alumno:\n\n";
    for (size_t i = 0; i < alumnos.size(); ++i) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              10); // Verde neón para el número
      cout << i + 1 << ". ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante para el texto
      cout << alumnos[i] << "\n";
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
    cout << "0. Cancelar y regresar\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << "\nOpción: ";
    string entrada;
    float seleccion;
    getline(cin, entrada);
    if (!validarEntradaNumerica(entrada, seleccion, 0, alumnos.size())) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
      mostrarTextoAnimado("Selección inválida. Intente de nuevo.", 12);
      esperar(700);
      continue;
    }
    if (seleccion == 0) {
      animacionSalida("asistencias", false, 11);
      return;
    }
    string alumnoSeleccionado = alumnos[static_cast<int>(seleccion) - 1];
    // Animación de transición
    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    mostrarTextoAnimado(
        "=== MODIFICAR ASISTENCIA DE: [" + alumnoSeleccionado + "] ===", 11);
    mostrarBarraProgreso(500);
    esperar(300);

    // Selección de fecha
    while (true) {
      system("cls");
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      mostrarTextoAnimado(
          "=== MODIFICAR ASISTENCIA DE: [" + alumnoSeleccionado + "] ===", 11);
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      cout << "\nSeleccione la fecha:\n\n";
      for (size_t i = 0; i < fechas.size(); ++i) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                10); // Verde neón para el número
        cout << i + 1 << ". ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                11); // Azul brillante para el texto
        cout << fechas[i] << "\n";
      }
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
      cout << "0. Cancelar y regresar\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      cout << "\nOpción: ";
      string entradaFecha;
      float seleccionFecha;
      getline(cin, entradaFecha);
      if (!validarEntradaNumerica(entradaFecha, seleccionFecha, 0,
                                  fechas.size())) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
        mostrarTextoAnimado("Selección inválida. Intente de nuevo.", 12);
        esperar(700);
        continue;
      }
      if (seleccionFecha == 0) {
        animacionSalida("asistencias", false, 11);
        return;
      }
      string fechaSeleccionada = fechas[static_cast<int>(seleccionFecha) - 1];
      // Animación de transición
      system("cls");
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      mostrarTextoAnimado(
          "=== MODIFICAR ASISTENCIA DE: [" + alumnoSeleccionado + "] ===", 11);
      mostrarBarraProgreso(500);
      esperar(300);

      // Mostrar estado actual y pedir nueva asistencia
      ifstream archivoAsistencias("asistencias.txt");
      vector<vector<string>> asistencias;
      while (getline(archivoAsistencias, linea)) {
        asistencias.push_back(split(linea, ','));
      }
      archivoAsistencias.close();

      // Rellenar con ceros si faltan filas o columnas
      int columnasNecesarias = static_cast<int>(fechas.size());
      while (asistencias.size() < alumnos.size()) {
        asistencias.push_back(vector<string>(columnasNecesarias, "0"));
      }
      for (auto &fila : asistencias) {
        if (fila.size() < columnasNecesarias) {
          fila.resize(columnasNecesarias, "0");
        }
      }

      size_t idxAlumno = static_cast<size_t>(seleccion) - 1;
      size_t idxFecha = static_cast<size_t>(seleccionFecha) - 1;

      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      cout << "\nAlumno: " << alumnoSeleccionado << "\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              11); // Azul brillante
      cout << "Fecha: " << fechaSeleccionada << "\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);

      cout << "Asistencia actual: ";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              10); // Verde neón
      cout << (asistencias[idxAlumno][idxFecha] == "1" ? "Presente" : "Ausente")
           << "\n";
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);

      cout << "\nIngrese la nueva asistencia (1 = Presente, 0 = Ausente): ";
      float nuevaAsistencia;
      string entradaAsistencia;
      getline(cin, entradaAsistencia);
      if (!validarEntradaNumerica(entradaAsistencia, nuevaAsistencia, 0, 1)) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                12); // Rojo neón
        mostrarTextoAnimado("Valor invalido. Debe ser 0 o 1.", 12);
        esperar(700);
        continue;
      }
      asistencias[idxAlumno][idxFecha] =
          to_string(static_cast<int>(nuevaAsistencia));

      // Animación de guardado
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              10); // Verde neón
      mostrarTextoAnimado("Guardando asistencia...", 10);
      mostrarBarraProgreso(900);
      esperar(300);

      ofstream archivoAsistenciasOut("asistencias.txt");
      if (!archivoAsistenciasOut.is_open()) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        mostrarTextoAnimado(
            "Error al abrir el archivo de asistencias para escritura.", 12);
        esperar(1000);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
        animacionSalida("asistencias", false, 11);
        return;
      }
      for (const auto &fila : asistencias) {
        for (size_t i = 0; i < fila.size(); ++i) {
          archivoAsistenciasOut << fila[i];
          if (i < fila.size() - 1) {
            archivoAsistenciasOut << ",";
          }
        }
        archivoAsistenciasOut << "\n";
      }
      archivoAsistenciasOut.close();

      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              10); // Verde neón
      mostrarTextoAnimado("¡Asistencia actualizada correctamente!", 10);
      mostrarBarraProgreso(700);
      esperar(700);
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
      animacionSalida("asistencias", false, 11);
      return;
    }
  }
}

void calcularPromedioFinal() {
  system("cls");
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta neón
  mostrarTextoAnimado("=== CALCULO DE PROMEDIO FINAL ===", 13);
  mostrarBarraProgreso(700);
  esperar(300);

  ifstream archivoAlumnos("alumnos.txt");
  ifstream archivoAsistencias("asistencias.txt");
  ifstream archivoCalificaciones("calificaciones.txt");
  ifstream archivoCriterios("criterios.txt");

  if (!archivoAlumnos.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12);
    esperar(1000);
    animacionSalida("promedios", false, 11);
    return;
  }
  if (!archivoAsistencias.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    mostrarTextoAnimado("Error al abrir el archivo de asistencias.", 12);
    esperar(1000);
    animacionSalida("promedios", false, 11);
    return;
  }
  if (!archivoCalificaciones.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    mostrarTextoAnimado("Error al abrir el archivo de calificaciones.", 12);
    esperar(1000);
    animacionSalida("promedios", false, 11);
    return;
  }
  if (!archivoCriterios.is_open()) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    mostrarTextoAnimado("Error al abrir el archivo de criterios de evaluacion. "
                        "Asegurate de haberlos configurado.",
                        14);
    esperar(1500);
    animacionSalida("promedios", false, 11);
    return;
  }

  vector<string> alumnos;
  string linea;
  while (getline(archivoAlumnos, linea)) {
    alumnos.push_back(linea);
  }
  archivoAlumnos.close();

  vector<vector<string>> asistencias;
  while (getline(archivoAsistencias, linea)) {
    auto datos = split(linea, ',');
    asistencias.push_back(datos);
  }
  archivoAsistencias.close();

  vector<vector<string>> calificaciones;
  while (getline(archivoCalificaciones, linea)) {
    auto datos = split(linea, ',');
    calificaciones.push_back(datos);
  }
  archivoCalificaciones.close();

  // Leer criterios de evaluacion
  float pesoAsistencias = 0, pesoTrabajos = 0, pesoExamen = 0;
  string lineaCriterios;
  if (getline(archivoCriterios, lineaCriterios)) {
    auto criterios = split(lineaCriterios, ',');
    if (criterios.size() >= 3) {
      try {
        pesoAsistencias = stof(criterios[0]);
        pesoTrabajos = stof(criterios[1]);
        pesoExamen = stof(criterios[2]);
      } catch (...) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        mostrarTextoAnimado(
            "Error al leer los pesos de los criterios de evaluacion.", 12);
        esperar(1000);
        animacionSalida("promedios", false, 11);
        return;
      }
    } else {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
      mostrarTextoAnimado("Formato incorrecto en criterios.txt.", 12);
      esperar(1000);
      animacionSalida("promedios", false, 11);
      return;
    }
  }
  archivoCriterios.close();

  // Encontrar el indice del examen en trabajos.txt (asumimos que el ultimo
  // trabajo es el examen)
  int indiceExamen = -1;
  ifstream archivoTrabajosIn("trabajos.txt");
  string lineaTrabajos;
  if (getline(archivoTrabajosIn, lineaTrabajos)) {
    auto trabajos = split(lineaTrabajos, ',');
    if (!trabajos.empty()) {
      indiceExamen = trabajos.size() - 1; // El último elemento es el examen
    }
  }
  archivoTrabajosIn.close();

  if (indiceExamen == -1) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    mostrarTextoAnimado("No se encontro ningun trabajo/examen. Asegurate de "
                        "haber agregado uno.",
                        14);
    esperar(1500);
    animacionSalida("promedios", false, 11);
    return;
  }

  cout << "\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  mostrarTextoAnimado("Promedios Finales (Escala 0-10):", 11);
  esperar(300);

  for (size_t i = 0; i < alumnos.size(); ++i) {
    float promedioFinal = 0;
    string nombreAlumno = alumnos[i];

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
    promedioFinal += contribucionAsistencia;

    // Contribucion trabajos (promedio de calificaciones de trabajos)
    float sumaCalificacionesTrabajos = 0;
    int numTrabajosCalificados = 0;
    float calificacionExamen = 0;

    if (i < calificaciones.size()) {
      for (size_t j = 0; j < calificaciones[i].size(); ++j) {
        if (static_cast<int>(j) == indiceExamen) {
          try {
            calificacionExamen = stof(calificaciones[i][j]);
          } catch (...) {
            calificacionExamen = 0;
          }
        } else if (static_cast<int>(j) <
                   indiceExamen) { // Considerar solo trabajos ANTES del examen
          try {
            sumaCalificacionesTrabajos += stof(calificaciones[i][j]);
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
    promedioFinal += contribucionTrabajos;

    // Contribucion examen
    float contribucionExamen =
        (calificacionExamen / 10.0) *
        (pesoExamen / 100.0); // Asumiendo examen calificado sobre 10
    promedioFinal += contribucionExamen;

    promedioFinal *= 10.0; // Mostrar en escala 0-10

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << nombreAlumno << ": ";
    if (promedioFinal >= 6.0) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                              10); // Verde neón
    } else {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
    }
    cout << fixed << setprecision(1) << promedioFinal << "\n";
    esperar(100);
  }

  cout << "\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  cout << "==============================================\n";
  cout << "||   ";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde neón
  cout << "Presiona ENTER para continuar...";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          11); // Azul brillante
  cout << "   ||\n";
  cout << "==============================================\n";
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
  cin.sync();
  cin.get();

  animacionSalida("promedios", false, 11);
}

// Declaración de la función centrarTexto
string centrarTexto(const string &texto) {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  int ancho = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  int espacios = (ancho - texto.length()) / 2;
  return string(espacios, ' ') + texto;
}

int main() {
  setUTF8();
  inicializarArchivos();
  int opcion;
  do {
    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << centrarTexto("Docente");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Rosa neón
    cout << "Z";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            11); // Azul brillante
    cout << "\n\n";
    mostrarMenu();
    cout << "\nSelecciona una opcion: ";
    cin >> opcion;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    switch (opcion) {
    case 1:
      mostrarCuadriculaAsistencias();
      break;
    case 2:
      mostrarCuadriculaTrabajos();
      break;
    case 3:
      modificarAsistencias();
      break;
    case 4:
      registrarTrabajos();
      break;
    case 5:
      agregarAlumno();
      break;
    case 6:
      gestionarFechas();
      break;
    case 7:
      gestionarTrabajos();
      break;
    case 8:
      gestionarCriteriosEvaluacion();
      break;
    case 9:
      modificarCalificacionAlumno();
      break;
    case 10:
      modificarAsistenciaDeUnAlumno();
      break;
    case 11:
      calcularPromedioParcial();
      break;
    case 12:
      calcularPromedioSemestre();
      break;
    case 0:
      animacionSalida("sistema", true, 11);
      break;
    default:
      animacionSalida("sistema", true, 11);
      break;
    }
  } while (opcion != 0);
  return 0;
}

void animacionSalida(const std::string &nombreSeccion, bool esSalidaSistema,
                     int color) {
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                          color); // Color personalizado
  cout << "\n\n";                 // Salto de línea antes del texto
  for (char c : ("Saliendo de " + nombreSeccion + "...")) {
    cout << c;
    cout.flush();
    esperar(30);
  }
  cout << "\n"; // Salto de línea antes de los puntos
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo neón
  for (int i = 0; i < 3; ++i) {
    cout << ".";
    cout.flush();
    esperar(350);
  }
  cout << "\n";
  if (!esSalidaSistema) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    for (char c : std::string("Regresando al menú principal.")) {
      cout << c;
      cout.flush();
      esperar(30);
    }
    cout << "\n";
  }
  esperar(600);
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}
