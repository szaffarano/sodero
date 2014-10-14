#ifndef _SYS_EJECUTAR_H_
#define _SYS_EJECUTAR_H_

/**
 * sys_ejecutar.h Biblioteca que incluye los include necesarios para el 
 * funcionamiento de la sys_call ejecutar.
 */

#include <timer.h>
#include <sodero/registros.h>
#include <sodero/memoria.h>
#include <video.h>
#include <sodero/idt.h>
#include <fat.h>
#include <sodero/disquete.h>
#include <sodero/proceso.h>
#include <sodero/gdt.h>

/**
 * sys_ejecutar: System call utilizada para ejecutar un proceso. Se cargan todo
 * el entorno del proceso en memoria y setean todos los segmentos que les
 * correspondan.
 */
int sys_ejecutar ( char*, char** );

#endif // _SYS_EJECUTAR_H_
