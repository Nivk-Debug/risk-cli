#include "Territorio.h"

// --- IMPLEMENTACION DE OPERACIONES DEL TAD TERRITORIO ---

void inicializarTerritorio(Territorio& t, int codigo, const std::string& nombre, int continente, const std::string& colorJugador, int unidades) {
    t.codigoT = codigo;
    t.nombreT = nombre;
    t.continenteT = continente;
    t.colorJugadorT = colorJugador;
    t.unidadesT = unidades;
    t.vecinosT.clear();
}

int obtenerCodigoTerritorio(const Territorio& t) {
    return t.codigoT;
}

const std::string& obtenerNombreTerritorio(const Territorio& t) {
    return t.nombreT;
}

int obtenerContinenteTerritorio(const Territorio& t) {
    return t.continenteT;
}

const std::string& obtenerColorTerritorio(const Territorio& t) {
    return t.colorJugadorT;
}

int obtenerUnidadesTerritorio(const Territorio& t) {
    return t.unidadesT;
}

int obtenerCantidadVecinosTerritorio(const Territorio& t) {
    return static_cast<int>(t.vecinosT.size());
}

int obtenerVecinoEnTerritorio(const Territorio& t, int indice) {
    if (indice >= 0 && indice < static_cast<int>(t.vecinosT.size())) {
        return t.vecinosT[indice];
    }
    return -1;
}

void asignarCodigoTerritorio(Territorio& t, int nuevoCodigo) {
    t.codigoT = nuevoCodigo;
}

void asignarColorTerritorio(Territorio& t, const std::string& nuevoColor) {
    t.colorJugadorT = nuevoColor;
}

void asignarUnidadesTerritorio(Territorio& t, int nuevasUnidades) {
    t.unidadesT = nuevasUnidades;
}

void modificarUnidadesTerritorio(Territorio& t, int delta) {
    t.unidadesT += delta;
    if (t.unidadesT < 0) {
        t.unidadesT = 0;
    }
}

void agregarVecinoTerritorio(Territorio& t, int codigoVecino) {
    t.vecinosT.push_back(codigoVecino);
}

void limpiarVecinosTerritorio(Territorio& t) {
    t.vecinosT.clear();
}

bool esVecinoTerritorio(const Territorio& t, int codigoVecino) {
    for (int v : t.vecinosT) {
        if (v == codigoVecino) {
            return true;
        }
    }
    return false;
}

bool perteneceAJugador(const Territorio& t, const std::string& color) {
    return t.colorJugadorT == color;
}
