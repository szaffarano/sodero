#ifndef _DEFINICIONES_H_
#define _DEFINICIONES_H_

/**
 * sodero/definiciones.h
 * esta biblioteca contiene las definiciones de tipo de datos necesarias
 * para trabajar con bytes, words y double words (referenciar los mismos
 * datos de assembler desde C)
 */

#define NULL 0

#define NOALIGN __attribute__((packed))

#define CLOCK 1193180
#define QUANTUM 1
#define INTERVALO 0xFFFF //( CLOCK / ( 1000/QUANTUM ) )

//#define DEBUG 1

typedef unsigned int dword;
typedef unsigned short word;
typedef unsigned char byte;

#endif //_DEFINICIONES_H_
