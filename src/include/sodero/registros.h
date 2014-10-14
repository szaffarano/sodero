#ifndef _REGISTROS_H_
#define _REGISTROS_H_

/**
 * sodero/registros.h
 * esta biblioteca contiene los headers de funciones de bajo nivel
 * que obtienen los valores de los registros del CPU
 */

#include <sodero/definiciones.h>

#define sti() __asm__ ("sti"::)
#define cli() __asm__ ("cli"::)

/**
 * Devuelve el Stack Segment
 */
word get_ss();

/**
 * Devuelve el Stack Segment
 */
word get_es();

/**
 * Devuelve el Segmento de Pila
 */
word get_cs();

/**
 * Devuelve el El segmento de Datos
 */
word get_ds();

/**
 * Devuelve el Segmento F
 */
word get_fs();

/**
 * Devuelve el Segmento G
 */
word get_gs();

/**
 * Devuelve el Puntero de Pila (ESP)
 */
dword get_esp();

#endif //_REGISTROS_H_
