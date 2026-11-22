#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Juego.h"
#include "Estrategia.h"

using namespace std;

// --- CONSTANTES PARA PARSEO (CLI) ---
const int MAX_LINEA = 512;

// --- DECLARACION DE FUNCIONES DE LA INTERFAZ (CLI) ---
void mostrarMenuVisual();
void mostrarAyudaGeneral();
void mostrarAyudaComando(const string& comando);
bool validarParametros(const string& comando, int cantParams);
vector<string> dividirCadena(const string& linea);
bool convertirEnteroCLI(const string& texto, int& valor);
void procesarComando(const vector<string>& tokens, bool& enEjecucion, bool modoScript);
void EjecutarComandos(const string& nombreArchivo, bool& enEjecucion);

// --- FUNCION PRINCIPAL ---
int main() {
    // Inicializar la semilla para los dados aleatorios
    srand(time(0));

    bool enEjecucion = true;

    // Mostrar menu visual inicial
    mostrarMenuVisual();

    // Bucle interactivo de la consola
    while (enEjecucion) {
        cout << "\nRISK-CLI $ ";

        string linea;
        if (!getline(cin, linea)) {
            break; // Manejo de EOF o cierre
        }
        if (linea.size() > static_cast<size_t>(MAX_LINEA)) {
            linea.resize(MAX_LINEA);
        }

        // Dividir la cadena de caracteres en palabras/tokens
        vector<string> tokens = dividirCadena(linea);
        if (tokens.empty()) {
            continue;
        }

        procesarComando(tokens, enEjecucion, false);
    }

    return 0;
}

// --- ENRUTAMIENTO DE COMANDOS ---
// modoScript = true cuando el comando viene de un archivo (ejecutar_comandos):
// en ese caso no se hacen preguntas por consola.
void procesarComando(const vector<string>& tokens, bool& enEjecucion, bool modoScript) {
    const string& comando = tokens[0];
    int cantParams = static_cast<int>(tokens.size()) - 1;
    int totalTokens = static_cast<int>(tokens.size());

    if (comando == "menu") {
        mostrarMenuVisual();
    }
    else if (comando == "ayuda") {
        if (cantParams == 0) {
            mostrarAyudaGeneral();
        } else {
            mostrarAyudaComando(tokens[1]);
        }
    }
    else if (comando == "salir") {
        if (cantParams > 0) {
            cout << "Error: El comando 'salir' no recibe parametros." << endl;
        } else {
            Salir(enEjecucion); // Enrutamiento correcto al comando 2.6
        }
    }
    else if (comando == "ejecutar_comandos") {
        if (modoScript) {
            cout << "Error: No se puede ejecutar un archivo de comandos dentro de otro." << endl;
        } else if (validarParametros(comando, cantParams)) {
            EjecutarComandos(tokens[1], enEjecucion);
        }
    }
    else if (comando == "atacar" && cantParams >= 4) {
        // Forma directa: atacar <jugador> <origen> <destino> <dados...> [traslado]
        vector<int> dados;
        for (int i = 4; i < totalTokens; i++) {
            int valor;
            if (!convertirEnteroCLI(tokens[i], valor)) {
                cout << "Error: '" << tokens[i] << "' no es un numero valido de dado." << endl;
                return;
            }
            dados.push_back(valor);
        }
        AtacarDirecto(tokens[1], tokens[2], tokens[3], dados);
    }
    else if (comando == "fortificar" && cantParams == 4) {
        // Forma directa: fortificar <jugador> <origen> <destino> <cantidad>
        int cantidad;
        if (!convertirEnteroCLI(tokens[4], cantidad)) {
            cout << "Error: '" << tokens[4] << "' no es una cantidad valida." << endl;
            return;
        }
        FortificarDirecto(tokens[1], tokens[2], tokens[3], cantidad);
    }
    else if (comando == "ubicar_unidad") {
        if (validarParametros(comando, cantParams)) {
            int cantidad;
            if (!convertirEnteroCLI(tokens[3], cantidad)) {
                cout << "Error: '" << tokens[3] << "' no es una cantidad valida." << endl;
                return;
            }
            UbicarUnidad(tokens[1], tokens[2], cantidad);
        }
    }
    else if (comando == "robar_carta") {
        if (validarParametros(comando, cantParams)) {
            int idCarta;
            if (!convertirEnteroCLI(tokens[2], idCarta)) {
                cout << "Error: '" << tokens[2] << "' no es un identificador de carta valido." << endl;
                return;
            }
            RobarCarta(tokens[1], idCarta);
        }
    }
    else if (comando == "canjear_cartas") {
        if (validarParametros(comando, cantParams)) {
            int ids[3];
            for (int i = 0; i < 3; i++) {
                if (!convertirEnteroCLI(tokens[2 + i], ids[i])) {
                    cout << "Error: '" << tokens[2 + i] << "' no es un identificador de carta valido." << endl;
                    return;
                }
            }
            CanjearCartas(tokens[1], ids[0], ids[1], ids[2]);
        }
    }
    else if (comando == "turno_siguiente") {
        if (validarParametros(comando, cantParams)) {
            TurnoSiguiente();
        }
    }
    else if (comando == "guardar_juego" || comando == "guardar") {
        if (validarParametros(comando, cantParams)) {
            GuardarJuego(tokens[1]);
        }
    }
    else if (comando == "inicializar" ||
             comando == "obtener_unidades" ||
             comando == "atacar" ||
             comando == "fortificar" ||
             comando == "guardar_comprimido" ||
             comando == "costo_conquista" ||
             comando == "conquista_mas_barata" ||
             comando == "estado_juego") {

        // Validar cantidad de parametros esperados
        if (validarParametros(comando, cantParams)) {

            // --- ENRUTAMIENTO DE COMANDOS VALIDOS ---
            if (comando == "inicializar") {
                Inicializar(tokens[1]);
            }
            else if (comando == "obtener_unidades") {
                if (modoScript) {
                    ObtenerUnidadesDirecto(tokens[1]); // Luego se usan comandos ubicar_unidad
                } else {
                    ObtenerUnidades(tokens[1]);        // Pregunta por consola donde ubicar
                }
            }
            else if (comando == "atacar") {
                if (modoScript) {
                    cout << "Error: En un archivo de comandos use: atacar <jugador> <origen> <destino> <dados...>" << endl;
                } else {
                    Atacar(tokens[1]);
                }
            }
            else if (comando == "fortificar") {
                if (modoScript) {
                    cout << "Error: En un archivo de comandos use: fortificar <jugador> <origen> <destino> <cantidad>" << endl;
                } else {
                    Fortificar(tokens[1]);
                }
            }
            else if (comando == "estado_juego") {
                EstadoJuego();
            }
            else if (comando == "guardar_comprimido") {
                GuardarComprimido(tokens[1]);
            }
            else if (comando == "costo_conquista") {
                CostoConquista(tokens[1], tokens[2]);
            }
            else if (comando == "conquista_mas_barata") {
                ConquistaMasBarata(tokens[1]);
            }
        }
    }
    else {
        cout << "Error: Comando no reconocido. Escriba 'menu' o 'ayuda' para ver las opciones." << endl;
    }
}

// --- EJECUCION DE UN ARCHIVO DE COMANDOS ---
// Lee el archivo linea por linea, ignora lineas vacias y comentarios (#) y
// aplica cada comando sobre la partida actual, mostrando lo que ocurre.
void EjecutarComandos(const string& nombreArchivo, bool& enEjecucion) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "(Archivo erroneo) " << nombreArchivo << " no se encuentra o no puede leerse." << endl;
        return;
    }

    string linea;
    int numeroLinea = 0;
    int ejecutados = 0;

    cout << "=== Ejecutando comandos de " << nombreArchivo << " ===" << endl;
    while (enEjecucion && getline(archivo, linea)) {
        numeroLinea++;

        // Quitar comentarios
        size_t posComentario = linea.find('#');
        if (posComentario != string::npos) {
            linea.erase(posComentario);
        }

        vector<string> tokens = dividirCadena(linea);
        if (tokens.empty()) {
            continue;
        }

        cout << "\n[linea " << numeroLinea << "] >";
        for (size_t i = 0; i < tokens.size(); i++) {
            cout << " " << tokens[i];
        }
        cout << endl;

        procesarComando(tokens, enEjecucion, true);
        ejecutados++;
    }
    archivo.close();
    cout << "\n=== Fin del archivo " << nombreArchivo << ": " << ejecutados << " comandos procesados ===" << endl;
}

// --- IMPLEMENTACION DE FUNCIONES AUXILIARES ---

bool convertirEnteroCLI(const string& texto, int& valor) {
    size_t i = 0;
    bool negativo = false;
    if (!texto.empty() && texto[0] == '-') {
        negativo = true;
        i = 1;
    }
    if (i >= texto.size()) return false;
    valor = 0;
    for (; i < texto.size(); i++) {
        if (texto[i] < '0' || texto[i] > '9') return false;
        valor = valor * 10 + (texto[i] - '0');
    }
    if (negativo) valor = -valor;
    return true;
}

// Divide la linea en palabras separadas por espacios/tabs usando un
// istringstream (extraccion formateada con >>), que ya ignora espacios en
// blanco repetidos al inicio, en medio y al final.
vector<string> dividirCadena(const string& linea) {
    vector<string> tokens;
    istringstream flujo(linea);
    string palabra;
    while (flujo >> palabra) {
        tokens.push_back(palabra);
    }
    return tokens;
}

void mostrarMenuVisual() {
    cout << "                  SISTEMA DE APOYO - RISK                        " << endl;
    cout << " COMANDOS DISPONIBLES:                                           " << endl;
    cout << "  * inicializar <archivo.txt>       -> Cargar estado inicial (con o sin cartas)" << endl;
    cout << "  * ejecutar_comandos <archivo.txt> -> Aplicar al juego los comandos de un archivo" << endl;
    cout << "  * guardar <archivo.txt>           -> Guardar el estado actual en texto plano" << endl;
    cout << "  * guardar_comprimido <archivo.bin> -> Guardar el estado comprimido con Huffman" << endl;
    cout << "  * obtener_unidades <jugador>                                     " << endl;
    cout << "  * ubicar_unidad <jugador> <territorio> <cantidad>                " << endl;
    cout << "  * atacar <jugador>                                               " << endl;
    cout << "  * atacar <jugador> <origen> <destino> <dados...> [traslado]      " << endl;
    cout << "  * fortificar <jugador>                                           " << endl;
    cout << "  * fortificar <jugador> <origen> <destino> <cantidad>             " << endl;
    cout << "  * robar_carta <jugador> <id_carta>                               " << endl;
    cout << "  * canjear_cartas <jugador> <id1> <id2> <id3>                     " << endl;
    cout << "  * turno_siguiente                                                " << endl;
    cout << "  * estado_juego                                                   " << endl;
    cout << "  * costo_conquista <jugador> <territorio>                          " << endl;
    cout << "  * conquista_mas_barata <jugador>                                  " << endl;
    cout << "  * menu             -> Mostrar nuevamente este menu interactivo    " << endl;
    cout << "  * ayuda [comando]  -> Ver descripcion detallada de un comando     " << endl;
    cout << "  * salir            -> Salir del programa                          " << endl;
}

void mostrarAyudaGeneral() {
    cout << "\n--- AYUDA GENERAL DE COMANDOS ---" << endl;
    cout << "Escriba 'ayuda <nombre_comando>' para mas detalles." << endl;
}

void mostrarAyudaComando(const string& cmd) {
    if (cmd == "inicializar") {
        cout << "Comando: inicializar <archivo_inicio.txt>" << endl;
        cout << "Descripcion: Inicializa el juego a partir de un archivo de texto o de un .bin" << endl;
        cout << "  generado por guardar_comprimido. El texto acepta el formato basico" << endl;
        cout << "  (jugadores + 42 territorios) o el formato con cartas:" << endl;
        cout << "    <numJugadores>" << endl;
        cout << "    <nombre> <color> <numCartas> [<id> <territorio> <tipo>]..." << endl;
        cout << "    <numCartasMazo>" << endl;
        cout << "    <id> <territorio> <tipo>          (una por carta del mazo)" << endl;
        cout << "    <territorio> <color> <unidades>   (42 lineas)" << endl;
    } else if (cmd == "ejecutar_comandos") {
        cout << "Comando: ejecutar_comandos <archivo_comandos.txt>" << endl;
        cout << "Descripcion: Lee el archivo linea por linea y aplica cada comando al juego." << endl;
        cout << "  Las lineas vacias y lo que sigue a '#' se ignoran." << endl;
    } else if (cmd == "guardar_juego" || cmd == "guardar") {
        cout << "Comando: guardar <archivo.txt>   (tambien: guardar_juego)" << endl;
        cout << "Descripcion: Crea un archivo de texto plano con el estado actual (jugadores con sus" << endl;
        cout << "  cartas, mazo y territorios), en el mismo formato que acepta 'inicializar'." << endl;
    } else if (cmd == "guardar_comprimido") {
        cout << "Comando: guardar_comprimido <archivo.bin>" << endl;
        cout << "Descripcion: Guarda la misma informacion que 'guardar', comprimida con codificacion" << endl;
        cout << "  de Huffman en un archivo binario: n c1 f1 ... cn fn w binary_code." << endl;
        cout << "  'inicializar' puede leer ese .bin directamente." << endl;
    } else if (cmd == "obtener_unidades") {
        cout << "Comando: obtener_unidades <nombre_jugador>" << endl;
        cout << "Descripcion: Calcula las unidades del turno (territorios, continentes y canje)." << endl;
        cout << "  En consola pregunta donde ubicarlas; en un archivo de comandos se ubican con ubicar_unidad." << endl;
    } else if (cmd == "ubicar_unidad") {
        cout << "Comando: ubicar_unidad <jugador> <territorio> <cantidad>" << endl;
        cout << "Descripcion: Ubica unidades reclamadas en un territorio propio." << endl;
    } else if (cmd == "atacar") {
        cout << "Comando: atacar <nombre_jugador>" << endl;
        cout << "Descripcion: Inicia la fase de ataques utilizando dados aleatorios (interactivo)." << endl;
        cout << "Comando: atacar <jugador> <origen> <destino> <dados...> [traslado]" << endl;
        cout << "Descripcion: Resuelve una tirada con los dados indicados: primero los del atacante" << endl;
        cout << "  (min(3, unidades-1)) y luego los del defensor (min(2, unidades)). Si conquista," << endl;
        cout << "  se trasladan [traslado] unidades (por defecto: dados del atacante - 1)." << endl;
    } else if (cmd == "fortificar") {
        cout << "Comando: fortificar <nombre_jugador>" << endl;
        cout << "Descripcion: Permite trasladar unidades entre territorios propios vecinos al cierre del turno." << endl;
        cout << "Comando: fortificar <jugador> <origen> <destino> <cantidad>" << endl;
        cout << "Descripcion: Traslada unidades entre territorios propios conectados (usar luego turno_siguiente)." << endl;
    } else if (cmd == "robar_carta") {
        cout << "Comando: robar_carta <jugador> <id_carta>" << endl;
        cout << "Descripcion: Pasa la carta indicada del mazo a la mano del jugador." << endl;
    } else if (cmd == "canjear_cartas") {
        cout << "Comando: canjear_cartas <jugador> <id1> <id2> <id3>" << endl;
        cout << "Descripcion: Canjea un trio (3 iguales o 1 de cada tipo) por unidades: 4, 6, 8, 10, 12, 15, +5..." << endl;
    } else if (cmd == "turno_siguiente") {
        cout << "Comando: turno_siguiente" << endl;
        cout << "Descripcion: Termina el turno actual y pasa al siguiente jugador activo." << endl;
    } else if (cmd == "estado_juego") {
        cout << "Comando: estado_juego" << endl;
        cout << "Descripcion: Presenta un resumen completo del estado actual de la partida y sus territorios." << endl;
    } else if (cmd == "costo_conquista") {
        cout << "Comando: costo_conquista <nombre_jugador> <territorio>" << endl;
        cout << "Descripcion: Calcula la secuencia de territorios que el jugador debe conquistar (y las" << endl;
        cout << "  unidades enemigas que debe vencer) para controlar el territorio dado, atacando desde" << endl;
        cout << "  el territorio propio mas cercano (ruta de menor costo en el grafo del tablero)." << endl;
    } else if (cmd == "conquista_mas_barata") {
        cout << "Comando: conquista_mas_barata <nombre_jugador>" << endl;
        cout << "Descripcion: Entre todos los territorios enemigos, indica cual implica perder menos" << endl;
        cout << "  unidades de ejercito, desde donde atacar y por cuales territorios pasar." << endl;
    } else if (cmd == "salir") {
        cout << "Comando: salir" << endl;
        cout << "Descripcion: Termina la ejecucion de la aplicacion." << endl;
    } else {
        cout << "El comando '" << cmd << "' no existe o no tiene ayuda disponible." << endl;
    }
}

bool validarParametros(const string& comando, int cantParams) {
    int esperados = 0;
    if (comando == "inicializar" ||
        comando == "obtener_unidades" ||
        comando == "atacar" ||
        comando == "fortificar" ||
        comando == "guardar" ||
        comando == "guardar_juego" ||
        comando == "guardar_comprimido" ||
        comando == "conquista_mas_barata" ||
        comando == "ejecutar_comandos") {
        esperados = 1;
    } else if (comando == "costo_conquista" ||
               comando == "robar_carta") {
        esperados = 2;
    } else if (comando == "ubicar_unidad") {
        esperados = 3;
    } else if (comando == "canjear_cartas") {
        esperados = 4;
    } else if (comando == "estado_juego" ||
               comando == "turno_siguiente") {
        esperados = 0;
    }

    if (cantParams != esperados) {
        cout << "Error: Cantidad de parametros incorrecta para el comando '" << comando << "'." << endl;
        cout << "Se esperaban " << esperados << " parametro(s) y se recibieron " << cantParams << "." << endl;
        return false;
    }
    return true;
}
