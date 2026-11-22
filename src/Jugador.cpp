#include "Jugador.h"

// --- IMPLEMENTACION DE OPERACIONES DEL TAD CARTA ---

void inicializarCarta(Carta& c, int id, const std::string& territorio, const std::string& tipo) {
    c.idC = id;
    c.territorioC = territorio;
    c.tipoC = tipo;
}

int obtenerIdCarta(const Carta& c) {
    return c.idC;
}

const std::string& obtenerTerritorioCarta(const Carta& c) {
    return c.territorioC;
}

const std::string& obtenerTipoCarta(const Carta& c) {
    return c.tipoC;
}

// --- IMPLEMENTACION DE OPERACIONES DEL TAD JUGADOR ---

void inicializarJugador(Jugador& j, const std::string& nombre, const std::string& color) {
    j.nombreJ = nombre;
    j.colorJ = color;
    j.unidadesPendientesJ = 0;
    j.activoJ = true;
    j.cartasJ.clear();
    j.bonoCanjeJ = 0;
}

const std::string& obtenerNombreJugador(const Jugador& j) {
    return j.nombreJ;
}

const std::string& obtenerColorJugador(const Jugador& j) {
    return j.colorJ;
}

int obtenerUnidadesPendientesJugador(const Jugador& j) {
    return j.unidadesPendientesJ;
}

bool estaActivoJugador(const Jugador& j) {
    return j.activoJ;
}

int obtenerCantidadCartasJugador(const Jugador& j) {
    return static_cast<int>(j.cartasJ.size());
}

const Carta& obtenerCartaJugador(const Jugador& j, int indice) {
    return j.cartasJ[indice];
}

int buscarCartaJugador(const Jugador& j, int idCarta) {
    for (int i = 0; i < static_cast<int>(j.cartasJ.size()); i++) {
        if (j.cartasJ[i].idC == idCarta) {
            return i;
        }
    }
    return -1;
}

int obtenerBonoCanjeJugador(const Jugador& j) {
    return j.bonoCanjeJ;
}

void asignarUnidadesPendientesJugador(Jugador& j, int unidades) {
    j.unidadesPendientesJ = unidades;
}

void modificarUnidadesPendientesJugador(Jugador& j, int delta) {
    j.unidadesPendientesJ += delta;
    if (j.unidadesPendientesJ < 0) {
        j.unidadesPendientesJ = 0;
    }
}

void setActivoJugador(Jugador& j, bool activo) {
    j.activoJ = activo;
}

bool agregarCartaJugador(Jugador& j, const Carta& c) {
    j.cartasJ.push_back(c);
    return true;
}

bool quitarCartaJugador(Jugador& j, int idCarta) {
    int idx = buscarCartaJugador(j, idCarta);
    if (idx == -1) {
        return false;
    }
    // erase conserva el orden de las cartas restantes en la mano
    j.cartasJ.erase(j.cartasJ.begin() + idx);
    return true;
}

void asignarBonoCanjeJugador(Jugador& j, int bono) {
    j.bonoCanjeJ = bono;
}
