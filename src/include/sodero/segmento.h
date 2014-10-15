#ifndef _SEGMENTO_H_
#define _SEGMENTO_H_

/**
 * sodero/segmento.h
 * biblioteca que contiene la deficion de un descriptor de segmento
 */

#include <sodero/definiciones.h>

/**
 * Estructura representando un descriptor de segmento dentro de la GDT/LDT
 */
typedef struct {
  word limite_0_15;
  word base_0_15;
  byte base_16_23;
  unsigned tipo:4;
  unsigned tipo_descriptor:1; // 0 -> sistema 1->codigo
  unsigned dpl:2;
  unsigned presente:1;
  unsigned limite_16_19:4;
  unsigned disponible:1;
  unsigned cero:1; // va siempre cero
  unsigned tamanio_operacion:1;
  unsigned granularidad:1;
  byte base_24_31;
} NOALIGN seg_desc;
/*
 dw limite
 dw base
 db base
 bit   present
 bit:2 dpl
 bit   siempre en 1 
 bit   code
 bit   cnf ever 0
 bit   read ever 1
 bit   accedido ever 0
 4bit  1100 -> granularidad, use32bit, 0, 0
 4bit  limite
 db base
*/
 
   
#endif // _SEGMENTO_H_
