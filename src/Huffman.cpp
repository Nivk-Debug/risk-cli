#include "Huffman.h"
#include <fstream>
#include <cstring>

using namespace std;

// --- CONSTRUCCION DEL ARBOL ---

NodoHuffman* crearNodoHuffman(unsigned char simbolo, unsigned long long frecuencia,
                              NodoHuffman* izquierdo, NodoHuffman* derecho) {
    NodoHuffman* nodo = new NodoHuffman;
    nodo->simbolo = simbolo;
    nodo->frecuencia = frecuencia;
    nodo->izquierdo = izquierdo;
    nodo->derecho = derecho;
    return nodo;
}

bool esHojaHuffman(const NodoHuffman* nodo) {
    return nodo != nullptr && nodo->izquierdo == nullptr && nodo->derecho == nullptr;
}

// Indice del nodo de menor frecuencia en la lista (el primero en caso de empate)
int buscarMinimo(NodoHuffman* lista[], int cantidad) {
    int minimo = 0;
    for (int i = 1; i < cantidad; i++) {
        if (lista[i]->frecuencia < lista[minimo]->frecuencia) {
            minimo = i;
        }
    }
    return minimo;
}

// Saca el elemento 'indice' de la lista conservando el orden de los demas
NodoHuffman* extraerDeLista(NodoHuffman* lista[], int& cantidad, int indice) {
    NodoHuffman* nodo = lista[indice];
    for (int i = indice; i < cantidad - 1; i++) {
        lista[i] = lista[i + 1];
    }
    cantidad--;
    return nodo;
}

// Recorre el arbol acumulando '0' (izquierda) y '1' (derecha) hasta las hojas
void generarCodigos(ArbolHuffman& arbol, const NodoHuffman* nodo, char* camino, int profundidad) {
    if (nodo == nullptr) return;
    if (esHojaHuffman(nodo)) {
        if (profundidad == 0) {
            // Mensaje con un solo simbolo distinto: se le asigna el codigo "0"
            camino[profundidad++] = '0';
        }
        camino[profundidad] = '\0';
        strcpy(arbol.codigos[nodo->simbolo], camino);
        return;
    }
    camino[profundidad] = '0';
    generarCodigos(arbol, nodo->izquierdo, camino, profundidad + 1);
    camino[profundidad] = '1';
    generarCodigos(arbol, nodo->derecho, camino, profundidad + 1);
}

void construirArbolHuffman(ArbolHuffman& arbol, const unsigned long long frecuencias[TOTAL_SIMBOLOS]) {
    arbol.raiz = nullptr;
    for (int s = 0; s < TOTAL_SIMBOLOS; s++) {
        arbol.codigos[s][0] = '\0';
    }

    // Una hoja por cada simbolo presente, en orden de codigo ASCII
    NodoHuffman* lista[TOTAL_SIMBOLOS];
    int cantidad = 0;
    for (int s = 0; s < TOTAL_SIMBOLOS; s++) {
        if (frecuencias[s] > 0) {
            lista[cantidad++] = crearNodoHuffman((unsigned char)s, frecuencias[s], nullptr, nullptr);
        }
    }
    if (cantidad == 0) return;

    // Se unen repetidamente los dos nodos de menor frecuencia: el primero
    // queda como hijo izquierdo y el segundo como hijo derecho.
    while (cantidad > 1) {
        NodoHuffman* menor1 = extraerDeLista(lista, cantidad, buscarMinimo(lista, cantidad));
        NodoHuffman* menor2 = extraerDeLista(lista, cantidad, buscarMinimo(lista, cantidad));
        lista[cantidad++] = crearNodoHuffman(0, menor1->frecuencia + menor2->frecuencia, menor1, menor2);
    }
    arbol.raiz = lista[0];

    char camino[TOTAL_SIMBOLOS + 1];
    generarCodigos(arbol, arbol.raiz, camino, 0);
}

void liberarNodos(NodoHuffman* nodo) {
    if (nodo == nullptr) return;
    liberarNodos(nodo->izquierdo);
    liberarNodos(nodo->derecho);
    delete nodo;
}

void liberarArbolHuffman(ArbolHuffman& arbol) {
    liberarNodos(arbol.raiz);
    arbol.raiz = nullptr;
}

const char* obtenerCodigoHuffman(const ArbolHuffman& arbol, unsigned char simbolo) {
    return arbol.codigos[simbolo];
}

// --- ESCRITURA / LECTURA DE ENTEROS EN LITTLE-ENDIAN ---

void escribirEntero(ofstream& salida, unsigned long long valor, int bytes) {
    for (int i = 0; i < bytes; i++) {
        char b = (char)((valor >> (8 * i)) & 0xFF);
        salida.write(&b, 1);
    }
}

bool leerEntero(ifstream& entrada, unsigned long long& valor, int bytes) {
    valor = 0;
    for (int i = 0; i < bytes; i++) {
        char b;
        if (!entrada.read(&b, 1)) return false;
        valor |= ((unsigned long long)(unsigned char)b) << (8 * i);
    }
    return true;
}

// --- CODIFICACION ---

bool comprimirHuffman(const char* texto, unsigned long long largo, const char* nombreArchivo) {
    if (texto == nullptr || largo == 0) return false;

    unsigned long long frecuencias[TOTAL_SIMBOLOS] = {0};
    for (unsigned long long i = 0; i < largo; i++) {
        frecuencias[(unsigned char)texto[i]]++;
    }

    ArbolHuffman* arbol = new ArbolHuffman; // Dinamico: la tabla de codigos ocupa ~64 KB
    construirArbolHuffman(*arbol, frecuencias);

    ofstream salida(nombreArchivo, ios::binary);
    if (!salida.is_open()) {
        liberarArbolHuffman(*arbol);
        delete arbol;
        return false;
    }

    // n: cantidad de caracteres diferentes (2 bytes)
    int distintos = 0;
    for (int s = 0; s < TOTAL_SIMBOLOS; s++) {
        if (frecuencias[s] > 0) distintos++;
    }
    escribirEntero(salida, distintos, 2);

    // ci (1 byte) y fi (8 bytes) por cada caracter
    for (int s = 0; s < TOTAL_SIMBOLOS; s++) {
        if (frecuencias[s] > 0) {
            escribirEntero(salida, s, 1);
            escribirEntero(salida, frecuencias[s], 8);
        }
    }

    // w: longitud del archivo original (8 bytes)
    escribirEntero(salida, largo, 8);

    // binary_code: los bits se agrupan de a 8 (el primer bit es el mas significativo)
    unsigned char byteActual = 0;
    int bitsEnByte = 0;
    for (unsigned long long i = 0; i < largo; i++) {
        const char* codigo = obtenerCodigoHuffman(*arbol, (unsigned char)texto[i]);
        for (int k = 0; codigo[k] != '\0'; k++) {
            byteActual = (unsigned char)(byteActual << 1);
            if (codigo[k] == '1') byteActual |= 1;
            bitsEnByte++;
            if (bitsEnByte == 8) {
                salida.write((const char*)&byteActual, 1);
                byteActual = 0;
                bitsEnByte = 0;
            }
        }
    }
    if (bitsEnByte > 0) {
        byteActual = (unsigned char)(byteActual << (8 - bitsEnByte)); // Relleno con '0'
        salida.write((const char*)&byteActual, 1);
    }

    bool ok = salida.good();
    salida.close();
    liberarArbolHuffman(*arbol);
    delete arbol;
    return ok;
}

// --- DECODIFICACION ---

int descomprimirHuffman(const char* nombreArchivo, char*& texto, unsigned long long& largo) {
    texto = nullptr;
    largo = 0;

    ifstream entrada(nombreArchivo, ios::binary);
    if (!entrada.is_open()) return HUFFMAN_NO_ABRE;

    entrada.seekg(0, ios::end);
    if (entrada.tellg() == 0) return HUFFMAN_VACIO;
    entrada.seekg(0, ios::beg);

    unsigned long long distintos;
    if (!leerEntero(entrada, distintos, 2) || distintos == 0 || distintos > TOTAL_SIMBOLOS) {
        return HUFFMAN_SIN_FORMATO;
    }

    unsigned long long frecuencias[TOTAL_SIMBOLOS] = {0};
    unsigned long long sumaFrecuencias = 0;
    for (unsigned long long i = 0; i < distintos; i++) {
        unsigned long long simbolo, frecuencia;
        if (!leerEntero(entrada, simbolo, 1) || !leerEntero(entrada, frecuencia, 8)) {
            return HUFFMAN_SIN_FORMATO;
        }
        if (frecuencia == 0 || frecuencias[simbolo] != 0) {
            return HUFFMAN_SIN_FORMATO; // Frecuencia invalida o caracter repetido
        }
        frecuencias[simbolo] = frecuencia;
        sumaFrecuencias += frecuencia;
    }

    unsigned long long w;
    if (!leerEntero(entrada, w, 8) || w != sumaFrecuencias || w > 100000000ULL) {
        return HUFFMAN_SIN_FORMATO;
    }

    ArbolHuffman* arbol = new ArbolHuffman;
    construirArbolHuffman(*arbol, frecuencias);

    char* resultado = new char[w + 1];
    unsigned long long escritos = 0;
    const NodoHuffman* nodo = arbol->raiz;
    bool unSoloSimbolo = esHojaHuffman(arbol->raiz);
    char byteLeido;

    while (escritos < w && entrada.read(&byteLeido, 1)) {
        for (int bit = 7; bit >= 0 && escritos < w; bit--) {
            bool esUno = (((unsigned char)byteLeido >> bit) & 1) != 0;
            if (unSoloSimbolo) {
                resultado[escritos++] = (char)arbol->raiz->simbolo; // Cada bit es un simbolo
                continue;
            }
            nodo = esUno ? nodo->derecho : nodo->izquierdo;
            if (esHojaHuffman(nodo)) {
                resultado[escritos++] = (char)nodo->simbolo;
                nodo = arbol->raiz;
            }
        }
    }

    liberarArbolHuffman(*arbol);
    delete arbol;

    if (escritos != w) {
        delete[] resultado; // Faltaron bits: archivo truncado
        return HUFFMAN_SIN_FORMATO;
    }
    resultado[w] = '\0';
    texto = resultado;
    largo = w;
    return HUFFMAN_OK;
}
