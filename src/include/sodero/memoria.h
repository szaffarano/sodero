#ifndef _MEMORIA_H_
#define _MEMORIA_H_

#include <sodero/definiciones.h>

typedef enum {
    ALG
} algoritmo_memoria_id;

void* alocar (const dword);
void liberar (void*);
void* setear_memoria (void*, const int, const dword);

#endif //_MEMORIA_H_
