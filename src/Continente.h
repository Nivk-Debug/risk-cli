#ifndef CONTINENTE_H
#define CONTINENTE_H

#include <string>

// --- 3.2. TAD Continente ---
struct Continente {
    int codigoC;
    std::string nombreC;
    std::string colorMapaC;
    int cantidadTerritoriosC;
    int bonoUnidadesC;
};

// --- OPERACIONES DEL TAD CONTINENTE ---

// Constructor / Inicializador
void inicializarContinente(Continente& c, int codigo, const std::string& nombre, const std::string& colorMapa, int cantidadTerritorios, int bonoUnidades);

// Getters (Consultores)
int obtenerCodigoContinente(const Continente& c);
const std::string& obtenerNombreContinente(const Continente& c);
const std::string& obtenerColorMapaContinente(const Continente& c);
int obtenerCantidadTerritoriosContinente(const Continente& c);
int obtenerBonoContinente(const Continente& c);

// Setters
void asignarBonoContinente(Continente& c, int nuevoBono);

#endif // CONTINENTE_H
