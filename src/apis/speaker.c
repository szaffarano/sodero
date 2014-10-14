/*
 * Modulo que maneja el parlantito de la PC ( speaker )
 */
#include <speaker.h>
#include <sodero/registros.h>
#include <timer.h>
#include <sodero/puertos.h>

/**
 * sonido: activa el parlante
 * @param frecuencia la frecuencia del sonido con que se activara el speaker
 */
void sonido ( const unsigned int frecuencia ) {
   byte lectura;

   outb ( 0xB6, 0x43 );
   outb ( (1193180/frecuencia), 0x42 );
   outb ( ((1193180/frecuencia)>>8), 0x42 );

   lectura = inb (0x61);

   if ( lectura != (lectura | 3) ) {
      outb ( lectura | 3, 0x61 );
   }
}

/**
 * silencio: desactiva el parlante
 */
void silencio () {
   outb ( (inb(0x61) & 0xFC), 0x61 );
}

/**
 * beep: Produce un sonido
 * @param frecuencia la frecuencia del sonido a producir
 * @param tiempo tiempo que estara el speaker activado
 */
void beep ( const unsigned int frecuencia, const unsigned long tiempo ) {
   sonido ( frecuencia );
   delay ( tiempo );
   silencio ();
}
