#include <video.h>
#include <sodero/registros.h>
#include <sodero/puertos.h>
#include <sodero/definiciones.h>
#include <sodero/teclado.h>
#include <sodero/syscalls.h>

/*
 * Modulo que implementa la logica de video. 
 */

extern int recuperar_base ();

int comparar_cadenas (const char*, const char*);

/**
 * imprimir_cadena: Imprime una cadena por pantalla.
 * @param cad cadena a imprimir.
 * @return cantidad de caracteres impresos
 */
int imprimir_cadena (const char* cad) {
   while ( *cad != 0 ) {
      printc ( *cad );
      cad++;
   }
   return 0;
}

/**
 * longitud_cadena: Devuelve la longitud de una cadena (teniendo en cuenta 
 *                  que el caracter de finalizacion de una cadena sera el 
 *                  caracter 0, ascii 48).
 * @param cad cadena a la cual se le contara la longitud
 * @return la longitud de la cadena
 */
int longitud_cadena (const char* cad) {
   static int ret;
   ret = 0;
   while ( cad[ret++] != '\0' );

   return ret;
}

/**
 * imprimir_entero: Imprime un entero o un doble word (32 bits) por pantalla
 * @param nro entero a imprimir.
 */
void imprimir_entero (const int nro) {
   // 2^32 = 4294967296 --> a lo sumo el entero tendra 10 digitos + signo + \0
   static char str[12];

   itoa (nro, (char*) str);
   imprimir_cadena (str);

}

/**
 * imprimir_word: Imprime un word (16 bits) por pantalla
 * @param nro entero a imprimir.
 */
void imprimir_word (const word nro) {
   // 2^16 = 65535 --> a lo sumo el entero tendra 5 digitos + signo + \0
   static char str[7];

   wtoa (nro, (char*) str);
   imprimir_cadena (str);

}

/**
 * imprimir_byte: Imprime un byte (8 bits) por pantalla.
 * @param nro entero a imprimir.
 */
void imprimir_byte (const byte nro) {
   // 2^16 = 65535 --> a lo sumo el entero tendra 5 digitos + signo + \0
   static char str[7];

   btoa (nro, (char*) str);
   imprimir_cadena (str);
}

/**
 * imprimir_hexa: Imprime un entero o doble word (32 bits) en base 16 por 
 *                pantalla.
 * @param nro entero a imprimir.
 */
void imprimir_hexa (const int nro) {
   // 2^32 = 0xFFFFFFFF --> a lo sumo el entero tendra 8 digitos + signo + \0
   static char str[10];

   itoh (nro, (char*) str);
   imprimir_cadena (str);

}

/**
 * imprimir_word_hexa: Imprime un word (16 bits) en base 16 por pantalla.
 * @param nro entero a imprimir.
 */
void imprimir_word_hexa (const word nro) {
   // 2^16 = 0xFFFF --> a lo sumo el entero tendra 4 digitos + signo + \0
   static char str[6];

   wtoh (nro, (char*) str);
   imprimir_cadena (str);
}

/**
 * imprimir_byte_hexa: Imprime un byte (8 bits) en base 16 por pantalla.
 * @param nro entero a imprimir.
 */
void imprimir_byte_hexa (const byte nro) {
   // 2^16 = 0xFF --> a lo sumo el entero tendra 2 digitos + signo + \0
   static char str[4];

   btoh (nro, (char*) str);
   imprimir_cadena (str);
}

/**
 * imprimir: Imprime una cadena por pantalla, recibiendo argumentos variables,
 *           en forma analoga a la funcion de biblioteca de C printf().
 * @param str cadena a imprimir y parsear para conocer los argumentos variables
 * @param ... de cero a N argumentos con parametros para la funcion imprimir().
 */
void imprimir (const char* str, ... ) {
   va_list lista_argumentos;
   va_start (lista_argumentos, str);

   while (*str) {
      if ( *str == '%' ) {
         str++;
         switch (*str) {
            case 'c':
               printc (va_arg (lista_argumentos, char));
               break;
            case 'd':
               imprimir_entero (va_arg (lista_argumentos, int));
               break;
            case 'w':
               imprimir_entero (va_arg (lista_argumentos, word));
               break;
            case 'b':
               imprimir_byte (va_arg (lista_argumentos, byte));
               break;
            case 'x':
               str++;
               if ( *str == 'w' ) {
                  imprimir_word_hexa (va_arg (lista_argumentos, word));
               } else if ( *str == 'b' ) {
                  imprimir_byte_hexa (va_arg (lista_argumentos, byte));
               } else {
                  str--;
                  imprimir_hexa (va_arg (lista_argumentos, int));
               }
               break;
            case 's':
               imprimir_cadena (va_arg (lista_argumentos, char*));
               break;
         }
      } else {
         printc (*str );
      }
      str++;
   }
   va_end (lista_argumentos);
}

/**
 * itoa: transforma un entero en ascii
 * @param number numero a transformar
 * @param buf posicion de memoria donde se almacenara el numero
 *
 * <pre>
 * pseudocodigo:
 *    el numero se va dividiendo por diez y el resto mas 48 es lo
 *    que hay que imprimir.  Ej:
 *
 * int nro = 345
 *
 *    resto = nro%10; // resto = 5
 *    nro /= 10; // nro = 34
 *    etc....
 * 
 * debido a que el nro se imprimiria al revez, se almacena en un aux.
 * </pre>
 */
void itoa (int number, char* buf) {
   static int aux;
   aux = number;

   if ( number < 0 ) {
      number *= -1;
      *buf++ = '-';
   }

   do {
      *buf++ = 48;
   } while ( (aux/=10) != 0 );

   *buf-- = '\0';
   while ( number != 0 ) {
      aux = number%10;
      number = number/10;
      *buf-- = aux + 48;
   }
}

/**
 * ctoi: transforma una cadena en entero
 * @param buf posicion de memoria donde viene el numero
 * @return el numero transformado
 */
int ctoi ( char* buf ) {
   int i, x, largo;
   int ponderacion = 1;
   int nro = 0;

   largo = longitud_cadena (buf);
   for (i = largo - 2; i >= 0; i--) {
      if ( buf[i] < 48 || buf[i] > 57 ) {
         return NULL;
      }
      ponderacion = 1;
      for (x = 0; x < (largo - i - 2); x++) {
         ponderacion *= 10;
      }
      nro += ( ((int)(buf[i]) - 48) * ponderacion );
   }
   return nro;
}

/**
 * itoh: transforma un entero en ascii ( formato hexa )
 * @param number numero a transformar
 * @param buf posicion de memoria donde se almacenara el numero
 */
void itoh (int number, char* buf) {
   static int aux;
   aux = number;

   if ( number < 0 ) {
      number *= -1;
      *buf++ = '-';
   }

   do {
      *buf++ = 48;
   } while ( (aux/=16) != 0 );

   *buf-- = '\0';
   while ( number != 0 ) {
      aux = number%16;
      number = number/16;
      if ( aux < 10 )
         *buf-- = aux + 48;
      else
         *buf-- = aux + 55;
   }
}
/**
 * wtoh: transforma un word en ascii (formato hexa)
 * @param number numero a transformar
 * @param buf posicion de memoria donde se almacenara el numero
 */
void wtoh (word number, char* buf) {
   static word aux;
   aux = number;

   do {
      *buf++ = 48;
   } while ( (aux/=16) != 0 );

   *buf-- = '\0';
   while ( number != 0 ) {
      aux = number%16;
      number = number/16;
      if ( aux < 10 )
         *buf-- = aux + 48;
      else
         *buf-- = aux + 55;
   }
}

/**
 * btoh: transforma un byte en ascii ( formato hexa )
 * @param number numero a transformar
 * @param buf posicion de memoria donde se almacenara el numero
 */
void btoh (byte number, char* buf) {
   static byte aux;
   aux = number;

   do {
      *buf++ = 48;
   } while ( (aux/=16) != 0 );

   *buf-- = '\0';
   while ( number != 0 ) {
      aux = number%16;
      number = number/16;
      if ( aux < 10 )
         *buf-- = aux + 48;
      else
         *buf-- = aux + 55;
   }
}

/**
 * wtoa: transforma un word en ascii
 * @param number numero a transformar
 * @param buf posicion de memoria donde se almacenara el numero
 */
void wtoa (word number, char* buf) {
   static word aux;
   aux = number;

   do {
      *buf++ = 48;
   } while ( (aux/=10) != 0 );

   *buf-- = '\0';
   while ( number != 0 ) {
      aux = number%10;
      number = number/10;
      *buf-- = aux + 48;
   }
}

/**
 * btoa: transforma un byte en ascii
 * @param number numero a transformar
 * @param buf posicion de memoria donde se almacenara el numero
 */
void btoa (byte number, char* buf) {
   static byte aux;
   aux = number;

   do {
      *buf++ = 48;
   } while ( (aux/=10) != 0 );

   *buf-- = '\0';
   while ( number != 0 ) {
      aux = number%10;
      number = number/10;
      *buf-- = aux + 48;
   }
}

/**
 * ingresar_cadena: analoga a la funcion gets() de UNIX
 * @param dato buffer donde se almacenara la cadena leida
 * @return puntero a la cadena leida
 */
char* ingresar_cadena (char* dato) {
   int lectura;
   int idx;

   idx = 0;
   while ( (lectura =  getc ()) != '\n' ) {
      dato[idx++] = lectura;
   }
   dato[idx] = '\0';

   return dato;
}

/**
 * getc: obtiene un caracter leyendo del teclado ( a travez de la sys call )
 * @return el ASCII del caracter leido
 */
int getc () {
   return llamar_sys_call ( SYS_GETC, 0, 0, 0 );
}

/**
 * comparar_cadenas: compara dos cadenas
 * @param cad1 primer cadena a comparar
 * @param cad2 segunda cadena a comparar
 * @return -1 si son diferentes, 0 si son iguales
 */
int comparar_cadenas (const char* cad1, const char* cad2) {
   int i;

   if ( longitud_cadena (cad1) != longitud_cadena (cad2) ) {
      return -1;
   } 

   i = 0;
   while ( cad1[i] != '\0' ) {
      if ( cad1[i] != cad2[i++] ) {
         return -1;
      }
   }
   return 0;
}
