#include "Estrategia.h"
#include <iostream>

using namespace std;

// --- IMPLEMENTACION DEL TAD GRAFOTABLERO ---

void construirGrafoTablero(GrafoTablero& g, const Juego& juego, const string& color) {
    g.cantidadVertices = TOTAL_TERRITORIOS;
    g.adyacencia.assign(TOTAL_TERRITORIOS, vector<int>());
    g.peso.assign(TOTAL_TERRITORIOS, 0);
    g.propio.assign(TOTAL_TERRITORIOS, false);

    for (int i = 0; i < TOTAL_TERRITORIOS; i++) {
        const Territorio& t = obtenerTerritorioConst(juego, i);
        g.peso[i] = obtenerUnidadesTerritorio(t);
        g.propio[i] = perteneceAJugador(t, color);
        for (int k = 0; k < obtenerCantidadVecinosTerritorio(t); k++) {
            g.adyacencia[i].push_back(obtenerVecinoEnTerritorio(t, k));
        }
    }
}

int obtenerCantidadVerticesGrafo(const GrafoTablero& g) {
    return g.cantidadVertices;
}

bool esPropioGrafo(const GrafoTablero& g, int vertice) {
    return g.propio[vertice];
}

int obtenerPesoGrafo(const GrafoTablero& g, int vertice) {
    return g.peso[vertice];
}

// Dijkstra O(V^2) con pesos en los vertices. Todos los territorios propios
// son fuentes con costo 0. Solo se expanden hacia territorios enemigos: pasar
// por un territorio propio nunca conviene, porque ese territorio ya es una
// fuente de costo 0. El orden de comparacion es (costo, saltos), de modo que
// ante igual costo se prefiere la ruta con menos territorios.
void dijkstraConquista(const GrafoTablero& g, ResultadoDijkstra& r) {
    int n = obtenerCantidadVerticesGrafo(g);
    r.costo.assign(n, -1);
    r.saltos.assign(n, -1);
    r.predecesor.assign(n, -1);
    vector<bool> definitivo(n, false);

    for (int i = 0; i < n; i++) {
        if (esPropioGrafo(g, i)) {
            r.costo[i] = 0;
            r.saltos[i] = 0;
        }
    }

    for (int iteracion = 0; iteracion < n; iteracion++) {
        // Vertice no definitivo con menor (costo, saltos)
        int u = -1;
        for (int i = 0; i < n; i++) {
            if (definitivo[i] || r.costo[i] == -1) continue;
            if (u == -1 || r.costo[i] < r.costo[u] ||
                (r.costo[i] == r.costo[u] && r.saltos[i] < r.saltos[u])) {
                u = i;
            }
        }
        if (u == -1) break; // Lo que queda no es alcanzable
        definitivo[u] = true;

        for (size_t k = 0; k < g.adyacencia[u].size(); k++) {
            int v = g.adyacencia[u][k];
            if (definitivo[v] || esPropioGrafo(g, v)) continue;
            int nuevoCosto = r.costo[u] + obtenerPesoGrafo(g, v);
            int nuevosSaltos = r.saltos[u] + 1;
            if (r.costo[v] == -1 || nuevoCosto < r.costo[v] ||
                (nuevoCosto == r.costo[v] && nuevosSaltos < r.saltos[v])) {
                r.costo[v] = nuevoCosto;
                r.saltos[v] = nuevosSaltos;
                r.predecesor[v] = u;
            }
        }
    }
}

void reconstruirRuta(const ResultadoDijkstra& r, int destino, RutaConquista& ruta) {
    ruta.camino.clear();
    if (r.costo[destino] == -1) {
        ruta.alcanzable = false;
        ruta.costo = 0;
        return;
    }
    ruta.alcanzable = true;
    ruta.costo = r.costo[destino];
    for (int v = destino; v != -1; v = r.predecesor[v]) {
        ruta.camino.insert(ruta.camino.begin(), v);
    }
}

// --- UTILIDADES DE PRESENTACION ---

// Valida juego inicializado, no terminado y jugador existente y activo.
// A diferencia de los comandos de turno, el analisis no exige que sea su turno.
static bool validarJugadorEstrategia(const string& nombreJugador, int& indiceJugador) {
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
    return true;
}

static const string& nombreDeVertice(int vertice) {
    return obtenerNombreTerritorio(obtenerTerritorioConst(partidaActual, vertice));
}

// "Para conquistar el territorio X, J debe atacar desde A, pasando por los
// territorios B, C, ... Debe conquistar n unidades de ejercito." La parte
// "pasando por" se omite si X es vecino directo de A; las n unidades incluyen
// las de los territorios intermedios y las del propio X.
static void imprimirRuta(const string& nombreJugador, const RutaConquista& ruta) {
    int destino = ruta.camino.back();
    cout << "Para conquistar el territorio " << nombreDeVertice(destino) << ", "
         << nombreJugador << " debe atacar desde " << nombreDeVertice(ruta.camino[0]);
    // Territorios intermedios (entre el origen y el destino, sin incluirlos)
    if (ruta.camino.size() > 2) {
        cout << ", pasando por los territorios ";
        for (size_t i = 1; i + 1 < ruta.camino.size(); i++) {
            if (i > 1) cout << ", ";
            cout << nombreDeVertice(ruta.camino[i]);
        }
    }
    cout << ". Debe conquistar " << ruta.costo << " unidades de ejercito." << endl;
}

// --- COMANDOS DEL COMPONENTE 3 ---

void CostoConquista(const string& nombreJugador, const string& territorio) {
    int idxJugador;
    if (!validarJugadorEstrategia(nombreJugador, idxJugador)) return;

    int idxDestino = buscarIndiceTerritorio(partidaActual, territorio);
    if (idxDestino == -1) {
        cout << "(Territorio no valido) El territorio " << territorio << " no existe en el tablero." << endl;
        return;
    }

    const string& color = obtenerColorJugador(obtenerJugadorConst(partidaActual, idxJugador));
    GrafoTablero g;
    construirGrafoTablero(g, partidaActual, color);

    if (esPropioGrafo(g, idxDestino)) {
        cout << "(Territorio propio) El jugador " << nombreJugador << " ya controla el territorio "
             << territorio << ". No hay nada que conquistar." << endl;
        return;
    }

    ResultadoDijkstra r;
    dijkstraConquista(g, r);
    RutaConquista ruta;
    reconstruirRuta(r, idxDestino, ruta);

    if (!ruta.alcanzable) {
        cout << "(Sin ruta) El jugador " << nombreJugador << " no tiene territorios desde donde atacar "
             << territorio << "." << endl;
        return;
    }
    imprimirRuta(nombreJugador, ruta);
}

void ConquistaMasBarata(const string& nombreJugador) {
    int idxJugador;
    if (!validarJugadorEstrategia(nombreJugador, idxJugador)) return;

    const string& color = obtenerColorJugador(obtenerJugadorConst(partidaActual, idxJugador));
    GrafoTablero g;
    construirGrafoTablero(g, partidaActual, color);

    ResultadoDijkstra r;
    dijkstraConquista(g, r);

    // Territorio enemigo alcanzable con menor costo (desempate: menos saltos,
    // luego el de menor posicion en el tablero)
    int mejor = -1;
    for (int i = 0; i < obtenerCantidadVerticesGrafo(g); i++) {
        if (esPropioGrafo(g, i) || r.costo[i] == -1) continue;
        if (mejor == -1 || r.costo[i] < r.costo[mejor] ||
            (r.costo[i] == r.costo[mejor] && r.saltos[i] < r.saltos[mejor])) {
            mejor = i;
        }
    }

    if (mejor == -1) {
        cout << "(Sin conquista posible) El jugador " << nombreJugador
             << " no tiene territorios enemigos que pueda atacar." << endl;
        return;
    }

    RutaConquista ruta;
    reconstruirRuta(r, mejor, ruta);
    cout << "La conquista mas barata es avanzar sobre el territorio " << nombreDeVertice(mejor)
         << " desde el territorio " << nombreDeVertice(ruta.camino[0]) << ". ";
    imprimirRuta(nombreJugador, ruta);
}
