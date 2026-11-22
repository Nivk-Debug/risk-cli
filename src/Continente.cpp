#include "Continente.h"

// --- IMPLEMENTACION DE OPERACIONES DEL TAD CONTINENTE ---

void inicializarContinente(Continente& c, int codigo, const std::string& nombre, const std::string& colorMapa, int cantidadTerritorios, int bonoUnidades) {
    c.codigoC = codigo;
    c.nombreC = nombre;
    c.colorMapaC = colorMapa;
    c.cantidadTerritoriosC = cantidadTerritorios;
    c.bonoUnidadesC = bonoUnidades;
}

int obtenerCodigoContinente(const Continente& c) {
    return c.codigoC;
}

const std::string& obtenerNombreContinente(const Continente& c) {
    return c.nombreC;
}

const std::string& obtenerColorMapaContinente(const Continente& c) {
    return c.colorMapaC;
}

int obtenerCantidadTerritoriosContinente(const Continente& c) {
    return c.cantidadTerritoriosC;
}

int obtenerBonoContinente(const Continente& c) {
    return c.bonoUnidadesC;
}

void asignarBonoContinente(Continente& c, int nuevoBono) {
    c.bonoUnidadesC = nuevoBono;
}
