#ifndef TERRITORIO_H
#define TERRITORIO_H

#include <string>
#include <vector>

// --- 3.1. TAD Territorio ---
struct Territorio {
    int codigoT;
    std::string nombreT;
    int continenteT; // Identificador del continente (0 al 5)
    std::string colorJugadorT;
    int unidadesT;
    std::vector<int> vecinosT; // Sin limite fijo: crece segun el mapa cargado
};

// --- OPERACIONES DEL TAD TERRITORIO ---

// Constructor / Inicializador
void inicializarTerritorio(Territorio& t, int codigo, const std::string& nombre, int continente, const std::string& colorJugador, int unidades);

// Getters (Consultores)
int obtenerCodigoTerritorio(const Territorio& t);
const std::string& obtenerNombreTerritorio(const Territorio& t);
int obtenerContinenteTerritorio(const Territorio& t);
const std::string& obtenerColorTerritorio(const Territorio& t);
int obtenerUnidadesTerritorio(const Territorio& t);
int obtenerCantidadVecinosTerritorio(const Territorio& t);
int obtenerVecinoEnTerritorio(const Territorio& t, int indice);

// Setters y Modificadores
void asignarCodigoTerritorio(Territorio& t, int nuevoCodigo);
void asignarColorTerritorio(Territorio& t, const std::string& nuevoColor);
void asignarUnidadesTerritorio(Territorio& t, int nuevasUnidades);
void modificarUnidadesTerritorio(Territorio& t, int delta);
void agregarVecinoTerritorio(Territorio& t, int codigoVecino);
void limpiarVecinosTerritorio(Territorio& t);

// Operaciones de Dominio
bool esVecinoTerritorio(const Territorio& t, int codigoVecino);
bool perteneceAJugador(const Territorio& t, const std::string& color);

#endif // TERRITORIO_H
