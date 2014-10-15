#include <sodero/syscalls.h>
#include <sodero/proceso.h>
#include <sodero/gdt.h>
#include <timer.h>

/**
 * Puntero a la ultima posicion de memoria donde se aloco.
 */
dword ultima_posicion;

/**
 * tipo de algoritmo de memoria implementado
 */
algoritmo_memoria_id algoritmo_memoria = ALG;

extern int recuperar_base ();

/**
 * sys_alocar: asigna un sector de memoria para leer/escribir en funcion del 
 * algoritmo de alocacion de memoria (actualmente implementamos uno que 
 * aloca incrementalmente)
 *
 * @see tipo_algoritmo_memoria
 *
 * @param cantidad (dword) cantidad de bytes que se desean alocar en memoria
 *
 * @return (int) posicion de memoria asignada para escribir
 */
int sys_alocar (const dword cantidad) {
   static dword aux;
   switch (algoritmo_memoria) {
     case ALG:
       aux = ultima_posicion;
       ultima_posicion += cantidad; 
       break;
   }
   return aux;
}
