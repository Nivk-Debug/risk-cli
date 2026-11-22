#ifndef JUEGO_H
#define JUEGO_H

#include <string>
#include <vector>
#include "Territorio.h"
#include "Continente.h"
#include "Jugador.h"

// --- CONSTANTES DEL JUEGO ---
const int MAX_JUGADORES_JUEGO = 6;
const int MIN_JUGADORES = 3;
const int TOTAL_TERRITORIOS = 42;
const int TOTAL_CONTINENTES = 6;

// --- 3.4. TAD Juego ---
struct Juego {
    std::vector<Jugador> jugadores;
    int cantidadJugadores;
    std::vector<Territorio> territorios;
    std::vector<Continente> continentes;
    int turnoActual;
    std::string fase; // "RECLUTAR", "UBICAR", "ATACAR" o "FORTIFICAR"
    bool inicializado;
    bool terminado;
    std::string ganador;
    std::vector<Carta> mazo; // Cartas que aun no tiene ningun jugador
    int cantidadCartasMazo;
    int canjesRealizados;    // Numero de canjes globales (define el bono del siguiente)
};

// Instancia global del Juego
extern Juego partidaActual;

// --- OPERACIONES DEL TAD JUEGO ---

// Constructor / Reinicializador
void reiniciarJuego(Juego& juego);

// Getters (Consultores)
bool estaInicializadoJuego(const Juego& juego);
bool estaTerminadoJuego(const Juego& juego);
int obtenerCantidadJugadoresJuego(const Juego& juego);
int obtenerTurnoActualJuego(const Juego& juego);
const std::string& obtenerFaseActualJuego(const Juego& juego);
const std::string& obtenerGanadorJuego(const Juego& juego);

Jugador& obtenerJugadorRef(Juego& juego, int indice);
const Jugador& obtenerJugadorConst(const Juego& juego, int indice);
Territorio& obtenerTerritorioRef(Juego& juego, int indice);
const Territorio& obtenerTerritorioConst(const Juego& juego, int indice);
Continente& obtenerContinenteRef(Juego& juego, int indice);
const Continente& obtenerContinenteConst(const Juego& juego, int indice);

int buscarIndiceJugador(const Juego& juego, const std::string& nombre);
int buscarIndiceTerritorio(const Juego& juego, const std::string& nombre);

// Modificadores y Operaciones de Dominio
void asignarFaseJuego(Juego& juego, const std::string& nuevaFase);
void asignarGanadorJuego(Juego& juego, const std::string& ganador);
void pasarSiguienteTurno(Juego& juego);
int contarTerritoriosControlados(const Juego& juego, const std::string& color);
int calcularBonoContinentesTotal(const Juego& juego, const std::string& color);

// --- COMANDOS DEL SISTEMA RISK (Controlador de Partida) ---
void Inicializar(const std::string& nombreArchivo);
void ObtenerUnidades(const std::string& nombreJugador);
void Atacar(const std::string& nombreJugador);
void Fortificar(const std::string& nombreJugador);
void EstadoJuego();
void Salir(bool& enEjecucion);

// --- COMANDOS NO INTERACTIVOS (usados por archivos de comandos) ---
// Calcula las unidades del turno (territorios + continentes + canje) y las deja
// pendientes; luego se ubican con UbicarUnidad.
void ObtenerUnidadesDirecto(const std::string& nombreJugador);
void UbicarUnidad(const std::string& nombreJugador, const std::string& territorio, int cantidad);
// Una tirada de dados: dados trae primero los del atacante y luego los del
// defensor. Si sobra un valor al final, es la cantidad de unidades a trasladar
// en caso de conquista.
void AtacarDirecto(const std::string& nombreJugador, const std::string& origen, const std::string& destino,
                    const std::vector<int>& dados);
void FortificarDirecto(const std::string& nombreJugador, const std::string& origen, const std::string& destino, int cantidad);
void TurnoSiguiente();
void RobarCarta(const std::string& nombreJugador, int idCarta);
void CanjearCartas(const std::string& nombreJugador, int idCarta1, int idCarta2, int idCarta3);
void GuardarJuego(const std::string& nombreArchivo);
void GuardarComprimido(const std::string& nombreArchivo);

#endif // JUEGO_H
