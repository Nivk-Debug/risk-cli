#ifndef HUFFMAN_H
#define HUFFMAN_H

// =============================================================================
// --- 2.2. Componente 2: Codificacion de Huffman ---
// =============================================================================
//
// Formato del archivo binario (.bin), todos los enteros en little-endian:
//   n                 2 bytes  cantidad de caracteres diferentes
//   c1 f1 ... cn fn   1 byte (codigo ASCII) + 8 bytes (frecuencia) por caracter
//   w                 8 bytes  longitud del archivo original (en caracteres)
//   binary_code       secuencia de bits agrupada en bytes; el ultimo byte se
//                     completa con '0' si la secuencia no es multiplo de 8.
//
// En el arbol, la arista al hijo izquierdo vale '0' y al derecho '1'.

const int TOTAL_SIMBOLOS = 256;

// --- TAD NodoHuffman ---
// Las hojas guardan un simbolo y su frecuencia; los nodos internos (y la raiz)
// solo guardan la suma de las frecuencias de sus hijos.
struct NodoHuffman {
    unsigned char simbolo;
    unsigned long long frecuencia;
    NodoHuffman* izquierdo;
    NodoHuffman* derecho;
};

// --- TAD ArbolHuffman ---
struct ArbolHuffman {
    NodoHuffman* raiz;
    char codigos[TOTAL_SIMBOLOS][TOTAL_SIMBOLOS + 1]; // Codigo de cada simbolo como texto de '0' y '1'
};

// --- OPERACIONES DEL TAD ARBOL DE HUFFMAN ---

// Construye el arbol a partir de la tabla de frecuencias (indice = codigo ASCII)
// y calcula el codigo de cada simbolo. El procedimiento es determinista, por lo
// que la misma tabla siempre produce el mismo arbol (necesario para decodificar).
void construirArbolHuffman(ArbolHuffman& arbol, const unsigned long long frecuencias[TOTAL_SIMBOLOS]);
void liberarArbolHuffman(ArbolHuffman& arbol);
bool esHojaHuffman(const NodoHuffman* nodo);
const char* obtenerCodigoHuffman(const ArbolHuffman& arbol, unsigned char simbolo);

// --- CODIFICACION / DECODIFICACION DE ARCHIVOS ---

// Codifica 'texto' (de 'largo' caracteres) y lo guarda en 'nombreArchivo'.
// Retorna true si todo salio bien.
bool comprimirHuffman(const char* texto, unsigned long long largo, const char* nombreArchivo);

// Resultados posibles de descomprimirHuffman
const int HUFFMAN_OK = 0;
const int HUFFMAN_NO_ABRE = 1;      // El archivo no existe o no puede leerse
const int HUFFMAN_VACIO = 2;        // El archivo no tiene contenido
const int HUFFMAN_SIN_FORMATO = 3;  // El contenido no respeta el formato .bin

// Lee 'nombreArchivo' y reconstruye el texto original. Si retorna HUFFMAN_OK,
// 'texto' apunta a un arreglo dinamico (terminado en '\0') que el llamador
// debe liberar con delete[].
int descomprimirHuffman(const char* nombreArchivo, char*& texto, unsigned long long& largo);

#endif // HUFFMAN_H
