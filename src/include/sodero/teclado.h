#ifndef _TECLADO_H_
#define _TECLADO_H_

#include <sodero/definiciones.h>
#include <sodero/proceso.h>
#include <speaker.h>

#define CANT_CHAR 60

#define BACKSPACE 14
#define CONTROL 29
#define SHIFT_IZQUIERDO 42
#define UNSHIFT_IZQUIERDO (42 | 0x80)
#define SHIFT_DERECHO 54
#define UNSHIFT_DERECHO (54 | 0x80)
#define ALT 56
#define CAPS_LOCK 58 //Averiguar si no es: 250 - 186.
#define NUM_LOCK 69 //AVERIGUAR. Sino 197
#define SCROLL_LOCK 70 //AVERIGUAR. Sino 198
#define ENTER 28
#define TAB 15
//flechas direccionales.
#define FLECHA_ARRIBA 0x48
#define FLECHA_IZQUIERDA 0x4B
#define FLECHA_DERECHA 0x4D
#define FLECHA_ABAJO 0x50

#define ASCII_ENTER '\n'
#define ASCII_TABULADOR '\t'
#define ASCII_BACKSPACE '\b'
#define ASCII_FIN_LINEA '\0'

extern pcb* tarea_actual;

void inicializa_teclado ();
void teclado_handler (dword);
void setear_leds (byte, byte, byte); 
char obtener_caracter_teclado ();

#endif //_TECLADO_H_
