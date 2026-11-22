#ifndef ESTRATEGIA_H
#define ESTRATEGIA_H

#include <string>
#include <vector>
#include "Juego.h"

// --- 3.5. TAD GrafoTablero (Componente 3: estrategias de juego) ---
// Grafo no dirigido y ponderado en los vertices que modela el tablero desde el
// punto de vista de un jugador:
//   - Vertice i = territorio i de la partida (0 a TOTAL_TERRITORIOS-1).
//   - Arista (i, j) = los territorios i y j son vecinos.
//   - peso[i]   = unidades de ejercito ubicadas en el territorio i.
//   - propio[i] = true si el territorio i pertenece al jugador analizado.
// Conquistar un territorio enemigo "cuesta" las unidades que lo ocupan; los
// territorios propios cuestan 0 y son los puntos de partida del ataque.
struct GrafoTablero {
    int cantidadVertices;
    std::vector<std::vector<int> > adyacencia;
    std::vector<int> peso;
    std::vector<bool> propio;
};

// Resultado de buscar la ruta de conquista mas barata hacia un territorio.
// camino[0] es el territorio propio desde donde se ataca; los demas son los
// territorios enemigos a conquistar, en orden, y el ultimo es el destino.
struct RutaConquista {
    bool alcanzable;
    int costo;               // Suma de unidades de los territorios a conquistar
    std::vector<int> camino;
};

// Resultado de la busqueda de Dijkstra desde todos los territorios propios.
struct ResultadoDijkstra {
    std::vector<int> costo;      // Costo minimo para conquistar cada vertice (-1 si no alcanzable)
    std::vector<int> saltos;     // Cantidad de territorios a conquistar hasta el vertice
    std::vector<int> predecesor; // Vertice anterior en la ruta (-1 si es propio o no alcanzable)
};

// --- OPERACIONES DEL TAD GRAFOTABLERO ---
void construirGrafoTablero(GrafoTablero& g, const Juego& juego, const std::string& color);
int obtenerCantidadVerticesGrafo(const GrafoTablero& g);
bool esPropioGrafo(const GrafoTablero& g, int vertice);
int obtenerPesoGrafo(const GrafoTablero& g, int vertice);

// Dijkstra con multiples fuentes (todos los territorios propios, costo 0).
void dijkstraConquista(const GrafoTablero& g, ResultadoDijkstra& r);
// Reconstruye la ruta hacia 'destino' a partir del resultado de Dijkstra.
void reconstruirRuta(const ResultadoDijkstra& r, int destino, RutaConquista& ruta);

// --- COMANDOS DEL COMPONENTE 3 ---
void CostoConquista(const std::string& nombreJugador, const std::string& territorio);
void ConquistaMasBarata(const std::string& nombreJugador);

#endif // ESTRATEGIA_H
