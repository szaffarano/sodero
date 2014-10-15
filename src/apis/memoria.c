/*
 * Modulo del manejo de memoria
 */
#include <sodero/memoria.h>
#include <sodero/syscalls.h>

/**
 * alocar: reserva espacio en memoria para una determinada cantidad de bytes
 * @param cantidad cantidad de bytes a reservar
 * @return posicion de memoria de la ubicacion reservada
 */
void* alocar (const dword cantidad) {
    return (void*)llamar_sys_call ( SYS_ALOCAR, (dword)cantidad, 0, 0 );
}

/**
 * liberar: libera una posicion de memoria
 * @param puntero posicion de memoria a liberar
 */
void liberar (void* puntero) {
}

/**
 * setear_memoria: setea una posicion de memoria con un determinado valor
 * @param direccion direccion de memoria a setear
 * @param valor valor con el que se seteara la direccion de memoria
 * @param tamanio cantidad de bytes que se setearan
 *
 * @return posicion de memoria seteada
 */
void* setear_memoria (void* direccion, const int valor, const dword tamanio) {
   dword i;
   for ( i = 0; i < tamanio; i++ ) {
      ((unsigned char*)direccion)[i] = valor;
   }
   return direccion;
}
