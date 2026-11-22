#ifndef JUGADOR_H
#define JUGADOR_H

#include <string>
#include <vector>

// --- TAD Carta ---
// Cada carta tiene un identificador unico, el territorio que representa y su
// tipo (Infanteria, Caballeria, Artilleria o Comodin).
struct Carta {
    int idC;
    std::string territorioC;
    std::string tipoC;
};

void inicializarCarta(Carta& c, int id, const std::string& territorio, const std::string& tipo);
int obtenerIdCarta(const Carta& c);
const std::string& obtenerTerritorioCarta(const Carta& c);
const std::string& obtenerTipoCarta(const Carta& c);

// --- 3.3. TAD Jugador ---
struct Jugador {
    std::string nombreJ; // Maximo 8 caracteres segun el enunciado
    std::string colorJ;
    int unidadesPendientesJ;
    bool activoJ;
    std::vector<Carta> cartasJ; // Cartas en la mano del jugador (sin limite fijo)
    int bonoCanjeJ;             // Unidades ganadas por canje, se suman en obtener_unidades
};

// --- OPERACIONES DEL TAD JUGADOR ---

// Constructor / Inicializador
void inicializarJugador(Jugador& j, const std::string& nombre, const std::string& color);

// Getters (Consultores)
const std::string& obtenerNombreJugador(const Jugador& j);
const std::string& obtenerColorJugador(const Jugador& j);
int obtenerUnidadesPendientesJugador(const Jugador& j);
bool estaActivoJugador(const Jugador& j);
int obtenerCantidadCartasJugador(const Jugador& j);
const Carta& obtenerCartaJugador(const Jugador& j, int indice);
int buscarCartaJugador(const Jugador& j, int idCarta); // Indice en la mano o -1
int obtenerBonoCanjeJugador(const Jugador& j);

// Setters y Modificadores
void asignarUnidadesPendientesJugador(Jugador& j, int unidades);
void modificarUnidadesPendientesJugador(Jugador& j, int delta); // Sumar o restar
void setActivoJugador(Jugador& j, bool activo);
bool agregarCartaJugador(Jugador& j, const Carta& c);
bool quitarCartaJugador(Jugador& j, int idCarta);
void asignarBonoCanjeJugador(Jugador& j, int bono);

#endif // JUGADOR_H
