#ifndef _RATON_H
#define _RATON_H


#include <timer.h>
#include <sodero/puertos.h>

#define PUERTO_SERIE1  0x3F8   //COM1
#define PUERTO_SERIE2  0x2F8   //COM2

int verificar_raton ();
int raton_handler ();


#endif // _RATON_H
