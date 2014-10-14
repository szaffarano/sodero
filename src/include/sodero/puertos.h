#ifndef _PUERTOS_H_
#define _PUERTOS_H_

#include <sodero/definiciones.h>

/**
 * inb: funcion que lee un byte de un puerto determinado (implementada en 
 * assembler @see puertos.asm)
 *
 * @param (word) numero de puerto a leer
 *
 * @return (byte) lectura realizada
 */
byte inb(word);

/**
 * inw: funcion que lee una palabea de un puerto determinado (implementada en 
 * assembler @see puertos.asm)
 *
 * @param (word) numero de puerto a leer
 *
 * @return (word) lectura realizada
 */
word inw(word);

/**
 * outb: funcion que escribe un byte en un puerto determinado (implementada en
 * assembler @see puertos.asm)
 *
 * @param valor (byte) valor a escribir en el puerto
 *
 * @param puerto (word) numero de puerto donde escribir
 */
void outb(const byte valor, const word puerto);

/**
 * outw: funcion que escribe una palabra en un puerto determinado (implementada
 * en assembler @see puertos.asm)
 *
 * @param valor (word) valor a escribir en el puerto
 *
 * @param puerto (word) numero de puerto donde escribir
 */
void outw(const word valor, const word puerto);

#endif //_PUERTOS_H_
