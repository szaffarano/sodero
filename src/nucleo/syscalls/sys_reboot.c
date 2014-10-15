#include <reboot.h>
#include <video.h>
#include <timer.h>

/**
 * sys_reboot: handler de la system call que permite resetear la PC
 *
 * @return (int) exito de la operacion
 */
int sys_reboot ( ) {
   int i = 5;
   imprimir_cadena ("\nRESETEANDO LA PC ");
   while ( i > 0 ) {
      imprimir_entero ( i-- );
      printc ( ' ' );
      delay ( MICRO ( 500000L ) );
   }
   /**
    * Rutina de reset: MUCHAS GRACIAS LINUS (de nuevo ;-)
    */
   __asm__ __volatile__ ( 
                          "pushl %eax\n" \
                          "1: inb $0x64, %al\n" \
                          "testb $0x02, %al\n" \
                          "jne 1b\n" \
                          "popl %eax\n" \
                          "movb $0xFC, %al\n" \
                          "outb %al, $0x64\n" );

   //                       "movw $0x1234, 0x472\n"
   return 0;
}
