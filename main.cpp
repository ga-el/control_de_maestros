#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <limits> // Necesario para std::numeric_limits
#ifndef NOMINMAX // Previene que windows.h defina min/max como macros si ya está definido
#define NOMINMAX
#endif
#include <windows.h>

// Declaraciones de funciones
void setUTF8();
std::vector<std::string> split(const std::string& linea, char delim);
void mostrarMenu();
void mostrarCuadriculaAsistencias();
void modificarAsistencias();
void gestionarFechas();
void agregarAlumno();
void esperar(int milisegundos);
void mostrarTextoAnimado(const std::string& texto, int color);
void mostrarBarraProgreso(int duracion);

void setUTF8() {
    system("chcp 65001 > nul");
}

std::vector<std::string> split(const std::string& linea, char delim) {
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
    std::cout << "4. Promedio del parcial\n";
    std::cout << "5. Promedio del semestre\n";
    
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Nuevo color para 6, 7, 8 (Verde)
    std::cout << "6. Agregar alumnos\n";
    std::cout << "7. Agregar trabajos\n";
    std::cout << "8. Gestionar Fechas\n";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Nuevo color para 9 y 0 (Magenta)
    std::cout << "9. Definir metodo de calificacion\n";
    std::cout << "0. Salir\n";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Volver a blanco por defecto
}

void mostrarCuadriculaAsistencias() {
    std::ifstream archivoAlumnos("alumnos.txt");
    std::ifstream archivoFechas("fechas.txt");
    std::ifstream archivoAsistencias("asistencias.txt");

    system("cls");
    mostrarTextoAnimado("=== CARGANDO LISTA DE ASISTENCIAS ===", 11); // Celeste para el mensaje de carga
    esperar(500); // Pequeña pausa

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
        mostrarTextoAnimado("Error al abrir el archivo de asistencias.", 12); // Rojo
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

    system("cls");
    mostrarTextoAnimado("=== LISTA DE ASISTENCIAS ===", 14); // Amarillo para el título principal de la tabla
    esperar(500);

    // Definir anchos de columna
    const int anchoNombre = 20; // Ancho fijo para el nombre del alumno
    const int anchoAsistencia = 12; // Ancho fijo para 'Sí' o 'No' y fechas

    // Mostrar encabezado de la tabla
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco para bordes y separadores
    std::cout << "+";
    for (int i = 0; i < anchoNombre; ++i) std::cout << "-";
    std::cout << "+";
    for (size_t i = 0; i < fechas.size(); ++i) {
        for (int j = 0; j < anchoAsistencia; ++j) std::cout << "-";
        std::cout << "+";
    }
    std::cout << "\n";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco para bordes y separadores
    std::cout << "|";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); // Celeste para el encabezado Nombre
    std::cout << std::left << std::setw(anchoNombre) << "Nombre";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
    std::cout << "|";
    for (const auto& fecha : fechas) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); // Celeste para las fechas
        int padding = anchoAsistencia - fecha.length();
        int padding_left = padding / 2;
        int padding_right = padding - padding_left;
        for(int i=0; i < padding_left; ++i) std::cout << " ";
        std::cout << fecha;
        for(int i=0; i < padding_right; ++i) std::cout << " ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
        std::cout << "|";
    }
    std::cout << "\n";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco para bordes y separadores
    std::cout << "+";
    for (int i = 0; i < anchoNombre; ++i) std::cout << "-";
    std::cout << "+";
    for (size_t i = 0; i < fechas.size(); ++i) {
        for (int j = 0; j < anchoAsistencia; ++j) std::cout << "-";
        std::cout << "+";
    }
    std::cout << "\n";

    // Mostrar filas de asistencia con animacion y colores
    for (size_t i = 0; i < alumnos.size(); ++i) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco para el borde izquierdo
        std::cout << "|";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde para el nombre del alumno
        std::cout << std::left << std::setw(anchoNombre) << alumnos[i];
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
        std::cout << "|";

        for (size_t j = 0; j < fechas.size(); ++j) {
             std::string valor = " "; // Espacio por defecto
             int colorValor = 15; // Blanco por defecto

             if (i < asistencias.size() && j < asistencias[i].size()) {
                valor = (asistencias[i][j] == "1") ? "Sí" : "No";
                colorValor = (asistencias[i][j] == "1") ? 10 : 12; // Verde para Sí, Rojo para No
             }

            int padding = anchoAsistencia - valor.length();
            int padding_left = padding / 2;
            int padding_right = padding - padding_left;

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco para el padding izquierdo
            for(int k=0; k < padding_left; ++k) std::cout << " ";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorValor); // Color del valor (Sí/No)
            std::cout << valor;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco para el padding derecho y borde
            for(int k=0; k < padding_right; ++k) std::cout << " ";
            std::cout << "|";
        }
        std::cout << "\n";
        esperar(100); // Pequeña pausa para la animación de filas
    }

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco para el borde inferior
    std::cout << "+";
    for (int i = 0; i < anchoNombre; ++i) std::cout << "-";
    std::cout << "+";
    for (size_t i = 0; i < fechas.size(); ++i) {
        for (int j = 0; j < anchoAsistencia; ++j) std::cout << "-";
        std::cout << "+";
    }
    std::cout << "\n";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Volver a blanco
    system("pause"); // Esperar a que el usuario presione una tecla antes de regresar
}

void esperar(int milisegundos) {
    Sleep(milisegundos);
}

void mostrarTextoAnimado(const std::string& texto, int color) {
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
    int opcionModificar;
    do {
        system("cls");
        mostrarTextoAnimado("=== MODIFICACION DE ASISTENCIAS ===", 14); // Amarillo
        esperar(500);

        std::ifstream archivoAlumnos("alumnos.txt");
        std::ifstream archivoFechas("fechas.txt");
        std::ifstream archivoAsistencias("asistencias.txt");

        if (!archivoAlumnos.is_open()) {
            mostrarTextoAnimado("Error al abrir el archivo de alumnos.", 12); // Rojo
            esperar(1000);
            return; // Salir completamente si hay error grave de archivo
        }
         if (!archivoFechas.is_open()) {
            mostrarTextoAnimado("Error al abrir el archivo de fechas.", 12); // Rojo
             esperar(1000);
            return; // Salir completamente si hay error grave de archivo
        }
        if (!archivoAsistencias.is_open()) {
            mostrarTextoAnimado("Error al abrir el archivo de asistencias.", 12); // Rojo
             esperar(1000);
            return; // Salir completamente si hay error grave de archivo
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

        // Si no hay fechas o alumnos, no podemos modificar nada relevante
        if (fechas.empty() || alumnos.empty()) {
             if (fechas.empty()) mostrarTextoAnimado("No hay fechas disponibles para modificar.", 14); // Amarillo
             if (alumnos.empty()) mostrarTextoAnimado("No hay alumnos disponibles para modificar.", 14); // Amarillo
             esperar(1500);
             opcionModificar = 0; // Salir del bucle si no hay datos
             continue; // Ir al final del bucle
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

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta para regresar
        std::cout << "0. Regresar al Menu Principal\n";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco


        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
        std::cout << "\nOpcion: ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
        std::cin >> opcionModificar;

        // --- Manejar la opción de salir primero ---
        if (opcionModificar == 0) {
            mostrarTextoAnimado("Regresando al Menu Principal...", 13); // Magenta
            esperar(500);
            break; // Salir del bucle do-while
        }

        // Ajustar la opción si no es 0 para que coincida con el índice base 0 del vector fechas
        // Si la opcion era 0, ya salimos, así que esta parte solo se ejecuta para 1 en adelante
        opcionModificar--;

        // --- Ahora comprobar si la opción de fecha es válida (excluyendo el caso 0 ya manejado) ---
        if (opcionModificar < 0 || opcionModificar >= fechas.size()) { // opcionModificar < 0 ya no debería ocurrir si se ingresó > 0
            mostrarTextoAnimado("Opcion invalida.", 12); // Rojo
            esperar(1000);
            continue; // Volver al inicio del bucle (mostrar menú de fechas de nuevo)
        }


        // --- Si llegamos aquí, una fecha válida (índice >= 0) fue seleccionada ---
        // Resto de la lógica de modificación de asistencia para la fecha seleccionada
        system("cls");
        mostrarTextoAnimado("=== REGISTRO DE ASISTENCIAS ===", 14); // Amarillo
        mostrarTextoAnimado("Fecha: " + fechas[opcionModificar], 11); // Celeste
        std::cout << "\n";

        for (size_t i = 0; i < alumnos.size(); ++i) {
            if (i < asistencias.size() && opcionModificar < asistencias[i].size()) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // Verde
                std::cout << "\n" << alumnos[i] << "\n";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo
                std::cout << "Asistencia (1 = Sí, 0 = No): ";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
                std::string valor;
                std::cin >> valor;
                if (valor != "1" && valor != "0") valor = "0";
                asistencias[i][opcionModificar] = valor;
            } else {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); // Rojo
                std::cout << "\nAdvertencia: Datos de asistencia faltantes para " << alumnos[i] << " en esta fecha.\n";
                 SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco
            }
        }

        mostrarTextoAnimado("\nGuardando cambios...", 11); // Celeste
        mostrarBarraProgreso(1000);

        std::ofstream archivoOut("asistencias.txt");
        if (!archivoOut.is_open()) {
            mostrarTextoAnimado("Error al guardar el archivo de asistencias.", 12); // Rojo
            esperar(1000);
            // Considerar qué hacer aquí: ¿deberíamos intentar guardar de nuevo? Por ahora, solo mostramos el error.
        } else {
            for (size_t i = 0; i < asistencias.size(); ++i) {
                for (size_t j = 0; j < asistencias[i].size(); ++j) {
                    archivoOut << asistencias[i][j];
                    if (j != asistencias[i].size() - 1) archivoOut << ",";
                }
                archivoOut << "\n";
            }
            archivoOut.close();
            mostrarTextoAnimado("\n¡Asistencias actualizadas correctamente!", 10); // Verde
        }

        esperar(1000);

    } while (opcionModificar != -1); // Continuar hasta que la opción sea la de salir
}

void gestionarFechas() {
    int opcionGestionFechas;
    do {
        system("cls");
        mostrarTextoAnimado("=== GESTION DE FECHAS ===", 10); // Verde para el título principal

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); // Amarillo para las opciones
        std::cout << "\nSelecciona una opcion:\n";
        std::cout << "1. Agregar Fecha\n";
        std::cout << "2. Eliminar Fecha\n";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13); // Magenta para regresar
        std::cout << "0. Regresar al Menu Principal\n";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco

        std::cout << "\nSelecciona una opcion: ";
        std::cin >> opcionGestionFechas;
        std::cin.ignore();

        switch (opcionGestionFechas) {
            case 1: { // Agregar Fecha
                system("cls");
                mostrarTextoAnimado("=== AGREGAR FECHA ===", 10); // Verde para el título de esta sección
                esperar(500); // Pequeña pausa

                std::string nuevaFecha;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); // Celeste para la instrucción
                std::cout << "\nIntroduce la nueva fecha (ej. DD/MM/AAAA): ";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco para la entrada del usuario
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
                    mostrarTextoAnimado("Error al crear archivo temporal para fechas.", 12); // Rojo
                    esperar(1000);
                    break;
                }

                for(size_t i = 0; i < fechasActuales.size(); ++i) {
                    archivoFechasTemp << fechasActuales[i];
                    if (i < fechasActuales.size() - 1) archivoFechasTemp << ",";
                }
                archivoFechasTemp << "\n";
                archivoFechasTemp.close();

                // Reemplazar el archivo original
                remove("fechas.txt");
                rename("fechas.temp", "fechas.txt");
                mostrarTextoAnimado("Fecha agregada a fechas.txt", 10); // Verde

                // Actualizar asistencias.txt
                 mostrarTextoAnimado("Actualizando archivo de asistencias...", 11); // Celeste
                 mostrarBarraProgreso(1000); // Barra de progreso para la actualización grande

                std::vector<std::string> lineasAsistencia;
                std::ifstream archivoAsistenciasIn("asistencias.txt");
                std::string lineaAsistencia;
                while(getline(archivoAsistenciasIn, lineaAsistencia)) {
                    lineasAsistencia.push_back(lineaAsistencia);
                }
                archivoAsistenciasIn.close();

                std::ofstream archivoAsistenciasOut("asistencias.temp");
                 if (!archivoAsistenciasOut.is_open()) {
                     mostrarTextoAnimado("Error al crear archivo temporal para asistencias.", 12); // Rojo
                     esperar(1000);
                     break;
                 }

                for(const auto& l : lineasAsistencia) {
                    if (!l.empty()) {
                        archivoAsistenciasOut << l << ",0\n";
                    } else {
                        archivoAsistenciasOut << "\n";
                    }
                }
                archivoAsistenciasOut.close();

                remove("asistencias.txt");
                rename("asistencias.temp", "asistencias.txt");
                mostrarTextoAnimado("Archivo de asistencias actualizado con la nueva columna.", 10); // Verde

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

                mostrarTextoAnimado("\nSelecciona el numero de la fecha a eliminar:\n", 11); // Celeste
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

                mostrarTextoAnimado("\nEliminando fecha y actualizando archivos...", 11); // Mensaje antes de la carga
                mostrarBarraProgreso(1500); // Animación de carga (ajusta la duración si es necesario)

                // Escribir fechas sin la fecha eliminada al archivo temporal
                std::ofstream archivoFechasTemp("fechas.temp");
                 if (!archivoFechasTemp.is_open()) {
                    mostrarTextoAnimado("Error al crear archivo temporal para fechas.", 12); // Rojo
                    esperar(1000);
                    break;
                }
                for(size_t i = 0; i < fechas.size(); ++i) {
                    archivoFechasTemp << fechas[i];
                    if (i < fechas.size() - 1) archivoFechasTemp << ",";
                }
                archivoFechasTemp << "\n";
                archivoFechasTemp.close();

                // Reemplazar el archivo original
                remove("fechas.txt");
                rename("fechas.temp", "fechas.txt");
                mostrarTextoAnimado("Fecha " + fechaEliminada + " eliminada de fechas.txt", 10); // Verde

                // Eliminar la columna correspondiente en asistencias.txt
                 std::vector<std::vector<std::string>> asistencias;
                 std::ifstream archivoAsistenciasIn("asistencias.txt");
                 std::string lineaAsistencia;
                 while(getline(archivoAsistenciasIn, lineaAsistencia)) {
                    std::vector<std::string> datos = split(lineaAsistencia, ',');
                     if (opcionFechaEliminar < datos.size()) {
                        datos.erase(datos.begin() + opcionFechaEliminar);
                     }
                    asistencias.push_back(datos);
                }
                archivoAsistenciasIn.close();

                 std::ofstream archivoAsistenciasOut("asistencias.temp");
                 if (!archivoAsistenciasOut.is_open()) {
                     mostrarTextoAnimado("Error al crear archivo temporal para asistencias.", 12); // Rojo
                     esperar(1000);
                     break;
                 }

                for(size_t i = 0; i < asistencias.size(); ++i) {
                    for(size_t j = 0; j < asistencias[i].size(); ++j) {
                         archivoAsistenciasOut << asistencias[i][j];
                         if (j < asistencias[i].size() - 1) archivoAsistenciasOut << ",";
                    }
                     archivoAsistenciasOut << "\n";
                }
                 archivoAsistenciasOut.close();

                remove("asistencias.txt");
                rename("asistencias.temp", "asistencias.txt");

                mostrarTextoAnimado("Columna de asistencia correspondiente eliminada.", 10); // Verde

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

void agregarAlumno() {
    system("cls");
    mostrarTextoAnimado("=== AGREGAR ALUMNO ===", 10); // Verde para el título
    esperar(500);

    std::string nuevoNombre;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); // Celeste para la instrucción
    std::cout << "\nIntroduce el nombre completo del nuevo alumno: ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); // Blanco para la entrada del usuario
    // Usar getline para leer el nombre completo, incluyendo espacios
    std::getline(std::cin, nuevoNombre);

    if (nuevoNombre.empty()) {
        mostrarTextoAnimado("Nombre invalido.", 12); // Rojo
        esperar(1000);
        return;
    }

    // Añadir alumno a alumnos.txt
    mostrarTextoAnimado("Guardando alumno en alumnos.txt...", 11); // Celeste
    std::ofstream archivoAlumnosOut("alumnos.txt", std::ios::app);
    if (!archivoAlumnosOut.is_open()) {
        mostrarTextoAnimado("Error al abrir el archivo de alumnos para agregar.", 12); // Rojo
        esperar(1000);
        return;
    }
    archivoAlumnosOut << nuevoNombre << "\n"; // Cada alumno en una nueva linea
    archivoAlumnosOut.close();
    mostrarTextoAnimado("Alumno agregado a alumnos.txt", 10); // Verde

    // Añadir fila correspondiente en asistencias.txt con '0's
    mostrarTextoAnimado("Actualizando asistencias.txt...", 11); // Celeste

    // Primero, leer el numero de fechas para saber cuantas columnas añadir
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

    std::ofstream archivoAsistenciasOut("asistencias.txt", std::ios::app);
     if (!archivoAsistenciasOut.is_open()) {
         mostrarTextoAnimado("Error al abrir el archivo de asistencias para actualizar.", 12); // Rojo
         esperar(1000);
         return;
     }

    // Escribir la nueva fila de '0's
    for(size_t i = 0; i < fechas.size(); ++i) {
        archivoAsistenciasOut << "0";
        if (i < fechas.size() - 1) archivoAsistenciasOut << ",";
    }
     archivoAsistenciasOut << "\n"; // Nueva linea para el nuevo alumno
    archivoAsistenciasOut.close();
    mostrarTextoAnimado("Asistencias.txt actualizado con la nueva fila.", 10); // Verde

    mostrarTextoAnimado("\nAlumno " + nuevoNombre + " agregado con exito!", 10); // Mensaje final de éxito
    esperar(1500);
}

int main() {
    setUTF8();

    int opcion;
    do {
        mostrarMenu();
        std::cout << "\nSelecciona una opcion: ";
        std::cin >> opcion;
        // Limpiar el buffer de entrada después de leer el número
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (opcion) {
            case 1: mostrarCuadriculaAsistencias(); break;
            case 2: modificarAsistencias(); break;
            case 3: std::cout << "Lista de trabajos\n"; break;
            case 4: std::cout << "Promedio del parcial\n"; break;
            case 5: std::cout << "Promedio del semestre\n"; break;
            case 6: agregarAlumno(); break;
            case 7: std::cout << "Agregar trabajos\n"; break;
            case 8: gestionarFechas(); break;
            case 9: std::cout << "Definir metodo de calificacion\n"; break;
            case 0: std::cout << "Saliendo...\n"; break;
            default: std::cout << "Opcion invalida\n";
        }
    } while (opcion != 0);

    return 0;
}
