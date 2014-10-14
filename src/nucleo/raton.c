#include <sodero/raton.h>


#define DTR_EN_UNO 1

/**
 * verificar_raton: verifica que exista un raton conectado al sistema
 *
 * @return (int) el exito de la operacion (aun no implementada)
 */
 int verificar_raton () {
    int ret = -1;
    int i;

     //outb (PUERTO_SERIE1, DTR_EN_UNO);  //  poner en uno la senal de DTR del puerto serie
     for (i = 0; i < 50; i++) {  // verificar luego de un rato que el raton
        if (inb (PUERTO_SERIE1) == 0x4D) { // devuelva el ascii de la 'M'
           ret = 0;
           break;
        }
        delay (MICRO (500));
     }
     return (ret);
 }


/**
 * raton_habler: funcion que controla el movimiento del raton atendiendo la 
 * irq que el dispositivo genera (aun no implementada)
 *
 * @return (int) exito de la operacion
 */
int raton_handler () {

    return (0);
}
