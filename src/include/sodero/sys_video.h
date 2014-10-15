#ifndef SYS_VIDEO_H
#define SYS_VIDEO_H

#include <video.h>
#include <sodero/teclado.h>
#include <sodero/puertos.h>

/**
 * Direccion de memoria de Video
 */
#define MEM_VIDEO 0x000B8000

/**
 * Devuelve la direccion de memoria para escribir un char en las
 * coordenadas (x,y)
 */
#define POSICION(pos_x, pos_y) ((dword)(MEM_VIDEO + (160 * pos_y + pos_x * 2)))

void tabulador ();
void scroll ();
void nueva_linea ();
void borrar_caracter ();

extern void sys_setear_x ( dword );
extern void sys_setear_y ( dword );
extern void sys_setear_color ( byte );
extern dword sys_obtener_x ();
extern dword sys_obtener_y ();
extern byte sys_obtener_color ();

int sys_printc_asm ( byte );

#endif //SYS_VIDEO_H
