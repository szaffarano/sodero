#ifndef _VIDEO_H_
#define _VIDEO_H_

/**
 * video.h
 * Biblioteca que contiene todas las funciones del driver de video
 */

#include <sodero/definiciones.h>

#define LIMITE 255

typedef char* va_list;

// macros
/**
 *  Constantes que representan los colores
 */
#define NEGRO 0
#define AZUL 1
#define VERDE 2
#define CIAN 3
#define ROJO 4
#define MAGENTA 5
#define MARRON 6
#define BLANCO 7
#define GRIS 8
#define AZUL_CLARO 9
#define VERDE_CLARO 10
#define CYAN_CLARO 11
#define ROJO_CLARO 12
#define MAGENTA_CLARO 13
#define AMARILLO 14
#define BLANCO_BRILLATE 15



/**
 * Utilizada por imprimir(...) para manejar argumentos variables
 * Extraida del compilador GNU/gcc @see http://www.gnu.org
 */
#define va_rounded_size(type) \
(((sizeof (type) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

/**
 * Utilizada por imprimir(...) para manejar argumentos variables
 * Extraida del compilador GNU/gcc @see http://www.gnu.org
 */
#define va_start(ap, v) \
((void) (ap = (va_list) &v + va_rounded_size (v)))

/**
 * Utilizada por imprimir(...) para manejar argumentos variables
 * Extraida del compilador GNU/gcc @see http://www.gnu.org
 */
#define va_arg(ap, type) \
(ap += va_rounded_size (type), *((type *)(ap - va_rounded_size (type))))

/**
 * Utilizada por imprimir(...) para manejar argumentos variables
 * Extraida del compilador GNU/gcc @see http://www.gnu.org
 */
#define va_end(ap) ((void) (ap = 0))


/**
 * Imprime la cadena que recibe como argumento
 */
int imprimir_cadena (const char*);

/**
 * Obtiene la longitud de una cadena (quiza deberia ir en otra ubicacion!)
 */
int longitud_cadena (const char*);

/**
 * Imprime un numero entero por pantalla
 */
void imprimir_entero (const int);

/**
 * Imprime un numero entero por pantalla en formato hexa
 */
void imprimir_hexa (const int);

/**
 * Imprime un numero de 16 bits por pantalla en formato hexa
 */
void imprimir_word_hexa (const word);

/**
 * Imprime un numero de 8 bits por pantalla en formato hexa
 */
void imprimir_byte_hexa (const byte);

/**
 * Imprime una cadena por pantalla, aceptando argumentos variables
 */
void imprimir (const char*, ... );
char* ingresar_cadena ( char* );

int printc ( char ch );
int getc ();
int ctoi (char* buf);
void itoa (int number, char* buf);
void itoh (int number, char* buf);
void wtoa (word number, char* buf);
void wtoh (word number, char* buf);
void btoa (byte number, char* buf);
void btoh (byte number, char* buf);

#endif //_VIDEO_H_
