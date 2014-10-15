#include <sodero/teclado.h>
#include <sodero/idt.h>
#include <sodero/definiciones.h>
#include <video.h>
#include <sodero/puertos.h>
#include <timer.h>
#include <sodero/syscalls.h>

static void agregar_caracter ( char );

/*
 * Juego de caracteres de letrs minusculas
 */
char caracteres_minuscula [CANT_CHAR] = { 'U', 'U', '1', '2', '3', '4', '5',
                                          '6', '7', '8', '9', '0', '-', '=',
                                          'B', '\t', 'q', 'w', 'e', 'r', 't',
                                          'y', 'u', 'i', 'o', 'p', '[', ']',
                                          '\n', 'C', 'a', 's', 'd', 'f', 'g',
                                          'h', 'j', 'k', 'l', ';', '\'', '`',
                                          'S', '\\', 'z', 'x', 'c', 'v', 'b',
                                          'n', 'm', ',', '.', '/', 'S', 'U',
                                          'A', ' ', 'M' };
//16 - 25 de la 'q' a la 'p'
//30 - 38 de la 'a' a la 'l'
//44 - 50 de la 'z' a la 'm'
 
/*
 * Juego de caracteres de letras mayusculas (SHIFT)
 */
char caracteres_shift [CANT_CHAR] = { 'U', 'U', '!', '@', '#', '$', '%', '^',
                                      '&', '*', '(', ')', '_', '+', 'B', '\t',
                                      'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
                                      'O', 'P', '{', '}', '\n', 'C', 'A', 'S',
                                      'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
                                      '\"', '~', 'S', '|', 'Z', 'X', 'C', 'V',
                                      'B', 'N', 'M', '<', '>', '?', 'S', 'U',
                                      'A', ' ', 'M' };

/*
 * Juego de caracteres cuando se presiono SHIFT y CAPSLOCK a la vez
 */
char caract_shift_caps [CANT_CHAR] = { 'U', 'U', '!', '@', '#', '$', '%', '^',
                                       '&', '*', '(', ')', '_', '+', 'B', '\t',
                                       'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
                                       'o', 'p', '{', '}', '\n', 'C', 'a', 's',
                                       'd', 'f', 'g', 'h', 'j', 'k', 'l', ':',
                                       '\"', '~', 'S', '|', 'z', 'x', 'c', 'v',
                                       'b', 'n', 'm', '<', '>', '?', 'S', 'U',
                                       'A', ' ', 'M' };

/*
 * Juego de caracteres de letrs mayusculas (CAPSLOCK)
 */

char caracteres_capslock [CANT_CHAR] = { 'U', 'U', '1', '2', '3', '4', '5',
                                         '6', '7', '8', '9', '0', '-', '=',
                                         'B', '\t', 'Q', 'W', 'E', 'R', 'T',
                                         'Y', 'U', 'I', 'O', 'P', '[', ']',
                                         '\n', 'C', 'A', 'S', 'D', 'F', 'G',
                                         'H', 'J', 'K', 'L', ';', '\'', '`',
                                         'S', '\\', 'Z', 'X', 'C', 'V', 'B',
                                         'N', 'M', ',', '.', '/', 'S', 'U',
                                         'A', ' ', 'M' };


/*
41: '`'
57: ' ' --> barra espaciadora
*/


/**
 * Puntero al buffer de caracteres del teclado
 */
char* buffer_teclado;

/**
 * Indice por donde va escribiendo el teclado dentro del buffer
 */
int indice_lectura = 0;
int indice_escritura = 0;
int caracteres_leidos = 0;

   /**
    * inicializa_teclado: Funcion utilizada para inicializar el teclado.
    * Se encarga de alocar el buffer_teclado, inicializar los indices del 
    * buffer a 0. Deja habilitadas las teclas y los leds.
    * Tambien , el comando 0xF3 al port 0x60 setea la velocidad de retardo,
    * a continuacion se escribe al port 0x63 un byte con los valores:
    *
    * <PRE>
    *
    *   +---+---+---+---+---+---+---+---+
    *   | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    *   +---+---+---+---+---+---+---+---+
    *     |   |   |   |   |   |   |   |
    *     |   |   |   |   |   +---+---+--> Numero A
    *     |   |   |   |   |
    *     |   |   |   +---+--> Numero B
    *     |   |   |
    *     |   +---+--> Tiempo que transcurre desde que se presiona una tecla
    *     |            hasta que empieza a repetirse (en intervalos de 0.25seg).
    *     |   
    *     +--> Siempre va seteado a cero   
    *                                                    1
    *  Tasa de Autorepeticion (Teclas/sec): --------------------------- 
    *                                        ( 8 + A ) * 2^B * 0.00417
    *
    *  Si seteamos al teclado para que espere 0.25 segundos y acepte 10
    *  caracteres por segundo, este byte deberia valer:
    *  0x2C = 0 01 01 100 -> A:4 B:1 
    *  0x40 = 0 10 00 000
    *
    * </PRE>
    */
void inicializa_teclado() {

   //outb ( 0xFF, 0x60 ); // escribo el comando 0xFF RESET(incluye BAT)
   //while ( inb (0x60) != 0xAA ); // espero final OK del BAT. 
   //while ( inb (0x60) != 0xFA ); // espero la senial de reconocimiento

   outb ( 0xF3, 0x60 ); // escribo el comando 0xF3
   while ( inb (0x60) != 0xFA ); // espero la senial de reconocimiento
   outb ( 0x20, 0x60 ); // escribo los parametros del comando 0xF3
   while ( inb (0x60) != 0xFA ); // vuelvo a esperar reconocimiento

   //Se inicializan los leds. Todos Apagados.
   setear_leds ( (byte)0, (byte)0, (byte)0 );
   
   buffer_teclado = (char*) sys_alocar (LIMITE);
   buffer_teclado = setear_memoria (buffer_teclado, ASCII_FIN_LINEA, LIMITE);

   indice_lectura = 0;
   indice_escritura = 0;
   caracteres_leidos = 0;
}

/**
 * teclado_handler: Funcion que representa a la tarea que se ejecuta cuando
 * ocurre una interrupcion de teclado (IRQ1). Se encarga de validar la tecla
 * que se ingreso y manejar la escritura y desplazamiento en el buffer de
 * teclado.
 *
 * @param irq Nro de IRQ la cual llama a la funcion.
 */
void teclado_handler ( dword irq ) {
   dword lectura;
   char char_impresion;
   static byte shift = 0;

   // Variables que representan si los LEDs estan encendidos
   static byte mayusculas = 0;
   static byte teclado_numerico = 0;
   static byte desplazamiento = 0;

   //Realizo la lectura del teclado.
   lectura = inb (0x60);

   //imprimir ("\ncodigo lectura: %d\n",lectura);

   switch (lectura) {
     case 0xFE: //ERROR.
        // Mejorar este error!!!!!!!!!!!!!!
        imprimir ("Error en el teclado!!!!!!!\n");
        while(1);
        break;
     case FLECHA_ARRIBA:
        break;
     case FLECHA_IZQUIERDA:
        break;
     case FLECHA_DERECHA:
        break;
     case FLECHA_ABAJO:
        break;
     case CONTROL:
        break;
     case ALT:
        break;
     case TAB:
        printc ( ASCII_TABULADOR );
        break;
     case CAPS_LOCK:
        mayusculas = !mayusculas;
        setear_leds (desplazamiento, teclado_numerico, mayusculas);
        break;
     case NUM_LOCK:
        teclado_numerico = !teclado_numerico;
        setear_leds (desplazamiento, teclado_numerico, mayusculas);
        break;
     case SCROLL_LOCK:
        desplazamiento = !desplazamiento;
        setear_leds (desplazamiento, teclado_numerico, mayusculas);
        break;
      case BACKSPACE:
         agregar_caracter ( ASCII_BACKSPACE );
         break;
      case ENTER:
         printc ( ASCII_ENTER );
         agregar_caracter ( ASCII_ENTER );
         break;
      case SHIFT_DERECHO:
      case SHIFT_IZQUIERDO:
         shift = 1;
         break;
      case UNSHIFT_DERECHO:
      case UNSHIFT_IZQUIERDO:
         shift = 0;
         break;
      default: 
         //cualquier otra tecla.  
         if ( lectura < CANT_CHAR && caracteres_minuscula[lectura] != 'U' ) {
            //minusculas
            if ( (mayusculas && shift) ) {
               char_impresion = caract_shift_caps [lectura];
            } else if ( (!mayusculas && !shift) ) {
               char_impresion = caracteres_minuscula [lectura];
            } else if ( mayusculas && !shift ) { // capslock
               char_impresion = caracteres_capslock [lectura];
            } else if ( !mayusculas && shift ) { // shift
               char_impresion = caracteres_shift [lectura];
            }
            imprimir ( "%c", char_impresion );
            agregar_caracter ( char_impresion );
         }
         break;
      }
      outb (0x20, 0x20);
}

/**
 * agregar_caracter: Esta funcion agrega un caracter al buffer de teclado e
 * incrementa el indice de escritura siempre y cuando no pase el limite de 
 * caracteres a poder ser leidos por el buffer.
 *
 * @param char_impresion Caracter a agregar en el buffer del teclado.
 */
static void agregar_caracter ( char char_impresion ) {
   if ( caracteres_leidos < LIMITE ) {
      buffer_teclado [indice_escritura] = char_impresion;
      caracteres_leidos++;
      indice_escritura = (indice_escritura + 1) % LIMITE;
   } else {
      imprimir ( "No se pueden leer mas caracteres!!\n" );
   }
}

/**
 * obtener_caracter_teclado: Esta funcion obtiene un caracter del buffer de 
 * teclado. Con esto decrementa los caracteres leidos en uno e incrementa el
 * indice de lectura en uno.
 *
 * @return caracter_teclado caracter apuntado por el indice de lectura.
 */
char obtener_caracter_teclado () { 
   char ret = 0;
   if ( caracteres_leidos > 0 ) {
      ret = buffer_teclado [ indice_lectura ];
      caracteres_leidos--;
      indice_lectura = (indice_lectura + 1) % LIMITE;
   }
   return ret;
}

/**
 * setear_leds: esta funcion se encargar de encender o apagar los leds del
 * teclado segun corresponda.
 * bit 0=SCROLL_LOCK - bit 1=NUM_LOCK - bit 2 CAPS_LOCK - bits 3-7 reservados
 *
 * @param desplazamiento Viene en 1 cuando se quiere enceder el SCROLL_LOCK 
 *                       en 0 si NO.
 *
 * @param teclado_numerico Viene en 1 cuando se quiere enceder el NUM_LOCK 
 *                         en 0 si NO.
 *
 * @param mayusculas Viene en 1 cuando se quiere enceder el CAPS_LOCK 
 *                   en 0 si NO.
 */
void setear_leds (byte desplazamiento, byte teclado_numerico, byte mayusculas) {
    static byte leds;

    leds = 0;

    leds = leds | desplazamiento;          //seteo el bit 0 = SCROLL_LOCK
    leds = leds | (teclado_numerico << 1); //seteo el bit 1 = NUM_LOCK
    leds = leds | (mayusculas << 2);       //seteo el bit 2 = CAPS_LOCK

    outb(0xED, 0x60); //Comando de seteo de LEDS. 
    while ( inb (0x60) != 0xFA ); //Espero senial de reconocimiento

    outb(leds, 0x60); // bit 2 CAPS_LOCK - 3-7 reservados 
    while ( inb (0x60) != 0xFA ); //Espero senial de reconocimiento
}
