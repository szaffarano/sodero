#include <timer.h>
#include <sodero/registros.h>
#include <sodero/puertos.h>


/**
 * setear_temporizador: setea el chip del temporizador 8254 para que dispare 
 * la irq0 en el menor tiempo posible
 */
void setear_temporizador() {
   /**
    * Palabra de control del temporizador 8254:
    *
    * +---+---+---+---+---+---+---+---+
    * | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    * +---+---+---+---+---+---+---+---+
    *   |   |   |   |   |   |   |   |
    *   |   |   |   |   |   |   |   +--> Si 1 la cuenta es BCD sino binario
    *   |   |   |   |   |   |   |
    *   |   |   |   |   +---+---+--> Selecciona el modo ( 0 - 5 )
    *   |   |   |   |
    *   |   |   +---+--> Control de lectura/escritura:
    *   |   |                00 = comando de registro del contador
    *   |   |                01 = lectura/escritura del LSB solamente
    *   |   |                10 = lectura/escritura del MSB solamente
    *   |   |                11 = lectura/escritura del LSB seguido del MSB
    *   |   |
    *   +---+--> Seleccion del contador
    *
    * 0x36 = 00 11 011 0 -> - la cuenta es en binario.
    *                       - el modo es 3.
    *                       - se haran dos escrituras, primero el LSB y luego
    *                         el MSB.
    *                       - seleccionar el contador numero cero.
    *
    * 0x43 -> port que direcciona al registro de control (descrito arriba)
    *
    * 0x40 -> port que direcciona al primer contador (00b en bits 6-7)
    * 0x41 -> port que direcciona al segundo contador (01b en bits 6-7)
    * 0x42 -> port que direcciona al tercer contador (10b en bits 6-7)
    */
   outb ( 0x36, 0x43 ); // se escribe la palabra de control (port 0x43)
                        // descrita arriba.

   outb(INTERVALO & 0xFF, 0x40); // escritura del LSB
   outb(INTERVALO >> 8, 0x40); // escritura del MSB
}
