/*
 * Modulo con toda la logica del manejo de pantalla
 */
#include <sodero/sys_video.h>
#include <sodero/proceso.h>

#define TAM_TABULADOR 8

// funciones privadas al modulo
static inline void incrementar_coordenadas ();
static void actualizar_cursor ();

/**
 * sys_printc: imprime un caracter en las coordenadas especificas
 *
 * @param ch caracter a imprimir
 * @return estado de la operacion
 *
 */
int sys_printc ( char ch ) {
   switch ( ch ) {
      case ASCII_BACKSPACE:
         borrar_caracter ();
         break;
      case ASCII_ENTER:
         nueva_linea (); 
         break;
      case ASCII_TABULADOR:
         tabulador ();
         break;
      default:
         sys_printc_asm ( ch );
         incrementar_coordenadas ();
         break;
   }
   actualizar_cursor ();
   return 1;
}


/**
 * tabulador: Imprime un TAB por pantalla
 */
void tabulador () {
   static int indice;
   static int cant_espacios;

   int pos_x = sys_obtener_x ();

   cant_espacios = ( ( (pos_x / TAM_TABULADOR) + 1 ) * TAM_TABULADOR ) - pos_x;
   for ( indice = 0; indice < cant_espacios; indice++ ) {
      sys_printc_asm ( ' ' );
      incrementar_coordenadas ();
   }
}

/**
 * nueva_linea: Imprime una nueva linea
 */
void nueva_linea () {
   int pos_y = sys_obtener_y ();
   sys_setear_x ( 0 );
   if ( pos_y == 24 ) {
      scroll ();
   } else {
      sys_setear_y ( ++pos_y );
   }
}

/**
 * borrar_caracter: borra un caracter de pantalla y actualiza el cursor
 */
void borrar_caracter () {
   int pos_x = sys_obtener_x ();
   int pos_y = sys_obtener_y ();
   if ( --pos_x == -1 ) {
      pos_x = 79;
      sys_setear_y ( --pos_y );
   }
   sys_setear_x ( pos_x );
   //sys_printc_asm  ( ' ', POSICION (pos_x, pos_y) );
   sys_printc_asm  ( ' ' );
   //actualizar_cursor ();
}


/**
 * scroll: Desplaza la pantalla hacia arriba una linea
 */
void scroll () {
   static int i, j;
   static char* memoria_actual;
   static char* memoria_anterior;

   memoria_actual = (char*) POSICION (0,1);
   memoria_anterior = (char*) POSICION (0,0);

   for ( i = 0; i < 80; i++ ) {
      for ( j = 0; j < 24; j++ ) {
         *memoria_anterior++ = *memoria_actual++;
         *memoria_anterior++ = *memoria_actual++;
      }
   }
   for ( i = 0; i < 80; i++ ) {
      *memoria_anterior++ = ' ';
      *memoria_anterior++ = sys_obtener_color ();
   }

   sys_setear_y ( 24 );
   sys_setear_x ( 0 );
}


/**
 * actualizar_cursor: actualiza la posicion del cursor en base a las
 *                    coordenadas actuales.
 * <pre>
 * lo que hay que escribirle a 0x3D5 es (para un par X,Y): Y * 80 + X
 * por ejemplo, para (10,8) -> 8 * 80 + 10 = 650 = 0x028A = 0000001010001010
 * pero se le escribe en dos partes, el word mas bajo (seteando
 * el port 0x3D4 con 0xF) en este caso 0x8A, y el word mas alto, seteando
 * el port 0x3D4 con 0xE), en este caso con 0x02.
 * </pre>
 */
static void actualizar_cursor() {
   int pos_x;
   int pos_y;

   pos_x = sys_obtener_x ();
   pos_y = sys_obtener_y ();

   outb (0xF, 0x3D4);
   outb ((pos_y * 80 + pos_x) & 0x00FF, 0x3D5);
   outb (0xE, 0x3D4);
   outb (0x2, 0x3D5);
   outb (((pos_y * 80 + pos_x) >> 8) & 0x00FF, 0x3D5);
}


/**
 * incrementar_coordenadas: Funcion privada al modulo que incrementa las 
 *                          coordenadas por cada caracter que imprime.
 */
static inline void incrementar_coordenadas () {
   int pos_x = sys_obtener_x ();
   int pos_y = sys_obtener_y ();

   if ( ++pos_x == 80 ) {
      if ( pos_y == 24 ) {
         scroll();
      } else {
         pos_y++;
      }
      pos_x = 0;
   }
    sys_setear_x ( pos_x );
    sys_setear_y ( pos_y );
}

/**
 * sys_getc: syscall que obtiene un caracter leido por teclado.
 * @return el caracter leido
 */
int sys_getc () {
   static char buffer [LIMITE];
   static int caracteres_leidos = 0;
   static int puntero_lectura = 0;
   static int puntero_escritura = 0;

   int ret = 0;

   if ( caracteres_leidos == 0 ) {
      while ( ret != ASCII_ENTER && caracteres_leidos < LIMITE ) {
         while ( ( ret = obtener_caracter_teclado () ) == 0 );
         if ( ret != ASCII_BACKSPACE ) {
            caracteres_leidos++;
            buffer [puntero_escritura] = ret;
            puntero_escritura = (puntero_escritura + 1) % LIMITE;
         } else {
            if ( caracteres_leidos > 0 ) {
               printc ( ASCII_BACKSPACE );
               puntero_escritura = ( puntero_escritura + LIMITE - 1 ) % LIMITE;
               caracteres_leidos--;
            } else {
               beep ( 1000, 1000 );
            }
         }
      }
   }
   ret = buffer [ puntero_lectura ];
   puntero_lectura = (puntero_lectura + 1) % LIMITE;
   caracteres_leidos--;

   return ret;
}
