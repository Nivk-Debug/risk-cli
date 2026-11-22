#include "Juego.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <vector>
#include <string>
#include "Huffman.h"

using namespace std;

// Instancia global del Juego
Juego partidaActual;

// --- ASIGNACION DE CONTINENTE A PARTIR DEL CODIGO OFICIAL DE TERRITORIO ---
// El enunciado identifica cada territorio exclusivamente con el codigo oficial
// "continente.indice" de la Tabla 1 (p. ej. "1.1" = Alaska, "5.6" = Kamchatka).
// Este es el UNICO identificador aceptado por el archivo de inicio: no se
// reconocen nombres reales de territorio (p. ej. "Alaska"), precisamente
// porque ese fue el error senalado en la retroalimentacion de la Entrega 1
// ("contiene un territorio desconocido (1.1)": el archivo del profesor usa el
// codigo numerico, no el nombre). codigo debe ser exactamente "N.M", con N
// entre 1 y TOTAL_CONTINENTES; cualquier otra cosa (incluido un nombre real)
// se considera un territorio desconocido.
int determinarContinentePorCodigo(const string& codigo) {
    if (codigo.empty()) return -1;
    if (!isdigit(static_cast<unsigned char>(codigo[0]))) return -1;

    size_t i = 0;
    int numeroContinente = 0;
    while (i < codigo.size() && isdigit(static_cast<unsigned char>(codigo[i]))) {
        numeroContinente = numeroContinente * 10 + (codigo[i] - '0');
        i++;
    }
    if (i >= codigo.size() || codigo[i] != '.' || i + 1 >= codigo.size() ||
        !isdigit(static_cast<unsigned char>(codigo[i + 1]))) {
        return -1;
    }
    i++;
    while (i < codigo.size() && isdigit(static_cast<unsigned char>(codigo[i]))) { i++; }

    if (i != codigo.size() || numeroContinente < 1 || numeroContinente > TOTAL_CONTINENTES) {
        return -1; // Formato distinto a "N.M", o continente fuera de rango
    }
    return numeroContinente - 1;
}

// --- CONEXION DE ADYACENCIAS REALES DEL TABLERO DE RISK ---
// Si alguno de los dos codigos no corresponde a un territorio cargado, la
// llamada simplemente no hace nada (ver buscarIndiceTerritorio).
void conectarAdyacencia(Juego& juego, const string& t1, const string& t2) {
    int idx1 = buscarIndiceTerritorio(juego, t1);
    int idx2 = buscarIndiceTerritorio(juego, t2);
    if (idx1 != -1 && idx2 != -1) {
        if (!esVecinoTerritorio(juego.territorios[idx1], idx2)) {
            agregarVecinoTerritorio(juego.territorios[idx1], idx2);
        }
        if (!esVecinoTerritorio(juego.territorios[idx2], idx1)) {
            agregarVecinoTerritorio(juego.territorios[idx2], idx1);
        }
    }
}

// --- MAPA REAL DE RISK, INDEXADO POR CODIGO OFICIAL "N.M" (Tabla 1 del enunciado) ---
// El archivo de inicio solo identifica territorios por su codigo oficial
// ("1.1".."6.4"), asi que esta es la unica funcion que arma la vecindad real
// del tablero; limpia primero cualquier vecino previo.
//
// Tabla de codigos usada (segun el enunciado):
//  1.1 Alaska, 1.2 Alberta, 1.3 America_Central, 1.4 Estados_Unidos_Oriente,
//  1.5 Groenlandia, 1.6 Territorio_Noroccidental, 1.7 Ontario, 1.8 Quebec,
//  1.9 Estados_Unidos_Occidente
//  2.1 Argentina, 2.2 Brasil, 2.3 Peru, 2.4 Venezuela
//  3.1 Gran_Bretana, 3.2 Islandia, 3.3 Europa_del_Norte, 3.4 Escandinavia,
//  3.5 Europa_del_Sur, 3.6 Ucrania(Europa_Oriental), 3.7 Europa_Occidental
//  4.1 Congo, 4.2 Africa_Oriental, 4.3 Egipto, 4.4 Madagascar,
//  4.5 Africa_del_Norte, 4.6 Africa_del_Sur(Sudafrica)
//  5.1 Afganistan, 5.2 China, 5.3 India, 5.4 Irkutsk, 5.5 Japon,
//  5.6 Kamchatka, 5.7 Oriente_Medio(Medio_Oriente), 5.8 Mongolia,
//  5.9 Tailandia(Siam), 5.10 Siberia, 5.11 Ural, 5.12 Rusia(Yakutsk)
//  6.1 Australia_Oriental, 6.2 Indonesia, 6.3 Nueva_Guinea, 6.4 Australia_Occidental
void configurarMapaPorCodigoOficial(Juego& juego) {
    for (int i = 0; i < TOTAL_TERRITORIOS; i++) {
        limpiarVecinosTerritorio(juego.territorios[i]);
    }

    // --- AMERICA DEL NORTE ---
    conectarAdyacencia(juego, "1.1", "1.6");
    conectarAdyacencia(juego, "1.1", "1.2");
    conectarAdyacencia(juego, "1.1", "5.6"); // Intercontinental (Kamchatka)

    conectarAdyacencia(juego, "1.6", "1.2");
    conectarAdyacencia(juego, "1.6", "1.7");
    conectarAdyacencia(juego, "1.6", "1.5");

    conectarAdyacencia(juego, "1.2", "1.7");
    conectarAdyacencia(juego, "1.2", "1.9");

    conectarAdyacencia(juego, "1.5", "1.8");
    conectarAdyacencia(juego, "1.5", "1.7");
    conectarAdyacencia(juego, "1.5", "3.2"); // Intercontinental (Islandia)

    conectarAdyacencia(juego, "1.7", "1.8");
    conectarAdyacencia(juego, "1.7", "1.9");
    conectarAdyacencia(juego, "1.7", "1.4");

    conectarAdyacencia(juego, "1.8", "1.4");

    conectarAdyacencia(juego, "1.9", "1.4");
    conectarAdyacencia(juego, "1.9", "1.3");

    conectarAdyacencia(juego, "1.4", "1.3");
    conectarAdyacencia(juego, "1.3", "2.4"); // Intercontinental (Venezuela)

    // --- AMERICA DEL SUR ---
    conectarAdyacencia(juego, "2.4", "2.3");
    conectarAdyacencia(juego, "2.4", "2.2");
    conectarAdyacencia(juego, "2.3", "2.2");
    conectarAdyacencia(juego, "2.3", "2.1");
    conectarAdyacencia(juego, "2.2", "2.1");
    // NOTA IMPORTANTE (confirmar con el profesor): el mapa real de Risk conecta
    // Brasil (2.2) con Africa del Norte (4.5), pero el ejemplo del enunciado
    // dice explicitamente que Brasil "podria atacar... a Africa del Sur" (4.6).
    // Se agregan AMBAS aristas intercontinentales como resguardo, mientras se
    // confirma cual exige la catedra.
    conectarAdyacencia(juego, "2.2", "4.5"); // Intercontinental (Africa del Norte)
    conectarAdyacencia(juego, "2.2", "4.6"); // Intercontinental (Africa del Sur) - resguardo

    // --- EUROPA ---
    conectarAdyacencia(juego, "3.2", "3.1");
    conectarAdyacencia(juego, "3.2", "3.4");

    conectarAdyacencia(juego, "3.1", "3.4");
    conectarAdyacencia(juego, "3.1", "3.3");
    conectarAdyacencia(juego, "3.1", "3.7");

    conectarAdyacencia(juego, "3.4", "3.3");
    conectarAdyacencia(juego, "3.4", "3.6");

    conectarAdyacencia(juego, "3.3", "3.7");
    conectarAdyacencia(juego, "3.3", "3.5");
    conectarAdyacencia(juego, "3.3", "3.6");

    conectarAdyacencia(juego, "3.7", "3.5");
    conectarAdyacencia(juego, "3.7", "4.5"); // Intercontinental

    conectarAdyacencia(juego, "3.5", "3.6");
    conectarAdyacencia(juego, "3.5", "4.5"); // Intercontinental
    conectarAdyacencia(juego, "3.5", "4.3"); // Intercontinental (Egipto)
    conectarAdyacencia(juego, "3.5", "5.7"); // Intercontinental (Medio Oriente)

    conectarAdyacencia(juego, "3.6", "5.11"); // Intercontinental (Ural)
    conectarAdyacencia(juego, "3.6", "5.1");  // Intercontinental (Afganistan)
    conectarAdyacencia(juego, "3.6", "5.7");  // Intercontinental (Medio Oriente)

    // --- AFRICA ---
    conectarAdyacencia(juego, "4.5", "4.3");
    conectarAdyacencia(juego, "4.5", "4.2");
    conectarAdyacencia(juego, "4.5", "4.1");

    conectarAdyacencia(juego, "4.3", "5.7"); // Intercontinental
    conectarAdyacencia(juego, "4.3", "4.2");

    conectarAdyacencia(juego, "4.2", "4.1");
    conectarAdyacencia(juego, "4.2", "4.6");
    conectarAdyacencia(juego, "4.2", "4.4");
    conectarAdyacencia(juego, "4.2", "5.7"); // Intercontinental

    conectarAdyacencia(juego, "4.1", "4.6");
    conectarAdyacencia(juego, "4.4", "4.6");

    // --- ASIA ---
    conectarAdyacencia(juego, "5.11", "5.10");
    conectarAdyacencia(juego, "5.11", "5.2");
    conectarAdyacencia(juego, "5.11", "5.1");

    conectarAdyacencia(juego, "5.10", "5.12");
    conectarAdyacencia(juego, "5.10", "5.4");
    conectarAdyacencia(juego, "5.10", "5.8");
    conectarAdyacencia(juego, "5.10", "5.2");

    conectarAdyacencia(juego, "5.12", "5.4");
    conectarAdyacencia(juego, "5.12", "5.6");

    conectarAdyacencia(juego, "5.4", "5.6");
    conectarAdyacencia(juego, "5.4", "5.8");

    conectarAdyacencia(juego, "5.6", "5.5");
    conectarAdyacencia(juego, "5.6", "5.8");

    conectarAdyacencia(juego, "5.5", "5.8");
    conectarAdyacencia(juego, "5.8", "5.2");

    conectarAdyacencia(juego, "5.1", "5.2");
    conectarAdyacencia(juego, "5.1", "5.3");
    conectarAdyacencia(juego, "5.1", "5.7");

    conectarAdyacencia(juego, "5.2", "5.3");
    conectarAdyacencia(juego, "5.2", "5.9");

    conectarAdyacencia(juego, "5.7", "5.3");
    conectarAdyacencia(juego, "5.3", "5.9");
    conectarAdyacencia(juego, "5.9", "6.2"); // Intercontinental (Indonesia)

    // --- OCEANIA ---
    conectarAdyacencia(juego, "6.2", "6.3");
    conectarAdyacencia(juego, "6.2", "6.4");
    conectarAdyacencia(juego, "6.3", "6.4");
    conectarAdyacencia(juego, "6.3", "6.1");
    conectarAdyacencia(juego, "6.4", "6.1");
}

// --- MAPA GENERICO DE RESPALDO (ultimo recurso) ---
// Si el archivo de partida no usa ni los nombres reales ni los codigos
// oficiales "N.M" (por ejemplo, identificadores propios de otra prueba),
// ninguna de las dos funciones anteriores logra conectar nada y el mapa queda
// sin ninguna adyacencia, lo que impide atacar o fortificar. Esta funcion
// construye una vecindad generica a partir del continente ya asignado a cada
// territorio (sin importar su nombre): conecta entre si todos los territorios
// de un mismo continente y agrega un puente entre continentes consecutivos,
// para que el grafo quede conexo.
void configurarMapaGenerico(Juego& juego) {
    for (int i = 0; i < TOTAL_TERRITORIOS; i++) {
        limpiarVecinosTerritorio(juego.territorios[i]);
    }

    for (int c = 0; c < TOTAL_CONTINENTES; c++) {
        vector<int> miembros;
        for (int t = 0; t < TOTAL_TERRITORIOS; t++) {
            if (obtenerContinenteTerritorio(juego.territorios[t]) == c) {
                miembros.push_back(t);
            }
        }
        // Todos los territorios de un mismo continente son vecinos entre si
        for (size_t i = 0; i < miembros.size(); i++) {
            for (size_t k = i + 1; k < miembros.size(); k++) {
                int a = miembros[i];
                int b = miembros[k];
                if (!esVecinoTerritorio(juego.territorios[a], b)) agregarVecinoTerritorio(juego.territorios[a], b);
                if (!esVecinoTerritorio(juego.territorios[b], a)) agregarVecinoTerritorio(juego.territorios[b], a);
            }
        }
    }

    // Puente entre continentes consecutivos (en el orden en que tienen territorios)
    vector<int> primerDeContinente(TOTAL_CONTINENTES, -1);
    vector<int> ultimoDeContinente(TOTAL_CONTINENTES, -1);
    for (int t = 0; t < TOTAL_TERRITORIOS; t++) {
        int c = obtenerContinenteTerritorio(juego.territorios[t]);
        if (c < 0 || c >= TOTAL_CONTINENTES) continue;
        if (primerDeContinente[c] == -1) primerDeContinente[c] = t;
        ultimoDeContinente[c] = t;
    }
    int continenteAnterior = -1;
    for (int c = 0; c < TOTAL_CONTINENTES; c++) {
        if (primerDeContinente[c] == -1) continue; // sin territorios en este archivo
        if (continenteAnterior != -1) {
            int a = ultimoDeContinente[continenteAnterior];
            int b = primerDeContinente[c];
            if (!esVecinoTerritorio(juego.territorios[a], b)) agregarVecinoTerritorio(juego.territorios[a], b);
            if (!esVecinoTerritorio(juego.territorios[b], a)) agregarVecinoTerritorio(juego.territorios[b], a);
        }
        continenteAnterior = c;
    }
}

// --- IMPLEMENTACION DE OPERACIONES DEL TAD JUEGO ---

void reiniciarJuego(Juego& juego) {
    juego.jugadores.clear();
    juego.cantidadJugadores = 0;
    juego.territorios.assign(TOTAL_TERRITORIOS, Territorio());
    juego.continentes.assign(TOTAL_CONTINENTES, Continente());
    juego.turnoActual = 0;
    juego.fase = "RECLUTAR";
    juego.inicializado = false;
    juego.terminado = false;
    juego.ganador = "";
    juego.mazo.clear();
    juego.cantidadCartasMazo = 0;
    juego.canjesRealizados = 0;
}

bool estaInicializadoJuego(const Juego& juego) {
    return juego.inicializado;
}

bool estaTerminadoJuego(const Juego& juego) {
    return juego.terminado;
}

int obtenerCantidadJugadoresJuego(const Juego& juego) {
    return juego.cantidadJugadores;
}

int obtenerTurnoActualJuego(const Juego& juego) {
    return juego.turnoActual;
}

const string& obtenerFaseActualJuego(const Juego& juego) {
    return juego.fase;
}

const string& obtenerGanadorJuego(const Juego& juego) {
    return juego.ganador;
}

Jugador& obtenerJugadorRef(Juego& juego, int indice) {
    return juego.jugadores[indice];
}

const Jugador& obtenerJugadorConst(const Juego& juego, int indice) {
    return juego.jugadores[indice];
}

Territorio& obtenerTerritorioRef(Juego& juego, int indice) {
    return juego.territorios[indice];
}

const Territorio& obtenerTerritorioConst(const Juego& juego, int indice) {
    return juego.territorios[indice];
}

Continente& obtenerContinenteRef(Juego& juego, int indice) {
    return juego.continentes[indice];
}

const Continente& obtenerContinenteConst(const Juego& juego, int indice) {
    return juego.continentes[indice];
}

int buscarIndiceJugador(const Juego& juego, const string& nombre) {
    for (int i = 0; i < juego.cantidadJugadores; i++) {
        if (obtenerNombreJugador(juego.jugadores[i]) == nombre) {
            return i;
        }
    }
    return -1;
}

int buscarIndiceTerritorio(const Juego& juego, const string& nombre) {
    for (int i = 0; i < TOTAL_TERRITORIOS; i++) {
        if (obtenerNombreTerritorio(juego.territorios[i]) == nombre) {
            return i;
        }
    }
    return -1;
}

void asignarFaseJuego(Juego& juego, const string& nuevaFase) {
    juego.fase = nuevaFase;
}

void asignarGanadorJuego(Juego& juego, const string& ganador) {
    juego.ganador = ganador;
}

void pasarSiguienteTurno(Juego& juego) {
    do {
        juego.turnoActual = (juego.turnoActual + 1) % juego.cantidadJugadores;
    } while (!estaActivoJugador(juego.jugadores[juego.turnoActual]));
    asignarFaseJuego(juego, "RECLUTAR");
}

int contarTerritoriosControlados(const Juego& juego, const string& color) {
    int conteo = 0;
    for (int i = 0; i < TOTAL_TERRITORIOS; i++) {
        if (perteneceAJugador(juego.territorios[i], color)) {
            conteo++;
        }
    }
    return conteo;
}

int calcularBonoContinentesTotal(const Juego& juego, const string& color) {
    int bonoTotal = 0;
    for (int c = 0; c < TOTAL_CONTINENTES; c++) {
        bool continenteControlado = true;
        int countTerritorios = 0;
        for (int t = 0; t < TOTAL_TERRITORIOS; t++) {
            if (obtenerContinenteTerritorio(juego.territorios[t]) == c) {
                countTerritorios++;
                if (!perteneceAJugador(juego.territorios[t], color)) {
                    continenteControlado = false;
                    break;
                }
            }
        }
        if (continenteControlado && countTerritorios > 0) {
            bonoTotal += obtenerBonoContinente(juego.continentes[c]);
        }
    }
    return bonoTotal;
}

// --- IMPLEMENTACION DE COMANDOS (CONTROLADOR DEL JUEGO) ---

// Limites de validacion del archivo de partida (no son limites estructurales
// del TAD Jugador/Juego, que usan vectores sin limite fijo; son solo cotas de
// sensatez para detectar archivos corruptos o maliciosos sin formato).
const int MAX_CARTAS_SANO = 60;
const int MAX_CARTAS_JUGADOR_SANO = 60;

// --- LECTOR DE PALABRAS CON "DEVOLUCION" ---
// Permite leer una palabra del archivo y, si no era lo que se esperaba,
// devolverla para que la siguiente lectura la vuelva a entregar. Asi se puede
// distinguir el formato basico (sin cartas) del formato extendido (con cartas).
struct LectorPalabras {
    istream* archivo;
    string pendiente;
    bool hayPendiente;
};

bool leerPalabra(LectorPalabras& lector, string& destino) {
    if (lector.hayPendiente) {
        destino = lector.pendiente;
        lector.hayPendiente = false;
        return true;
    }
    if (!(*lector.archivo >> destino)) {
        return false;
    }
    return true;
}

void devolverPalabra(LectorPalabras& lector, const string& palabra) {
    lector.pendiente = palabra;
    lector.hayPendiente = true;
}

// Retorna true si la cadena es un entero no negativo (solo digitos)
bool esEnteroNoNegativo(const string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (c < '0' || c > '9') return false;
    }
    return true;
}

int convertirEntero(const string& s) {
    int valor = 0;
    for (char c : s) {
        valor = valor * 10 + (c - '0');
    }
    return valor;
}

bool esTipoCartaValido(const string& tipo) {
    return tipo == "Infanteria" || tipo == "Caballeria" ||
           tipo == "Artilleria" || tipo == "Comodin";
}

// Lee una carta "<id> <territorio> <tipo>". Retorna false si el formato es invalido.
bool leerCarta(LectorPalabras& lector, Carta& carta) {
    string idTxt, territorio, tipo;
    if (!leerPalabra(lector, idTxt) ||
        !leerPalabra(lector, territorio) ||
        !leerPalabra(lector, tipo)) {
        return false;
    }
    if (!esEnteroNoNegativo(idTxt) || !esTipoCartaValido(tipo)) {
        return false;
    }
    inicializarCarta(carta, convertirEntero(idTxt), territorio, tipo);
    return true;
}

// FORMATOS ACEPTADOS POR "inicializar":
//
// 1) Formato basico (el original, y el canonico segun el enunciado: el
//    identificador de cada territorio es el codigo oficial "N.M" de la Tabla 1,
//    aunque tambien se aceptan los nombres reales usados en partidas de prueba):
//      <numJugadores>
//      <nombre> <color>                       (una linea por jugador)
//      <territorio> <color> <unidades>        (42 lineas)
//    Se valida la cuota inicial de unidades (35/30/25/20 segun jugadores).
//
// 2) Formato extendido (estado de partida, igual al que produce guardar_juego):
//      <numJugadores>
//      <nombre> <color> <numCartas> [<id> <territorio> <tipo>]...
//      <numCartasMazo>
//      <id> <territorio> <tipo>               (una linea por carta del mazo)
//      <territorio> <color> <unidades>        (42 lineas)
//    Como puede representar una partida ya avanzada, NO se exige la cuota
//    inicial de unidades; solo que cada territorio tenga al menos 1 unidad.
//
// En ambos formatos, el nombre de cada jugador no puede superar los 8
// caracteres (limite del TAD Jugador segun el enunciado).
void inicializarDesdeFlujo(istream& archivo, const string& nombreArchivo) {
    LectorPalabras lector;
    lector.archivo = &archivo;
    lector.hayPendiente = false;

    string palabra;
    int numJugadores;
    vector<Jugador> tempJugadores;
    vector<Carta> tempMazo;
    int tempCantMazo = 0;
    bool formatoExtendido = false;

    if (!leerPalabra(lector, palabra) || !esEnteroNoNegativo(palabra)) {
        cout << "(Archivo sin formato) " << nombreArchivo << " no contiene informacion en el formato esperado." << endl;
        return;
    }
    numJugadores = convertirEntero(palabra);
    if (numJugadores < MIN_JUGADORES || numJugadores > MAX_JUGADORES_JUEGO) {
        cout << "(Archivo sin formato) " << nombreArchivo << " no contiene informacion en el formato esperado." << endl;
        return;
    }

    tempJugadores.resize(numJugadores);

    for (int i = 0; i < numJugadores; i++) {
        string nombreJugador, colorJugador;
        if (!leerPalabra(lector, nombreJugador) ||
            !leerPalabra(lector, colorJugador)) {
            cout << "(Archivo sin formato) " << nombreArchivo << " no contiene informacion en el formato esperado." << endl;
                return;
        }

        // Limite de 8 caracteres para el nombre del jugador (TAD Jugador)
        if (nombreJugador.size() > 8) {
            cout << "(Archivo sin formato) " << nombreArchivo << ": el nombre de jugador \"" << nombreJugador
                 << "\" supera el maximo de 8 caracteres." << endl;
                return;
        }

        // Validacion de nombres y colores repetidos entre jugadores
        for (int k = 0; k < i; k++) {
            if (obtenerNombreJugador(tempJugadores[k]) == nombreJugador ||
                obtenerColorJugador(tempJugadores[k]) == colorJugador) {
                cout << "(Archivo sin formato) " << nombreArchivo << " contiene jugadores con nombres o colores duplicados." << endl;
                        return;
            }
        }

        inicializarJugador(tempJugadores[i], nombreJugador, colorJugador);

        // Si despues del color viene un numero, es la cantidad de cartas del jugador
        if (leerPalabra(lector, palabra)) {
            if (esEnteroNoNegativo(palabra)) {
                formatoExtendido = true;
                int numCartas = convertirEntero(palabra);
                if (numCartas > MAX_CARTAS_JUGADOR_SANO) {
                    cout << "(Archivo sin formato) " << nombreArchivo << " contiene demasiadas cartas para el jugador " << nombreJugador << "." << endl;
                                return;
                }
                for (int c = 0; c < numCartas; c++) {
                    Carta carta;
                    if (!leerCarta(lector, carta)) {
                        cout << "(Archivo sin formato) " << nombreArchivo << " contiene cartas invalidas para el jugador " << nombreJugador << "." << endl;
                                        return;
                    }
                    agregarCartaJugador(tempJugadores[i], carta);
                }
            } else {
                devolverPalabra(lector, palabra); // Formato basico: era el siguiente jugador o territorio
            }
        }
    }

    // Mazo de cartas: solo existe en el formato extendido (un numero tras los jugadores)
    if (leerPalabra(lector, palabra)) {
        if (esEnteroNoNegativo(palabra)) {
            formatoExtendido = true;
            tempCantMazo = convertirEntero(palabra);
            if (tempCantMazo > MAX_CARTAS_SANO) {
                cout << "(Archivo sin formato) " << nombreArchivo << " contiene demasiadas cartas en el mazo." << endl;
                        return;
            }
            tempMazo.resize(tempCantMazo);
            for (int c = 0; c < tempCantMazo; c++) {
                if (!leerCarta(lector, tempMazo[c])) {
                    cout << "(Archivo sin formato) " << nombreArchivo << " contiene cartas del mazo invalidas." << endl;
                                return;
                }
            }
        } else {
            devolverPalabra(lector, palabra);
        }
    }

    vector<int> conteoUnidades(numJugadores, 0);
    vector<Territorio> tempTerritorios(TOTAL_TERRITORIOS);
    int territoriosLeidos = 0;

    // Configuracion real oficial de Risk para tamanos y bonos de continentes:
    int tamanioContinentes[6] = {9, 4, 7, 6, 12, 4};
    int bonosContinentes[6]   = {5, 2, 5, 3,  7, 2};
    vector<string> nombresContinentes = {
        "America del Norte", "America del Sur", "Europa", "Africa", "Asia", "Australia"
    };

    for (int c = 0; c < TOTAL_CONTINENTES; c++) {
        inicializarContinente(partidaActual.continentes[c], c, nombresContinentes[c], "", tamanioContinentes[c], bonosContinentes[c]);
    }

    while (territoriosLeidos < TOTAL_TERRITORIOS) {
        string nombreT, colorT, unidadesTxt;

        if (!leerPalabra(lector, nombreT) ||
            !leerPalabra(lector, colorT) ||
            !leerPalabra(lector, unidadesTxt) ||
            !esEnteroNoNegativo(unidadesTxt)) {
            cout << "(Archivo sin formato) " << nombreArchivo << " no contiene informacion en el formato esperado." << endl;
                return;
        }
        int unidadesT = convertirEntero(unidadesTxt);

        if (unidadesT <= 0) {
            cout << "(Archivo sin formato) " << nombreArchivo << " contiene territorios con unidades invalidas." << endl;
                return;
        }

        // Validacion de nombres de territorio unicos (no repetidos)
        for (int k = 0; k < territoriosLeidos; k++) {
            if (obtenerNombreTerritorio(tempTerritorios[k]) == nombreT) {
                cout << "(Archivo sin formato) " << nombreArchivo << " contiene nombres de territorios duplicados (" << nombreT << ")." << endl;
                        return;
            }
        }

        // Asignacion de continente por nombre real y validacion estricta de nombre de territorio
        int idContinente = determinarContinentePorCodigo(nombreT);
        if (idContinente == -1) {
            cout << "(Archivo sin formato) " << nombreArchivo << " contiene un territorio desconocido (" << nombreT << ")." << endl;
                return;
        }

        int codigoTerritorio = territoriosLeidos + 1; // Codigo identificador unico oficial (1 al 42)

        inicializarTerritorio(tempTerritorios[territoriosLeidos], codigoTerritorio, nombreT, idContinente, colorT, unidadesT);

        bool colorEncontrado = false;
        for (int i = 0; i < numJugadores; i++) {
            if (obtenerColorJugador(tempJugadores[i]) == colorT) {
                conteoUnidades[i] += unidadesT;
                colorEncontrado = true;
                break;
            }
        }

        if (!colorEncontrado) {
            cout << "(Archivo sin formato) " << nombreArchivo << " no contiene informacion en el formato esperado." << endl;
                return;
        }

        territoriosLeidos++;
    }

    if (!formatoExtendido) {
        // Validacion de cuota inicial de unidades segun el numero de jugadores
        // 3 jugadores -> 35 c/u, 4 -> 30 c/u, 5 -> 25 c/u, 6 -> 20 c/u
        int cuotaEsperada = 35 - 5 * (numJugadores - 3);
        for (int i = 0; i < numJugadores; i++) {
            if (conteoUnidades[i] != cuotaEsperada) {
                cout << "(Archivo sin formato) " << nombreArchivo << ": El jugador "
                     << obtenerNombreJugador(tempJugadores[i]) << " tiene " << conteoUnidades[i]
                     << " unidades (se esperaban exactamente " << cuotaEsperada << ")." << endl;
                        return;
            }
        }
    } else {
        // Validacion de cartas: ids unicos y territorios existentes
        vector<int> ids;
        for (int i = 0; i <= numJugadores; i++) {
            int cant = (i < numJugadores) ? obtenerCantidadCartasJugador(tempJugadores[i]) : tempCantMazo;
            for (int c = 0; c < cant; c++) {
                const Carta& carta = (i < numJugadores) ? obtenerCartaJugador(tempJugadores[i], c) : tempMazo[c];
                for (size_t k = 0; k < ids.size(); k++) {
                    if (ids[k] == obtenerIdCarta(carta)) {
                        cout << "(Archivo sin formato) " << nombreArchivo << " contiene la carta "
                             << obtenerIdCarta(carta) << " repetida." << endl;
                                        return;
                    }
                }
                ids.push_back(obtenerIdCarta(carta));

                bool territorioExiste = false;
                for (int t = 0; t < TOTAL_TERRITORIOS; t++) {
                    if (obtenerNombreTerritorio(tempTerritorios[t]) == obtenerTerritorioCarta(carta)) {
                        territorioExiste = true;
                        break;
                    }
                }
                if (!territorioExiste) {
                    cout << "(Archivo sin formato) " << nombreArchivo << " contiene la carta "
                         << obtenerIdCarta(carta) << " con un territorio desconocido ("
                         << obtenerTerritorioCarta(carta) << ")." << endl;
                                return;
                }
            }
        }
    }

    // Debe haber al menos dos jugadores con territorios para que la partida tenga sentido
    int jugadoresConTerritorios = 0;
    for (int i = 0; i < numJugadores; i++) {
        if (conteoUnidades[i] > 0) jugadoresConTerritorios++;
    }
    if (jugadoresConTerritorios < 2) {
        cout << "(Archivo sin formato) " << nombreArchivo << ": la partida ya tiene un unico jugador con territorios." << endl;
        return;
    }

    partidaActual.cantidadJugadores = numJugadores;
    partidaActual.jugadores.assign(tempJugadores.begin(), tempJugadores.end());
    for (int i = 0; i < numJugadores; i++) {
        // Un jugador sin territorios en un estado guardado ya fue eliminado
        if (conteoUnidades[i] == 0) {
            setActivoJugador(partidaActual.jugadores[i], false);
        }
    }
    for (int i = 0; i < TOTAL_TERRITORIOS; i++) {
        partidaActual.territorios[i] = tempTerritorios[i];
    }
    partidaActual.cantidadCartasMazo = tempCantMazo;
    partidaActual.mazo.assign(tempMazo.begin(), tempMazo.end());
    partidaActual.canjesRealizados = 0;

    // Configurar el grafo de adyacencias reales del tablero oficial de Risk,
    // indexado por el codigo oficial "N.M" (unico identificador aceptado).
    configurarMapaPorCodigoOficial(partidaActual);

    // Si por alguna razon no se conecto ninguna adyacencia (no deberia ocurrir,
    // ya que todo codigo valido "N.M" esta cubierto arriba), se usa el mapa
    // generico de respaldo basado unicamente en el continente de cada territorio.
    {
        bool sinNingunaAdyacencia = true;
        for (int i = 0; i < TOTAL_TERRITORIOS; i++) {
            if (obtenerCantidadVecinosTerritorio(partidaActual.territorios[i]) > 0) {
                sinNingunaAdyacencia = false;
                break;
            }
        }
        if (sinNingunaAdyacencia) {
            configurarMapaGenerico(partidaActual);
        }
    }

    partidaActual.turnoActual = 0;
    while (!estaActivoJugador(partidaActual.jugadores[partidaActual.turnoActual])) {
        partidaActual.turnoActual++;
    }
    asignarFaseJuego(partidaActual, "RECLUTAR");
    partidaActual.inicializado = true;
    partidaActual.terminado = false;
    asignarGanadorJuego(partidaActual, "");

    cout << "(Comando correcto) El juego se ha inicializado correctamente." << endl;
}

// Retorna true si el nombre del archivo termina en ".bin"
bool tieneExtensionBin(const string& nombreArchivo) {
    size_t largo = nombreArchivo.size();
    return largo >= 4 && nombreArchivo.compare(largo - 4, 4, ".bin") == 0;
}

// Inicializa el juego desde un archivo de texto (generado por guardar) o desde
// un archivo binario comprimido con Huffman (generado por guardar_comprimido).
void Inicializar(const string& nombreArchivo) {
    if (estaInicializadoJuego(partidaActual)) {
        cout << "(Juego en curso) El juego ya ha sido inicializado." << endl;
        return;
    }

    reiniciarJuego(partidaActual);

    if (tieneExtensionBin(nombreArchivo)) {
        char* texto = nullptr;
        unsigned long long largo = 0;
        int resultado = descomprimirHuffman(nombreArchivo.c_str(), texto, largo);
        if (resultado == HUFFMAN_NO_ABRE) {
            cout << "(Archivo erroneo) " << nombreArchivo << " no se encuentra o no puede leerse." << endl;
            return;
        }
        if (resultado == HUFFMAN_VACIO) {
            cout << "(Archivo vacio) " << nombreArchivo << " no contiene informacion." << endl;
            return;
        }
        if (resultado != HUFFMAN_OK) {
            cout << "(Archivo sin formato) " << nombreArchivo << " no contiene informacion en el formato esperado." << endl;
            return;
        }
        string textoStr(texto);
        delete[] texto;
        istringstream flujo(textoStr);
        inicializarDesdeFlujo(flujo, nombreArchivo);
        return;
    }

    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "(Archivo erroneo) " << nombreArchivo << " no se encuentra o no puede leerse." << endl;
        return;
    }

    archivo.seekg(0, ios::end);
    if (archivo.tellg() == 0) {
        cout << "(Archivo vacio) " << nombreArchivo << " no contiene informacion." << endl;
        return;
    }
    archivo.seekg(0, ios::beg);

    inicializarDesdeFlujo(archivo, nombreArchivo);
}

void ObtenerUnidades(const string& nombreJugador) {
    if (!estaInicializadoJuego(partidaActual)) {
        cout << "(Juego no inicializado) Esta partida no ha sido inicializada correctamente." << endl;
        return;
    }

    if (estaTerminadoJuego(partidaActual)) {
        cout << "(Juego terminado) Esta partida ya tuvo un ganador." << endl;
        return;
    }

    int indiceJugador = buscarIndiceJugador(partidaActual, nombreJugador);
    if (indiceJugador == -1) {
        cout << "(Jugador no valido) El jugador " << nombreJugador << " no forma parte de esta partida." << endl;
        return;
    }

    if (obtenerTurnoActualJuego(partidaActual) != indiceJugador) {
        cout << "(Jugador fuera de turno) No es el turno del jugador " << nombreJugador << "." << endl;
        return;
    }

    if (obtenerFaseActualJuego(partidaActual) != "RECLUTAR") {
        cout << "(Jugador fuera de turno) No se encuentra en la fase de reclutamiento." << endl;
        return;
    }

    Jugador& jActual = obtenerJugadorRef(partidaActual, indiceJugador);
    const string& colorJ = obtenerColorJugador(jActual);

    int cantTerritoriosOcupados = contarTerritoriosControlados(partidaActual, colorJ);
    int unidadesCalculadas = cantTerritoriosOcupados / 3;
    if (unidadesCalculadas < 3) {
        unidadesCalculadas = 3;
    }

    int bonoContinentesTotal = calcularBonoContinentesTotal(partidaActual, colorJ);
    int bonoCanje = obtenerBonoCanjeJugador(jActual);
    int totalUnidadesAReclamar = unidadesCalculadas + bonoContinentesTotal + bonoCanje;
    asignarUnidadesPendientesJugador(jActual, totalUnidadesAReclamar);
    asignarBonoCanjeJugador(jActual, 0);

    cout << "El jugador " << obtenerNombreJugador(jActual) << " reclama " << totalUnidadesAReclamar
         << " unidades (" << unidadesCalculadas << " por territorios, "
         << bonoContinentesTotal << " por continentes y " << bonoCanje << " por canje de cartas)." << endl;

    while (obtenerUnidadesPendientesJugador(jActual) > 0) {
        if (cin.eof()) {
            return;
        }

        cout << "Unidades pendientes por ubicar: " << obtenerUnidadesPendientesJugador(jActual) << endl;
        cout << "Ingrese el nombre del territorio donde desea colocar unidades: ";

        string nombreTerritorio;
        if (!(cin >> nombreTerritorio)) {
            if (cin.eof()) return;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        int unidadesAColocar;
        cout << "Ingrese la cantidad de unidades a colocar: ";
        if (!(cin >> unidadesAColocar)) {
            if (cin.eof()) return;
            cout << "Entrada invalida. Intente de nuevo." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        if (unidadesAColocar <= 0 || unidadesAColocar > obtenerUnidadesPendientesJugador(jActual)) {
            cout << "Error: Cantidad de unidades invalida. Tiene " << obtenerUnidadesPendientesJugador(jActual) << " disponibles." << endl;
            continue;
        }

        int idxT = buscarIndiceTerritorio(partidaActual, nombreTerritorio);
        if (idxT == -1) {
            cout << "Error: El territorio ingresado no existe." << endl;
            continue;
        }

        Territorio& tDestino = obtenerTerritorioRef(partidaActual, idxT);
        if (perteneceAJugador(tDestino, colorJ)) {
            modificarUnidadesTerritorio(tDestino, unidadesAColocar);
            modificarUnidadesPendientesJugador(jActual, -unidadesAColocar);
            cout << "Unidades ubicadas exitosamente en " << nombreTerritorio << "." << endl;
        } else {
            cout << "Error: El territorio " << nombreTerritorio << " no le pertenece." << endl;
        }
    }

    asignarFaseJuego(partidaActual, "ATACAR");
    cout << "(Comando correcto) El jugador " << nombreJugador << " ha terminado de reclamar y ubicar sus unidades." << endl;
}

void ordenarDescendente(vector<int>& arr) {
    for (size_t i = 0; i + 1 < arr.size(); i++) {
        for (size_t j = 0; j + 1 < arr.size() - i; j++) {
            if (arr[j] < arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void Atacar(const string& nombreJugador) {
    if (!estaInicializadoJuego(partidaActual)) {
        cout << "(Juego no inicializado) Esta partida no ha sido inicializada correctamente." << endl;
        return;
    }

    if (estaTerminadoJuego(partidaActual)) {
        cout << "(Juego terminado) Esta partida ya tuvo un ganador." << endl;
        return;
    }

    int indiceJugador = buscarIndiceJugador(partidaActual, nombreJugador);
    if (indiceJugador == -1) {
        cout << "(Jugador no valido) El jugador " << nombreJugador << " no forma parte de esta partida." << endl;
        return;
    }

    Jugador& jActual = obtenerJugadorRef(partidaActual, indiceJugador);
    if (!estaActivoJugador(jActual)) {
        cout << "(Jugador no valido) El jugador " << nombreJugador << " ya ha sido eliminado de la partida." << endl;
        return;
    }

    if (obtenerTurnoActualJuego(partidaActual) != indiceJugador) {
        cout << "(Jugador fuera de turno) No es el turno del jugador " << nombreJugador << "." << endl;
        return;
    }

    if (obtenerFaseActualJuego(partidaActual) != "ATACAR") {
        cout << "(Jugador no ha ubicado unidades) El jugador " << nombreJugador << " no ha ejecutado el comando obtener_unidades." << endl;
        return;
    }

    const string& colorJ = obtenerColorJugador(jActual);

    char continuarAtacando = 's';
    while (continuarAtacando == 's' || continuarAtacando == 'S') {
        if (cin.eof()) return;

        cout << "\n--- FASE DE ATAQUE ---" << endl;
        cout << "Ingrese el nombre del territorio de origen (propio): ";
        string nombreOrigen;
        if (!(cin >> nombreOrigen)) {
            if (cin.eof()) return;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        cout << "Ingrese el nombre del territorio de destino (enemigo vecino): ";
        string nombreDestino;
        if (!(cin >> nombreDestino)) {
            if (cin.eof()) return;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        int idxOrigen = buscarIndiceTerritorio(partidaActual, nombreOrigen);
        int idxDestino = buscarIndiceTerritorio(partidaActual, nombreDestino);

        if (idxOrigen == -1 || idxDestino == -1) {
            cout << "Error: Uno o ambos territorios ingresados no existen." << endl;
            cout << "Desea intentar otro ataque? (s/n): ";
            if (!(cin >> continuarAtacando)) {
                if (cin.eof()) return;
                cin.clear();
                cin.ignore(10000, '\n');
            }
            continue;
        }

        Territorio& tOrigen = obtenerTerritorioRef(partidaActual, idxOrigen);
        Territorio& tDestino = obtenerTerritorioRef(partidaActual, idxDestino);

        if (!perteneceAJugador(tOrigen, colorJ)) {
            cout << "Error: El territorio de origen no le pertenece." << endl;
            cout << "Desea intentar otro ataque? (s/n): ";
            if (!(cin >> continuarAtacando)) {
                if (cin.eof()) return;
                cin.clear();
                cin.ignore(10000, '\n');
            }
            continue;
        }

        if (obtenerUnidadesTerritorio(tOrigen) < 2) {
            cout << "Error: El territorio de origen debe tener al menos 2 unidades para atacar (tiene "
                 << obtenerUnidadesTerritorio(tOrigen) << ")." << endl;
            cout << "Desea intentar otro ataque? (s/n): ";
            if (!(cin >> continuarAtacando)) {
                if (cin.eof()) return;
                cin.clear();
                cin.ignore(10000, '\n');
            }
            continue;
        }

        if (perteneceAJugador(tDestino, colorJ)) {
            cout << "Error: No puede atacar un territorio propio." << endl;
            cout << "Desea intentar otro ataque? (s/n): ";
            if (!(cin >> continuarAtacando)) {
                if (cin.eof()) return;
                cin.clear();
                cin.ignore(10000, '\n');
            }
            continue;
        }

        // VALIDACION DE VECINDAD / ADYACENCIA REAL DEL TABLERO
        if (!esVecinoTerritorio(tOrigen, idxDestino)) {
            cout << "Error: El territorio " << obtenerNombreTerritorio(tDestino)
                 << " no es vecino de " << obtenerNombreTerritorio(tOrigen) << "." << endl;
            cout << "Desea intentar otro ataque? (s/n): ";
            if (!(cin >> continuarAtacando)) {
                if (cin.eof()) return;
                cin.clear();
                cin.ignore(10000, '\n');
            }
            continue;
        }

        while (obtenerUnidadesTerritorio(tOrigen) >= 2 && obtenerUnidadesTerritorio(tDestino) > 0) {
            if (cin.eof()) return;

            cout << "\nBatalla entre " << obtenerNombreTerritorio(tOrigen) << " (" << obtenerUnidadesTerritorio(tOrigen) << " unidades) y "
                 << obtenerNombreTerritorio(tDestino) << " (" << obtenerUnidadesTerritorio(tDestino) << " unidades)." << endl;

            int dadosAtacante = obtenerUnidadesTerritorio(tOrigen) - 1;
            if (dadosAtacante > 3) dadosAtacante = 3;

            int dadosDefensor = obtenerUnidadesTerritorio(tDestino);
            if (dadosDefensor > 2) dadosDefensor = 2;

            vector<int> arrAtacante(dadosAtacante);
            vector<int> arrDefensor(dadosDefensor);

            cout << "Dados Atacante: ";
            for (int i = 0; i < dadosAtacante; i++) {
                arrAtacante[i] = (rand() % 6) + 1;
                cout << arrAtacante[i] << " ";
            }
            cout << endl;

            cout << "Dados Defensor: ";
            for (int i = 0; i < dadosDefensor; i++) {
                arrDefensor[i] = (rand() % 6) + 1;
                cout << arrDefensor[i] << " ";
            }
            cout << endl;

            ordenarDescendente(arrAtacante);
            ordenarDescendente(arrDefensor);

            int enfrentamientos = (dadosAtacante < dadosDefensor) ? dadosAtacante : dadosDefensor;
            for (int i = 0; i < enfrentamientos; i++) {
                if (arrAtacante[i] > arrDefensor[i]) {
                    modificarUnidadesTerritorio(tDestino, -1);
                    cout << "El atacante gana el combate #" << (i + 1) << ". Defensor pierde 1 unidad." << endl;
                } else {
                    modificarUnidadesTerritorio(tOrigen, -1);
                    cout << "El defensor gana el combate #" << (i + 1) << ". Atacante pierde 1 unidad." << endl;
                }
            }

            if (obtenerUnidadesTerritorio(tDestino) == 0) {
                cout << "\nEl territorio " << obtenerNombreTerritorio(tDestino) << " ha sido conquistado!" << endl;

                // Guardar el color del defensor derrotado
                string colorDefensor = obtenerColorTerritorio(tDestino);

                asignarColorTerritorio(tDestino, colorJ);

                int unidadesTraslado;
                cout << "Cuantas unidades desea trasladar desde " << obtenerNombreTerritorio(tOrigen)
                     << " (Max: " << (obtenerUnidadesTerritorio(tOrigen) - 1) << ", Min: 1): ";

                while (true) {
                    if (!(cin >> unidadesTraslado)) {
                        if (cin.eof()) return;
                        cout << "Entrada invalida. Intente de nuevo: ";
                        cin.clear();
                        cin.ignore(10000, '\n');
                        continue;
                    }
                    if (unidadesTraslado < 1 || unidadesTraslado >= obtenerUnidadesTerritorio(tOrigen)) {
                        cout << "Cantidad invalida. Intente de nuevo: ";
                        continue;
                    }
                    break;
                }

                modificarUnidadesTerritorio(tOrigen, -unidadesTraslado);
                asignarUnidadesTerritorio(tDestino, unidadesTraslado);
                cout << "Traslado exitoso. Nuevo estado: " << obtenerNombreTerritorio(tOrigen) << " ("
                     << obtenerUnidadesTerritorio(tOrigen) << ") y "
                     << obtenerNombreTerritorio(tDestino) << " (" << obtenerUnidadesTerritorio(tDestino) << ")." << endl;

                // --- COMPROBACION DE ELIMINACION DE JUGADOR ---
                int idxDef = -1;
                for (int i = 0; i < obtenerCantidadJugadoresJuego(partidaActual); i++) {
                    if (obtenerColorJugador(obtenerJugadorConst(partidaActual, i)) == colorDefensor) {
                        idxDef = i;
                        break;
                    }
                }

                if (idxDef != -1 && estaActivoJugador(obtenerJugadorConst(partidaActual, idxDef))) {
                    if (contarTerritoriosControlados(partidaActual, colorDefensor) == 0) {
                        Jugador& jDefensor = obtenerJugadorRef(partidaActual, idxDef);
                        setActivoJugador(jDefensor, false);
                        cout << "\nATENCION! El jugador " << obtenerNombreJugador(jDefensor)
                             << " se ha quedado sin territorios y ha sido ELIMINADO de la partida." << endl;
                    }
                }

                // --- COMPROBACION INMEDIATA DE VICTORIA TRAS CONQUISTA ---
                int territoriosControlados = contarTerritoriosControlados(partidaActual, colorJ);
                if (territoriosControlados == TOTAL_TERRITORIOS) {
                    partidaActual.terminado = true;
                    asignarGanadorJuego(partidaActual, obtenerNombreJugador(jActual));
                    cout << "\nFELICIDADES! El jugador " << obtenerNombreJugador(jActual)
                         << " ha conquistado todo el mapa y ganado la partida." << endl;
                    cout << "(Comando correcto) El jugador " << nombreJugador << " ha terminado de atacar." << endl;
                    return;
                }

                break;
            }

            if (obtenerUnidadesTerritorio(tOrigen) < 2) {
                cout << "El territorio de origen se ha quedado sin suficientes unidades para continuar atacando." << endl;
                break;
            }

            cout << "Desea continuar atacando este mismo territorio? (s/n): ";
            char seguirAtacandoTerritorio;
            if (!(cin >> seguirAtacandoTerritorio)) {
                if (cin.eof()) return;
                cin.clear();
                cin.ignore(10000, '\n');
                break;
            }
            if (seguirAtacandoTerritorio != 's' && seguirAtacandoTerritorio != 'S') {
                break;
            }
        }

        cout << "Desea realizar otro ataque desde otro territorio? (s/n): ";
        if (!(cin >> continuarAtacando)) {
            if (cin.eof()) return;
            cin.clear();
            cin.ignore(10000, '\n');
            break;
        }
    }

    asignarFaseJuego(partidaActual, "FORTIFICAR");
    cout << "(Comando correcto) El jugador " << nombreJugador << " ha terminado de atacar." << endl;
}

void Fortificar(const string& nombreJugador) {
    if (!estaInicializadoJuego(partidaActual)) {
        cout << "(Juego no inicializado) Esta partida no ha sido inicializada correctamente." << endl;
        return;
    }

    if (estaTerminadoJuego(partidaActual)) {
        cout << "(Juego terminado) Esta partida ya tuvo un ganador." << endl;
        return;
    }

    int indiceJugador = buscarIndiceJugador(partidaActual, nombreJugador);
    if (indiceJugador == -1) {
        cout << "(Jugador no valido) El jugador " << nombreJugador << " no forma parte de esta partida." << endl;
        return;
    }

    Jugador& jActual = obtenerJugadorRef(partidaActual, indiceJugador);
    if (!estaActivoJugador(jActual)) {
        cout << "(Jugador no valido) El jugador " << nombreJugador << " ya ha sido eliminado de la partida." << endl;
        return;
    }

    if (obtenerTurnoActualJuego(partidaActual) != indiceJugador) {
        cout << "(Jugador fuera de turno) No es el turno del jugador " << nombreJugador << "." << endl;
        return;
    }

    if (obtenerFaseActualJuego(partidaActual) != "FORTIFICAR") {
        cout << "(Jugador no ha atacado) El jugador " << nombreJugador << " no ha ejecutado el comando atacar." << endl;
        return;
    }

    const string& colorJ = obtenerColorJugador(jActual);

    cout << "\n--- FASE DE FORTIFICACION ---" << endl;
    cout << "Ingrese el nombre del territorio de origen (propio): ";
    string nombreOrigen;
    if (!(cin >> nombreOrigen)) {
        if (cin.eof()) return;
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }

    cout << "Ingrese el nombre del territorio de destino (propio vecino): ";
    string nombreDestino;
    if (!(cin >> nombreDestino)) {
        if (cin.eof()) return;
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }

    int idxOrigen = buscarIndiceTerritorio(partidaActual, nombreOrigen);
    int idxDestino = buscarIndiceTerritorio(partidaActual, nombreDestino);

    if (idxOrigen == -1 || idxDestino == -1) {
        cout << "Error: Uno o ambos territorios ingresados no existen." << endl;
        return;
    }

    Territorio& tOrigen = obtenerTerritorioRef(partidaActual, idxOrigen);
    Territorio& tDestino = obtenerTerritorioRef(partidaActual, idxDestino);

    if (!perteneceAJugador(tOrigen, colorJ) || !perteneceAJugador(tDestino, colorJ)) {
        cout << "Error: Ambos territorios deben pertenecer al jugador " << obtenerNombreJugador(jActual) << "." << endl;
        return;
    }

    if (!esVecinoTerritorio(tOrigen, idxDestino)) {
        cout << "Error: El territorio de origen y el de destino no son vecinos entre si." << endl;
        return;
    }

    int unidadesMovimiento;
    cout << "Ingrese la cantidad de unidades a trasladar (Origen tiene " << obtenerUnidadesTerritorio(tOrigen)
         << ", debe conservar al menos 1): ";
    if (!(cin >> unidadesMovimiento)) {
        cout << "Entrada invalida." << endl;
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }

    if (unidadesMovimiento <= 0 || unidadesMovimiento >= obtenerUnidadesTerritorio(tOrigen)) {
        cout << "Error: Cantidad de unidades a mover invalida. El origen debe conservar al menos 1 unidad." << endl;
        return;
    }

    modificarUnidadesTerritorio(tOrigen, -unidadesMovimiento);
    modificarUnidadesTerritorio(tDestino, unidadesMovimiento);

    cout << "Fortificacion exitosa. " << nombreOrigen << " ahora tiene " << obtenerUnidadesTerritorio(tOrigen)
         << " unidades y " << nombreDestino << " ahora tiene " << obtenerUnidadesTerritorio(tDestino) << " unidades." << endl;

    int territoriosControlados = contarTerritoriosControlados(partidaActual, colorJ);

    if (territoriosControlados == TOTAL_TERRITORIOS) {
        partidaActual.terminado = true;
        asignarGanadorJuego(partidaActual, obtenerNombreJugador(jActual));
        cout << "\nFELICIDADES! El jugador " << obtenerNombreJugador(jActual) << " ha conquistado todo el mapa y ganado la partida." << endl;
    } else {
        pasarSiguienteTurno(partidaActual);
        int turnoNuevo = obtenerTurnoActualJuego(partidaActual);
        cout << "Turno finalizado. Ahora es el turno del jugador: "
             << obtenerNombreJugador(obtenerJugadorConst(partidaActual, turnoNuevo)) << endl;
    }

    cout << "(Comando correcto) El jugador " << nombreJugador << " ha terminado de fortificar su posicion." << endl;
}

void EstadoJuego() {
    if (!estaInicializadoJuego(partidaActual)) {
        cout << "(Juego no inicializado) Esta partida no ha sido inicializada correctamente." << endl;
        return;
    }

    if (estaTerminadoJuego(partidaActual)) {
        cout << "(Juego terminado) Esta partida ya tuvo un ganador." << endl;
        return;
    }

    cout << "\n=============================================" << endl;
    cout << "          RESUMEN DE LA PARTIDA RISK         " << endl;
    cout << "=============================================" << endl;
    cout << "Numero de jugadores: " << obtenerCantidadJugadoresJuego(partidaActual) << endl;

    cout << "\nListado de Jugadores:" << endl;
    for (int i = 0; i < obtenerCantidadJugadoresJuego(partidaActual); i++) {
        const Jugador& j = obtenerJugadorConst(partidaActual, i);
        cout << "  - Nombre: " << obtenerNombreJugador(j)
             << " | Color: " << obtenerColorJugador(j)
             << (estaActivoJugador(j) ? " (Activo)" : " (Eliminado)")
             << " | Cartas: " << obtenerCantidadCartasJugador(j);
        for (int c = 0; c < obtenerCantidadCartasJugador(j); c++) {
            const Carta& carta = obtenerCartaJugador(j, c);
            cout << (c == 0 ? " [" : ", ") << obtenerIdCarta(carta) << " " << obtenerTerritorioCarta(carta)
                 << " " << obtenerTipoCarta(carta);
        }
        if (obtenerCantidadCartasJugador(j) > 0) cout << "]";
        cout << endl;
    }
    cout << "Cartas en el mazo: " << partidaActual.cantidadCartasMazo << endl;

    int turnoAct = obtenerTurnoActualJuego(partidaActual);
    const Jugador& jTurno = obtenerJugadorConst(partidaActual, turnoAct);
    cout << "\nTurno actual: " << obtenerNombreJugador(jTurno)
         << " (Fase: " << obtenerFaseActualJuego(partidaActual) << ")" << endl;

    cout << "\nLista de los " << TOTAL_TERRITORIOS << " territorios:" << endl;
    cout << "---------------------------------------------" << endl;
    for (int i = 0; i < TOTAL_TERRITORIOS; i++) {
        const Territorio& t = obtenerTerritorioConst(partidaActual, i);
        int idCont = obtenerContinenteTerritorio(t);
        string nomCont = (idCont >= 0 && idCont < TOTAL_CONTINENTES) ?
                              obtenerNombreContinente(partidaActual.continentes[idCont]) : string("Desconocido");

        cout << " Territorio [" << obtenerCodigoTerritorio(t) << "]: " << obtenerNombreTerritorio(t)
             << " | Continente: " << nomCont
             << " | Controla (Color): " << obtenerColorTerritorio(t)
             << " | Unidades: " << obtenerUnidadesTerritorio(t) << endl;
    }
    cout << "=============================================" << endl;
    cout << "(Exito) Resumen de la partida presentado correctamente." << endl;
}

void Salir(bool& enEjecucion) {
    cout << "\n============================================" << endl;
    cout << "   Gracias por usar el Sistema RISK. Adios! " << endl;
    cout << "============================================" << endl;
    enEjecucion = false;
}

// =============================================================================
// --- COMANDOS NO INTERACTIVOS (archivo de comandos / ejecutar_comandos) ---
// =============================================================================

// Bono de unidades del n-esimo canje global (n empieza en 1):
// 4, 6, 8, 10, 12, 15 y luego +5 por cada canje adicional.
int calcularBonoCanje(int numeroCanje) {
    if (numeroCanje <= 5) return 2 + 2 * numeroCanje;
    return 15 + 5 * (numeroCanje - 6);
}

// Valida que el juego este activo y que el jugador exista, este activo y sea su turno.
bool validarJugadorEnTurno(const string& nombreJugador, int& indiceJugador) {
    if (!estaInicializadoJuego(partidaActual)) {
        cout << "(Juego no inicializado) Esta partida no ha sido inicializada correctamente." << endl;
        return false;
    }
    if (estaTerminadoJuego(partidaActual)) {
        cout << "(Juego terminado) Esta partida ya tuvo un ganador." << endl;
        return false;
    }
    indiceJugador = buscarIndiceJugador(partidaActual, nombreJugador);
    if (indiceJugador == -1) {
        cout << "(Jugador no valido) El jugador " << nombreJugador << " no forma parte de esta partida." << endl;
        return false;
    }
    if (!estaActivoJugador(obtenerJugadorConst(partidaActual, indiceJugador))) {
        cout << "(Jugador no valido) El jugador " << nombreJugador << " ya ha sido eliminado de la partida." << endl;
        return false;
    }
    if (obtenerTurnoActualJuego(partidaActual) != indiceJugador) {
        cout << "(Jugador fuera de turno) No es el turno del jugador " << nombreJugador << "." << endl;
        return false;
    }
    return true;
}

// Calcula las unidades que le corresponden al jugador en este turno.
int calcularUnidadesTurno(const Jugador& j, int& porTerritorios, int& porContinentes, int& porCanje) {
    const string& colorJ = obtenerColorJugador(j);
    porTerritorios = contarTerritoriosControlados(partidaActual, colorJ) / 3;
    if (porTerritorios < 3) {
        porTerritorios = 3;
    }
    porContinentes = calcularBonoContinentesTotal(partidaActual, colorJ);
    porCanje = obtenerBonoCanjeJugador(j);
    return porTerritorios + porContinentes + porCanje;
}

void ObtenerUnidadesDirecto(const string& nombreJugador) {
    int indiceJugador;
    if (!validarJugadorEnTurno(nombreJugador, indiceJugador)) return;

    if (obtenerFaseActualJuego(partidaActual) != "RECLUTAR") {
        cout << "(Jugador fuera de turno) No se encuentra en la fase de reclutamiento." << endl;
        return;
    }

    Jugador& jActual = obtenerJugadorRef(partidaActual, indiceJugador);
    int porTerritorios, porContinentes, porCanje;
    int total = calcularUnidadesTurno(jActual, porTerritorios, porContinentes, porCanje);
    asignarUnidadesPendientesJugador(jActual, total);
    asignarBonoCanjeJugador(jActual, 0);
    asignarFaseJuego(partidaActual, "UBICAR");

    cout << "(Comando correcto) El jugador " << nombreJugador << " reclama " << total
         << " unidades (" << porTerritorios << " por territorios, " << porContinentes
         << " por continentes y " << porCanje << " por canje de cartas)." << endl;
}

void UbicarUnidad(const string& nombreJugador, const string& territorio, int cantidad) {
    int indiceJugador;
    if (!validarJugadorEnTurno(nombreJugador, indiceJugador)) return;

    if (obtenerFaseActualJuego(partidaActual) != "UBICAR") {
        cout << "(Fase incorrecta) El jugador " << nombreJugador
             << " no tiene unidades por ubicar (ejecute obtener_unidades primero)." << endl;
        return;
    }

    Jugador& jActual = obtenerJugadorRef(partidaActual, indiceJugador);
    if (cantidad <= 0 || cantidad > obtenerUnidadesPendientesJugador(jActual)) {
        cout << "(Cantidad invalida) Tiene " << obtenerUnidadesPendientesJugador(jActual)
             << " unidades pendientes y pidio ubicar " << cantidad << "." << endl;
        return;
    }

    int idxT = buscarIndiceTerritorio(partidaActual, territorio);
    if (idxT == -1) {
        cout << "(Territorio no valido) El territorio " << territorio << " no existe." << endl;
        return;
    }
    Territorio& t = obtenerTerritorioRef(partidaActual, idxT);
    if (!perteneceAJugador(t, obtenerColorJugador(jActual))) {
        cout << "(Territorio no valido) El territorio " << territorio << " no le pertenece a " << nombreJugador << "." << endl;
        return;
    }

    modificarUnidadesTerritorio(t, cantidad);
    modificarUnidadesPendientesJugador(jActual, -cantidad);
    cout << "(Comando correcto) " << cantidad << " unidades ubicadas en " << territorio
         << " (ahora tiene " << obtenerUnidadesTerritorio(t) << "). Pendientes: "
         << obtenerUnidadesPendientesJugador(jActual) << "." << endl;

    if (obtenerUnidadesPendientesJugador(jActual) == 0) {
        asignarFaseJuego(partidaActual, "ATACAR");
        cout << "El jugador " << nombreJugador << " ha terminado de ubicar sus unidades." << endl;
    }
}

void AtacarDirecto(const string& nombreJugador, const string& origen, const string& destino,
                    const vector<int>& dados) {
    int indiceJugador;
    if (!validarJugadorEnTurno(nombreJugador, indiceJugador)) return;

    if (obtenerFaseActualJuego(partidaActual) != "ATACAR") {
        cout << "(Fase incorrecta) El jugador " << nombreJugador
             << " no esta en fase de ataque (debe obtener y ubicar sus unidades primero)." << endl;
        return;
    }

    Jugador& jActual = obtenerJugadorRef(partidaActual, indiceJugador);
    const string& colorJ = obtenerColorJugador(jActual);

    int idxOrigen = buscarIndiceTerritorio(partidaActual, origen);
    int idxDestino = buscarIndiceTerritorio(partidaActual, destino);
    if (idxOrigen == -1 || idxDestino == -1) {
        cout << "(Territorio no valido) Uno o ambos territorios (" << origen << ", " << destino << ") no existen." << endl;
        return;
    }

    Territorio& tOrigen = obtenerTerritorioRef(partidaActual, idxOrigen);
    Territorio& tDestino = obtenerTerritorioRef(partidaActual, idxDestino);

    if (!perteneceAJugador(tOrigen, colorJ)) {
        cout << "(Ataque invalido) El territorio de origen " << origen << " no le pertenece." << endl;
        return;
    }
    if (obtenerUnidadesTerritorio(tOrigen) < 2) {
        cout << "(Ataque invalido) " << origen << " debe tener al menos 2 unidades para atacar (tiene "
             << obtenerUnidadesTerritorio(tOrigen) << ")." << endl;
        return;
    }
    if (perteneceAJugador(tDestino, colorJ)) {
        cout << "(Ataque invalido) No puede atacar un territorio propio (" << destino << ")." << endl;
        return;
    }
    if (!esVecinoTerritorio(tOrigen, idxDestino)) {
        cout << "(Ataque invalido) El territorio " << destino << " no es vecino de " << origen << "." << endl;
        return;
    }

    int dadosAtacante = obtenerUnidadesTerritorio(tOrigen) - 1;
    if (dadosAtacante > 3) dadosAtacante = 3;
    int dadosDefensor = obtenerUnidadesTerritorio(tDestino);
    if (dadosDefensor > 2) dadosDefensor = 2;

    int cantDados = static_cast<int>(dados.size());
    int esperados = dadosAtacante + dadosDefensor;
    if (cantDados != esperados && cantDados != esperados + 1) {
        cout << "(Dados invalidos) Se esperaban " << esperados << " dados (" << dadosAtacante
             << " del atacante y " << dadosDefensor << " del defensor) y se recibieron " << cantDados << "." << endl;
        return;
    }
    for (int i = 0; i < esperados; i++) {
        if (dados[i] < 1 || dados[i] > 6) {
            cout << "(Dados invalidos) El valor " << dados[i] << " no es un dado valido (1 a 6)." << endl;
            return;
        }
    }

    vector<int> arrAtacante(dadosAtacante);
    vector<int> arrDefensor(dadosDefensor);
    cout << "Batalla " << origen << " (" << obtenerUnidadesTerritorio(tOrigen) << ") vs "
         << destino << " (" << obtenerUnidadesTerritorio(tDestino) << ")" << endl;
    cout << "  Dados Atacante: ";
    for (int i = 0; i < dadosAtacante; i++) {
        arrAtacante[i] = dados[i];
        cout << arrAtacante[i] << " ";
    }
    cout << "| Dados Defensor: ";
    for (int i = 0; i < dadosDefensor; i++) {
        arrDefensor[i] = dados[dadosAtacante + i];
        cout << arrDefensor[i] << " ";
    }
    cout << endl;

    ordenarDescendente(arrAtacante);
    ordenarDescendente(arrDefensor);

    int enfrentamientos = (dadosAtacante < dadosDefensor) ? dadosAtacante : dadosDefensor;
    int perdidasAtacante = 0, perdidasDefensor = 0;
    for (int i = 0; i < enfrentamientos; i++) {
        if (arrAtacante[i] > arrDefensor[i]) {
            modificarUnidadesTerritorio(tDestino, -1);
            perdidasDefensor++;
        } else {
            modificarUnidadesTerritorio(tOrigen, -1);
            perdidasAtacante++;
        }
    }
    cout << "  Atacante pierde " << perdidasAtacante << ", defensor pierde " << perdidasDefensor
         << ". Quedan " << origen << " (" << obtenerUnidadesTerritorio(tOrigen) << ") y "
         << destino << " (" << obtenerUnidadesTerritorio(tDestino) << ")." << endl;

    if (obtenerUnidadesTerritorio(tDestino) == 0) {
        string colorDefensor = obtenerColorTerritorio(tDestino);

        // Unidades a trasladar: valor extra al final del comando, o por defecto
        // (dados del atacante - 1), siempre dejando al menos 1 en el origen.
        int maximo = obtenerUnidadesTerritorio(tOrigen) - 1;
        int traslado;
        if (cantDados == esperados + 1) {
            traslado = dados[esperados];
            if (traslado < 1 || traslado > maximo) {
                cout << "  Traslado de " << traslado << " invalido (min 1, max " << maximo << "); se trasladara 1." << endl;
                traslado = 1;
            }
        } else {
            traslado = dadosAtacante - 1;
            if (traslado < 1) traslado = 1;
            if (traslado > maximo) traslado = maximo;
        }

        asignarColorTerritorio(tDestino, colorJ);
        modificarUnidadesTerritorio(tOrigen, -traslado);
        asignarUnidadesTerritorio(tDestino, traslado);
        cout << "  El territorio " << destino << " ha sido conquistado! Se trasladan " << traslado
             << " unidades. " << origen << " (" << obtenerUnidadesTerritorio(tOrigen) << "), "
             << destino << " (" << obtenerUnidadesTerritorio(tDestino) << ")." << endl;

        // Eliminacion del defensor: el atacante recibe sus cartas
        for (int i = 0; i < obtenerCantidadJugadoresJuego(partidaActual); i++) {
            Jugador& jDef = obtenerJugadorRef(partidaActual, i);
            if (obtenerColorJugador(jDef) == colorDefensor && estaActivoJugador(jDef) &&
                contarTerritoriosControlados(partidaActual, colorDefensor) == 0) {
                setActivoJugador(jDef, false);
                while (obtenerCantidadCartasJugador(jDef) > 0) {
                    Carta c = obtenerCartaJugador(jDef, 0);
                    quitarCartaJugador(jDef, obtenerIdCarta(c));
                    agregarCartaJugador(jActual, c);
                }
                cout << "  ATENCION! El jugador " << obtenerNombreJugador(jDef)
                     << " ha sido ELIMINADO; sus cartas pasan a " << nombreJugador << "." << endl;
            }
        }

        if (contarTerritoriosControlados(partidaActual, colorJ) == TOTAL_TERRITORIOS) {
            partidaActual.terminado = true;
            asignarGanadorJuego(partidaActual, obtenerNombreJugador(jActual));
            cout << "  FELICIDADES! El jugador " << nombreJugador << " ha conquistado todo el mapa." << endl;
        }
    }
    cout << "(Comando correcto) Ataque de " << nombreJugador << " resuelto." << endl;
}

// Verifica si existe un camino entre origen y destino pasando solo por territorios del color dado.
bool estanConectadosPorPropios(const Juego& juego, int origen, int destino, const string& color) {
    vector<bool> visitado(TOTAL_TERRITORIOS, false);
    vector<int> cola;
    cola.reserve(TOTAL_TERRITORIOS);
    size_t inicio = 0;
    cola.push_back(origen);
    visitado[origen] = true;
    while (inicio < cola.size()) {
        int actual = cola[inicio++];
        if (actual == destino) return true;
        const Territorio& t = juego.territorios[actual];
        for (int v = 0; v < obtenerCantidadVecinosTerritorio(t); v++) {
            int vecino = obtenerVecinoEnTerritorio(t, v);
            if (vecino >= 0 && vecino < TOTAL_TERRITORIOS && !visitado[vecino] &&
                perteneceAJugador(juego.territorios[vecino], color)) {
                visitado[vecino] = true;
                cola.push_back(vecino);
            }
        }
    }
    return false;
}

void FortificarDirecto(const string& nombreJugador, const string& origen, const string& destino, int cantidad) {
    int indiceJugador;
    if (!validarJugadorEnTurno(nombreJugador, indiceJugador)) return;

    const string& fase = obtenerFaseActualJuego(partidaActual);
    if (fase != "ATACAR" && fase != "FORTIFICAR") {
        if (fase == "FIN_TURNO") {
            cout << "(Fase incorrecta) El jugador " << nombreJugador << " ya fortifico en este turno." << endl;
        } else {
            cout << "(Fase incorrecta) El jugador " << nombreJugador << " debe obtener y ubicar sus unidades antes de fortificar." << endl;
        }
        return;
    }

    const Jugador& jActual = obtenerJugadorConst(partidaActual, indiceJugador);
    const string& colorJ = obtenerColorJugador(jActual);

    int idxOrigen = buscarIndiceTerritorio(partidaActual, origen);
    int idxDestino = buscarIndiceTerritorio(partidaActual, destino);
    if (idxOrigen == -1 || idxDestino == -1) {
        cout << "(Territorio no valido) Uno o ambos territorios (" << origen << ", " << destino << ") no existen." << endl;
        return;
    }
    if (idxOrigen == idxDestino) {
        cout << "(Fortificacion invalida) El origen y el destino deben ser distintos." << endl;
        return;
    }

    Territorio& tOrigen = obtenerTerritorioRef(partidaActual, idxOrigen);
    Territorio& tDestino = obtenerTerritorioRef(partidaActual, idxDestino);

    if (!perteneceAJugador(tOrigen, colorJ) || !perteneceAJugador(tDestino, colorJ)) {
        cout << "(Fortificacion invalida) Ambos territorios deben pertenecer a " << nombreJugador << "." << endl;
        return;
    }
    if (!estanConectadosPorPropios(partidaActual, idxOrigen, idxDestino, colorJ)) {
        cout << "(Fortificacion invalida) " << origen << " y " << destino
             << " no estan conectados por territorios propios." << endl;
        return;
    }
    if (cantidad <= 0 || cantidad >= obtenerUnidadesTerritorio(tOrigen)) {
        cout << "(Fortificacion invalida) " << origen << " tiene " << obtenerUnidadesTerritorio(tOrigen)
             << " unidades y debe conservar al menos 1 (pidio mover " << cantidad << ")." << endl;
        return;
    }

    modificarUnidadesTerritorio(tOrigen, -cantidad);
    modificarUnidadesTerritorio(tDestino, cantidad);
    asignarFaseJuego(partidaActual, "FIN_TURNO");
    cout << "(Comando correcto) Fortificacion: " << origen << " (" << obtenerUnidadesTerritorio(tOrigen)
         << ") -> " << destino << " (" << obtenerUnidadesTerritorio(tDestino) << ")." << endl;
}

void TurnoSiguiente() {
    if (!estaInicializadoJuego(partidaActual)) {
        cout << "(Juego no inicializado) Esta partida no ha sido inicializada correctamente." << endl;
        return;
    }
    if (estaTerminadoJuego(partidaActual)) {
        cout << "(Juego terminado) Esta partida ya tuvo un ganador." << endl;
        return;
    }
    const Jugador& jActual = obtenerJugadorConst(partidaActual, obtenerTurnoActualJuego(partidaActual));
    if (obtenerFaseActualJuego(partidaActual) == "UBICAR" && obtenerUnidadesPendientesJugador(jActual) > 0) {
        cout << "(Turno incompleto) El jugador " << obtenerNombreJugador(jActual) << " aun tiene "
             << obtenerUnidadesPendientesJugador(jActual) << " unidades por ubicar." << endl;
        return;
    }
    pasarSiguienteTurno(partidaActual);
    cout << "(Comando correcto) Turno finalizado. Ahora es el turno de "
         << obtenerNombreJugador(obtenerJugadorConst(partidaActual, obtenerTurnoActualJuego(partidaActual))) << "." << endl;
}

void RobarCarta(const string& nombreJugador, int idCarta) {
    int indiceJugador;
    if (!validarJugadorEnTurno(nombreJugador, indiceJugador)) return;

    int idxMazo = -1;
    for (int i = 0; i < partidaActual.cantidadCartasMazo; i++) {
        if (obtenerIdCarta(partidaActual.mazo[i]) == idCarta) {
            idxMazo = i;
            break;
        }
    }
    if (idxMazo == -1) {
        cout << "(Carta no valida) La carta " << idCarta << " no esta en el mazo." << endl;
        return;
    }

    Jugador& jActual = obtenerJugadorRef(partidaActual, indiceJugador);
    if (!agregarCartaJugador(jActual, partidaActual.mazo[idxMazo])) {
        cout << "(Carta no valida) El jugador " << nombreJugador << " no puede tener mas cartas." << endl;
        return;
    }
    // Retirar la carta del mazo conservando el orden de las demas
    partidaActual.mazo.erase(partidaActual.mazo.begin() + idxMazo);
    partidaActual.cantidadCartasMazo--;

    const Carta& c = obtenerCartaJugador(jActual, obtenerCantidadCartasJugador(jActual) - 1);
    cout << "(Comando correcto) " << nombreJugador << " roba la carta " << idCarta << " ("
         << obtenerTerritorioCarta(c) << ", " << obtenerTipoCarta(c) << "). Tiene "
         << obtenerCantidadCartasJugador(jActual) << " cartas." << endl;
}

void CanjearCartas(const string& nombreJugador, int idCarta1, int idCarta2, int idCarta3) {
    int indiceJugador;
    if (!validarJugadorEnTurno(nombreJugador, indiceJugador)) return;

    const string& fase = obtenerFaseActualJuego(partidaActual);
    if (fase != "RECLUTAR" && fase != "UBICAR") {
        cout << "(Fase incorrecta) Las cartas solo se canjean al inicio del turno (antes de atacar)." << endl;
        return;
    }
    if (idCarta1 == idCarta2 || idCarta1 == idCarta3 || idCarta2 == idCarta3) {
        cout << "(Canje invalido) Debe indicar tres cartas distintas." << endl;
        return;
    }

    Jugador& jActual = obtenerJugadorRef(partidaActual, indiceJugador);
    int ids[3] = {idCarta1, idCarta2, idCarta3};
    string tipos[3];
    for (int i = 0; i < 3; i++) {
        int idx = buscarCartaJugador(jActual, ids[i]);
        if (idx == -1) {
            cout << "(Canje invalido) El jugador " << nombreJugador << " no tiene la carta " << ids[i] << "." << endl;
            return;
        }
        tipos[i] = obtenerTipoCarta(obtenerCartaJugador(jActual, idx));
    }

    // Trio valido: tres iguales, tres distintos, o cualquier combinacion con comodin
    int comodines = 0;
    for (int i = 0; i < 3; i++) {
        if (tipos[i] == "Comodin") comodines++;
    }
    bool igual01 = (tipos[0] == tipos[1]);
    bool igual02 = (tipos[0] == tipos[2]);
    bool igual12 = (tipos[1] == tipos[2]);
    bool todosIguales = igual01 && igual12;
    bool todosDistintos = !igual01 && !igual02 && !igual12;
    if (comodines == 0 && !todosIguales && !todosDistintos) {
        cout << "(Canje invalido) Las cartas deben ser tres del mismo tipo o una de cada tipo." << endl;
        return;
    }

    for (int i = 0; i < 3; i++) {
        quitarCartaJugador(jActual, ids[i]);
    }
    partidaActual.canjesRealizados++;
    int bono = calcularBonoCanje(partidaActual.canjesRealizados);

    if (fase == "UBICAR") {
        modificarUnidadesPendientesJugador(jActual, bono); // Ya reclamo: se suman a las pendientes
    } else {
        asignarBonoCanjeJugador(jActual, obtenerBonoCanjeJugador(jActual) + bono); // Se suman en obtener_unidades
    }
    cout << "(Comando correcto) " << nombreJugador << " canjea las cartas " << idCarta1 << ", "
         << idCarta2 << ", " << idCarta3 << " (canje global #" << partidaActual.canjesRealizados
         << ") y gana " << bono << " unidades." << endl;
}

// Guarda el estado en el formato extendido (el mismo que acepta inicializar):
//   <numJugadores>
//   <nombre> <color> <numCartas> [<id> <territorio> <tipo>]...
//   <numCartasMazo>
//   <id> <territorio> <tipo>
//   <territorio> <color> <unidades>   (42 lineas)
// Escribe el estado completo del juego en el flujo dado
void escribirEstadoJuego(ostream& salida) {
    salida << obtenerCantidadJugadoresJuego(partidaActual) << "\n";
    for (int i = 0; i < obtenerCantidadJugadoresJuego(partidaActual); i++) {
        const Jugador& j = obtenerJugadorConst(partidaActual, i);
        salida << obtenerNombreJugador(j) << " " << obtenerColorJugador(j) << " " << obtenerCantidadCartasJugador(j);
        for (int c = 0; c < obtenerCantidadCartasJugador(j); c++) {
            const Carta& carta = obtenerCartaJugador(j, c);
            salida << " " << obtenerIdCarta(carta) << " " << obtenerTerritorioCarta(carta) << " " << obtenerTipoCarta(carta);
        }
        salida << "\n";
    }

    salida << partidaActual.cantidadCartasMazo << "\n";
    for (int c = 0; c < partidaActual.cantidadCartasMazo; c++) {
        const Carta& carta = partidaActual.mazo[c];
        salida << obtenerIdCarta(carta) << " " << obtenerTerritorioCarta(carta) << " " << obtenerTipoCarta(carta) << "\n";
    }

    for (int t = 0; t < TOTAL_TERRITORIOS; t++) {
        const Territorio& terr = obtenerTerritorioConst(partidaActual, t);
        salida << obtenerNombreTerritorio(terr) << " " << obtenerColorTerritorio(terr) << " " << obtenerUnidadesTerritorio(terr);
        if (t < TOTAL_TERRITORIOS - 1) salida << "\n";
    }
}

// Comando guardar: archivo de texto plano, mismo formato que acepta inicializar
void GuardarJuego(const string& nombreArchivo) {
    if (!estaInicializadoJuego(partidaActual)) {
        cout << "(Juego no inicializado) Esta partida no ha sido inicializada correctamente." << endl;
        return;
    }

    ofstream salida(nombreArchivo);
    if (!salida.is_open()) {
        cout << "(Error al guardar) La partida no ha sido guardada correctamente." << endl;
        return;
    }
    escribirEstadoJuego(salida);
    bool ok = salida.good();
    salida.close();

    if (ok) {
        cout << "(Comando correcto) La partida ha sido guardada correctamente." << endl;
    } else {
        cout << "(Error al guardar) La partida no ha sido guardada correctamente." << endl;
    }
}

// Comando guardar_comprimido: misma informacion que guardar, codificada con
// Huffman en un archivo binario .bin (si el nombre no termina en .bin, se agrega).
void GuardarComprimido(const string& nombreArchivo) {
    if (!estaInicializadoJuego(partidaActual)) {
        cout << "(Juego no inicializado) Esta partida no ha sido inicializada correctamente." << endl;
        return;
    }

    string nombreFinal = nombreArchivo;
    if (!tieneExtensionBin(nombreFinal)) {
        nombreFinal += ".bin";
    }

    ostringstream flujo;
    escribirEstadoJuego(flujo);
    string texto = flujo.str();

    if (comprimirHuffman(texto.c_str(), texto.size(), nombreFinal.c_str())) {
        cout << "(Comando correcto) La partida ha sido codificada y guardada correctamente." << endl;
    } else {
        cout << "(Error al codificar y/o guardar) La partida no ha sido codificada ni guardada correctamente." << endl;
    }
}
