#ifndef _DIR_H_
#define _DIR_H_

#include <sodero/syscalls.h>

/**
 * Estructura que representa la entrada de directorio actual. A partir de la
 * cual se va a trabajar.
 */
typedef struct {
   int sector_a_leer;
   int indice;
   int tamanio_dir_raiz;
   int cant_entradas;
} dir;

//int abrir_directorio (dir*);
//int leer_entrada_dir (char*, dir*);
int listar ();

#endif // _DIR_H_
